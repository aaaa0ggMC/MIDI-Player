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
        cout << "输入 -help或者-h或者-? 来获取帮助" << endl;
        cout << "----------------------MIDI Player------------------" << endl;
        cout << "文件格式:" << endl;
        cout << "[主音调描述符] [速度] [主音量] [是否循环] [乐器] [每小节休息时间] [音符描述]..." << endl;
        cout << "[主音量描述符]: 由两个部分组成：[音阶:char][升降八度:int]" << endl;
        cout << "                    [音阶]: 音阶允许 C,D,E,F,G,A,B,c,d,e,f,g,a,b" << endl;
        cout << "                其中小写字母比大写字母升了半调（F,B除外，它们是半音）" << endl;
        cout << "                    [升降八度]:升降八度可选，支持所有整数,表示主音量升降八度数量，一般不填" << endl;
        cout << "[速度]        : 描述歌曲有多快，设为1为默认速度，0.X减速，>1加速，负数倒放" << endl;
        cout << "[主音量]      : 0-100,描述音量大小" << endl;
        cout << "[是否循环]    : 一个非负整数，如果为零则不循环，其余循环" << endl;
        cout << "[乐器]        : 支持两个参数 \"Drum\" 与 \"Piano\",事实上如果不是Drum其余全为Paino" << endl;
        cout << "[小节休息时间]: 每小节后，sleep时间(ms)，不能为0，可以为 1" << endl;
        cout << "[音符描述]    : 音符描述由四个部分组成: [相对音调][升降八度][时值][描述符]" << endl;
        cout << "                    [相对音调]:支持任意整数（建议最好0-7）,0不发音" << endl;
        cout << "                    [升降八度]:前一个音升降多少八度" << endl;
        cout << "                    [时值]:sleep时间(ms),0表示全音(1000ms),1表示半音(500ms),2表示四分音符(250ms),3表示八分音符(125ms)" << endl;
        cout << "                其余时间直接作为sleep时间，时值为0播放时会忽略这个音符，但是解析时会读取" << endl;
        cout << "                    [描述符]:可叠加,支'.','@','#','V'与'E'" << endl;
        cout << "                             . : 附点，延长时值一半" << endl;
        cout << "                             # : 注释，忽略当前这行后面全部内容，优先级第二大,但是会导致当前音符无法加入列表中" << endl;
        cout << "                             @ : 表示一小节结束，忽略这一行后面的内容" << endl;
        cout << "                             V : 后面接一个非负数，表述局部音量大小切换" << endl;
        cout << "                             E : 结束读取，必须要有！" << endl;
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
