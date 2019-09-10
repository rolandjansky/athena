/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  Header file for class VP1ExpertSettings                             //
//                                                                      //
//  Description: Additional VP1 settings                                //
//                                                                      //
//  Author: Sebastian Andreas Merkt (sebastian.andreas.merkt@cern.ch)   //
//  Initial version: August 2017                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef VP1EXPERTSETTINGS_H
#define VP1EXPERTSETTINGS_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QCheckBox>

class VP1ExpertSettings : public QDialog
{
    Q_OBJECT
public:
    explicit VP1ExpertSettings(QWidget *parent = 0);

public slots:
    void setPluginPath(const QString &path);
    void setFileSelectDir(const QString &path);
    void setScreenshotDir(const QString &path);
    void setAuthLog(const QString &path);

private:
    QString m_pluginPath;
    QString m_fileSelectDir;
    QString m_screenshotDir;
    QString m_authLog;
    QDialogButtonBox* m_buttonBox;
    QTabWidget*       m_tabWidget;
    QPushButton*      m_okButton;

    bool checkBoxState();
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void setExpertSetting(const QString &type, const QString &name);
};






class GeneralTab : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralTab(QWidget *parent = 0);

private slots:
    #if defined BUILDVP1LIGHT
        void setExpertSetting(const QString &type, const QString &name);
    #else
        void setExpertSetting(const QString &name);
    #endif
    void on_browseButton1_clicked();
    void slotScreenshotDirChanged(const QString &path);

signals:
    void signalScreenshotDirChanged(const QString &path);

private:
    QCheckBox*   m_checkboxVerbose;
    QCheckBox*   m_checkboxDebug;
    QCheckBox*   m_checkboxDisallowMultipleChannels;
    QCheckBox*   m_checkboxDisplayMouseClicks;
    QCheckBox*   m_checkboxEnableAskOnClose;
    QCheckBox*   m_checkboxGuidesSphereInsteadOfCoordaxes;
    QCheckBox*   m_checkboxAntiAliasing;
    QPushButton* m_browseButton1;
    QLineEdit*   m_lineEdit1;
};






class AdvancedTab : public QWidget
{
    Q_OBJECT

public:
    explicit AdvancedTab(QWidget *parent = 0);

private slots:
    void on_browseButton1_clicked();
    void on_browseButton2_clicked();
    void on_browseButton3_clicked();
    void slotPluginPathChanged(const QString &path);
    void slotFileSelectDirChanged(const QString &path);
    void slotAuthLogChanged(const QString &path);
    #if defined BUILDVP1LIGHT
        void setExpertSetting(const QString &type, const QString &name);
    #else
        void setExpertSetting(const QString &name);
    #endif

signals:
    void signalPluginPathChanged(const QString &path);
    void signalFileSelectDirChanged(const QString &path);
    void signalAuthLogChanged(const QString &path);

private:
    QPushButton* m_browseButton1;
    QPushButton* m_browseButton2;
    QPushButton* m_browseButton3;
    QCheckBox* m_checkboxEnableInformOnEndOfJob;
    QCheckBox* m_checkboxHardExitAtEnd;
    QCheckBox* m_checkboxDevelShowAllCruiseAndEventControls;
    QLineEdit* m_lineEdit1;
    QLineEdit* m_lineEdit2;
    QLineEdit* m_lineEdit3;
};


#endif // VP1EXPERTSETTINGS_H
