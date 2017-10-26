/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackHandle_SimulationTrack            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/TrackHandle_SimulationTrack.h"
#include "VP1TrackSystems/AscObj_TruthPoint.h"
#include "VP1Base/VP1Msg.h"

#include "TrkTrack/Track.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "TrkTrack/Track.h"
#include "AthContainers/DataVector.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "HepMC/GenParticle.h"

//____________________________________________________________________
class TrackHandle_SimulationTrack::Imp {
public:
  Imp(TrackHandle_SimulationTrack * tc,
      const SimBarCode& sbc,const SimHitList& shl)
    : theclass(tc),
      simBarCode(sbc),
      simHitList(shl),
      ascObjVis(false),
      ascObjs(0),
      trkTrack(0) {}
  TrackHandle_SimulationTrack * theclass;
  SimBarCode simBarCode;
  SimHitList simHitList;

  bool ascObjVis;
  std::vector<AscObj_TruthPoint*> * ascObjs;
  void ensureInitAscObjs();
  const Trk::Track * trkTrack;
  void ensureInitTrkTracks();

  static Trk::TrackStateOnSurface * createTSOS(const Trk::TrackParameters * pars)
  {
    return pars ? new Trk::TrackStateOnSurface(0,pars,0,0) : 0;
  }
  static void addPars(DataVector<const Trk::TrackStateOnSurface>* dv, const Trk::TrackParameters * pars)
  {
    if (!pars)
      return;
    Trk::TrackStateOnSurface * tsos = createTSOS(pars);
    if (tsos)
      dv->push_back(tsos);
  }

  void createTrack(DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces)
  {
    if (!trackStateOnSurfaces) {
      VP1Msg::messageDebug("TrackHandle_SimulationTrack WARNING: Could not create track due to null TSOS vector");
      return;
    }
    if (trackStateOnSurfaces->size()==0) {
      VP1Msg::messageDebug("TrackHandle_SimulationTrack WARNING: Could not create track due to empty TSOS vector");
      delete trackStateOnSurfaces;
      return;
    }
    if (trkTrack) {
      VP1Msg::messageDebug("TrackHandle_SimulationTrack ERROR: Already create trkTrack previously!");
      delete trackStateOnSurfaces;
      return;
    }

    Trk::TrackInfo ti(Trk::TrackInfo::Unknown,theclass->extrapolationParticleHypothesis());
    trkTrack = new Trk::Track(ti,trackStateOnSurfaces/*track assumes ownership*/,0/*fitquality*/);

//     if (VP1Msg::verbose())
//       VP1Msg::messageVerbose("TrackHandle_SimulationTrack created track with "
// 			     +QString::number(trackStateOnSurfaces->size())+" parameters");
  }

};

//____________________________________________________________________
TrackHandle_SimulationTrack::TrackHandle_SimulationTrack( TrackCollHandleBase* ch,
						const SimBarCode& simBarCode,
						const SimHitList& simHitList )
  : TrackHandleBase(ch), m_d(new Imp(this,simBarCode,simHitList))
{
  if (VP1Msg::verbose()) {
    //Check genparticle barcode is same as in simBarCode. (and event index in parent_event())
    //Check that genparticle has production vertex.
    //Check if genparticle has end vertex, that there are no sim hits.
    //all pdg codes of simhits and genparticle should be identical.
    //all simhits should have same barcode.
    //hitTime should be rising in all simhits. NB: Remember to correct hit times with bunch crossing!!
    //fixme!
  }
}

//____________________________________________________________________
TrackHandle_SimulationTrack::~TrackHandle_SimulationTrack()
{
  //Fixme: delete simhitlist here?
  setAscObjsVisible(false);
  delete m_d->ascObjs;
  delete m_d->trkTrack;
  delete m_d;
}

//____________________________________________________________________
void TrackHandle_SimulationTrack::Imp::ensureInitTrkTracks()
{
  if (trkTrack)
    return;

  if (!theclass->hasCharge()) {
    VP1Msg::message("TrackHandle_SimulationTrack ERROR: Could not determine particle charge (pdg="
		    +QString::number(theclass->pdgCode())+").");//Fixme: I guess we could show non-extrapolated version?
    return;
  }
  //const double charge = theclass->charge(); unused

  DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;


    SimHitList::const_iterator it, itE(simHitList.end());
    for ( it = simHitList.begin(); it != itE; ++it ) {
      //Fixme: momentum() < 0 (i.e. not present);
      //Fixme: Possibly add points for both posStart() and posEnd() (and use energy loss information to get different momenta?)
      addPars(trackStateOnSurfaces,it->second->createTrackParameters());
    }


  createTrack(trackStateOnSurfaces);
}

//____________________________________________________________________
QStringList TrackHandle_SimulationTrack::clicked() const
{

  QStringList l;
  l << "Truth track";
  l << TrackHandleBase::baseInfo();
  l << "Evt index = "+QString::number(m_d->simBarCode.evtIndex());
  l << "BarCode = "+QString::number(m_d->simBarCode.barCode());
  //fixme - more info
  //   l << "Truth track clicked [evt index = "+QString::number(m_d->simBarCode.second)
  //     +", barcode = "+(m_d->simBarCode.barCode<0?QString("Unknown (G4 secondary)"):QString::number(m_d->simBarCode.first))+"]";//fixme - more info
  return l;
}


//____________________________________________________________________
const Trk::Track * TrackHandle_SimulationTrack::provide_pathInfoTrkTrack() const
{
  m_d->ensureInitTrkTracks();
  return m_d->trkTrack;
}


//____________________________________________________________________
int TrackHandle_SimulationTrack::pdgCode() const
{
  return m_d->simBarCode.pdgCode();
}

//____________________________________________________________________
bool TrackHandle_SimulationTrack::hasBarCodeZero() const
{
  return m_d->simBarCode.isNonUniqueSecondary();
}

//____________________________________________________________________
Amg::Vector3D TrackHandle_SimulationTrack::momentum() const
{
  SimHitList::const_iterator it, itE(m_d->simHitList.end());
  for ( it = m_d->simHitList.begin(); it != itE; ++it ) {
    if (it->second->momentum()>=0)
      return (it->second->momentum()) * (it->second->momentumDirection());
  }
  //Unknown:
  return TrackHandleBase::momentum();
}

//____________________________________________________________________
void TrackHandle_SimulationTrack::visibleStateChanged()
{
  if (visible()&&m_d->ascObjVis&&!m_d->ascObjs)
    m_d->ensureInitAscObjs();
}

//____________________________________________________________________
void TrackHandle_SimulationTrack::setAscObjsVisible(bool b)
{
  if (m_d->ascObjVis==b)
    return;
  m_d->ascObjVis=b;
//   const bool visnow = visible()&&m_d->ascObjVis;
//   const bool visbefore = visible()&&!m_d->ascObjVis;
//   if (visnow==visbefore)
//     return;
//   VP1Msg::messageVerbose("TrackHandle_SimulationTrack::AscObjs visible state -> "+VP1Msg::str(b));

  if (!m_d->ascObjs) {
    if (!b||!visible())
      return;
    m_d->ensureInitAscObjs();
  }

  std::vector<AscObj_TruthPoint*>::iterator it(m_d->ascObjs->begin()), itE(m_d->ascObjs->end());
  for (;it!=itE;++it)
    (*it)->setVisible(b);
}

//____________________________________________________________________
void TrackHandle_SimulationTrack::Imp::ensureInitAscObjs()
{
  if (ascObjs)
    return;
  ascObjs = new std::vector<AscObj_TruthPoint*>;
  ascObjs->reserve(simHitList.size());
{
  SimHitList::const_iterator it, itE(simHitList.end());
  for ( it = simHitList.begin(); it != itE; ++it )
    ascObjs->push_back(new AscObj_TruthPoint(theclass,it->second));
 }
  { 
  std::vector<AscObj_TruthPoint*>::iterator it, itE(ascObjs->end());
  for (it=ascObjs->begin();it!=itE;++it)
    theclass->registerAssocObject(*it);
  for (it=ascObjs->begin();it!=itE;++it)
    (*it)->setVisible(ascObjVis);
    }
}

//____________________________________________________________________
double TrackHandle_SimulationTrack::calculateCharge() const
{
  if (!m_d->simHitList.empty()) {
    if (m_d->simHitList.at(0).second->hasCharge())
      return m_d->simHitList.at(0).second->charge();
    else
      VP1Msg::messageVerbose("TrackHandle_SimulationTrack::calculateCharge() WARNING: Simhit did not have charge!");
  }

  return TrackHandleBase::calculateCharge();
}
