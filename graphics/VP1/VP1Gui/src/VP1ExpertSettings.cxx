/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  Source file for class VP1ExpertSettings                             //
//                                                                      //
//  Description: Additional VP1 settings                                //
//                                                                      //
//  Author: Sebastian Andreas Merkt (sebastian.andreas.merkt@cern.ch)   //
//  Initial version: August 2017                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "VP1Gui/VP1ExpertSettings.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Base/VP1Msg.h"

#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QDir>
#include <QKeyEvent>

#include <QtWidgets>


template <typename... Args> inline void unused(Args&&...) {} // to declare variables as 'unused'



VP1ExpertSettings::VP1ExpertSettings(QWidget *parent) : QDialog(parent)
{

    m_tabWidget = new QTabWidget;
    GeneralTab *generalTab = new GeneralTab();
    AdvancedTab *advancedTab = new AdvancedTab();
    m_tabWidget->addTab(generalTab, tr("Settings"));
    connect(generalTab, &GeneralTab::signalScreenshotDirChanged, this, &VP1ExpertSettings::setScreenshotDir);

    #if defined BUILDVP1LIGHT
        QString pluginPath=VP1QtUtils::expertSettingValue("expert","ExpertSettings/VP1PLUGINPATH");
        QString fileSelectDir=VP1QtUtils::expertSettingValue("expert","ExpertSettings/VP1_FILESELECTDIR");
        QString screenshotDir=VP1QtUtils::expertSettingValue("general","ExpertSettings/VP1_SCREENSHOTS_DIR");
        QString authLog=VP1QtUtils::expertSettingValue("expert","ExpertSettings/VP1_AUTH_ENABLELOG");

        if(VP1QtUtils::expertSettingIsOn("general", "ExpertSettings/enableExpertSettings")){
            m_tabWidget->addTab(advancedTab, tr("Advanced"));
            connect(advancedTab, &AdvancedTab::signalPluginPathChanged, this, &VP1ExpertSettings::setPluginPath);
            connect(advancedTab, &AdvancedTab::signalFileSelectDirChanged, this, &VP1ExpertSettings::setFileSelectDir);
            connect(advancedTab, &AdvancedTab::signalAuthLogChanged, this, &VP1ExpertSettings::setAuthLog);
        }
    #else
        QString pluginPath=VP1QtUtils::environmentVariableValue("VP1PLUGINPATH");
        QString fileSelectDir=VP1QtUtils::environmentVariableValue("VP1_FILESELECTDIR");
        QString screenshotDir=VP1QtUtils::environmentVariableValue("VP1_SCREENSHOTS_DIR");
        QString authLog=VP1QtUtils::environmentVariableValue("VP1_AUTH_ENABLELOG");

        // Always enable Advanced settings for VP1
        m_tabWidget->addTab(advancedTab, tr("Advanced"));
        connect(advancedTab, &AdvancedTab::signalPluginPathChanged, this, &VP1ExpertSettings::setPluginPath);
        connect(advancedTab, &AdvancedTab::signalFileSelectDirChanged, this, &VP1ExpertSettings::setFileSelectDir);
        connect(advancedTab, &AdvancedTab::signalAuthLogChanged, this, &VP1ExpertSettings::setAuthLog);
    #endif

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::close);
    // connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_tabWidget);
    mainLayout->addWidget(m_buttonBox);
    setLayout(mainLayout);

    this->setWindowTitle(tr("VP1 Settings"));


}

void VP1ExpertSettings::closeEvent(QCloseEvent *event)
{
    //Update settings
    #if defined BUILDVP1LIGHT
        VP1Msg::enableMsg("general", "ExpertSettings/VP1_VERBOSE_OUTPUT");
        VP1Msg::enableMsg("general", "ExpertSettings/VP1_DEBUG_OUTPUT");
    #else
        VP1Msg::enableMsg("VP1_VERBOSE_OUTPUT");
        VP1Msg::enableMsg("VP1_DEBUG_OUTPUT");
    #endif

    // if(QDir(pluginPath).exists()||pluginPath==""){
        #if defined BUILDVP1LIGHT
            VP1QtUtils::setExpertSetting("expert","ExpertSettings/VP1PLUGINPATH", m_pluginPath);
            VP1QtUtils::setExpertSetting("expert","ExpertSettings/VP1_FILESELECTDIR", m_fileSelectDir);
            VP1QtUtils::setExpertSetting("expert","ExpertSettings/VP1_AUTH_ENABLELOG", m_authLog);
            VP1QtUtils::setExpertSetting("general","ExpertSettings/VP1_SCREENSHOTS_DIR", m_screenshotDir);
        #else
            VP1QtUtils::setEnvironmentVariable("VP1PLUGINPATH", m_pluginPath);
            VP1QtUtils::setEnvironmentVariable("VP1_FILESELECTDIR", m_fileSelectDir);
            VP1QtUtils::setEnvironmentVariable("VP1_AUTH_ENABLELOG", m_authLog);
            VP1QtUtils::setEnvironmentVariable("VP1_SCREENSHOTS_DIR", m_screenshotDir);
        #endif

    #if defined BUILDVP1LIGHT
        QMessageBox msgBox;
        QCheckBox *cb = new QCheckBox("Don't ask again.");
        msgBox.setWindowTitle("Settings");
        msgBox.setText("Some settings may require a restart of VP1.");
        msgBox.setIcon(QMessageBox::Icon::Information);
        msgBox.setCheckBox(cb);

        if(VP1QtUtils::expertSettingIsOn("general","Configuration/enableCloseSettingsReminder")){
                cb->setChecked(true);
            }
        connect(cb, &QCheckBox::toggled, [this](){VP1ExpertSettings::setExpertSetting("general","Configuration/enableCloseSettingsReminder");});

        if(cb->checkState()==Qt::Unchecked){
            msgBox.exec();
        }

        event->accept();
    #else
        unused(event);
    #endif


}

void VP1ExpertSettings::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Escape)
        VP1ExpertSettings::close();
}

void VP1ExpertSettings::setPluginPath(const QString &path){
    m_pluginPath = path;
}

void VP1ExpertSettings::setFileSelectDir(const QString &path){
    m_fileSelectDir = path;
}

void VP1ExpertSettings::setScreenshotDir(const QString &path){
    m_screenshotDir = path;
}

void VP1ExpertSettings::setAuthLog(const QString &path){
    m_authLog = path;
}

void VP1ExpertSettings::setExpertSetting(const QString &type, const QString &name){
    if(!VP1QtUtils::expertSettingIsOn(type, name)){
        VP1QtUtils::setExpertSetting(type, name, "1");
    } else {
        VP1QtUtils::setExpertSetting(type, name, "");
    }
}

GeneralTab::GeneralTab(QWidget *parent)
    : QWidget(parent)
{

    QGroupBox *generalGroup = new QGroupBox(tr("General settings"));

    m_checkboxVerbose = new QCheckBox("&Verbose output", this);
    m_checkboxVerbose->setToolTip( QApplication::translate(__FUNCTION__, "    <html>\n"
                                                  "    <div style=\"width: 300px;\">Enable A LOT more verbose output to stdout from VP1. It is very useful if you run with this before sending us logfiles for bug reports.</div>"
                                                  "    </html>", 0
                                 ));
    #if defined BUILDVP1LIGHT
        if(VP1QtUtils::expertSettingIsOn("general","ExpertSettings/VP1_VERBOSE_OUTPUT")){
            m_checkboxVerbose->setChecked(true);
        }
        connect(m_checkboxVerbose, &QCheckBox::toggled, [this](){GeneralTab::setExpertSetting("general","ExpertSettings/VP1_VERBOSE_OUTPUT");});
    #else
        if(VP1QtUtils::environmentVariableIsOn("VP1_VERBOSE_OUTPUT")){
            m_checkboxVerbose->setChecked(true);
        }
        connect(m_checkboxVerbose, &QCheckBox::toggled, [this](){GeneralTab::setExpertSetting("VP1_VERBOSE_OUTPUT");});
    #endif

    m_checkboxDebug = new QCheckBox("&Debug output", this);
    m_checkboxDebug->setToolTip( QApplication::translate(__FUNCTION__, "    <html>\n"
                                                  "    <div style=\"width: 300px;\">Enable A LOT more debug output to stdout from VP1. It is very useful if you run with this before sending us logfiles for bug reports.</div>"
                                                  "    </html>", 0
                                 ));
    #if defined BUILDVP1LIGHT
        if(VP1QtUtils::expertSettingIsOn("general","ExpertSettings/VP1_DEBUG_OUTPUT")){
            m_checkboxDebug->setChecked(true);
        }
        connect(m_checkboxDebug, &QCheckBox::toggled, [this](){GeneralTab::setExpertSetting("general","ExpertSettings/VP1_DEBUG_OUTPUT");});
    #else
        if(VP1QtUtils::environmentVariableIsOn("VP1_DEBUG_OUTPUT")){
            m_checkboxDebug->setChecked(true);
        }
        connect(m_checkboxDebug, &QCheckBox::toggled, [this](){GeneralTab::setExpertSetting("VP1_DEBUG_OUTPUT");});
    #endif

    m_checkboxDisallowMultipleChannels = new QCheckBox("D&isallow multiple channels", this);
    m_checkboxDisallowMultipleChannels->setToolTip( QApplication::translate(__FUNCTION__, "    <html>\n"
                                                  "    <div style=\"width: 300px;\">This variable exists to help users whose graphics drivers are broken in such a way as to make VP1 crash when showing multiple 3D widgets. Setting this variable will thus ensure that the user can only ever open one channel, and it will remove the little 3D preview window normally shown when editing a material.</div>"
                                                  "    </html>", 0
                                 ));
    #if defined BUILDVP1LIGHT
        if(VP1QtUtils::expertSettingIsOn("general","ExpertSettings/VP1_DISALLOW_MULTIPLE_CHANNELS")){
            m_checkboxDisallowMultipleChannels->setChecked(true);
        }
        connect(m_checkboxDisallowMultipleChannels, &QCheckBox::toggled, [this](){GeneralTab::setExpertSetting("general","ExpertSettings/VP1_DISALLOW_MULTIPLE_CHANNELS");});
    #else
        if(VP1QtUtils::environmentVariableIsOn("VP1_DISALLOW_MULTIPLE_CHANNELS")){
            m_checkboxDisallowMultipleChannels->setChecked(true);
        }
        connect(m_checkboxDisallowMultipleChannels, &QCheckBox::toggled, [this](){GeneralTab::setExpertSetting("VP1_DISALLOW_MULTIPLE_CHANNELS");});
    #endif

    m_checkboxDisplayMouseClicks = new QCheckBox("Dis&play mouse clicks", this);
    m_checkboxDisplayMouseClicks->setToolTip( QApplication::translate(__FUNCTION__, "    <html>\n"
                                                  "    <div style=\"width: 300px;\">Makes a small popup window appear whenever the user clicks a mouse-button in VP1. Basically this feature exists since it is used when producing the movies (screencasts) on this webpage.</div>"
                                                  "    </html>", 0
                                 ));
    #if defined BUILDVP1LIGHT
        if(VP1QtUtils::expertSettingIsOn("general","ExpertSettings/VP1_DISPLAY_MOUSE_CLICKS")){
            m_checkboxDisplayMouseClicks->setChecked(true);
        }
        connect(m_checkboxDisplayMouseClicks, &QCheckBox::toggled, [this](){GeneralTab::setExpertSetting("general","ExpertSettings/VP1_DISPLAY_MOUSE_CLICKS");});
    #else
        if(VP1QtUtils::environmentVariableIsOn("VP1_DISPLAY_MOUSE_CLICKS")){
            m_checkboxDisplayMouseClicks->setChecked(true);
        }
        connect(m_checkboxDisplayMouseClicks, &QCheckBox::toggled, [this](){GeneralTab::setExpertSetting("VP1_DISPLAY_MOUSE_CLICKS");});
    #endif

    m_checkboxEnableAskOnClose = new QCheckBox("&Ask on close", this);
    m_checkboxEnableAskOnClose->setToolTip( QApplication::translate(__FUNCTION__, "    <html>\n"
                                                  "    <div style=\"width: 300px;\">By default, closing the VP1 window immediately results in the job being finished. Setting this variable will instead make VP1 ask the user if it should really close.</div>"
                                                  "    </html>", 0
                                 ));
    #if defined BUILDVP1LIGHT
        if(VP1QtUtils::expertSettingIsOn("general","ExpertSettings/VP1_ENABLE_ASK_ON_CLOSE")){
            m_checkboxEnableAskOnClose->setChecked(true);
        }
        connect(m_checkboxEnableAskOnClose, &QCheckBox::toggled, [this](){GeneralTab::setExpertSetting("general","ExpertSettings/VP1_ENABLE_ASK_ON_CLOSE");});
    #else
        if(VP1QtUtils::environmentVariableIsOn("VP1_ENABLE_ASK_ON_CLOSE")){
            m_checkboxEnableAskOnClose->setChecked(true);
        }
        connect(m_checkboxEnableAskOnClose, &QCheckBox::toggled, [this](){GeneralTab::setExpertSetting("VP1_ENABLE_ASK_ON_CLOSE");});
    #endif


    m_checkboxGuidesSphereInsteadOfCoordaxes = new QCheckBox("G&uides sphere instead of coordinate axes", this);
    m_checkboxGuidesSphereInsteadOfCoordaxes->setToolTip( QApplication::translate(__FUNCTION__, "    <html>\n"
                                                  "    <div style=\"width: 300px;\">Replaces the coordinate axes with a red sphere.</div>"
                                                  "    </html>", 0
                                 ));
    #if defined BUILDVP1LIGHT
        if(VP1QtUtils::expertSettingIsOn("general","ExpertSettings/VP1_GUIDES_SPHERE_INSTEAD_OF_COORDAXES")){
            m_checkboxGuidesSphereInsteadOfCoordaxes->setChecked(true);
        }
        connect(m_checkboxGuidesSphereInsteadOfCoordaxes, &QCheckBox::toggled, [this](){GeneralTab::setExpertSetting("general","ExpertSettings/VP1_GUIDES_SPHERE_INSTEAD_OF_COORDAXES");});
    #else
        if(VP1QtUtils::environmentVariableIsOn("VP1_GUIDES_SPHERE_INSTEAD_OF_COORDAXES")){
            m_checkboxGuidesSphereInsteadOfCoordaxes->setChecked(true);
        }
        connect(m_checkboxGuidesSphereInsteadOfCoordaxes, &QCheckBox::toggled, [this](){GeneralTab::setExpertSetting("VP1_GUIDES_SPHERE_INSTEAD_OF_COORDAXES");});
    #endif


    m_checkboxAntiAliasing = new QCheckBox("Advanced Anti-Aliasing", this);
    m_checkboxAntiAliasing->setToolTip( QApplication::translate(__FUNCTION__, "    <html>\n"
                                                  "    <div style=\"width: 300px;\">Switches on advanced Anti-Aliasing. WARNING: May cause issues with certain graphics drivers.</div>"
                                                  "    </html>", 0
                                 ));
    #if defined BUILDVP1LIGHT
        if(VP1QtUtils::expertSettingIsOn("general","ExpertSettings/VP1_ADVANCED_ANTIALIASING")){
            m_checkboxAntiAliasing->setChecked(true);
        }
        connect(m_checkboxAntiAliasing, &QCheckBox::toggled, [this](){GeneralTab::setExpertSetting("general","ExpertSettings/VP1_ADVANCED_ANTIALIASING");});
    #else
        if(VP1QtUtils::environmentVariableIsOn("VP1_ADVANCED_ANTIALIASING")){
            m_checkboxAntiAliasing->setChecked(true);
        }
        connect(m_checkboxAntiAliasing, &QCheckBox::toggled, [this](){GeneralTab::setExpertSetting("VP1_ADVANCED_ANTIALIASING");});
    #endif



    QGroupBox *directoryGroup = new QGroupBox(tr("Directory settings"));

    m_browseButton1 = new QPushButton(tr("&Browse..."), this);
    connect(m_browseButton1, &QAbstractButton::clicked, this, &GeneralTab::on_browseButton1_clicked);
    #if defined BUILDVP1LIGHT
        m_lineEdit1 = new QLineEdit(VP1QtUtils::expertSettingValue("general","ExpertSettings/VP1_SCREENSHOTS_DIR"), this);
    #else
        m_lineEdit1 = new QLineEdit(VP1QtUtils::environmentVariableValue("VP1_SCREENSHOTS_DIR"), this);
    #endif
    m_lineEdit1->setToolTip( QApplication::translate(__FUNCTION__, "    <html>\n"
                                                  "    <div style=\"width: 300px;\">Turns on the automatic generation of screen-shots for all channels at every event. The value must be a valid writable directory, where all generated screenshot files will be stored.</div>"
                                                  "    </html>", 0
                                 ));
    connect(m_lineEdit1, &QLineEdit::textChanged, this, &GeneralTab::slotScreenshotDirChanged);


    QVBoxLayout *generalLayout = new QVBoxLayout;
    generalLayout->addWidget(m_checkboxVerbose);
    generalLayout->addWidget(m_checkboxDebug);
    generalLayout->addWidget(m_checkboxDisallowMultipleChannels);
    generalLayout->addWidget(m_checkboxDisplayMouseClicks);
    generalLayout->addWidget(m_checkboxEnableAskOnClose);
    generalLayout->addWidget(m_checkboxGuidesSphereInsteadOfCoordaxes);
    generalLayout->addWidget(m_checkboxAntiAliasing);
    generalGroup->setLayout(generalLayout);

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->addWidget(new QLabel(tr("Screenshots directory:")), 0, 0);
    gridLayout->addWidget(m_lineEdit1,1,0);
    gridLayout->addWidget(m_browseButton1,1,1);
    directoryGroup->setLayout(gridLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(generalGroup);
    mainLayout->addWidget(directoryGroup);
    mainLayout->addStretch(1);
    setLayout(mainLayout);

}

#if defined BUILDVP1LIGHT
void GeneralTab::setExpertSetting(const QString &type, const QString &name){
    if(!VP1QtUtils::expertSettingIsOn(type, name)){
        VP1QtUtils::setExpertSetting(type, name, "1");
    } else {
        VP1QtUtils::setExpertSetting(type, name, "");
    }
}
#else
void GeneralTab::setExpertSetting(const QString &name){
    if(!VP1QtUtils::environmentVariableIsOn(name)){
        VP1QtUtils::setEnvironmentVariable(name, "1");
    } else {
        VP1QtUtils::unsetEnvironmentVariable(name);
    }
}
#endif

void GeneralTab::on_browseButton1_clicked(){
    QString fileName = QFileDialog::getExistingDirectory(this,
        tr("Select screenshot directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    m_lineEdit1->setText(fileName);
}

void GeneralTab::slotScreenshotDirChanged(const QString &path){
    emit signalScreenshotDirChanged(path);
}


AdvancedTab::AdvancedTab(QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *checkBoxGroup = new QGroupBox(tr("Advanced settings"));

    m_checkboxEnableInformOnEndOfJob = new QCheckBox("&Enable inform on end of job", this);
    m_checkboxEnableInformOnEndOfJob->setToolTip( QApplication::translate(__FUNCTION__, "    <html>\n"
                                                  "    <div style=\"width: 300px;\">By default, VP1 simply exits at the end of the ATHENA job, which might be confusing to some. When this variable is set, VP1 will show an dialog, letting the user know that the end of the job has been reached.</div>"
                                                  "    </html>", 0
                                 ));
    #if defined BUILDVP1LIGHT
        if(VP1QtUtils::expertSettingIsOn("expert","ExpertSettings/VP1_ENABLE_INFORM_ON_END_OF_JOB")){
            m_checkboxEnableInformOnEndOfJob->setChecked(true);
        }
        connect(m_checkboxEnableInformOnEndOfJob, &QCheckBox::toggled, [this](){AdvancedTab::setExpertSetting("expert","ExpertSettings/VP1_ENABLE_INFORM_ON_END_OF_JOB");});
    #else
        if(VP1QtUtils::environmentVariableIsOn("VP1_ENABLE_INFORM_ON_END_OF_JOB")){
            m_checkboxEnableInformOnEndOfJob->setChecked(true);
        }
        connect(m_checkboxEnableInformOnEndOfJob, &QCheckBox::toggled, [this](){AdvancedTab::setExpertSetting("VP1_ENABLE_INFORM_ON_END_OF_JOB");});
    #endif

    m_checkboxHardExitAtEnd = new QCheckBox("&Hard exit at end", this);
    m_checkboxHardExitAtEnd->setToolTip( QApplication::translate(__FUNCTION__, "    <html>\n"
                                                  "    <div style=\"width: 300px;\">By default, the VP1 algorithm simply tells ATHENA to end the job when VP1 is being closed. This means that ATHENA will shut down with its usual procedure, calling finalise on algorithms, generating summaries, etc. If this variable is set, then VP1 will make the process end immediately and abrubtly instead - saving the user some time.</div>"
                                                  "    </html>", 0
                                 ));
    #if defined BUILDVP1LIGHT
        if(VP1QtUtils::expertSettingIsOn("expert","ExpertSettings/VP1_HARD_EXIT_AT_END")){
            m_checkboxHardExitAtEnd->setChecked(true);
        }
        connect(m_checkboxHardExitAtEnd, &QCheckBox::toggled, [this](){AdvancedTab::setExpertSetting("expert","ExpertSettings/VP1_HARD_EXIT_AT_END");});
    #else
        if(VP1QtUtils::environmentVariableIsOn("VP1_HARD_EXIT_AT_END")){
            m_checkboxHardExitAtEnd->setChecked(true);
        }
        connect(m_checkboxHardExitAtEnd, &QCheckBox::toggled, [this](){AdvancedTab::setExpertSetting("VP1_HARD_EXIT_AT_END");});
    #endif

    m_checkboxDevelShowAllCruiseAndEventControls = new QCheckBox("&Show all cruise and event controls", this);
    m_checkboxDevelShowAllCruiseAndEventControls->setToolTip( QApplication::translate(__FUNCTION__, "    <html>\n"
                                                  "    <div style=\"width: 300px;\">Use to enable a few controls related to VP1 cruise-mode, which have presently been disabled.</div>"
                                                  "    </html>", 0
                                 ));
    #if defined BUILDVP1LIGHT
        if(VP1QtUtils::expertSettingIsOn("expert","ExpertSettings/VP1_DEVEL_SHOW_ALL_CRUISE_AND_EVENT_CONTROLS")){
            m_checkboxDevelShowAllCruiseAndEventControls->setChecked(true);
        }
        connect(m_checkboxDevelShowAllCruiseAndEventControls, &QCheckBox::toggled, [this](){AdvancedTab::setExpertSetting("expert","ExpertSettings/VP1_DEVEL_SHOW_ALL_CRUISE_AND_EVENT_CONTROLS");});
    #else
        if(VP1QtUtils::environmentVariableIsOn("VP1_DEVEL_SHOW_ALL_CRUISE_AND_EVENT_CONTROLS")){
            m_checkboxDevelShowAllCruiseAndEventControls->setChecked(true);
        }
        connect(m_checkboxDevelShowAllCruiseAndEventControls, &QCheckBox::toggled, [this](){AdvancedTab::setExpertSetting("VP1_DEVEL_SHOW_ALL_CRUISE_AND_EVENT_CONTROLS");});
    #endif


    QGroupBox *directoryGroup = new QGroupBox(tr("Directory settings"));

    m_browseButton1 = new QPushButton(tr("&Browse..."), this);
    connect(m_browseButton1, &QAbstractButton::clicked, this, &AdvancedTab::on_browseButton1_clicked);
    m_lineEdit1 = new QLineEdit("", this);

    #if defined BUILDVP1LIGHT
    if(VP1QtUtils::expertSettingValue("expert","ExpertSettings/VP1PLUGINPATH")==""){
        #ifdef MACBUNDLE
            m_lineEdit1->setText(QCoreApplication::applicationDirPath()+"/../Frameworks");
        #else
           m_lineEdit1->setText(QCoreApplication::applicationDirPath()+"/../lib");
        #endif
    } else {
        m_lineEdit1->setText(VP1QtUtils::expertSettingValue("expert","ExpertSettings/VP1PLUGINPATH"));
    }
    #else
    if(VP1QtUtils::environmentVariableValue("VP1PLUGINPATH")==""){
        m_lineEdit1->setText(QCoreApplication::applicationDirPath()+"/../lib");
    } else {
        m_lineEdit1->setText(VP1QtUtils::environmentVariableValue("VP1PLUGINPATH"));
    }
    #endif
    m_lineEdit1->setToolTip( QApplication::translate(__FUNCTION__, "    <html>\n"
                                                  "    <div style=\"width: 300px;\">Select the directory that contains the plugin libraries.</div>"
                                                  "    </html>", 0
                                 ));
    connect(m_lineEdit1, &QLineEdit::textChanged, this, &AdvancedTab::slotPluginPathChanged);

    m_browseButton2 = new QPushButton(tr("B&rowse..."), this);
    connect(m_browseButton2, &QAbstractButton::clicked, this, &AdvancedTab::on_browseButton2_clicked);
    #if defined BUILDVP1LIGHT
        m_lineEdit2 = new QLineEdit(VP1QtUtils::expertSettingValue("expert","ExpertSettings/VP1_FILESELECTDIR"), this);
    #else
        m_lineEdit2 = new QLineEdit(VP1QtUtils::environmentVariableValue("VP1_FILESELECTDIR"), this);
    #endif
    m_lineEdit2->setToolTip( QApplication::translate(__FUNCTION__, "    <html>\n"
                                                  "    <div style=\"width: 300px;\">By default, the various VP1 file selectors starts in the job run-directory. Setting this variable to a directory ensures that that directory is used by default instead.</div>"
                                                  "    </html>", 0
                                 ));
    connect(m_lineEdit2, &QLineEdit::textChanged, this, &AdvancedTab::slotFileSelectDirChanged);

    m_browseButton3 = new QPushButton(tr("Br&owse..."), this);
    connect(m_browseButton3, &QAbstractButton::clicked, this, &AdvancedTab::on_browseButton3_clicked);
    #if defined BUILDVP1LIGHT
        m_lineEdit3 = new QLineEdit(VP1QtUtils::expertSettingValue("expert","ExpertSettings/VP1_AUTH_ENABLELOG"), this);
    #else
        m_lineEdit3 = new QLineEdit(VP1QtUtils::environmentVariableValue("VP1_AUTH_ENABLELOG"), this);
    #endif
    m_lineEdit3->setToolTip( QApplication::translate(__FUNCTION__, "    <html>\n"
                                                  "    <div style=\"width: 300px;\">Enables creation of an authentication log file for VP1 live.</div>"
                                                  "    </html>", 0
                                 ));
    connect(m_lineEdit3, &QLineEdit::textChanged, this, &AdvancedTab::slotAuthLogChanged);


    QVBoxLayout *checkBoxLayout = new QVBoxLayout;
    checkBoxLayout->addWidget(m_checkboxEnableInformOnEndOfJob);
    checkBoxLayout->addWidget(m_checkboxHardExitAtEnd);
    checkBoxLayout->addWidget(m_checkboxDevelShowAllCruiseAndEventControls);
    checkBoxGroup->setLayout(checkBoxLayout);

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->addWidget(new QLabel(tr("Plugin path:")), 0, 0);
    gridLayout->addWidget(m_lineEdit1,1,0);
    gridLayout->addWidget(m_browseButton1,1,1);
    gridLayout->addWidget(new QLabel(tr("File selection directory:")), 2, 0);
    gridLayout->addWidget(m_lineEdit2,3,0);
    gridLayout->addWidget(m_browseButton2,3,1);
    gridLayout->addWidget(new QLabel(tr("Authentication log directory:")), 4, 0);
    gridLayout->addWidget(m_lineEdit3,5,0);
    gridLayout->addWidget(m_browseButton3,5,1);
    directoryGroup->setLayout(gridLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(checkBoxGroup);
    mainLayout->addWidget(directoryGroup);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

void AdvancedTab::on_browseButton1_clicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this,
        tr("Select Plugin Path"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    m_lineEdit1->setText(fileName);
}

void AdvancedTab::on_browseButton2_clicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this,
        tr("Select file directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    m_lineEdit2->setText(fileName);
}

void AdvancedTab::on_browseButton3_clicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this,
        tr("Select authentication directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    m_lineEdit3->setText(fileName);
}

void AdvancedTab::slotPluginPathChanged(const QString &path){
    emit signalPluginPathChanged(path);
}

void AdvancedTab::slotFileSelectDirChanged(const QString &path){
    emit signalFileSelectDirChanged(path);
}

void AdvancedTab::slotAuthLogChanged(const QString &path){
    emit signalAuthLogChanged(path);
}

#if defined BUILDVP1LIGHT
void AdvancedTab::setExpertSetting(const QString &type, const QString &name){
    if(!VP1QtUtils::expertSettingIsOn(type, name)) {
        VP1QtUtils::setExpertSetting(type, name, "1");
    } else {
        VP1QtUtils::setExpertSetting(type, name, "");
    }
}
#else
void AdvancedTab::setExpertSetting(const QString &name){
    if(!VP1QtUtils::environmentVariableIsOn(name)) {
        VP1QtUtils::setEnvironmentVariable(name, "1");
    } else {
        VP1QtUtils::unsetEnvironmentVariable(name);
    }
}
#endif
