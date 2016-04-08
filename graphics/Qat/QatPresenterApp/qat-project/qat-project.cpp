// This is a program to write a program:
#include "QatProjectForm.h"
#include <QApplication>

int main (int argc, char ** argv) {

  QApplication *app = new QApplication(argc, argv);
  QatProjectForm *form = new QatProjectForm(NULL);
  form->show();
  return app->exec();

}
