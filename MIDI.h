#ifndef MIDI_H_INCLUDED
#define MIDI_H_INCLUDED
#include <vector>
#include <windows.h>
#include <thread>
#include <atomic>
#include <math.h>
#include <iostream>
#include <fstream>

#define NMIDI_TIME_FULL 1000
#define NMIDI_TIME_HALF 500
#define NMIDI_TIME_QUATER 250
#define NMIDI_TIME_HQUA 125

namespace midi{

    using namespace std;

    struct Pitches{
        static const unsigned int C = 60;
        static const unsigned int D = 62;
        static const unsigned int E = 64;
        static const unsigned int F = 65;
        static const unsigned int G = 67;
        static const unsigned int A = 69;
        static const unsigned int B = 71;

        static const vector<unsigned int> arr;

        static unsigned int ChangeFull8(unsigned int base,int level);
        static unsigned int WaveUp(unsigned int base,int notec = 1);
    };

    struct Note{
        Note(unsigned int pitch,int pitch_level = 0,float vper = 1,unsigned int time = NMIDI_TIME_QUATER);

        unsigned int pitch;
        unsigned int time;
        float volume_percentage;
    };

    class MIDIPlayer{
    public:
        vector<Note> notes;

        static const unsigned int Piano = 0x90;
        static const unsigned int Drum = 0x99;

        MIDIPlayer(unsigned int base = Pitches::C,bool loop = false,unsigned int vol = 100,unsigned int instru = Piano,unsigned int deviceId=0);
        ~MIDIPlayer();
        HMIDIOUT GetHandle();

        void LoadFromStream(ifstream &ist);

        void Play(bool restart = false);
        void Pause();

        void PlayStuck(bool restart = false);
        void Restart();
        void Stop();
        bool IsPlaying();

        void SetInstrument(unsigned int instru);
        void SetVolume(unsigned int vol);
        void SetLoop(bool l);
        void SetSpeed(float speed);

        void SetProgress(int p);
        void MoveProgress(int v);

        unsigned int GetNoteProgress();
        float GetNoteProgressPerc();

        void SetDirection(int dr);

        void SetPartRestTime(unsigned int t);

    private:

        void PlayFn();

        HMIDIOUT handle;
        thread run;
        atomic<unsigned int> base;
        atomic<float> speed;
        atomic<bool> playing;
        atomic<bool> loop;

        atomic<unsigned int> progress;
        atomic<int> direction;

        atomic<bool> signalX;

        unsigned int volume;
        unsigned int instru;

        unsigned int part_rest;

        bool available;
    };
}

#endif // MIDI_H_INCLUDED
