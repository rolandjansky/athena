#include "QatProjectForm.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QCheckBox>
#include <QRadioButton>
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

QatProjectForm::QatProjectForm(QWidget *parent) :
  QWidget(parent) 
{
  ui.setupUi(this);
  connect(ui.quitButton,                  SIGNAL(clicked()),         qApp, SLOT(closeAllWindows()));
  connect(ui.createProjectButton,         SIGNAL(clicked()),         this, SLOT(createProject()));
  connect(ui.programNameLineEdit,         SIGNAL(editingFinished()), this, SLOT(editingFinished()));
  connect(ui.directoryNamePushButton,     SIGNAL(clicked()),         this, SLOT(changeDir()));
}

QatProjectForm::~QatProjectForm() {
}

void QatProjectForm::createProject() {
  
  std::string programName = ui.programNameLineEdit->text().toStdString();
  if (programName=="") {

    QMessageBox::warning(this, tr("upvs-project"),
			 tr("You must select a name for your program"),
			 QMessageBox::Ok);

    return;
  }

  std::string directoryName = ui.directoryLabel->text().toStdString();
  if (directoryName=="") {

    QMessageBox::warning(this, tr("upvs-project"),
			 tr("You must select a directory for your program"),
			 QMessageBox::Ok);

    return;
  }
  

  bool plotView=ui.plotViewCheckBox->isChecked();
  enum Type {ZeroToZero, OneToZero, NToZero, ZeroToOne, OneToOne, NToOne};

  Type type = ZeroToZero;
  if (ui.zeroToOneRadioButton->isChecked()) type=ZeroToOne;
  if (ui.oneToOneRadioButton->isChecked())  type=OneToOne;
  if (ui.nToOneRadioButton->isChecked())    type=NToOne;
  if (ui.nToZeroRadioButton->isChecked())   type=NToZero;
  if (ui.oneToZeroRadioButton->isChecked()) type=OneToZero;
  

  // Make the directory:
  if (mkdir(directoryName.c_str(), 0755)) {
    QMessageBox::warning(this, tr("upvs-project"),
			 QString("Error creating directory ") + QString(directoryName.c_str())+ QString(": ") + QString(strerror(errno)),
			 QMessageBox::Ok);
    
    return;
  }
  mkdir((directoryName+"/src").c_str(),0755);
  system ((std::string("cp /usr/local/share/templates/qt.pro ") + directoryName+"/src").c_str()); 

#ifdef __APPLE__
  system (("sed -i .orig s/\"<app>\"/" + programName + "/g " + directoryName + "/src/qt.pro").c_str());
#else
  system (("sed -i  s/\\<app\\>/" + programName + "/g " + directoryName + "/src/qt.pro").c_str());
#endif
  // There are eight templates.  Choose one of them:
  if (plotView) {
    if (type==ZeroToZero) system (("cp /usr/local/share/templates/templateZeroToZeroView.cpp " + directoryName + "/src/" + programName + ".cpp").c_str()); 
    if (type==OneToZero)  system (("cp /usr/local/share/templates/templateOneToZeroView.cpp "  + directoryName + "/src/" + programName + ".cpp").c_str()); 
    if (type==NToZero)    system (("cp /usr/local/share/templates/templateNToZeroView.cpp "    + directoryName + "/src/" + programName + ".cpp").c_str()); 
    if (type==ZeroToOne)  system (("cp /usr/local/share/templates/templateZeroToOneView.cpp "  + directoryName + "/src/" + programName + ".cpp").c_str()); 
    if (type==OneToOne)   system (("cp /usr/local/share/templates/templateOneToOneView.cpp "   + directoryName + "/src/" + programName + ".cpp").c_str()); 
    if (type==NToOne)     system (("cp /usr/local/share/templates/templateNToOneView.cpp "     + directoryName + "/src/" + programName + ".cpp").c_str()); 
  }
  else {
    if (type==ZeroToZero) system (("cp /usr/local/share/templates/templateZeroToZero.cpp "     + directoryName + "/src/" + programName + ".cpp").c_str()); 
    if (type==OneToZero)  system (("cp /usr/local/share/templates/templateOneToZero.cpp "      + directoryName + "/src/" + programName + ".cpp").c_str()); 
    if (type==NToZero)    system (("cp /usr/local/share/templates/templateNToZero.cpp "        + directoryName + "/src/" + programName + ".cpp").c_str()); 
    if (type==ZeroToOne)  system (("cp /usr/local/share/templates/templateZeroToOne.cpp "      + directoryName + "/src/" + programName + ".cpp").c_str()); 
    if (type==OneToOne)   system (("cp /usr/local/share/templates/templateOneToOne.cpp "       + directoryName + "/src/" + programName + ".cpp").c_str()); 
    if (type==NToOne)     system (("cp /usr/local/share/templates/templateNToOne.cpp "         + directoryName + "/src/" + programName + ".cpp").c_str()); 
  }

  qApp->closeAllWindows();


}

void QatProjectForm::editingFinished() {
  char BUFF[1024];
  if (!ui.programNameLineEdit->text().isEmpty() && ui.directoryLabel->text().isEmpty()) 
    ui.directoryLabel->setText(QString(getcwd(BUFF,1024))+QString("/")+ui.programNameLineEdit->text().toUpper()); 
}

void QatProjectForm::changeDir() {
  char BUFF[1024];
  QString fileName = QFileDialog::getSaveFileName(this, tr("Set Project Directory"),
						  getcwd(BUFF,1024),
						  QString(""),
						  NULL,
						  QFileDialog::ShowDirsOnly);
  ui.directoryLabel->setText(fileName);
}
