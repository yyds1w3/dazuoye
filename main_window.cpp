/**
 * 文件名: main_window.cpp
 * 描述: 包含了所有的界面布局代码和按钮交互逻辑。
 */
#include "main_window.h"
#include <QApplication>
#include <QHeaderView>

// 构造函数：初始化界面并设置窗口大小
MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    initUI();
    resize(800, 600); // 设置窗口默认大小
    setWindowTitle("王者农药");
}

MainWindow::~MainWindow() {}

// 初始化 UI 框架
void MainWindow::initUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // QStackedWidget 是一个容器，可以存放多个页面，但一次只显示一个
    // 就像一副扑克牌，我们通过 setCurrentIndex 来切牌
    stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(stackedWidget);

    // 按顺序创建 5 个页面，索引分别是 0, 1, 2, 3, 4
    createLoginPage();      // Index 0
    createLobbyPage();      // Index 1
    createHeroSelectPage(); // Index 2
    createBattlePage();     // Index 3
    createRankPage();       // Index 4

    stackedWidget->setCurrentIndex(0); // 程序启动默认显示登录页
}

// ================== 1. 登录页面 ==================
void MainWindow::createLoginPage() {
    QWidget *page = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(page);
    
    QLabel *title = new QLabel("欢迎来到《王者农药》");
    title->setStyleSheet("font-size: 24px; font-weight: bold; margin-bottom: 20px;");
    title->setAlignment(Qt::AlignCenter);

    inputUser = new QLineEdit; inputUser->setPlaceholderText("用户名");
    inputPass = new QLineEdit; inputPass->setPlaceholderText("密码");
    inputPass->setEchoMode(QLineEdit::Password); // 密码模式显示星号

    QPushButton *btnLogin = new QPushButton("登录");
    QPushButton *btnReg = new QPushButton("注册");

    // 连接信号与槽：点击按钮 -> 触发对应的函数
    connect(btnLogin, &QPushButton::clicked, this, &MainWindow::onBtnLoginClicked);
    connect(btnReg, &QPushButton::clicked, this, &MainWindow::onBtnRegisterClicked);

    // 使用 addStretch 挤压布局，使内容垂直居中
    layout->addStretch();
    layout->addWidget(title);
    layout->addWidget(inputUser);
    layout->addWidget(inputPass);
    layout->addWidget(btnLogin);
    layout->addWidget(btnReg);
    layout->addStretch();
    
    // 限制输入框宽度，美观
    inputUser->setMaximumWidth(300);
    inputPass->setMaximumWidth(300);
    
    // 使用一个中间 Widget 包裹布局来实现完全居中
    QWidget *centerWidget = new QWidget;
    centerWidget->setLayout(layout);
    QVBoxLayout *outerLayout = new QVBoxLayout(page);
    outerLayout->addWidget(centerWidget, 0, Qt::AlignCenter);

    stackedWidget->addWidget(page);
}

void MainWindow::onBtnLoginClicked() {
    QString u = inputUser->text();
    QString p = inputPass->text();
    // 调用逻辑层 DataManager 进行验证
    if(dataMgr.login(u.toStdString(), p.toStdString())) {
        labelWelcome->setText("欢迎回来，召唤师: " + u);
        stackedWidget->setCurrentIndex(1); // 登录成功，跳转到大厅(Index 1)
    } else {
        QMessageBox::warning(this, "错误", "用户名或密码错误");
    }
}

void MainWindow::onBtnRegisterClicked() {
    QString u = inputUser->text();
    QString p = inputPass->text();
    if(u.isEmpty() || p.isEmpty()) {
        QMessageBox::warning(this, "提示", "用户名密码不能为空");
        return;
    }
    if(dataMgr.registerUser(u.toStdString(), p.toStdString())) {
        QMessageBox::information(this, "成功", "注册成功，请登录");
    } else {
        QMessageBox::warning(this, "错误", "用户名已存在");
    }
}

// ================== 2. 大厅页面 ==================
void MainWindow::createLobbyPage() {
    QWidget *page = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(page);

    labelWelcome = new QLabel("欢迎");
    labelWelcome->setAlignment(Qt::AlignCenter);
    
    QPushButton *btnStart = new QPushButton("开始游戏");
    QPushButton *btnRank = new QPushButton("查看排行榜");
    QPushButton *btnExit = new QPushButton("退出");

    btnStart->setMinimumHeight(50); // 让开始按钮大一点

    connect(btnStart, &QPushButton::clicked, this, &MainWindow::onBtnStartGameClicked);
    connect(btnRank, &QPushButton::clicked, this, &MainWindow::onBtnRankClicked);
    connect(btnExit, &QPushButton::clicked, qApp, &QApplication::quit); // 退出程序

    layout->addStretch();
    layout->addWidget(labelWelcome);
    layout->addWidget(btnStart);
    layout->addWidget(btnRank);
    layout->addWidget(btnExit);
    layout->addStretch();

    stackedWidget->addWidget(page);
}

void MainWindow::onBtnStartGameClicked() {
    refreshHeroList(); // 加载最新英雄数据
    myHeroIndices.clear();
    listSelected->clear();
    stackedWidget->setCurrentIndex(2); // 跳转到选人页
}

// ================== 3. 选人页面 ==================
void MainWindow::createHeroSelectPage() {
    QWidget *page = new QWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout(page); // 左右布局

    // 左边：所有英雄列表
    QGroupBox *grpLeft = new QGroupBox("所有英雄 (双击添加)");
    QVBoxLayout *leftLayout = new QVBoxLayout(grpLeft);
    listAllHeroes = new QListWidget;
    connect(listAllHeroes, &QListWidget::itemDoubleClicked, this, &MainWindow::onBtnHeroSelectAddClicked);
    leftLayout->addWidget(listAllHeroes);

    // 右边：已选阵容
    QGroupBox *grpRight = new QGroupBox("已选阵容 (最多3个)");
    QVBoxLayout *rightLayout = new QVBoxLayout(grpRight);
    listSelected = new QListWidget;
    QPushButton *btnConfirm = new QPushButton("出征！");
    connect(btnConfirm, &QPushButton::clicked, this, &MainWindow::onBtnHeroSelectConfirmClicked);
    
    rightLayout->addWidget(listSelected);
    rightLayout->addWidget(btnConfirm);

    mainLayout->addWidget(grpLeft);
    mainLayout->addWidget(grpRight);

    stackedWidget->addWidget(page);
}

// 刷新左侧英雄列表
void MainWindow::refreshHeroList() {
    listAllHeroes->clear();
    for(int i=0; i<dataMgr.heroes.size(); ++i) {
        const auto& h = dataMgr.heroes[i];
        QString info = QString::fromStdString(h.name) + 
                       QString(" (剪%1/石%2/布%3)").arg(h.s).arg(h.r).arg(h.p);
        QListWidgetItem *item = new QListWidgetItem(info);
        // 使用 UserRole 存储该英雄在 vector 中的原始索引，方便后续查找
        item->setData(Qt::UserRole, i); 
        listAllHeroes->addItem(item);
    }
}

// 双击添加英雄
void MainWindow::onBtnHeroSelectAddClicked() {
    if(myHeroIndices.size() >= 3) return; // 最多选3个
    
    QListWidgetItem *item = listAllHeroes->currentItem();
    int idx = item->data(Qt::UserRole).toInt();

    // 查重：防止选择同一个英雄
    for(int i : myHeroIndices) if(i == idx) {
        QMessageBox::information(this, "提示", "不能重复选择英雄");
        return;
    }

    myHeroIndices.push_back(idx);
    listSelected->addItem(item->text());
}

// 确认阵容，进入战斗
void MainWindow::onBtnHeroSelectConfirmClicked() {
    if(myHeroIndices.size() < 3) {
        QMessageBox::warning(this, "提示", "请选满3个英雄");
        return;
    }
    startNewGame();
    stackedWidget->setCurrentIndex(3); // 进战斗页
}

// ================== 4. 战斗页面 (核心逻辑) ==================
void MainWindow::createBattlePage() {
    QWidget *page = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(page);

    // 顶部：回合显示
    labelRoundInfo = new QLabel("第 1 回合");
    labelRoundInfo->setStyleSheet("font-size: 18px; color: blue; font-weight: bold;");
    labelRoundInfo->setAlignment(Qt::AlignCenter);
    
    // === 【新增代码开始】 ===
    // 创建倒计时标签，设为红色显眼字体
    labelTimer = new QLabel("剩余时间: 10");
    labelTimer->setStyleSheet("font-size: 16px; color: red; font-weight: bold;");
    labelTimer->setAlignment(Qt::AlignCenter);

    // 初始化定时器
    battleTimer = new QTimer(this);
    // 连接信号：每当定时器“响”一次，就执行 onBattleTimerTick
    connect(battleTimer, &QTimer::timeout, this, &MainWindow::onBattleTimerTick);
    // === 【新增代码结束】 ===

    // 中部：双方状态
    QHBoxLayout *statusLayout = new QHBoxLayout;
    labelMyStatus = new QLabel("我方: ???");
    labelCpuStatus = new QLabel("电脑: ???");
    statusLayout->addWidget(labelMyStatus);
    statusLayout->addWidget(labelCpuStatus);

    // 中部：战斗日志 (只读)
    battleLog = new QTextEdit;
    battleLog->setReadOnly(true);

    // 底部：出招按钮
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnScissors = new QPushButton("剪刀");
    btnRock = new QPushButton("石头");
    btnPaper = new QPushButton("布");
    
    connect(btnScissors, &QPushButton::clicked, this, &MainWindow::onUseScissors);
    connect(btnRock, &QPushButton::clicked, this, &MainWindow::onUseRock);
    connect(btnPaper, &QPushButton::clicked, this, &MainWindow::onUsePaper);

    btnLayout->addWidget(btnScissors);
    btnLayout->addWidget(btnRock);
    btnLayout->addWidget(btnPaper);

    layout->addWidget(labelRoundInfo);
    layout->addWidget(labelTimer); // <--- 【别忘了把标签加进布局里】
    layout->addLayout(statusLayout);
    layout->addWidget(battleLog);
    layout->addLayout(btnLayout);

    stackedWidget->addWidget(page);
}

// 游戏初始化
void MainWindow::startNewGame() {
    currentRound = 1;
    myScore = 0; cpuScore = 0;
    battleLog->clear();
    battleLog->append("=== 战斗开始 ===");

    // 重置我方选中的英雄状态 (满血复活)
    for(int idx : myHeroIndices) dataMgr.heroes[idx].reset();

    // 电脑随机选 3 个不同的英雄
    cpuHeroIndices.clear();
    vector<int> pool;
    for(int i=0; i<dataMgr.heroes.size(); ++i) pool.push_back(i);
    // 随机打乱索引数组
    shuffle(pool.begin(), pool.end(), mt19937(time(0))); 
    for(int i=0; i<3; ++i) {
        cpuHeroIndices.push_back(pool[i]);
        dataMgr.heroes[pool[i]].reset(); // 重置电脑英雄状态
    }
    
    QString cpuNames;
    for(int idx : cpuHeroIndices) cpuNames += QString::fromStdString(dataMgr.heroes[idx].name) + " ";
    battleLog->append("电脑选择了: " + cpuNames);

    startRound(); // 开始第1回合
}

// 回合开始逻辑
void MainWindow::startRound() {
    if(currentRound > 9) { // 超过9回合，游戏结束
        endGame();
        return;
    }

    labelRoundInfo->setText(QString("--- 第 %1 回合 ---").arg(currentRound));

    // 1. 电脑选人 (必须有招可用)
    vector<int> cpuValid;
    for(int i=0; i<3; ++i) if(dataMgr.heroes[cpuHeroIndices[i]].hasMoves()) cpuValid.push_back(i);
    
    if(cpuValid.empty()) {
        battleLog->append("电脑无牌可出，提前认输！");
        endGame();
        return;
    }
    
    // 电脑随机选一个可用的英雄
    static mt19937 rng(time(0));
    currentCpuHeroIndex = cpuValid[rng() % cpuValid.size()];
    int realCpuIdx = cpuHeroIndices[currentCpuHeroIndex];
    
    // 电脑预先出招 (此时不告诉玩家出了什么，只记录在 cpuNextMove)
    cpuNextMove = dataMgr.heroes[realCpuIdx].makeRandomMove();
    
    labelCpuStatus->setText(QString("电脑派出: %1 (已出招)").arg(QString::fromStdString(dataMgr.heroes[realCpuIdx].name)));

    // 2. 更新我方按钮状态 (UI 交互优化)
    // 检查我方选中的3个英雄中，是否有剪刀/石头/布
    bool hasS = false, hasR = false, hasP = false;
    for(int idx : myHeroIndices) {
        if(dataMgr.heroes[idx].canUse(SCISSORS)) hasS = true;
        if(dataMgr.heroes[idx].canUse(ROCK)) hasR = true;
        if(dataMgr.heroes[idx].canUse(PAPER)) hasP = true;
    }
    // 如果没有任何英雄有“剪刀”，则禁用“剪刀”按钮，防止误操作
    btnScissors->setEnabled(hasS);
    btnRock->setEnabled(hasR);
    btnPaper->setEnabled(hasP);

    battleLog->append("请出招...");
    // === 【新增代码】 ===
    remainingTime = TIME_LIMIT; // 重置时间
    labelTimer->setText(QString("剩余时间: %1 秒").arg(remainingTime));
    battleTimer->start(1000); // 启动定时器，参数 1000 代表每 1000毫秒 (1秒) 触发一次
}

// 结算回合逻辑
void MainWindow::endRound(MoveType myMove) {
    // === 【新增代码】 ===
    battleTimer->stop(); // 玩家已操作，停止计时！
    // 1. 玩家出招：系统自动寻找我方第一个拥有该招数的英雄 (简化逻辑)
    int realMyIdx = -1;
    for(int idx : myHeroIndices) {
        if(dataMgr.heroes[idx].canUse(myMove)) {
            realMyIdx = idx;
            break;
        }
    }
    
    Hero& myHero = dataMgr.heroes[realMyIdx];
    myHero.useMove(myMove); // 扣除库存

    Hero& cpuHero = dataMgr.heroes[cpuHeroIndices[currentCpuHeroIndex]];

    // 2. 胜负判定算法
    // (0-2=-2 -> +3=1 -> %3=1)
    int res = (myMove - cpuNextMove + 3) % 3;
    QString resultStr;
    
    // 更新局内数据和全局榜单数据
    myHero.totalMatches++;
    cpuHero.totalMatches++;

    if(res == 1) { // 胜
        resultStr = "胜";
        myScore++;
        myHero.winMatches++;
    } else if (res == 2) { // 负
        resultStr = "负";
        cpuScore++;
    } else {
        resultStr = "平";
    }

    // 3. 记录日志
    QString log = QString("我方[%1]出%2 vs 电脑[%3]出%4 -> %5")
            .arg(QString::fromStdString(myHero.name))
            .arg(QString::fromStdString(moveToString(myMove)))
            .arg(QString::fromStdString(cpuHero.name))
            .arg(QString::fromStdString(moveToString(cpuNextMove)))
            .arg(resultStr);
    
    battleLog->append(log);

    // 4. 准备下一回合
    currentRound++;
    // 使用 QTimer 延迟 1秒 进入下一回合，给玩家看清楚结果的时间
    QTimer::singleShot(1000, this, &MainWindow::startRound);
}

void MainWindow::onUseScissors() { endRound(SCISSORS); }
void MainWindow::onUseRock() { endRound(ROCK); }
void MainWindow::onUsePaper() { endRound(PAPER); }

// 游戏结束结算
void MainWindow::endGame() {
    QString finalMsg = QString("游戏结束！\n比分 %1 : %2\n").arg(myScore).arg(cpuScore);
    if(myScore > cpuScore) {
        finalMsg += "你赢了！";
        dataMgr.currentUser->totalWins++; // 增加玩家胜场
        dataMgr.savePlayers(); // 立即存盘
    } else if(myScore < cpuScore) {
        finalMsg += "你输了。";
    } else {
        finalMsg += "平局。";
    }
    QMessageBox::information(this, "结果", finalMsg);
    
    // 记录对战历史到文件
    ofstream file("gamedata.txt", ios::app);
    file << "Game: " << dataMgr.currentUser->username << " " << myScore << ":" << cpuScore << endl;
    
    stackedWidget->setCurrentIndex(1); // 回大厅
}

// ================== 5. 排行榜 ==================
void MainWindow::createRankPage() {
    QWidget *page = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(page);
    
    textRank = new QTextEdit;
    textRank->setReadOnly(true);
    
    QPushButton *btnBack = new QPushButton("返回大厅");
    connect(btnBack, &QPushButton::clicked, this, &MainWindow::onBtnBackToLobbyClicked);

    layout->addWidget(new QLabel("排行榜"));
    layout->addWidget(textRank);
    layout->addWidget(btnBack);
    
    stackedWidget->addWidget(page);
}

void MainWindow::onBtnRankClicked() {
    stringstream ss;
    ss << "=== 玩家胜场榜 ===\n";
    
    // 复制并排序玩家列表
    vector<Player> sortedP = dataMgr.players;
    sort(sortedP.begin(), sortedP.end(), [](const Player& a, const Player& b){
        return a.totalWins > b.totalWins;
    });
    for(auto& p : sortedP) ss << p.username << "\t" << p.totalWins << "胜\n";

    ss << "\n=== 英雄胜率榜 ===\n";
    // 复制并排序英雄列表
    vector<Hero> sortedH = dataMgr.heroes;
    sort(sortedH.begin(), sortedH.end(), [](const Hero& a, const Hero& b){
        return a.getWinRate() > b.getWinRate();
    });
    for(auto& h : sortedH) {
        ss << h.name << "\t" << (int)h.getWinRate() << "% (" << h.totalMatches << "场)\n"; 
    }

    textRank->setText(QString::fromStdString(ss.str()));
    stackedWidget->setCurrentIndex(4);
}

void MainWindow::onBtnBackToLobbyClicked() {
    stackedWidget->setCurrentIndex(1); // 返回大厅
}

void MainWindow::onBattleTimerTick() {
    remainingTime--; // 时间减 1
    labelTimer->setText(QString("剩余时间: %1 秒").arg(remainingTime));

    // 如果时间到了 0 (或小于0)
    if (remainingTime <= 0) {
        battleTimer->stop(); // 停止计时
        battleLog->append(">>> ⚠ 思考超时！系统自动为您随机出招！");

        // --- 随机替玩家选一个可用的招数 ---
        vector<MoveType> validMoves;
        // 检查我方选中的3个英雄，把所有能用的招数都收集起来
        for (int idx : myHeroIndices) {
            if (dataMgr.heroes[idx].canUse(SCISSORS)) validMoves.push_back(SCISSORS);
            if (dataMgr.heroes[idx].canUse(ROCK))     validMoves.push_back(ROCK);
            if (dataMgr.heroes[idx].canUse(PAPER))    validMoves.push_back(PAPER);
        }

        // 理论上 validMoves 不会为空，因为 startRound 检查过是否有招
        if (!validMoves.empty()) {
            // 随机选一个索引
            static mt19937 rng(time(0));
            int randIndex = rng() % validMoves.size();
            MoveType randomMove = validMoves[randIndex];
            
            // 就像玩家自己点了一样，调用 endRound
            endRound(randomMove);
        } else {
            // 极端情况（不应该发生）：无招可出
            battleLog->append("错误：无招可出。");
        }
    }
}