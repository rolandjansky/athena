/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonStationIntersectSvc/MuonStationIntersectSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "Identifier/Identifier.h"

#include "StoreGate/StoreGateSvc.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MdtDetectorElement.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

#include "MuonStationIntersectSvc/MuonIntersectGeometry.h"
#include "MuonStationIntersectSvc/MdtIntersectGeometry.h"
#include "TrkDriftCircleMath/MdtChamberGeometry.h"

#include "MuonCondInterface/IMDTConditionsSvc.h"

class MuonStationIntersectSvc::Imp {
public:
  Imp(std::string name) : m_detMgr(0),m_stNameMax(0),m_stPhiMax(0),m_stEtaMax(0),m_stEtaMin(0),m_etaRange(0), 
                          m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"), 
			  m_mdtSummarySvc("MDTCondSummarySvc", name), m_invalidStationData(0,0),
			  m_name(name),m_log(0),m_debug(false),m_verbose(false),m_initGeometry(true) {}

  typedef std::pair<const MuonGM::MdtDetectorElement*,const Muon::MuonIntersectGeometry*> StationData;

  void initGeometry() const;
  void clearGeometry();

  const StationData& getStationGeometry( int hash ) const {

    // range check
    if( hash < 0 || hash >= (int)m_geometry.size() ) return m_invalidStationData;

    StationData& data = m_geometry[hash];

    // check whether chamber exists in geometry,
    if( data.first && !data.second ){
      // chamber exits but is not initialised -> initialise it
      data.second = createMdtChamberGeometry( data.first->identify());
    }

    return data;
  }


  int toBin( const Identifier& id ) const {
    int stName = m_idHelper->mdtIdHelper().stationName(id);
    int stPhi = m_idHelper->mdtIdHelper().stationPhi(id);
    int stEta = m_idHelper->mdtIdHelper().stationEta(id);
    return toBin( stName, stPhi, stEta );
  }

  int toBin( int stName, int stPhi, int stEta ) const {
    return stName*m_stPhiMax*(m_etaRange) + (stPhi-1)*(m_etaRange) + stEta-m_stEtaMin;
  }

  const std::vector<int>& binPlusneighbours( const Identifier& id ) const;

  const Muon::MuonIntersectGeometry* createMdtChamberGeometry( const Identifier& id ) const;

  const MuonGM::MuonDetectorManager*  m_detMgr;

  mutable int m_stNameMax;
  mutable int m_stPhiMax;
  mutable int m_stEtaMax;
  mutable int m_stEtaMin;
  mutable int m_etaRange;
  mutable std::vector<int> m_stEtaMinEB;
  mutable std::vector<int> m_stEtaMaxEB;

  ToolHandle<Muon::MuonIdHelperTool> m_idHelper;
  ServiceHandle<IMDTConditionsSvc>   m_mdtSummarySvc;

  mutable std::vector<StationData>   m_geometry;
  StationData                        m_invalidStationData;
  mutable Muon::MuonStationIntersect m_intersection;

  // caches for results to avoid vector creation
  mutable std::vector<int>           m_bins;
  mutable std::vector<const Muon::MuonIntersectGeometry*> m_stations;

  std::string m_name;
  MsgStream* m_log;
  bool m_debug;
  bool m_verbose;
  bool m_initGeometry;

};


MuonStationIntersectSvc::MuonStationIntersectSvc(const std::string& name,ISvcLocator* sl)
  : AthService(name,sl)
{
  m_imp = new Imp(name);
  m_imp->m_log = 0;
  m_imp->m_debug = false;
  m_imp->m_verbose = false;
  m_imp->m_initGeometry = true;
  declareProperty( "CreateGeometryAtInitialization", m_imp->m_initGeometry = false );
  declareProperty( "IdHelper", m_imp->m_idHelper );
  declareProperty( "MDTCondSummarySvc", m_imp->m_mdtSummarySvc );

}

MuonStationIntersectSvc::~MuonStationIntersectSvc() {
  delete m_imp;
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
  m_imp->m_log = new MsgStream(msgSvc(), name());
  m_imp->m_debug = m_imp->m_log->level() <= MSG::DEBUG;
  m_imp->m_verbose = m_imp->m_log->level() <= MSG::VERBOSE;

  if ( AthService::initialize().isFailure() ) {
    *m_imp->m_log << MSG::ERROR << "Service::initialise() failed" << endmsg;
    return StatusCode::FAILURE;
  }

  StoreGateSvc* detStore=0;
  if ( serviceLocator()->service("DetectorStore", detStore).isSuccess() ) {
    if ( detStore->retrieve( m_imp->m_detMgr ).isFailure() ) {
      *m_imp->m_log << MSG::ERROR << " Cannot retrieve MuonDetectorManager " << endmsg;
      return StatusCode::FAILURE;
    }
  }else{
    return StatusCode::FAILURE;
  }

//   // Set to be listener for end of event
  IIncidentSvc* incSvc;
  if (service("IncidentSvc",incSvc).isFailure()) {
    *m_imp->m_log << MSG::ERROR << "Unable to get the IncidentSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  long int pri=100;
  incSvc->addListener( this, "EndEvent", pri);

  if( m_imp->m_idHelper.retrieve().isFailure() ){
    *m_imp->m_log << MSG::ERROR << "Failed to retrieve " << m_imp->m_idHelper << endmsg;
    return StatusCode::FAILURE;
  }

  if( m_imp->m_mdtSummarySvc.retrieve().isFailure() ){
    *m_imp->m_log << MSG::ERROR << "Failed to retrieve " << m_imp->m_mdtSummarySvc << endmsg;
    return StatusCode::FAILURE;
  }


  delete m_imp->m_log;
  m_imp->m_log = new MsgStream(msgSvc(), name());
  m_imp->m_debug = m_imp->m_log->level() <= MSG::DEBUG;
  m_imp->m_verbose = m_imp->m_log->level() <= MSG::VERBOSE;

  // create geometry
  m_imp->initGeometry();

  return StatusCode::SUCCESS;
}

StatusCode MuonStationIntersectSvc::finalize() {
  if( m_imp->m_debug ) *m_imp->m_log << MSG::DEBUG << "Finalizing " << endmsg;

  // free memory of geometry
  m_imp->clearGeometry();
  delete m_imp->m_log;

  return StatusCode::SUCCESS;;
}

const std::vector<const Muon::MuonIntersectGeometry*>& MuonStationIntersectSvc::getStationGeometry( const Identifier& id ) const {

  // get bins of geometry associated with identifier
  const std::vector<int>& chambers = m_imp->binPlusneighbours( id );

  // vector to store result
  m_imp->m_stations.clear();

  // loop over bins, retrieve geometry
  std::vector<int>::const_iterator it = chambers.begin();
  std::vector<int>::const_iterator it_end = chambers.end();
  for( ; it!=it_end; ++it ){

    // get dat for the chamber
    const Imp::StationData& data = m_imp->getStationGeometry( *it );

    // add to vector if valid chamber
    if( data.second ) m_imp->m_stations.push_back( data.second );
  }
  return m_imp->m_stations;
}

const Muon::MuonIntersectGeometry* MuonStationIntersectSvc::getChamberGeometry( const Identifier& id ) const {

  // get hash for chamber
  int chamberHash = m_imp->toBin( id );

  return m_imp->getStationGeometry( chamberHash ).second;
}

const Muon::MuonStationIntersect&
MuonStationIntersectSvc::tubesCrossedByTrack( const Identifier& id,
					      const Amg::Vector3D& pos,
					      const Amg::Vector3D& dir ) const
{
  const std::vector<const Muon::MuonIntersectGeometry*>& stations = getStationGeometry( id );
  std::vector<const Muon::MuonIntersectGeometry*>::const_iterator it = stations.begin();
  std::vector<const Muon::MuonIntersectGeometry*>::const_iterator it_end = stations.end();

  if( m_imp->m_debug ){
    *m_imp->m_log << MSG::DEBUG << " Calculating holes for chamber " << m_imp->m_idHelper->toString(id)
		  << " accounted stations " << stations.size() << endmsg;
  }

  Muon::MuonStationIntersect::TubeIntersects tubeIntersects;
  for( ; it!=it_end; ++it ){

    if( m_imp->m_verbose ){
      const Muon::MdtIntersectGeometry* geo = dynamic_cast<const Muon::MdtIntersectGeometry*>(*it);
      if( geo ){
	const TrkDriftCircleMath::MdtChamberGeometry* mdtChamberGeometry = geo->mdtChamberGeometry();
	*m_imp->m_log << MSG::VERBOSE << " In chamber " << m_imp->m_idHelper->toString(geo->chamberId()) << " address " << geo << endmsg;
	if( mdtChamberGeometry ) mdtChamberGeometry->print();
      }
    }
    const Muon::MuonStationIntersect& intersect = (*it)->intersection(pos,dir);
    tubeIntersects.insert( tubeIntersects.end(), intersect.tubeIntersects().begin(),
			   intersect.tubeIntersects().end() );
  }

  m_imp->m_intersection.setTubeIntersects( tubeIntersects );
  return m_imp->m_intersection;
}

const std::vector<int>& MuonStationIntersectSvc::Imp::binPlusneighbours( const Identifier& id ) const
{
  m_bins.clear();
  int stName = m_idHelper->mdtIdHelper().stationName(id);
  int stPhi = m_idHelper->mdtIdHelper().stationPhi(id);
  int stEta = m_idHelper->mdtIdHelper().stationEta(id);
  int isBarrel = m_idHelper->mdtIdHelper().isBarrel( id ) ? 1 : 0;

  int chEtaLeft = stEta-1;
  int chEtaRight = stEta+1;

  // chamber with smallest eta
  if( chEtaLeft < m_stEtaMinEB[isBarrel] ) chEtaLeft = -999;

  // chamber with largest eta
  if( chEtaRight > m_stEtaMaxEB[isBarrel] ) chEtaRight = -999;

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

  if( chEtaLeft != -999 ) m_bins.push_back( toBin( stName, stPhi, chEtaLeft ) );
  m_bins.push_back( toBin( stName, stPhi, stEta ) );
  if( chEtaRight != -999 ) m_bins.push_back( toBin( stName, stPhi, chEtaRight ) );

  if( m_verbose ) {
    *m_log << MSG::VERBOSE << " returning hashes for id " << m_idHelper->toString(id) << " nhashes " << m_bins.size()
	   << "   eta " << chEtaLeft  << "     " << stEta << "     " << chEtaRight << " bins ";
    for( unsigned int i=0;i<m_bins.size();++i ) *m_log << " " << m_bins[i];
    *m_log << endmsg;
  }

  return m_bins;
}

void MuonStationIntersectSvc::Imp::initGeometry() const
{

  // only do this once
  if( m_geometry.empty() ) {
    m_bins.reserve(3);
    m_stations.reserve(3);
    m_stNameMax = m_idHelper->mdtIdHelper().stationNameIndexMax();
    m_stPhiMax  = m_idHelper->mdtIdHelper().stationPhiMax();
    int stPhiMin = m_idHelper->mdtIdHelper().stationPhiMin();

    m_stEtaMinEB.push_back(m_idHelper->mdtIdHelper().stationEtaMin(false));
    m_stEtaMinEB.push_back(m_idHelper->mdtIdHelper().stationEtaMin(true));
    m_stEtaMaxEB.push_back(m_idHelper->mdtIdHelper().stationEtaMax(false));
    m_stEtaMaxEB.push_back(m_idHelper->mdtIdHelper().stationEtaMax(true));

    m_stEtaMin  = m_stEtaMinEB[0] < m_stEtaMinEB[1] ? m_stEtaMinEB[0] : m_stEtaMinEB[1];
    m_stEtaMax  = m_stEtaMaxEB[0] > m_stEtaMaxEB[1] ? m_stEtaMaxEB[0] : m_stEtaMaxEB[1];

    m_etaRange = m_stEtaMax-m_stEtaMin+1;

    int hashmax = (1+m_stNameMax)*m_stPhiMax*(m_etaRange);
    m_geometry.resize( hashmax, StationData(0,0) );
    if( m_debug ) {
      *m_log << MSG::DEBUG << " initGeometry " << std::endl
	     << " max station name index " << m_stNameMax << std::endl
	     << " station phi " << stPhiMin << "  " << m_stPhiMax << endmsg
	     << " station eta: barrel " << m_stEtaMinEB[1] << "  " << m_stEtaMaxEB[1] << std::endl
	     << " station eta: endcap " << m_stEtaMinEB[0] << "  " << m_stEtaMaxEB[0] << std::endl
	     << " station eta: range  " << m_stEtaMin << "  " << m_stEtaMax
	     << " hash max " << hashmax << endmsg;
    }

  }
  MdtIdHelper::const_id_iterator it     = m_idHelper->mdtIdHelper().module_begin();
  MdtIdHelper::const_id_iterator it_end = m_idHelper->mdtIdHelper().module_end();
  for( ; it!=it_end;++it ){


    // get pointers to geometry

    IdentifierHash chHash;
    m_idHelper->mdtIdHelper().get_module_hash( *it, chHash );
    const MuonGM::MdtReadoutElement*  mdtROEl  =  m_detMgr->getMdtReadoutElement( *it );
    const MuonGM::MdtDetectorElement* mdtDetEl =  m_detMgr->getMdtDetectorElement( chHash );

    // consistency checks
    if( (!mdtROEl && mdtDetEl) || (!mdtDetEl && mdtROEl ) ){
      *m_log << MSG::WARNING << "Incompletely geometry found for " << m_idHelper->toStringChamber(*it)
	     << " DetEl " << mdtDetEl << " ReadoutEl " << mdtROEl << endmsg;
    }

    int bin = toBin(*it);
    if( bin < 0 || bin >= (int)m_geometry.size() ){
      *m_log << MSG::WARNING << " index out of range " << bin
	     << " for " << m_idHelper->toString(*it) << endmsg;
      continue;
    }

    if( mdtDetEl ){

      // sanity check
      if( mdtDetEl->identify() != *it ){
	*m_log << MSG::ERROR << " MdtDetectorElement return by geomodel has different Id than input Id: " << endmsg
	       << " In     " << m_idHelper->toString(*it) << endmsg
	       << " DetEl  " << m_idHelper->toString(mdtDetEl->identify()) << endmsg;
      }

      // get reference to bin
      StationData& data = m_geometry[bin];
      if( data.first ) {
	*m_log << MSG::WARNING << " problem initialising MdtDetectorElement vector, bin occupied " << bin << endmsg;
      }else{

	// set pointer to MdtDetectorElement
	data.first = mdtDetEl;

	if( m_verbose ) *m_log << MSG::VERBOSE << " Adding chamber " << m_idHelper->toString(mdtDetEl->identify()) << " bin " << bin << endmsg;

	// init geometry if requested
	if( m_initGeometry ) data.second = createMdtChamberGeometry( *it );

	if( m_verbose && data.second ){
	  const Muon::MdtIntersectGeometry* geo = dynamic_cast<const Muon::MdtIntersectGeometry*>(data.second);
	  if( geo ){
	    const TrkDriftCircleMath::MdtChamberGeometry* mdtChamberGeometry = geo->mdtChamberGeometry();
	    if( mdtChamberGeometry ) mdtChamberGeometry->print();
	  }else{
	    *m_log << MSG::WARNING << " dynamic_cast to MdtIntersectGeometry failed " << endmsg;
	  }
	}
      }

      // final sanity check, the first MdtReadoutElement for the MdtDetectorElement should be the same as the one from the Identifier
      if( mdtROEl != mdtDetEl->getMdtReadoutElement(1) && mdtROEl ){
	*m_log << MSG::ERROR << " Detected inconsistency in MuonGeoModel. MdtReadoutElement pointer returned from MdtDetectorElement wrong " << endmsg
	       << " ReadoutEl     " << m_idHelper->toString(mdtROEl->identify()) << endmsg
	       << " DetEl         " << m_idHelper->toString(mdtDetEl->identify()) << endmsg;
      }
    }
  }
}

void MuonStationIntersectSvc::Imp::clearGeometry() {
  // loop over geometry and delete entries
  std::vector<StationData>::iterator it = m_geometry.begin();
  std::vector<StationData>::iterator it_end = m_geometry.end();
  for( ;it!=it_end;++it ){
    if( it->second ){ // delete if not zero
      delete it->second;
      it->second = 0;
    }
  }
}

const Muon::MuonIntersectGeometry* MuonStationIntersectSvc::Imp::createMdtChamberGeometry( const Identifier& chid ) const{
  if( m_verbose ) *m_log << MSG::VERBOSE << " Creating geometry for chamber " << m_idHelper->toStringChamber(chid) << endmsg;

  // check if chamber is active using IMDTConditionsSvc
  if( !m_mdtSummarySvc->isGoodChamber(chid) ) {
    if( m_debug ) *m_log << MSG::DEBUG << " Chamber flagged as dead by IMDTConditionsSvc " << m_idHelper->toStringChamber(chid) << endmsg;
    return 0;
  }

  return new Muon::MdtIntersectGeometry( chid, m_detMgr, &*m_mdtSummarySvc );
}

void MuonStationIntersectSvc::handle(const Incident& inc) {
  if( m_imp->m_debug ) *m_imp->m_log << MSG::DEBUG << "entering handle(), incidence type " << inc.type()
				     << " from " << inc.source() << endmsg;

  // Only clear cache for EndEvent incident
  if (inc.type() != "EndEvent") return;

  m_imp->clearGeometry();

}
