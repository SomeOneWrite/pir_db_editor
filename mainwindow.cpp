#include "mainwindow.h"
#include "collection_model.h"
#include "caption_model.h"
#include "positions_model.h"
#include "pricevalues_model.h"
#include "position_dialog.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("sbz.sqlite");
    database.open();
    database.transaction();
    QVBoxLayout *layout = new QVBoxLayout;

    QHBoxLayout *tree_layout = new QHBoxLayout;
    layout->addLayout(tree_layout);
    QTableView *collectionView = new QTableView();
    QTreeView *captionView = new QTreeView();
    QTableView *positionView = new QTableView();
    tree_layout->addWidget(collectionView);
    tree_layout->addWidget(captionView);
    tree_layout->addWidget(positionView);

    collectionView->setFixedWidth(300);
    captionView->setFixedWidth(1200);

    auto collectionAddBtn = new QPushButton("Добавить сборник");
    auto collectionRemoveBtn = new QPushButton("Удалить сборник");

    auto captionAddBtn = new QPushButton("Добавить заголовок");
    auto captionRemoveBtn = new QPushButton("Удалить заголовок");

    auto positionAddBtn = new QPushButton("Добавить позицию");
    auto positionRemoveBtn = new QPushButton("Удалить позицию");

    auto priceValueAddBtn = new QPushButton("Добавить элемент в позицию");
    auto priceValueRemoveBtn = new QPushButton("Удалить элемент из позиции");



    QTableView *priceValuesView = new QTableView();
    layout->addWidget(priceValuesView);

    auto collectionModel = new CollectionModel();
    collectionView->setModel(collectionModel);
    connect(collectionAddBtn, &QPushButton::clicked, this, [ = ]()
    {
        collectionModel->addCollection();
        collectionView->update();
    });
    connect(captionAddBtn, &QPushButton::clicked, this, [ = ]()
    {
        auto index = captionView->selectionModel()->currentIndex();
        currentCaptionModel->addCaption(index, currentCollectionId);

        captionView->update();
    });
    connect(positionAddBtn, &QPushButton::clicked, this, [ = ]()
    {
        auto dialog = new PositionDialog(currentCaptionId, nullptr, this);
        dialog->exec();/*
        currentPositionsModel->addPosition(currentCaptionId);*/
        positionView->update();
    });
    connect(priceValueAddBtn, &QPushButton::clicked, this, [ = ]()
    {
        collectionModel->addCollection();
    });
    connect(collectionView, &QTableView::clicked, this, [ = ](const QModelIndex & index)
    {
        auto id = collectionModel->getId(index.row());
        currentCaptionModel = new CaptionModel(id);
        captionView->setModel(currentCaptionModel);
        captionView->resizeColumnToContents(0);
        currentCollectionId = id;
    });

    connect(captionView, &QTreeView::clicked, this, [ = ](const QModelIndex & index)
    {
        auto item = static_cast<CaptionItem*>(index.internalPointer());
        if(item)
        {
            currentPositionsModel = new PositionsModel();
            currentPositionsModel->updateItems(item->getId());
            positionView->setModel(currentPositionsModel);
            positionView->resizeColumnToContents(1);
            currentCaptionId = item->getId();
        }
    });

    connect(positionView, &QTreeView::clicked, this, [ = ](const QModelIndex & index)
    {
        if(currentCaptionModel)
        {
            auto id = currentPositionsModel->getId(index.row());
            auto priceValueModel = new PriceValueModel();
            priceValueModel->updateItems(id);
            priceValuesView->setModel(priceValueModel);
            currentPositionId = id;
        }
    });

    connect(positionView, &QTreeView::doubleClicked, this, [ = ](const QModelIndex & index)
    {
        if(currentCaptionModel)
        {
            auto index = positionView->selectionModel()->currentIndex();
            if(index.isValid())
            {
                auto item = currentPositionsModel->getItem(index);
                auto dialog = new PositionDialog(currentCaptionId, item, this);
                dialog->exec();
            }
            return;/*
            auto id = currentPositionsModel->getId(index.row());
            auto priceValueModel = new PriceValueModel();
            priceValueModel->updateItems(id);
            priceValuesView->setModel(priceValueModel);
            currentPositionId = id;*/
        }
    });

    QHBoxLayout *btnsLay = new QHBoxLayout;
    btnsLay->addWidget(collectionAddBtn);
    btnsLay->addWidget(captionAddBtn);
    btnsLay->addWidget(positionAddBtn);
    layout->addLayout(btnsLay);
    setLayout(layout);
}

MainWindow::~MainWindow()
{
    database.commit();
}
