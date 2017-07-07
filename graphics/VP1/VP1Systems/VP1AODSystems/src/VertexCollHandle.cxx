/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VertexCollHandle                  //
//                                                            //
//  Author: edward.moyse@cern.ch                              //
//  Initial version: June 2015                                //
//                                                            //
////////////////////////////////////////////////////////////////

//Local includes
#include "VertexHandle.h"
#include "VertexCollHandle.h"
#include "AODSysCommonData.h"
#include "VertexCollectionSettingsButton.h"
#include "VP1AODSystems/VP1AODSystem.h"
#include "VP1AODSystems/AODSystemController.h"

//VP1 base includes
#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Utils/VP1LinAlgUtils.h"
#include "VP1Base/IVP13DSystem.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Utils/VP1SGContentsHelper.h"

//SoCoin
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSwitch.h>
#include "Inventor/nodes/SoDrawStyle.h"
#include "Inventor/nodes/SoLightModel.h"

//Athena
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

//AOD
#include "xAODTracking/VertexContainer.h"

//Qt
#include <QComboBox>
#include <QTreeWidgetItem>
#include <qdatetime.h>
#include <vector>
#include <QString>

//____________________________________________________________________
class VertexCollHandle::Imp {
public:
  VertexCollHandle * theclass;

  //Vector of iParticle handles:
  std::vector<VertexHandle*> handles;
  // QList of IParticle handles
  QList<AODHandleBase*> handlesList;

  //For iteration:
  std::vector<VertexHandle*>::iterator itHandles;
  std::vector<VertexHandle*>::iterator itHandlesEnd;

  //Extra widgets;
  VertexCollectionSettingsButton* collSettingsButton;
  
  // int updateGUICounter;
  //
  // void possiblyUpdateGUI() {//Fixme: to IParticleCollHandleBase
  //   if (!((updateGUICounter++)%50)) {
  //     theclass->systemBase()->updateGUI();
  //   }
  // }
};



//____________________________________________________________________
VertexCollHandle::VertexCollHandle( AODSysCommonData * cd, const QString& name, xAOD::Type::ObjectType type)
  : AODCollHandleBase(cd, name, type),  
    d(new Imp), // Need to add back ObjectType once simple way to create string is added to xAODBase
    m_nshownhandles(0),
    m_cut_allowedY(VP1Interval(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity())),
    m_cut_allowedR(VP1Interval(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity())),
    m_cut_allowedZ(VP1Interval(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity())),
    m_cut_r_allowall(true),
    m_cut_z_allowall(true),
    m_cut_rz_allwillfail(false)
{
  d->theclass = this;

  //The object names should not contain all sorts of funky chars (mat button style sheets wont work for instance):
  QString safetext(text());
  safetext.replace(' ','_');
  safetext.replace('[','_');
  safetext.replace(']','_');
  safetext.replace('/','_');
  safetext.replace('.','_');
  safetext.replace(',','_');
  safetext.replace('<','_');
  safetext.replace('>','_');
  safetext.replace('&','_');
}

//____________________________________________________________________
VertexCollHandle::~VertexCollHandle()
{
  messageVerbose("destructor start");

  // clean the vector<handle>
  cleanupPtrContainer(d->handles);

  // clean the QList<handle>
  foreach(AODHandleBase* handle, d->handlesList) {
    delete handle;
  }

  // delete the Imp instance
  delete d;

  messageVerbose("destructor end");
}

//____________________________________________________________________
QStringList VertexCollHandle::availableCollections( IVP1System*sys )
{
  return VP1SGContentsHelper(sys).getKeys<xAOD::VertexContainer>();
}


//____________________________________________________________________
void VertexCollHandle::init(VP1MaterialButtonBase*)
{
  d->collSettingsButton = new VertexCollectionSettingsButton;
  d->collSettingsButton->setMaterialText(name());
  VP1StdCollection::init(d->collSettingsButton);//this call is required. Passing in d->collSettingsButton means we have the more complex button. 
  setupSettingsFromController(common()->controller());
  connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(collVisibilityChanged(bool)));
  
  collSwitch()->addChild(d->collSettingsButton->vertexLightModel());
  collSwitch()->addChild(d->collSettingsButton->vertexDrawStyle());
}

void VertexCollHandle::setupSettingsFromControllerSpecific(AODSystemController*) {
  //cuts
  // R
  connect(d->collSettingsButton,SIGNAL(cutAllowedRChanged(const VP1Interval&)),this,SLOT(setCutAllowedR(const VP1Interval&)));
  setCutAllowedR(d->collSettingsButton->cutAllowedR());
  // Y
  connect(d->collSettingsButton,SIGNAL(cutAllowedYChanged(const VP1Interval&)),this,SLOT(setCutAllowedY(const VP1Interval&)));
  setCutAllowedY(d->collSettingsButton->cutAllowedY());
  // Z
  connect(d->collSettingsButton,SIGNAL(cutAllowedZChanged(const VP1Interval&)),this,SLOT(setCutAllowedZ(const VP1Interval&)));
  setCutAllowedZ(d->collSettingsButton->cutAllowedZ());
  // size
  connect(d->collSettingsButton,SIGNAL(vertexSizeChanged(int)),this,SLOT(setVertexSize(int)));
  setVertexSize(d->collSettingsButton->vertexSize());
}

void VertexCollHandle::resetCachedValuesCuts()
{
	// TODO: it is not used so far! Check Other collections and update accordingly

	// kinetic cuts
	setCutAllowedR(d->collSettingsButton->cutAllowedR());
	setCutAllowedY(d->collSettingsButton->cutAllowedY());
	setCutAllowedZ(d->collSettingsButton->cutAllowedZ());
	// other settings
	setVertexSize(d->collSettingsButton->vertexSize());
}


const VertexCollectionSettingsButton& VertexCollHandle::collSettingsButton() const {
  if (!d->collSettingsButton){
    messageVerbose("No collSettingsButton set! Can't call init(), so crash is imminent...");
  }
  return *d->collSettingsButton;
}

//____________________________________________________________________
bool VertexCollHandle::load()
{
  messageVerbose("loading Vertex collection");

  //Get collection:
  const xAOD::VertexContainer * coll(0);
  if (!VP1SGAccessHelper(systemBase()).retrieve(coll, name())) {
    message("Error: Could not retrieve vertex collection with key="+name());
    return false;
  }

  // // Retrieve the xAOD particles:
  //  const xAOD::VertexContainer* xaod = evtStore()->retrieve<const xAOD::VertexContainer>( m_VertexCollection );

  // Check that the auxiliary store association was made successfully:
  if( ! coll->hasStore() ) {
    message("No auxiliary store got associated to the Vertex container with key: " +name());
  }

  // This is needed for now, until the issues around the DV code are
  // sorted out...
  const_cast< xAOD::VertexContainer* >( coll )->setStore(
    ( SG::IAuxStore* ) coll->getConstStore() );

  //Make appropriate vertex handles:
  xAOD::VertexContainer::const_iterator it, itEnd = coll->end();
  for ( it = coll->begin() ; it != itEnd; ++it) {
    // d->possiblyUpdateGUI(); FIXME
    systemBase()->updateGUI();
    if (!*it) {
      messageDebug("WARNING: Ignoring null Vertex pointer.");
      continue;
    }
    addHandle(new VertexHandle(this,*it));
  }

  return true;
}


//____________________________________________________________________
QList<AODHandleBase*> VertexCollHandle::getHandlesList() const
{
  messageVerbose("VertexCollHandle::getHandlesList()");
  return d->handlesList;
}

//____________________________________________________________________
bool VertexCollHandle::cut(AODHandleBase* ah)
{
  messageVerbose("VertexCollHandle::cut()");

  VertexHandle* handle = dynamic_cast<VertexHandle*>(ah);
  if (!handle) {
    messageVerbose("VertexCollHandle::cut() - passing in handle of wrong type! Aborting.");
    return false;
  }

  if (m_cut_rz_allwillfail){
    messageVerbose("VertexCollHandle::cut() - returning false as m_cut_rz_allwillfail is set.");
    return false;
  }

  // FIXME
  float vertexR = 0.0;
  //float vertexZ = 0.0;

  float vertexX = handle->getPositionX();
  float vertexY = handle->getPositionY();
  float vertexZ = handle->getPositionZ();
  
  messageVerbose("Vertex R: " + QString::number(vertexR));
  messageVerbose("Vertex x, y, z: " + QString::number(vertexX) + ", " + QString::number(vertexY)+ ", " + QString::number(vertexZ));



  if (!m_cut_r_allowall || !m_cut_y_allowall || !m_cut_z_allowall)
  {
    messageVerbose("evaluating cuts...");
    
    // *** r CUT ***
    messageVerbose("vertexR: " + QString::number(vertexR) + " - m_cut_allowedR: " + m_cut_allowedR.toString() + " - excludeInterval: " + QString::number(m_cut_allowedR.excludeInterval()) );
    if (!m_cut_r_allowall && !m_cut_allowedR.contains(vertexR)){ 
      messageVerbose("r cut not passed...");
      return false;
    }

    // *** Y CUT ***
    messageVerbose("m_cut_allowedY: " + m_cut_allowedY.toString() + " - excludeInterval: " + QString::number(m_cut_allowedY.excludeInterval()) );
    if (!m_cut_y_allowall && !m_cut_allowedY.contains(vertexY)){ 
      messageVerbose("Y cut not passed...");
      return false;
    }

    // *** Z CUT ***
    messageVerbose("m_cut_allowedZ: " + m_cut_allowedZ.toString() + " - excludeInterval: " + QString::number(m_cut_allowedZ.excludeInterval()) );
    if (!m_cut_z_allowall && !m_cut_allowedZ.contains(vertexZ)){ 
      messageVerbose("Z cut not passed...");
      return false;
    }
  }

  messageVerbose("cut() - Returning true!");
  return true;
}

//____________________________________________________________________
void VertexCollHandle::hintNumberOfHandlesInEvent(unsigned n)
{
  d->handles.reserve(n);
}

//____________________________________________________________________
void VertexCollHandle::addHandle(AODHandleBase* ah)
{
  VertexHandle* handle = dynamic_cast<VertexHandle* >(ah);
  if (!handle) {
    message("ERROR - wrong handle type passed to VertexCollHandle::addHandle!");
    return;
  }
  d->handles.push_back(handle); // for the vector<handle>
  d->handlesList << handle; // for the QList<handle>
}

//____________________________________________________________________
void VertexCollHandle::handleIterationBegin()
{
  d->itHandles = d->handles.begin();
  d->itHandlesEnd = d->handles.end();
}

//____________________________________________________________________
AODHandleBase* VertexCollHandle::getNextHandle() {
  if (d->itHandles==d->itHandlesEnd)
    return 0;
  else
    return *(d->itHandles++);
}

//____________________________________________________________________
void VertexCollHandle::setCutAllowedR(const VP1Interval& allowedR)
{
  
  messageVerbose("signal received in setCutAllowedR ("+allowedR.toString()+") with m_cut_allowedR = "+m_cut_allowedR.toString());
  if (!allowedR.isSane())
    return;

  if (m_cut_allowedR==allowedR)
    return;
  
  m_cut_r_allowall = allowedR.isAllR();
  m_cut_rz_allwillfail = allowedR.isEmpty() || m_cut_allowedR.isEmpty();

  std::cout<<"m_cut_rz_allwillfail= "<<m_cut_rz_allwillfail<< " because allowedR.isEmpty()="<<allowedR.isEmpty()<<" || m_cut_allowedR.isEmpty() = "<<m_cut_allowedR.isEmpty()<<std::endl;

  if (!m_cut_allowedR.contains(allowedR)&&!allowedR.contains(m_cut_allowedR)) {
    m_cut_allowedR = allowedR;
    recheckCutStatusOfAllHandles();
    return;
  }
  bool relaxcut = allowedR.contains(m_cut_allowedR);
  m_cut_allowedR = allowedR;
  if (relaxcut)
    recheckCutStatusOfAllNotVisibleHandles();
  else
    recheckCutStatusOfAllVisibleHandles();
}

//____________________________________________________________________
void VertexCollHandle::setCutAllowedY(const VP1Interval& allowedY)
{
  messageVerbose("signal received in setCutAllowedY ("+allowedY.toString()+")");
  if (!allowedY.isSane())
    return;

  if (m_cut_allowedY==allowedY)
    return;
  m_cut_y_allowall = allowedY.isAllR();
  m_cut_ry_allwillfail = allowedY.isEmpty() || m_cut_allowedY.isEmpty();

  if (!m_cut_allowedY.contains(allowedY) && !allowedY.contains(m_cut_allowedY)) {
    m_cut_allowedY = allowedY;
    recheckCutStatusOfAllHandles();
    return;
  }
  bool relaxcut = allowedY.contains(m_cut_allowedY);
  m_cut_allowedY = allowedY;
  if (relaxcut)
    recheckCutStatusOfAllNotVisibleHandles();
  else
    recheckCutStatusOfAllVisibleHandles();
}
//____________________________________________________________________
void VertexCollHandle::setCutAllowedZ(const VP1Interval& allowedZ)
{
  messageVerbose("signal received in setCutAllowedZ ("+allowedZ.toString()+")");
  if (!allowedZ.isSane())
    return;

  if (m_cut_allowedZ==allowedZ)
    return;
  m_cut_z_allowall = allowedZ.isAllR();
  m_cut_rz_allwillfail = allowedZ.isEmpty() || m_cut_allowedZ.isEmpty();

  if (!m_cut_allowedZ.contains(allowedZ)&&!allowedZ.contains(m_cut_allowedZ)) {
    m_cut_allowedZ = allowedZ;
    recheckCutStatusOfAllHandles();
    return;
  }
  bool relaxcut = allowedZ.contains(m_cut_allowedZ);
  m_cut_allowedZ = allowedZ;
  if (relaxcut)
    recheckCutStatusOfAllNotVisibleHandles();
  else
    recheckCutStatusOfAllVisibleHandles();
}

void VertexCollHandle::setVertexSize(int size)
{
  messageVerbose("signal received in vertexSizeChanged ("+str(size)+")");

  update3DObjectsOfAllHandles();
}

//____________________________________________________________________
void VertexCollHandle::setState(const QByteArray&state)
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
QByteArray VertexCollHandle::persistifiableState() const
{
  messageDebug("VertexCollHandle::persistifiableState() - start...");

  // if (!d->matButton) {
  //   message("ERROR: persistifiableState() called before init()");
  //   return QByteArray();
  // }
  VP1Serialise serialise(1/*version*/);
  serialise.disableUnsavedChecks();

  // SAVE THE CHECKED/UNCHECKED STATUS OF THE COLLECTION
  serialise.save(visible());

  // SAVE THE MATERIAL BUTTON
  // Q_ASSERT(d->matButton&&"Did you forget to call init() on this VP1StdCollection?");
  // serialise.save(d->matButton->saveState());

  // SAVE THE EXTRA-STATES
  serialise.save(extraWidgetsState());//version 1+

  messageDebug("VertexCollHandle::persistifiableState() - end.");
  return serialise.result();
}
