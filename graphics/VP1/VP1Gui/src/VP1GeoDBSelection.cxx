/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  Source file for class VP1GeoDBSelection                             //
//                                                                      //
//  Description: Dialog for the selection of a geometry database        //
//                                                                      //
//  Author: Sebastian Andreas Merkt (sebastian.andreas.merkt@cern.ch)   //
//  Initial version: August 2017                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "VP1Gui/VP1GeoDBSelection.h"

#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QFileInfo>

#include <QtWidgets>

VP1GeoDBSelection::VP1GeoDBSelection(QWidget *parent) : QDialog(parent)
{
    //Set default dialog size
    int nWidth = 800;
    int nHeight = 220;
    if (parent != NULL)
        setGeometry(parent->x() + parent->width()/2 - nWidth/2,
            parent->y() + parent->height()/2 - nHeight/2,
            nWidth, nHeight);
    else
        resize(nWidth, nHeight);

    //Browse button to select database
    m_browseButton = new QPushButton(tr("&Browse..."), this);
    connect(m_browseButton, &QAbstractButton::clicked, this, &VP1GeoDBSelection::on_browseButton_clicked);

    //Buttonbox to set Open, Cancel buttons
    m_buttonBox = new QDialogButtonBox(this);
    m_openButton = m_buttonBox->addButton(tr("&Open"), QDialogButtonBox::AcceptRole);
    m_cancelButton = m_buttonBox->addButton(tr("&Cancel"),QDialogButtonBox::RejectRole);
    connect(m_openButton, &QPushButton::clicked, this, &VP1GeoDBSelection::loadDatabase);
    connect(m_cancelButton, &QPushButton::clicked, this, &VP1GeoDBSelection::reject);

    //Open QSettings to store path do database
    QSettings settings("ATLAS", "VP1Light");
    QString text;

    //If no db file has been provided via command line argument, the "db/path" value is ""
    if(settings.value("db/path").toString()==""){
        m_openButton->setEnabled(false);
        text = "(*.db)";
    } else {    //If a db file has been provided show it in the combobox
        text = settings.value("db/path").toString();
    }
    //Create the combobox
    m_directoryComboBox = createComboBox(text);

    //Create the main layout
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(new QLabel(tr("Select Geometry Database:")), 0, 0);
    mainLayout->addWidget(m_directoryComboBox, 1, 0, 1, 2);
    mainLayout->addWidget(m_browseButton, 1, 4);
    mainLayout->addWidget(m_buttonBox, 3, 4);

    m_browseButton->setMinimumWidth(200);
    m_buttonBox->setMinimumWidth(200);
    m_browseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_buttonBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

//Method to create the drop down combobox
QComboBox *VP1GeoDBSelection::createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setMinimumWidth(600);
    comboBox->setEditable(false);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return comboBox;
}

//Animate the click
void VP1GeoDBSelection::animateFindClick()
{
    m_openButton->animateClick();
}

//Open the file selection dialog
void VP1GeoDBSelection::on_browseButton_clicked()
{
    m_fileName = QFileDialog::getOpenFileName(this,
        tr("Select Database"), QDir::currentPath(),
        tr("DB Files (*.db)"));

    // Add the selected file to the combobox
    if (!m_fileName.isEmpty()) {
        if (m_directoryComboBox->findText(m_fileName) == -1)
            m_directoryComboBox->addItem(m_fileName);
        m_directoryComboBox->setCurrentIndex(m_directoryComboBox->findText(m_fileName));

        //When valid a database is selected, enable the Open button to confirm
        m_openButton->setEnabled(true);
    }

}

//Check and Save the settings
void VP1GeoDBSelection::loadDatabase(){

    //Save DB path to settings
    QSettings settings("ATLAS", "VP1Light");
    settings.setValue("db/path", m_directoryComboBox->itemText(m_directoryComboBox->currentIndex()));

    //If the selected DB does not exists, go back
    if (!QFileInfo::exists(m_directoryComboBox->itemText(m_directoryComboBox->currentIndex()))){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Virtual Point 1");
        msgBox.setText("Database does not exist. Please choose another database file.");
        msgBox.exec();
        return;
    }
    VP1GeoDBSelection::setResult(1);
    VP1GeoDBSelection::accept();
}
