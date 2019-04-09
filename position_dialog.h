#ifndef POSITION_DIALOG_H
#define POSITION_DIALOG_H


#include <QDialog>

#include <QLineEdit>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHBoxLayout>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPushButton>


#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "positions_model.h"




class CoeffsTableItem : public QTreeWidgetItem
{
public:
    CoeffsTableItem(QTreeWidgetItem *parent = nullptr);
    virtual QVariant data(int column, int role) const override;
    virtual void setData(int column, int role, const QVariant &value) override;
public:
    QJsonObject getData();
    void setData(QJsonObject obj);
private:
    QString name;
    double p, rd, rp;
};

class CoeffsTable : public QTreeWidget
{
public:
    CoeffsTable(QWidget *parent);
    void addItem();
    QJsonObject getJson();
    void setJson(QJsonObject);
};

class PositionDialog : public QDialog
{
public:
    PositionDialog(int caption_id, PositionItem *item, QWidget *parent);
private:
    PositionItem *item;
    QLineEdit *proveEdit;
    QLineEdit *nameEdit;
    QLineEdit *unitEdit;
    CoeffsTable *coeffsTable;
    QPushButton *addCoeffBtn;
    QPushButton *removeCoeffBtn;
    int caption_id;
    double k1, k2;
};

#endif // POSITION_DIALOG_H
