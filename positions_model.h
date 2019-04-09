#ifndef POSITIONS_MODEL_H
#define POSITIONS_MODEL_H

#include <QAbstractTableModel>

struct PositionItem
{
    PositionItem(QString id,
                 QString prove,
                 QString name,
                 QString unit,
                 QString metadata);
    QString id;
    QString prove;
    QString name;
    QString unit;
    QString metadata;
};


class PositionsModel : public QAbstractTableModel
{
public:
    PositionsModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void updateItems(int caption_index);
    int getId(int row);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    void addPosition(int captionIndex);
    PositionItem* getItem(QModelIndex index);
private:
    QVector<PositionItem*> items;
};

#endif // POSITIONS_MODEL_H
