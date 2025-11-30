#ifndef ANIMATEUR_H
#define ANIMATEUR_H

#include <QMainWindow>
#include <QSqlQueryModel>

QT_BEGIN_NAMESPACE
namespace Ui { class animateur; }
QT_END_NAMESPACE

class animateur : public QMainWindow
{
    Q_OBJECT

public:
    explicit animateur(QWidget *parent = nullptr);
    ~animateur();

    QSqlQueryModel* afficher();    // fonction pour afficher les animateurs
    QSqlQueryModel* afficherv2();    // fonction pour afficher les animateurs
    QSqlQueryModel* tage();
    QSqlQueryModel* tgenre();



private:
    Ui::animateur *ui;
};

#endif // ANIMATEUR_H
