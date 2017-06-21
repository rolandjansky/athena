/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class AODCollHandleBase               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

//Local includes
//#include "AODCollHandleBase.h"
#include "AODHandleBase.h"
#include "AODCollHandleBase.h"

#include "VP1AODSystems/VP1AODSystem.h"
#include "VP1AODSystems/AODSystemController.h"
#include "AODSysCommonData.h"

//VP1 base includes
#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Utils/VP1LinAlgUtils.h"
#include "VP1Base/IVP13DSystem.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"

//SoCoin
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSwitch.h>
#include "Inventor/nodes/SoDrawStyle.h"
#include "Inventor/nodes/SoLightModel.h"

//Athena
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

//Qt
#include <QComboBox>
#include <QTreeWidgetItem>
#include <qdatetime.h>
#include <vector>
#include <QString>

//____________________________________________________________________
class AODCollHandleBase::Imp {
public:
  AODCollHandleBase * theclass;
  QString name;

  //Extra widgets:
  QTreeWidgetItem* objBrowseTree;

  // N.B. Material button defined in children.
};



//____________________________________________________________________
AODCollHandleBase::AODCollHandleBase( AODSysCommonData * cd, const QString& name, xAOD::Type::ObjectType type	)
  //AODCollHandleBase::AODCollHandleBase( AODSysCommonData * cd, const QString& name)
  : VP1StdCollection(cd->system(),"AODCollHandleBase_FIXME_"+name), d(new Imp), // Need to add back ObjectType once simple way to create string is added to xAODBase
  m_nshownhandles(0),
  m_type(type),
  m_commonData(cd),
  m_sephelper(0)
{
  d->theclass = this;
  d->name = name;
  d->objBrowseTree = 0;
}

// //____________________________________________________________________
// void AODCollHandleBase::init(VP1MaterialButtonBase*)
// {
//     // d->matButton = new TrackCollectionSettingsButton;
//     // d->matButton->setText(text());
//     // VP1StdCollection::init(d->matButton);//this call is required. Passing in TrackCollectionSettingsButton means we have the more complex button. 
//   VP1StdCollection::init();//FIXME
//   setupSettingsFromController(common()->controller());
//   connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(collVisibilityChanged(bool)));
// 	
//   // collSwitch()->addChild(d->matButton->trackDrawStyle());
// }

//____________________________________________________________________
AODCollHandleBase::~AODCollHandleBase()
{
  messageVerbose("destructor start");


  // delete the Imp instance
  delete d;

  if (m_sephelper) {
    SoSeparator * sep = m_sephelper->topSeparator();
    sep->ref();
    delete m_sephelper;
    sep->unref();
  }

  messageVerbose("destructor end");
}

//____________________________________________________________________
void AODCollHandleBase::setupSettingsFromController(AODSystemController* controller)
{
  messageVerbose("setupSettingsFromController start");
  if (!controller) {
    message("Not properly initialized: controller pointer is zero.");
    return;
  }

  largeChangesBegin();
  // connect(common()->system(),SIGNAL(newHandleSelected( const AODHandleBase&)),this,SLOT(handleSelectionChanged()));
  setupSettingsFromControllerSpecific(controller);

  largeChangesEnd();
  messageVerbose("setupSettingsFromController end");
}


//____________________________________________________________________
QString AODCollHandleBase::name() const
{
  return d->name;
}


//____________________________________________________________________
void AODCollHandleBase::recheckCutStatus(AODHandleBase* handle)
{
  messageVerbose("AODCollHandleBase::recheckCutStatus() & visible="+str(visible()));
  handle->setVisible( visible() && cut(handle));
}

//____________________________________________________________________
void AODCollHandleBase::recheckCutStatusOfAllVisibleHandles()
{
  messageVerbose("AODCollHandleBase::recheckCutStatusOfAllVisibleHandles");

  if (!isLoaded())
    return;

  //This method is called when a cut is tightened - thus we better start by deselectAll to avoid weird highlighting issues.
  common()->system()->deselectAll();

  largeChangesBegin();
  handleIterationBegin();
  AODHandleBase* handle=0;
  while ((handle=getNextHandle()))
  {
    if (handle->visible())
      recheckCutStatus(handle);
  }
  // handle=getNextHandle();
  // recheckCutStatus(handle);
  
  // std::vector<AODHandleBase*>::iterator it(d->handles.begin()),itE(d->handles.end());
  // for (;it!=itE;++it) {
  //   if ((*it)->visible())
  //     recheckCutStatus(*it);
  // }
  updateObjectBrowserVisibilityCounts();
  largeChangesEnd();

  message("recheckCutStatusOfAllVisibleHandles:  "+str(nShownHandles())+"/"+str(getHandlesList().count())+" pass cuts");
}

//____________________________________________________________________
void AODCollHandleBase::recheckCutStatusOfAllNotVisibleHandles()
{
  messageVerbose("AODCollHandleBase::recheckCutStatusOfAllNotVisibleHandles");

  if (!isLoaded()){
    messageVerbose("AODCollHandleBase::recheckCutStatusOfAllNotVisibleHandles - not yet loaded. Aborting.");
    return;    
  }

  largeChangesBegin();
  AODHandleBase* handle=0;
  // unsigned int i=0;
  handleIterationBegin();
  while ((handle=getNextHandle()))
  {
    // std::cout<<"Looking at handle "<<++i<<" with visible="<<handle->visible()<<std::endl;
    if (!handle->visible())
      recheckCutStatus(handle);
  }
  updateObjectBrowserVisibilityCounts();
  largeChangesEnd();

  message("recheckCutStatusOfAllNotVisibleHandles:  "+str(nShownHandles())+"/"+str(getHandlesList().count())+" pass cuts");
}

//____________________________________________________________________
void AODCollHandleBase::recheckCutStatusOfAllHandles()
{
  messageVerbose("AODCollHandleBase::recheckCutStatusOfAllHandles()");
  if (!isLoaded())
    return;
  largeChangesBegin();
  handleIterationBegin();
  AODHandleBase* handle=0;
  while ((handle=getNextHandle()))
  {
    if (handle->visible())
      recheckCutStatus(handle);
  }
  updateObjectBrowserVisibilityCounts();
  largeChangesEnd();

  message("recheckCutStatusOfAllHandles:  "+str(nShownHandles())+"/"+str(getHandlesList().count())+" pass cuts");
}

//____________________________________________________________________
void AODCollHandleBase::update3DObjectsOfAllHandles()
{
  if (!isLoaded())
    return;
  if (verbose())
    messageVerbose("update3DObjectsOfAllHandles start");
  largeChangesBegin();
  handleIterationBegin();
  AODHandleBase* handle=0;
  while ((handle=getNextHandle()))
    handle->update3DObjects();
  
  largeChangesEnd();
  messageVerbose("update3DObjectsOfAllHandles end");
}

//____________________________________________________________________
void AODCollHandleBase::updateMaterialOfAllHandles()
{
  if (!isLoaded())
    return;
  messageVerbose("updateMaterialOfAllHandles start");
  largeChangesBegin();
  handleIterationBegin();
  AODHandleBase* handle=0;
  while ((handle=getNextHandle()))
    handle->updateMaterial();
  largeChangesEnd();
  messageVerbose("updateMaterialOfAllHandles end");
}



// //____________________________________________________________________
// void AODCollHandleBase::setLabels( AODSystemController::TrackLabelModes labels )
// {
//   // messageVerbose("setLabels called");
//   // if (m_labels==labels)
//   //   return;
//   // messageVerbose("setLabels ==> Changed");
//   // m_labels = labels;
//   // update3DObjectsOfAllHandles();
// }
// 
// //____________________________________________________________________
// void AODCollHandleBase::setLabelTrkOffset( float offset)
// {
//   // messageVerbose("setLabelTrkOffset called");
//   // if (m_labelsTrkOffset==offset)
//   //   return;
//   // messageVerbose("setLabelTrkOffset ==> Changed to "+QString::number(offset));
//   // m_labelsTrkOffset = offset;
//   // update3DObjectsOfAllHandles();
// }
// 
// //____________________________________________________________________
// void AODCollHandleBase::setLabelPosOffsets( QList<int> offsets)
// {
//   // messageVerbose("setLabelPosOffsets called");
//   // if (m_labelsPosOffsets==offsets)
//   //   return;
//   // messageVerbose("setLabelPosOffsets ==> Changed");
//   // m_labelsPosOffsets = offsets;
//   // update3DObjectsOfAllHandles();
// }

// //____________________________________________________________________
// void AODCollHandleBase::setColourBy( AODCollHandleBase::COLOURBY cb )
// {
//   // messageVerbose("setColourBy called");
//   // if (m_colourby==cb)
//   //   return;
//   // messageVerbose("setColourBy ==> Changed");
//   // m_colourby=cb;
//   // 
//   // //Update gui combobox:
//   // QString targetText;
//   // switch(cb) {
//   // case COLOUR_BYPID:
//   //   targetText = Imp::comboBoxEntry_ColourByPID();
//   //   break;
//   // case COLOUR_RANDOM:
//   //   targetText = Imp::comboBoxEntry_ColourByRandom();
//   //   break;
//   // case COLOUR_MOMENTUM:
//   //   targetText = Imp::comboBoxEntry_ColourByMomentum();
//   //   break;
//   // case COLOUR_CHARGE:
//   //   targetText = Imp::comboBoxEntry_ColourByCharge();
//   //   break;
//   // case COLOUR_DISTANCE:
//   //   targetText = Imp::comboBoxEntry_ColourByDistanceFromSelectedTrack();
//   //   break;
//   // case COLOUR_VERTEX:
//   //   targetText = Imp::comboBoxEntry_ColourByVertex();
//   //   break;
//   // default:
//   // case COLOUR_PERCOLLECTION:
//   //   targetText = Imp::comboBoxEntry_ColourByCollection();
//   //   break;
//   // }
//   // if (targetText!=d->comboBox_colourby->currentText()) {
//   //   int i = d->comboBox_colourby->findText(targetText);
//   //   if (i>=0&&i<d->comboBox_colourby->count()) {
//   //     bool save = d->comboBox_colourby->blockSignals(true);
//   //     d->comboBox_colourby->setCurrentIndex(i);
//   //     d->comboBox_colourby->blockSignals(save);
//   //   } else {
//   //     message("ERROR: Problems finding correct text in combo box");
//   //   }
//   // }
//   // 
//   // //Actual material updates:
//   // largeChangesBegin();
//   // m_commonData->system()->deselectAll();//Todo: Reselect the selected track handles
//   //                                       //afterwards (collhandles should know selected handles)
//   // updateMaterialOfAllHandles();
//   // largeChangesEnd();
// 
// }
// 
// //____________________________________________________________________
// void AODCollHandleBase::rerandomiseRandomTrackColours()
// {
//   // if (!isLoaded())
//   //   return;
//   // messageVerbose("rerandomiseRandomTrackColours start");
//   // largeChangesBegin();
//   // std::vector<AODHandleBase*>::iterator it(d->handles.begin()),itE(d->handles.end());
//   // for (;it!=itE;++it)
//   //   (*it)->rerandomiseRandomMaterial();
//   // largeChangesEnd();
//   // messageVerbose("rerandomiseRandomTrackColours end");
// }
// 
// //____________________________________________________________________
// void AODCollHandleBase::handleSelectionChanged()
// {
//   // if (!isLoaded() || colourBy()!= COLOUR_DISTANCE)
//   //   return;
//   // messageVerbose("handleSelectionChanged start");
//   // largeChangesBegin();
//   // std::vector<AODHandleBase*>::iterator it(d->handles.begin()),itE(d->handles.end());
//   // for (;it!=itE;++it)
//   //   (*it)->updateMaterial();
//   // largeChangesEnd();
//   // messageVerbose("handleSelectionChanged end");
// }



//____________________________________________________________________
qint32 AODCollHandleBase::provideCollTypeID() const
{
  return m_type; // This is the xAOD::Type::ObjectType value
}

QString AODCollHandleBase::provideSection() const {

  std::stringstream ss;
  ss << m_type;
  QString section = QString::fromStdString(ss.str());
  return section;
}

QString AODCollHandleBase::provideSectionToolTip() const {
  return QString("TODO!");
}

//____________________________________________________________________
void AODCollHandleBase::collVisibilityChanged(bool vis)
{
  if (verbose())
    messageVerbose("AODCollHandleBase::collVisibilityChanged => "+str(vis));

  if (!m_sephelper)
    m_sephelper = new VP1ExtraSepLayerHelper(collSep());

  if (!d->objBrowseTree)
    fillObjectBrowser();
  
  if (vis){
    recheckCutStatusOfAllNotVisibleHandles();//Fixme -> ofallhandles? All must be not visible anyway...
    if (d->objBrowseTree) d->objBrowseTree->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled); //  selectable,  enabled
  }else{
    recheckCutStatusOfAllVisibleHandles();
    //    QTreeWidget* trkObjBrowser = common()->controller()->trackObjBrowser();
    //    if (d->objBrowseTree && trkObjBrowser) {
    //      trkObjBrowser->takeTopLevelItem(trkObjBrowser->indexOfTopLevelItem(d->objBrowseTree));
    //      delete d->objBrowseTree; d->objBrowseTree=0;
    //    }
    // FIXME - need to loop through handles setting pointers to deleted QTreeWidgetItems
    if (d->objBrowseTree) d->objBrowseTree->setFlags(0); // not selectable, not enabled
  }
} 

void AODCollHandleBase::updateObjectBrowserVisibilityCounts() {
  messageVerbose("AODCollHandleBase::updateObjectBrowserVisibilityCounts called for "+name());
  QTreeWidget* trkObjBrowser = common()->controller()->objBrowser();
  if (!trkObjBrowser || !d->objBrowseTree) {
    messageVerbose("AODCollHandleBase::updateObjectBrowserVisibilityCounts: no common()->controller()->objBrowser() and/or d->objBrowseTree. Aborting");
    messageVerbose("trkObjBrowser: "+str(trkObjBrowser)+"\t d->objBrowseTree: "+str(d->objBrowseTree));
    return;
  }
  QString text(QString(": (")+QString::number(nShownHandles())+QString("/")+QString::number(getHandlesList().count())+QString(") visible"));
  d->objBrowseTree->setText(1, text);
}

void AODCollHandleBase::fillObjectBrowser()
{
  QTime t;
  t.start();
  messageVerbose("AODCollHandleBase::fillObjectBrowser called for "+name());

  QTreeWidget* trkObjBrowser = common()->controller()->objBrowser();
  if (!trkObjBrowser) {
    messageVerbose("AODCollHandleBase::fillObjectBrowser: no common()->controller()->objBrowser(). Aborting");
    return;
  }
  // if (!nShownHandles()) {
  //   messageVerbose("AODCollHandleBase::fillObjectBrowser: no shown handles, so leaving.");
  //   return; // don't bother with hidden collection
  // }

  trkObjBrowser->setUpdatesEnabled(false);

  bool firstTime=false;
  if (!d->objBrowseTree) {
    d->objBrowseTree = new QTreeWidgetItem(0);
    firstTime=true;
    messageVerbose("AODCollHandleBase::fillObjectBrowser: First time so creating QTreeWidgetItem.");
  } else {
    int index = trkObjBrowser->indexOfTopLevelItem(d->objBrowseTree);
    if (index==-1 ) {
      messageVerbose("Missing from WidgetTree! Will continue but something must be wrong");
    }
  }

  messageVerbose("AODCollHandleBase::fillObjectBrowser about to start looping over handles at "+QString::number(t.elapsed())+" ms");

  QList<QTreeWidgetItem *> list;
  handleIterationBegin();
  AODHandleBase* handle=0;
  unsigned int i=0;
  unsigned int numVisible=0;
  while ((handle=getNextHandle()))
  {
    if (firstTime){
      handle->fillObjectBrowser(list);
    } else {
      handle->updateObjectBrowser();
    }

    // messageVerbose("AODCollHandleBase::fillObjectBrowser for handle completed in "+QString::number(t.elapsed()));

    if (handle->visible() ) numVisible++;
    i++;
  }

  QString text(QString(": (")+QString::number(numVisible)+QString("/")+QString::number(i)+QString(") visible"));

  d->objBrowseTree->setText(0, name());
  d->objBrowseTree->setText(1, text);
  d->objBrowseTree->addChildren(list);
  trkObjBrowser->addTopLevelItem(d->objBrowseTree);
  trkObjBrowser->setUpdatesEnabled(true);

  messageVerbose("AODCollHandleBase::fillObjectBrowser completed in "+QString::number(t.elapsed())+" ms");

}

//____________________________________________________________________
void AODCollHandleBase::assignDefaultMaterial(SoMaterial * m) const
{
	messageDebug("AODCollHandleBase::assignDefaultMaterial()");
  VP1QtInventorUtils::setMatColor( m, defaultColor(), 0.18/*brightness*/ );
}


//____________________________________________________________________
QList<QWidget*> AODCollHandleBase::provideExtraWidgetsForGuiRow() const
{
  // return QList<QWidget*>() << d->comboBox_colourby;
  return QList<QWidget*>();
}

//____________________________________________________________________
QByteArray AODCollHandleBase::extraWidgetsState() const
{
  VP1Serialise serialise(0/*version*/,systemBase());
  // serialise.save(d->comboBox_colourby);
  // serialise.disableUnsavedChecks();
  return serialise.result();
}

//____________________________________________________________________
void AODCollHandleBase::setExtraWidgetsState(const QByteArray& ba)
{
  messageDebug(" AODCollHandleBase::setExtraWidgetsState() - ba: " + ba);

  if (ba.isEmpty())
    messageVerbose("ExtraWidgetState ByteArray is empty.");

  // VP1Deserialise state(ba, systemBase());
  // if (state.version()!=0)
  //   return;//just ignore silently... i guess we ought to warn?
  // state.restore(d->comboBox_colourby);
  // state.disableUnrestoredChecks();
  // colourByComboBoxItemChanged();
}


//____________________________________________________________________
void AODCollHandleBase::colourByComboBoxItemChanged()
{
  messageVerbose("AODCollHandleBase::colourByComboBoxItemChanged()");
  messageVerbose("Collection detail level combo box changed index");

  messageVerbose("TO BE IMPLEMENTED!!!");
  /*
    if (d->comboBox_colourby->currentText()==Imp::comboBoxEntry_ColourByRandom())
      setColourBy(COLOUR_RANDOM);
    else
      setColourBy(COLOUR_PERCOLLECTION);
  */
}

//____________________________________________________________________
void AODCollHandleBase::setState(const QByteArray&state)
{
  VP1Deserialise des(state);
  des.disableUnrestoredChecks();
  if (des.version()!=0&&des.version()!=1) {
    messageDebug("Warning: Ignoring state with wrong version");
    return;
  }
  bool vis = des.restoreBool();

  QByteArray matState = des.restoreByteArray();
  // d->matButton->restoreFromState(matState);
  QByteArray extraWidgetState = des.version()>=1 ? des.restoreByteArray() : QByteArray();
  setVisible(vis);

  if (extraWidgetState!=QByteArray())
    setExtraWidgetsState(extraWidgetState);
}

//____________________________________________________________________
QByteArray AODCollHandleBase::persistifiableState() const
{
  // if (!d->matButton) {
  //   message("ERROR: persistifiableState() called before init()");
  //   return QByteArray();
  // }
  VP1Serialise serialise(1/*version*/);
  serialise.disableUnsavedChecks();
  serialise.save(visible());
  // Q_ASSERT(d->matButton&&"Did you forget to call init() on this VP1StdCollection?");
  // serialise.save(d->matButton->saveState());
  serialise.save(extraWidgetsState());//version 1+
  return serialise.result();
}


