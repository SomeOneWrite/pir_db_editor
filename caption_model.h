#ifndef CAPTION_MODEL_H
#define CAPTION_MODEL_H

#include <QAbstractItemModel>

class CaptionItem
{
public:
    explicit CaptionItem(const QString name, const QString id, CaptionItem *parentItem = nullptr);
    ~CaptionItem();

    void appendChild(CaptionItem *child);
    void removeChild(int index);
    void removeChild(CaptionItem *child);
    CaptionItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    CaptionItem *parentItem();

    void checkChilds();
    int getId();

    void setName(QString name);
private:
    QList<CaptionItem*> m_childItems;
    QString id;
    QString name;
    QList<QVariant> m_itemData;
    CaptionItem *m_parentItem;
};

class CaptionModel : public QAbstractItemModel
{
public:
    explicit CaptionModel(int collection_id, QObject *parent = nullptr);
    virtual ~CaptionModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    void addCaption(QModelIndex parent_index, int collection_id);
    void removeCaption(QModelIndex index);
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
private:
    void setupModelData(int collection_id);
    CaptionItem *root_item;
};

#endif // CAPTION_MODEL_H
