/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class IParticleCollHandle_TrackParticle //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

//Local
#include "IParticleCollHandle_TrackParticle.h"
#include "IParticleHandle_TrackParticle.h"
#include "TrackParticleCollectionSettingsButton.h"
#include "AODSysCommonData.h"

//xAOD
#include "xAODTracking/TrackParticleContainer.h"

//VP1
#include "VP1Base/IVP1System.h"
#ifndef BUILDVP1LIGHT
  #include "VP1Utils/VP1SGAccessHelper.h"
  #include "VP1Utils/VP1SGContentsHelper.h"
#endif // BUILDVP1LIGHT

//Qt
#include <QStringList>

// SoCoin
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoMaterial.h>
#include "Inventor/nodes/SoDrawStyle.h"
#include "Inventor/nodes/SoLightModel.h"

#ifdef BUILDVP1LIGHT
  #include <QSettings>
  #include "xAODRootAccess/Init.h"
  #include "xAODRootAccess/TEvent.h"
#endif // BUILDVP1LIGHT

//____________________________________________________________________
#if defined BUILDVP1LIGHT
  QStringList IParticleCollHandle_TrackParticle::availableCollections( IVP1System*sys )
  {
    return sys->getObjectList(xAOD::Type::TrackParticle);
  }
#else
  QStringList IParticleCollHandle_TrackParticle::availableCollections( IVP1System*sys )
  {
    return VP1SGContentsHelper(sys).getKeys<xAOD::TrackParticleContainer>();
  }
#endif // BUILDVP1LIGHT

//____________________________________________________________________
class IParticleCollHandle_TrackParticle::Imp {
public:
  IParticleCollHandle_TrackParticle * theclass;
  int updateGUICounter;
  TrackParticleCollectionSettingsButton* collSettingsButton;
  bool shouldLoad;

  void possiblyUpdateGUI() {//Fixme: to IParticleCollHandleBase
    if (!((updateGUICounter++)%50)) {
      theclass->systemBase()->updateGUI();
    }
  }
};

//____________________________________________________________________
IParticleCollHandle_TrackParticle::IParticleCollHandle_TrackParticle(AODSysCommonData * cd,
  const QString& name, xAOD::Type::ObjectType type, bool shouldLoad)
  : IParticleCollHandleBase(cd,name,type), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->updateGUICounter = 0;
  m_d->collSettingsButton=0;
  m_d->shouldLoad = shouldLoad;

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

  // SoMaterial* mat 
  //   = m_d->collSettingsButton->defaultParameterMaterial();
  // mat->setName(("IParticleCollHandle_TrackParticle"+safetext).toStdString().c_str()); // Useful for debugging
  // m_d->collSettingsButton->setDefaultParameterMaterial(m_d->defaultParametersMaterial); FIXME
}

//____________________________________________________________________
IParticleCollHandle_TrackParticle::~IParticleCollHandle_TrackParticle()
{
  delete m_d;
  // FIXME - check that we're not leaking handles if (!m_d->shouldLoad){

}

//____________________________________________________________________
void IParticleCollHandle_TrackParticle::init(VP1MaterialButtonBase*)
{
  // std::cout<<"IParticleCollHandle_TrackParticle::init 1"<<std::endl;
  m_d->collSettingsButton = new TrackParticleCollectionSettingsButton;
  m_d->collSettingsButton->setMaterialText(name());
  // std::cout<<"Calling VP1StdCollection::init with m_d->collSettingsButton (TrackParticleCollectionSettingsButton)="<<m_d->collSettingsButton<<std::endl;
  VP1StdCollection::init(m_d->collSettingsButton);//this call is required. Passing in m_d->collSettingsButton means we have the more complex button. 
  setupSettingsFromController(common()->controller());
  connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(collVisibilityChanged(bool)));

  // std::cout<<"IParticleCollHandle_TrackParticle::init 2"<<std::endl;
  // std::cout<<"swi: "<<collSwitch()<<std::endl;
  // std::cout<<"sep: "<<collSep()<<std::endl;
  // std::cout<<"mat: "<<material()<<std::endl;
  
  collSwitch()->addChild(m_d->collSettingsButton->trackLightModel());
  collSwitch()->addChild(m_d->collSettingsButton->trackDrawStyle());
}

void IParticleCollHandle_TrackParticle::setupSettingsFromControllerSpecific(AODSystemController*) {
  //cuts  
  connect(m_d->collSettingsButton,SIGNAL(cutAllowedPtChanged(const VP1Interval&)),this,SLOT(setCutAllowedPt(const VP1Interval&)));
  setCutAllowedPt(m_d->collSettingsButton->cutAllowedPt());
  // 

  connect(m_d->collSettingsButton,SIGNAL(cutAllowedEtaChanged(const VP1Interval&)),this,SLOT(setCutAllowedEta(const VP1Interval&)));
  setCutAllowedEta(m_d->collSettingsButton->cutAllowedEta());
  // 

  connect(m_d->collSettingsButton,SIGNAL(cutAllowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(setCutAllowedPhi(const QList<VP1Interval>&)));
  setCutAllowedPhi(m_d->collSettingsButton->cutAllowedPhi());

  // Propagation
  connect(m_d->collSettingsButton,SIGNAL(propagationOptionsChanged()),        this,SLOT(propagationOptionsChanged()));

  // Parameters
  connect(m_d->collSettingsButton,SIGNAL(showParametersChanged(bool)),        this,SLOT(showParametersChanged(bool)));
  connect(m_d->collSettingsButton,SIGNAL(colourParametersByTypeChanged(bool)),this,SLOT(showParametersChanged(bool)));
  // Just reusing the same slot, since it doesn 

}

void IParticleCollHandle_TrackParticle::resetCachedValuesCuts()
{
	// TODO: it is not used so far! Check Other collections and update accordingly

	// kinetic cuts
	setCutAllowedPt(m_d->collSettingsButton->cutAllowedPt());
	setCutAllowedEta(m_d->collSettingsButton->cutAllowedEta());
	setCutAllowedPhi(m_d->collSettingsButton->cutAllowedPhi());

  // TODO: adding "propagation" and "parameters" settings as well??
}


const TrackParticleCollectionSettingsButton& IParticleCollHandle_TrackParticle::collSettingsButton() const {
  if (!m_d->collSettingsButton){
    messageVerbose("No collSettingsButton set! Can't call init(), so crash is imminent...");
  }
  return *m_d->collSettingsButton;
}

//____________________________________________________________________
bool IParticleCollHandle_TrackParticle::load()
{
  if (!m_d->shouldLoad){
    messageVerbose("not loading TrackParticle collection - contains associated objects.");
    return true;
  }
  messageVerbose("loading TrackParticle collection");

  //Get collection:
  const xAOD::TrackParticleContainer * coll(nullptr);

  #if defined BUILDVP1LIGHT
    // Retrieve objects from the event
    if( !(systemBase()->getEvent())->retrieve( coll, name().toStdString()).isSuccess() ) {
      message("Error: Could not retrieve collection with key="+name());
       return false;
    }
  #else
    if (!VP1SGAccessHelper(systemBase()).retrieve(coll, name())) {
      message("Error: Could not retrieve track particle collection with key="+name());
      return false;
    }
  #endif // BUILDVP1LIGHT

  // // Retrieve the xAOD particles:
  //  const xAOD::TrackParticleContainer* xaod = evtStore()->retrieve<const xAOD::TrackParticleContainer>( m_TrackParticleCollection );

  // Check that the auxiliary store association was made successfully:
  if( ! coll->hasStore() ) {
    message("No auxiliary store got associated to the TrackParticle container with key: " +name());
  }

  // This is needed for now, until the issues around the DV code are
  // sorted out...
  const_cast< xAOD::TrackParticleContainer* >( coll )->setStore(
    ( SG::IAuxStore* ) coll->getConstStore() );

  //Make appropriate trk::track handles:
  // hintNumberOfTracksInEvent(coll->size());
  xAOD::TrackParticleContainer::const_iterator it, itEnd = coll->end();
  for ( it = coll->begin() ; it != itEnd; ++it) {
    m_d->possiblyUpdateGUI();
    if (!*it) {
      messageDebug("WARNING: Ignoring null TrackParticle pointer.");
      continue;
    }
    if ((*it)->charge()==0.0) {
      messageDebug("WARNING: Ignoring TrackParticle which claims to be neutral (charge()==0.0).");
      continue;
    }
    addHandle(new IParticleHandle_TrackParticle(this,*it));
  }

  return true;
}

//____________________________________________________________________
bool IParticleCollHandle_TrackParticle::cut(AODHandleBase* handle)
{
	messageVerbose("IParticleCollHandle_TrackParticle::cut()");

  if (!IParticleCollHandleBase::cut(handle))
    return false;

  //FIXME - add cuts on tracksummary etc.

  return true;
}

void IParticleCollHandle_TrackParticle::showParametersChanged(bool val) {
  messageVerbose("IParticleCollHandle_TrackParticle::showParametersChanged to "+str(val));
  update3DObjectsOfAllHandles();
}

void IParticleCollHandle_TrackParticle::propagationOptionsChanged() {
  messageVerbose("IParticleCollHandle_TrackParticle::propagationOptionsChanged");
  update3DObjectsOfAllHandles();
}

void IParticleCollHandle_TrackParticle::dumpToJSON( std::ofstream& str) const {
  str << "\""<<name().toLatin1().data()<<"\":{";
  
  unsigned int num=0;
  for (auto handle : getHandlesList() ) {
    if (handle->visible()) {
      if (num) str <<",\n";
      handle->dumpToJSON(str);
    }
  }
  
  str << "}";
}
