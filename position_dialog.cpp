#include "position_dialog.h"


CoeffsTableItem::CoeffsTableItem(QTreeWidgetItem *parent)
    : QTreeWidgetItem(parent)
{
    this->setFlags(this->flags() | Qt::ItemIsEditable);
}

QVariant CoeffsTableItem::data(int column, int role) const
{
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if(column == 0)
            return name;
        if(column == 1)
            return p;
        if(column == 2)
            return rd;
        if(column == 3)
            return rp;
    }
    return QTreeWidgetItem::data(column, role);
}

void CoeffsTableItem::setData(int column, int role, const QVariant &value)
{
    if(role == Qt::EditRole)
    {
        if(column == 0)
            name = value.toString();
        if(column == 1)
            p = value.toDouble();
        if(column == 2)
            rd = value.toDouble();
        if(column == 3)
            rp = value.toDouble();
    }
    QTreeWidgetItem::setData(column, role, value);
}




PositionDialog::PositionDialog(int caption_id, PositionItem *item, QWidget *parent)
    : QDialog(parent)
    , item(item)
    , caption_id(caption_id)
{
    this->setWindowTitle("Добавить новую позицию");
    QHBoxLayout *editLay = new QHBoxLayout;
    proveEdit = new QLineEdit(this);
    proveEdit->setPlaceholderText("prove");
    editLay->addWidget(proveEdit);
    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("name");
    editLay->addWidget(nameEdit);
    unitEdit = new QLineEdit(this);
    unitEdit->setPlaceholderText("unit");
    editLay->addWidget(unitEdit);


    QVBoxLayout *lay = new QVBoxLayout;
    lay->addLayout(editLay);
    coeffsTable = new CoeffsTable(this);
    addCoeffBtn = new QPushButton("add");
    removeCoeffBtn = new QPushButton("remove");
    QHBoxLayout *btnsCoeffLay = new QHBoxLayout;
    btnsCoeffLay->addWidget(addCoeffBtn);
    btnsCoeffLay->addWidget(removeCoeffBtn);
    connect(addCoeffBtn, &QPushButton::clicked, this, [ = ]()
    {
        coeffsTable->addItem();
    });
    connect(removeCoeffBtn, &QPushButton::clicked, this, [ = ]()
    {
        auto items = coeffsTable->selectedItems();
        if(items.isEmpty()) return;
        auto item = items.first();
        delete item;
    });
    lay->addWidget(coeffsTable);
    lay->addLayout(btnsCoeffLay);
    QPushButton *saveBtn = new QPushButton("save");
    lay->addWidget(saveBtn);
    if(item)
    {
        this->proveEdit->setText(item->prove);
        this->nameEdit->setText(item->name);
        this->unitEdit->setText(item->unit);
        auto obj = QJsonDocument::fromJson(item->metadata.toUtf8()).object();
        k1 = obj["k1"].toString().replace(",", ".").toDouble();
        k2 = obj["k2"].toString().replace(",", ".").toDouble();
        this->coeffsTable->setJson(obj);
        connect(saveBtn, &QPushButton::clicked, this, [ = ]()
        {
            QSqlQuery query;
            query.prepare("update positions set prove = ?, name = ?, unit = ?, metadata = ? where id = ?");
            query.addBindValue(this->proveEdit->text());
            query.addBindValue(nameEdit->text());
            query.addBindValue(unitEdit->text());
            auto obj = coeffsTable->getJson();
            obj["k1"] = k1;
            obj["k2"] = k2;
            auto doc = QJsonDocument(obj);
            query.addBindValue(QString(doc.toJson(QJsonDocument::Compact)));
            query.addBindValue(item->id);
            query.exec();
            if(query.lastError().type() == QSqlError::NoError)
            {
                QDialog::accept();
                return;
            }
            if(query.lastError().type() != QSqlError::NoError)
            {
                qDebug() << query.lastError().text();
                return;
            }
            QDialog::reject();
        });
    }
    else
    {
        connect(saveBtn, &QPushButton::clicked, this, [ = ]()
        {
            QSqlQuery query;
            query.prepare("insert into positions (caption_id, prove, name, unit, metadata) values(?, ?, ?, ?)");
            query.addBindValue(caption_id);
            query.addBindValue(proveEdit->text());
            query.addBindValue(nameEdit->text());
            query.addBindValue(unitEdit->text());
            query.addBindValue(coeffsTable->getJson());
            query.exec();
            if(query.lastError().type() == QSqlError::NoError)
                QDialog::accept();
            if(query.lastError().type() != QSqlError::NoError)
            {
                qDebug() << query.lastError().text();
                return;
            }
            QDialog::reject();
        });
    }
    this->setLayout(lay);
}


CoeffsTable::CoeffsTable(QWidget *parent)
    : QTreeWidget(parent)
{
    this->setColumnCount(4);
}

void CoeffsTable::addItem()
{
    this->invisibleRootItem()->addChild(new CoeffsTableItem(this->invisibleRootItem()));
}

QJsonObject CoeffsTableItem::getData()
{
    QJsonObject coeffs;
    coeffs["P"] = p;
    coeffs["RD"] = rd;
    coeffs["RP"] = rp;
    return QJsonObject(
    {
        {"name", name},
        {"coeffs", coeffs}
    });
}

void CoeffsTableItem::setData(QJsonObject obj)
{
    qDebug() << obj["coeffs"].toObject();
    qDebug() << obj["coeffs"].toObject()["P"];
    qDebug() << obj["coeffs"].toObject()["P"].toString();
    qDebug() << obj["coeffs"].toObject()["P"].toString().replace(",", ".");
    qDebug() << obj["coeffs"].toObject()["P"].toString().replace(",", ".").toDouble();
    if(obj["coeffs"].toObject()["P"].isDouble())
    {
        p = obj["coeffs"].toObject()["P"].toDouble();
        rd = obj["coeffs"].toObject()["RD"].toDouble();
        rp = obj["coeffs"].toObject()["RP"].toDouble();
    }
    else
    {
        p = obj["coeffs"].toObject()["P"].toString().replace(",", ".").toDouble();
        rd = obj["coeffs"].toObject()["RD"].toString().replace(",", ".").toDouble();
        rp = obj["coeffs"].toObject()["RP"].toString().replace(",", ".").toDouble();
    }
    name = obj["name"].toString();
}

QJsonObject CoeffsTable::getJson()
{
    QJsonObject obj;
    QJsonArray arr;
    for(int i = 0; i < this->invisibleRootItem()->childCount(); i++)
    {
        auto data = static_cast<CoeffsTableItem*>(this->invisibleRootItem()->child(i))->getData();
        arr.push_back(data);
    }
    obj["coeffs"] = arr;
    return obj;
}

void CoeffsTable::setJson(QJsonObject obj)
{
    for(auto it : obj["coeffs"].toArray())
    {
        auto item = new CoeffsTableItem(this->invisibleRootItem());
        item->setData(it.toObject());
        this->invisibleRootItem()->addChild(item);
    }
}
