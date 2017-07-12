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
#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Utils/VP1SGContentsHelper.h"

//Qt
#include <QStringList>

// SoCoin
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoMaterial.h>
#include "Inventor/nodes/SoDrawStyle.h"
#include "Inventor/nodes/SoLightModel.h"

//____________________________________________________________________
QStringList IParticleCollHandle_TrackParticle::availableCollections( IVP1System*sys )
{
  return VP1SGContentsHelper(sys).getKeys<xAOD::TrackParticleContainer>();
}

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
  : IParticleCollHandleBase(cd,name,type), d(new Imp)
{
  d->theclass = this;
  d->updateGUICounter = 0;
  d->collSettingsButton=0;
  d->shouldLoad = shouldLoad;

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
  //   = d->collSettingsButton->defaultParameterMaterial();
  // mat->setName(("IParticleCollHandle_TrackParticle"+safetext).toStdString().c_str()); // Useful for debugging
  // d->collSettingsButton->setDefaultParameterMaterial(d->defaultParametersMaterial); FIXME
}

//____________________________________________________________________
IParticleCollHandle_TrackParticle::~IParticleCollHandle_TrackParticle()
{
  delete d;
  // FIXME - check that we're not leaking handles if (!d->shouldLoad){

}

//____________________________________________________________________
void IParticleCollHandle_TrackParticle::init(VP1MaterialButtonBase*)
{
  // std::cout<<"IParticleCollHandle_TrackParticle::init 1"<<std::endl;
  d->collSettingsButton = new TrackParticleCollectionSettingsButton;
  d->collSettingsButton->setMaterialText(name());
  // std::cout<<"Calling VP1StdCollection::init with d->collSettingsButton (TrackParticleCollectionSettingsButton)="<<d->collSettingsButton<<std::endl;
  VP1StdCollection::init(d->collSettingsButton);//this call is required. Passing in d->collSettingsButton means we have the more complex button. 
  setupSettingsFromController(common()->controller());
  connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(collVisibilityChanged(bool)));

  // std::cout<<"IParticleCollHandle_TrackParticle::init 2"<<std::endl;
  // std::cout<<"swi: "<<collSwitch()<<std::endl;
  // std::cout<<"sep: "<<collSep()<<std::endl;
  // std::cout<<"mat: "<<material()<<std::endl;
  
  collSwitch()->addChild(d->collSettingsButton->trackLightModel());
  collSwitch()->addChild(d->collSettingsButton->trackDrawStyle());
}

void IParticleCollHandle_TrackParticle::setupSettingsFromControllerSpecific(AODSystemController*) {
  //cuts  
  connect(d->collSettingsButton,SIGNAL(cutAllowedPtChanged(const VP1Interval&)),this,SLOT(setCutAllowedPt(const VP1Interval&)));
  setCutAllowedPt(d->collSettingsButton->cutAllowedPt());
  // 

  connect(d->collSettingsButton,SIGNAL(cutAllowedEtaChanged(const VP1Interval&)),this,SLOT(setCutAllowedEta(const VP1Interval&)));
  setCutAllowedEta(d->collSettingsButton->cutAllowedEta());
  // 

  connect(d->collSettingsButton,SIGNAL(cutAllowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(setCutAllowedPhi(const QList<VP1Interval>&)));
  setCutAllowedPhi(d->collSettingsButton->cutAllowedPhi());

  // Propagation
  connect(d->collSettingsButton,SIGNAL(propagationOptionsChanged()),        this,SLOT(propagationOptionsChanged()));

  // Parameters
  connect(d->collSettingsButton,SIGNAL(showParametersChanged(bool)),        this,SLOT(showParametersChanged(bool)));
  connect(d->collSettingsButton,SIGNAL(colourParametersByTypeChanged(bool)),this,SLOT(showParametersChanged(bool)));
  // Just reusing the same slot, since it doesn 

}

void IParticleCollHandle_TrackParticle::resetCachedValuesCuts()
{
	// TODO: it is not used so far! Check Other collections and update accordingly

	// kinetic cuts
	setCutAllowedPt(d->collSettingsButton->cutAllowedPt());
	setCutAllowedEta(d->collSettingsButton->cutAllowedEta());
	setCutAllowedPhi(d->collSettingsButton->cutAllowedPhi());

  // TODO: adding "propagation" and "parameters" settings as well??
}


const TrackParticleCollectionSettingsButton& IParticleCollHandle_TrackParticle::collSettingsButton() const {
  if (!d->collSettingsButton){
    messageVerbose("No collSettingsButton set! Can't call init(), so crash is imminent...");
  }
  return *d->collSettingsButton;
}

//____________________________________________________________________
bool IParticleCollHandle_TrackParticle::load()
{
  if (!d->shouldLoad){
    messageVerbose("not loading TrackParticle collection - contains associated objects.");
    return true;
  }
  messageVerbose("loading TrackParticle collection");

  //Get collection:
  const xAOD::TrackParticleContainer * coll(0);
  if (!VP1SGAccessHelper(systemBase()).retrieve(coll, name())) {
    message("Error: Could not retrieve track particle collection with key="+name());
    return false;
  }

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
    d->possiblyUpdateGUI();
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
