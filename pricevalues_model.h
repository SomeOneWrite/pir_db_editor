#ifndef PRICEVALUES_MODEL_H
#define PRICEVALUES_MODEL_H
#include <QAbstractItemModel>

#include <QAbstractTableModel>

struct PriceValueItem
{
    PriceValueItem(QString id,
                   QString name,
                   double a_price,
                   double b_price,
                   double min_border,
                   double border);
    QString id;
    QString name;
    double a_price;
    double b_price;
    double min_border;
    double border;
    QString metadata;
};


class PriceValueModel : public QAbstractTableModel
{
public:
    PriceValueModel();
    void addPriceValue(int position_id);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void updateItems(int caption_index);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
private:
    QVector<PriceValueItem*> items;
};

#endif // PRICEVALUES_MODEL_H
