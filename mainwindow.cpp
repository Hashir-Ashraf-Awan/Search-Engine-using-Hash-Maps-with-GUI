#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <QStringListModel>
#include <QDesktopServices>
#include "searchengine.cpp"
#include "second.h"
#include <algorithm>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->listWidget->clear();

    QString query = ui->lineEdit->text();

    if (!query.isEmpty())
    {
        backStack.push_back(query);
        forwardStack.clear();
    }
    if (home.empty()){
        home.push_back(query);
    }
    SearchEngine engine;
    engine.Clear();

    engine.ReadFolderandParse("E:/DSA/DS_Project/Documents");

    engine.ParseUserQuery(query.toStdString());

    engine.SearchQuery(query.toStdString());

    engine.booleanLogic();

    vector<unordered_map<string, int>> results = engine.final();
    QString noResultsMessage = "No Documents found";

    if (!results.empty())
    {
        for (auto resultSet : results)
        {
            vector<pair<string, int>> sortedResultSet(resultSet.begin(), resultSet.end());

            sort(sortedResultSet.begin(), sortedResultSet.end(), [](auto a, auto b) {
                return a.second > b.second;
            });

            for (auto doc : sortedResultSet)
            {
                ui->listWidget->addItem(QString::fromStdString(doc.first) + "\t\t\t(Relevance = " + QString::number(doc.second) + " )");
            }
        }
    }
    else
    {
        ui->listWidget->addItem(noResultsMessage);
    }
    if (ui->listWidget->count() == 0)
    {
        ui->listWidget->addItem(noResultsMessage);
    }
}

void MainWindow::on_listWidget_doubleClicked(const QModelIndex &index)
{
    QListWidgetItem *item = ui->listWidget->item(index.row());
    if (item)
    {
        QString filePath = item->text();
        int index = filePath.indexOf(".txt");
        if (index != -1)
        {
            filePath.truncate(index + 4);
            FileContentDialog dialog(filePath, this);
            dialog.exec();
        }
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    if (backStack.size() > 1)
    {
        forwardStack.push_back(backStack.back());
        backStack.pop_back();
        QString previousQuery = backStack.back();
        ui->lineEdit->setText(previousQuery);
    }


}

void MainWindow::on_pushButton_2_clicked()
{
    if (!forwardStack.empty())
    {
        backStack.push_back(forwardStack.back());

        QString nextQuery = forwardStack.back();
        ui->lineEdit->setText(nextQuery);
        forwardStack.pop_back();

    }

}

void MainWindow::on_pushButton_4_clicked()
{
    ui->lineEdit->setText(home.at(0));
    on_pushButton_clicked();
}

