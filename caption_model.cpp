#include "caption_model.h"


#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>


CaptionModel::CaptionModel(int collection_id, QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "Title" << "Summary";
    root_item = new CaptionItem("name", "id");
    setupModelData(collection_id);
}

CaptionModel::~CaptionModel()
{
    delete root_item;
}

QVariant CaptionModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    CaptionItem *item = static_cast<CaptionItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags CaptionModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;

    return Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;
}

QVariant CaptionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return root_item->data(section);

    return QVariant();
}

QModelIndex CaptionModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
        return QModelIndex();

    CaptionItem *parentItem;

    if(!parent.isValid())
        parentItem = root_item;
    else
        parentItem = static_cast<CaptionItem*>(parent.internalPointer());

    CaptionItem *childItem = parentItem->child(row);
    if(childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex CaptionModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
        return QModelIndex();

    CaptionItem *childItem = static_cast<CaptionItem*>(index.internalPointer());
    CaptionItem *parentItem = childItem->parentItem();

    if(parentItem == root_item)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int CaptionModel::rowCount(const QModelIndex &parent) const
{
    CaptionItem *parentItem;
    if(parent.column() > 0)
        return 0;

    if(!parent.isValid())
        parentItem = root_item;
    else
        parentItem = static_cast<CaptionItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int CaptionModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return static_cast<CaptionItem*>(parent.internalPointer())->columnCount();
    else
        return root_item->columnCount();
}

void CaptionModel::addCaption(QModelIndex parent_index, int collection_id)
{
    QSqlQuery query;
    if(!parent_index.isValid())
    {
        query.prepare("insert into captions(parent_id, name, collection_id) values(null, 'Новый заголовок', ?)");
        query.addBindValue(collection_id);
        query.exec();
        auto item = new CaptionItem("новый заголовок", query.lastInsertId().toString(), root_item);
        beginInsertRows(parent_index, this->rowCount(), this->rowCount() + 1);
        this->root_item->appendChild(item);
        endInsertRows();
    }
    else
    {
        query.prepare("insert into captions(parent_id, name, collection_id) values(?, 'Новый заголовок', ?)");
        auto item = static_cast<CaptionItem*>(parent_index.internalPointer());
        query.addBindValue(item->getId());
        query.addBindValue(collection_id);
        query.exec();
        beginInsertRows(parent_index, this->rowCount(parent_index), this->rowCount(parent_index) + 1);
        item->appendChild(new CaptionItem("Новый заголовок", query.lastInsertId().toString(), item));
        endInsertRows();

    }
    if(query.lastError().type() != QSqlError::NoError)
    {
        qDebug() << query.lastError().text();
        return;
    }
    this->dataChanged(this->index(0, 0), this->index(this->rowCount(parent_index), this->columnCount()));
}

void CaptionModel::removeCaption(QModelIndex index)
{
    if(!index.isValid()) return;

    auto item = static_cast<CaptionItem*>(index.internalPointer());
    auto parent = item->parentItem();
    if(item)
    {
        QSqlQuery query;
        query.prepare("delete from captions where id = ?");
        query.addBindValue(item->getId());
        query.exec();
        if(query.lastError().type() != QSqlError::NoError)
        {
            qDebug() << query.lastError().text();
            return;
        }
        this->dataChanged(this->index(0, 0), this->index(this->rowCount(this->index(parent->row(), parent->row())), this->columnCount()));
        return;
        if(item->parentItem())
        {
            item->parentItem()->removeChild(item);
        }
        else
        {
            qDebug() << " item has no parent";
            return;
        }

    }
    else
    {
        return;
    }
    this->dataChanged(this->index(0, 0), this->index(this->rowCount(this->index(parent->row(), parent->row())), this->columnCount()));
}

bool CaptionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.isValid() && role == Qt::EditRole)
    {
        auto item = static_cast<CaptionItem*>(index.internalPointer());
        QSqlQuery query;
        if(index.column() == 0)
        {
            query.prepare("update captions set name = ? where id = ?");
            query.addBindValue(value);
            query.addBindValue(item->getId());
            query.exec();
            item->setName(value.toString());
        }
        else return false;
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

void CaptionModel::setupModelData(int collection_id)
{
    QSqlQuery query;
    query.prepare("select id, name from captions where parent_id is null and collection_id = ?");
    query.addBindValue(collection_id);
    query.exec();
    if(query.lastError().type() != QSqlError::NoError)
    {
        qDebug() << query.lastError().text();
        return;
    }
    QList<CaptionItem*> addedItems;
    while(query.next())
    {
        auto item = new CaptionItem(query.value(1).toString(), query.value(0).toString(), root_item);
        this->root_item->appendChild(item);
        addedItems.push_back(item);
    }
    for(auto it : addedItems)
    {
        it->checkChilds();
    }
}

CaptionItem::CaptionItem(const QString name, const QString id, CaptionItem *parentItem)
    : id(id)
    , name(name)
    , m_parentItem(parentItem)
{

}

CaptionItem::~CaptionItem()
{
    qDeleteAll(m_childItems);
}

void CaptionItem::appendChild(CaptionItem *child)
{
    m_childItems.append(child);
}

void CaptionItem::removeChild(int index)
{
    m_childItems.at(index)->~CaptionItem();
    m_childItems.removeAt(index);
}

void CaptionItem::removeChild(CaptionItem *child)
{
    auto index = m_childItems.indexOf(child);
    if(index < 0) return;
    m_childItems.at(index)->~CaptionItem();
    m_childItems.removeAt(index);
}

CaptionItem *CaptionItem::child(int row)
{
    return m_childItems.value(row);
}

int CaptionItem::childCount() const
{
    return m_childItems.count();
}

int CaptionItem::columnCount() const
{
    return 1;
}

QVariant CaptionItem::data(int column) const
{
    if(column == 0)
        return name;
    return QVariant();
}

int CaptionItem::row() const
{
    if(m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<CaptionItem*>(this));

    return 0;
}

CaptionItem *CaptionItem::parentItem()
{
    return m_parentItem;
}

void CaptionItem::checkChilds()
{
    QSqlQuery query;
    query.prepare("select id, name from captions where parent_id = ?");
    query.addBindValue(id);
    query.exec();
    if(query.lastError().type() != QSqlError::NoError)
    {
        qDebug() << query.lastError().text();
        return;
    }
    QList<CaptionItem*> addedItems;
    while(query.next())
    {
        auto item = new CaptionItem(query.value(1).toString(), query.value(0).toString(), this);
        this->appendChild(item);
        addedItems.push_back(item);
    }
    for(auto it : addedItems)
    {
        it->checkChilds();
    }
}

int CaptionItem::getId()
{
    return id.toInt();
}

void CaptionItem::setName(QString name)
{
    this->name = name;
}


