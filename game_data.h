/**
 * 文件名: game_data.h
 * 描述: 核心逻辑模块 - 包含英雄(Hero)、玩家(Player)定义以及数据管理(DataManager)。
 * 注意: 这是一个纯逻辑头文件，不包含任何 Qt 界面代码，符合 MVC 设计模式中的 Model 层。
 */
#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

// 招数枚举：使用枚举比使用 0,1,2 魔法数字更易读、更易维护
enum MoveType { SCISSORS = 0, ROCK = 1, PAPER = 2, NONE = -1 };

// 辅助函数：将枚举转换为中文，用于日志显示
static string moveToString(MoveType m) {
    switch (m) {
        case SCISSORS: return "剪刀";
        case ROCK:     return "石头";
        case PAPER:    return "布";
        default:       return "未知";
    }
}

// ==========================================
// 类: Hero (英雄)
// 描述: 存储单个英雄的属性（名字、招数库存）和对局统计
// ==========================================
class Hero {
public:
    string name;
    int s, r, p; // 初始库存：剪刀(Scissors)、石头(Rock)、布(Paper)
    int currentS, currentR, currentP; // 运行时库存：当前对局中剩余的数量
    
    // 统计数据 (用于排行榜)
    int winMatches = 0;   // 该英雄获胜的回合数
    int totalMatches = 0; // 该英雄出战的总回合数

    // 构造函数
    Hero(string n, int _s, int _r, int _p) 
        : name(n), s(_s), r(_r), p(_p), currentS(_s), currentR(_r), currentP(_p) {}

    // 重置状态：每局游戏开始前调用，将当前库存恢复为初始值
    void reset() {
        currentS = s; currentR = r; currentP = p;
    }

    // 判断英雄是否还有招可用（任意一种招数 > 0）
    bool hasMoves() const { return (currentS + currentR + currentP) > 0; }
    
    // 检查特定招数是否可用 (用于 UI 按钮变灰逻辑)
    bool canUse(MoveType m) const {
        if (m == SCISSORS) return currentS > 0;
        if (m == ROCK) return currentR > 0;
        if (m == PAPER) return currentP > 0;
        return false;
    }

    // 核心算法：随机出招（并自动扣除库存）
    // 算法亮点：使用“加权随机”逻辑，而非简单的 rand()%3
    MoveType makeRandomMove() {
        vector<MoveType> pool;
        // 将剩余招数展开到 pool 中。例如：剩2剪刀1石头 -> {剪, 剪, 石}
        // 这样随机取一个元素，自然符合概率分布
        for(int i=0; i<currentS; ++i) pool.push_back(SCISSORS);
        for(int i=0; i<currentR; ++i) pool.push_back(ROCK);
        for(int i=0; i<currentP; ++i) pool.push_back(PAPER);
        
        if(pool.empty()) return NONE;

        // 使用 C++11 标准的随机数生成器 (比 C 语言的 rand() 更均匀)
        static mt19937 rng(time(0));
        uniform_int_distribution<int> dist(0, pool.size()-1);
        MoveType m = pool[dist(rng)];
        
        useMove(m); // 扣除库存
        return m;
    }

    // 指定出招（扣除库存）
    void useMove(MoveType m) {
        if(m == SCISSORS) currentS--;
        else if(m == ROCK) currentR--;
        else if(m == PAPER) currentP--;
    }

    // 计算胜率 (百分比)，注意除数不能为 0
    double getWinRate() const {
        if(totalMatches == 0) return 0.0;
        return (double)winMatches / totalMatches * 100.0;
    }
};

// ==========================================
// 类: Player (玩家)
// 描述: 简单的用户账户结构
// ==========================================
class Player {
public:
    string username;
    string password;
    int totalWins; // 累计胜场
    Player(string u, string p, int w) : username(u), password(p), totalWins(w) {}
};

// ==========================================
// 类: DataManager (数据管理器)
// 描述: 负责全局数据的加载、保存和查找
// ==========================================
class DataManager {
public:
    vector<Hero> heroes;    // 所有可选英雄
    vector<Player> players; // 所有注册玩家
    Player* currentUser = nullptr; // 当前登录玩家的指针

    DataManager() {
        initHeroes();
        loadPlayers();
    }
    
    // 析构函数：程序退出时自动保存数据
    ~DataManager() {
        savePlayers();
    }

    // 初始化题目指定的 15 位英雄数据
    void initHeroes() {
        heroes = {
            {"赵云", 2, 2, 2}, {"宫本武藏", 4, 1, 1}, {"凯", 2, 3, 1},
            {"白起", 5, 0, 1}, {"韩信", 1, 2, 3}, {"诸葛亮", 2, 1, 3},
            {"刘邦", 2, 0, 4}, {"后羿", 0, 3, 3}, {"王昭君", 1, 1, 4},
            {"妲己", 4, 1, 1}, {"安琪拉", 1, 4, 1}, {"貂蝉", 3, 2, 1},
            {"露娜", 1, 3, 2}, {"不知火舞", 4, 2, 0}, {"蔡文姬", 0, 5, 1}
        };
    }

    // 从文件读取玩家数据
    void loadPlayers() {
        ifstream file("users.txt");
        if(!file.is_open()) return; // 文件不存在可能是第一次运行，直接忽略
        string u, p; int w;
        while(file >> u >> p >> w) players.emplace_back(u, p, w);
    }

    // 保存玩家数据到文件
    void savePlayers() {
        ofstream file("users.txt");
        for(const auto& p : players) file << p.username << " " << p.password << " " << p.totalWins << endl;
    }

    // 注册逻辑：查重 -> 添加 -> 保存
    bool registerUser(string u, string p) {
        for(auto& user : players) if(user.username == u) return false; // 用户名已存在
        players.emplace_back(u, p, 0);
        savePlayers();
        return true;
    }

    // 登录逻辑：遍历查找匹配
    bool login(string u, string p) {
        for(auto& user : players) {
            if(user.username == u && user.password == p) {
                currentUser = &user; // 记录当前登录状态
                return true;
            }
        }
        return false;
    }
};

#endif