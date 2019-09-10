/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  Header file for class VP1SelectEvent                                //
//                                                                      //
//  Description: Dialog for the selection of a specific event in a file //
//                                                                      //
//  Author: Sebastian Andreas Merkt (sebastian.andreas.merkt@cern.ch)   //
//  Initial version: August 2018                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef VP1SELECTEVENT_H
#define VP1SELECTEVENT_H

#include <QDialog>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QSpinBox>

class VP1SelectEvent : public QDialog
{
    Q_OBJECT
public:
    explicit VP1SelectEvent(int totEvtNr, int evtNr, QWidget *parent = 0);

public slots:

private:
    void passEvtNr();
    void reject();

    QPushButton* m_cancelButton;
    QPushButton* m_openButton;
    QDialogButtonBox* m_buttonBox;
    QSpinBox* m_spinBox;
};

#endif // VP1SELECTEVENT_H
