#include "MuonPatternSegmentMaker/MuonPatternSegmentMaker.h"

#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <utility>

#include "MuonPattern/MuonPatternChamberIntersect.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonSegment/MuonSegment.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

#include "TrkSegment/SegmentCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"

namespace Muon {

  MuonPatternSegmentMaker::MuonPatternSegmentMaker(const std::string& t,const std::string& n,const IInterface* p)  :  
    AthAlgTool(t,n,p),
    m_segmentMaker("Muon::DCMathSegmentMaker/DCMathSegmentMaker"),
    m_mdtCreator("Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"),
    m_clusterCreator("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_keyRpc("RPC_Measurements"),
    m_keyTgc("TGC_Measurements")
  {
    declareInterface<IMuonPatternSegmentMaker>(this);

    declareProperty("SegmentMaker",m_segmentMaker);
    declareProperty("MdtCreator",m_mdtCreator);
    declareProperty("ClusterCreator",m_clusterCreator);
    declareProperty("DoNtuple",m_doNtuple = false);
    declareProperty("DoMultiChamberAnalysis",m_doMultiAnalysis = true);
    declareProperty("DropDistance",m_dropDistance = 1500.);
    declareProperty("AngleCutPhi",m_phiAngleCut = 1e9 );
    declareProperty("FullFinder",m_doFullFinder = true);
    declareProperty("RpcPrepDataContainer", m_keyRpc);
    declareProperty("TgcPrepDataContainer", m_keyTgc);
    declareProperty("DoSummary",m_doSummary = false);
    declareProperty("RecoverTriggerHits",m_recoverTriggerHits = true);
    declareProperty("RemoveDoubleMdtHits",m_removeDoubleMdtHits = true);
  }

  MuonPatternSegmentMaker::~MuonPatternSegmentMaker()
  {
  }

  StatusCode MuonPatternSegmentMaker::initialize()
  {

    if( m_mdtCreator.retrieve().isFailure() ){
      ATH_MSG_FATAL("Could not get " << m_mdtCreator); 
      return StatusCode::FAILURE;
    }

    if( m_printer.retrieve().isFailure() ){
      ATH_MSG_FATAL("Could not get " << m_printer); 
      return StatusCode::FAILURE;
    }

    if( m_idHelper.retrieve().isFailure() ){
      ATH_MSG_FATAL("Could not get " << m_idHelper); 
      return StatusCode::FAILURE;
    }

    if( m_segmentMaker.retrieve().isFailure() ){
      ATH_MSG_FATAL("Could not get " << m_segmentMaker); 
      return StatusCode::FAILURE;
    }
  
    if( m_clusterCreator.retrieve().isFailure() ){
      ATH_MSG_FATAL("Could not get " << m_clusterCreator); 
      return StatusCode::FAILURE;
    }

    if(!m_keyRpc.initialize()){
      ATH_MSG_ERROR("Couldn't initalize RPC ReadHandleKey");
      return StatusCode::FAILURE;
    }
    if(!m_keyTgc.initialize()){
      ATH_MSG_ERROR("Couldn't initalize TGC ReadHandleKey");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS; 
  }


  MuonSegmentCombinationCollection* MuonPatternSegmentMaker::find( const MuonPatternCombinationCollection& patterns, MuonSegmentCombPatternCombAssociationMap* segPattMap ) const {

    if( m_recoverTriggerHits ) retrieveTriggerHitContainers();
  
    MuonSegmentCombinationCollection* combiCol = new MuonSegmentCombinationCollection();

    MuonPatternCombinationCollection::const_iterator it = patterns.begin();
    for( ; it!=patterns.end();++it ){

      const MuonPatternCombination* pattern = *it;

      if( !pattern ) {
	continue;
      }

      MuonSegmentCombination* combi = find( *pattern );

      // insert new combination into collection
      combiCol->push_back( combi );

      // create link between pattern and segment combination
      segPattMap->insert(std::make_pair( combi, pattern) );

    } // end loop over patterns

    return combiCol;
  }
  
  
  MuonSegmentCombination* MuonPatternSegmentMaker::find( const MuonPatternCombination& pattern ) const {

    if( pattern.chamberData().empty() )  return 0; // 0 at the moment! we need convention howto fill this, JS
    ATH_MSG_DEBUG(" New global Pattern:  pos " << pattern.chamberData().front().intersectPosition() 
		  << " dir " << pattern.chamberData().front().intersectDirection());

    bool hasPhiMeasurements = checkForPhiMeasurements( pattern );

    // sort hits per region 
    RegionMap regionMap;
    createRegionMap( pattern, regionMap, hasPhiMeasurements );
    
    // printout region Map
    if( msgLvl(MSG::DEBUG) || m_doSummary ) printRegionMap( regionMap );
    
    // calibrate hits 
    ROTsPerRegion hitsPerRegion;
    calibrateRegionMap( regionMap, hitsPerRegion );
    
    //if( m_doNtuple ) xxx_EventNtWriter->fill( hitsPerRegion, &pattern, hasPhiMeasurements );
    
    // create MuonSegmentCombintation for the given pattern
    MuonSegmentCombination* combi = new MuonSegmentCombination();
    
    
    ROTsPerRegionIt rit = hitsPerRegion.begin();
    ROTsPerRegionIt rit_end = hitsPerRegion.end();
    
    for( ;rit!=rit_end;++rit){
      //int regionId = rit->regionId;

      MdtVecIt mdtit     = rit->mdts.begin();
      MdtVecIt mdtit_end = rit->mdts.end();
      for( ; mdtit!=mdtit_end; ++mdtit ){
	if( mdtit->empty() ) continue;

	if( mdtit->size() < 3 ) continue;

	std::vector<const MuonSegment*>* segments = 0;
	
	if( m_doFullFinder ){
	  segments = m_segmentMaker->find( rit->regionPos, rit->regionDir, *mdtit, rit->clusters, 
					   hasPhiMeasurements, rit->regionDir.mag() );
	}else{
	  std::vector<const Trk::RIO_OnTrack*> rios;
	  rios.insert( rios.begin(), mdtit->begin(), mdtit->end() );
	  segments = m_segmentMaker->find(rios);
	}
	
	ATH_MSG_VERBOSE(" search in " << m_idHelper->toStringChamber(mdtit->front()->identify()) << " nhits " << mdtit->size());

	if( segments ) {
	  ATH_MSG_VERBOSE(" found segments " << segments->size());

	  combi->addSegments( segments );

	}else{
	  ATH_MSG_VERBOSE(" no segments found ");
	}

      }// end loop over MDTs

    } // end loop over regions
      
      // delete pointer to ROT in region mapx
    clearRotsPerRegion( hitsPerRegion );

    return combi;
  }


  StatusCode MuonPatternSegmentMaker::finalize()
  {
    return StatusCode::SUCCESS;
  }

  int MuonPatternSegmentMaker::getRegionId( const Identifier& id ) const
  {
  
    // simple division of MuonSpectrometer in regions using barrel/endcap seperation plus
    // inner/middle/outer seperation

    int stIndex = (int)m_idHelper->stationIndex(id);
    //int sector = m_idHelper->sector(id);
    int eta = m_idHelper->stationEta( id );
    
    int regionId = stIndex;
    if( eta < 0 ) regionId *= -1;

    return regionId;
  }


  bool MuonPatternSegmentMaker::checkForPhiMeasurements( const MuonPatternCombination& pat ) const {

    std::vector< MuonPatternChamberIntersect >::const_iterator it = pat.chamberData().begin();
    for (; it!=pat.chamberData().end(); ++it) {
      std::vector< const Trk::PrepRawData* >::const_iterator pit = (*it).prepRawDataVec().begin();
      for (; pit!=(*it).prepRawDataVec().end(); ++pit)	  {
	if( m_idHelper->measuresPhi((*pit)->identify()) ) {
	  return true;
	}
      }
    }
    return false;
  }

  void MuonPatternSegmentMaker::createRegionMap( const MuonPatternCombination& pat, RegionMap& regionMap, 
						 bool hasPhiMeasurements ) const {
    
    if( hasPhiMeasurements )
      ATH_MSG_DEBUG(" pattern has phi measurements using extrapolation to determine second coordinate ");
    else 
      ATH_MSG_DEBUG(" No phi measurements using center tubes ");
  

  
    std::vector< MuonPatternChamberIntersect >::const_iterator it = pat.chamberData().begin();
    for (; it!=pat.chamberData().end(); ++it) {

      if( it->prepRawDataVec().empty() ) continue;

      const Amg::Vector3D& patpose = (*it).intersectPosition();
      const Amg::Vector3D patdire = (*it).intersectDirection().unit();

      /** Try to recover missing phi clusters:
	  - loop over the clusters in the region and sort them by collection
	  - count the number of eta and phi clusters per collection
	  
      */
      std::map<int,EtaPhiHits> etaPhiHitsPerChamber;
      std::set<Identifier>     clusterIds;


      
      const Trk::PrepRawData* prd = (*it).prepRawDataVec().front();
      if( !prd ) continue;
      Identifier id = prd->identify();
      
      // apply cut on the opening angle between pattern and chamber phi
      // do some magic to avoid problems at phi = 0 and 2*pi
      double phiStart = patdire.phi();
      double chPhi  = prd->detectorElement()->center().phi();
      double phiRange = 0.75*M_PI;
      double phiRange2 = 0.25*M_PI;
      double phiOffset = 0.;
      if( phiStart > phiRange || phiStart < -phiRange ) phiOffset = 2*M_PI;
      else if( phiStart > -phiRange2 && phiStart < phiRange2 ) phiOffset = M_PI;

      if( phiOffset > 1.5*M_PI ){
	if( phiStart < 0 ) phiStart += phiOffset;
	if( chPhi < 0 )    chPhi += phiOffset;
      }else if( phiOffset > 0. ) {
	phiStart += phiOffset;
	chPhi    += phiOffset;
      }
      double dphi = fabs(phiStart - chPhi);
      
      if( dphi > m_phiAngleCut ) {
	ATH_MSG_DEBUG(" Large angular phi difference between pattern and chamber " << std::endl
		      << " phi pattern " << patdire.phi() << "   phi chamber " << prd->detectorElement()->center().phi());
	continue;
      }

      // map to find duplicate hits in the chamber
      std::map<Identifier,const MdtPrepData*> idMdtMap;

      std::vector< const Trk::PrepRawData* >::const_iterator pit = (*it).prepRawDataVec().begin();
      for (; pit!=(*it).prepRawDataVec().end(); ++pit) {

	if( !(*pit) ) continue;

	const MdtPrepData* mdt = dynamic_cast<const MdtPrepData*>(*pit);
	if( mdt) {
	  
	  if( m_removeDoubleMdtHits ){
	    const MdtPrepData*& previousMdt = idMdtMap[mdt->identify()];
	    if( !previousMdt || previousMdt->tdc() > mdt->tdc() ) previousMdt = mdt;
	    else continue;
	  }

	  insertMdt(*mdt,regionMap,patpose,patdire,hasPhiMeasurements);
	}else{
	  
	  const MuonCluster* clus = dynamic_cast<const MuonCluster*>(*pit);
	  if( !clus ) continue;
	  const Identifier& id = clus->identify();
	  
	  if( clusterIds.count(id) ) continue;
	  clusterIds.insert(id);
	  
	  if( m_recoverTriggerHits ){
	    bool measuresPhi = m_idHelper->measuresPhi(id);
	    int colHash = clus->collectionHash();

	    EtaPhiHits& hitsPerChamber = etaPhiHitsPerChamber[colHash];
	    if( measuresPhi ) ++hitsPerChamber.nphi;
	    else              ++hitsPerChamber.neta;
	  }
	  insertCluster(*clus,regionMap,patpose,patdire,hasPhiMeasurements);
	}
      }
      if( !etaPhiHitsPerChamber.empty() ){
	std::map<int,EtaPhiHits>::iterator chit = etaPhiHitsPerChamber.begin(); 
	std::map<int,EtaPhiHits>::iterator chit_end = etaPhiHitsPerChamber.end(); 
	for( ;chit!=chit_end;++chit ){
	  EtaPhiHits& hits = chit->second;
	  if( (hits.neta > 0 && hits.nphi == 0) || (hits.nphi > 0 && hits.neta == 0) ){
	    if( m_idHelper->isRpc(id) && m_rpcPrdContainer ){
	      
	      RpcPrepDataContainer::const_iterator pos = m_rpcPrdContainer->indexFind(chit->first);
	      if( pos == m_rpcPrdContainer->end() ){
		ATH_MSG_DEBUG(" RpcPrepDataCollection not found in container!! ");
	      }else{
		RpcPrepDataCollection::const_iterator rpcit = (*pos)->begin();
		RpcPrepDataCollection::const_iterator rpcit_end = (*pos)->end();
		for( ;rpcit!=rpcit_end;++rpcit ){
		  if( clusterIds.count( (*rpcit)->identify() ) ) continue;
		  const MuonCluster* clus = dynamic_cast<const MuonCluster*>(*rpcit);
		  insertCluster(*clus,regionMap,patpose,patdire,hasPhiMeasurements);		  
		  clusterIds.insert(clus->identify());
		}
	      }
	    }else if( m_idHelper->isTgc(id) && m_tgcPrdContainer ){
	      TgcPrepDataContainer::const_iterator pos = m_tgcPrdContainer->indexFind(chit->first);
	      if( pos == m_tgcPrdContainer->end() ){
		ATH_MSG_DEBUG(" TgcPrepDataCollection not found in container!! ");
	      }else{
		TgcPrepDataCollection::const_iterator tgcit = (*pos)->begin();
		TgcPrepDataCollection::const_iterator tgcit_end = (*pos)->end();
		for( ;tgcit!=tgcit_end;++tgcit ){
		  if( clusterIds.count( (*tgcit)->identify() ) ) continue;
		  const MuonCluster* clus = dynamic_cast<const MuonCluster*>(*tgcit);
		  insertCluster(*clus,regionMap,patpose,patdire,hasPhiMeasurements);		  
		  clusterIds.insert(clus->identify());
		}
	      }
	    }
	  }
	}
      }      
    }
  }

  void MuonPatternSegmentMaker::insertCluster( const MuonCluster& clus, RegionMap& regionMap, 
					       const Amg::Vector3D& patpose, const Amg::Vector3D& patdire,
					       bool hasPhiMeasurements ) const {

    const Identifier& id = clus.identify();
    // check whether we are measuring phi or eta
    bool measuresPhi = m_idHelper->measuresPhi(id);
	  
    Amg::Vector3D globalpos = clus.globalPosition();
    Amg::Vector3D intersect;

    if( hasPhiMeasurements  ){
      // if there is a phi measurement in the pattern use the global direction to calculate the intersect with measurement plane
      // and use the intersect to calculate the position along the strip
	
      // calculate intersect pattern measurement plane
      const Trk::Surface& surf = clus.detectorElement()->surface(id);
      const Amg::Vector3D& planepostion = surf.center();
      const Amg::Vector3D& planenormal  = surf.normal();
      double denom = patdire.dot(planenormal);
      double u = (planenormal.dot(planepostion - patpose))/denom;
      Amg::Vector3D  piOnPlane = ( patpose + u * patdire);

      // transform to local plane coordiantes
      const Amg::Transform3D gToLocal = clus.detectorElement()->surface().transform().inverse();
      Amg::Vector3D ilpos = gToLocal*piOnPlane;
      Amg::Vector3D glpos = gToLocal*globalpos;
	
      // strip length
      double striplen(0.);

      // projective strips
      bool hasPointingPhiStrips= false;

      // detector specific stuff
      const RpcPrepData* rpc = dynamic_cast<const RpcPrepData*>( &clus );	
      if( rpc ){
	striplen = rpc->detectorElement()->StripLength(measuresPhi);
      }else{
	const TgcPrepData* tgc = dynamic_cast<const TgcPrepData*>( &clus );
	if( !tgc ) return;

	int gasGap = m_idHelper->tgcIdHelper().gasGap(id);
	if( measuresPhi){
	  hasPointingPhiStrips = true;
	  striplen = tgc->detectorElement()->StripLength(gasGap);
	}else{ 
	  int wire = m_idHelper->tgcIdHelper().channel(id);
	  striplen = tgc->detectorElement()->WireLength(gasGap,wire);
	}
      }	
      
      // set the position along the strip
      if( !measuresPhi ) {
	glpos[0] = ilpos.x() ;
      }else{
	if( hasPointingPhiStrips ) {
	  // do some special for tgcs
	  glpos[1] = ilpos.y();
	}else{
	  glpos[1] = ilpos.y();
	}
      }
	
      // transform back to global coordinates
      intersect = gToLocal.inverse()*glpos;
      Amg::Vector3D dif = globalpos - intersect;
      if( (intersect - piOnPlane).mag() > m_dropDistance || dif.mag() > 0.5*striplen + m_dropDistance ){

	ATH_MSG_VERBOSE(" >>>> extrapolated position far outside volume, dropping hit " 
			<< m_idHelper->toString( id ) << std::endl
			<< "  dist along strip " << dif.mag() << " 1/2 strip len " << 0.5*striplen 
			<< " dist measurement plane " << (intersect - piOnPlane).mag());
	return;
      }
      if( dif.mag() > 0.5*striplen ){
	Amg::Vector3D newpos = globalpos - dif*(0.5*striplen/dif.mag());
	
	ATH_MSG_VERBOSE(" >>>> extrapolated position outside volume, shifting position " 
			<< m_idHelper->toString( id ) << std::endl
			<< "  position along strip " << dif.mag() << " 1/2 tube len " << 0.5*striplen
			<< " dist To strip " << (intersect - piOnPlane).mag() << std::endl
			<< "  dist to newpos " << (newpos-globalpos).mag() << " pos " << newpos);
	
	intersect = newpos;
      }
    }else{
      // no phi measurements, use strip center
      intersect = globalpos;
    }
      
    // enter hit in map
    int regionId = getRegionId( id );

    RegionMapIt mip = regionMap.find(regionId);
    if(  mip == regionMap.end() ){
      Region region;
      region.triggerPrds.push_back( std::make_pair( intersect, &clus ) );
      region.regionDir = patdire;
      region.regionPos = patpose;
      regionMap.insert( std::make_pair(regionId,region) );
    }else{
      Region& region = mip->second;
      region.triggerPrds.push_back( std::make_pair( intersect, &clus ) );
    }
  }


  void MuonPatternSegmentMaker::insertMdt( const MdtPrepData& mdt, RegionMap& regionMap, 
					   const Amg::Vector3D& patpose, const Amg::Vector3D& patdire,
					   bool hasPhiMeasurements ) const {

    Amg::Vector3D intersect;
    const Identifier& id = mdt.identify();

    const MuonGM::MdtReadoutElement* detEl = mdt.detectorElement();
    const Amg::Vector3D& tubePos = mdt.globalPosition();
	  
    if( hasPhiMeasurements ){
      // if there is a phi measurement in the pattern use the global direction to calculate the intersect with the tube
      // use the intersect to calculate the second coordinate

      const Amg::Transform3D amdbToGlobal = detEl->AmdbLRSToGlobalTransform();
	    
	    
      // calculate intersect pattern measurement plane
      const Amg::Vector3D& planepostion = tubePos;
	    
      // always project on plane with normal in radial direction
      Amg::Vector3D planenormal = !m_idHelper->isEndcap(id) ? 
	amdbToGlobal.linear()*Amg::Vector3D(0.,0.,1.) : amdbToGlobal.linear()*Amg::Vector3D(0.,1.,0.);
	    
      double denom = patdire.dot(planenormal);
      double u = (planenormal.dot(planepostion - patpose))/denom;
      Amg::Vector3D  piOnPlane = ( patpose + u * patdire);
	    
      Amg::Vector3D lpiOnPlane = amdbToGlobal.inverse()*piOnPlane;
      Amg::Vector3D ltubePos   = amdbToGlobal.inverse()*tubePos;
	    
      intersect = amdbToGlobal*Amg::Vector3D( lpiOnPlane.x(), ltubePos.y(), ltubePos.z() );
	    
      Amg::Vector3D dif = tubePos - intersect;
      double tubelen = detEl->getActiveTubeLength( m_idHelper->mdtIdHelper().tubeLayer(id),m_idHelper->mdtIdHelper().tube(id) );
	    
      if( dif.mag() > 0.5*tubelen ){
	Amg::Vector3D newpos = tubePos - dif*(0.5*tubelen/dif.mag());
	      
	ATH_MSG_VERBOSE(" >>>> extrapolated position outside volume, shifting position "
			<< m_idHelper->toString( id ) << std::endl
			<< "  position along strip " << dif.mag() << " 1/2 tube len " << 0.5*tubelen
			<< " dist To Wire " << (piOnPlane-intersect).mag() << std::endl
			<< "  dist to newpos " << (newpos-tubePos).mag() << " pos " << newpos);
	      
	intersect = newpos;
      }
    }else{
      // not phi measurement, use tube center
      intersect = tubePos;
    }
	  
    // enter hit in map
    Identifier elId = m_idHelper->mdtIdHelper().elementID( id );
	  
    MuonStationIndex::ChIndex chIndex = m_idHelper->chamberIndex(elId);
    int chFlag = elId.get_identifier32().get_compact();
    if( m_doMultiAnalysis ){
      if( m_idHelper->isSmallChamber(id) ){
	ATH_MSG_VERBOSE(" Small chamber " << m_idHelper->toString( elId ));
	chFlag = 0;
	if( chIndex == MuonStationIndex::BIS ){
	  int eta = m_idHelper->stationEta(elId);
	  if( abs(eta) == 8 ) {
	    ATH_MSG_VERBOSE(" BIS8 chamber " << m_idHelper->toString( elId ));
	    chFlag = 3;
	  }
	}
      }else{
	ATH_MSG_VERBOSE(" Large chamber " << m_idHelper->toString( elId ));
	chFlag = 1;
	if( chIndex == MuonStationIndex::BIL ){
	  std::string stName = m_idHelper->chamberNameString(id);
	  if( stName[2] == 'R' ){
	    ATH_MSG_VERBOSE(" BIR chamber " << m_idHelper->toString( elId ));
	    chFlag = 2;
	  }
	}
	else if( chIndex == MuonStationIndex::BOL ){
	  if( abs( m_idHelper->stationEta(id) ) == 7 ){
	    ATH_MSG_VERBOSE(" BOE chamber " << m_idHelper->toString( elId ));
	    chFlag = 4;
	  }
	}
      }
      
      int phi = m_idHelper->stationPhi(id);
      
      chFlag += 10*phi;
    }
    // use center tube for region assignment
    int regionId = getRegionId( id );

    RegionMapIt mip = regionMap.find(regionId);
    if(  mip == regionMap.end() ){
      Region region;
      region.mdtPrdsPerChamber[chFlag].push_back( std::make_pair( intersect, &mdt ) );
      region.regionPos = patpose;
      region.regionDir = patdire;
      regionMap.insert( std::make_pair(regionId,region) );
    }else{
      Region& region = mip->second;
      region.mdtPrdsPerChamber[chFlag].push_back( std::make_pair( intersect, &mdt ) );
    }

  }

  void MuonPatternSegmentMaker::clearRotsPerRegion( MuonPatternSegmentMaker::ROTsPerRegion& hitsPerRegion ) const 
  {
    // loop over regions, delete all ROTS in map
    ROTsPerRegionIt rit = hitsPerRegion.begin();
    ROTsPerRegionIt rit_end = hitsPerRegion.end();
    for( ; rit!=rit_end;++rit ){

      // loop over mdt hits vectors in region
      MdtVecIt mdtvit     = rit->mdts.begin();
      MdtVecIt mdtvit_end = rit->mdts.end();
      for( ; mdtvit!=mdtvit_end; ++mdtvit ){
      
	// loop over mdt hits
	MdtIt mdtit     = mdtvit->begin();
	MdtIt mdtit_end = mdtvit->end();
	for( ; mdtit!=mdtit_end;++mdtit ){
	  delete *mdtit;
	}

      }

      // loop over clusters in region
      ClusterIt clit = rit->clusters.begin();
      ClusterIt clit_end = rit->clusters.end();
      for( ;clit!=clit_end;++clit ){
	delete *clit;
      }    
    }

  }


  void MuonPatternSegmentMaker::printRegionMap( const RegionMap& regionMap) const {

    RegionMapCit mit = regionMap.begin();
    RegionMapCit mit_end = regionMap.end();
    msg(MSG::INFO) << "Summarizing input " << std::endl; 

    for( ; mit!=mit_end;++mit ){
      msg(MSG::INFO) << "new region " << mit->first << " trigger " << mit->second.triggerPrds.size() 
		     << " mdt ch " << mit->second.mdtPrdsPerChamber.size() << std::endl;
    
      if( !mit->second.triggerPrds.empty() ) msg(MSG::INFO) << " trigger hits " << mit->second.triggerPrds.size() << std::endl;


      ISPrdIt pit = mit->second.triggerPrds.begin();
      ISPrdIt pit_end = mit->second.triggerPrds.end();
      for( ;pit!=pit_end;++pit ){
	msg(MSG::INFO) << "  " << m_printer->print(*(pit->second)) << std::endl;;
      }

      RegionIdMapIt idit = mit->second.mdtPrdsPerChamber.begin();
      RegionIdMapIt idit_end = mit->second.mdtPrdsPerChamber.end();
      for( ;idit!=idit_end;++idit ){
	msg(MSG::INFO) << " new MDT chamber with " << idit->second.size() << " hits" << std::endl;
      
	ISPrdMdtIt mdtit = idit->second.begin();
	ISPrdMdtIt mdtit_end = idit->second.end();
	for( ;mdtit!=mdtit_end;++mdtit ){
	  const MdtPrepData* prd = mdtit->second;	
	  msg(MSG::INFO) << "  " << m_printer->print(*prd) << std::endl;
	}
      }
    }
    msg(MSG::INFO).doOutput();

  }

  void MuonPatternSegmentMaker::calibrateRegionMap( const RegionMap& regionMap, ROTsPerRegion& hitsPerRegion ) const 
  {

    RegionMapCit mit = regionMap.begin();
    RegionMapCit mit_end = regionMap.end();

    for( ; mit!=mit_end;++mit ){


      ROTRegion rotRegion;
      rotRegion.regionId  = mit->first;
      rotRegion.regionPos = mit->second.regionPos;
      rotRegion.regionDir = mit->second.regionDir;


      ISPrdIt pit = mit->second.triggerPrds.begin();
      ISPrdIt pit_end = mit->second.triggerPrds.end();
      for( ;pit!=pit_end;++pit ){
      
	if( !pit->second ) continue;

	const MuonClusterOnTrack* cluster = m_clusterCreator->createRIO_OnTrack( *(pit->second), pit->first ); 
	if( !cluster ) continue;

	rotRegion.clusters.push_back( cluster );

      }

      RegionIdMapIt idit = mit->second.mdtPrdsPerChamber.begin();
      RegionIdMapIt idit_end = mit->second.mdtPrdsPerChamber.end();
      for( ;idit!=idit_end;++idit ){

	ISPrdMdtIt mdtit = idit->second.begin();
	ISPrdMdtIt mdtit_end = idit->second.end();
      
	MdtVec mdtROTs;
	for( ;mdtit!=mdtit_end;++mdtit ){
	  const MdtPrepData* prd = mdtit->second;	
	  if( !prd ) continue;
	  Amg::Vector3D dr=mdtit->first;
	  const MdtDriftCircleOnTrack* mdt = m_mdtCreator->createRIO_OnTrack( *prd, mdtit->first,  &dr ); 

	  if( !mdt ) {
	    ATH_MSG_VERBOSE(" Failed to calibrate " << m_idHelper->toString(prd->identify()));
	    continue;
	  }
	  mdtROTs.push_back( mdt ); 
	}
	if( !mdtROTs.empty() ) rotRegion.mdts.push_back( mdtROTs );
      }
      hitsPerRegion.push_back( rotRegion );
    }

  }

  const MuonSegment* MuonPatternSegmentMaker::isSubSet( const MuonSegment* seg1,  const MuonSegment* seg2 ) const
  {
    double theta1 = seg1->globalDirection().theta();
    double phi1   = seg1->globalDirection().phi();
    double R1     = seg1->globalPosition().perp();
    double z1     = seg1->globalPosition().z();
  
    double theta2 = seg2->globalDirection().theta();
    double phi2   = seg2->globalDirection().phi();
    double R2     = seg2->globalPosition().perp();
    double z2     = seg2->globalPosition().z();
  
    double dtheta = theta1-theta2;
    double dphi = phi1-phi2;
    double dR = R1-R2;
    double dz = z1-z2;

    if( std::abs(dtheta) < 0.001 && std::abs(dphi) < 0.001 && std::abs( dR*dR + dz*dz ) < 0.01 ){
      return seg1;
    }
    return 0;
  }

  void MuonPatternSegmentMaker::retrieveTriggerHitContainers() const {
    m_rpcPrdContainer=0;
    SG::ReadHandle<Muon::RpcPrepDataContainer> RpcCont(m_keyRpc);
    if( !RpcCont.isValid() ) {
      ATH_MSG_DEBUG(" Failed to retrieve RpcPrepDataContainer, will not recover rpc trigger hits ");
    }
    else m_rpcPrdContainer=RpcCont.cptr();
    
    m_tgcPrdContainer=0;
    SG::ReadHandle<Muon::TgcPrepDataContainer> TgcCont(m_keyTgc);
    if(!TgcCont.isValid() ) {
	ATH_MSG_DEBUG(" Failed to retrieve TgcPrepDataContainer, will not recover tgc trigger hits ");
    }
    else m_tgcPrdContainer=TgcCont.cptr();
    
  }
}
