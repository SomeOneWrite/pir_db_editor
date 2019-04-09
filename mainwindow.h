#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSplitter>

#include <QTableView>
#include <QPushButton>
#include <QTreeView>


class CaptionModel;
class PositionsModel;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QHBoxLayout *lay;
    QSqlDatabase database;
    QSqlQuery *query;
    CaptionModel *currentCaptionModel;
    PositionsModel *currentPositionsModel;
    int currentCollectionId;
    int currentCaptionId;
    int currentPositionId;
};

#endif // MAINWINDOW_H
