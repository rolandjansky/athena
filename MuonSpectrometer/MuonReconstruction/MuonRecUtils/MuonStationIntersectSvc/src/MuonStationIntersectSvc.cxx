/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonStationIntersectSvc/MuonStationIntersectSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"

#include "Identifier/Identifier.h"

#include "StoreGate/StoreGateSvc.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

#include "MuonStationIntersectSvc/MuonIntersectGeometry.h"
#include "MuonStationIntersectSvc/MdtIntersectGeometry.h"

#include "MuonCondData/MdtCondDbData.h"

MuonStationIntersectSvc::MuonStationIntersectSvc(const std::string& name,ISvcLocator* sl)
  : AthService(name,sl)
{
}

MuonStationIntersectSvc::~MuonStationIntersectSvc() {
}

// queryInterface
StatusCode MuonStationIntersectSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if(IID_IMuonStationIntersectSvc.versionMatch(riid) )
    {
      *ppvIF = (MuonStationIntersectSvc*)this;
    } else {
    return Service::queryInterface(riid, ppvIF);
  }
  return StatusCode::SUCCESS;
}


StatusCode MuonStationIntersectSvc::initialize()
{
  ATH_CHECK(m_idHelper.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode MuonStationIntersectSvc::finalize() {
  ATH_MSG_DEBUG("MuonStationIntersectSvc::finalize");
  return StatusCode::SUCCESS;
}

const std::vector<std::unique_ptr<Muon::MdtIntersectGeometry> > MuonStationIntersectSvc::getStationGeometry( const Identifier& id, const MdtCondDbData* dbData, 
													      const MuonGM::MuonDetectorManager* detMgr ) const {
  // get ids of geometry associated with identifier
  const std::vector<Identifier> chambers = binPlusneighbours( id );

  // vector to store result
  std::vector<std::unique_ptr<Muon::MdtIntersectGeometry> > stations;

  // loop over bins, retrieve geometry
  for( const auto chId : chambers){
    if(dbData){
      if(!dbData->isGoodStation(chId)){
	ATH_MSG_VERBOSE ("chamber "<<m_idHelper->toString(chId)<<" is dead");
	continue;
      }
    }
    stations.push_back(std::unique_ptr<Muon::MdtIntersectGeometry>(new Muon::MdtIntersectGeometry( chId, detMgr,dbData,&this->msgStream())));
  }
  return stations;
}

const Muon::MuonStationIntersect
MuonStationIntersectSvc::tubesCrossedByTrack( const Identifier& id,
					      const Amg::Vector3D& pos,
					      const Amg::Vector3D& dir,
					      const MdtCondDbData* dbData,
					      const MuonGM::MuonDetectorManager* detMgr) const
{
  const std::vector<std::unique_ptr<Muon::MdtIntersectGeometry> >& stations = getStationGeometry( id, dbData, detMgr );
  std::vector<std::unique_ptr<Muon::MdtIntersectGeometry> >::const_iterator it = stations.begin();
  std::vector<std::unique_ptr<Muon::MdtIntersectGeometry> >::const_iterator it_end = stations.end();

  ATH_MSG_DEBUG(" Calculating intersections for chamber " << m_idHelper->toString(id)
		<< " accounting for " << stations.size()<<" stations");

  Muon::MuonStationIntersect::TubeIntersects tubeIntersects;
  for( ; it!=it_end; ++it ){

    const Muon::MuonStationIntersect intersect = (*it)->intersection(pos,dir);
    tubeIntersects.insert( tubeIntersects.end(), intersect.tubeIntersects().begin(),
			   intersect.tubeIntersects().end() );
  }

  Muon::MuonStationIntersect intersection;
  intersection.setTubeIntersects( tubeIntersects );
  return intersection;
}

const std::vector<Identifier> MuonStationIntersectSvc::binPlusneighbours( const Identifier& id ) const
{

  std::vector<Identifier> chIds;
  int stName = m_idHelper->mdtIdHelper().stationName(id);
  int stPhi = m_idHelper->mdtIdHelper().stationPhi(id);
  int stEta = m_idHelper->mdtIdHelper().stationEta(id);
  bool isBarrel = m_idHelper->mdtIdHelper().isBarrel( id );
  int stEtaMin=m_idHelper->mdtIdHelper().stationEtaMin(id);
  int stEtaMax=m_idHelper->mdtIdHelper().stationEtaMax(id);

  int chEtaLeft = stEta-1;
  int chEtaRight = stEta+1;

  // chamber with smallest eta
  if( chEtaLeft < stEtaMin ) chEtaLeft = -999;

  // chamber with largest eta
  if( chEtaRight > stEtaMax ) chEtaRight = -999;

  Muon::MuonStationIndex::ChIndex chIndex = m_idHelper->chamberIndex(id);

  // special treatment of EOS chambers
  if( chIndex == Muon::MuonStationIndex::EOS ) {
    chEtaRight = -999;
    chEtaLeft  = -999;
  }

  if( isBarrel ){
    // eta = 0 doesn't exist take next
    if( chEtaLeft == 0 ) chEtaLeft -= 1;
    if( chEtaRight == 0 ) chEtaRight += 1;
  }else{
    // do not combined positive and negative endcaps
    if( chEtaLeft == 0 ) chEtaLeft  = -999;
    if( chEtaRight == 0 ) chEtaRight = -999;
  }

  // no neighbours for BIS8
  if( chIndex == Muon::MuonStationIndex::BIS && abs(stEta) == 8 ){
    chEtaLeft  = -999;
    chEtaRight = -999;
  }

  // BIS 8 never neighbour of a chamber
  if( chIndex == Muon::MuonStationIndex::BIS ){
    if( abs(chEtaLeft) == 8 )  chEtaLeft  = -999;
    if(abs(chEtaRight) == 8 )  chEtaRight  = -999;
  }

  if( chEtaLeft != -999 && m_idHelper->mdtIdHelper().validElement(m_idHelper->mdtIdHelper().elementID(stName,chEtaLeft,stPhi)) ) 
    chIds.push_back( m_idHelper->mdtIdHelper().elementID(stName,chEtaLeft,stPhi));
  chIds.push_back(m_idHelper->mdtIdHelper().elementID(id));
  if( chEtaRight != -999 && m_idHelper->mdtIdHelper().validElement(m_idHelper->mdtIdHelper().elementID(stName,chEtaRight,stPhi)) ) 
    chIds.push_back( m_idHelper->mdtIdHelper().elementID( stName,chEtaRight,stPhi ) );

  ATH_MSG_VERBOSE(" returning chambers for id " << m_idHelper->toString(id) << " ids " << chIds.size()
		  << "   eta " << chEtaLeft  << "     " << stEta << "     " << chEtaRight << " chambers: ");
  for( unsigned int i=0;i<chIds.size();++i ) ATH_MSG_VERBOSE(m_idHelper->toString(chIds[i]));

  return chIds;
}

