/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class IParticleCollHandle_Electron      //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

//Local
#include "IParticleCollHandle_Electron.h"
#include "IParticleHandle_Electron.h"
#include "ElectronCollectionSettingsButton.h"
#include "AODSysCommonData.h"

//xAOD
#include "xAODEgamma/ElectronContainer.h"

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
  QStringList IParticleCollHandle_Electron::availableCollections( IVP1System*sys )
  {
    return sys->getObjectList(xAOD::Type::Electron);
  }
#else
  QStringList IParticleCollHandle_Electron::availableCollections( IVP1System*sys )
  {
    return VP1SGContentsHelper(sys).getKeys<xAOD::ElectronContainer>();
  }
#endif // BUILDVP1LIGHT

//____________________________________________________________________
class IParticleCollHandle_Electron::Imp {
public:
  IParticleCollHandle_Electron * theclass;
  int updateGUICounter;
  ElectronCollectionSettingsButton* collSettingsButton;
  bool shouldLoad;

  void possiblyUpdateGUI() {//Fixme: to IParticleCollHandleBase
    if (!((updateGUICounter++)%50)) {
      theclass->systemBase()->updateGUI();
    }
  }
};

//____________________________________________________________________
IParticleCollHandle_Electron::IParticleCollHandle_Electron(AODSysCommonData * cd,
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
  // mat->setName(("IParticleCollHandle_Electron"+safetext).toStdString().c_str()); // Useful for debugging
  // m_d->collSettingsButton->setDefaultParameterMaterial(m_d->defaultParametersMaterial); FIXME
}

//____________________________________________________________________
IParticleCollHandle_Electron::~IParticleCollHandle_Electron()
{
  delete m_d;
  // FIXME - check that we're not leaking handles if (!m_d->shouldLoad){

}

//____________________________________________________________________
void IParticleCollHandle_Electron::init(VP1MaterialButtonBase*)
{
  // std::cout<<"IParticleCollHandle_Electron::init 1"<<std::endl;
  m_d->collSettingsButton = new ElectronCollectionSettingsButton;
  m_d->collSettingsButton->setMaterialText(name());
  // std::cout<<"Calling VP1StdCollection::init with m_d->collSettingsButton (ElectronCollectionSettingsButton)="<<m_d->collSettingsButton<<std::endl;
  VP1StdCollection::init(m_d->collSettingsButton);//this call is required. Passing in m_d->collSettingsButton means we have the more complex button. 
  setupSettingsFromController(common()->controller());
  connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(collVisibilityChanged(bool)));

  // std::cout<<"IParticleCollHandle_Electron::init 2"<<std::endl;
  // std::cout<<"swi: "<<collSwitch()<<std::endl;
  // std::cout<<"sep: "<<collSep()<<std::endl;
  // std::cout<<"mat: "<<material()<<std::endl;
  
  collSwitch()->addChild(m_d->collSettingsButton->trackLightModel());
  collSwitch()->addChild(m_d->collSettingsButton->trackDrawStyle());
}

void IParticleCollHandle_Electron::setupSettingsFromControllerSpecific(AODSystemController*) {
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

void IParticleCollHandle_Electron::resetCachedValuesCuts()
{
	// TODO: it is not used so far! Check Other collections and update accordingly

	// kinetic cuts
	setCutAllowedPt(m_d->collSettingsButton->cutAllowedPt());
	setCutAllowedEta(m_d->collSettingsButton->cutAllowedEta());
	setCutAllowedPhi(m_d->collSettingsButton->cutAllowedPhi());

  // TODO: adding "propagation" and "parameters" settings as well??
}


const ElectronCollectionSettingsButton& IParticleCollHandle_Electron::collSettingsButton() const {
  if (!m_d->collSettingsButton){
    messageVerbose("No collSettingsButton set! Can't call init(), so crash is imminent...");
  }
  return *m_d->collSettingsButton;
}

//____________________________________________________________________
bool IParticleCollHandle_Electron::load()
{
  if (!m_d->shouldLoad){
    messageVerbose("not loading Electron collection - contains associated objects.");
    return true;
  }
  messageVerbose("loading Electron collection");

  //Get collection:
  const xAOD::ElectronContainer * coll(nullptr);

  #if defined BUILDVP1LIGHT
    // Retrieve objects from the event
    if( !(systemBase()->getEvent())->retrieve( coll, name().toStdString()).isSuccess() ) {
      QString errMsg = "Failed to retrieve " + name();
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
  //  const xAOD::ElectronContainer* xaod = evtStore()->retrieve<const xAOD::ElectronContainer>( m_ElectronCollection );

  // Check that the auxiliary store association was made successfully:
  if( ! coll->hasStore() ) {
    message("No auxiliary store got associated to the Electron container with key: " +name());
  }

  // This is needed for now, until the issues around the DV code are
  // sorted out...
  const_cast< xAOD::ElectronContainer* >( coll )->setStore(
    ( SG::IAuxStore* ) coll->getConstStore() );

  //Make appropriate trk::track handles:
  // hintNumberOfTracksInEvent(coll->size());
  xAOD::ElectronContainer::const_iterator it, itEnd = coll->end();
  for ( it = coll->begin() ; it != itEnd; ++it) {
    m_d->possiblyUpdateGUI();
    if (!*it) {
      messageDebug("WARNING: Ignoring null Electron pointer.");
      continue;
    }
    if ((*it)->charge()==0.0) {
      messageDebug("WARNING: Ignoring Electron which claims to be neutral (charge()==0.0).");
      continue;
    }
    IParticleHandle_Electron* elH = new IParticleHandle_Electron(this,*it); 
    std::cout << "elHandle ---> " << elH->electron().eta() << ", " 
              <<  elH->electron().nTrackParticles() << ", " 
              << elH->electron().trackParticle(0) << std::endl; 
    addHandle(elH);
  }

  return true;
}

//____________________________________________________________________
bool IParticleCollHandle_Electron::cut(AODHandleBase* handle)
{
	messageVerbose("IParticleCollHandle_Electron::cut()");

  if (!IParticleCollHandleBase::cut(handle))
    return false;

  //FIXME - add cuts on tracksummary etc.

  return true;
}

void IParticleCollHandle_Electron::showParametersChanged(bool val) {
  messageVerbose("IParticleCollHandle_Electron::showParametersChanged to "+str(val));
  update3DObjectsOfAllHandles();
}

void IParticleCollHandle_Electron::propagationOptionsChanged() {
  messageVerbose("IParticleCollHandle_Electron::propagationOptionsChanged");
  update3DObjectsOfAllHandles();
}

void IParticleCollHandle_Electron::dumpToJSON( std::ofstream& str) const {
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
