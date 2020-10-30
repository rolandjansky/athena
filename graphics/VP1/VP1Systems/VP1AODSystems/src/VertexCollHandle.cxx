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
#ifndef BUILDVP1LIGHT
  #include "VP1Utils/VP1SGAccessHelper.h"
  #include "VP1Utils/VP1SGContentsHelper.h"
#endif

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
#include <QDebug>

#ifdef BUILDVP1LIGHT
  #include <QSettings>
  #include "xAODRootAccess/Init.h"
  #include "xAODRootAccess/TEvent.h"
#endif //  BUILDVP1LIGHT


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
    m_d(new Imp), // Need to add back ObjectType once simple way to create string is added to xAODBase
    m_nshownhandles(0),
    m_cut_allowedY(VP1Interval(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity())),
    m_cut_allowedR(VP1Interval(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity())),
    m_cut_allowedZ(VP1Interval(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity())),
    m_cut_r_allowall(true),
    m_cut_z_allowall(true),
    m_cut_rz_allwillfail(false)
{
  m_d->theclass = this;

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
  #ifndef BUILDVP1LIGHT
    cleanupPtrContainer(m_d->handles);
  #endif

  // delete the Imp instance
  delete m_d;

  messageVerbose("destructor end");
}

//____________________________________________________________________
#if defined BUILDVP1LIGHT
  QStringList VertexCollHandle::availableCollections( IVP1System*sys )
  {
      return sys->getObjectList(xAOD::Type::Vertex);
  }
#else
  QStringList VertexCollHandle::availableCollections( IVP1System*sys )
  {
    return VP1SGContentsHelper(sys).getKeys<xAOD::VertexContainer>();
  }
#endif // BUILDVP1LIGHT


//____________________________________________________________________
void VertexCollHandle::init(VP1MaterialButtonBase*)
{
  m_d->collSettingsButton = new VertexCollectionSettingsButton;
  m_d->collSettingsButton->setMaterialText(name());
  VP1StdCollection::init(m_d->collSettingsButton);//this call is required. Passing in m_d->collSettingsButton means we have the more complex button. 
  setupSettingsFromController(common()->controller());
  connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(collVisibilityChanged(bool)));
  
  collSwitch()->addChild(m_d->collSettingsButton->vertexLightModel());
  collSwitch()->addChild(m_d->collSettingsButton->vertexDrawStyle());
}

void VertexCollHandle::setupSettingsFromControllerSpecific(AODSystemController*) {
  //cuts
  // R
  connect(m_d->collSettingsButton,SIGNAL(cutAllowedRChanged(const VP1Interval&)),this,SLOT(setCutAllowedR(const VP1Interval&)));
  setCutAllowedR(m_d->collSettingsButton->cutAllowedR());
  // Y
  connect(m_d->collSettingsButton,SIGNAL(cutAllowedYChanged(const VP1Interval&)),this,SLOT(setCutAllowedY(const VP1Interval&)));
  setCutAllowedY(m_d->collSettingsButton->cutAllowedY());
  // Z
  connect(m_d->collSettingsButton,SIGNAL(cutAllowedZChanged(const VP1Interval&)),this,SLOT(setCutAllowedZ(const VP1Interval&)));
  setCutAllowedZ(m_d->collSettingsButton->cutAllowedZ());
  // size
  connect(m_d->collSettingsButton,SIGNAL(vertexSizeChanged(int)),this,SLOT(setVertexSize(int)));
  setVertexSize(m_d->collSettingsButton->vertexSize());
}

void VertexCollHandle::resetCachedValuesCuts()
{
	// TODO: it is not used so far! Check Other collections and update accordingly

	// kinetic cuts
	setCutAllowedR(m_d->collSettingsButton->cutAllowedR());
	setCutAllowedY(m_d->collSettingsButton->cutAllowedY());
	setCutAllowedZ(m_d->collSettingsButton->cutAllowedZ());
	// other settings
	setVertexSize(m_d->collSettingsButton->vertexSize());
}


const VertexCollectionSettingsButton& VertexCollHandle::collSettingsButton() const {
  if (!m_d->collSettingsButton){
    messageVerbose("No collSettingsButton set! Can't call init(), so crash is imminent...");
  }
  return *m_d->collSettingsButton;
}

//____________________________________________________________________
bool VertexCollHandle::load()
{
  messageVerbose("loading Vertex collection");

  //Get collection:
  const xAOD::VertexContainer * coll(nullptr);

  #if defined BUILDVP1LIGHT
    // // Get the name of the application:
    // const char* appName = "VP1Light";

    // // Initialize the environment:
    // if( !xAOD::Init( appName ).isSuccess() ) {
    //    message("Failed to execute xAOD::Init");
    //    return false;
    // }

    // Retrieve objects from the event
    if( !(systemBase()->getEvent())->retrieve( coll, name().toStdString()).isSuccess() ) {
      message("Error: Could not retrieve collection with key="+name());
       return false;
    }
  #else
      if (!VP1SGAccessHelper(systemBase()).retrieve(coll, name())) {
        message("Error: Could not retrieve vertex collection with key="+name());
        return false;
      }
  #endif //BUILDVP1LIGHT

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
      for (auto vertex : *coll) {
        qInfo() << "  (x, y, z) =  (" << vertex->x() << ", " << vertex->y() << ", " << vertex->z() << ")"; // just to print out something
      } // end for loop over vertices

  //Make appropriate vertex handles:
  xAOD::VertexContainer::const_iterator it, itEnd = coll->end();
  for ( it = coll->begin() ; it != itEnd; ++it) {
    // m_d->possiblyUpdateGUI(); FIXME
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
  return m_d->handlesList;
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



  if ( !(m_cut_r_allowall) || !(m_cut_y_allowall) || !(m_cut_z_allowall) )
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
  m_d->handles.reserve(n);
}

//____________________________________________________________________
void VertexCollHandle::addHandle(AODHandleBase* ah)
{
  VertexHandle* handle = dynamic_cast<VertexHandle* >(ah);
  if (!handle) {
    message("ERROR - wrong handle type passed to VertexCollHandle::addHandle!");
    return;
  }
  m_d->handles.push_back(handle); // for the vector<handle>
  m_d->handlesList << handle; // for the QList<handle>
}

//____________________________________________________________________
void VertexCollHandle::handleIterationBegin()
{
  m_d->itHandles = m_d->handles.begin();
  m_d->itHandlesEnd = m_d->handles.end();
}

//____________________________________________________________________
AODHandleBase* VertexCollHandle::getNextHandle() {
  if (m_d->itHandles==m_d->itHandlesEnd)
    return 0;
  else
    return *(m_d->itHandles++);
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
  // m_d->matButton->restoreFromState(matState);
  QByteArray extraWidgetState = des.version()>=1 ? des.restoreByteArray() : QByteArray();
  setVisible(vis);

  if (extraWidgetState!=QByteArray())
    setExtraWidgetsState(extraWidgetState);
}

//____________________________________________________________________
QByteArray VertexCollHandle::persistifiableState() const
{
  messageDebug("VertexCollHandle::persistifiableState() - start...");

  // if (!m_d->matButton) {
  //   message("ERROR: persistifiableState() called before init()");
  //   return QByteArray();
  // }
  VP1Serialise serialise(1/*version*/);
  serialise.disableUnsavedChecks();

  // SAVE THE CHECKED/UNCHECKED STATUS OF THE COLLECTION
  serialise.save(visible());

  // SAVE THE MATERIAL BUTTON
  // Q_ASSERT(m_d->matButton&&"Did you forget to call init() on this VP1StdCollection?");
  // serialise.save(m_d->matButton->saveState());

  // SAVE THE EXTRA-STATES
  serialise.save(extraWidgetsState());//version 1+

  messageDebug("VertexCollHandle::persistifiableState() - end.");
  return serialise.result();
}
