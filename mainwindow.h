#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QSqlDatabase>
#include "animateur.h"
#include <QString>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlQueryModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_BtnAjoutAnimateur_clicked();
    void on_BtnSupprimerAnimateur_clicked();
    void on_BtnModifierAnimateur_clicked();




    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

    void on_lineEdit_5_textChanged(const QString &arg1);

    void on_pushButton_35_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_15_clicked();

    void on_tableView_2_activated(const QModelIndex &index);
    void on_BtnChoisirImage_clicked();
    void afficherStatsParStatut();

    void on_pushButton_4_clicked();

    QSqlQueryModel* afficherv2();
    void on_BtnSupprimerActivite_clicked();

private:
    Ui::animateur *ui;
    animateur a;   // objet animateur au lieu de club
    int id;
    QString currentImagePath;  // chemin temporaire de l'image sélectionnée
     QImage currentImage;


};

#endif // MAINWINDOW_H
