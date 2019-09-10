/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  Header file for class VP1AODSelection                               //
//                                                                      //
//  Description: Dialog for the selection of a xAOD file                //
//                                                                      //
//  Author: Sebastian Andreas Merkt (sebastian.andreas.merkt@cern.ch)   //
//  Initial version: November 2017                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "VP1Gui/VP1AODSelection.h"

#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QFileInfo>

#include <QtWidgets>

VP1AODSelection::VP1AODSelection(QWidget *parent) : QDialog(parent)
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
    connect(m_browseButton, &QAbstractButton::clicked, this, &VP1AODSelection::on_browseButton_clicked);

    //Buttonbox to set Open, Cancel buttons
    m_buttonBox = new QDialogButtonBox(this);
    m_openButton = m_buttonBox->addButton(tr("&Open"), QDialogButtonBox::AcceptRole);
    m_cancelButton = m_buttonBox->addButton(tr("&Cancel"),QDialogButtonBox::RejectRole);
    connect(m_openButton, &QPushButton::clicked, this, &VP1AODSelection::loadDatabase);
    connect(m_cancelButton, &QPushButton::clicked, this, &VP1AODSelection::reject);

    //Open QSettings to store path do database
    QSettings settings("ATLAS", "VP1Light");
    QString text;

    //If no xAOD file has been provided via command line argument, the "xaod/path" value is ""
    if(settings.value("aod/path").toString()==""){
        m_openButton->setEnabled(false);
        text = "(*)";
    } else {    //If a xAOD file has been provided show it in the combobox
        text = settings.value("aod/path").toString();
    }
    //Create the combobox
    m_directoryComboBox = createComboBox(text);

    //Create the main layout
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(new QLabel(tr("Select AOD file:")), 0, 0);
    mainLayout->addWidget(m_directoryComboBox, 1, 0, 1, 2);
    mainLayout->addWidget(m_browseButton, 1, 4);
    mainLayout->addWidget(m_buttonBox, 3, 4);

    m_browseButton->setMinimumWidth(200);
    m_buttonBox->setMinimumWidth(200);
    m_browseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_buttonBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

//Method to create the drop down combobox
QComboBox *VP1AODSelection::createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setMinimumWidth(600);
    comboBox->setEditable(false);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return comboBox;
}

//Animate the click
void VP1AODSelection::animateFindClick()
{
    m_openButton->animateClick();
}

//Open the file selection dialog
void VP1AODSelection::on_browseButton_clicked()
{
    m_fileName = QFileDialog::getOpenFileName(this,
        tr("Select AOD file"), QDir::currentPath(),
        tr("AOD Files (*)"));

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
void VP1AODSelection::loadDatabase(){

    //Save xAOD path to settings
    QSettings settings("ATLAS", "VP1Light");
    settings.setValue("aod/path", m_directoryComboBox->itemText(m_directoryComboBox->currentIndex()));

    //If the selected xAOD does not exists, go back
    if (!QFileInfo::exists(m_directoryComboBox->itemText(m_directoryComboBox->currentIndex()))){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Virtual Point 1");
        msgBox.setText("AOD file does not exist. Please choose another file.");
        msgBox.exec();
        return;
    }
    VP1AODSelection::setResult(1);
    VP1AODSelection::accept();
}
