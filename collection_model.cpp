#include "collection_model.h"

#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>

CollectionModel::CollectionModel()
    : QAbstractTableModel()
{
    updateItems();
}

int CollectionModel::rowCount(const QModelIndex &parent) const
{
    return items.count();
}

int CollectionModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant CollectionModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) return QVariant();
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if(index.column() == 0)
        {
            return items[index.row()]->name;
        }
    }
    return QVariant();
}

int CollectionModel::getId(int row)
{
    return items[row]->id.toInt();
}

void CollectionModel::updateItems()
{
    QSqlQuery query;
    query.exec("select id, name from collections");
    if(query.lastError().type() != QSqlError::NoError)
    {
        qDebug() << query.lastError().text();
        return;
    }
    while(query.next())
    {
        items.push_back(new CollectionItem(query.value(0).toString(), query.value(1).toString()));
    }
}

void CollectionModel::addCollection()
{
    QSqlQuery query;
    query.exec("insert into collections(dir_id, type, name) values(1, 12, 'Новый сборник')");
    if(query.lastError().type() != QSqlError::NoError)
    {
        qDebug() << query.lastError().text();
        return;
    }
    this->dataChanged(this->index(0, 0), this->index(this->rowCount(), this->columnCount()));
}

void CollectionModel::updateItem(int index)
{
    QSqlQuery query;
    query.prepare("select id, name from collections where id = ?");
    query.addBindValue(index);
    query.exec();
    if(query.lastError().type() != QSqlError::NoError)
    {
        qDebug() << query.lastError().text();
        return;
    }
    query.next();
    items.push_back(new CollectionItem(query.value(0).toString(), query.value(0).toString()));
}

bool CollectionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.isValid() && role == Qt::EditRole)
    {
        auto item = items[index.row()];
        QSqlQuery query;
        if(index.column() == 0)
        {
            query.prepare("update captions set name = ? where id = ?");
            query.addBindValue(value);
            query.addBindValue(item->id);
            query.exec();
            item->name = value.toString();
        }
        else return false;

        items.replace(index.row(), item);
        if(query.lastError().type() != QSqlError::NoError)
        {
            qDebug() << query.lastError().text();
            return false;
        }

        emit(dataChanged(index, index));

        return true;
    }

    return false;

}

Qt::ItemFlags CollectionModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;
    return Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;
}

CollectionItem::CollectionItem(QString id, QString name)
    : id(id)
    , name(name)
{

}
