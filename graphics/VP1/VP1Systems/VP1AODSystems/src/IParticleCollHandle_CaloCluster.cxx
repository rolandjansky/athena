/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class IParticleCollHandle_CaloCluster           //
//                                                            //
//  Author: Riccardo-Maria BIANCHI (rbianchi@cern.ch)         //
//  Initial version: May 2014                                 //
//                                                            //
////////////////////////////////////////////////////////////////

//Local
#include "IParticleCollHandle_CaloCluster.h"
#include "IParticleHandle_CaloCluster.h"
#include "VP1AODSystems/CaloClusterCollectionSettingsButton.h"
#include "AODSysCommonData.h"

//VP1
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Utils/VP1SGContentsHelper.h"

//Qt
#include <QtCore/QStringList>

//xAOD
#include "xAODCaloEvent/CaloClusterContainer.h"


// Coin3D
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoMaterial.h>
//#include "Inventor/nodes/SoDrawStyle.h"
//#include "Inventor/nodes/SoLightModel.h"
#include <Inventor/nodes/SoTexture2.h>


#include "PathResolver/PathResolver.h"



//____________________________________________________________________
QStringList IParticleCollHandle_CaloCluster::availableCollections( IVP1System*sys )
{
  return VP1SGContentsHelper(sys).getKeys<xAOD::CaloClusterContainer>();
}

//____________________________________________________________________
class IParticleCollHandle_CaloCluster::Imp {

public:

//  Imp () : theclass(0), updateGUICounter(0), collSettingsButton(0), scale(1.0), randomColours(false), bTaggingTagger("MV1"), bTaggingCut(0.98), bTaggingSwitch(0), bTaggingTexture(0), bTaggingMaterial(0) {}
  Imp () : theclass(0), updateGUICounter(0), collSettingsButton(0) {}

  IParticleCollHandle_CaloCluster * theclass;
  int updateGUICounter;
  CaloClusterCollectionSettingsButton* collSettingsButton;

  VP1ExtraSepLayerHelper * sephelper;
//  QString key;


//   settings
//  double scale = 1.0; // dummy value. The actual default value is set in the IParticleHandle_CaloCluster::Imp class definition.
////  bool randomColours = false;
//  bool isMaxR = false; // default
//  double maxR = 1.0; // dummy value. The actual default value is set in the IParticleHandle_CaloCluster::Imp class definition.

//  // b-tagging
//  std::string bTaggingTagger;
//  double bTaggingCut;
//  SoSwitch   *bTaggingSwitch;
//  SoTexture2* bTaggingTexture;
//  SoMaterial* bTaggingMaterial;


  //	SoMaterial*        defaultParametersMaterial; /// This will be given to the VP1MaterialButton in



  // settings
  bool considerTransverseEnergies;
  QPair<bool,double> scale;
  bool showOutlines;
  VP1Interval allowedEnergies;
  VP1Interval allowedEta;
  QList<VP1Interval> allowedPhi;
  double last_highestEnergy;
  double calculateHighestVisibleClusterEnergy() const;


  double calo_start_r = theclass->calo_start_r;
  double calo_start_z = theclass-> calo_start_z;
  double calo_crack_eta = theclass->calo_crack_eta;

  void possiblyUpdateGUI() {//Fixme: to IParticleCollHandleBase
    if (!((updateGUICounter++)%50)) {
      theclass->systemBase()->updateGUI();
    }
  }
};

/*
 * DEFINITIONS
 */
//Fixme: Just some approximate values for now:
double IParticleCollHandle_CaloCluster::calo_start_r = 1.1*CLHEP::m + 0.05*CLHEP::m;
double IParticleCollHandle_CaloCluster::calo_start_z = 3.671*CLHEP::m + 0.05*CLHEP::m;
double IParticleCollHandle_CaloCluster::calo_crack_eta = fabs(log(tan(0.5*atan(calo_start_r/calo_start_z))));



//____________________________________________________________________
IParticleCollHandle_CaloCluster::IParticleCollHandle_CaloCluster(AODSysCommonData * cd,
const QString& name, xAOD::Type::ObjectType type)
  : IParticleCollHandleBase(cd,name,type), d(new Imp)
{
  d->theclass = this;
  d->updateGUICounter = 0;
  d->collSettingsButton=0;


//  d->controller = controller;
//  d->key = key;
  d->sephelper = 0;
  //We start out with no clusters visible:
  d->last_highestEnergy = 0;
  d->showOutlines = false;
  d->considerTransverseEnergies = true;

//  //==========
//  // b-tagging
//  d->bTaggingSwitch=new SoSwitch;
//  d->bTaggingSwitch->whichChild=SO_SWITCH_ALL;
//  // new b-tagged jets and material
//  d->bTaggingTexture = new SoTexture2;
//  d->bTaggingMaterial = new SoMaterial;


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

  //  d->defaultParametersMaterial = new SoMaterial;
  //  d->defaultParametersMaterial->setName(("IParticleCollHandle_CaloCluster"+safetext).toStdString().c_str());
  //  d->defaultParametersMaterial->ref();
  // d->matButton->setDefaultParameterMaterial(d->defaultParametersMaterial); FIXME
}

//____________________________________________________________________
IParticleCollHandle_CaloCluster::~IParticleCollHandle_CaloCluster()
{
  //	d->defaultParametersMaterial->unref();

//  if (d->bTaggingSwitch) d->bTaggingSwitch->unref();

  delete d;
}

//____________________________________________________________________
void IParticleCollHandle_CaloCluster::init(VP1MaterialButtonBase*)
{
  // std::cout<<"IParticleCollHandle_CaloCluster::init 1"<<std::endl;


  //	d->matButton = new CaloClusterCollectionSettingsButton;
  //	d->matButton->setMaterialText(name());

  // INIT THE COLLECTION 'MATERIAL' BUTTON
  d->collSettingsButton = new CaloClusterCollectionSettingsButton;
  d->collSettingsButton->setMaterialText(name());

  // pass a collection pointer to the button
  d->collSettingsButton->setCollHandle(this);

  // std::cout<<"Calling VP1StdCollection::init with d->matButton (CaloClusterCollectionSettingsButton)="<<d->matButton<<std::endl;
  VP1StdCollection::init(d->collSettingsButton);//this call is required. Passing in d->collSettingsButton means we have the more complex button.
  setupSettingsFromController(common()->controller());
  connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(collVisibilityChanged(bool)));


  // std::cout<<"IParticleCollHandle_CaloCluster::init 2"<<std::endl;
  // std::cout<<"swi: "<<collSwitch()<<std::endl;
  // std::cout<<"sep: "<<collSep()<<std::endl;
  // std::cout<<"mat: "<<material()<<std::endl;

  //	collSwitch()->addChild(d->collSettingsButton->trackLightModel()); // TODO: update for jets
  //	collSwitch()->addChild(d->collSettingsButton->trackDrawStyle()); // TODO: update for jets

//  //==========
//  // b-tagging
//  if(d->collSettingsButton->is_bTaggingSkinEnabled()) {
//    std::cout << "switch texture" << std::endl;
//    setBTaggingSkin(d->collSettingsButton->bTaggingSkin());
//    //	  d->bTaggingSwitch->addChild(d->bTaggingTexture);
//  }
//  else if (d->collSettingsButton->is_bTaggingMaterialEnabled()) {
//    std::cout << "switch material" << std::endl;
//    setBTaggingMaterial();
//    //	  d->bTaggingMaterial = controller->bTaggingMaterial();
//    //	  d->bTaggingSwitch->addChild(d->bTaggingMaterial);
//  }
//  else {
//    messageVerbose("Info - No b-tag rendering selected.");
//  }
//
//  // we want these nodes to stay around even when removed from nodes, thus we increment the ref count by one
//  d->bTaggingSwitch->ref();
//  //  d->bTaggingTexture->ref();
//  //  d->bTaggingMaterial->ref();
//  //==========


}



//____________________________________________________________________
double IParticleCollHandle_CaloCluster::energyToLength(const double&energy) const
{
	VP1Msg::messageDebug("IParticleCollHandle_CaloCluster::energyToLength()");

	return std::max(1*CLHEP::mm, d->scale.second*(d->scale.first?log(1+fabs(energy)):energy));
}

//____________________________________________________________________
bool IParticleCollHandle_CaloCluster::showOutlines() const
{
	VP1Msg::messageDebug("IParticleCollHandle_CaloCluster::showOutlines()");

	return d->showOutlines;
}

//____________________________________________________________________
void IParticleCollHandle_CaloCluster::setScale()
{
	this->setScale( collSettingsButton().scale() );
}

#include <Inventor/nodes/SoMaterial.h>//Fixme: just for hack below
//____________________________________________________________________
void IParticleCollHandle_CaloCluster::setScale(const QPair<bool,double>& s)
{
//  std::cout << "s: " << s.first << std::endl;

  VP1Msg::messageDebug("IParticleCollHandle_CaloCluster::setScale()");


  if (d->scale==s) {
	  VP1Msg::messageDebug("Scale is the same. Returning...");
	  return;
  }

  VP1Msg::messageDebug("setting scale: (" + QString::number(s.first) + ", " + QString::number(s.second) + " )");
  d->scale=s;

  if (!isLoaded()) {
	  VP1Msg::messageDebug("Collection not loaded/shown. Returning...");
	  return;
  }
//  largeChangesBegin();
//  foreach(Imp::ClusterHandle*cluster,d->clusters)
//    if (cluster->attached())
//      cluster->updateShapePars(d);
//  largeChangesEnd();

  largeChangesBegin();
  handleIterationBegin();
  AODHandleBase* handle=0;
  while ((handle=getNextHandle()))
  {
	  IParticleHandle_CaloCluster* cluster = dynamic_cast<IParticleHandle_CaloCluster*>(handle);
	  if (cluster && cluster->has3DObjects()) {
//		  cluster->setScale(d->scale);
		  cluster->updateShape(this);
//		  calo->updateHeight();
	  } else {
		  message("ERROR Handle of wrong type!");
	  }
  }
  largeChangesEnd();



  //   if (verbose())
  //     static_cast<IVP13DSystemSimple *>(systemBase())->warnOnDisabledNotifications();

  material()->touch();//FIXME: This is needed (and nothing else seems to matter). BUT WHY?????? Where is the ***REMOVED***?!?!?
}


//____________________________________________________________________
QPair<bool,double> IParticleCollHandle_CaloCluster::scale() const
{
	VP1Msg::messageDebug("IParticleCollHandle_CaloCluster::scale()");
  return d->scale;
}


//____________________________________________________________________
void IParticleCollHandle_CaloCluster::setupSettingsFromControllerSpecific(AODSystemController*) {

	VP1Msg::messageDebug("IParticleCollHandle_CaloCluster::setupSettingsFromControllerSpecific()");

  CaloClusterCollectionSettingsButton* controller = d->collSettingsButton;

  // *** common cuts ***
  connect(controller,SIGNAL(cutAllowedPtChanged(const VP1Interval&)),this,SLOT(setCutAllowedPt(const VP1Interval&)));
  setCutAllowedPt(controller->cutAllowedPt());

  connect(controller,SIGNAL(energyTypeChanged()),this,SLOT(rebuildAllObjects()));

  connect(controller,SIGNAL(cutAllowedEtaChanged(const VP1Interval&)),this,SLOT(setCutAllowedEta(const VP1Interval&)));
  setCutAllowedEta(controller->cutAllowedEta());

  connect(controller,SIGNAL(cutAllowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(setCutAllowedPhi(const QList<VP1Interval>&)));
  setCutAllowedPhi(controller->cutAllowedPhi());

  // *** specific cuts and settings ***

  // scale
  connect(controller,SIGNAL(scaleChanged(const QPair<bool,double>&)),this,SLOT(setScale(const QPair<bool,double>&)));
  setScale( controller->scale() ); // FIXME:

  // after completed 'recheckHighestVisibleClusterEnergy()', we call 'scale()' in the controller
  connect(this,SIGNAL(highestVisibleClusterEnergyChanged()),controller,SLOT(possibleChange_scale()));


//  // random jet colors
//  connect(controller,SIGNAL(rerandomise()),this,SLOT(rerandomise()));
//  connect(controller,SIGNAL(randomJetColoursChanged(const bool&)),this,SLOT(setRandomJetColours(const bool&)));
//  setRandomJetColours(controller->randomJetColours());

  // maxR
//  connect(controller, SIGNAL(maxRChanged(const double&)), this, SLOT(setMaxR(const double&)));
//  connect(controller, SIGNAL(signalMaxR(bool)), this, SLOT(setIsMaxR(bool)));

}

void IParticleCollHandle_CaloCluster::resetCachedValuesCuts()
{
	// kinetic cuts
	setCutAllowedPt(d->collSettingsButton->cutAllowedPt());
	setCutAllowedEta(d->collSettingsButton->cutAllowedEta());
	setCutAllowedPhi(d->collSettingsButton->cutAllowedPhi());
	// other settings
	setScale( d->collSettingsButton->scale() ); // FIXME:

}

//____________________________________________________________________
//SoMaterial* IParticleCollHandle_CaloCluster::defaultParameterMaterial() const {
//	return d->defaultParametersMaterial;
//}


//____________________________________________________________________
const CaloClusterCollectionSettingsButton& IParticleCollHandle_CaloCluster::collSettingsButton() const {
  if (!d->collSettingsButton){
    messageVerbose("CaloCluster - No collSettingsButton set! Can't call init(), so crash is imminent...");
  }
  return *d->collSettingsButton;
}



//____________________________________________________________________
void IParticleCollHandle_CaloCluster::rebuildAllObjects()
{
	VP1Msg::messageVerbose("IParticleCollHandle_CaloCluster::rebuildAllObjects()");

	recheckCutStatusOfAllHandles();
	recheckCutStatusOfAllNotVisibleHandles(); // Redundant, but needed! TODO: remove this when new 'contains' is ready
}


//____________________________________________________________________
bool IParticleCollHandle_CaloCluster::isConsiderTransverseEnergy() const
{
  VP1Msg::messageVerbose("IParticleCollHandle_CaloCluster::isConsiderTransverseEnergy()");
  return d->collSettingsButton->isTransverseEnergy();
}

////____________________________________________________________________
//void IParticleCollHandle_CaloCluster::setTransverseEnergyForCuts(bool var){
//  VP1Msg::messageVerbose("IParticleCollHandle_CaloCluster::setTransverseEnergyForCuts()");
//  d->collSettingsButton->isTransverseEnergy();
//}

//____________________________________________________________________
void IParticleCollHandle_CaloCluster::setShowVolumeOutLines(bool b)
{
	VP1Msg::messageDebug("IParticleCollHandle_CaloCluster::setShowVolumeOutLines()");

  if (d->showOutlines==b)
    return;

  d->showOutlines=b;

  if (!isLoaded())
    return;

//  largeChangesBegin();
//  foreach(Imp::ClusterHandle*cluster,d->clusters)
//    if (cluster->genericBox())
//      cluster->genericBox()->drawEdgeLines = b;
//  largeChangesEnd();


  largeChangesBegin();
  handleIterationBegin();
  AODHandleBase* handle=0;
  while ((handle=getNextHandle()))
  {
	  IParticleHandle_CaloCluster* cluster = dynamic_cast<IParticleHandle_CaloCluster*>(handle);
	  if (cluster && cluster->has3DObjects()) {
		  if (cluster->genericBox())
		        cluster->genericBox()->drawEdgeLines = b;
	  } else {
		  message("ERROR Handle of wrong type!");
	  }
  }
  largeChangesEnd();


}



////____________________________________________________________________
//bool IParticleCollHandle_CaloCluster::isRandomColors() const
//{
//  VP1Msg::messageVerbose("IParticleCollHandle_CaloCluster::isRandomColors()");
//  return d->randomColours;
//}

////____________________________________________________________________
//bool IParticleCollHandle_CaloCluster::isMaxR() const
//{
//	VP1Msg::messageVerbose("IParticleCollHandle_CaloCluster::isMaxR()");
//	return d->isMaxR;
//}


////____________________________________________________________________
//void IParticleCollHandle_CaloCluster::setRandomJetColours(const bool& b)
//{
//  messageVerbose("IParticleCollHandle_CaloCluster::setRandomJetColours() - b: "+QString::number(b));
//
//  if (d->randomColours == b)
//    return;
//
//  d->randomColours = b;
//  std::cout << "isRandom: " << d->randomColours << std::endl;;
//
//  if (!isLoaded())
//    return;
//
//  largeChangesBegin();
//  int ijet = 0;
//  handleIterationBegin();
//  AODHandleBase* handle=0;
//  while ((handle=getNextHandle()))
//  {
//    ++ijet;
//    IParticleHandle_CaloCluster* jet = dynamic_cast<IParticleHandle_CaloCluster*>(handle);
//    if (jet && jet->has3DObjects()) {
//      messageVerbose("considering jet: "+QString::number(ijet));
//      int randomColors = isRandomColors();
//      messageVerbose("setting random material on jet: "+QString::number(ijet)+" - random colors? "+QString::number(randomColors));
//      //			std::cout << "isRandomColors() --> " << isRandomColors() << std::endl;
//      jet->updateMaterial( randomColors );
//
//    } else {
//      message("ERROR Handle of wrong type!");
//    }
//  }
//  largeChangesEnd();
//}

////____________________________________________________________________
//void IParticleCollHandle_CaloCluster::rerandomise()
//{
//  messageVerbose("IParticleCollHandle_CaloCluster::rerandomise()");
//
//  if (!isLoaded()) {
//    messageVerbose("IParticleCollHandle_CaloCluster::rerandomise() - Returning.");
//    return;
//  }
//  if (!isRandomColors()) {
//    VP1Msg::message("'Random jet colours' is not checked. Nothing to do. Returning.");
//    return;
//  }
//
//  largeChangesBegin();
//  handleIterationBegin();
//  unsigned int ijet;
//  AODHandleBase* handle=0;
//  while ((handle=getNextHandle()))
//  {
//    ++ijet;
//    IParticleHandle_CaloCluster* jet = dynamic_cast<IParticleHandle_CaloCluster*>(handle);
//    if (jet && jet->has3DObjects()) {
//      jet->rerandomiseMaterial();
//    } else {
//      message("ERROR Handle of wrong type!");
//    }
//  }
//  largeChangesEnd();
//}

//____________________________________________________________________
bool IParticleCollHandle_CaloCluster::load()
{
  messageVerbose("loading CaloCluster collection");

  //Get collection:
  const xAOD::CaloClusterContainer * coll(0);
  if (!VP1SGAccessHelper(systemBase()).retrieve(coll, name())) {
    message("Error: Could not retrieve collection with key="+name());
    return false;
  }

  // // Retrieve the xAOD particles:
  //  const xAOD::JetContainer* xaod = evtStore()->retrieve<const xAOD::JetContainer>( m_JetCollection );

  // Check that the auxiliary store association was made successfully:
  if( ! coll->hasStore() ) {
    message("No auxiliary store got associated to the CaloCluster container with key: " +name());
  }

  // This is needed for now, until the issues around the DV code are
  // sorted out...
  const_cast< xAOD::CaloClusterContainer* >( coll )->setStore(
    ( SG::IAuxStore* ) coll->getConstStore() );

  //Make appropriate CaloCluster handles:
  // hintNumberOfTracksInEvent(coll->size());
  xAOD::CaloClusterContainer::const_iterator it, itEnd = coll->end();
  for ( it = coll->begin() ; it != itEnd; ++it) {
    d->possiblyUpdateGUI();
    if (!*it) {
      messageDebug("WARNING: Ignoring null CaloCluster pointer.");
      continue;
    }
    /*
    if ((*it)->charge()==0.0) {
    messageDebug("WARNING: Ignoring  which claims to be neutral (charge()==0.0).");
    continue;
    }
    */
    addHandle(new IParticleHandle_CaloCluster(this,*it));
  }

  // get handles list and update locally
  // d->jets = this->getHandlesList();

  return true;
}

//Cuts:
//____________________________________________________________________
bool IParticleCollHandle_CaloCluster::cut(AODHandleBase* c) {
  messageVerbose("IParticleCollHandle_CaloCluster::cut()");

  if (IParticleHandle_CaloCluster* handle = dynamic_cast<IParticleHandle_CaloCluster*>(c))
  {

	  message("CaloCluster::cut() --- to be implemented......");


    //------ ENERGY CUTS
    double jEnergy = handle->energyForCuts();
    double jEnergySqr = jEnergy * jEnergy; // in IParticleCollHandleBase the allowedPt is stored as squared, to avoid square-root operations
    messageVerbose("object's energy, object's energy^2, allowedPt^2, isPtAll: " + QString::number(jEnergy) + " - " + QString::number(jEnergySqr) + " - " + getCutAllowedPt().toString() + " - " + QString::number(int(getPtAllowall()))  );
    if (!getPtAllowall() && !getCutAllowedPt().contains(jEnergySqr) ) {
      messageVerbose("Energy cut not passed");
      return false;
    }

    //------ ETA CUTS
    messageVerbose("object's eta, etaCut, EtaAll: " + QString::number(handle->eta()) + " - " + getCutAllowedEta().toString() + " - " + QString::number(int(getEtaAllowall())) );
    if (!getEtaAllowall() && !getCutAllowedEta().contains(handle->eta())) {
      messageVerbose("Eta cut not passed");
      return false;
    }

    //------ PHI CUTS
    if (!getPhiAllowall() ) {
      double phi = handle->phi();
      bool ok(false);
      foreach (VP1Interval phirange, getCutAllowedPhi() ) {
        messageVerbose("object's phi, phiCut, PhiAll: " + QString::number(phi)  + " - " + phirange.toString() + " - " + QString::number(int(getPhiAllowall())) );
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
//
//    //------ OTHER CUTS
//    messageVerbose("Other cuts??? Calling base method...");
//    if (!IParticleCollHandleBase::cut(handle))
//      return false;

  } // if(dynamic_cast)
  else{
    messageVerbose("Not a IParticleCollHandle_CaloCluster. Calling base method...");
    if (!IParticleCollHandleBase::cut(handle))
      return false;
  }

  return true; //default

}



//____________________________________________________________________
void IParticleCollHandle_CaloCluster::showParametersChanged(bool val) {
	messageDebug("IParticleCollHandle_CaloCluster::showParametersChanged to "+str(val));
  update3DObjectsOfAllHandles();
}


//____________________________________________________________________
double IParticleCollHandle_CaloCluster::highestVisibleClusterEnergy() const
{
	messageDebug("IParticleCollHandle_CaloCluster::highestVisibleClusterEnergy()");
  return d->last_highestEnergy;
}


//____________________________________________________________________
double IParticleCollHandle_CaloCluster::calculateHighestVisibleClusterEnergy()
{
	messageDebug("IParticleCollHandle_CaloCluster::calculateHighestVisibleClusterEnergy()");
	 if (!isLoaded())
		 messageDebug("Not loaded. Returning...");
	    return 0;

  double e = 0;
//  if (this->visible()) {
//	  largeChangesBegin(); // not needed here...
	    handleIterationBegin();
	    AODHandleBase* handle=0;
	    while ((handle=getNextHandle()))
	    {
	  	  IParticleHandle_CaloCluster* cluster = dynamic_cast<IParticleHandle_CaloCluster*>(handle);
	  	  if (cluster && cluster->has3DObjects()) {
//	  		if ( e < cluster->energyForLengthAndCuts(this) )
//	  			e = cluster->energyForLengthAndCuts(this);
	  		if ( e < cluster->energyForCuts() )
	  			e = cluster->energyForCuts();
	  	  } else {
	  		  message("ERROR Handle of wrong type!");
	  	  }
	    }
//	    largeChangesEnd();


//    foreach(Imp::ClusterHandle*cluster,clusters) {
//      if (cluster->attached()&&e<cluster->energyForLengthAndCuts(this))
//	e=cluster->energyForLengthAndCuts(this);
//    }

//  }
  return e;
}

//____________________________________________________________________
void IParticleCollHandle_CaloCluster::recheckHighestVisibleClusterEnergy()
{
  //When visibility or cut changes:
  double newhighest = d->calculateHighestVisibleClusterEnergy();
  if (d->last_highestEnergy == newhighest)
    return;
  d->last_highestEnergy = newhighest;
  emit highestVisibleClusterEnergyChanged();
}


////____________________________________________________________________
//void IParticleCollHandle_CaloCluster::setBTaggingEnabled(const bool& flag) {
//  messageVerbose("IParticleCollHandle_CaloCluster::setBTaggingEnabled - "+str(flag));
//  d->bTaggingSwitch->whichChild = (flag ? SO_SWITCH_ALL : SO_SWITCH_NONE);
//  if (flag)
//    setBTaggingMaterialChanged(true); // we set default "Material"
//}


////____________________________________________________________________
//void IParticleCollHandle_CaloCluster::setBTaggingSkin(const QString &filename){
//
//  if (d->collSettingsButton->bTaggingRenderingSkin()) {
//
//    //remove the previous skin and material
//    d->bTaggingSwitch->removeChild(d->bTaggingTexture);
//    d->bTaggingSwitch->removeChild(d->bTaggingMaterial);
//    //		delete d->bTaggingTexture;
//    //		delete d->bTaggingMaterial;
//    d->bTaggingTexture = 0;
//    d->bTaggingMaterial = 0;
//
//    // setting the texture
//    d->bTaggingTexture = new SoTexture2;
//    d->bTaggingMaterial = new SoMaterial;
//
//    // original from Joe, for skins/textures
//    std::string fName = PathResolver::find_file (filename.toStdString()+".png", "DATAPATH");
//    messageVerbose("texture found: " + QString::fromStdString(fName) );
//    d->bTaggingTexture->filename.setValue(fName.c_str());
//
//    // adding the texture to the SoSwitch
//    d->bTaggingSwitch->addChild(d->bTaggingTexture);
//  }
//
//  updateBTaggingSwitchAllJets(); // update switch all jets
//  updateBTaggingAllJets(); // update cut for all jets
//}

////____________________________________________________________________
//void IParticleCollHandle_CaloCluster::setBTaggingMaterial(SoMaterial* mat)
//{
//  messageVerbose("IParticleCollHandle_CaloCluster::setBTaggingMaterial()");
//
//  if (d->collSettingsButton->bTaggingRenderingMaterial()) {
//
//    messageVerbose("Updating the material node");
//
//    //std::cout << "Updating - old mat: " << d->bTaggingMaterial << "..." << std::endl; // it continues below...
//
//    //remove the previous skin and material
//    d->bTaggingSwitch->removeChild(d->bTaggingTexture);
//    d->bTaggingSwitch->removeChild(d->bTaggingMaterial);
//    //		delete d->bTaggingTexture;
//    //		delete d->bTaggingMaterial;
//    d->bTaggingTexture = 0;
//    d->bTaggingMaterial = 0;
//
//    //		float r = 0.4; float g = 0.15; float b = 0.0; float br = 0.8; float tr = 0.3; // reddish color
//    //		VP1MaterialButton::setMaterialParameters( mat, r, g, b, br /*brightness*/, tr /*transparency*/ );
//
//    if (! mat) {
//      messageVerbose("taking the material from the controller");
//      d->bTaggingMaterial = d->collSettingsButton->bTaggingMaterial();
//      d->bTaggingSwitch->addChild( d->bTaggingMaterial );
//    }
//    else {
//      messageVerbose("taking the argument material");
//      d->bTaggingMaterial = mat;
//      d->bTaggingSwitch->addChild( d->bTaggingMaterial );
//    }
//
//    //std::cout << "--> new mat: " << d->bTaggingMaterial << std::endl;
//
//    updateBTaggingSwitchAllJets(); // update switch all jets
//    updateBTaggingAllJets(); // update cut for all jets
//  }
//}


////____________________________________________________________________
//void IParticleCollHandle_CaloCluster::setBTaggingTagger(const QString & tagger){
//
//  //	std::cout << "current bTaggingTagger: " << d->bTaggingTagger << " - new: " << tagger << std::endl;
//
//  if (d->bTaggingTagger == tagger.toStdString())
//    return;
//
//  d->bTaggingTagger = tagger.toStdString();
//
//  if (!isLoaded())
//    return;
//
//  messageVerbose("BTaggingTagger change to: " +tagger+ " (with cut: " + QString::number(d->bTaggingCut) + "). Updating "+str(getHandlesList().count())+" jets");
//
//  updateBTaggingAllJets(); // update all jets
//}


////____________________________________________________________________
//void IParticleCollHandle_CaloCluster::setBTaggingCut(const double& wCut){
//
//  std::cout << d->bTaggingCut << std::endl;
//
//  if (d->bTaggingCut == wCut)
//    return;
//
//  d->bTaggingCut = wCut;
//
//  if (!isLoaded())
//    return;
//
//  messageVerbose("BTaggingCut change to "+str(d->bTaggingCut)+". Updating "+str(getHandlesList().count())+" jets");
//
//  updateBTaggingAllJets(); // update all jets
//
//}

////____________________________________________________________________
//void IParticleCollHandle_CaloCluster::updateBTaggingAllJets()
//{
//  messageVerbose("IParticleCollHandle_CaloCluster::updateBTaggingAllJets()");
//  largeChangesBegin();
//  handleIterationBegin();
//  AODHandleBase* handle=0;
//  while ((handle=getNextHandle()))
//  {
//    IParticleHandle_CaloCluster* jet = dynamic_cast<IParticleHandle_CaloCluster*>(handle);
//    if (jet && jet->has3DObjects()) {
//      jet->updateBTagging(d->bTaggingTagger, d->bTaggingCut);
//    } else {
//      message("ERROR Handle of wrong type!");
//    }
//  }
//
//
//  largeChangesEnd();
//}

////____________________________________________________________________
//void IParticleCollHandle_CaloCluster::updateBTaggingSwitchAllJets()
//{
//  messageVerbose("IParticleCollHandle_CaloCluster::updateBTaggingSwitchAllJets()");
//  largeChangesBegin();
//  handleIterationBegin();
//  AODHandleBase* handle=0;
//  while ((handle=getNextHandle()))
//  {
//    IParticleHandle_CaloCluster* jet = dynamic_cast<IParticleHandle_CaloCluster*>(handle);
//    if (jet && jet->has3DObjects()) {
//      jet->updateBTaggingSwitch(d->bTaggingSwitch);
//    } else {
//      message("ERROR Handle of wrong type!");
//    }
//  }
//  largeChangesEnd();
//}
//
//
////____________________________________________________________________
//void IParticleCollHandle_CaloCluster::setBTaggingMaterialChanged(const bool& ok)
//{
//  if (ok) {
//    messageVerbose("IParticleCollHandle_CaloCluster::setBTaggingMaterialChanged()");
//    setBTaggingMaterial();
//  }
//}
//
////____________________________________________________________________
//void IParticleCollHandle_CaloCluster::setBTaggingSkinChanged(const bool& ok) {
//  if (ok)
//    setBTaggingSkin(d->collSettingsButton->bTaggingSkin());
//}


//____________________________________________________________________
QByteArray IParticleCollHandle_CaloCluster::persistifiableState() const
{
  messageDebug("IParticleCollHandle_CaloCluster::persistifiableState() - start...");

  // if (!d->matButton) {
  //   message("ERROR: persistifiableState() called before init()");
  //   return QByteArray();
  // }


  VP1Serialise serialise(1/*version*/);

  // settings
  serialise.disableUnsavedChecks();

  // SAVE THE CHECKED/UNCHECKED STATUS OF THE COLLECTION
  serialise.save(visible());

  // SAVE THE MATERIAL BUTTON
  //Q_ASSERT(d->matButton&&"Did you forget to call init() on this VP1StdCollection?");
  //serialise.save(d->matButton->saveState());

  // SAVE THE EXTRA-STATES
  serialise.save(extraWidgetsState());//version 1+

  // SAVE MATERIAL SETTINGS / CUTS
  serialise.save(d->collSettingsButton->saveState());

  messageDebug("IParticleCollHandle_CaloCluster::persistifiableState() - end.");
  return serialise.result();
}

//____________________________________________________________________
void IParticleCollHandle_CaloCluster::setState(const QByteArray&state)
{
  VP1Deserialise des(state);
  des.disableUnrestoredChecks();
  if (des.version()!=0&&des.version()!=1) {
    messageDebug("Warning: Ignoring state with wrong version");
    return;
  }

  // save state

  bool vis = des.restoreBool();

  //	QByteArray matState = des.restoreByteArray();
  // d->matButton->restoreFromState(matState);
  QByteArray extraWidgetState = des.version()>=1 ? des.restoreByteArray() : QByteArray();
  setVisible(vis);

  if (extraWidgetState!=QByteArray())
    setExtraWidgetsState(extraWidgetState);

  // MATERIAL SETTINGS / CUTS
  messageDebug("restoring material collection button...");
  des.restore(d->collSettingsButton);

  messageDebug("reset all caches storing values for cuts...");
  resetCachedValuesCuts();

  messageDebug("recheck all handles...");
  recheckCutStatusOfAllVisibleHandles();
}

void IParticleCollHandle_CaloCluster::dumpToJSON( std::ofstream& str) const {
  str << "\""<<name().toLatin1().data()<<"\":{";
  
  unsigned int num=0;
  for (auto handle : getHandlesList() ) {
    if (handle->visible()) {
      if (num) str <<",\n";
      str << "\"Clus "<<num++<< "\":{";
      handle->dumpToJSON(str);
      str << "}";
    }
  }
  
  str << "}";
}

