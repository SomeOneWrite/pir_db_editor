#include "pricevalues_model.h"

#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>


PriceValueModel::PriceValueModel()
    : QAbstractTableModel()
{

}

void PriceValueModel::addPriceValue(int position_id)
{
    QSqlQuery query;
    query.prepare("insert into price_values(name, position_id) values(?, ?)");
    query.addBindValue("Новая позиция");
    query.addBindValue(position_id);
    query.exec();
    if(query.lastError().type() != QSqlError::NoError)
    {
        qDebug() << query.lastError().text();
        return;
    }
}

int PriceValueModel::rowCount(const QModelIndex &parent) const
{
    return items.count();
}

int PriceValueModel::columnCount(const QModelIndex &parent) const
{
    return 5;
}

QVariant PriceValueModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) return QVariant();
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if(index.column() == 0)
        {
            return items[index.row()]->name;
        }
        if(index.column() == 1)
        {
            return items[index.row()]->a_price;
        }
        if(index.column() == 2)
        {
            return items[index.row()]->b_price;
        }
        if(index.column() == 3)
        {
            return items[index.row()]->min_border;
        }
        if(index.column() == 4)
        {
            return items[index.row()]->border;
        }
    }
    return QVariant();
}

void PriceValueModel::updateItems(int caption_index)
{
    QSqlQuery query;
    query.prepare("select id, name, a_price, b_price, min_border, border from price_values where position_id = ?");
    query.addBindValue(caption_index);
    query.exec();
    if(query.lastError().type() != QSqlError::NoError)
    {
        qDebug() << query.lastError().text();
        return;
    }
    while(query.next())
    {
        items.push_back(new PriceValueItem(query.value(0).toString(), query.value(1).toString(), query.value(2).toString().replace(",", ".").toDouble(), query.value(3).toString().replace(",", ".").toDouble(), query.value(4).toString().replace(",", ".").toDouble(), query.value(5).toString().replace(",", ".").toDouble()));
    }
}

Qt::ItemFlags PriceValueModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;
    return Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;
}

bool PriceValueModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.isValid() && role == Qt::EditRole)
    {
        auto item = items[index.row()];
        QSqlQuery query;
        if(index.column() == 0)
        {
            query.prepare("update price_values set name = ? where id = ?");
            query.addBindValue(value);
            query.addBindValue(item->id);
            query.exec();
            item->name = value.toString();
        }
        else if(index.column() == 1)
        {
            query.prepare("update price_values set a_price = ? where id = ?");
            query.addBindValue(value);
            query.addBindValue(item->id);
            query.exec();
            item->a_price = value.toDouble();
        }
        else if(index.column() == 2)
        {
            query.prepare("update price_values set b_price = ? where id = ?");
            query.addBindValue(value);
            query.addBindValue(item->id);
            query.exec();
            item->b_price = value.toDouble();
        }
        else if(index.column() == 3)
        {
            query.prepare("update price_values set min_border = ? where id = ?");
            query.addBindValue(value);
            query.addBindValue(item->id);
            query.exec();
            item->min_border = value.toDouble();
        }
        else if(index.column() == 4)
        {
            query.prepare("update price_values set border = ? where id = ?");
            query.addBindValue(value);
            query.addBindValue(item->id);
            query.exec();
            item->border = value.toDouble();
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


PriceValueItem::PriceValueItem(QString id, QString name, double a_price, double b_price, double min_border, double border)
    : id(id)
    , name(name)
    , a_price(a_price)
    , b_price(b_price)
    , min_border(min_border)
    , border(border)
{

}
