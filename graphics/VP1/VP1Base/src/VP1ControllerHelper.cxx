/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1ControllerHelper            //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/VP1ControllerHelper.h"
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1TabWidget.h"
#include "VP1Base/VP1ColorSelectButton.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QVBoxLayout>


//_____________________________________________________________________________________________________________
QWidget * VP1ControllerHelper::compositionController( const QList<IVP1System*>& systemsWithControllersAllowed,
						      QMap<IVP1System*,QWidget*>& sys2tabpage,
						      VP1TabWidget*&tabWidget,
						      const QList<QPair<IVP1System*,bool> >& system2switchable,
						      QMap<QCheckBox*,IVP1System*>& checkbox2system,
						      VP1ColorSelectButton* & colorselectbutton,
						      QWidget * extrawidget, bool nobgdcolorsel)
{
  tabWidget = 0;

  //First we figure out which system controllers we should add:
  QList<QPair<QWidget*,QString> > controllerTabs;
  foreach (IVP1System* s, systemsWithControllersAllowed) {
    QWidget* w = s->controllerWidget();
    if (w) {
      controllerTabs << QPair<QWidget*,QString>(w,s->name());
      sys2tabpage.insert(s,w);
    }
  }

  //Setup the general tab which switches controllers on and off.
  QWidget * tab_general = new QWidget(0);
  QVBoxLayout * vboxLayout1 = new QVBoxLayout(tab_general);
  vboxLayout1->setSpacing(6);
  vboxLayout1->setMargin(9);
  QHBoxLayout * hboxLayout = new QHBoxLayout();
  hboxLayout->setSpacing(6);
  hboxLayout->setMargin(0);
  QLabel * tab_general_label = new QLabel(tab_general);
  tab_general_label->setText("No systems can be switched on/off in this channel.");
  hboxLayout->addWidget(tab_general_label);
  QSpacerItem * spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  hboxLayout->addItem(spacerItem);
  vboxLayout1->addLayout(hboxLayout);
  QHBoxLayout * hboxLayout1 = new QHBoxLayout();
  hboxLayout1->setSpacing(0);
  hboxLayout1->setMargin(0);
  QSpacerItem * spacerItem1 = new QSpacerItem(25, 2, QSizePolicy::Fixed, QSizePolicy::Fixed);
  hboxLayout1->addItem(spacerItem1);
  QVBoxLayout *vboxLayout_checkboxes = new QVBoxLayout();
  vboxLayout_checkboxes->setSpacing(6);
  vboxLayout_checkboxes->setMargin(0);
  hboxLayout1->addLayout(vboxLayout_checkboxes);
  QSpacerItem * spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  hboxLayout1->addItem(spacerItem2);
  vboxLayout1->addLayout(hboxLayout1);

  //Add checkboxes for switching systems on/off
  for (int i = 0; i < system2switchable.count(); ++i) {
    QCheckBox * checkBox = new QCheckBox(tab_general);
    checkBox->setText(system2switchable.at(i).first->name());
    QString info(system2switchable.at(i).first->information());
    if (!info.isEmpty())
      checkBox->setToolTip(info);
    checkBox->setChecked(true);
    vboxLayout_checkboxes->addWidget(checkBox);
    if (system2switchable.at(i).second) {
      tab_general_label->setText("Select active systems:");
      checkbox2system.insert(checkBox,system2switchable.at(i).first);
    } else {
      checkBox->setEnabled(false);
    }
  }

  //Add pushbutton for choosing background color:
  if (!nobgdcolorsel) {
    colorselectbutton = new VP1ColorSelectButton(tab_general,Qt::black);
    QHBoxLayout * hboxLayout2 = new QHBoxLayout();
    hboxLayout2->setSpacing(6);
    hboxLayout2->setMargin(0);
    hboxLayout2->addWidget(colorselectbutton);
    hboxLayout2->addWidget(new QLabel("Set background"));
    QSpacerItem * spacerItem4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout2->addItem(spacerItem4);
    QSpacerItem * spacerItem5 = new QSpacerItem(10, 10, QSizePolicy::Minimum,QSizePolicy::Fixed);
    vboxLayout1->addItem(spacerItem5);
    vboxLayout1->addLayout(hboxLayout2);
  }

  //Maybe add another custom widget:
  if (extrawidget) {
    extrawidget->setParent(tab_general);
    QHBoxLayout * hboxLayout_extra = new QHBoxLayout();
    hboxLayout_extra->setSpacing(6);
    hboxLayout_extra->setMargin(0);
    hboxLayout_extra->addWidget(extrawidget);
    QSpacerItem * spacerItem_extra1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout_extra->addItem(spacerItem_extra1);
    QSpacerItem * spacerItem_extra2 = new QSpacerItem(10, 10, QSizePolicy::Minimum,QSizePolicy::Fixed);
    vboxLayout1->addItem(spacerItem_extra2);
    vboxLayout1->addLayout(hboxLayout_extra);
    QSpacerItem * spacerItem_extra3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vboxLayout1->addItem(spacerItem_extra3);
  }

  QSpacerItem * spacerItem3 = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
  vboxLayout1->addItem(spacerItem3);

  //If we dont have any system controllers to add, that tab is the whole controller:
  if (controllerTabs.empty()) {
    return tab_general;
  }

  //Let us make an actual tab widget:
  tabWidget = new VP1TabWidget(0);
  tabWidget->addTab(tab_general, "General");

  //Add system tabs:
  for (int i = 0; i<controllerTabs.count();++i) {
    controllerTabs.at(i).first->setParent(tabWidget);//We assume ownership of the system controller.
    tabWidget->addTab(controllerTabs.at(i).first,controllerTabs.at(i).second);
  }

  //Show first tab:
  tabWidget->setCurrentIndex(0);

  return tabWidget;
}

