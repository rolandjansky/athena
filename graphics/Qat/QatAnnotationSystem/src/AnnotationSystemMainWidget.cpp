#include "QatAnnotationSystem/AnnotationSystemMainWidget.h"
#include <QtGui/QFileDialog>
#include <QClipboard>
#include <QPixmap>
#include <QPrinter>
#include <QPrintDialog>
#include <QtGui/QMessageBox>
#include <iostream>
#include <sstream>
#include <fstream>
#include <libgen.h>

#include <unistd.h>

AnnotationSystemMainWidget::AnnotationSystemMainWidget (QWidget *parent ):QWidget(parent) {
  ui.setupUi(this);
}

void AnnotationSystemMainWidget::insertDate() {
  ui.textEdit->insertDate();
}

void AnnotationSystemMainWidget::insertImage() {
 
  char dirbuff[1024];
  getcwd(dirbuff,1024);
  static std::string workingDir=dirbuff;
  static std::string attachFileName="";

  QFileDialog dialog(0);
  QStringList filters;
  filters.append("Image files (*.png *.gif *.jpg)");
  filters.append("All files (*)");
  dialog.setFilters(filters);
  dialog.setDirectory(workingDir.c_str());
  dialog.selectFile(attachFileName.c_str());
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (dialog.exec()) {
    
    QStringList openFilenames=dialog.selectedFiles();
    if (openFilenames.count()!=1) return;
    QString openFileName = openFilenames[0];
    workingDir=dirname((char *) openFileName.toStdString().c_str());
    
    if (!openFileName.isNull()) {
      QPixmap px(openFileName);

      int height = px.height();
      int width  = px.width();

      attachFileName = openFileName.toStdString();
      QTextEdit *tE = textEdit();
      std::ostringstream xstream;
      xstream << "<img SRC="  << attachFileName << " height=" << height << " width=" << width << " >";
      std::string imageString = xstream.str();
      tE->insertHtml(imageString.c_str());
    }
  }
}
void AnnotationSystemMainWidget::print() {

  QPrinter printer;
  printer.setColorMode(QPrinter::Color);
  printer.setResolution(600);
  printer.setFullPage(TRUE);
  QPrintDialog pDialog(&printer, this);
  if (pDialog.exec()) {
    QTextEdit *tE = textEdit();
    tE->document()->print(&printer);

  }
}
void AnnotationSystemMainWidget::save() {
 
  char dirbuff[1024];
  getcwd(dirbuff,1024);
  static std::string workingDir=dirbuff;
  static std::string attachFileName="";

  QFileDialog dialog(0);
  QStringList filters;
  filters.append("Html files (*.png *.html)");
  filters.append("All files (*)");
  dialog.setFilters(filters);
  dialog.setDirectory(workingDir.c_str());
  dialog.selectFile(attachFileName.c_str());
  dialog.setFileMode(QFileDialog::AnyFile);
  if (dialog.exec()) {
    
    QStringList openFilenames=dialog.selectedFiles();
    if (openFilenames.count()!=1) return;
    QString openFileName = openFilenames[0];
    workingDir=dirname((char *) openFileName.toStdString().c_str());
    
    if (!openFileName.isNull()) {
      attachFileName = openFileName.toStdString();
      QTextEdit *tE = textEdit();
      std::string html = tE->document()->toHtml().toStdString();
      std::ofstream o(attachFileName.c_str());
      o << html;
      
    }
  }
}

void AnnotationSystemMainWidget::pasteImage() {
  system("mkdir -p ~/.presenter");
  QPixmap px = QApplication::clipboard()->pixmap();
  std::ostringstream genFileNameStream;
  std::string homeDir=getenv("HOME");
  genFileNameStream << homeDir << "/.presenter/annotationSystemImage"
		    << rand() << ".png";
  if (!px.save(genFileNameStream.str().c_str())) {
    QMessageBox::warning(this, tr("Warning"),
			 tr(("Cannot save image file "+genFileNameStream.str()).c_str()),
			 QMessageBox::Ok);
  }
  else {
    int height = px.height();
    int width  = px.width();

    QTextEdit *tE = textEdit();
    
    std::ostringstream xstream;
    xstream << "<img SRC=" << genFileNameStream.str() << " height=" << height << " width=" << width << " >";

    std::string imageString = xstream.str();
    tE->insertHtml(imageString.c_str());
  }
}


QTextEdit *AnnotationSystemMainWidget::textEdit() {
  return ui.textEdit->textEdit();
}

TextEditor *AnnotationSystemMainWidget::textEditor() {
  return ui.textEdit;
}
