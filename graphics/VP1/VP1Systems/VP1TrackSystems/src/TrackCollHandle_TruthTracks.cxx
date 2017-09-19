/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackCollHandle_TruthTracks       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/TrackCollHandle_TruthTracks.h"
#include "VP1TrackSystems/TrackHandle_TruthTrack.h"
#include "VP1TrackSystems/SimHitHandleBase.h"
#include "VP1TrackSystems/SimHitHandle_TrackRecord.h"
#include "VP1TrackSystems/SimHitHandle_TRTHit.h"
#include "VP1TrackSystems/SimHitHandle_SiHit.h"
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

#include "InDetSimEvent/SiHitCollection.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "CLHEP/Vector/LorentzVector.h"

#include <QStringList>
#include <QTime>


//____________________________________________________________________
class TrackCollHandle_TruthTracks::Imp {
public:
  double mag(const HepMC::FourVector& v) const {
    return std::sqrt( v.x()*v.x() + v.y()*v.y() + v.z()*v.z() );
  }

  TrackCollHandle_TruthTracks * theclass;
  bool loadHitLists(std::map<SimBarCode,SimHitList> & hitLists);
  void loadGenParticles( std::map<SimBarCode,const HepMC::GenParticle*> & genParticles,
			 const HepMC::GenVertex* vtx );
  bool loadGenParticles( std::map<SimBarCode,const HepMC::GenParticle*> & genParticles,
			 const QString& hepMcCollKey );

  template <class collT>
  void addHitCollections(std::map<SimBarCode,SimHitList> & hitLists);

//  static SimHitHandleBase * createHitHandle( const TrackRecord * h ) { return new SimHitHandle_TrackRecord(h); }
  static SimHitHandleBase * createHitHandle( const TrackRecord& h ) { return new SimHitHandle_TrackRecord(&h); }
  static SimHitHandleBase * createHitHandle( const TRTUncompressedHit& h ) { return new SimHitHandle_TRTHit(&h); }
  static SimHitHandleBase * createHitHandle( const SiHit& h ) { return new SimHitHandle_SiHit(&h); }

  static QString nameHepMCAugmentedEnd;
  static QString nameAugmentedOnly;

  void createSecondaryHitLists(const SimBarCode& origSimBarCode,
			       const SimHitList& origHitList,
			       std::map<SimBarCode,SimHitList> & outlists,
			       int& newBarCode );
  std::list<SimHitHandleBase*>::iterator closestCompatibleHandleItr(SimHitHandleBase* handle,
								    const std::list<SimHitHandleBase*>::iterator& itFirst,
								    std::list<SimHitHandleBase*>& handleList,
								    const double& massSquared);

  std::map<SimBarCode::ExtBarCode,int> extBarCode2pdg;

  int updateGUICounter;
  void possiblyUpdateGUI() {
    if (!((updateGUICounter++)%750)) {
      theclass->systemBase()->updateGUI();
    }
  }

  bool cut_fromIROnly;
  bool cut_excludeBarcodeZero;
  bool cut_excludeNeutrals;

  bool displayAscObjs;
  void updateVisibleAssociatedObjects();

  bool fixMomentumInfoInSimHits(const HepMC::GenParticle* p,SimHitList& hitlist);

  static const int maxPdgCode = 1000000000;

};

QString TrackCollHandle_TruthTracks::Imp::nameHepMCAugmentedEnd = " [augmented]";
QString TrackCollHandle_TruthTracks::Imp::nameAugmentedOnly = "Sim hits/trk rec.";

//____________________________________________________________________
QStringList TrackCollHandle_TruthTracks::availableCollections( IVP1System*sys )
{
  QStringList l;
  VP1SGContentsHelper sgcont(sys);

  QStringList mcevent_keys = sgcont.getKeys<McEventCollection>();
  QStringList trackrecord_keys = sgcont.getKeys<TrackRecordCollection>();

  QStringList keys_siliconhits, keys_trthits, keys_mdthits,
    keys_rpchits, keys_tgchits, keys_cschits;

  if (VP1JobConfigInfo::hasPixelGeometry()&&VP1JobConfigInfo::hasSCTGeometry())
    keys_siliconhits = sgcont.getKeys<SiHitCollection>();//"PixelHits" and "SCT_Hits"
  if (VP1JobConfigInfo::hasTRTGeometry())
    keys_trthits = sgcont.getKeys<TRTUncompressedHitCollection>();//"TRTUncompressedHits"
  if (false&&/*fixme!!*/VP1JobConfigInfo::hasMuonGeometry()) {
    keys_mdthits = sgcont.getKeys<MDTSimHitCollection>();
    keys_rpchits = sgcont.getKeys<RPCSimHitCollection>();
    keys_tgchits = sgcont.getKeys<TGCSimHitCollection>();
    keys_cschits = sgcont.getKeys<CSCSimHitCollection>();
  }

  bool extrainfo = ! ( keys_siliconhits.empty() && keys_trthits.empty()
		       && keys_mdthits.empty() && keys_rpchits.empty()
		       && keys_tgchits.empty() && keys_cschits.empty()
		       && trackrecord_keys.empty() );

  if (extrainfo&&mcevent_keys.empty()) {
    l << Imp::nameAugmentedOnly;
    return l;
  }

  foreach (QString mcevent_key,mcevent_keys) {
    l << mcevent_key;
    if (extrainfo)
      l << mcevent_key + Imp::nameHepMCAugmentedEnd;
  }

  //Fixme: REMOVE THIS (only for testing!!):
  if (extrainfo)
    l << Imp::nameAugmentedOnly;

  return l;
}

//____________________________________________________________________
TrackCollHandle_TruthTracks::TrackCollHandle_TruthTracks(TrackSysCommonData * cd,
							 const QString& name)
  : TrackCollHandleBase(cd,name,TrackType::TruthTrack), m_d(new Imp)
{
  setHelperClassName("TrackCollHandle_TruthTracks");
  m_d->theclass = this;
  m_d->updateGUICounter = 0;
  m_d->cut_fromIROnly = true;
  m_d->cut_excludeBarcodeZero = true;
  m_d->cut_excludeNeutrals = true;
  m_d->displayAscObjs = false;
}

//____________________________________________________________________
TrackCollHandle_TruthTracks::~TrackCollHandle_TruthTracks()
{
  delete m_d;
}

//____________________________________________________________________
void TrackCollHandle_TruthTracks::setupSettingsFromControllerSpecific(TrackSystemController* controller)
{
  connect(controller,SIGNAL(cutTruthFromIROnlyChanged(bool)),this,SLOT(setCutFromIROnly(bool)));
  setCutFromIROnly(controller->cutTruthFromIROnly());

  connect(controller,SIGNAL(cutExcludeBarcodeZeroChanged(bool)),this,SLOT(setCutExcludeBarcodeZero(bool)));
  setCutExcludeBarcodeZero(controller->cutExcludeBarcodeZero());

  connect(controller,SIGNAL(cutTruthExcludeNeutralsChanged(bool)),this,SLOT(setCutExcludeNeutrals(bool)));
  setCutExcludeNeutrals(controller->cutTruthExcludeNeutrals());

  connect(controller,SIGNAL(showTruthAscObjsChanged(bool)),this,SLOT(setShowAscObjs(bool)));
  setShowAscObjs(controller->showTruthAscObjs());
}

//____________________________________________________________________
void TrackCollHandle_TruthTracks::setShowAscObjs(bool b)
{
  if (m_d->displayAscObjs==b)
    return;
  m_d->displayAscObjs=b;
  messageVerbose("Associated objects shown flag changed to " + str(b));
  m_d->updateVisibleAssociatedObjects();
}

//____________________________________________________________________
template <class collT>
void TrackCollHandle_TruthTracks::Imp::addHitCollections(std::map<SimBarCode,SimHitList> & hitLists)
{
  std::map<SimBarCode,SimHitList>::iterator itHitList;

  VP1SGAccessHelper sgaccess(theclass->systemBase());

  foreach (QString key, VP1SGContentsHelper(theclass->systemBase()).getKeys<collT>()) {
    const collT * hitColl;
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
  	if (trackID.isNonUniqueSecondary()) {
  	  double absmom = handle->momentum();
  	  if (absmom>=0&&absmom<1.0*CLHEP::MeV) {//Fixme: Useful? Put it higher??
	    //    	    if (theclass->verbose())
	    //    	      theclass->messageVerbose("Ignoring low momentum sim hit for non. unique secondary particle");
  	    delete handle;
  	    continue;
  	  }
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
bool TrackCollHandle_TruthTracks::Imp::loadHitLists(std::map<SimBarCode,SimHitList> & hitLists)
{
  //Fixme: Return false if we do not find at least one collection
  addHitCollections<TrackRecordCollection>(hitLists);

  //Important that collections which inherently contains pdg codes (TRT) are loaded first!
  if (VP1JobConfigInfo::hasTRTGeometry())
     addHitCollections<TRTUncompressedHitCollection>(hitLists);

   if (VP1JobConfigInfo::hasPixelGeometry() && VP1JobConfigInfo::hasSCTGeometry())
     addHitCollections<SiHitCollection>(hitLists);

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

  //Fixme: Add hits from muon subsystems.

  //Sort hitLists:
  for (it = hitLists.begin(); it!=itE; ++it) {
    sort(it->second.begin(),it->second.end());
    possiblyUpdateGUI();
  }
  return true;
}


//____________________________________________________________________
void TrackCollHandle_TruthTracks::fixPDGCode(SimHitHandleBase* handle) const
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
void TrackCollHandle_TruthTracks::Imp::loadGenParticles( std::map<SimBarCode,const HepMC::GenParticle*> & genParticles,
							 const HepMC::GenVertex* vtx )
{
  if (!vtx)
    return;
  HepMC::GenVertex::particles_out_const_iterator itPart(vtx->particles_out_const_begin());
  HepMC::GenVertex::particles_out_const_iterator itPartEnd(vtx->particles_out_const_end());

//   HepMC::GenVertex::particle_iterator itPart = vtx->particles_begin(HepMC::children);
//   HepMC::GenVertex::particle_iterator itPartEnd = vtx->particles_end(HepMC::children);
  for (;itPart!=itPartEnd;++itPart) {
    const HepMC::GenParticle* p = *itPart;
    if (!p)//fixme: message.
      continue;
    const HepMC::GenEvent* evt = p->parent_event();
    if (!evt)
      continue;//fixme: message.
    //Fixme: If verbose: check barcode does not already exists!
    SimBarCode simBarCode(p->barcode(),0/*evt->event_number()...fixme: correct??*/,p->pdg_id());
    genParticles[simBarCode] = p;
    if (!simBarCode.isNonUniqueSecondary())
      extBarCode2pdg[simBarCode.extBarCode()] = p->pdg_id();

    loadGenParticles(genParticles,p->end_vertex());
  }
}

//____________________________________________________________________
bool TrackCollHandle_TruthTracks::Imp::loadGenParticles( std::map<SimBarCode,const HepMC::GenParticle*> & genParticles,
							 const QString& hepMcCollKey )
{
  possiblyUpdateGUI();
  const McEventCollection* mcColl;
  if (!VP1SGAccessHelper(theclass->systemBase()).retrieve(mcColl,hepMcCollKey)) {
	theclass->message("Error: Could not retrieve "+QString(typeid(McEventCollection).name())+" collection with key = "+hepMcCollKey);
    return false;//fixme message
  }

  McEventCollection::const_iterator itEvt(mcColl->begin()), itEvtEnd(mcColl->end());
  if (itEvt==itEvtEnd)
    return false;//fixme message

  for (;itEvt!=itEvtEnd;++itEvt) {
    //NB: Signal is always the first event in collection!
    HepMC::GenEvent * evt = *itEvt;
    if (!evt)
      continue;
    HepMC::GenEvent::vertex_const_iterator itVtx(evt->vertices_begin()), itVtxEnd(evt->vertices_end());
    for(;itVtx!=itVtxEnd;++itVtx) {
      loadGenParticles(genParticles,*itVtx);
    }
  }

  return true;
}

//____________________________________________________________________
bool TrackCollHandle_TruthTracks::load()
{
  //Decode name to figure out if we need to load simhits/track records and which HepMC key to use.
  bool augmentedonly = name()==Imp::nameAugmentedOnly;
  bool augmented = augmentedonly || name().endsWith(Imp::nameHepMCAugmentedEnd);
  QString hepmckey;
  if (!augmentedonly) {
    hepmckey = name();
    if (augmented)
      hepmckey.chop(Imp::nameHepMCAugmentedEnd.count());
  }

  //get genparticles (should be done BEFORE we load sim. hits., so the barCode2pdg map gets filled):
  std::map<SimBarCode,const HepMC::GenParticle*> genParticles;
  if (!hepmckey.isEmpty())
    if (!m_d->loadGenParticles(genParticles,hepmckey))
      return false;

  //get sim hits and track records:
  std::map<SimBarCode,SimHitList> hitLists;
  if (augmented) {
    if (!m_d->loadHitLists(hitLists))
      return false;
    messageVerbose("TrackCollHandle_TruthTracks "+name()
		   +": Found "+str(hitLists.size())+" truth particles from simhits");
  }

  //Finally we need to combine the info we thus found, and construct
  //actual track handles:

  //We do this by looping through the simhit list, and checking for
  //genparticles with the same SimBarCode. Those gen particles we use
  //in this way, we take out of the map (put to null), to indicate we
  //already used them. In the final step we add genparticle-only
  //handles for the remaining genparticle (unless they have production
  //and decay vertices ultra-close to each other):

  std::map<SimBarCode,const HepMC::GenParticle*>::iterator itGenPart, itGenPartEnd(genParticles.end());
  std::map<SimBarCode,SimHitList>::iterator itHitList, itHitListEnd(hitLists.end()), itHitListTemp;

  //First we attempt to sort secondaries with barcode=0 into new lists
  //of hits that are likely to have come from the same particle.

  int newBarCode(-1);
  std::map<SimBarCode,SimHitList> secondaryHitLists;
  messageVerbose("Sorting non-unique secondaries into lists of hits likely to originate from the same track.");
  QTime timer;timer.start();
  for (itHitList = hitLists.begin();itHitList!=itHitListEnd;) {
    if (itHitList->first.isNonUniqueSecondary()) {
       m_d->createSecondaryHitLists(itHitList->first,itHitList->second,secondaryHitLists,newBarCode);
       itHitListTemp = itHitList;
       ++itHitList;
       hitLists.erase(itHitListTemp);
    } else {
       ++itHitList;
    }
  }
  messageVerbose("Finished sorting non-unique secondaries into lists. Time spent: "+str(timer.elapsed()*0.001)+" seconds");
  std::map<SimBarCode,SimHitList>::iterator itSecondaryList,itSecondaryListEnd(secondaryHitLists.end());
  for (itSecondaryList = secondaryHitLists.begin();itSecondaryList!=itSecondaryListEnd;++itSecondaryList)
    hitLists[itSecondaryList->first] = itSecondaryList->second;//Fixme: Check that it does not already exist!

  for (itHitList = hitLists.begin();itHitList!=itHitListEnd;++itHitList) {
    if (itHitList->second.empty()) {
      message("load WARNING: Ignoring empty hit list.");
      continue;
    }
    itGenPart = genParticles.find(itHitList->first);
    const HepMC::GenParticle * p(0);
    if (itGenPart!=itGenPartEnd) {
      p = itGenPart->second;
      itGenPart->second = 0;
    }

//     if (!itHitList->first.isNonUniqueSecondary()) continue;//FIXME
//    if (!itHitList->first.actualBarCode()==0) continue;//FIXME
    m_d->possiblyUpdateGUI();

    if (m_d->fixMomentumInfoInSimHits(p,itHitList->second))//Provide guesses for momentum in simhits that needs them (and deletes the rest).
      addTrackHandle( new TrackHandle_TruthTrack( this, itHitList->first, itHitList->second, p ) );
  }

  const double minSpacialSeparation = 1.0e-3*CLHEP::mm;
  const double minSepSq = minSpacialSeparation*minSpacialSeparation;
  for (itGenPart=genParticles.begin();itGenPart!=itGenPartEnd;++itGenPart) {
    const HepMC::GenParticle * p = itGenPart->second;
    if (!p)
      continue;
     if (abs(p->pdg_id())>=Imp::maxPdgCode)//Internal particle... (fixme: find proper limit!!)
       continue;
    if (!p->production_vertex())
      continue;
    if (p->end_vertex()) {
      double dx(p->end_vertex()->position().x()-p->production_vertex()->position().x());
      double dy(p->end_vertex()->position().y()-p->production_vertex()->position().y());
      double dz(p->end_vertex()->position().z()-p->production_vertex()->position().z());
      if ( dx*dx+dy*dy+dz*dz < minSepSq )
	continue;
    }
    m_d->possiblyUpdateGUI();
    addTrackHandle( new TrackHandle_TruthTrack( this, itGenPart->first, SimHitList(), p ) );
  }

  //Maybe we need to show measurements, etc.:
  m_d->updateVisibleAssociatedObjects();

  return true;
}

//____________________________________________________________________
bool TrackCollHandle_TruthTracks::cut(TrackHandleBase* handle)
{
  if (!TrackCollHandleBase::cut(handle))
    return false;

  if (m_d->cut_excludeNeutrals && handle->hasCharge() && handle->charge()==0.0)
    return false;

  TrackHandle_TruthTrack * truthhandle = static_cast<TrackHandle_TruthTrack *>(handle);
  if (m_d->cut_excludeBarcodeZero && truthhandle->hasBarCodeZero())
    return false;

  if (m_d->cut_fromIROnly && ! truthhandle->hasVertexAtIR(2.8*CLHEP::cm*2.8*CLHEP::cm,50*CLHEP::cm))
    return false;

  return true;
}

//____________________________________________________________________
void TrackCollHandle_TruthTracks::setCutFromIROnly(bool b)
{
  if (m_d->cut_fromIROnly == b)
    return;
  m_d->cut_fromIROnly = b;
  if (b)
    recheckCutStatusOfAllVisibleHandles();
  else
    recheckCutStatusOfAllNotVisibleHandles();
}

//____________________________________________________________________
void TrackCollHandle_TruthTracks::setCutExcludeBarcodeZero(bool b)
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
void TrackCollHandle_TruthTracks::setCutExcludeNeutrals(bool b)
{
  if (m_d->cut_excludeNeutrals==b)
    return;
  m_d->cut_excludeNeutrals=b;
  if (b)
    recheckCutStatusOfAllVisibleHandles();
  else
    recheckCutStatusOfAllNotVisibleHandles();
}

//____________________________________________________________________
void TrackCollHandle_TruthTracks::Imp::createSecondaryHitLists(const SimBarCode& origSimBarCode,
							       const SimHitList& origHitList,
							       std::map<SimBarCode,SimHitList> & outlists,
							       int& newBarCode )
{
  if (!origSimBarCode.isNonUniqueSecondary()||newBarCode>=0) {
    theclass->message("createSecondaryHitLists"
		      " ERROR: Unexpected input");
    return;
  }

  unsigned ntothitinput = origHitList.size();
  HepMcParticleLink::index_type evtIndex = origSimBarCode.evtIndex();
  int pdgCode = origSimBarCode.pdgCode();

  ////////////////////////////////////////////////////////////////////////////
  //  Temporarily put the (time,handle) pairs from the vector into a list.  //
  ////////////////////////////////////////////////////////////////////////////

  SimHitList::const_iterator itOrig(origHitList.begin()),itOrigE(origHitList.end());
  std::list<SimHitHandleBase*> handleList;
  for(;itOrig!=itOrigE;++itOrig)
    handleList.push_back(itOrig->second);

  ////////////////////////////////////////////////////////////////
  //  Produce handle lists based on requirements of proximity,  //
  //  decreasing momenta and causality.                         //
  ////////////////////////////////////////////////////////////////
  std::set<std::list<SimHitHandleBase*> > outHandleLists;

  bool ok;
  double mass = VP1ParticleData::particleMass(pdgCode,ok);
  double massSquared( (ok&&mass>=0) ? mass*mass : -1);
  while (!handleList.empty()) {
    std::list<SimHitHandleBase*> list;
    std::list<SimHitHandleBase*>::iterator it(handleList.begin()), itNext, itTemp;
    //Take this handle from the list, and increment to next one:
    SimHitHandleBase * handle = *it;
    list.push_back(handle);
    itTemp = it++;
    handleList.erase(itTemp);
    while ( true ) {
      if (it==handleList.end())
	break;//TEST
      itNext = closestCompatibleHandleItr(handle,it,handleList,massSquared);
      possiblyUpdateGUI();
      if (itNext == handleList.end())//Fixme: cache handleList.end()? Check erase invalidation!
	break;
      handle = *itNext;
      list.push_back(handle);
      it = itNext; ++it;
      handleList.erase(itNext);
      if (it == handleList.end())//Fixme: cache handleList.end()? Check erase invalidation!
	break;
    }
    if (list.size()==1) {//5 is ok for trt, but probably not for silicon!
      //We need at least two sim hits//FIXME: 5?
      //Fixme: Make minimum number of hits depend on energy?
//       theclass->messageVerbose("Ignoring secondary with barCode 0, since it seems to be the only sim. hit. by this track.");//Fixme: We could do something with posStart/posEnd?//fixme: update text to reflect 5
     //FIXME: use this!:      delete *(list.begin());
    } else {
      outHandleLists.insert(list);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////
  //  Put handle lists in output handle lists, and assign unique new fake barcodes //
  ///////////////////////////////////////////////////////////////////////////////////

  //We assign fake negative barcodes to
  //secondaries, since negative barcodes are
  //by convention reserved for vertices in
  //the HepMC collection.

  std::set<std::list<SimHitHandleBase*> >::iterator itOutList(outHandleLists.begin()), itOutListE(outHandleLists.end());
  unsigned totused(0);
  for (;itOutList!=itOutListE;++itOutList) {
    const SimBarCode fakeBarCode(newBarCode--,evtIndex,pdgCode);
    //Fixme: Update barcodes contained in simhithandles!!
    const unsigned n = itOutList->size();
    totused += n;
    outlists[fakeBarCode] = SimHitList();
    std::map<SimBarCode,SimHitList>::iterator itActualOutList = outlists.find(fakeBarCode);
    itActualOutList->second.reserve(n);

    std::list<SimHitHandleBase*>::const_iterator itHandle(itOutList->begin()),itHandleE(itOutList->end());
    for (;itHandle!=itHandleE;++itHandle)
      itActualOutList->second.push_back(std::pair<double,SimHitHandleBase*>((*itHandle)->hitTime(),*itHandle));

    //Should be ok already, but just to be safe: (fixme: don't do this?)
    sort(itActualOutList->second.begin(),itActualOutList->second.end());
  }

  if (theclass->verbose())
    theclass->messageVerbose("Grouped "+str(ntothitinput)+" secondaries with pdgCode = "
			     +str(pdgCode)+" into "+str(outHandleLists.size())
			     +" tracks ("+str(ntothitinput-totused)+" went unused).");
}


//____________________________________________________________________
std::list<SimHitHandleBase*>::iterator TrackCollHandle_TruthTracks::Imp::closestCompatibleHandleItr(SimHitHandleBase* handle,
												    const std::list<SimHitHandleBase*>::iterator& itFirst,
												    std::list<SimHitHandleBase*>& handleList,
												    const double& massSquared) {
//   if (itFirst==handleList.end())
//     return handleList.end();
  const double mom = handle->momentum();
  const double momSq = mom*mom;
  const double betaSqMax = ( (mom < 0 || massSquared<=0 ? 1 : (momSq/(momSq+massSquared)) ));
  const double speedSqMax = 4.0* CLHEP::c_squared * betaSqMax;//NB: The factor of 4 is a fudge factor, not really clear why needed!!! VP1 or GEANT4 bug!
  //  const double speedSqMax = 1.0001 * c_squared;/*fixme test*/
//    const double speedSqMax = 1.1*betaSqMax*300*300;/*fixme test*/
//   double testmsq=0.511*MeV*0.511*MeV;
//   const double speedSqMax = 1.0001 * c_squared * ( (mom < 0 || testmsq<=0 ? 1 : (momSq/(momSq+testmsq)) ));

  unsigned ichecked(0);
  unsigned maxchecked(50);

  const double hitTime = handle->hitTime();
  const Amg::Vector3D pos = handle->posStart();
  double mom2, flightTime;
  std::list<SimHitHandleBase*>::iterator it(itFirst), itE(handleList.end());
  std::list<SimHitHandleBase*>::iterator itMinDist(itE);
  double minDistSq(100*CLHEP::cm*100*CLHEP::cm);//Put to 1.0e99 for no hard limit.
  if (mom>0)
    if (mom<500*CLHEP::MeV) {
      minDistSq = 20*CLHEP::cm*20*CLHEP::cm;
      if (mom<100*CLHEP::MeV) {
	minDistSq = 10*CLHEP::cm*10*CLHEP::cm;
	if (mom<10*CLHEP::MeV)
	  minDistSq = 5*CLHEP::cm*5*CLHEP::cm;
      }
    }

  for (;it!=itE;++it) {
    ++ichecked;
    mom2 = (*it)->momentum();
    //Possible decreasing momentum requirement:
    if (mom>=0&&mom2>=0) {
      if (mom2>mom)
	continue;
      if (mom2<0.5*mom)//Guard against delta-rays-ish.
	continue;
    }

    //Maximal separation requirement:
    const double distSquared = ((*it)->posStart()-pos).mag2();

    //Only investigate if it is the closest one:
    if (distSquared>=minDistSq)
      continue;

    flightTime = (*it)->hitTime() - hitTime;
    if (flightTime<=0||flightTime>100*CLHEP::ns) {
      //Second hit comes before (shouldn't happen), or waaaay after the first hit.
      if (flightTime<0)
	theclass->message("closestCompatibleHandleItr WARNING: Should never happen. T1="+str(hitTime)+", T2="+str((*it)->hitTime()));
      continue;
    } else {
      if (distSquared>flightTime*flightTime*speedSqMax)
	continue;
    }


    //Check for coalignment of momenta:
    double mindotproduct = -0.5;
    if (mom>10.0*CLHEP::MeV) {
      mindotproduct = -0.1;
      if (mom>1000.0*CLHEP::MeV) {
	mindotproduct = 0.5;
	if (mom>10000.0*CLHEP::MeV) {
	  mindotproduct = 0.80;
	}
      }
    }
    if (mindotproduct>-1.0)
      if (handle->momentumDirection().dot((*it)->momentumDirection())<mindotproduct)
	continue;

    //     theclass->messageDebug("Test. Hit passed mom and causality.");

    //     //Possible minimal separation requirement: //FIXME: Turn this on if momentum is greater than...?
    //     if (mom2>=0&&massSquared>=0) {
    //       const double speedSqMin = 0.9999 * c_squared * momSq/(momSq+massSquared);
    //       const double minDistSquared = flightTimeSq*speedSqMin;
    //       if (distSquared<minDistSquared)
    // 	continue;
    //     }
    //Fixme: We might also make some requirement that distance should be less than e.g. 20cm??

    //Hits are in principle compatible. Store it if it is also the closest:
    //     if (distSquared<minDistSq) {
    minDistSq = distSquared;
    itMinDist = it;

    if (distSquared<15*15*CLHEP::cm*CLHEP::cm) {
      //We already found a very good hit - should not look much further.
      if (distSquared<5*5*CLHEP::cm*CLHEP::cm)
	maxchecked = ichecked + 5;
      else
	maxchecked = ichecked + 15;
    }

    if (ichecked>maxchecked)//For performance reasons
      break;
  }
  return itMinDist;
}


//____________________________________________________________________
bool TrackCollHandle_TruthTracks::Imp::fixMomentumInfoInSimHits(const HepMC::GenParticle* p,SimHitList& hitlist) {
  //Returns false only if we prune down to zero information!

  if (hitlist.empty())
    return true;


  static double unknown = -1.0e99;
  double mom(unknown), time(unknown);
  if (p) {
    const HepMC::GenVertex * v = p->production_vertex();
    if (v) {
      mom = mag(p->momentum());
      time = v->position().t()/CLHEP::c_light;
      //       theclass->messageDebug("fixMomentumInfoInSimHits genparticle "+str(mom/GeV)+" GeV, time = "+str(time/ns)+" ns");
    }
  }

  //First thing we do is to quickly determine if we are in the special
  //situation of all hits+genparticle missing momentum information, or
  //all hits already including momentum information.

  bool sawhitwithoutmominfo(false);
  bool sawhitwithmominfo(mom!=unknown);
  SimHitList::iterator it(hitlist.begin()), itE(hitlist.end());
  for (;it!=itE;++it) {
    const bool hasinfo = it->second->momentum()>=0.0;
    if (hasinfo)
      sawhitwithmominfo = true;
    else
      sawhitwithoutmominfo = true;
    if (sawhitwithoutmominfo&&sawhitwithmominfo)
      break;
  }

  if (!sawhitwithoutmominfo) {
    //Already perfectly fine
    return true;
  }
  if (!sawhitwithmominfo) {
    //Worst case scenario. Discard all for now.
    theclass->messageDebug("Discarding hitlist." );
    SimHitList::iterator it(hitlist.begin()), itE(hitlist.end());
    for (;it!=itE;++it)
      delete it->second;
    hitlist.clear();
    return false;
  }

//   {
//     if (time!=unknown)
//       theclass->messageDebug("BEFORE STARTS WITH GP time = "+str(time/ns)+" ns, mom = "+str(mom/GeV)+" GeV" );
//     SimHitList::iterator it(hitlist.begin()), itE(hitlist.end());
//     for (;it!=itE;++it) {
//       theclass->messageDebug("BEFORE time = "+str(it->second->hitTime()/ns)+" ns, mom = "+str(it->second->momentum()/GeV)+" GeV" );
//     }
//   }

  //OK, we have some mom info, but not in all hits. Time to do some dirty work!

  //First (if no genparticle), we check if the hitlist begins with
  //hits without momentum information. If it does, we simply supply
  //the first hit with an *extrapolated* guess!
  if (mom==unknown) {
    if (hitlist.at(0).second->momentum()<0.0) {
      SimHitList::iterator it(hitlist.begin()), itE(hitlist.end());
      for (;it!=itE;++it) {
	if (it->second->momentum()>=0.0) {
	  hitlist.at(0).second->setFakeMomentum(it->second->momentum()*1.00001);//We make it slightly bigger. Just because.
	  break;
	}
      }
      if (hitlist.at(0).second->momentum()<0.0) {
	theclass->messageDebug("fixMomentumInfoInSimHits ERROR: Should not happen! (1)" );
	//Fixme: clear hitlist.
	return false;
      }
    }
    mom = hitlist.at(0).second->momentum();
    time = hitlist.at(0).second->hitTime();
  }

  //Then we check if the hitlist ends with hits without momentum
  //information. If it does we simply supply the very last of the hits
  //with an *extrapolated* guess!  (FIXME: ASSUME 0.01% loss every CLHEP::ns or
  //something else that is simple? Or even use the extrapolator to the
  //last of those hits?)
  unsigned ilast = hitlist.size()-1;
  if (hitlist.at(ilast).second->momentum()<0.0) {
    for (int iLastWithMom = ilast-1;iLastWithMom>=0;--iLastWithMom) {
      if (hitlist.at(iLastWithMom).second->momentum()>0.0) {
	hitlist.at(ilast).second->setFakeMomentum(hitlist.at(iLastWithMom).second->momentum()*0.99999);//Slight smaller. Just because.
	break;
      }
    }
    if (hitlist.at(ilast).second->momentum()<0.0) {
      //Get it from the genparticle:
      if (mom==unknown) {
	theclass->messageDebug("fixMomentumInfoInSimHits ERROR: Should not happen! (2)" );
	//Fixme: clear hitlist.
	return false;
      }
      hitlist.at(ilast).second->setFakeMomentum(mom*0.99999);
    }
  }

  //Every unknown momentum is now surrounded in time with momentum information. Time to interpolate!!
  if (mom==unknown||time==unknown) {
    //No genparticle. Initialise from first hit.
    mom = hitlist.at(0).second->momentum();
    time = hitlist.at(0).second->hitTime();
  }

  unsigned iNextWithMom(0);
  for (unsigned i = 0; i < hitlist.size(); ++i) {
    if (hitlist.at(i).second->momentum()>=0.0) {
      mom = hitlist.at(i).second->momentum();
      time = hitlist.at(i).second->hitTime();
      continue;
    }
    if (iNextWithMom<=i) {
      for (unsigned j = i+1;j<hitlist.size();++j) {
	if (hitlist.at(j).second->momentum()>=0.0) {
	  iNextWithMom = j;
	  break;
	}
      }
      if (iNextWithMom<=i) {
	theclass->messageDebug("fixMomentumInfoInSimHits ERROR: Should not happen! (3)" );
	//Fixme: clear hitlist.
	return false;
      }
    }
    //
    double time2 = hitlist.at(iNextWithMom).second->hitTime();
    double mom2 = hitlist.at(iNextWithMom).second->momentum();
    double t = hitlist.at(i).second->hitTime();
//     theclass->message("ABOUT TO INTERPOLATE time="+str(time/ns)+", t="+str(t/ns)+", time2="+str(time2/ns)+", mom="+str(mom/GeV)+", mom2="+str(mom2/GeV));//fixme

    if (t<=time||t>=time2||time2<=time)
      theclass->message("SUSPICIOUS TIME");//fixme
    if (mom2>=mom)
      theclass->message("SUSPICIOUS MOM mom="+str(mom)+", mom2="+str(mom2));//fixme
    mom += (mom2-mom)*(t-time)/(time2-time);
    time = t;
    hitlist.at(i).second->setFakeMomentum(mom);
  }




//   //Now we loop through the list and see what we need to fix, if anything:

//   unsigned iNextWithMom(0);
//   for (unsigned i = 1/*already dealt with first*/; i < hitlist.size(); ++i) {
//     if (hitlist.at(i).second->momentum()>=0.0) {
//       mom = hitlist.at(i).second->momentum();
//       time = hitlist.at(i).second->hitTime();
//     } else {
//       if (iNextWithMom<=i) {
// 	for (unsigned j = i+1;j<hitlist.size();++j) {
// 	  if (hitlist.at(j).second->momentum()>=0.0) {
// 	    iNextWithMom = j;
// 	    break;
// 	  }
// 	}
// 	if (iNextWithMom<=i) {
// 	  //Discard end of list!!
// 	  unsigned j = i;
// 	  for (;j<hitlist.size();++j) {
// 	    //TESTdelete hitlist.at(j).second;
// 	  }
// 	  hitlist.resize(j);
// 	  theclass->messageDebug("Discarded "+str(hitlist.size()-i)+" simhits due to missing momentum information at the start of the list");
// 	  return !hitlist.empty() || !p;
// 	}
// 	//Interpolate mom(time) values:
// 	double time2 = hitlist.at(iNextWithMom).second->hitTime();
// 	double mom2 = hitlist.at(iNextWithMom).second->momentum();
// 	double t = hitlist.at(i).second->hitTime();
// 	if (t<=time||t>=time2||time2<=time)
// 	  theclass->message("SUSPICIOUS TIME");//fixme
// 	if (mom2>=mom)
// 	  theclass->message("SUSPICIOUS MOM");//fixme
// 	mom = (t-time)/(time2-time)*(mom2-mom);
// 	time = t;
// 	hitlist.at(i).second->setFakeMomentum(mom);
//       }
//     }
//   }


// // typedef std::vector<std::pair<double,SimHitHandleBase*> > SimHitList;//hitTime() to SimHitHandle's


//   {
//     SimHitList::iterator it(hitlist.begin()), itE(hitlist.end());
//     for (;it!=itE;++it) {
//       theclass->messageDebug("AFTER time = "+str(it->second->hitTime()/ns)+" ns, mom = "+str(it->second->momentum()/GeV)+" GeV" );
//     }
//   }

  return true;

}


//If has momentum - Check light-speed consistency given positions and time.
//mom should be decreasing

//____________________________________________________________________
void TrackCollHandle_TruthTracks::Imp::updateVisibleAssociatedObjects()
{
      
  theclass->message("updateVisibleAssociatedObjects");//fixme
  theclass->largeChangesBegin();
  theclass->trackHandleIterationBegin();
  TrackHandle_TruthTrack* handle;
  while ((handle=static_cast<TrackHandle_TruthTrack*>(theclass->getNextTrackHandle()))) {
    handle->setAscObjsVisible(displayAscObjs);
  }
  theclass->largeChangesEnd();
}

