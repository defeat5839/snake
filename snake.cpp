//贪吃蛇游戏
//author  wsw9603
//date    2018-08-12
//version 1.0

//设计思路
//用一个二维数组表示整个地图
//定期刷新显示地图


//功能描述
//wsad或2846控制蛇的上下左右移动，在没有按键按下时蛇会自动向前进方向按照一定速度前进，
//需要定义几个类，窗口类用来表示整个地图，包含地图的显示等方法，蛇类用来表示窗口中的蛇
//以及其运动状态。
//特别的，当按下与前进方向相同的按键时，蛇立即向前前进一格，即加速前进。

//几个问题
//如何记录/表示一条蛇？蛇的移动体现在甚麽地方？
//暂定  用一个二元组向量表示一条蛇，即向量的每一个元素都是一个二元组，分别表示蛇身的每一
//个部分的位置。蛇向前进一格相当于：最后一格消失，蛇头的前进方向产生一个蛇头。用一个向量
//表示蛇的身体，另外用一个下表表示蛇头的位置，或者用链表这种数据结构来表示蛇身。


//蛇每隔一段时间（period）向前移动一格，程序每秒钟显示/计算ups次，即程序每秒钟检测按键ups
//次。每一个period里可以多次按下按键，但只有最后一次按下的按键有效。

//游戏过程
//1蛇的起始位置，长度
//2随机产生食物，在合理的范围内
//3控制蛇去吃食物
//4重复23

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "conio.h"

using namespace std;

const int width = 14;
const int hight = 10;

//控制按键
const char auto_run = 'e';
const char quit = 'q';
const char manual = 'r';
const char def = '\0';

//不同部分（墙、蛇、食物、蛇头、背景）的数字编号
const int FOOD_NUM = 2;
const int BODY_NUM = 3;
const int BK_NUM = 0;
const int WALL_NUM = 1;
const int HEAD_NUM = 4;

const char* WALL ="\033[43;34m  \033[0m";
const char* BACKGROUND = "\033[42;34m  \033[0m";
const char* BODY= "\033[44;34m  \033[0m";
const char* HEAD = "\033[45;34m  \033[0m";
const char* FOOD = "\033[46;34m  \033[0m";

//一些事件
const int EATEN = 1;  //蛇吃掉了食物，要求窗口重新产生食物
const int DEATH = 2;  //蛇撞墙或者撞自己，游戏结束
const int EVERYTHING_IS_OK = 0;  //蛇正常走完一步，一切正常

float period = 0.5;//表示每隔一段时间向前移动一下
const int ups = 10; //每秒钟检测按键的次数

struct pos{
    int x;
    int y;
    pos(int a=1,int b=1):x{a},y{b}{}
};

bool operator==(pos a,pos b){ return a.x==b.x&&a.y==b.y;}
bool operator!=(pos a,pos b){ return !(a==b);}

enum Direction {LEFT,RIGHT,UP,DOWN};

//********************Snake start*************************************
class Snake{
    public:
    Snake();
    int deal_key(char ch);
    const vector<pos>& get_body() const {return body;}
    int get_head() const {return head;}
    void set_food(pos temp){food.x=temp.x;food.y=temp.y;}
    char auto_run();

    private:
    void eat(pos food);
    int turn(Direction dir);    //按键按下后蛇的转向操作，如果可能的话会吃掉食物或者撞墙

    pos food;   //记录食物的位置
    vector<pos> body;
    int head;   //指出蛇头在向量中的位置
    Direction direction;
};

Snake::Snake()
{
    body.push_back(pos{hight/2,width/2});
    head = 0;
    direction = LEFT;
}
int Snake::deal_key(char ch)
{
    switch (ch)
    {
        case 'w':
        case '2':
            if (direction != DOWN)
                return turn(UP);
        case 's':
        case '8':
            if (direction != UP)
                return turn(DOWN);
        case 'a':
        case '4':
            if(direction != RIGHT)
                return turn(LEFT);
        case 'd':
        case '6':
            if (direction != LEFT)
                return turn(RIGHT);
        default:
            return turn(direction);
    }
}
void Snake::eat(pos food)
{
    body.push_back(body.back());
    for (int i=body.size()-1;i>head;--i)
        body[i] = body[i-1];
    body[head]=food;
}
int Snake::turn(Direction dir)
{
    pos temp;
    int next = (head+body.size()-1)%body.size();

    direction = dir;//此行必须在前面
    switch(dir)
    {
        case LEFT:
            temp = pos{body[head].x,body[head].y-1};
            break;
        case RIGHT:
            temp = pos{body[head].x,body[head].y+1};
            break;
        case UP:
            temp = pos{body[head].x-1,body[head].y};
            break;
        case DOWN:
            temp = pos{body[head].x+1,body[head].y};
            break;
    }
    if (temp.x==0 || temp.x== hight+1 || temp.y==0 || temp.y==width+1)//检查是否会撞墙
        return DEATH;
    for (int i = head;i!=next;i=(i+1)%body.size())//检查是否会撞自己
        if (body[i]==temp)
            return DEATH;
    if (temp == food)
        {eat(temp);return EATEN;}

    head = next;
    body[head] = temp;
    return EVERYTHING_IS_OK;
}
char Snake::auto_run()
{
    return 'w';
}
//********************Snake end****************************************



//****************Window start ****************************************
class Window{
    public:
    Window();
    void run();

    private:
    Snake snake;
    int map[hight+2][width+2];

    void draw(); //显示整个页面
    void attach();
    void deteach();
    void produce_food();
};
Window::Window()
{
    for (int i=1;i<width+1;++i)
        map[0][i]=map[11][i]=WALL_NUM;
    for (int i=1;i<hight+1;++i)
        for (int j=1;j<width+1;++j)
            map[i][j]=BK_NUM;
    for (int i=0;i<hight+2;++i)
        map[i][0]=map[i][15]=WALL_NUM;
}
void Window::draw()
{
    for (int i=0;i<hight+2;++i)
    {
        for (int j=0;j<width+2;++j)
            switch(map[i][j]){
                case BK_NUM:
                    cout<<BACKGROUND;
                    break;
                case WALL_NUM:
                    cout<<WALL;
                    break;
                case BODY_NUM:
                    cout<<BODY;
                    break;
                case HEAD_NUM:
                    cout<<HEAD;
                    break;
                case FOOD_NUM:
                    cout<<FOOD;
                    break;
            }
        cout<<endl;
    }
}

void Window::attach()
{
    auto body = snake.get_body();
    int head = snake.get_head();
    for (auto itera = body.begin();itera != body.end();++itera)
        map[itera->x][itera->y]=BODY_NUM;
    map[body[head].x][body[head].y]=HEAD_NUM;
}

void Window::deteach()
{
    auto body = snake.get_body();
    for (auto itera = body.begin();itera != body.end();++itera)
        map[itera->x][itera->y]=BK_NUM;
}
void Window::produce_food()
{
    srand(time(0));
    pos food;
    while(1)
    {
        food = pos{rand()%hight+1,rand()%width+1};
        auto body = snake.get_body();
        auto itera = body.begin();
        while(itera!=body.end()&&*itera!=food)//检查食物是否在蛇的身体上
            ++itera;
        if(itera==body.end())
            break;
    }
    map[food.x][food.y]=FOOD_NUM;
    snake.set_food(food);
}
void Window::run()
{
    produce_food();
    bool flag = true;
    char ch=def;
    while(flag)
    {
        system("clear");
        attach();
        draw();
        deteach();
        //检测按键按下
        for (int i=0;i<ups*period;++i)
        {
            if (kbhit())
                {ch = getch();break;}
            usleep(1000000/ups);
        }
        char ctrl = ch;
        //处理按键按下
        switch(ch)
        {
            case quit:  //结束程序
                flag = false;
                break;
            case manual:  //退出自动运行
                ch = def;
                break;
            case auto_run:  //设计目标是自动运行，自己吃食物
                ctrl = snake.auto_run();
            default :
            {
                switch(snake.deal_key(ctrl))  //剩下的交给蛇去处理
                {
                    case EATEN:
                        produce_food();
                        break;
                    case EVERYTHING_IS_OK:
                        break;
                    case DEATH:
                        flag = false;
                        break;
                }
            }
        }
    }
}

//*************Window end**********************************************

int main()
{
    Window win;
    win.run();

    return 0;
}
