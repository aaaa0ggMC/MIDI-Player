#define private public
#include <iostream>
#include <string>
#include <fstream>
#include "MIDI.h"

using namespace std;
using namespace midi;

int main(int argc,char * args[])
{
    MIDIPlayer player;
    if(argc >= 2 && (!strcmp(args[1],"-help") || !strcmp(args[1],"-h") || !strcmp(args[1],"-?"))){
        cout << "���� -help����-h����-? ����ȡ����" << endl;
        cout << "----------------------MIDI Player------------------" << endl;
        cout << "�ļ���ʽ:" << endl;
        cout << "[������������] [�ٶ�] [������] [�Ƿ�ѭ��] [����] [ÿС����Ϣʱ��] [��������]..." << endl;
        cout << "[������������]: ������������ɣ�[����:char][�����˶�:int]" << endl;
        cout << "                    [����]: �������� C,D,E,F,G,A,B,c,d,e,f,g,a,b" << endl;
        cout << "                ����Сд��ĸ�ȴ�д��ĸ���˰����F,B���⣬�����ǰ�����" << endl;
        cout << "                    [�����˶�]:�����˶ȿ�ѡ��֧����������,��ʾ�����������˶�������һ�㲻��" << endl;
        cout << "[�ٶ�]        : ���������ж�죬��Ϊ1ΪĬ���ٶȣ�0.X���٣�>1���٣���������" << endl;
        cout << "[������]      : 0-100,����������С" << endl;
        cout << "[�Ƿ�ѭ��]    : һ���Ǹ����������Ϊ����ѭ��������ѭ��" << endl;
        cout << "[����]        : ֧���������� \"Drum\" �� \"Piano\",��ʵ���������Drum����ȫΪPaino" << endl;
        cout << "[С����Ϣʱ��]: ÿС�ں�sleepʱ��(ms)������Ϊ0������Ϊ 1" << endl;
        cout << "[��������]    : �����������ĸ��������: [�������][�����˶�][ʱֵ][������]" << endl;
        cout << "                    [�������]:֧�������������������0-7��,0������" << endl;
        cout << "                    [�����˶�]:ǰһ�����������ٰ˶�" << endl;
        cout << "                    [ʱֵ]:sleepʱ��(ms),0��ʾȫ��(1000ms),1��ʾ����(500ms),2��ʾ�ķ�����(250ms),3��ʾ�˷�����(125ms)" << endl;
        cout << "                ����ʱ��ֱ����Ϊsleepʱ�䣬ʱֵΪ0����ʱ�����������������ǽ���ʱ���ȡ" << endl;
        cout << "                    [������]:�ɵ���,֧'.','@','#','V'��'E'" << endl;
        cout << "                             . : ���㣬�ӳ�ʱֵһ��" << endl;
        cout << "                             # : ע�ͣ����Ե�ǰ���к���ȫ�����ݣ����ȼ��ڶ���,���ǻᵼ�µ�ǰ�����޷������б���" << endl;
        cout << "                             @ : ��ʾһС�ڽ�����������һ�к��������" << endl;
        cout << "                             V : �����һ���Ǹ����������ֲ�������С�л�" << endl;
        cout << "                             E : ������ȡ������Ҫ�У�" << endl;
        system("pause");
        return 0;
    }
    try{
        const char * opener = (argc>2)?args[1]:"midi.txt";
        if(argc < 2){
            cout << "Due to the lack of argument 2,we use \"midi.txt\" as the file." << endl;
        }
        ifstream ifs(opener);
        if(!ifs.good()){
            throw string("Cannot open file \"") + opener + "\"!The file is not available.Use -? to get some help!";
        }
        player.LoadFromStream(ifs);
        ifs.close();
        player.PlayStuck();
    }catch(string s){
        cout << "*Exception:" << s << endl;
    }
    system("pause");
    return 0;
}
