/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class IParticleCollHandle_Muon //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

//Local
#include "IParticleCollHandle_Muon.h"
#include "IParticleHandle_Muon.h"
#include "MuonCollectionSettingsButton.h"
#include "AODSysCommonData.h"

//xAOD
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"

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
QStringList IParticleCollHandle_Muon::availableCollections( IVP1System*sys )
{
  return VP1SGContentsHelper(sys).getKeys<xAOD::MuonContainer>();
}

//____________________________________________________________________
class IParticleCollHandle_Muon::Imp {
public:
  IParticleCollHandle_Muon * theclass;
  int updateGUICounter;
  MuonCollectionSettingsButton* collSettingsButton;

  unsigned int last_minimumQuality;

  void possiblyUpdateGUI() {//Fixme: to IParticleCollHandleBase
    if (!((updateGUICounter++)%50)) {
      theclass->systemBase()->updateGUI();
    }
  }
};

//____________________________________________________________________
IParticleCollHandle_Muon::IParticleCollHandle_Muon(AODSysCommonData * cd,
  const QString& name, xAOD::Type::ObjectType type)
  : IParticleCollHandleBase(cd,name,type), d(new Imp)
{
  d->theclass = this;
  d->updateGUICounter = 0;
  d->collSettingsButton=0;
  d->last_minimumQuality=0;

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
  // mat->setName(("IParticleCollHandle_Muon"+safetext).toStdString().c_str()); // Useful for debugging
  // d->collSettingsButton->setDefaultParameterMaterial(d->defaultParametersMaterial); FIXME
}

//____________________________________________________________________
IParticleCollHandle_Muon::~IParticleCollHandle_Muon()
{
  delete d;
}

//____________________________________________________________________
void IParticleCollHandle_Muon::init(VP1MaterialButtonBase*)
{
  // std::cout<<"IParticleCollHandle_Muon::init 1"<<std::endl;
  d->collSettingsButton = new MuonCollectionSettingsButton;
  d->collSettingsButton->setMaterialText(name());
  // std::cout<<"Calling VP1StdCollection::init with d->collSettingsButton (MuonCollectionSettingsButton)="<<d->collSettingsButton<<std::endl;
  VP1StdCollection::init(d->collSettingsButton);//this call is required. Passing in d->collSettingsButton means we have the more complex button. 
  setupSettingsFromController(common()->controller());
  connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(collVisibilityChanged(bool)));
  
  // Connections back to system
  connect(this,SIGNAL(shownAssociatedObjectsChanged(const QList<const xAOD::TrackParticle*>&)),systemBase(),SLOT(updateAssociatedObjects(const QList<const xAOD::TrackParticle*>&))) ;

  // std::cout<<"IParticleCollHandle_Muon::init 2"<<std::endl;
  // std::cout<<"swi: "<<collSwitch()<<std::endl;
  // std::cout<<"sep: "<<collSep()<<std::endl;
  // std::cout<<"mat: "<<material()<<std::endl;
  
  collSwitch()->addChild(d->collSettingsButton->trackLightModel());
  collSwitch()->addChild(d->collSettingsButton->trackDrawStyle());
}

void IParticleCollHandle_Muon::setupSettingsFromControllerSpecific(AODSystemController*) {
  //cuts  
  connect(d->collSettingsButton,SIGNAL(cutAllowedPtChanged(const VP1Interval&)),this,SLOT(setCutAllowedPt(const VP1Interval&)));
  setCutAllowedPt(d->collSettingsButton->cutAllowedPt());
  // 

  connect(d->collSettingsButton,SIGNAL(cutAllowedEtaChanged(const VP1Interval&)),this,SLOT(setCutAllowedEta(const VP1Interval&)));
  setCutAllowedEta(d->collSettingsButton->cutAllowedEta());
  // 

  connect(d->collSettingsButton,SIGNAL(cutAllowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(setCutAllowedPhi(const QList<VP1Interval>&)));
  setCutAllowedPhi(d->collSettingsButton->cutAllowedPhi());

  connect(d->collSettingsButton,SIGNAL(minimumQualityChanged(unsigned int)),this,SLOT(setMinimumQuality(unsigned int)));
  setMinimumQuality(d->collSettingsButton->minimumQuality());

  connect(d->collSettingsButton,SIGNAL(shownAssociatedObjectsChanged(MuonCollectionSettingsButton::ShownAssociatedObjects)),this,SLOT(updateShownAssociatedObjects()));
}


void IParticleCollHandle_Muon::resetCachedValuesCuts()
{
	// TODO: it is not used so far! Check Other collections and update accordingly

	// kinetic cuts
	setCutAllowedPt(d->collSettingsButton->cutAllowedPt());
	setCutAllowedEta(d->collSettingsButton->cutAllowedEta());
	setCutAllowedPhi(d->collSettingsButton->cutAllowedPhi());
	// other settings
	setMinimumQuality(d->collSettingsButton->minimumQuality());

  // TODO: adding "shownAssociatedObjectsChanged" settings as well??
}



const MuonCollectionSettingsButton& IParticleCollHandle_Muon::collSettingsButton() const {
  if (!d->collSettingsButton){
    messageVerbose("No collSettingsButton set! Can't call init(), so crash is imminent...");
  }
  return *d->collSettingsButton;
}

//____________________________________________________________________
bool IParticleCollHandle_Muon::load()
{
  messageVerbose("loading Muon collection");

  //Get collection:
  const xAOD::MuonContainer * coll(0);
  if (!VP1SGAccessHelper(systemBase()).retrieve(coll, name())) {
    message("Error: Could not retrieve Muon collection with key="+name());
    return false;
  }

  // // Retrieve the xAOD particles:
  //  const xAOD::MuonContainer* xaod = evtStore()->retrieve<const xAOD::MuonContainer>( m_MuonCollection );

  // Check that the auxiliary store association was made successfully:
  if( ! coll->hasStore() ) {
    message("No auxiliary store got associated to the Muon container with key: " +name());
  }

  // This is needed for now, until the issues around the DV code are
  // sorted out...
  const_cast< xAOD::MuonContainer* >( coll )->setStore(
    ( SG::IAuxStore* ) coll->getConstStore() );

  QList<const xAOD::TrackParticle*> listOfTrackParticles;
  //Make appropriate trk::track handles:
  // hintNumberOfTracksInEvent(coll->size());
  xAOD::MuonContainer::const_iterator it, itEnd = coll->end();
  for ( it = coll->begin() ; it != itEnd; ++it) {
    d->possiblyUpdateGUI();
    if (!*it) {
      messageDebug("WARNING: Ignoring null Muon pointer.");
      continue;
    }
    // if ((*it)->primaryTrackParticle()->charge()==0.0) {
    //   messageDebug("WARNING: Ignoring Muon which claims to be neutral (charge()==0.0).");
    //   continue;
    // }
    IParticleHandle_Muon* muonH = new IParticleHandle_Muon(this,*it);
    addHandle(muonH );
    listOfTrackParticles<<muonH->muon().primaryTrackParticle();
    
  }
  emit shownAssociatedObjectsChanged(listOfTrackParticles);

  return true;
}

//____________________________________________________________________
bool IParticleCollHandle_Muon::cut(AODHandleBase* handle)
{
  
  if (!IParticleCollHandleBase::cut(handle))
    return false;

  IParticleHandle_Muon * muon = dynamic_cast<IParticleHandle_Muon*>(handle);
  if (not muon) return false;
  std::cout<<"muon: "<<muon<<"\t muon->quality()="<<muon->quality()<<" collSettingsButton().minimumQuality()="<<collSettingsButton().minimumQuality()<<std::endl;
  if (static_cast<unsigned int>(muon->quality()) > collSettingsButton().minimumQuality() )
    return false;
  //Fixme: more? Or just use base class method and remove this?

  return true;
}

void IParticleCollHandle_Muon::setMinimumQuality(unsigned int quality) {
  if (quality == d->last_minimumQuality)
    return;
  std::cout<<"IParticleCollHandle_Muon::setMinimumQuality : "<<quality<<" last = "<<d->last_minimumQuality<<std::endl;
  bool relaxcut = quality > d->last_minimumQuality;
  d->last_minimumQuality = quality;

  if (relaxcut)
    recheckCutStatusOfAllNotVisibleHandles();
  else
    recheckCutStatusOfAllVisibleHandles();
  
  // If we shift this to the base class, then we can probably put it into one of the check methods
  updateAssociatedTrackParticles();
}

void IParticleCollHandle_Muon::updateAssociatedTrackParticles()
{
  QList<const xAOD::TrackParticle*> listOfTrackParticles;
  MuonCollectionSettingsButton::ShownAssociatedObjects so = collSettingsButton().shownAssociatedObjects();
  
  handleIterationBegin ();  
  AODHandleBase * handle;
  while ( (handle = getNextHandle() ) ) {
    if (handle->visible()){
      IParticleHandle_Muon* mhandle = dynamic_cast<IParticleHandle_Muon*>(handle);
      if (mhandle && so) {        
        // Only add if bit set, and if link!=0;
        if (so.testFlag(MuonCollectionSettingsButton::TrackParticlesPrimary) &&  mhandle->muon().trackParticle( xAOD::Muon::Primary) ) 
          listOfTrackParticles<<mhandle->muon().trackParticle( xAOD::Muon::Primary);
        if (so.testFlag(MuonCollectionSettingsButton::TrackParticlesCB)  &&  mhandle->muon().trackParticle( xAOD::Muon::CombinedTrackParticle) ) 
          listOfTrackParticles<<mhandle->muon().trackParticle( xAOD::Muon::CombinedTrackParticle);
        if (so.testFlag(MuonCollectionSettingsButton::TrackParticlesID) &&  mhandle->muon().trackParticle( xAOD::Muon::InnerDetectorTrackParticle) ) 
          listOfTrackParticles<<mhandle->muon().trackParticle( xAOD::Muon::InnerDetectorTrackParticle);
        if (so.testFlag(MuonCollectionSettingsButton::TrackParticlesMS) &&  mhandle->muon().trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle) ) 
          listOfTrackParticles<<mhandle->muon().trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle);
        if (so.testFlag(MuonCollectionSettingsButton::TrackParticlesME) &&  mhandle->muon().trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle) ) 
          listOfTrackParticles<<mhandle->muon().trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);
      }
    }
  }
  std::cout<<"IParticleCollHandle_Muon::updateAssociatedTrackParticles Emitting "<<listOfTrackParticles.size()<<" track particles."<<std::endl;
  emit shownAssociatedObjectsChanged(listOfTrackParticles);
}

void IParticleCollHandle_Muon::updateShownAssociatedObjects()
{
  std::cout<<"IParticleCollHandle_Muon::setShownAssociatedObjects : "<<std::endl;
  updateAssociatedTrackParticles();
}



