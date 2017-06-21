/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class MissingEtCollHandle           //
//                                                            //
//  Author: Riccardo-Maria BIANCHI (rbianchi@cern.ch)         //
//  Initial version: May 2014                                 //
//                                                            //
////////////////////////////////////////////////////////////////

//Local
#include "MissingEtCollHandle.h"

#include "AODHandleBase.h"
#include "AODCollHandleBase.h"
#include "AODSysCommonData.h"

//xAOD
//#include "xAODJet/MissingETContainer.h"
#include "xAODMissingET/MissingETContainer.h"


//VP1
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/VP1Interval.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Utils/VP1SGContentsHelper.h"

//Qt
#include <QStringList>

// SoCoin
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoMaterial.h>

#include "MissingEtCollectionSettingsButton.h"
#include "MissingEtHandle.h"
//#include "Inventor/nodes/SoDrawStyle.h"
//#include "Inventor/nodes/SoLightModel.h"


//____________________________________________________________________
QStringList MissingEtCollHandle::availableCollections( IVP1System*sys )
{
  return VP1SGContentsHelper(sys).getKeys<xAOD::MissingETContainer>();
}

//____________________________________________________________________
class MissingEtCollHandle::Imp {
public:
  MissingEtCollHandle * theclass;
  int updateGUICounter;
  MissingEtCollectionSettingsButton* collSettingsButton;

  //Vector of iParticle handles:
  std::vector<MissingEtHandle*> handles;
  // QList of xAOD handles
  QList<AODHandleBase*> handlesList;

  //For iteration:
  std::vector<MissingEtHandle*>::iterator itHandles;
  std::vector<MissingEtHandle*>::iterator itHandlesEnd;

  // settings
  double scale = 1.; // the default scale of all jets
  bool randomColours = false;

  //	SoMaterial*        defaultParametersMaterial; /// This will be given to the VP1MaterialButton in

  void possiblyUpdateGUI() {//Fixme: to AODCollHandleBase
    if (!((updateGUICounter++)%50)) {
      theclass->systemBase()->updateGUI();
    }
  }
};

//____________________________________________________________________
MissingEtCollHandle::MissingEtCollHandle(AODSysCommonData * cd, const QString& name, xAOD::Type::ObjectType type)
  : AODCollHandleBase(cd,name,type), d(new Imp), m_cut_phi_allowall{}, m_cut_phi_allwillfail{}
{
  d->theclass = this;
  d->updateGUICounter = 0;
  d->collSettingsButton=0;


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
  //  d->defaultParametersMaterial->setName(("MissingEtCollHandle"+safetext).toStdString().c_str());
  //  d->defaultParametersMaterial->ref();
  // d->matButton->setDefaultParameterMaterial(d->defaultParametersMaterial); FIXME
}

//____________________________________________________________________
MissingEtCollHandle::~MissingEtCollHandle()
{
  //	d->defaultParametersMaterial->unref();

  // clean the QList<handle>
  foreach(AODHandleBase* handle, d->handles) {
    delete handle;
  }

  // TODO: implemet this del
  //	if (d->bTaggingSwitch) d->bTaggingSwitch->unref();

  delete d;
}

//____________________________________________________________________
void MissingEtCollHandle::init(VP1MaterialButtonBase*)
{
  // std::cout<<"MissingEtCollHandle::init 1"<<std::endl;

  //	d->matButton = new MissingEtCollectionSettingsButton; // TODO: update for jet
  //	d->matButton->setMaterialText(name());
  d->collSettingsButton = new MissingEtCollectionSettingsButton;
  d->collSettingsButton->setMaterialText(name());

  // std::cout<<"Calling VP1StdCollection::init with d->matButton (MissingEtCollectionSettingsButton)="<<d->matButton<<std::endl;
  VP1StdCollection::init(d->collSettingsButton);//this call is required. Passing in d->collSettingsButton means we have the more complex button.
  setupSettingsFromController(common()->controller());
  connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(collVisibilityChanged(bool)));

  // std::cout<<"MissingEtCollHandle::init 2"<<std::endl;
  // std::cout<<"swi: "<<collSwitch()<<std::endl;
  // std::cout<<"sep: "<<collSep()<<std::endl;
  // std::cout<<"mat: "<<material()<<std::endl;

  //	collSwitch()->addChild(d->collSettingsButton->trackLightModel()); // TODO: update for jets
  //	collSwitch()->addChild(d->collSettingsButton->trackDrawStyle()); // TODO: update for jets
}

//____________________________________________________________________
void MissingEtCollHandle::hintNumberOfHandlesInEvent(unsigned n)
{
  d->handles.reserve(n);
}

//____________________________________________________________________
void MissingEtCollHandle::addHandle(AODHandleBase* ah)
{
  MissingEtHandle* handle = dynamic_cast<MissingEtHandle* >(ah);
  if (!handle) {
    message("ERROR - wrong handle type passed to MissingEtCollHandle::addHandle!");
    return;
  }
  d->handles.push_back(handle); // for the vector<handle>
  d->handlesList << handle; // for the QList<handle>
}

//____________________________________________________________________
void MissingEtCollHandle::handleIterationBegin()
{
  d->itHandles = d->handles.begin();
  d->itHandlesEnd = d->handles.end();
}

//____________________________________________________________________
AODHandleBase* MissingEtCollHandle::getNextHandle() {
  if (d->itHandles==d->itHandlesEnd)
    return 0;
  else
    return *(d->itHandles++);
}

//____________________________________________________________________
QList<AODHandleBase*> MissingEtCollHandle::getHandlesList() const
{
  messageVerbose("AODCollHandleBase::getHandlesList()");
  return d->handlesList;
}



void MissingEtCollHandle::setupSettingsFromControllerSpecific(AODSystemController*) {

  //cuts  
  //	connect(d->collSettingsButton,SIGNAL(cutAllowedPtChanged(const VP1Interval&)),this,SLOT(setCutAllowedPt(const VP1Interval&)));
  //	setCutAllowedPt(d->collSettingsButton->cutAllowedPt());
  //
  //	connect(d->collSettingsButton,SIGNAL(cutAllowedEtaChanged(const VP1Interval&)),this,SLOT(setCutAllowedEta(const VP1Interval&)));
  //	setCutAllowedEta(d->collSettingsButton->cutAllowedEta());
  //
  //	connect(d->collSettingsButton,SIGNAL(cutAllowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(setCutAllowedPhi(const QList<VP1Interval>&)));
  //	setCutAllowedPhi(d->collSettingsButton->cutAllowedPhi());

  //	// Parameters
  //	connect(d->collSettingsButton,SIGNAL(showParametersChanged(bool)),        this,SLOT(showParametersChanged(bool)));
  //	connect(d->collSettingsButton,SIGNAL(colourParametersByTypeChanged(bool)),this,SLOT(showParametersChanged(bool)));
  // Just reusing the same slot, since it doesn't

  // scale
  //connect(d->collSettingsButton,SIGNAL(scaleChanged(const double&)),this,SLOT(setScale(const double&)));
  //setScale( this->scale() );

  // MET length and thickness
  connect(d->collSettingsButton,SIGNAL(metSizeChanged(int)),this,SLOT(setMetSize(int)));
  setMetSize(d->collSettingsButton->metLength());

  // Phi cut
  connect(d->collSettingsButton,SIGNAL(cutAllowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(setCutAllowedPhi(const QList<VP1Interval>&)));
  setCutAllowedPhi(d->collSettingsButton->cutAllowedPhi());

  // random jet colors
  //	connect(d->collSettingsButton,SIGNAL(rerandomise()),this,SLOT(rerandomise()));
  //	connect(d->collSettingsButton,SIGNAL(randomJetColoursChanged(const bool&)),this,SLOT(setRandomJetColours(const bool&)));
  //	setRandomJetColours(d->collSettingsButton->randomJetColours());
}


void MissingEtCollHandle::resetCachedValuesCuts()
{
	// TODO: it is not used so far! Check Other collections and update accordingly

	// kinetic cuts
	setCutAllowedPhi(d->collSettingsButton->cutAllowedPhi());
	// other settings
	setMetSize(d->collSettingsButton->metLength());

}

//SoMaterial* MissingEtCollHandle::defaultParameterMaterial() const {
//	return d->defaultParametersMaterial;
//}


const MissingEtCollectionSettingsButton& MissingEtCollHandle::collSettingsButton() const {
  if (!d->collSettingsButton){
    messageVerbose("MET - No collSettingsButton set! Can't call init(), so crash is imminent...");
  }
  return *d->collSettingsButton;
}



////____________________________________________________________________
//void MissingEtCollHandle::setScale(const double& sca)
//{
//
//  messageVerbose("MissingEtCollHandle::setScale() - scale: " + QString::number(sca));
//
//  if (d->handlesList.isEmpty()) {
//    messageVerbose("no jet handles defined! returning.");
//    return;
//  }
//
//  if (d->scale == sca)
//    return;
//
//  d->scale = std::max(1*Gaudi::Units::mm/(100*Gaudi::Units::GeV),
//  std::min(99*Gaudi::Units::m/(1*Gaudi::Units::MeV),
//  //						d->collSettingsButton->lengthOf100GeV() * Gaudi::Units::m/(100.0*Gaudi::Units::GeV)));
//  sca * Gaudi::Units::m/(100.0*Gaudi::Units::GeV)));
//
//  if (!isLoaded())
//    return;
//
//  messageVerbose("Scale change: to "+str(d->scale/(Gaudi::Units::m/(100.0 * Gaudi::Units::GeV)))+" m/100GeV. Updating "+str(d->handlesList.count())+" jets");
//  std::cout << "Scale change: d->scale/(Gaudi::Units::m/(100.0*Gaudi::Units::GeV)))" <<  "m/100GeV. Updating " << d->handlesList.count() << " jets" << std::endl;
//  largeChangesBegin();
//
//  foreach(AODHandleBase* partHandle, d->handles) {
//    MissingEtHandle* met = dynamic_cast<MissingEtHandle*>(partHandle);
//    if (met->has3DObjects()) {
//      met->setScale(d->scale);
//      met->updateHeight();
//    }
//  }
//  largeChangesEnd();
//}


void MissingEtCollHandle::setMetSize(int size)
{
  messageVerbose("signal received: metSizeChanged ("+str(size)+")");

  update3DObjectsOfAllHandles();
}


////____________________________________________________________________
//double MissingEtCollHandle::scale() const
//{
//  return d->scale;
//}


////____________________________________________________________________
//bool MissingEtCollHandle::isRandomColors() const
//{
//	VP1Msg::messageDebug("MissingEtCollHandle::isRandomColors()");
//	return d->randomColours;
//}
//
//
////____________________________________________________________________
//void MissingEtCollHandle::setRandomJetColours(const bool& b)
//{
//	messageVerbose("MissingEtCollHandle::setRandomJetColours() - b: "+QString::number(b));
//
//	if (d->randomColours == b)
//		return;
//
//	d->randomColours = b;
//	std::cout << "isRandom: " << d->randomColours << std::endl;;
//
//	if (!isLoaded())
//		return;
//
//	largeChangesBegin();
//	int ijet = 0;
//	foreach(AODHandleBase* partHandle, d->handles) {
//		++ijet;
//		messageVerbose("considering jet: "+QString::number(ijet));
//		MissingEtCollHandle* jet = dynamic_cast<MissingEtCollHandle*>(partHandle);
//		if (jet->has3DObjects()) {
//			int randomColors = isRandomColors();
//			messageVerbose("setting random material on jet: "+QString::number(ijet)+" - random colors? "+QString::number(randomColors));
////			std::cout << "isRandomColors() --> " << isRandomColors() << std::endl;
//			jet->updateMaterial( randomColors );
//		}
//	}
//	largeChangesEnd();
//}



////____________________________________________________________________
//void MissingEtCollHandle::rerandomise()
//{
//	messageVerbose("MissingEtCollHandle::rerandomise()");
//
//	if (!isLoaded()) {
//		messageVerbose("MissingEtCollHandle::rerandomise() - Returning.");
//		return;
//	}
//	if (!isRandomColors()) {
//		VP1Msg::message("'Random jet colours' is not checked. Nothing to do. Returning.");
//		return;
//	}
//
//	largeChangesBegin();
//	foreach(AODHandleBase* partHandle, d->handles) {
//		MissingEtCollHandle* jet = dynamic_cast<MissingEtCollHandle*>(partHandle);
//		if (jet->has3DObjects())
//			jet->rerandomiseMaterial();
//	}
//	largeChangesEnd();
//}







//____________________________________________________________________
bool MissingEtCollHandle::load()
{
  messageVerbose("loading MissingEt collection");

  //Get collection:
  const xAOD::MissingETContainer * coll(0);
  if (!VP1SGAccessHelper(systemBase()).retrieve(coll, name())) {
    message("Error: Could not retrieve MET collection with key="+name());
    return false;
  }

  // // Retrieve the xAOD particles:
  //  const xAOD::MissingETContainer* xaod = evtStore()->retrieve<const xAOD::MissingETContainer>( m_JetCollection );

  // Check that the auxiliary store association was made successfully:
  if( ! coll->hasStore() ) {
    message("No auxiliary store got associated to the MET container with key: " +name());
  }

  // This is needed for now, until the issues around the DV code are
  // sorted out...
  const_cast< xAOD::MissingETContainer* >( coll )->setStore(
    ( SG::IAuxStore* ) coll->getConstStore() );

  //Make appropriate Jet handles:
  // hintNumberOfTracksInEvent(coll->size());
  xAOD::MissingETContainer::const_iterator it, itEnd = coll->end();
  for ( it = coll->begin() ; it != itEnd; ++it) {
    d->possiblyUpdateGUI();
    if (!*it) {
      messageDebug("WARNING: Ignoring null MET pointer.");
      continue;
    }
    /*
    if ((*it)->charge()==0.0) {
    messageDebug("WARNING: Ignoring Jet which claims to be neutral (charge()==0.0).");
    continue;
    }
    */
    addHandle(new MissingEtHandle(this,*it));
  }

  return true;
}



//Cuts:
//____________________________________________________________________
bool MissingEtCollHandle::cut(AODHandleBase* c) {
  messageVerbose("MissingEtCollHandle::cut()");

  if (MissingEtHandle* handle = dynamic_cast<MissingEtHandle*>(c))
  {

    //		//------ ENERGY CUTS
    //		messageVerbose("jet's energy, allowedPt^2, PtAll: - " + QString::number(handle->energyForCuts()) + " - " + getCutAllowedPt().toString() + " - " + QString::number(int(getPtAllowall()))  );
    //		double jEnergy = handle->energyForCuts();
    //		double jEnergySqr = jEnergy * jEnergy; // in AODCollHandleBase the allowedPt is stored as squared, to avoid square-root operations
    //		if (!getPtAllowall() && !getCutAllowedPt().contains(jEnergySqr) ) {
    //			messageVerbose("Pt cut not passed");
    //			return false;
    //		}
    //
    //		//------ ETA CUTS
    //		messageVerbose("jet's eta, etaCut, EtaAll: " + QString::number(handle->eta()) + " - " + getCutAllowedEta().toString() + " - " + QString::number(int(getEtaAllowall())) );
    //		if (!getEtaAllowall() && !getCutAllowedEta().contains(handle->eta())) {
    //			messageVerbose("Eta cut not passed");
    //			return false;
    //		}
    //
    		//------ PHI CUTS
    		if (!getPhiAllowall() ) {
    			double phi = handle->phi();
    			bool ok(false);
    			foreach (VP1Interval phirange, getCutAllowedPhi() ) {
    				messageVerbose("MET phi, phiCut, PhiAll: " + QString::number(phi)  + " - " + phirange.toString() + " - " + QString::number(int(getPhiAllowall())) );
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


  } // if(dynamic_cast)
  else{
    messageVerbose("Not a MissingEtCollHandle. Check your code! Returning 'false'...");
//    if (! AODCollHandleBase::cut(c)) // AODCollHandleBase::cut is pure virtual, now
      return false;
  }

  return true; //default

}



////____________________________________________________________________
//void MissingEtCollHandle::showParametersChanged(bool val) {
//  messageVerbose("MissingEtCollHandle::showParametersChanged to "+str(val));
//  update3DObjectsOfAllHandles();
//}


//____________________________________________________________________
void MissingEtCollHandle::setCutAllowedPhi(const QList<VP1Interval>& allowedPhi)
{
  VP1Msg::messageVerbose("MissingEtCollHandle::setCutAllowedPhi() - signal received in setCutAllowedPhi.");

  if (m_cut_allowedPhi==allowedPhi)
    return;

  m_cut_allowedPhi = allowedPhi;

  m_cut_phi_allowall = m_cut_allowedPhi.count()==1 && m_cut_allowedPhi.at(0).isAllR();
  m_cut_phi_allwillfail = m_cut_allowedPhi.isEmpty(); // || m_cut_allowedPtSq.isEmpty() || m_cut_allowedEta.isEmpty();

  if (m_cut_phi_allowall) {
	  recheckCutStatusOfAllNotVisibleHandles();
//	  update3DObjectsOfAllHandles();
    return;
  }
  if (m_cut_phi_allwillfail) {
	  recheckCutStatusOfAllVisibleHandles();
//	  update3DObjectsOfAllHandles();
    return;
  }

  // Test: is the current interval larger than the previous one?
  //bool relaxcut = allowedPhi.contains(m_cut_allowedPhi); // TODO: implement this 'contains' function
  // Store the current interval
//  m_cut_allowedPhi = allowedPhi;

//    if (relaxcut)
//      recheckCutStatusOfAllNotVisibleHandles();
//    else
//      recheckCutStatusOfAllVisibleHandles();

  recheckCutStatusOfAllHandles();
  recheckCutStatusOfAllNotVisibleHandles(); // Redundant, but needed! TODO: remove this when new 'contains' is ready

  return;
}

//____________________________________________________________________
QByteArray MissingEtCollHandle::persistifiableState() const
{
  messageDebug("MissingEtCollHandle::persistifiableState() - start...");

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

  messageDebug("MissingEtCollHandle::persistifiableState() - end.");
  return serialise.result();
}

//____________________________________________________________________
void MissingEtCollHandle::setState(const QByteArray&state)
{
  messageDebug("MissingEtCollHandle::setState()");

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
