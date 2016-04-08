#include "QatAnnotationSystem/AnnotationSystemMenu.h"
AnnotationSystemMenu::AnnotationSystemMenu (QWidget *parent ):QWidget(parent) {
  ui.setupUi(this);
  connect (ui.insertDatePushButton, SIGNAL(pressed()), this, SIGNAL(insertDate()));
  connect (ui.insertImagePushButton,SIGNAL(pressed()), this, SIGNAL(insertImage()));
  connect (ui.pasteImagePushButton, SIGNAL(pressed()), this, SIGNAL(pasteImage()));

  connect (ui.printPagePushButton, SIGNAL(pressed()), this, SIGNAL(print()));
  connect (ui.savePagePushButton,  SIGNAL(pressed()), this, SIGNAL(save()));
}
