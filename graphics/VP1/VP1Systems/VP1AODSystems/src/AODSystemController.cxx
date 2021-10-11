/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class AODSystemController             //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoSelection.h>

// Local
#include "VP1AODSystems/AODSystemController.h"
#include "VP1AODSystems/VP1AODSystem.h"
#include "AODSysCommonData.h"
#include "AODCollWidget.h"
#include "AODHandleBase.h"
#include "ui_vp1aodcontrollerform.h"
#include "ui_settings_cuts_form.h"
#include "ui_objectbrowser.h"

// VP1
#include "VP1Utils/VP1AvailableToolsHelper.h"
#ifndef BUILDVP1LIGHT
    #include "VP1Utils/VP1ToolAccessHelper.h"
    #include "VP1Utils/VP1JobConfigInfo.h"
    #include "VP1Utils/VP1ParticleData.h"
#endif
#include "VP1Base/VP1QtUtils.h"
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1SoMaterialMixer.h"
#include "VP1Base/SoCooperativeSelection.h"

// SoCoin
#include "Inventor/nodes/SoMaterial.h"
#include "Inventor/nodes/SoDrawStyle.h"
#include "Inventor/nodes/SoComplexity.h"
#include "Inventor/nodes/SoLightModel.h"
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/actions/SoSearchAction.h>

// Qt
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <typeinfo>
//____________________________________________________________________
class AODSystemController::Imp {
public:
  AODSystemController * theclass;
  Ui::VP1AODControllerForm ui;
  AODCollWidget * collwidget;
  bool updateComboBoxContents(QComboBox*cb,QStringList l,QString& restoredSelection);
  static const QString noneAvailString;

  // Ui::AODSysSettingsColouringForm ui_col;
  // Ui::AODSysSettingsExtrapolationsForm ui_extrap;
  // Ui::AODSysSettingsInteractionsForm ui_int;
  // Ui::AODSysSettingsCutsForm ui_cuts;
  Ui::AODObjectBrowser ui_objBrowser;
  
  QTreeWidget* objBrowserWidget;
  AODSysCommonData * common;
};

const QString AODSystemController::Imp::noneAvailString = QString("None available");


// //____________________________________________________________________
// QString AODSystemController::toString( const QList<unsigned>& l )
// {
//   QString s;
//   foreach(unsigned i,l) {
//     if (!s.isEmpty()) s+=", ";
//     s+=QString::number(i);
//   }
//   return "["+s+"]";
// }
// 
// //____________________________________________________________________
// QString AODSystemController::toString( const QList<int>& l )
// {
//   QString s;
//   foreach(int i,l) {
//     if (!s.isEmpty()) s+=", ";
//     s+=QString::number(i);
//   }
//   return "["+s+"]";
// }

//____________________________________________________________________


//____________________________________________________________________
AODSystemController::AODSystemController(IVP1System * sys)
  : VP1Controller(sys,"AODSystemController"), m_d(new Imp)
{
  m_d->theclass = this;
  //Stuff with tools waits until ::initTools() is called:
  m_d->ui.setupUi(this);
  m_d->collwidget = new AODCollWidget;
  setupCollWidgetInScrollArea(m_d->ui.collWidgetScrollArea,m_d->collwidget);

  initDialog(m_d->ui_objBrowser, m_d->ui.pushButton_ObjectBrowser);

  //init:  
  // m_d->initMaterials();



  //Disable elements based on job configuration:

  

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  Setup connections which monitor changes in the controller so that we may emit signals as appropriate:  //
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // TrackObjBrowser
  messageVerbose("Enabling object browser");
  m_d->objBrowserWidget = m_d->ui_objBrowser.treeWidget;
  m_d->objBrowserWidget->setSortingEnabled(false);
  QStringList l; 
  l<<"Object"<<"Information";
  m_d->objBrowserWidget->setHeaderLabels(l);
  connect(m_d->objBrowserWidget,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(objectBrowserClicked(QTreeWidgetItem *, int)));

  // Hide interactions until we're ready.
  m_d->ui.pushButton_interactions->hide();
  
  // Tell system to dump to JSON
  connect(m_d->ui.pushButton_dumpToJSON,SIGNAL(pressed()),systemBase(),SLOT(dumpToJSON()));
  
  if (VP1QtUtils::environmentVariableIsSet("VP1_DUMPTOJSON")){
    messageVerbose("AODSystemController enable dumping to JSON");
    m_d->ui.pushButton_dumpToJSON->setMaximumHeight(static_cast<int>(0.5+QFontMetricsF(m_d->ui.pushButton_dumpToJSON->font()).height()*1.05+2));
    m_d->ui.pushButton_dumpToJSON->setMinimumHeight(m_d->ui.pushButton_dumpToJSON->maximumHeight());
    m_d->ui.pushButton_dumpToJSON->setCheckable(true);
    
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_d->ui.pushButton_dumpToJSON->sizePolicy().hasHeightForWidth());
    m_d->ui.pushButton_dumpToJSON->setSizePolicy(sizePolicy);
    m_d->ui.pushButton_dumpToJSON->setFocusPolicy(Qt::NoFocus);
  } else {
    m_d->ui.pushButton_dumpToJSON->hide();
  }
  
  // we want "Print information" on single track selection turned ON by default
  // //m_d->ui_int.checkBox_selsingle_printinfo->setChecked(true);

  initLastVars();
}

//____________________________________________________________________
AODSystemController::~AODSystemController()
{
  messageVerbose("~AODSystemController begin");
  delete m_d;
  messageVerbose("~AODSystemController end");
}

//____________________________________________________________________
int AODSystemController::currentSettingsVersion() const
{
  int version = 1;
  messageVerbose("AODSystemController::currentSettingsVersion() - current version: " + QString::number(version));
  return version;
}

//____________________________________________________________________
void AODSystemController::actualSaveSettings(VP1Serialise&s) const
{
  messageVerbose("AODSystemController::actualSaveSettings()");
  messageDebug("version: " + QString::number(s.version()));

}

//____________________________________________________________________
void AODSystemController::actualRestoreSettings(VP1Deserialise& /**s*/)
{
  messageVerbose("AODSystemController::actualRestoreSettings()");
}

//____________________________________________________________________
AODCollWidget * AODSystemController::collWidget() const
{
  return m_d->collwidget;
}

//Access methods:

//____________________________________________________________________
//Returns false if "none available"
bool AODSystemController::Imp::updateComboBoxContents(QComboBox*cb,QStringList l,QString& restoredSelection)
{
  //current selection:
  QString ct = cb->currentText();
  if (ct==Imp::noneAvailString)
    ct = "";

  bool save = cb->blockSignals(true);

  cb->clear();

  bool enabled = false;
  if (l.isEmpty()) {
    cb->addItem(Imp::noneAvailString);
    cb->setEnabled(false);
  } else {
    cb->addItems(l);

    int i = restoredSelection.isEmpty() ? -1 : cb->findText(restoredSelection);
    if (i<0)
      i = ct.isEmpty() ? -1 : cb->findText(ct);
    restoredSelection = "";

    if (i>=0) {
      cb->setCurrentIndex(i);
    } else {
      //Let's try to pick the default to be VP1Extrapolater over
      //AtlasExtrapolater over... whatever (same for fitters):
      int i_vp1(-1), i_atlas(-1);
      for (int j = 0; j <cb->count();++j) {
        if (i_vp1==-1&&cb->itemText(j).contains("vp1",Qt::CaseInsensitive))
          i_vp1 = j;
        if (i_atlas==-1&&cb->itemText(j).contains("atlas",Qt::CaseInsensitive))
          i_atlas = j;
      }
      if (i_vp1>=0)
        cb->setCurrentIndex(i_vp1);
      else if (i_atlas>=0)
        cb->setCurrentIndex(i_atlas);
    }
    ////m_d->ui_extrap.radioButton_athenaExtrapolator->setEnabled(true);
    enabled = true;
    cb->setEnabled(true);
  }

  if (!save)
    cb->blockSignals(false);
  return enabled;
}

//____________________________________________________________________
bool AODSystemController::orientAndZoomOnSingleSelection() const
{
  return false;
  // return m_d->ui_int.checkBox_selsingle_orientzoom->isChecked();
}

//____________________________________________________________________
bool AODSystemController::printInfoOnSingleSelection() const
{
  return false;
  // return m_d->ui_int.checkBox_selsingle_printinfo->isChecked();
}

//____________________________________________________________________
bool AODSystemController::printVerboseInfoOnSingleSelection() const
{
  return false;
  // return printInfoOnSingleSelection() && m_d->ui_int.checkBox_selsingle_printinfo_verbose->isChecked();
}

//____________________________________________________________________
bool AODSystemController::printTotMomentumOnMultiTrackSelection() const
{
  return false;
  // return m_d->ui_int.checkBox_sel_printtotmom->isChecked();
}

//____________________________________________________________________
bool AODSystemController::showTotMomentumOnMultiTrackSelection() const
{
  return false;
  // return m_d->ui_int.checkBox_sel_showtotmom->isChecked();
}

QTreeWidget* AODSystemController::objBrowser() const
{
  return m_d->objBrowserWidget;
}

AODSysCommonData * AODSystemController::common() const {
  return m_d->common;
}
void AODSystemController::setCommonData(AODSysCommonData * common){
  m_d->common=common;
}

void AODSystemController::objectBrowserClicked(QTreeWidgetItem * item, int){
  messageVerbose("objectBrowserClicked for "+item->text(0));
  //  
   IVP1System* sysBase = systemBase();
   if (!sysBase){
     messageVerbose("AODSystemController::objectBrowserClicked: Couldn't get system base pointer");
     return;
   }
   
   VP1AODSystem* sys = dynamic_cast<VP1AODSystem*>(sysBase);
   if (!sys){
     messageVerbose("AODSystemController::objectBrowserClicked: Couldn't get VP1AODSystem pointer");
     std::cout<<"Pointer value = "<<sysBase<<" and type = "<<typeid(sysBase).name()<<std::endl;
     return;
   }
   
   
   sys->deselectAll(); // FIXME. necessary?
  // 
   SoNode* node = common()->getNodeFromBrowser(item);
   if (node) {
     std::cout<< "Have node from browser: "<<node<<std::endl;
     
     AODHandleBase* handle = common()->getHandleFromNode(node);
     handle->clicked();
     // okay, have object
     
     SoCooperativeSelection * sel = sys->selObjects();
     std::cout<< "sel->select(node): "<<node<<std::endl;

     sel->select(node);
   }
   messageVerbose("end objectBrowserClicked ");
}

///////////////////////////////////////////////////////////////////////////
// Test for possible changes in values and emit signals as appropriate:
// (possibleChange_XXX() slots code provided by macros)
#define VP1CONTROLLERCLASSNAME AODSystemController
#include "VP1Base/VP1ControllerMacros.h"

