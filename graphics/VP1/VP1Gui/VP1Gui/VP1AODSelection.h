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

#ifndef VP1AODSELECTION_H
#define VP1AODSELECTION_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>

class VP1AODSelection : public QDialog
{
    Q_OBJECT
public:
    explicit VP1AODSelection(QWidget *parent = 0);

private slots:
    //Open the file selection dialog
    void on_browseButton_clicked();

    //Check and Save the settings
    void loadDatabase();

public slots:

private:
    QString           m_fileName;
    QComboBox        *m_directoryComboBox;
    QPushButton      *m_browseButton;
    QPushButton      *m_openButton;
    QPushButton      *m_cancelButton;
    QDialogButtonBox *m_buttonBox;

    //Animate the click
    void animateFindClick();

    //Method to create the drop down combobox
    QComboBox *createComboBox(const QString &text = QString());
};

#endif // VP1AODSELECTION_H
