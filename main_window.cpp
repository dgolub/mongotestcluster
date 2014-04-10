/* (C) Copyright 2014, MongoDB, Inc. */

#include "main_window.h"
#include "add_host_dialog.h"
#include "test_cluster_model.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    _model = new TestClusterModel(this);

    _ui.setupUi(this);
    _ui.treeView->setModel(_model);
    _ui.treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    _hostContextMenu = new QMenu(this);
    _startHostAction = _hostContextMenu->addAction("Start");
    _stopHostAction = _hostContextMenu->addAction("Stop");

    _updateTimer = new QTimer(this);
    _updateTimer->start(100);

    connect(_ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(_ui.pushButtonAddHost, SIGNAL(clicked()), this, SLOT(addHost()));
    connect(
        _ui.treeView,
        SIGNAL(customContextMenuRequested(const QPoint&)),
        this,
        SLOT(doHostContextMenu(const QPoint&)));
    connect(_updateTimer, SIGNAL(timeout()), this, SLOT(updateHostStates()));
}

void MainWindow::addHost() {
    AddHostDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }
    _model->addHost(dlg.port(), dlg.dbPath(), dlg.type());
}

void MainWindow::doHostContextMenu(const QPoint& pos) {
    QModelIndexList indexes = _ui.treeView->selectionModel()->selectedRows();
    if (indexes.isEmpty()) {
        return;
    }
    QAction* action = _hostContextMenu->exec(_ui.treeView->mapToGlobal(pos));
    for (QModelIndex index : indexes) {
        if (action == _startHostAction) {
            _model->startHost(index);
        } else if (action == _stopHostAction) {
            _model->stopHost(index);
        }
    }
}

void MainWindow::updateHostStates() {
    _model->updateHostStates();
}
