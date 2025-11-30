#include "mainwindow.h"
#include "ui_animateur.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QFileDialog>
#include <QPixmap>
#include <QSqlError>
#include <QDebug>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QSqlRecord>
#include <QHeaderView>
#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QPixmap>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QDebug>
#include <QDebug>
#include <QBuffer>
#include <QByteArray>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::animateur)
{
    ui->setupUi(this);

    // Valider l'ID : uniquement des entiers entre 1 et 999999
    ui->lineEdit_6->setValidator(new QIntValidator(1, 999999, this));   // ID
    ui->tableViewEvents->setModel(afficherv2());
    // Charger la table des animateurs au démarrage
    ui->tableView_2->setModel(a.afficher());
    // Afficher les statistiques sous forme de chart

       afficherStatsParStatut();

}

MainWindow::~MainWindow()
{
    delete ui;
}



// =====================
// Ajouter un animateur
// =====================

void MainWindow::on_BtnAjoutAnimateur_clicked()
{   // Récupération des données saisies depuis l'interface
    QString id = ui->lineEdit_6->text().trimmed();
    QString nom = ui->lineEdit_7->text().trimmed();
    QString prenom = ui->lineEdit_8->text().trimmed();
    QString genre = ui->radioButton_2->isChecked() ? "H" : "F";
    QString specialite = ui->comboBox_10->currentText();
    QString statut = ui->comboBox_9->currentText();
    int age = ui->spinBox_12->value();
    int nbr_heure = ui->spinBox_2->value();

    // ---- Contrôles ----
    // Vérifications : empêcher les champs vides ou incohérents
    if (id.isEmpty())
    { QMessageBox::warning(this, "Erreur", "Veuillez saisir un ID !");
        return; }
    if (nom.isEmpty() || prenom.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez saisir le nom et le prénom !");
        return; }
    if (specialite == "Sélectionner...") { QMessageBox::warning(this, "Erreur", "Veuillez choisir une spécialité !");
        return; }
    if (statut == "Sélectionner...") { QMessageBox::warning(this, "Erreur", "Veuillez choisir un statut !");
        return; }
    if (age < 18 || age > 65) { QMessageBox::warning(this, "Erreur", "L’âge doit être entre 18 et 65 !");
        return; }
    if (nbr_heure <= 0)
    { QMessageBox::warning(this, "Erreur", "Le nombre d’heures doit être supérieur à 0 !");
        return; }

    // Vérifier si l’ID existe déjà pour éviter les doublons
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM GESTION_ANIMATEUR WHERE IDAN = :id");
    checkQuery.bindValue(":id", id);
    checkQuery.exec();
    checkQuery.next();

    if (checkQuery.value(0).toInt() > 0) {
        QMessageBox::warning(this, "Doublon", "Un animateur avec cet ID existe déjà !");
        return;
    }
    // Conversion de l'image affichée en bytes
    QPixmap pixmap = ui->labelImage->pixmap(Qt::ReturnByValue);
    QByteArray imageData;
    if (!pixmap.isNull()) {
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        pixmap.save(&buffer, "PNG");  // ou "JPG"
    }
   // Préparation de la requête INSERT
    QSqlQuery query;
    query.prepare("INSERT INTO GESTION_ANIMATEUR "
                  "(IDAN, NOM, PRENOM, GENRE, SPECIALITE, AGE, NBR_HEURE, STATUS, IMAGE) "
                  "VALUES (:id, :nom, :prenom, :genre, :specialite, :age, :nbr_heure, :statut, :image)");
    // Binding des valeurs sécurisées
    query.bindValue(":id", id);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":genre", genre);
    query.bindValue(":specialite", specialite);
    query.bindValue(":age", age);
    query.bindValue(":nbr_heure", nbr_heure);
    query.bindValue(":statut", statut);
    query.bindValue(":image", imageData);
    // Exécution de la requête SQL
    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur SQL", query.lastError().text());
        return;
    }

    QMessageBox::information(this, "Succès", "Animateur ajouté !");
    ui->tableView_2->setModel(a.afficher());
    afficherStatsParStatut(); // met à jour le chart automatiquement


    // RESET DES CHAMPS
    ui->lineEdit_6->clear();
    ui->lineEdit_7->clear();
    ui->lineEdit_8->clear();
    ui->radioButton_2->setChecked(false);
    ui->radioButton_3->setChecked(false);
    ui->comboBox_10->setCurrentIndex(0);
    ui->comboBox_9->setCurrentIndex(0);
    ui->spinBox_12->setValue(0);
    ui->spinBox_2->setValue(0);
    // ----- Désactiver exclusivité -----
    ui->radioButton_2->setAutoExclusive(false);
    ui->radioButton_3->setAutoExclusive(false);

    // ----- Décocher -----
    ui->radioButton_2->setChecked(false);
    ui->radioButton_3->setChecked(false);

    // ----- Réactiver exclusivité -----
    ui->radioButton_2->setAutoExclusive(true);
    ui->radioButton_3->setAutoExclusive(true);

    currentImagePath.clear();
    ui->labelImage->clear();

}


// =====================
// Supprimer un animateur
// =====================

void MainWindow::on_BtnSupprimerAnimateur_clicked()
{

    QString idd = QString::number(id);
    qDebug() << "Final Query = " << idd;

    if (QMessageBox::question(this, "Confirmation", "Supprimer cet animateur ?") != QMessageBox::Yes)
        return;

    QSqlQuery q;
    q.prepare("DELETE FROM GESTION_ANIMATEUR WHERE IDAN = :id");
    q.bindValue(":id", idd);

    if (!q.exec()) {
        QMessageBox::critical(this, "Erreur SQL", q.lastError().text());
        return;
    }

    QMessageBox::information(this, "Succès", "Animateur supprimé !");
    ui->tableView_2->setModel(a.afficher());
    afficherStatsParStatut(); // met à jour le chart automatiquement

}

// =====================
// Modifier un animateur
// =====================

void MainWindow::on_BtnModifierAnimateur_clicked()
{   // Conversion de l'ID en QString
    QString idd = QString::number(id);
    // Récupération des valeurs depuis l'interface
    QString nom = ui->lineEdit_7->text().trimmed();
    QString prenom = ui->lineEdit_8->text().trimmed();
    QString genre = ui->radioButton_2->isChecked() ? "H" : (ui->radioButton_3->isChecked() ? "F" : "");
    QString specialite = ui->comboBox_10->currentText();
    QString statut = ui->comboBox_9->currentText();
    int age = ui->spinBox_12->value();
    int nbr_heure = ui->spinBox_2->value();
   // Liste des champs à modifier et paramètres SQL
    QStringList updates;
    QMap<QString, QVariant> params;

    // Vérification de chaque champ avant de l'ajouter à la requête UPDATE
    if (!nom.isEmpty())
    { updates << "NOM=:nom";
        params[":nom"] = nom;
    }
    if (!prenom.isEmpty())
    { updates << "PRENOM=:prenom";
        params[":prenom"] = prenom;
    }
    if (!genre.isEmpty())
    { updates << "GENRE=:genre"
            ; params[":genre"] = genre;
    }
    if (specialite != "Sélectionner...")
    { updates << "SPECIALITE=:spec";
        params[":spec"] = specialite;
    }
    if (statut != "Sélectionner...")
    { updates << "STATUS=:statut";
        params[":statut"] = statut;
    }
    if (age > 0)
    { updates << "AGE=:age";
        params[":age"] = age;
    }
    if (nbr_heure > 0)
    { updates << "NBR_HEURE=:nbr";
        params[":nbr"] = nbr_heure;
    }

    // Image
    // --- IMAGE ---
    if (!currentImagePath.isEmpty()) {  // Si une nouvelle image a été choisie
        QPixmap pixmap(currentImagePath);
        if (!pixmap.isNull()) {
            QByteArray imageData;
            QBuffer buffer(&imageData);
            buffer.open(QIODevice::WriteOnly);

            // Enregistrement de l’image dans le buffer au format PNG
            pixmap.save(&buffer, "PNG");  // ou JPG
            updates << "IMAGE=:img";
            params[":img"] = imageData;
        }
    }
     // Si aucun champ n’a été modifié

    if (updates.isEmpty()) {
        QMessageBox::warning(this, "Alerte", "Aucun champ à modifier !");
        return;
    }
    // Construction dynamique de la requête SQL

    QString queryStr = "UPDATE GESTION_ANIMATEUR SET " + updates.join(", ") + " WHERE IDAN=:id";

    QSqlQuery q;
    q.prepare(queryStr);
    // Association des paramètres dynamiques avec la requête
    for (auto it = params.begin(); it != params.end(); ++it) {
        q.bindValue(it.key(), it.value());
    }
    q.bindValue(":id", idd);
    // Exécution de la requête SQL
    if (!q.exec()) {
        QMessageBox::critical(this, "Erreur SQL", q.lastError().text());
        return;
    }

    QMessageBox::information(this, "Succès", "Modification effectuée !");

    // --- RESET DES CHAMPS ---
    ui->lineEdit_7->clear();
    ui->lineEdit_8->clear();
    ui->comboBox_10->setCurrentIndex(0);
    ui->comboBox_9->setCurrentIndex(0);
    ui->spinBox_12->setValue(0);
    ui->spinBox_2->setValue(0);

    // --- RADIO BUTTONS ---
    ui->radioButton_2->setAutoExclusive(false);
    ui->radioButton_3->setAutoExclusive(false);
    ui->radioButton_2->setChecked(false);
    ui->radioButton_3->setChecked(false);
    ui->radioButton_2->setAutoExclusive(true);
    ui->radioButton_3->setAutoExclusive(true);

    // --- IMAGE ---
    currentImagePath.clear();
    ui->labelImage->clear();


    // --- Rafraîchir le tableau ---
    ui->tableView_2->setModel(a.afficher());
    afficherStatsParStatut(); // met à jour le chart automatiquement

}





// trie genre
void MainWindow::on_pushButton_clicked()
{
    ui->tableView_2->setModel(a.tgenre());
}
// trie par age
void MainWindow::on_pushButton_2_clicked()
{
    ui->tableView_2->setModel(a.tage());
}
// trie par id
void MainWindow::on_pushButton_3_clicked()
{
    ui->tableView_2->setModel(a.afficher());
}
// recherche
void MainWindow::on_lineEdit_5_textChanged(const QString &arg1)
{
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery *query = new QSqlQuery();

    query->prepare("SELECT IDAN, NOM, PRENOM, GENRE, SPECIALITE, AGE, NBR_HEURE, STATUS "
                   "FROM GESTION_ANIMATEUR "
                   "WHERE IDAN LIKE :text OR NOM LIKE :text OR PRENOM LIKE :text");

    query->bindValue(":text", arg1 + "%");// recherche qui commence par arg1
    query->exec();

    if (query->next()) {
        model->setQuery(*query);
        ui->tableView_2->setModel(model);
    } else {
        QMessageBox::critical(this, tr("Recherche"),
                              tr("Aucun résultat trouvé !"),
                              QMessageBox::Cancel);

        ui->lineEdit_5->clear();
    }
}
void MainWindow::on_pushButton_35_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choisir un dossier"),
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty()) return;

    QPdfWriter pdf(dir + "/Animateurs.pdf"); // Crée un PDF
    QPainter painter(&pdf);

    int y = 4000; // Position verticale pour le tableau

    // ---------------- Header ----------------
    QPixmap logo("C:/Users/inji/1.png"); // chemin vers ton logo
    if (!logo.isNull()) {
        painter.drawPixmap(QRect(100, 100, 1500,1500), logo); // dessiner logo
    }

    painter.setPen(QColor(255, 105, 180)); // pink
    painter.setFont(QFont("Times New Roman", 25, QFont::Bold)); // titre en gras
    painter.drawText(3000, 1400, "Liste des Animateurs");

    painter.setPen(Qt::black);
    painter.setFont(QFont("Times New Roman", 15));

    // ---------------- Cadres du tableau ----------------
    painter.drawRect(100, 100, 9400, 2500);   // cadre header
    painter.drawRect(100, 3000, 9400, 500);   // cadre colonnes

    // Noms des colonnes
    painter.setFont(QFont("Times New Roman", 9, QFont::Bold));
    painter.drawText(300, 3300, "ID");
    painter.drawText(1200, 3300, "Nom");
    painter.drawText(2200, 3300, "Prénom");
    painter.drawText(3200, 3300, "Genre");
    painter.drawText(4200, 3300, "Spécialité");
    painter.drawText(5500, 3300, "Âge");
    painter.drawText(6300, 3300, "Nbr Heure");
    painter.drawText(7400, 3300, "Status");
    painter.drawText(8400, 3300, "Salaire");

    painter.drawRect(100, 3000, 9400, 10700); // cadre pour les données

    // ---------------- Récupérer les données ----------------
    QSqlQuery query;
    query.prepare("SELECT IDAN, NOM, PRENOM, GENRE, SPECIALITE, AGE, NBR_HEURE, STATUS FROM GESTION_ANIMATEUR");
    query.exec();

    while (query.next()) {
        int nbrHeures = query.value(6).toInt();
        int salaire = nbrHeures * 30; // calculer salaire

        painter.drawText(300, y, query.value(0).toString());
        painter.drawText(1200, y, query.value(1).toString());
        painter.drawText(2200, y, query.value(2).toString());
        painter.drawText(3200, y, query.value(3).toString());
        painter.drawText(4200, y, query.value(4).toString());
        painter.drawText(5500, y, query.value(5).toString());
        painter.drawText(6300, y, query.value(6).toString());
        painter.drawText(7400, y, query.value(7).toString());
        painter.drawText(8400, y, QString::number(salaire) + " DT"); // afficher le salaire

        y += 500;
    }

    painter.end();

    int reponse = QMessageBox::question(this, "Générer PDF",
                                        "PDF Animateurs enregistré. Voulez-vous l’ouvrir ?",
                                        QMessageBox::Yes | QMessageBox::No);

    if (reponse == QMessageBox::Yes) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(dir + "/Animateurs.pdf"));
    }
}


void MainWindow::on_pushButton_14_clicked()
{
    ui->tableView_2->setModel(a.afficher());
}

void MainWindow::on_pushButton_15_clicked()
{

    int f1 = ui->f1->currentIndex();
    int f3 = ui->f3->currentIndex();
    QString f2 = ui->f2->currentText();
    QString f4 = ui->f4->currentText();
    QString queryString =
        "SELECT IDAN, NOM, PRENOM, GENRE, SPECIALITE, AGE, NBR_HEURE, STATUS  "
        "FROM GESTION_ANIMATEUR WHERE 1=1 ";

    // GENRE filter
    if (f1 == 1)       queryString += "AND GENRE = 'H' ";
    else if (f1 == 2)  queryString += "AND GENRE = 'F' ";

    // AGE filter
    if (f3 == 1)       queryString += "AND AGE > 30 ";
    else if (f3 == 2)  queryString += "AND AGE < 30 ";


    if ( f2 != "specialite")
        queryString += "AND SPECIALITE = '" + f2 + "' ";


    if ( f4 != "satuts")
        queryString += "AND STATUS = '" + f4 + "' ";


    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(queryString);

    ui->tableView_2->setModel(model);
}
void MainWindow::on_BtnChoisirImage_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Choisir une image"), "", tr("Images (*.png *.jpg *.bmp)"));

    if (!fileName.isEmpty()) {
        QPixmap pix(fileName);
        ui->labelImage->setPixmap(pix.scaled(ui->labelImage->size(),
                                                    Qt::KeepAspectRatio,
                                                    Qt::SmoothTransformation));
        currentImagePath = fileName; // stocker le chemin
    }
}
void MainWindow::afficherStatsParStatut()
{
    // Exécuter la requête SQL qui compte combien d'animateurs il y a par statut
    QSqlQuery query("SELECT STATUS, COUNT(*) FROM GESTION_ANIMATEUR GROUP BY STATUS");

    // Calculer le total pour le pourcentage
    int total = 0;
    while (query.next()) {
        total += query.value(1).toInt();
    }

    // Ré-exécuter la requête pour parcourir les résultats depuis le début
    query.first();
    query.previous();

    // Série pour le diagramme en secteurs (camembert)
    QPieSeries *series = new QPieSeries();

    // Définir les couleurs dans l'ordre : orange, rose, bleu ciel
    QList<QColor> colors = { QColor(255, 165, 0), QColor(255, 105, 180), QColor(135, 206, 235) };
    int colorIndex = 0;

    while (query.next()) {
        QString statut = query.value(0).toString();
        int count = query.value(1).toInt();

        // Ajouter une tranche au graphique
        QPieSlice *slice = series->append(statut, count);

        // Calculer le pourcentage
        double percent = (double)count / total * 100;

        // Afficher le label avec le nom du statut et le pourcentage
        slice->setLabel(QString("%1 : %2%").arg(statut).arg(QString::number(percent, 'f', 1)));

        // Appliquer la couleur selon l'ordre
        if (colorIndex < colors.size()) {
            slice->setBrush(colors[colorIndex]);
        } else {
            slice->setBrush(Qt::gray);
        }

        slice->setLabelVisible(true);
        colorIndex++;
    }

    // Créer le graphique
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Statistiques selon le statut");
    chart->setTitleFont(QFont("Arial", 12, QFont::Bold));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Légende visible et en dessous
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // Créer la vue du graphique
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Supprimer ancien layout s'il existe
    QLayout *oldLayout = ui->frame_10->layout();
    if (oldLayout) {
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete oldLayout;
    }

    // Créer et installer le nouveau layout
    QVBoxLayout *layout = new QVBoxLayout(ui->frame_10);
    layout->addWidget(chartView);
    ui->frame_10->setLayout(layout);
}












void MainWindow::on_tableView_2_activated(const QModelIndex &index)
{
    // Toujours récupérer l'ID de la ligne, même si on clique ailleurs
    int idSelected = ui->tableView_2->model()->data(index.siblingAtColumn(0)).toInt();

    this->id = idSelected;  // Stocker l'ID pour modifier/supprimer
    qDebug() << "ID sélectionné =" << idSelected;

    // --- Calcul du salaire ---
    int row = index.row();
    QAbstractItemModel *model = ui->tableView_2->model();
    int nbrHeures = model->data(model->index(row, 6)).toInt();
    int salaire = nbrHeures * 30;
    ui->label_salaire->setText(QString(" %1 DT").arg(salaire));

    // --- Charger l'image depuis la BD ---
    QSqlQuery query;
    query.prepare("SELECT IMAGE FROM GESTION_ANIMATEUR WHERE IDAN=:id");
    query.bindValue(":id", idSelected);

    if (query.exec() && query.next()) {
        QByteArray imageData = query.value(0).toByteArray();
        QPixmap pix;
        pix.loadFromData(imageData);
        ui->label->setPixmap(
            pix.scaled(ui->label->size(),
                       Qt::KeepAspectRatio,
                       Qt::SmoothTransformation)
            );
    }
}

QSqlQueryModel* MainWindow::afficherv2()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT g.nom AS animateur, a.nom AS activite , a.date_debut, a.date_fin FROM GESTION_ANIMATEUR g JOIN ACTIVITE a ON g.activite = a.id WHERE g.activite IS NOT NULL ORDER BY g.nom");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Animateur"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Activite"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Date debut"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Date fin"));
    return model;
}



void MainWindow::on_pushButton_4_clicked()
{
    QString idd = QString::number(id);  // ID de l'animateur sélectionné

    // --- Vérifier que l'animateur est ACTIF ---
    QSqlQuery checkStatus;
    checkStatus.prepare("SELECT STATUS FROM GESTION_ANIMATEUR WHERE IDAN = :id");
    checkStatus.bindValue(":id", idd);

    if (!checkStatus.exec()) {
        QMessageBox::critical(this, "Erreur SQL", "Impossible de vérifier le statut de l'animateur !");
        return;
    }

    if (checkStatus.next()) {
        QString statut = checkStatus.value(0).toString();
        if (statut != "Actif") {
            QMessageBox::warning(this, "Animateur inactif",
                                 "⚠️ Cet animateur n'est pas actif.\nVous ne pouvez pas lui affecter une activité.");
            return;
        }
    } else {
        QMessageBox::critical(this, "Erreur", "Animateur introuvable !");
        return;
    }

    // --- Récupérer nom de l'animateur ---
    QString animateurName;
    QSqlQuery animQuery;
    animQuery.prepare("SELECT nom FROM GESTION_ANIMATEUR WHERE IDAN = :idd");
    animQuery.bindValue(":idd", idd);

    if (!animQuery.exec()) {
        qDebug() << "Failed to fetch animator name:" << animQuery.lastError();
        animateurName = "Unknown";
    } else if (animQuery.next()) {
        animateurName = animQuery.value("nom").toString();
    }

    // --- Récupérer la spécialité de l'animateur ---
    QString specialite;
    QSqlQuery specQuery;
    specQuery.prepare("SELECT SPECIALITE FROM GESTION_ANIMATEUR WHERE IDAN = :idd");
    specQuery.bindValue(":idd", idd);

    if (!specQuery.exec()) {
        qDebug() << "Failed to fetch animator specialty:" << specQuery.lastError();
    } else if (specQuery.next()) {
        specialite = specQuery.value("specialite").toString();
    }

    // --- Fenêtre de dialogue d'affectation ---
    QDialog dialog(this);
    dialog.setWindowTitle("Choisir une activité");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *label = new QLabel(
        QString("Choisissez une activité pour l'animateur : %1\nSpécialité : %2")
            .arg(animateurName)
            .arg(specialite),
        &dialog
        );
    layout->addWidget(label);

    // --- QComboBox filtré par spécialité ---
    QComboBox *combo = new QComboBox(&dialog);

    QSqlQuery query;
    query.prepare(R"(
        SELECT id, nom
        FROM ACTIVITE
        WHERE TYPE_ACT = :spec
          AND id NOT IN (
                SELECT activite
                FROM GESTION_ANIMATEUR
                WHERE activite IS NOT NULL
          )  AND STATUS = 'Actif'
    )");

    query.bindValue(":spec", specialite);

    if (!query.exec()) {
        qDebug() << "Query failed:" << query.lastError();
    }

    while (query.next()) {
        combo->addItem(query.value("nom").toString(), query.value("id").toString());
    }

    layout->addWidget(combo);

    // --- Bouton sauvegarder ---
    QPushButton *saveBtn = new QPushButton("Save", &dialog);
    layout->addWidget(saveBtn);

    QObject::connect(saveBtn, &QPushButton::clicked, [&]() {
        QString selectedId = combo->currentData().toString();

        if (selectedId.isEmpty()) {
            QMessageBox::warning(&dialog, "Aucune activité",
                                 "Aucune activité disponible pour cette spécialité !");
            return;
        }

        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE GESTION_ANIMATEUR SET activite = :act WHERE IDAN = :idd");
        updateQuery.bindValue(":act", selectedId);
        updateQuery.bindValue(":idd", idd);

        if (!updateQuery.exec()) {
            qDebug() << "Update failed:" << updateQuery.lastError();
            QMessageBox::critical(&dialog, "Erreur", "Impossible d'affecter l'activité !");
            return;
        }

        // --- Rafraîchir le tableau des événements ---
        QSqlQueryModel *model = new QSqlQueryModel(this);
        model->setQuery(R"(
            SELECT g.nom AS animateur, a.nom AS activite, a.date_debut, a.date_fin
            FROM GESTION_ANIMATEUR g
            JOIN ACTIVITE a ON g.activite = a.id
            WHERE g.activite IS NOT NULL
            ORDER BY g.nom
        )");

        ui->tableViewEvents->setModel(model);

        dialog.accept();
    });

    dialog.exec();
}
void MainWindow::on_BtnSupprimerActivite_clicked()
{
    // Vérifier si une ligne est sélectionnée dans tableViewEvents
    QModelIndex index = ui->tableViewEvents->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "Sélection requise", "Veuillez sélectionner un événement à supprimer.");
        return;
    }

    // Récupérer le nom de l'animateur et de l'activité depuis la ligne sélectionnée
    QString animateur = ui->tableViewEvents->model()->data(ui->tableViewEvents->model()->index(index.row(), 0)).toString();
    QString activite = ui->tableViewEvents->model()->data(ui->tableViewEvents->model()->index(index.row(), 1)).toString();

    // Confirmation
    if (QMessageBox::question(this, "Confirmation",
                              QString("Voulez-vous désaffecter l'activité '%1' de l'animateur '%2' ?")
                                  .arg(activite).arg(animateur)) != QMessageBox::Yes)
        return;

    // Mettre activite à NULL dans la table GESTION_ANIMATEUR
    QSqlQuery query;
    query.prepare("UPDATE GESTION_ANIMATEUR SET activite = NULL WHERE IDAN = (SELECT IDAN FROM GESTION_ANIMATEUR WHERE NOM = :nom) ");
    query.bindValue(":nom", animateur);

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur SQL", query.lastError().text());
        return;
    }

    // Rafraîchir le tableau
    ui->tableViewEvents->setModel(afficherv2());

    QMessageBox::information(this, "Succès", "Activité désaffectée !");
}
