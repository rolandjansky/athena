/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class AODSystemController             //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

// FIXME! Remove!
#define protected public
#include <Inventor/nodes/SoSelection.h>
#undef protected

// Local
#include "VP1AODSystems/AODSystemController.h"
#include "VP1AODSystems/VP1AODSystem.h"
#include "AODSysCommonData.h"
#include "IParticleCollWidget.h"
#include "ui_vp1aodcontrollerform.h"
#include "ui_settings_cuts_form.h"
#include "ui_objectbrowser.h"

// VP1
#include "VP1Utils/VP1AvailableToolsHelper.h"
#include "VP1Utils/VP1ToolAccessHelper.h"
#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1ParticleData.h"
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

//____________________________________________________________________
class AODSystemController::Imp {
public:
  AODSystemController * theclass;
  Ui::VP1AODControllerForm ui;
  IParticleCollWidget * collwidget;
  bool updateComboBoxContents(QComboBox*cb,QStringList l,QString& restoredSelection);
  static const QString noneAvailString;

  // Ui::AODSysSettingsColouringForm ui_col;
  // Ui::AODSysSettingsExtrapolationsForm ui_extrap;
  // Ui::AODSysSettingsInteractionsForm ui_int;
  // Ui::AODSysSettingsCutsForm ui_cuts;
  Ui::ObjectBrowser ui_objBrowser;
  
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
  : VP1Controller(sys,"AODSystemController"), d(new Imp)
{
  d->theclass = this;
  //Stuff with tools waits until ::initTools() is called:
  d->ui.setupUi(this);
  d->collwidget = new IParticleCollWidget;
  setupCollWidgetInScrollArea(d->ui.collWidgetScrollArea,d->collwidget);

  initDialog(d->ui_objBrowser, d->ui.pushButton_ObjectBrowser);

  //init:  
  // d->initMaterials();



  //Disable elements based on job configuration:

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  Setup connections which monitor changes in the controller so that we may emit signals as appropriate:  //
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // TrackObjBrowser
  messageVerbose("Enabling object browser");
  d->objBrowserWidget = d->ui_objBrowser.treeWidget;
  d->objBrowserWidget->setSortingEnabled(false);
  QStringList l; 
  l<<"Object"<<"Information";
  d->objBrowserWidget->setHeaderLabels(l);
  connect(d->objBrowserWidget,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(objectBrowserClicked(QTreeWidgetItem *, int)));

  // we want "Print information" on single track selection turned ON by default
  // //d->ui_int.checkBox_selsingle_printinfo->setChecked(true);

  initLastVars();
}

//____________________________________________________________________
AODSystemController::~AODSystemController()
{
  messageVerbose("~AODSystemController begin");
  delete d;
  messageVerbose("~AODSystemController end");
}

//____________________________________________________________________
int AODSystemController::currentSettingsVersion() const
{
  return 1;
}

//____________________________________________________________________
void AODSystemController::actualSaveSettings(VP1Serialise&s) const
{
}

//____________________________________________________________________
void AODSystemController::actualRestoreSettings(VP1Deserialise& s)
{
}

//____________________________________________________________________
IParticleCollWidget * AODSystemController::collWidget() const
{
  return d->collwidget;
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
    ////d->ui_extrap.radioButton_athenaExtrapolator->setEnabled(true);
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
  // return d->ui_int.checkBox_selsingle_orientzoom->isChecked();
}

//____________________________________________________________________
bool AODSystemController::printInfoOnSingleSelection() const
{
  // return d->ui_int.checkBox_selsingle_printinfo->isChecked();
}

//____________________________________________________________________
bool AODSystemController::printVerboseInfoOnSingleSelection() const
{
  // return printInfoOnSingleSelection() && d->ui_int.checkBox_selsingle_printinfo_verbose->isChecked();
}

//____________________________________________________________________
bool AODSystemController::printTotMomentumOnMultiTrackSelection() const
{
  // return d->ui_int.checkBox_sel_printtotmom->isChecked();
}

//____________________________________________________________________
bool AODSystemController::showTotMomentumOnMultiTrackSelection() const
{
  // return d->ui_int.checkBox_sel_showtotmom->isChecked();
}

QTreeWidget* AODSystemController::objBrowser() const
{
    return d->objBrowserWidget;
}

AODSysCommonData * AODSystemController::common() const {
  return d->common;
}
void AODSystemController::setCommonData(AODSysCommonData * common){
  d->common=common;
}

void AODSystemController::objectBrowserClicked(QTreeWidgetItem * item, int){
  messageVerbose("objectBrowserClicked for "+item->text(0));
 //  
 //  VP1TrackSystem* sys = dynamic_cast<VP1TrackSystem*>(systemBase());
 //  if (!sys){
 //    messageVerbose("AODSystemController::objectBrowserClicked: Couldn't get VP1TrackSystem pointer");
 //    return;
 //  }
 //  
 //  //sys->deselectAll(); // FIXME. necessary?
 // 
 //  SoNode* node = common()->node(item);
 //  if (node) {
 //    // okay, have track
 //    SoCooperativeSelection * sel = sys->selTracks();
 //    sel->select(node);
 //  } else {
 //    // maybe it's a TSOS? Check first that it has a parent 
 //    if (item->parent()) node = common()->node(item->parent());
 //    if (node) {
 //      // yes, so now get index within track, as we can hopefully use this to find the AscObj_TSOS
 //      unsigned int index = item->parent()->indexOfChild(item);// should correspond to the TSOS number
 //      messageVerbose("AODSystemController::objectBrowserClicked: item has index of "+QString::number(index));
 //      TrackHandle_TrkTrack* trkHandle = dynamic_cast<TrackHandle_TrkTrack*>(common()->trackHandle(node));
 //      if (trkHandle && trkHandle->trkTrackPointer()) {
 //        if (index<trkHandle->trkTrackPointer()->trackStateOnSurfaces()->size() ){
 //          // in range
 //          const Trk::TrackStateOnSurface* tsos = (*trkHandle->trkTrackPointer()->trackStateOnSurfaces())[index];
 //          // now find matching AscObj_TSOS
 //          QList<AssociatedObjectHandleBase*> list = trkHandle->getVisibleMeasurements();
 //          for (int i = 0; i < list.size(); ++i) {
 //            messageVerbose("AODSystemController::objectBrowserClicked: checking ASC "+QString::number(i));
 // 
 //            AscObj_TSOS* asc = dynamic_cast<AscObj_TSOS*>(list.at(i));
 //            if (asc && asc->trackStateOnSurface()==tsos) {
 //               messageVerbose("AODSystemController::objectBrowserClicked: this ASC matches "+QString::number(i));
 //              //asc->zoomView();
 //               common()->ascObjSelectionManager()->pretendUserClicked(asc);          
 //            } else {
 //              messageVerbose("AODSystemController::objectBrowserClicked: no matching Asc found");
 //            }
 //          } // for loop
 //        } else {
 //          messageVerbose("AODSystemController::objectBrowserClicked: index of  "+QString::number(index)+" is greater than number of TSOSes:"+QString::number(trkHandle->trkTrackPointer()->trackStateOnSurfaces()->size()));
 //        }
 //      }
 //    } else {
 //      messageVerbose("AODSystemController::objectBrowserClicked: Couldn't get node pointer. Maybe object not visible?");
 //    }
 //  }
 //  
 //  
 //  // if (selTrack){
 //  //   SoCooperativeSelection * sel = sys->selTracks();
 //  //   sel->select(node);
 //  // } else {
 //  //   common()->ascObjSelectionManager()->pretendUserClicked();
 //  // }
 //  // 
 //  // SoCooperativeSelection * sel = sys->selTracks();
 //  // SoCooperativeSelection * selAsc  = dynamic_cast<SoCooperativeSelection*>(common()->ascObjSelectionManager()->getAscObjAttachSep());
 //  // SoSeparator* eventRoot = common()->ascObjSelectionManager()->eventRoot();  
 //  // 
 //  // SoSearchAction mySearchAction;
 //  // mySearchAction.setNode(node);
 //  // // SoSeparator* eventRoot = common()->ascObjSelectionManager()->eventRoot();
 //  // mySearchAction.apply(eventRoot); 
 //  // SoPath* path=mySearchAction.getPath();
 //  // if ( !path ) { 
 //  //   messageVerbose("AODSystemController::objectBrowserClicked: Couldn't get SoPath");
 //  //   return;
 //  // } else {
 //  //   messageVerbose("AODSystemController::objectBrowserClicked: got SoPath. Will now try to select.");
 //  //   messageVerbose("pathlength="+QString::number(path->getLength()));
 //  // }
 //  // 
 //  // //sel->select(path); //Doesn't work.
 //  // // messageVerbose("findpath="+QString::number(sel->findPath(path)));
 //  // sel->select(node); // Doesn't work.
 //  // if (sel->getNumSelected()==0){
 //  //   messageVerbose("AODSystemController::objectBrowserClicked: Couldn't select. Trying with ASC sel node.");
 //  //   
 //  //   selAsc->select(node);
 //  //   if (selAsc->getNumSelected()==0){
 //  //     messageVerbose("AODSystemController::objectBrowserClicked: Couldn't select. Trying with ASC sel using path.");
 //  //     
 //  //     selAsc->select(path);
 //  //   }
 //  // }
 //  // // messageVerbose("findpath="+QString::number(sel->findPath(path)));
 //  // 
 //  // // sel->touch();
 //  // messageVerbose("numselected="+QString::number(sel->getNumSelected()));
 //  
 // // path->unref();
}

///////////////////////////////////////////////////////////////////////////
// Test for possible changes in values and emit signals as appropriate:
// (possibleChange_XXX() slots code provided by macros)
#define VP1CONTROLLERCLASSNAME AODSystemController
#include "VP1Base/VP1ControllerMacros.h"

