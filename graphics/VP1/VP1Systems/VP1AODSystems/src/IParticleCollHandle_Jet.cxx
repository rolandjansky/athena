/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class IParticleCollHandle_Jet           //
//                                                            //
//  Author: Riccardo-Maria BIANCHI (rbianchi@cern.ch)         //
//  Initial version: May 2014                                 //
//                                                            //
////////////////////////////////////////////////////////////////

//Local
#include "IParticleCollHandle_Jet.h"
#include "IParticleHandle_Jet.h"
#include "VP1AODSystems/JetCollectionSettingsButton.h"
#include "AODSysCommonData.h"

//VP1
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#ifndef BUILDVP1LIGHT
  #include "VP1Utils/VP1SGAccessHelper.h"
  #include "VP1Utils/VP1SGContentsHelper.h"
#endif // BUILDVP1LIGHT

//Qt
#include <QStringList>

//xAOD
#include "xAODJet/JetContainer.h"


// Coin3D
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoMaterial.h>
//#include "Inventor/nodes/SoDrawStyle.h"
//#include "Inventor/nodes/SoLightModel.h"
#include <Inventor/nodes/SoTexture2.h>

#include "PathResolver/PathResolver.h"

#ifdef BUILDVP1LIGHT
  #include <QSettings>
  #include "xAODRootAccess/Init.h"
  #include "xAODRootAccess/TEvent.h"
#endif // BUILDVP1LIGHT

// System of units
#ifdef BUILDVP1LIGHT
	#include "GeoModelKernel/Units.h"
	#define SYSTEM_OF_UNITS GeoModelKernelUnits // --> 'GeoModelKernelUnits::cm'
#else
  #include "GaudiKernel/SystemOfUnits.h"
  #define SYSTEM_OF_UNITS Gaudi::Units // --> 'Gaudi::Units::cm'
#endif

//____________________________________________________________________
#if defined BUILDVP1LIGHT
  QStringList IParticleCollHandle_Jet::availableCollections( IVP1System*sys )
  {
    return sys->getObjectList(xAOD::Type::Jet);
  }
#else
  QStringList IParticleCollHandle_Jet::availableCollections( IVP1System*sys )
  {
    return VP1SGContentsHelper(sys).getKeys<xAOD::JetContainer>();
  }
#endif // BUILDVP1LIGHT

//____________________________________________________________________
class IParticleCollHandle_Jet::Imp {
public:

  Imp () : theclass(nullptr), updateGUICounter(0), collSettingsButton(nullptr), scale(1.0), 
    randomColours(false), m_jetMaterialDefault(nullptr), bTaggingTagger("MV1"), 
    bTaggingCut(0.98), bTaggingSwitch(nullptr), 
    bTaggingTexture(nullptr), bTaggingMaterial(nullptr) {}

  QString name;

  IParticleCollHandle_Jet * theclass;
  int updateGUICounter;
  JetCollectionSettingsButton* collSettingsButton;
  #ifdef BUILDVP1LIGHT
    QStringList jetList;
  #endif // BUILDVP1LIGHT

  // settings
  double scale = 1.0; // dummy value. The actual default value is set in the IParticleHandle_Jet::Imp class definition.
  bool randomColours = false;
  bool isMaxR = false; // default
  double maxR = 1.0; // dummy value. The actual default value is set in the IParticleHandle_Jet::Imp class definition.

  // jet materials
  SoMaterial* m_jetMaterialDefault;

  // b-tagging
  std::string bTaggingTagger;
  double bTaggingCut;
  SoSwitch   *bTaggingSwitch;
  SoTexture2* bTaggingTexture;
  SoMaterial* bTaggingMaterial;


  //	SoMaterial*        defaultParametersMaterial; /// This will be given to the VP1MaterialButton in



  void possiblyUpdateGUI() {//Fixme: to IParticleCollHandleBase
    if (!((updateGUICounter++)%50)) {
      theclass->systemBase()->updateGUI();
    }
  }
};

//____________________________________________________________________
IParticleCollHandle_Jet::IParticleCollHandle_Jet(AODSysCommonData * cd,
const QString& name, xAOD::Type::ObjectType type)
  : IParticleCollHandleBase(cd,name,type), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->updateGUICounter = 0;
  m_d->collSettingsButton=0;

  m_d->name = name;

  //==========
  // b-tagging
  m_d->bTaggingSwitch=new SoSwitch;
  m_d->bTaggingSwitch->whichChild=SO_SWITCH_ALL;
  // new b-tagged jets and material
  m_d->bTaggingTexture = new SoTexture2;
  m_d->bTaggingMaterial = new SoMaterial;


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

  //  m_d->defaultParametersMaterial = new SoMaterial;
  //  m_d->defaultParametersMaterial->setName(("IParticleCollHandle_Jet"+safetext).toStdString().c_str());
  //  m_d->defaultParametersMaterial->ref();
  // m_d->matButton->setDefaultParameterMaterial(m_d->defaultParametersMaterial); FIXME
}

//____________________________________________________________________
IParticleCollHandle_Jet::~IParticleCollHandle_Jet()
{
  //	m_d->defaultParametersMaterial->unref();

  if (m_d->bTaggingSwitch) m_d->bTaggingSwitch->unref();

  delete m_d;
}

//____________________________________________________________________
void IParticleCollHandle_Jet::init(VP1MaterialButtonBase*)
{
	messageDebug("IParticleCollHandle_Jet::init()");

  m_d->collSettingsButton = new JetCollectionSettingsButton(0, 25, m_d->name); // 0 and 25 are default values
  m_d->collSettingsButton->setMaterialText(name());

  // 1st - CALLING THE "init" OF THE BASE CLASS
  // NOTE!!
  // It must be called before other material/button settings,
  // otherwise they get overridden
  // std::cout<<"Calling VP1StdCollection::init with m_d->matButton (JetCollectionSettingsButton)="<<m_d->matButton<<std::endl;
  VP1StdCollection::init(m_d->collSettingsButton);  //--->this call is required!! Passing in m_d->collSettingsButton means we have the more complex button that allows cuts
  setupSettingsFromController(common()->controller());
  connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(collVisibilityChanged(bool)));



  // create semi-transparent material for all jets, with the default color
  	m_d->m_jetMaterialDefault = VP1MaterialButton::createMaterial(defaultColor(), 0.2, 0.3);  // RGBcolor, brightness, transparency
  	m_d->m_jetMaterialDefault->ref();
  // init material for all jets
  	m_d->collSettingsButton->setMaterial(m_d->m_jetMaterialDefault);
  	messageVerbose("set new jet color=" + str(m_d->collSettingsButton->getMaterialButton()->lastAppliedDiffuseColour()));


  // std::cout<<"IParticleCollHandle_Jet::init 2"<<std::endl;
  // std::cout<<"swi: "<<collSwitch()<<std::endl;
  // std::cout<<"sep: "<<collSep()<<std::endl;
  // std::cout<<"mat: "<<material()<<std::endl;

  //	collSwitch()->addChild(m_d->collSettingsButton->trackLightModel()); // TODO: update for jets
  //	collSwitch()->addChild(m_d->collSettingsButton->trackDrawStyle()); // TODO: update for jets

  //==========
  // b-tagging
  if(m_d->collSettingsButton->is_bTaggingSkinEnabled()) {
    std::cout << "switch texture" << std::endl;
    setBTaggingSkin(m_d->collSettingsButton->bTaggingSkin());
    //	  m_d->bTaggingSwitch->addChild(m_d->bTaggingTexture);
  }
  else if (m_d->collSettingsButton->is_bTaggingMaterialEnabled()) {
    std::cout << "switch material" << std::endl;
    setBTaggingMaterial();
    //	  m_d->bTaggingMaterial = controller->bTaggingMaterial();
    //	  m_d->bTaggingSwitch->addChild(m_d->bTaggingMaterial);
  }
  else {
    messageVerbose("Info - No b-tag rendering selected.");
  }

  // we want these nodes to stay around even when removed from nodes, thus we increment the ref count by one
  m_d->bTaggingSwitch->ref();
  //  m_d->bTaggingTexture->ref();
  //  m_d->bTaggingMaterial->ref();

}


//____________________________________________________________________
void IParticleCollHandle_Jet::setupSettingsFromControllerSpecific(AODSystemController*)
{
	messageDebug("IParticleCollHandle_Jet::setupSettingsFromControllerSpecific()");

  JetCollectionSettingsButton* controller = m_d->collSettingsButton;

  //cuts  
  connect(controller,SIGNAL(cutAllowedPtChanged(const VP1Interval&)),this,SLOT(setCutAllowedPt(const VP1Interval&)));
  setCutAllowedPt(controller->cutAllowedPt());

  connect(controller,SIGNAL(cutAllowedEtaChanged(const VP1Interval&)),this,SLOT(setCutAllowedEta(const VP1Interval&)));
  setCutAllowedEta(controller->cutAllowedEta());

  connect(controller,SIGNAL(cutAllowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(setCutAllowedPhi(const QList<VP1Interval>&)));
  setCutAllowedPhi(controller->cutAllowedPhi());

  //	// Parameters
  //	connect(controller,SIGNAL(showParametersChanged(bool)),        this,SLOT(showParametersChanged(bool)));
  //	connect(controller,SIGNAL(colourParametersByTypeChanged(bool)),this,SLOT(showParametersChanged(bool)));
  // Just reusing the same slot, since it doesn't

  // scale
  connect(controller,SIGNAL(scaleChanged(const double&)),this,SLOT(setScale(const double&)));
  setScale( this->scale() );

  // random jet colors
  connect(controller,SIGNAL(rerandomise()),this,SLOT(rerandomise()));
  connect(controller,SIGNAL(randomJetColoursChanged(const bool&)),this,SLOT(setRandomJetColours(const bool&)));
  setRandomJetColours(controller->randomJetColours());

  // maxR
  connect(controller, SIGNAL(maxRChanged(const double&)), this, SLOT(setMaxR(const double&)));

  // b-tagging
  connect(controller, SIGNAL(bTaggingEnabledChanged(const bool&)), this, SLOT(setBTaggingEnabled(const bool&)));
  setBTaggingEnabled(controller->bTaggingEnabled());
  connect(controller, SIGNAL(bTaggingTaggerChanged(const QString &)), this, SLOT(setBTaggingTagger(const QString &)));
  setBTaggingTagger(controller->bTaggingTagger());
  connect(controller, SIGNAL(bTaggingCutChanged(const double &)), this, SLOT(setBTaggingCut(const double &)));
  setBTaggingCut(controller->bTaggingCut());
  connect(controller,SIGNAL(bTaggingRenderingMaterialChanged(const bool&)), this, SLOT(setBTaggingMaterialChanged(const bool&)));

  connect(controller,SIGNAL(bTaggingRenderingSkinChanged(const bool&)), this, SLOT(setBTaggingSkinChanged(const bool&)));
}

//SoMaterial* IParticleCollHandle_Jet::defaultParameterMaterial() const {
//	return m_d->defaultParametersMaterial;
//}


const JetCollectionSettingsButton& IParticleCollHandle_Jet::collSettingsButton() const {
  if (!m_d->collSettingsButton){
    messageVerbose("Jet - No collSettingsButton set! Can't call init(), so crash is imminent...");
  }
  return *m_d->collSettingsButton;
}



//____________________________________________________________________
void IParticleCollHandle_Jet::setScale(const double& sca)
{

  messageVerbose("IParticleCollHandle_Jet::setScale() - scale: " + QString::number(sca));

  if (getHandlesList().isEmpty()) {
    messageVerbose("no jet handles defined! returning.");
    return;
  }
  if (m_d->scale == sca)
    return;

  bool isEt = sca>0;
  double tmpScale = std::max(1*SYSTEM_OF_UNITS::mm/(100*SYSTEM_OF_UNITS::GeV),
  std::min(99*SYSTEM_OF_UNITS::m/(1*SYSTEM_OF_UNITS::MeV),
  //						m_d->collSettingsButton->lengthOf100GeV() * Gaudi::Units::m/(100.0*Gaudi::Units::GeV)));
  std::fabs(sca) * SYSTEM_OF_UNITS::m/(100.0*SYSTEM_OF_UNITS::GeV)));

  m_d->scale = isEt ? tmpScale : -tmpScale;

  if (!isLoaded())
    return;

  messageVerbose("Scale change: to "+str(m_d->scale/(SYSTEM_OF_UNITS::m/(100.0 * SYSTEM_OF_UNITS::GeV)))+" m/100GeV. Updating "+str(getHandlesList().count())+" jets");
  // std::cout << "Scale change: d->scale/(SYSTEM_OF_UNITS::m/(100.0*SYSTEM_OF_UNITS::GeV)))" <<  "m/100GeV. Updating " << getHandlesList().count() << " jets" << std::endl;

  largeChangesBegin();
  handleIterationBegin();
  AODHandleBase* handle=0;
  while ((handle=getNextHandle()))
  {
    IParticleHandle_Jet* jet = dynamic_cast<IParticleHandle_Jet*>(handle);
    if (jet && jet->has3DObjects()) {
      jet->setScale(tmpScale, isEt);
      jet->updateHeight();
    } else {
      message("ERROR Handle of wrong type!");
    }
  }
  largeChangesEnd();
}

//____________________________________________________________________
double IParticleCollHandle_Jet::scale() const
{
  return m_d->scale;
}

//____________________________________________________________________
double IParticleCollHandle_Jet::maxR() const
{
  return m_d->maxR;
}


//____________________________________________________________________
void IParticleCollHandle_Jet::setMaxR(const double& maxR)
{
  messageVerbose("IParticleCollHandle_Jet::setMaxR() - maxR: " + QString::number(maxR));
  //	messageVerbose("setMaxR to: " + str(maxR)+str(", from: ")+str(m_d->maxR));

  if (m_d->maxR == maxR){ // no changes
    messageDebug("no changes, returning...");
    return;
    }

  m_d->maxR = maxR;

  if (!isLoaded()) {
    messageDebug("is not loaded, returning...");
    return;
    }

  messageDebug("modifying MaxR for all jets...");
  largeChangesBegin();
  handleIterationBegin();
  AODHandleBase* handle=0;
  while ((handle=getNextHandle()))
  {
    IParticleHandle_Jet* jet = dynamic_cast<IParticleHandle_Jet*>(handle);
    if (jet && jet->has3DObjects()) {
      jet->setMaxR(m_d->maxR);
      jet->updateHeight();
    } else {
      message("ERROR Handle of wrong type!");
    }
  }
  largeChangesEnd();
}


//____________________________________________________________________
bool IParticleCollHandle_Jet::isRandomColors() const
{
  VP1Msg::messageVerbose("IParticleCollHandle_Jet::isRandomColors()");
  return m_d->randomColours;
}

////____________________________________________________________________
//bool IParticleCollHandle_Jet::isMaxR() const
//{
//	VP1Msg::messageVerbose("IParticleCollHandle_Jet::isMaxR()");
//	return m_d->isMaxR;
//}


//____________________________________________________________________
void IParticleCollHandle_Jet::setRandomJetColours(const bool& b)
{
  messageVerbose("IParticleCollHandle_Jet::setRandomJetColours() - b: "+QString::number(b));

  if (m_d->randomColours == b)
    return;

  m_d->randomColours = b;
  std::cout << "isRandom: " << m_d->randomColours << std::endl;

  if (!isLoaded())
    return;

  largeChangesBegin();
  int ijet = 0;
  handleIterationBegin();
  AODHandleBase* handle=0;
  while ((handle=getNextHandle()))
  {    
    ++ijet;
    IParticleHandle_Jet* jet = dynamic_cast<IParticleHandle_Jet*>(handle);
    if (jet && jet->has3DObjects()) {
      messageVerbose("considering jet: "+QString::number(ijet));
      int randomColors = isRandomColors();
      messageVerbose("setting random material on jet: "+QString::number(ijet)+" - random colors? "+QString::number(randomColors));
      //			std::cout << "isRandomColors() --> " << isRandomColors() << std::endl;
      jet->updateMaterial( randomColors );

    } else {
      message("ERROR Handle of wrong type!");
    }
  }
  largeChangesEnd();
}

//____________________________________________________________________
void IParticleCollHandle_Jet::rerandomise()
{
  messageVerbose("IParticleCollHandle_Jet::rerandomise()");

  if (!isLoaded()) {
    messageVerbose("IParticleCollHandle_Jet::rerandomise() - Returning.");
    return;
  }
  if (!isRandomColors()) {
    VP1Msg::message("'Random jet colours' is not checked. Nothing to do. Returning.");
    return;
  }

  largeChangesBegin();
  handleIterationBegin();
  unsigned int ijet(0);
  AODHandleBase* handle=0;
  while ((handle=getNextHandle()))
  {    
    ++ijet;
    IParticleHandle_Jet* jet = dynamic_cast<IParticleHandle_Jet*>(handle);
    if (jet && jet->has3DObjects()) {
      jet->rerandomiseMaterial();
    } else {
      message("ERROR Handle of wrong type!");
    }
  }
  largeChangesEnd();
}

//____________________________________________________________________
bool IParticleCollHandle_Jet::load()
{
  messageVerbose("loading Jet collection");

  //Get collection:
  const xAOD::JetContainer * coll(nullptr);

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
      QString errMsg = "Failed to retrieve " + name();
      message("Error: Could not retrieve collection with key="+name());
       return false;
    }
  #else
  if (!VP1SGAccessHelper(systemBase()).retrieve(coll, name())) {
    message("Error: Could not retrieve Jet collection with key="+name());
    return false;
  } else {
     messageDebug("Jet collection '"+name()+"' loaded");
  }
  #endif // BUILDVP1LIGHT


  // // Retrieve the xAOD particles:
  //  const xAOD::JetContainer* xaod = evtStore()->retrieve<const xAOD::JetContainer>( m_JetCollection );

  // Check that the auxiliary store association was made successfully:
  if( ! coll->hasStore() ) {
    message("No auxiliary store got associated to the Jet container with key: " +name());
  }

  // This is needed for now, until the issues around the DV code are
  // sorted out...
  const_cast< xAOD::JetContainer* >( coll )->setStore(
    ( SG::IAuxStore* ) coll->getConstStore() );

  //Make appropriate Jet handles:
  // hintNumberOfTracksInEvent(coll->size());
  xAOD::JetContainer::const_iterator it, itEnd = coll->end();
  for ( it = coll->begin() ; it != itEnd; ++it) {
    m_d->possiblyUpdateGUI();
    if (!*it) {
      messageDebug("WARNING: Ignoring null Jet pointer.");
      continue;
    }
    /*
    if ((*it)->charge()==0.0) {
    messageDebug("WARNING: Ignoring  which claims to be neutral (charge()==0.0).");
    continue;
    }
    */
    addHandle(new IParticleHandle_Jet(this,*it));
  }

  // get handles list and update locally
  // m_d->jets = this->getHandlesList();

  return true;
}

//Cuts:
//____________________________________________________________________
bool IParticleCollHandle_Jet::cut(AODHandleBase* c) {
  messageVerbose("IParticleCollHandle_Jet::cut()");

  if (IParticleHandle_Jet* handle = dynamic_cast<IParticleHandle_Jet*>(c))
  {

    //------ ENERGY CUTS
    messageVerbose("jet's energy, allowedPt^2, PtAll: - " + QString::number(handle->energyForCuts()) + " - " + getCutAllowedPt().toString() + " - " + QString::number(int(getPtAllowall()))  );
    double jEnergy = handle->energyForCuts();
    double jEnergySqr = jEnergy * jEnergy; // in IParticleCollHandleBase the allowedPt is stored as squared, to avoid square-root operations
    if (!getPtAllowall() && !getCutAllowedPt().contains(jEnergySqr) ) {
      messageVerbose("Pt cut not passed");
      return false;
    } else {
      messageVerbose("Pt cut passed!!");
    }

    //------ ETA CUTS
    messageVerbose("jet's eta, etaCut, EtaAll: " + QString::number(handle->eta()) + " - " + getCutAllowedEta().toString() + " - " + QString::number(int(getEtaAllowall())) );
    if (!getEtaAllowall() && !getCutAllowedEta().contains(handle->eta())) {
      messageVerbose("Eta cut not passed");
      return false;
    }

    //------ PHI CUTS
    if (!getPhiAllowall() ) {
      double phi = handle->phi();
      bool ok(false);
      foreach (VP1Interval phirange, getCutAllowedPhi() ) {
        messageVerbose("jet's phi, phiCut, PhiAll: " + QString::number(phi)  + " - " + phirange.toString() + " - " + QString::number(int(getPhiAllowall())) );
        if (phirange.contains(phi)||phirange.contains(phi+2*M_PI)||phirange.contains(phi-2*M_PI)) {
          ok = true;
          break;
        }
      }
      if (!ok) {
        messageVerbose("Phi cut not passed");
        return false;
      }
    }

    //------ OTHER CUTS
    messageVerbose("Other cuts??? Calling base method...");
    if (!IParticleCollHandleBase::cut(handle))
      return false;

  } // if(dynamic_cast)
  else{
    messageVerbose("Not a IParticleCollHandle_Jet. Calling base method...");
    if (!IParticleCollHandleBase::cut(handle))
      return false;
  }

  return true; //default

}



//____________________________________________________________________
void IParticleCollHandle_Jet::showParametersChanged(bool val) {
  messageVerbose("IParticleCollHandle_Jet::showParametersChanged to "+str(val));
  update3DObjectsOfAllHandles();
}


//____________________________________________________________________
void IParticleCollHandle_Jet::setBTaggingEnabled(const bool& flag) {
  messageVerbose("IParticleCollHandle_Jet::setBTaggingEnabled - "+str(flag));
  m_d->bTaggingSwitch->whichChild = (flag ? SO_SWITCH_ALL : SO_SWITCH_NONE);
  if (flag) {
    setBTaggingMaterialChanged(true); // we set default "Material"
  }
}


//____________________________________________________________________
void IParticleCollHandle_Jet::setBTaggingSkin(const QString &filename){

  if (m_d->collSettingsButton->bTaggingRenderingSkin()) {

    //remove the previous skin and material
    m_d->bTaggingSwitch->removeChild(m_d->bTaggingTexture);
    m_d->bTaggingSwitch->removeChild(m_d->bTaggingMaterial);
    //		delete m_d->bTaggingTexture;
    //		delete m_d->bTaggingMaterial;
    m_d->bTaggingTexture = 0;
    m_d->bTaggingMaterial = 0;

    // setting the texture
    m_d->bTaggingTexture = new SoTexture2;
    m_d->bTaggingMaterial = new SoMaterial;

    // original from Joe, for skins/textures
    std::string fName = PathResolver::find_file (filename.toStdString()+".png", "DATAPATH");
    messageVerbose("texture found: " + QString::fromStdString(fName) );
    m_d->bTaggingTexture->filename.setValue(fName.c_str());

    // adding the texture to the SoSwitch
    m_d->bTaggingSwitch->addChild(m_d->bTaggingTexture);
  }

  updateBTaggingSwitchAllJets(); // update switch all jets
  updateBTaggingAllJets(); // update cut for all jets
}

//____________________________________________________________________
void IParticleCollHandle_Jet::setBTaggingMaterial(SoMaterial* mat)
{
  messageVerbose("IParticleCollHandle_Jet::setBTaggingMaterial()");

  if (m_d->collSettingsButton->bTaggingRenderingMaterial()) {

    messageVerbose("Updating the material node");

    //std::cout << "Updating - old mat: " << m_d->bTaggingMaterial << "..." << std::endl; // it continues below...

    //remove the previous skin and material
    m_d->bTaggingSwitch->removeChild(m_d->bTaggingTexture);
    m_d->bTaggingSwitch->removeChild(m_d->bTaggingMaterial);
    //		delete m_d->bTaggingTexture;
    //		delete m_d->bTaggingMaterial;
    m_d->bTaggingTexture = 0;
    m_d->bTaggingMaterial = 0;

    //		float r = 0.4; float g = 0.15; float b = 0.0; float br = 0.8; float tr = 0.3; // reddish color
    //		VP1MaterialButton::setMaterialParameters( mat, r, g, b, br /*brightness*/, tr /*transparency*/ );

    if (! mat) {
      messageVerbose("taking the material from the controller");
      m_d->bTaggingMaterial = m_d->collSettingsButton->bTaggingMaterial();
      m_d->bTaggingSwitch->addChild( m_d->bTaggingMaterial );
    }
    else {
      messageVerbose("taking the argument material");
      m_d->bTaggingMaterial = mat;
      m_d->bTaggingSwitch->addChild( m_d->bTaggingMaterial );
    }

    //std::cout << "--> new mat: " << m_d->bTaggingMaterial << std::endl;

    updateBTaggingSwitchAllJets(); // update switch all jets
    updateBTaggingAllJets(); // update cut for all jets
  }
}


//____________________________________________________________________
void IParticleCollHandle_Jet::setBTaggingTagger(const QString & tagger){

  //	std::cout << "current bTaggingTagger: " << m_d->bTaggingTagger << " - new: " << tagger << std::endl;

  if (m_d->bTaggingTagger == tagger.toStdString())
    return;

  m_d->bTaggingTagger = tagger.toStdString();

  if (!isLoaded())
    return;

  messageVerbose("BTaggingTagger change to: " +tagger+ " (with cut: " + QString::number(m_d->bTaggingCut) + "). Updating "+str(getHandlesList().count())+" jets");

  updateBTaggingAllJets(); // update all jets
}


//____________________________________________________________________
void IParticleCollHandle_Jet::setBTaggingCut(const double& wCut){

  std::cout << m_d->bTaggingCut << std::endl;

  if (m_d->bTaggingCut == wCut)
    return;

  m_d->bTaggingCut = wCut;

  if (!isLoaded())
    return;

  messageVerbose("BTaggingCut change to "+str(m_d->bTaggingCut)+". Updating "+str(getHandlesList().count())+" jets");

  updateBTaggingAllJets(); // update all jets

}

//____________________________________________________________________
void IParticleCollHandle_Jet::updateBTaggingAllJets()
{
  messageVerbose("IParticleCollHandle_Jet::updateBTaggingAllJets()");
  largeChangesBegin();
  handleIterationBegin();
  AODHandleBase* handle=0;
  while ((handle=getNextHandle()))
  {
    IParticleHandle_Jet* jet = dynamic_cast<IParticleHandle_Jet*>(handle);
    if (jet && jet->has3DObjects()) {
      jet->updateBTagging(m_d->bTaggingTagger, m_d->bTaggingCut);
    } else {
      message("ERROR Handle of wrong type!");
    }
  }
  largeChangesEnd();
}

//____________________________________________________________________
void IParticleCollHandle_Jet::updateBTaggingSwitchAllJets()
{
  messageVerbose("IParticleCollHandle_Jet::updateBTaggingSwitchAllJets()");
  largeChangesBegin();
  handleIterationBegin();
  AODHandleBase* handle=0;
  while ((handle=getNextHandle()))
  {
    IParticleHandle_Jet* jet = dynamic_cast<IParticleHandle_Jet*>(handle);
    if (jet && jet->has3DObjects()) {
      jet->updateBTaggingSwitch(m_d->bTaggingSwitch);
    } else {
      message("ERROR Handle of wrong type!");
    }
  }
  largeChangesEnd();
}


//____________________________________________________________________
void IParticleCollHandle_Jet::setBTaggingMaterialChanged(const bool& ok)
{
  if (ok) {
    messageVerbose("IParticleCollHandle_Jet::setBTaggingMaterialChanged()");
    setBTaggingMaterial();
  }
}

//____________________________________________________________________
void IParticleCollHandle_Jet::setBTaggingSkinChanged(const bool& ok) {
  if (ok)
    setBTaggingSkin(m_d->collSettingsButton->bTaggingSkin());
}


//____________________________________________________________________
QByteArray IParticleCollHandle_Jet::persistifiableState() const
{
  messageDebug("IParticleCollHandle_Jet::persistifiableState() - start...");

  // if (!m_d->matButton) {
  //   message("ERROR: persistifiableState() called before init()");
  //   return QByteArray();
  // }


  VP1Serialise serialise(1/*version*/);

  // settings
  serialise.disableUnsavedChecks();

  // SAVE THE CHECKED/UNCHECKED STATUS OF THE COLLECTION
  serialise.save(visible());

  // SAVE THE MATERIAL BUTTON
  //Q_ASSERT(m_d->matButton&&"Did you forget to call init() on this VP1StdCollection?");
  //serialise.save(m_d->matButton->saveState());

  // SAVE THE EXTRA-STATES
  serialise.save(extraWidgetsState());//version 1+

  // SAVE MATERIAL SETTINGS / CUTS
  serialise.save(m_d->collSettingsButton->saveState());

  messageDebug("IParticleCollHandle_Jet::persistifiableState() - end.");
  return serialise.result();
}

//____________________________________________________________________
void IParticleCollHandle_Jet::setState(const QByteArray&state)
{
  messageDebug("IParticleCollHandle_Jet::setState()");

  VP1Deserialise des(state);
  des.disableUnrestoredChecks();
  if (des.version()!=0&&des.version()!=1) {
    messageDebug("Warning: Ignoring state with wrong version");
    return;
  }

  // save state

  bool vis = des.restoreBool();

  //	QByteArray matState = des.restoreByteArray();
  // m_d->matButton->restoreFromState(matState);
  QByteArray extraWidgetState = des.version()>=1 ? des.restoreByteArray() : QByteArray();
  setVisible(vis);

  if (extraWidgetState!=QByteArray())
    setExtraWidgetsState(extraWidgetState);

  // MATERIAL SETTINGS / CUTS
  messageDebug("restoring material collection button...");
  des.restore(m_d->collSettingsButton);

  messageDebug("reset all caches storing values for cuts...");
  resetCachedValuesCuts();

  messageDebug("recheck all handles...");
  recheckCutStatusOfAllVisibleHandles();

}


void IParticleCollHandle_Jet::resetCachedValuesCuts()
{
	// kinetic cuts
	setCutAllowedPt(m_d->collSettingsButton->cutAllowedPt());
	setCutAllowedEta(m_d->collSettingsButton->cutAllowedEta());
	setCutAllowedPhi(m_d->collSettingsButton->cutAllowedPhi());
	setScale( this->scale() );
	// colouring
	setRandomJetColours(m_d->collSettingsButton->randomJetColours());
	// b-tagging
	setBTaggingEnabled(m_d->collSettingsButton->bTaggingEnabled());
	setBTaggingTagger(m_d->collSettingsButton->bTaggingTagger());
	setBTaggingCut(m_d->collSettingsButton->bTaggingCut());
	// maxR
	setMaxR(m_d->collSettingsButton->maxR());
}

void IParticleCollHandle_Jet::dumpToJSON( std::ofstream& str) const {
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

