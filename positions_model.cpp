#include "positions_model.h"

#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>

PositionsModel::PositionsModel()
    : QAbstractTableModel()
{

}

int PositionsModel::rowCount(const QModelIndex &parent) const
{
    return items.count();
}

int PositionsModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant PositionsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) return QVariant();
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if(index.column() == 0)
        {
            return items[index.row()]->prove;
        }
        if(index.column() == 1)
        {
            return items[index.row()]->name;
        }
        if(index.column() == 2)
        {
            return items[index.row()]->unit;
        }
    }
    return QVariant();
}

void PositionsModel::updateItems(int caption_index)
{
    QSqlQuery query;
    query.prepare("select id, prove, name, unit, metadata from positions where caption_id = ?");
    query.addBindValue(caption_index);
    query.exec();
    if(query.lastError().type() != QSqlError::NoError)
    {
        qDebug() << query.lastError().text();
        return;
    }
    while(query.next())
    {
        items.push_back(new PositionItem(query.value(0).toString(), query.value(1).toString(), query.value(2).toString(), query.value(3).toString(), query.value(4).toString()));
    }
}

int PositionsModel::getId(int row)
{
    return items[row]->id.toInt();
}

Qt::ItemFlags PositionsModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;
    return Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;
}

bool PositionsModel::setData(const QModelIndex &index, const QVariant &val, int role)
{
    if(index.isValid() && role == Qt::EditRole)
    {
        auto item = items[index.row()];
        QSqlQuery query;
        QVariant value = val.toString().replace("\n", "");
        if(index.column() == 0)
        {
            query.prepare("update positions set prove = ? where id = ?");
            query.addBindValue(value);
            query.addBindValue(item->id);
            query.exec();
            item->prove = value.toString();
        }
        else if(index.column() == 1)
        {
            query.prepare("update positions set name = ? where id = ?");
            query.addBindValue(value);
            query.addBindValue(item->id);
            query.exec();
            item->name = value.toString();
        }
        else if(index.column() == 2)
        {
            query.prepare("update positions set unit = ? where id = ?");
            query.addBindValue(value);
            query.addBindValue(item->id);
            query.exec();
            item->unit = value.toString();
        }
        else return false;
        if(query.lastError().type() != QSqlError::NoError)
        {
            qDebug() << query.lastError().text();
            return false;
        }
        items.replace(index.row(), item);

        emit(dataChanged(index, index));

        return true;
    }

    return false;
}

void PositionsModel::addPosition(int captionIndex)
{
    QSqlQuery query;
    query.prepare("insert into positions(caption_id, name) values(?, ?)");
    query.addBindValue(captionIndex);
    query.addBindValue("Новая позиция");
    query.exec();
    if(query.lastError().type() != QSqlError::NoError)
    {
        qDebug() << query.lastError().text();
    }
    items.push_back(new PositionItem(query.lastInsertId().toString(), "", "Новый заголовок", "", ""));
    this->dataChanged(this->index(0, 0), this->index(this->rowCount(), this->columnCount()));
}

PositionItem *PositionsModel::getItem(QModelIndex index)
{
    if(index.isValid())
        return items[index.row()];
    return nullptr;
}


PositionItem::PositionItem(QString id, QString prove, QString name, QString unit, QString metadata)
    : id(id)
    , prove(prove)
    , name(name)
    , unit(unit)
    , metadata(metadata)
{

}
