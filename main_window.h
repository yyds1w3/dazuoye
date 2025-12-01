/**
 * 文件名: main_window.h
 * 描述: 主窗口类的声明，定义了界面组件、槽函数和成员变量。
 */
#ifndef MAIN_WINDOW_H // 防止头文件重复包含
#define MAIN_WINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QStackedWidget> // 用于实现多页面切换
#include <QListWidget>
#include <QMessageBox>
#include <QTextEdit>
#include <QGroupBox>
#include <QTimer>
#include "game_data.h" // 引入逻辑层

class MainWindow : public QWidget {
    Q_OBJECT // [核心] 必须加上这个宏，才能使用 Qt 的信号与槽机制 (Signal & Slot)

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // --- 数据模型 ---
    DataManager dataMgr;       // 数据管理器实例
    vector<int> myHeroIndices; // 玩家选中的3个英雄在 allHeroes 中的索引
    vector<int> cpuHeroIndices; // 电脑选中的3个英雄的索引
    
    // --- 游戏运行时状态 ---
    int currentRound;          // 当前回合数 (1-9)
    int myScore, cpuScore;     // 双方得分
    int currentCpuHeroIndex;   // 电脑当前派出的是第几个英雄
    MoveType cpuNextMove;      // 电脑本回合预先决定出的招 (作弊防止：先算好，后展示)

    // --- UI 组件 (指针) ---
    // 使用指针是为了在堆上管理内存，并在不同函数间访问这些控件
    QStackedWidget *stackedWidget; // 页面栈管理器，用于切换登录/大厅/战斗等页面
    
    // Page 1: 登录/注册
    QLineEdit *inputUser, *inputPass;
    
    // Page 2: 游戏大厅
    QLabel *labelWelcome;

    // Page 3: 英雄选择
    QListWidget *listAllHeroes; // 左侧列表
    QListWidget *listSelected;  // 右侧已选列表
    
    // Page 4: 战斗界面
    QTextEdit *battleLog;       // 战斗日志显示框
    QLabel *labelRoundInfo;     // 回合数显示
    QLabel *labelCpuStatus;     // 电脑状态
    QLabel *labelMyStatus;      // 我方状态
    QPushButton *btnScissors, *btnRock, *btnPaper; // 出招按钮
    
    // Page 5: 排行榜
    QTextEdit *textRank;


    // === 【新增】定时器相关变量 ===
    QTimer *battleTimer;    // 定时器对象
    int remainingTime;      // 剩余秒数
    QLabel *labelTimer;     // 用于在界面显示倒计时的文字
    const int TIME_LIMIT = 10; // 设定超时时间，例如 10 秒

    // --- 界面构建函数 (将UI代码拆分，保持整洁) ---
    void initUI();
    void createLoginPage();
    void createLobbyPage();
    void createHeroSelectPage();
    void createBattlePage();
    void createRankPage();

    // --- 游戏流程逻辑 ---
    void refreshHeroList();         // 刷新选人列表
    void startNewGame();            // 初始化新游戏数据
    void startRound();              // 开始一个新的回合
    void endRound(MoveType myMove); // 结算当前回合
    void endGame();                 // 9回合结束，结算胜负

private slots: 
    // [槽函数]：这些函数会与界面上的按钮点击信号连接
    void onBtnLoginClicked();
    void onBtnRegisterClicked();
    void onBtnStartGameClicked();       // 大厅点击开始游戏
    void onBtnHeroSelectAddClicked();   // 双击添加英雄
    void onBtnHeroSelectConfirmClicked(); // 确认阵容
    void onBtnRankClicked();            // 查看排行
    void onBtnBackToLobbyClicked();     // 返回大厅
    
    // 战斗出招槽
    void onUseScissors();
    void onUseRock();
    void onUsePaper();
    void onBattleTimerTick(); // 每秒触发一次，用于更新倒计时和判断超时
};

#endif // MAIN_WINDOW_H