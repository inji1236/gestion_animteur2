#include "animateur.h"
#include "ui_animateur.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDebug>

animateur::animateur(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::animateur)
{
    ui->setupUi(this);
}

animateur::~animateur()
{
    delete ui;
}

QSqlQueryModel* animateur::afficherv2()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT g.nom AS animateur, a.nom AS activite FROM GESTION_ANIMATEUR JOIN ACTIVITE a ON g.activite = a.id WHERE g.activite IS NOT NULL ORDER BY g.nom");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Animateur"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Activite"));
    return model;
}

QSqlQueryModel* animateur::afficher()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT IDAN, NOM, PRENOM, GENRE, SPECIALITE, AGE, NBR_HEURE, STATUS FROM GESTION_ANIMATEUR");

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Prénom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Genre"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Spécialité"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Âge"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Nombre d'heures"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Status"));


    return model;
}
