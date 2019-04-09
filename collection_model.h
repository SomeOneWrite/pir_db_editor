#ifndef COLLECTION_MODEL_H
#define COLLECTION_MODEL_H

#include <QAbstractTableModel>

struct CollectionItem
{
    CollectionItem(QString id, QString name);
    QString id;
    QString name;
};


class CollectionModel : public QAbstractTableModel
{
public:
    CollectionModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int getId(int row);
    void updateItems();
    void addCollection();
    void updateItem(int index);
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    int last_inserted_id;
private:
    QVector<CollectionItem*> items;
};

#endif // COLLECTION_MODEL_H
