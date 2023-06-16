#include "MIDI.h"
#include <stdlib.h>
#include <fstream>

using namespace midi;
using namespace std;

const vector<unsigned int> Pitches::arr = {Pitches::C,Pitches::D,Pitches::E,Pitches::F,Pitches::G,Pitches::A,Pitches::B};

///Pitches
unsigned int Pitches::ChangeFull8(unsigned int base,int level){
    return base + level * 12;
}


unsigned int Pitches::WaveUp(unsigned int base,int notec){
    static const unsigned int arrx[] = {
        1,2,2,1,2,2,2
    };
    if(!notec)return base;
    ///Judge the note
    int index = -1;
    for(unsigned int cm = 0;cm < arr.size();++cm){
        if(arr[cm]%12 == base%12){
            index = cm;
        }
    }
    if(index < 0)return base;
    ///Wave up
    int vidx = index;
    int dr = (notec > 0?1:-1);
    for(unsigned int tm = 0;tm <  abs(notec);++tm){
        //加1是不让它加自己的数字
        vidx += dr;
        if(vidx < 0)vidx = 6;
        else if(vidx >= 7)vidx = 0;
        base += dr * arrx[vidx];
    }
    return base;
}

///Note
Note::Note(unsigned int p,int pl,float vp,unsigned int t){
    time = t;
    pitch = Pitches::ChangeFull8(p,pl);
    volume_percentage = vp;
}

///MIDIPlayer
MIDIPlayer::MIDIPlayer(unsigned int base,bool l,unsigned int v,unsigned int i,unsigned int deviceId){
    available = true;
    this->base = base;
    midiOutOpen(&handle,deviceId,0,0,0);
    playing = false;
    run = thread(PlayFn,this);
    instru = i;
    SetLoop(l);
    SetVolume(v);
    direction = 1;
    signalX = false;
    progress = 0;
    speed = 1;
    part_rest = 100;
}

MIDIPlayer::~MIDIPlayer(){
    available = false;
    midiOutClose(handle);
    run.detach();
}

HMIDIOUT MIDIPlayer::GetHandle(){
    return handle;
}


void MIDIPlayer::SetProgress(int p){
    unsigned int px = abs(p) % notes.size();
    if(p >= 0)progress = px;
    else progress = notes.size() - px;
}

void MIDIPlayer::MoveProgress(int v){
    SetProgress((progress.load() + v));
}

void MIDIPlayer::PlayFn(){
    while(available){
        if(!playing.load()){
            Sleep(40);
            continue;
        }
        int c = progress.load();
        if((c >= (int)notes.size() && direction == 1) || (c < 0 && direction == -1)){
            if(loop.load()){
                progress = 0;
                continue;
            }
        }
        const Note &note = notes[c];
        if(note.time == 0)continue;

        unsigned int vol = (unsigned int)((volume/100.0 * ((note.volume_percentage < 0?0:((note.volume_percentage) > 1?1:note.volume_percentage)))) * 0x7f);
        midiOutShortMsg(handle,(vol << 16) + (note.pitch << 8) + instru);
        Sleep((unsigned int)(note.time * (1/speed)));
        c += direction.load();
        if((c >= (int)notes.size() && direction == 1) || (c < 0 && direction == -1)){
            if(loop){
                c = direction==1?0:notes.size()-1;
            }
            else{
                playing = false;
                signalX = true;
            }
        }
        progress = c;
    }
}

void MIDIPlayer::Play(bool r){
    if(r)Restart();
    playing = true;
}

void MIDIPlayer::PlayStuck(bool r){
    Play(r);
    signalX = false;
    while(!(signalX.load())){
        Sleep(40);
    }
    Stop();
}

bool MIDIPlayer::IsPlaying(){return playing.load();}

unsigned int MIDIPlayer::GetNoteProgress(){return progress.load();}
float MIDIPlayer::GetNoteProgressPerc(){return ((float)(progress.load()))/notes.size();}

void MIDIPlayer::Restart(){
    progress = 0;
}

void MIDIPlayer::SetDirection(int dr){
    if(!dr)return;
    if(dr > 0)direction = 1;
    else direction = -1;
    if(direction == -1 && progress == 0)progress = notes.size() - 1;
}

void MIDIPlayer::SetInstrument(unsigned int inst){
    instru = inst;
}

void MIDIPlayer::SetLoop(bool l){loop = l;}

void MIDIPlayer::SetVolume(unsigned int v){
    if(v >= 100)v = 100;
    volume = v;
}

void MIDIPlayer::Stop(){
    Restart();
    Pause();
}

void MIDIPlayer::Pause(){
    playing = false;
}

void MIDIPlayer::SetSpeed(float s){
    speed = (s<=0)?0.1:s;
}

void MIDIPlayer::SetPartRestTime(unsigned int x){
    part_rest = x;
}

/**format
[Base Pitch] [Speed] [General Volume] [Loop?] [Instrument] [part_resttime] {[Notes]}

[Base Pitch]  : [DeclareBase][Level]
[Level]       : int
[Declare Base]: C,D,E,F,G,A,B
[Speed]       : float
[loop?]       : 0 or 1
[Instrument]  : "piano" or "drum"
[Notes]       : [offset][time_dec] [local volume]
[offset]      : [int][int_full]
[time_dec]    : 0:full 1:half 2:qua 3:half-qua [others]:time = [others]
*/
void MIDIPlayer::LoadFromStream(ifstream &ist){
    string token;
    float fv = 0;
    int iv = 0;
    ist >> token;
    bool rem = false;
    unsigned int base;
    switch(token[0]){
    case 'C':
        base = Pitches::C;
        break;
    case 'D':
        base = Pitches::D;
        break;
    case 'E':
        base = Pitches::E;
        break;
    case 'F':
        base = Pitches::F;
        break;
    case 'G':
        base = Pitches::G;
        break;
    case 'A':
        base = Pitches::A;
        break;
    case 'B':
        base = Pitches::B;
        break;
    case 'c':
        base = Pitches::C+1;
        break;
    case 'd':
        base = Pitches::D+1;
        break;
    case 'e':
        base = Pitches::E+1;
        break;
    case 'f':
        base = Pitches::F;
        break;
    case 'g':
        base = Pitches::G+1;
        break;
    case 'a':
        base = Pitches::A+1;
        break;
    case 'b':
        base = Pitches::B;
        break;
    default:
        throw string("无法正确读取文件的主音调!(Tips:命令行里 -help 查看帮助)");
    }
    if(token.size() >= 3){
        int level = atoi(&token[1]);
        base = Pitches::ChangeFull8(base,level);
    }
    this->base = base;

    ist >> fv;
    this->speed = abs(fv);
    if(fv > 0)SetDirection(1);
    else SetDirection(-1);

    ist >> base;
    SetVolume(base);

    ist >> base;
    if(base != 0)loop = true;
    else loop = false;

    ist >> token;
    if(!token.compare("Drum")){
        instru = Drum;
    }else instru = Piano;

    ist >> base;
    part_rest = base;

    bool newp,volt;

    float vpg = 1;

    while(!ist.eof()){
        Note n(0);
        newp = false;
        volt = false;
        ist >> base;
        if(base == 0)n.pitch = 0;
        else{
            base -= 1;
            n.pitch = Pitches::WaveUp(this->base,base);
        }
        ist >> iv;
        n.pitch = Pitches::ChangeFull8(n.pitch,iv);
        if(n.pitch >= 0x7f)continue;
        ist >> base;
        if(base == 0)n.time = NMIDI_TIME_FULL;
        else if(base == 1)n.time = NMIDI_TIME_HALF;
        else if(base == 2)n.time = NMIDI_TIME_QUATER;
        else if(base == 3)n.time = NMIDI_TIME_HQUA;
        else n.time = base;
        ist >> token;
        for(char & c : token){
            switch(c){
            case '.':
                n.time += n.time / 2;
                break;
            case '#':
                rem = true;
                break;
            case '@':
                //a part of rest
                newp = true;
                break;
            case 'V':
                volt = true;
                //set the volume
                break;
            case 'E':
                cout << "End Loading" << endl;
                return;
            default:
                break;
            }
        }
        if(rem){
            rem = false;
            getline(ist,token);
            continue;
        }
        if(volt){
            ist >> base;
            if(base > 100)base = 100;
            vpg = base / 100.0;
            continue;
        }
        if(newp && part_rest == 0)continue;
        if(newp){
            getline(ist,token);
            n.pitch = 0;
            n.time = part_rest;
            n.volume_percentage = 1;
        }
        n.volume_percentage = vpg;
        notes.push_back(n);
        cout << "Note:" << n.pitch << " " << n.time << " " << n.volume_percentage << endl;
        //ist >> std::noskipws;
    }
    cout << "Unexpected Rooad!" << endl;
    notes.clear();
}
