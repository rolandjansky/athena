/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackCollHandle_SimulationTracks       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/TrackCollHandle_SimulationTracks.h"
#include "VP1TrackSystems/TrackHandle_SimulationTrack.h"
#include "VP1TrackSystems/SimHitHandleBase.h"
#include "VP1TrackSystems/SimHitHandle_ForwardHit.h"
#include "VP1TrackSystems/SimBarCode.h"
#include "VP1TrackSystems/TrackSystemController.h"
#include "VP1Base/IVP1System.h"
#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1ParticleData.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "GeneratorObjects/McEventCollection.h"

#include "TrackRecord/TrackRecordCollection.h"

#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Utils/VP1SGAccessHelper.h"

#include "ForwardRegion_SimEv/SimulationHitCollection.h"
#include "ForwardRegion_SimEv/SimulationHit.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "CLHEP/Vector/LorentzVector.h"

#include <QStringList>
#include <QTime>

#include <iostream>

//____________________________________________________________________
class TrackCollHandle_SimulationTracks::Imp {
public:

  TrackCollHandle_SimulationTracks * theclass;
  bool loadHitLists(std::map<SimBarCode,SimHitList> & hitLists);

  template <class collT>
  void addHitCollections(std::map<SimBarCode,SimHitList> & hitLists);

  static SimHitHandleBase * createHitHandle( const SimulationHit& h ) { return new SimHitHandle_ForwardHit(&h); }

  int updateGUICounter;
  void possiblyUpdateGUI() {
    if (!((updateGUICounter++)%750)) {
      theclass->systemBase()->updateGUI();
    }
  }

  std::map<SimBarCode::ExtBarCode,int> extBarCode2pdg;
  bool cut_fromIROnly;
  bool cut_excludeBarcodeZero;
  bool cut_excludeNeutrals;

  bool displayAscObjs;
  void updateVisibleAssociatedObjects();

  static const int maxPdgCode = 1000000000;

};


//____________________________________________________________________
QStringList TrackCollHandle_SimulationTracks::availableCollections( IVP1System*sys )
{
  QStringList l;
  VP1SGContentsHelper sgcont(sys);

  l << sgcont.getKeys<SimulationHitCollection>();

  return l;
}

//____________________________________________________________________
TrackCollHandle_SimulationTracks::TrackCollHandle_SimulationTracks(TrackSysCommonData * cd,
							 const QString& name)
  : TrackCollHandleBase(cd,name,TrackType::SimulationTrack), m_d(new Imp)
{
  setHelperClassName("TrackCollHandle_SimulationTracks");
  m_d->theclass = this;
  m_d->updateGUICounter = 0;
  m_d->cut_fromIROnly = false;
  m_d->cut_excludeBarcodeZero = false;
  m_d->cut_excludeNeutrals = false;
  m_d->displayAscObjs = false;
}

//____________________________________________________________________
TrackCollHandle_SimulationTracks::~TrackCollHandle_SimulationTracks()
{
  delete m_d;
}

//____________________________________________________________________
void TrackCollHandle_SimulationTracks::setupSettingsFromControllerSpecific(TrackSystemController* controller)
{

  connect(controller,SIGNAL(cutExcludeBarcodeZeroChanged(bool)),this,SLOT(setCutExcludeBarcodeZero(bool)));
  setCutExcludeBarcodeZero(controller->cutExcludeBarcodeZero());

  connect(controller,SIGNAL(cutTruthExcludeNeutralsChanged(bool)),this,SLOT(setCutExcludeNeutrals(bool)));
  setCutExcludeNeutrals(controller->cutTruthExcludeNeutrals());

  connect(controller,SIGNAL(showTruthAscObjsChanged(bool)),this,SLOT(setShowAscObjs(bool)));
  setShowAscObjs(controller->showTruthAscObjs());
}

//____________________________________________________________________
void TrackCollHandle_SimulationTracks::setShowAscObjs(bool b)
{
  if (m_d->displayAscObjs==b)
    return;
  m_d->displayAscObjs=b;
  messageVerbose("Associated objects shown flag changed to " + str(b));
  m_d->updateVisibleAssociatedObjects();
}

//____________________________________________________________________
template <class collT>
void TrackCollHandle_SimulationTracks::Imp::addHitCollections(std::map<SimBarCode,SimHitList> & hitLists)
{
  std::map<SimBarCode,SimHitList>::iterator itHitList;

  theclass->message("Entered addHitCollections " + QString(typeid(collT).name()));
  VP1SGAccessHelper sgaccess(theclass->systemBase());

  foreach (QString key, VP1SGContentsHelper(theclass->systemBase()).getKeys<collT>()) {
    const collT * hitColl;
    theclass->message("Try to retrieve "+QString(typeid(collT).name())+" collection with key = "+key);
    if (!sgaccess.retrieve(hitColl, key)) {
	  theclass->message("Error: Could not retrieve "+QString(typeid(collT).name())+" collection with key = "+key);
	  continue;
    }
    theclass->messageVerbose("Retrieved hit collection of type "+QString(typeid(collT).name())+" with key = "+key);
    typename collT::const_iterator it, itE(hitColl->end());
    int itot(0), iadded(0);
    for (it=hitColl->begin();it!=itE;++it) {
	  ++itot;
	  possiblyUpdateGUI();
	  SimHitHandleBase * handle = createHitHandle(*it);
	  handle->cacheMomentum();
	  theclass->fixPDGCode(handle);
	  SimBarCode trackID = handle->simBarCode();
	  if (trackID.pdgCode()>maxPdgCode) {
	    continue;
	  }

	  itHitList = hitLists.find(trackID);
	  if ( itHitList == hitLists.end() ) {
	    SimHitList l;
	    l.push_back(std::pair<double,SimHitHandleBase*>(handle->hitTime(),handle));
	    hitLists[trackID] = l;
	  } else {
	    itHitList->second.push_back(std::pair<double,SimHitHandleBase*>(handle->hitTime(),handle));
	  }
	  ++iadded;
    }
    theclass->messageVerbose("  => used "+str(iadded)+" of "+str(itot)+" hits");
  }

}

//____________________________________________________________________
bool TrackCollHandle_SimulationTracks::Imp::loadHitLists(std::map<SimBarCode,SimHitList> & hitLists)
{
  //Fixme: Return false if we do not find at least one collection
  theclass->messageVerbose( "Searching " + str( hitLists.size() ) + " lists of sim. hits.");
  addHitCollections<SimulationHitCollection>(hitLists);

  if (theclass->verbose())
    theclass->messageVerbose( "Found " + str( hitLists.size() ) + " lists of sim. hits.");

  //Time to assign all simhits with known pdg code a charge:
  std::map<SimBarCode,SimHitList>::iterator it, itE(hitLists.end());
  for (it = hitLists.begin(); it!=itE; ++it) {
    if (it->first.unknownPdgCode())
      continue;
    bool ok;
    double charge = VP1ParticleData::particleCharge(it->first.pdgCode(),ok);
    if (!ok)
      continue;
    SimHitList::iterator itHit(it->second.begin()), itHitE(it->second.end());
    for (;itHit!=itHitE;++itHit)
      itHit->second->setCharge(charge);
    possiblyUpdateGUI();
  }

  //Sort hitLists:
  for (it = hitLists.begin(); it!=itE; ++it) {
    sort(it->second.begin(),it->second.end());
    possiblyUpdateGUI();
  }
  return true;
}


//____________________________________________________________________
bool TrackCollHandle_SimulationTracks::load()
{

  //get sim hits and track records:
  std::map<SimBarCode,SimHitList> hitLists;
  if (!m_d->loadHitLists(hitLists))
    return false;
  messageVerbose("TrackCollHandle_SimulationTracks "+name()
       +": Found "+str(hitLists.size())+" truth particles from simhits");

  //Finally we need to combine the info we thus found, and construct
  //actual track handles:

  std::map<SimBarCode,SimHitList>::iterator itHitList, itHitListEnd(hitLists.end()), itHitListTemp;

  for (itHitList = hitLists.begin();itHitList!=itHitListEnd;++itHitList) {
    if (itHitList->second.empty()) {
      message("load WARNING: Ignoring empty hit list.");
      continue;
    }
    addTrackHandle( new TrackHandle_SimulationTrack( this, itHitList->first, itHitList->second ) );
  }

  //Maybe we need to show measurements, etc.:
  m_d->updateVisibleAssociatedObjects();

  return true;
}

//____________________________________________________________________
void TrackCollHandle_SimulationTracks::fixPDGCode(SimHitHandleBase* handle) const
{
  int pdgfromsimhit =handle->actualPDGCodeFromSimHit();
  bool isNonUniqueSecondary = handle->simBarCode().isNonUniqueSecondary();
  SimBarCode::ExtBarCode extBarCode = handle->simBarCode().extBarCode();

  if (pdgfromsimhit!=SimBarCode::unknownPDG) {
    handle->setPDG(handle->actualPDGCodeFromSimHit());
    std::map<SimBarCode::ExtBarCode,int>::const_iterator it = m_d->extBarCode2pdg.find(extBarCode);
    if ( !isNonUniqueSecondary && it==m_d->extBarCode2pdg.end())
      m_d->extBarCode2pdg[extBarCode] = pdgfromsimhit;
    return;
  }
  if (isNonUniqueSecondary)
    return;
  std::map<SimBarCode::ExtBarCode,int>::const_iterator it = m_d->extBarCode2pdg.find(extBarCode);
  if (it!=m_d->extBarCode2pdg.end()) {
    handle->setPDG(it->second);
  }
}

//____________________________________________________________________
bool TrackCollHandle_SimulationTracks::cut(TrackHandleBase* handle)
{
  if (!TrackCollHandleBase::cut(handle))
    return false;

  if (m_d->cut_excludeNeutrals && handle->hasCharge() && handle->charge()==0.0)
    return false;

  TrackHandle_SimulationTrack * truthhandle = static_cast<TrackHandle_SimulationTrack *>(handle);
  if (m_d->cut_excludeBarcodeZero && truthhandle->hasBarCodeZero())
    return false;

  return true;
}

//____________________________________________________________________
void TrackCollHandle_SimulationTracks::setCutExcludeBarcodeZero(bool b)
{
  if (m_d->cut_excludeBarcodeZero==b)
    return;
  m_d->cut_excludeBarcodeZero=b;
  if (b)
    recheckCutStatusOfAllVisibleHandles();
  else
    recheckCutStatusOfAllNotVisibleHandles();
}

//____________________________________________________________________
void TrackCollHandle_SimulationTracks::setCutExcludeNeutrals(bool b)
{
  if (m_d->cut_excludeNeutrals==b)
    return;
  m_d->cut_excludeNeutrals=b;
  if (b)
    recheckCutStatusOfAllVisibleHandles();
  else
    recheckCutStatusOfAllNotVisibleHandles();
}


//If has momentum - Check light-speed consistency given positions and time.
//mom should be decreasing

//____________________________________________________________________
void TrackCollHandle_SimulationTracks::Imp::updateVisibleAssociatedObjects()
{
      
  theclass->message("updateVisibleAssociatedObjects");//fixme
  theclass->largeChangesBegin();
  theclass->trackHandleIterationBegin();
  TrackHandle_SimulationTrack* handle;
  while ((handle=static_cast<TrackHandle_SimulationTrack*>(theclass->getNextTrackHandle()))) {
    handle->setAscObjsVisible(displayAscObjs);
  }
  theclass->largeChangesEnd();
}

