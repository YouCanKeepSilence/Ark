#include "optionswindow.h"
#include "aboutwindow.h"
#include <QFile>
#include <QHeaderView>
#include <QApplication>
#include <QDesktopWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>


OptionsWindow::OptionsWindow(GameArea *area, int initDifficulty, int screenWidth, QWidget *parent):
    QWidget(parent),
    currentDifficulty(initDifficulty)
{
    gameArea=area;                                                          //прикрепляем игровую зону

    QSettings settings;
    settings.beginGroup("OptionsWindow");
        currentDifficulty=settings.value("difficulty",QVariant(initDifficulty)).toInt();
        gameArea->setDifficulty(currentDifficulty);
        bool isMuted=settings.value("isMuted",QVariant(false)).toBool();
        int volume=settings.value("volume",QVariant(50)).toInt();
    settings.endGroup();

    shapeScore = new NextShapeAndScore(screenWidth/68);                      //создаем виджеты
    names = new QLabel("Made by:\n@Befezdow\n@YouCanKeepSilence");
    names->setAlignment(Qt::AlignHCenter);
    names->setObjectName("DevNames");
//    names->setStyleSheet("background-color: rgb(219,157,34);"
//                         "border-radius: 25px;"
//                         "border: 3px ridge purple");
    names->setMargin(10);
    soundWidget = new SoundController(volume,isMuted);
    pause = new QPushButton("Pause");
    restart = new QPushButton("Restart");
    difficulty = new QPushButton("Difficulty");
    highScores = new QPushButton("Highscores");
    about = new QPushButton("About");
    vlay = new QVBoxLayout;
    buttonLay = new QVBoxLayout;

    pause->setFixedSize(area->width()/3,area->width()/10);
    restart->setFixedSize(area->width()/3,area->width()/10);
    difficulty->setFixedSize(area->width()/3,area->width()/10);
    highScores->setFixedSize(area->width()/3,area->width()/10);
    about->setFixedSize(area->width()/3,area->width()/10);
    pause->setObjectName("b1");
    restart->setObjectName("b1");
    difficulty->setObjectName("b1");
    highScores->setObjectName("b1");
    about->setObjectName("b1");

    //соединяем слоты
    QObject::connect(gameArea,SIGNAL(scoreChanged(unsigned int)),shapeScore,SLOT(changeScore(unsigned int)));
    QObject::connect(gameArea,SIGNAL(gameOver(uint)),this,SLOT(insertRecord(uint)));
    QObject::connect(gameArea,SIGNAL(throwNextFigure(int,QColor)),shapeScore,SLOT(setNextFigure(int,QColor)));
    QObject::connect(pause,SIGNAL(clicked(bool)),gameArea,SLOT(switchPause()));
    QObject::connect(restart,SIGNAL(clicked(bool)),gameArea,SLOT(start()));
    QObject::connect(difficulty,SIGNAL(clicked(bool)),this,SLOT(showDifficultyWindow()));
    QObject::connect(highScores,SIGNAL(clicked(bool)),this,SLOT(showScoreTable()));
    QObject::connect(about,SIGNAL(clicked(bool)),this,SLOT(aboutProgram()));

    QObject::connect(gameArea,SIGNAL(showHighScores()),this,SLOT(showScoreTable()));
    QObject::connect(gameArea,SIGNAL(showDifficulty()),this,SLOT(showDifficultyWindow()));

    //собираем виджеты в слои
    buttonLay->addWidget(pause,0,Qt::AlignHCenter);
    buttonLay->addWidget(restart,0,Qt::AlignHCenter);
    buttonLay->addWidget(difficulty,0,Qt::AlignHCenter);
    buttonLay->addWidget(highScores,0,Qt::AlignHCenter);
    buttonLay->addWidget(about,0,Qt::AlignHCenter);

    vlay->addWidget(shapeScore,0,Qt::AlignHCenter | Qt::AlignTop);
    vlay->addLayout(buttonLay);
    vlay->addWidget(names,1,Qt::AlignHCenter | Qt::AlignBottom);
    vlay->addWidget(soundWidget,0,Qt::AlignHCenter);

    this->setLayout(vlay);                  //устанавливаем слой

    shapeScore->setFocusProxy(gameArea);    //устанавливаем пренаправление фокуса клавиатуры на игровую зону
    pause->setFocusProxy(gameArea);
    restart->setFocusProxy(gameArea);
    difficulty->setFocusProxy(gameArea);
    highScores->setFocusProxy(gameArea);
    about->setFocusProxy(gameArea);
    soundWidget->setFocusProxy(gameArea);
    this->setFocusProxy(gameArea);

    scoresView = new ScoreWidget;

    this->attachFile(0,"saves0.svs");
    this->attachFile(1,"saves1.svs");
    this->attachFile(2,"saves2.svs");
    this->attachFile(3,"saves3.svs");
    this->attachFile(4,"saves4.svs");

    this->readScores();
}

OptionsWindow::~OptionsWindow()
{
    QSettings settings;
    settings.beginGroup("OptionsWindow");
        settings.setValue("difficulty",QVariant(currentDifficulty));
        settings.setValue("isMuted",QVariant(soundWidget->isMuted()));
        settings.setValue("volume",QVariant(soundWidget->getVolume()));
    settings.endGroup();
}

void OptionsWindow::attachFile(unsigned int dif, QString fileName)
{
    this->fileName[dif%5]=fileName;
}

void OptionsWindow::showDifficultyWindow()
{
    bool unpause=false;                                 //флаг для снятия паузы
    if (!gameArea->isPaused())                          //если игра не на паузе
    {
        unpause=true;                                   //говорим что нужно будет снять паузу
        gameArea->switchPause();                        //ставим игру на паузу
    }
    DifficultyWindow diff(currentDifficulty);           //создаем окно сложности

    if (diff.exec()==QDialog::Accepted)                 //вызываем его
    {                                                   //если юзер нажал принять
        currentDifficulty=diff.getCurrentDifficulty();  //получаем сложность
        gameArea->setDifficulty(currentDifficulty);     //устанавливаем сложность
        gameArea->start();                              //перезапускаем игру
    }
    else                                                //если нажал отмену или закрыл
    {
        if (unpause)                                    //если нужно снять с паузы
        {
            gameArea->switchPause();                    //снимаем игру с паузы
        }
    }
}

void OptionsWindow::readScores()
{
    for (int i=0; i<5; ++i)
    {
        scores[i].clear();                                  //очищаем текущие рекорды

        QFile file(fileName[i]);                            //сам файл
        if (!file.open(QIODevice::ReadOnly))                //открываем его
        {
            scores[i].push_back(ScoreTableElement(QDateTime::currentDateTime(),1000,"The Good"));
            scores[i].push_back(ScoreTableElement(QDateTime::currentDateTime(),500,"The Bad"));
            scores[i].push_back(ScoreTableElement(QDateTime::currentDateTime(),100,"The Ugly"));

            scoresView->updateInfo(i,scores[i]);

            continue;
        }

        QDataStream out(&file);                             //открываем поток для файла
        while (true)                                        //читаем рекорды
        {
            QDateTime t;
            unsigned int s;
            QString p;
            out>>t>>s>>p;                                   //читаем данные

            scores[i].push_back(ScoreTableElement(t,s,p)); //закидываем данные в рекорды
            if (out.atEnd())                                //если поток закончился
            {
                break;                                      //отваливаемся
            }
        }

        scoresView->updateInfo(i,scores[i]);

        file.close();                                       //закрываем файл
    }
}

void OptionsWindow::writeScores()
{
    for (int i=0;i<5;++i)
    {
        QFile file(fileName[i]);                            //сам файл
        if (!file.open(QIODevice::WriteOnly))               //открываем его
        {
            continue;
        }

        QDataStream out(&file);                             //открываем поток для файла

        for (int j=0;j<scores[i].size();++j)                //идем по списку рекордов
        {
            ScoreTableElement e=scores[i].at(j);               //получаем элемент
            out<<e.time<<e.score<<e.playerName;             //пишем его в файл
        }

        file.close();                                       //закрываем файл
    }
}

void OptionsWindow::addRecord(unsigned int score,QString player)
{
    scores[currentDifficulty].push_back(ScoreTableElement(QDateTime::currentDateTime(),score,player));
                                                                //добавляем новый рекорд
    std::sort(scores[currentDifficulty].begin(),scores[currentDifficulty].end());
                                                                //сортируем список рекордов
    if (scores[currentDifficulty].size()>10)                    //если в нем больше 10 элементов
    {
        scores[currentDifficulty].pop_back();                   //последний выкидываем
    }
    scoresView->updateInfo(currentDifficulty,scores[currentDifficulty]);
}

void OptionsWindow::showScoreTable()
{
    bool unpause=false;                                 //флаг для снятия паузы
    if (!gameArea->isPaused())                          //если игра не на паузе
    {
        unpause=true;                                   //говорим что нужно будет снять паузу
        gameArea->switchPause();                        //ставим игру на паузу
    }

    scoresView->exec();                                 //вызываем окно рекордов

    if (unpause)                                        //если нужно снять с паузы
    {
        gameArea->switchPause();                        //снимаем игру с паузы
    }
}

void OptionsWindow::insertRecord(unsigned int score)
{
    if (score==0)
    {
        return;
    }

    if (!checkForAdding(score))
    {
        return;
    }
    bool check=true;
    QString playerName=QInputDialog::getText(Q_NULLPTR,"Victory",
                                             "You have entered the high score table.\nPlease enter your nickname:",
                                             QLineEdit::Normal,"Winner",&check);
    playerName.truncate(30);
    if (check)
    {
        this->addRecord(score,playerName);
        this->writeScores();
    }
}

void OptionsWindow::aboutProgram()
{
//    QString aboutInfo="<h2>Vitae Of Blocks</h2>"
//                      "<h4>Version 1.3</h4>"
//                      "This program was written by a couple of<br>"
//                      "students in order to test their abilities.<br>"
//                      "It uses Qt version 5.8.0.<br><br>"
//                      "<b>Developers:</b><br>#Befezdow<br>#YouCanKeepSilence<br><br>"
//                      "<b>Special thanks to:</b><br>#Kampfer<br>#Forze<br>"
//                      "#Altum Silentium<br>#Lieee.s<br>#DuMaHbl4<br><br>"
//                      "<b>Music:</b><br>";

    bool unpause=false;                                 //флаг для снятия паузы
    if (!gameArea->isPaused())                          //если игра не на паузе
    {
        unpause=true;                                   //говорим что нужно будет снять паузу
        gameArea->switchPause();                        //ставим игру на паузу
    }

    AboutWindow aw;
    aw.exec();

    if (unpause)                                    //если нужно снять с паузы
    {
        gameArea->switchPause();                    //снимаем игру с паузы
    }
}

bool OptionsWindow::checkForAdding(unsigned int score)
{
    if (scores[currentDifficulty].size()<10)
    {
        return true;
    }
    else
    {
        if (scores[currentDifficulty].back().score<score)
        {
            return true;
        }
    }
    return false;
}

OptionsWindow::ScoreTableElement::ScoreTableElement():
    score(0)
{}

OptionsWindow::ScoreTableElement::ScoreTableElement(QDateTime t, unsigned int s, QString p):
    time(t),
    score(s),
    playerName(p)
{}

bool OptionsWindow::ScoreTableElement::operator<(const OptionsWindow::ScoreTableElement &ob) const
{
    if (score<ob.score)                         //сравниваем счет
    {
        return false;                           //инверсия, так как хочу сортировку по убыванию
    }
    else
    {
        if (score==ob.score && time>ob.time)    //если счет совпадает, сраниваем время
        {
            return false;                       //инверсия, так как хочу сортировку по убыванию
        }
        return true;                            //инверсия, так как хочу сортировку по убыванию
    }
}

OptionsWindow::ScoreWidget::ScoreWidget()
{
    for (int i=0;i<5;++i)
    {
        table[i]=new QTableWidget(0,4);                         //создаем виджет таблицы

        QStringList lst;                                        //временный список
        lst<<"Name"<<"Score"<<"Time"<<"Date";
        table[i]->setHorizontalHeaderLabels(lst);               //задаем горизонтальный заголовок

        table[i]->verticalHeader()->resize(10,10);              //без этого говорит, что ширина заголовка 0
        table[i]->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
                                                                //фиксируем высоту строки
        table[i]->setShowGrid(false);                           //убираем сетку
        table[i]->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                                                                //убираем вертикальный слайдер
        table[i]->horizontalHeader()->setMinimumSectionSize(qApp->desktop()->screen()->width()/15);
                                                                //устанавливаем минимальную ширину столбца
   }

    ok=new QPushButton("Ok");                                   //создаем кнопку и соединяем со слотом
    QObject::connect(ok,SIGNAL(clicked(bool)),this,SLOT(accept()));

    tabs=new QTabWidget;                                        //создаем виджет вкладок
    tabs->setTabPosition(QTabWidget::South);                    //размещаем вкладки снизу
    //TODO отцентрировать вкладки по горизонтали или растянуть по всеё ширине
    tabs->tabBar()->setExpanding(true);                         //почему то не растягивает!!!

    tabs->addTab(table[0],"Free");                              //добавляем вкладки
    tabs->addTab(table[1],"Easy");
    tabs->addTab(table[2],"Normal");
    tabs->addTab(table[3],"Hard");
    tabs->addTab(table[4],"Master");

    QVBoxLayout* lay=new QVBoxLayout;                           //создаем слой

    lay->addWidget(tabs);                                       //закидываем в него вкладки
    lay->addWidget(ok,0,Qt::AlignHCenter | Qt::AlignBottom);

    this->setLayout(lay);                                       //устанавливаем этот слой
    setMinimumWidth(tabs->minimumSizeHint().width());
}

void OptionsWindow::ScoreWidget::updateInfo(unsigned int dif, QList<OptionsWindow::ScoreTableElement> scoreList)
{
    unsigned int tableNumber=dif%5;                             //страхуемся от некорректных значений

    table[tableNumber]->clearContents();                        //очищаем таблицу

    table[tableNumber]->setRowCount(scoreList.size());          //устанавливаем нужное количество строк

    for (int i=0;i<scoreList.size();++i)                        //закидываем в таблицу элементы
    {
        ScoreTableElement elem=scoreList.at(i);                 //получаем элемент из списка
        QTableWidgetItem* itemName=new QTableWidgetItem(elem.playerName);                           //создаем ячейку имени
        QTableWidgetItem* itemScore=new QTableWidgetItem(QString::number(elem.score));              //создаем ячейку счета
        QTableWidgetItem* itemTime=new QTableWidgetItem(elem.time.time().toString("hh:mm:ss"));     //создаем ячейку времени
        QTableWidgetItem* itemDate=new QTableWidgetItem(elem.time.date().toString("dd.MM.yyyy"));   //создаем ячейку даты

        itemName->setFlags(Qt::ItemIsEnabled);                  //говорим, что ячейки не редактируемые и не выбираемые
        itemScore->setFlags(Qt::ItemIsEnabled);
        itemTime->setFlags(Qt::ItemIsEnabled);
        itemDate->setFlags(Qt::ItemIsEnabled);

        itemName->setTextAlignment(Qt::AlignCenter);            //устанавливаем выравнивание текста в ячейках
        itemScore->setTextAlignment(Qt::AlignCenter);
        itemTime->setTextAlignment(Qt::AlignCenter);
        itemDate->setTextAlignment(Qt::AlignCenter);

        table[tableNumber]->setItem(i,0,itemName);              //вставляем ячейки в таблицу в одну строчку
        table[tableNumber]->setItem(i,1,itemScore);
        table[tableNumber]->setItem(i,2,itemTime);
        table[tableNumber]->setItem(i,3,itemDate);
    }

    table[tableNumber]->resizeColumnsToContents();              //изменяем размер колонок в соответствии с содержимым

    int maxNameLengthInPixels=0;                                //ширина колонок с именами
    for (int i=0; i<5; ++i)                                     //идем по всем таблицам
    {
        int temp=table[i]->horizontalHeader()->sectionSize(0);  //временная переменная
        if (temp>maxNameLengthInPixels)                         //находим максимальную ширину
        {
            maxNameLengthInPixels=temp;
        }
    }

    for (int i=0; i<5; ++i)                                     //ставим её для всех таблиц
    {
        table[i]->horizontalHeader()->resizeSection(0,maxNameLengthInPixels);
    }

    int width=table[tableNumber]->verticalHeader()->width();    //считаем ширину поля
    for (int i=0;i<table[tableNumber]->columnCount();++i)
    {
         width+=table[tableNumber]->columnWidth(i);
    }

    int height=table[tableNumber]->horizontalHeader()->height();//считаем высоту поля
    for (int i=0;i<table[tableNumber]->rowCount();++i)
    {
        height+=table[tableNumber]->rowHeight(i);
    }

    table[tableNumber]->setMinimumSize(width+5,height);         //устанавливаем минимальный размер таблицы
    this->setMinimumSize(tabs->minimumSize());                  //устанавливаем минимальный размер окна
}
