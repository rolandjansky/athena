/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPatternSegmentMaker/EventNtWriter.h"

#include "MuonPattern/MuonPatternCombination.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include <iostream>


EventNtWriter::EventNtWriter(std::string name, const MdtIdHelper* mdtId, 
			     const RpcIdHelper* rpcId, const TgcIdHelper* tgcId, const MuonGM::MuonDetectorManager* detMgr) : 
  m_mdtIdHelper(mdtId),  m_tgcIdHelper(tgcId),  m_rpcIdHelper(rpcId), m_detMgr(detMgr)
{
  name += ".root";
  m_file = new TFile(name.c_str(),"RECREATE");
  m_file->cd();
  m_tree= new TTree("Segments","Segments");

  m_tree->Branch("nmdt",&m_nmdt,"nmdt/I");
  m_tree->Branch("mdtt",&m_mdtt,"mdtt[nmdt]/F");
  m_tree->Branch("mdtr",&m_mdtr,"mdtr[nmdt]/F");
  m_tree->Branch("mdtdr",&m_mdtdr,"mdtdr[nmdt]/F");
  m_tree->Branch("mdtx",&m_mdtx,"mdtx[nmdt]/F");
  m_tree->Branch("mdty",&m_mdty,"mdty[nmdt]/F");
  m_tree->Branch("mdtz",&m_mdtz,"mdtz[nmdt]/F");
  m_tree->Branch("mdtgx",&m_mdtgx,"mdtgx[nmdt]/F");
  m_tree->Branch("mdtgy",&m_mdtgy,"mdtgy[nmdt]/F");
  m_tree->Branch("mdtgz",&m_mdtgz,"mdtgz[nmdt]/F");
  m_tree->Branch("mdtid",&m_mdtid,"mdtid[nmdt]/I");
  m_tree->Branch("mdtpatid",&m_mdtpatid,"mdtpatid[nmdt]/I");
  m_tree->Branch("mdtsegid",&m_mdtsegid,"mdtsegid[nmdt]/I");

  m_tree->Branch("ntrig",&m_ntrig,"ntrig/I");
  m_tree->Branch("trigerr",&m_trigerr,"trigerr[ntrig]/F");
  m_tree->Branch("trigx",&m_trigx,"trigx[ntrig]/F");
  m_tree->Branch("trigy",&m_trigy,"trigy[ntrig]/F");
  m_tree->Branch("trigz",&m_trigz,"trigz[ntrig]/F");
  m_tree->Branch("triggx",&m_triggx,"triggx[ntrig]/F");
  m_tree->Branch("triggy",&m_triggy,"triggy[ntrig]/F");
  m_tree->Branch("triggz",&m_triggz,"triggz[ntrig]/F");
  m_tree->Branch("trigid",&m_trigid,"trigud[ntrig]/I");
  m_tree->Branch("trigpatid",&m_trigpatid,"trigsegid[ntrig]/I");
  m_tree->Branch("trigsegid",&m_trigsegid,"trigsegid[ntrig]/I");

  m_tree->Branch("nseg",&m_nseg,"nseg/I");
  m_tree->Branch("segid",&m_segid,"segid[nseg]/I");
  m_tree->Branch("seggeo",&m_seggeo,"seggeo[nseg]/I");
  m_tree->Branch("seggeotd",&m_seggeotd,"seggeotd[nseg]/F");
  m_tree->Branch("seggeots",&m_seggeots,"seggeots[nseg]/F");
  m_tree->Branch("seggeold",&m_seggeold,"seggeold[nseg]/F");
  m_tree->Branch("seggeoft0y",&m_seggeoft0y,"seggeoft0y[nseg]/F");
  m_tree->Branch("seggeoft0z",&m_seggeoft0z,"seggeoft0z[nseg]/F");
  m_tree->Branch("seggeoft1y",&m_seggeoft1y,"seggeoft1y[nseg]/F");
  m_tree->Branch("seggeoft1z",&m_seggeoft1z,"seggeoft1z[nseg]/F");
  m_tree->Branch("segid1",&m_segid1,"segid1[nseg]/I");
  m_tree->Branch("seggeo1",&m_seggeo1,"seggeo1[nseg]/I");
  m_tree->Branch("seggeotd1",&m_seggeotd1,"seggeotd1[nseg]/F");
  m_tree->Branch("seggeots1",&m_seggeots1,"seggeots1[nseg]/F");
  m_tree->Branch("seggeold1",&m_seggeold1,"seggeold1[nseg]/F");
  m_tree->Branch("seggeoft0y1",&m_seggeoft0y1,"seggeoft0y1[nseg]/F");
  m_tree->Branch("seggeoft0z1",&m_seggeoft0z1,"seggeoft0z1[nseg]/F");
  m_tree->Branch("seggeoft1y1",&m_seggeoft1y1,"seggeoft1y1[nseg]/F");
  m_tree->Branch("seggeoft1z1",&m_seggeoft1z1,"seggeoft1z1[nseg]/F");
  m_tree->Branch("segx",&m_segx,"segx[nseg]/F");
  m_tree->Branch("segy",&m_segy,"segy[nseg]/F");
  m_tree->Branch("segz",&m_segz,"segz[nseg]/F");
  m_tree->Branch("seggx",&m_seggx,"seggx[nseg]/F");
  m_tree->Branch("seggy",&m_seggy,"seggy[nseg]/F");
  m_tree->Branch("seggz",&m_seggz,"seggz[nseg]/F");
  m_tree->Branch("segdx",&m_segdx,"segdx[nseg]/F");
  m_tree->Branch("segdy",&m_segdy,"segdy[nseg]/F");
  m_tree->Branch("segdz",&m_segdz,"segdz[nseg]/F");
  m_tree->Branch("segdgx",&m_segdgx,"segdgx[nseg]/F");
  m_tree->Branch("segdgy",&m_segdgy,"segdgy[nseg]/F");
  m_tree->Branch("segdgz",&m_segdgz,"segdgz[nseg]/F");
  m_tree->Branch("segpatid",&m_segpatid,"segpatid[nseg]/I");

  m_tree->Branch("npat",&m_npat,"npat/I");
  m_tree->Branch("patgx",&m_patgx,"patgx[npat]/F");
  m_tree->Branch("patgy",&m_patgy,"patgy[npat]/F");
  m_tree->Branch("patgz",&m_patgz,"patgz[npat]/F");
  m_tree->Branch("patdgx",&m_patdgx,"patdgx[npat]/F");
  m_tree->Branch("patdgy",&m_patdgy,"patdgy[npat]/F");
  m_tree->Branch("patdgz",&m_patdgz,"patdgz[npat]/F");
  m_nmdt = 0;
  m_ntrig = 0;
  m_nseg = 0;
  m_npat = 0;
}

void EventNtWriter::close()
{
  m_file->cd();
  m_tree->Write();

  m_file->Write();
  m_file->Close();    
}

void EventNtWriter::endEvent()
{
  m_tree->Fill();
  m_nmdt = 0;
  m_ntrig = 0;
  m_nseg = 0;
  m_npat = 0;
}

void EventNtWriter::fill( Muon::MuonPatternSegmentMaker::ROTsPerRegion& rots, const Muon::MuonPatternCombination* pattern, bool hasPhi )
{
  // sanity check
  if( !m_mdtIdHelper || !m_tgcIdHelper || !m_rpcIdHelper || !m_detMgr ) return;

  Amg::Vector3D pgpos;
  Amg::Vector3D pgdir;

  //  std::cout << " filling pattern " << std::endl;
  if (pattern->trackParameter()) { //  TODO: 0 at the moment! we need convention howto fill this, JS
    pgpos = pattern->chamberData().front().intersectPosition();
    pgdir = pattern->chamberData().front().intersectDirection();
  }
  else {
    pgpos = pattern->chamberData().front().intersectPosition();
    pgdir = pattern->chamberData().front().intersectDirection();
  }

  if( m_npat < 10 ){
     m_patgx[m_npat] = pgpos.x();
     m_patgy[m_npat] = pgpos.y();
     m_patgz[m_npat] = pgpos.z();
     m_patdgx[m_npat] = pgdir.x();
     m_patdgy[m_npat] = pgdir.y();
     m_patdgz[m_npat] = pgdir.z();    
     ++m_npat;
  }else{
    //std::cout << " to many patterns " << std::endl;
  }
  
  Muon::MuonPatternSegmentMaker::ROTsPerRegionIt rit = rots.begin();
  Muon::MuonPatternSegmentMaker::ROTsPerRegionIt rit_end = rots.end();
  for( ; rit!=rit_end;++rit){

    Muon::MuonPatternSegmentMaker::ROTRegion& rotRegion = *rit;
    
    Muon::MuonPatternSegmentMaker::MdtVecIt mit = rotRegion.mdts.begin();
    Muon::MuonPatternSegmentMaker::MdtVecIt mit_end = rotRegion.mdts.end();
    for( ; mit != mit_end; ++mit ){
      
      Muon::MuonPatternSegmentMaker::MdtVec& mvec = *mit;
      if( mvec.empty() ) continue;

      Amg::Transform3D gToStation = mvec.front()->prepRawData()->detectorElement()->GlobalToAmdbLRSTransform();
      Identifier chid = m_mdtIdHelper->elementID(mvec.front()->prepRawData()->identify());
      
//       Amg::Vector3D pgpos = pattern->globalPosition();
//       Amg::Vector3D pgdir = pattern->globalDirection();

      Amg::Vector3D plpos = gToStation*pgpos;
      Amg::Vector3D pldir = gToStation.linear()*pgdir;

      double errorScaleFactor = 1.;
      if( !hasPhi ){

	// rescale errors 
	int stRegion = m_mdtIdHelper->stationRegion(chid);
	if( stRegion == 0 ) errorScaleFactor = 2.;        // inner
	else if( stRegion == 1 ) errorScaleFactor = 2.5;  // extended
	else if( stRegion == 2 ) errorScaleFactor = 2.5;  // middle
	else errorScaleFactor = 3.;                       // outer
      }

      
      if( m_nseg < 50 ){
	int seg_id = -1;
	int seg_geo = -1;
	double tubeDist = 0;
	double tubeStage = 0;
	double layDist = 0;
	Amg::Vector3D firstTubeMl0(0., 0., 0.);
	Amg::Vector3D firstTubeMl1(0., 0., 0.);
	if( m_mdtIdHelper ){
	  int eta =  m_mdtIdHelper->stationEta(chid);
	  int phi = m_mdtIdHelper->stationPhi(chid);
	  int name = m_mdtIdHelper->stationName(chid);
	  int isBarrel = m_mdtIdHelper->isBarrel(chid);
	  int nml = m_mdtIdHelper->numberOfMultilayers(chid);
	  if( m_detMgr ){
	    const MuonGM::MdtReadoutElement* detEl1 = m_detMgr->getMdtReadoutElement( m_mdtIdHelper->channelID( name,eta,phi,1,1,1 ) );
	    const MuonGM::MdtReadoutElement* detEl2 = nullptr;
	    int ntube2 = 0;
	    if( nml == 2 ){
	      Identifier firstIdml1 = m_mdtIdHelper->channelID( name,eta,phi,2,1,1 );
	      detEl2 = m_detMgr->getMdtReadoutElement( firstIdml1 ); 
	      firstTubeMl1 = gToStation*(detEl2->tubePos( firstIdml1 ));
	      ntube2 = detEl2->getNtubesperlayer();
	    }
	    
	    int nlay = detEl1->getNLayers();
	    int ntube1 = detEl1->getNtubesperlayer();
	    Identifier firstIdml0 = m_mdtIdHelper->channelID( name,eta,phi,1,1,1 );
	    firstTubeMl0 = gToStation*(detEl1->tubePos( firstIdml0 ));
	    
	    Identifier secondIdml0 = m_mdtIdHelper->channelID( name,eta,phi,1,1,2 );
	    Amg::Vector3D secondTubeMl0 = gToStation*(detEl1->tubePos( secondIdml0 ));

	    Identifier firstIdml0lay1 = m_mdtIdHelper->channelID( name,eta,phi,1,2,1 );
	    Amg::Vector3D firstTubeMl0lay1 = gToStation*(detEl1->tubePos( firstIdml0lay1 ));
	    
	    
	    tubeDist = (secondTubeMl0 - firstTubeMl0).y();
	    tubeStage = (firstTubeMl0lay1 - firstTubeMl0).y();
	    layDist = (firstTubeMl0lay1 - firstTubeMl0).z();
// 	    std::cout << " local chamber geometry " << m_mdtIdHelper->print_to_string(firstIdml0) << std::endl;
// 	    std::cout << " first tubes " << firstTubeMl0 << "  ml2 " << firstTubeMl1
// 		      << " sec ml0 " << secondTubeMl0 << std::endl;
// 	    std::cout << " tubeDist " << (secondTubeMl0 - firstTubeMl0)  << " " << tubeDist
// 		      << " tubeStage " << (firstTubeMl0lay1 - firstTubeMl0) << "  " << tubeStage 
// 		      << " ldist " << layDist << std::endl;
	    

	    if( nlay > 4 ){
	      std::cout << " TO MANY LAYERS in " << std::endl;
	      std::cout << " nml " << nml << " lay " << nlay << " nt1 " << ntube1 << "  nt2 " << ntube2 << std::endl;
	    }
	    //	    std::cout << m_mdtIdHelper->print_to_string(chid) << std::endl;
	    seg_geo = 100000*nml+10000*nlay+100*ntube2+ntube1;
// 	    std::cout << " seg_geo " << seg_geo << std::endl;
	  }
	  seg_id = 1000000*isBarrel+10000*name+100*(50+eta)+phi;
// 	  std::cout << " seg_id " << seg_id << std::endl;
	}
	m_segid[m_nseg] = seg_id;
	m_seggeo[m_nseg] = seg_geo;
	m_seggeotd[m_nseg] = tubeDist;
	m_seggeots[m_nseg] = tubeStage;
	m_seggeold[m_nseg] = layDist;
//  	std::cout << " - tubeDist " << tubeDist
// 		  << " tubeStage " << tubeStage 
// 		  << " ldist " << layDist << std::endl;
	
	m_seggeoft0y[m_nseg] = firstTubeMl0.y();
	m_seggeoft0z[m_nseg] = firstTubeMl0.z();
	m_seggeoft1y[m_nseg] = firstTubeMl1.y();
	m_seggeoft1z[m_nseg] = firstTubeMl1.z();
	
	//  ----------------- check if hits from more than one chamber are present
	m_segid1[m_nseg] = -1;
	Muon::MuonPatternSegmentMaker::MdtIt mmit = mvec.begin();
	Muon::MuonPatternSegmentMaker::MdtIt mmit_end = mvec.end();
	for( ; mmit!=mmit_end;++mmit){
	
	  Identifier newchid = m_mdtIdHelper->elementID( (*mmit)->identify() );


	  if( newchid != chid ){

// 	    std::cout << " hits from second chamber " << std::endl;
	    int seg_id = -1;
	    int seg_geo = -1;
	    double tubeDist = 0;
	    double tubeStage = 0;
	    double layDist = 0;
	    Amg::Vector3D firstTubeMl0(0., 0., 0.);
	    Amg::Vector3D firstTubeMl1(0., 0., 0.);
	    if( m_mdtIdHelper ){
	      int eta =  m_mdtIdHelper->stationEta(newchid);
	      int phi = m_mdtIdHelper->stationPhi(newchid);
	      int name = m_mdtIdHelper->stationName(newchid);
	      int isBarrel = m_mdtIdHelper->isBarrel(newchid);
	      int nml = m_mdtIdHelper->numberOfMultilayers(newchid);
	      if( m_detMgr ){
		const MuonGM::MdtReadoutElement* detEl1 = m_detMgr->getMdtReadoutElement( m_mdtIdHelper->channelID( name,eta,phi,1,1,1 ) );
		const MuonGM::MdtReadoutElement* detEl2 = nullptr;
		int ntube2 = 0;
		if( nml == 2 ){
		  Identifier firstIdml1 = m_mdtIdHelper->channelID( name,eta,phi,2,1,1 );
		  detEl2 = m_detMgr->getMdtReadoutElement( firstIdml1 ); 
		  firstTubeMl1 = gToStation*(detEl2->tubePos( firstIdml1 ));
		  ntube2 = detEl2->getNtubesperlayer();
		}
		
		int nlay = detEl1->getNLayers();
		int ntube1 = detEl1->getNtubesperlayer();
		Identifier firstIdml0 = m_mdtIdHelper->channelID( name,eta,phi,1,1,1 );
		firstTubeMl0 = gToStation*(detEl1->tubePos( firstIdml0 ));
		
		Identifier secondIdml0 = m_mdtIdHelper->channelID( name,eta,phi,1,1,2 );
		Amg::Vector3D secondTubeMl0 = gToStation*(detEl1->tubePos( secondIdml0 ));
		
		Identifier firstIdml0lay1 = m_mdtIdHelper->channelID( name,eta,phi,1,2,1 );
		Amg::Vector3D firstTubeMl0lay1 = gToStation*(detEl1->tubePos( firstIdml0lay1 ));
		
		
		tubeDist = (secondTubeMl0 - firstTubeMl0).y();
		tubeStage = (firstTubeMl0lay1 - firstTubeMl0).y();
		layDist = (firstTubeMl0lay1 - firstTubeMl0).z();
// 		std::cout << " local chamber geometry " << m_mdtIdHelper->print_to_string(firstIdml0) << std::endl;
// 		std::cout << " first tubes " << firstTubeMl0 << "  ml2 " << firstTubeMl1
// 			  << " sec ml0 " << secondTubeMl0 << std::endl;
// 		std::cout << " tubeDist " << (secondTubeMl0 - firstTubeMl0)  << " " << tubeDist
// 			  << " tubeStage " << (firstTubeMl0lay1 - firstTubeMl0) << "  " << tubeStage 
// 			  << " ldist " << layDist << std::endl;
		
		
		if( nlay > 4 ){
		  std::cout << " TO MANY LAYERS in " << std::endl;
		  std::cout << " nml " << nml << " lay " << nlay << " nt1 " << ntube1 << "  nt2 " << ntube2 << std::endl;
		}
		//		std::cout << m_mdtIdHelper->print_to_string(newchid) << std::endl;
		seg_geo = 100000*nml+10000*nlay+100*ntube2+ntube1;
// 		std::cout << " seg_geo " << seg_geo << std::endl;
	      }
	      seg_id = 100000*isBarrel+10000*name+100*(50+eta)+phi;
// 	      std::cout << " seg_id " << seg_id << std::endl;
	    }
	    m_segid1[m_nseg] = seg_id;
	    m_seggeo1[m_nseg] = seg_geo;
	    m_seggeotd1[m_nseg] = tubeDist;
	    m_seggeots1[m_nseg] = tubeStage;
	    m_seggeold1[m_nseg] = layDist;
// 	    std::cout << " - tubeDist " << tubeDist
// 		      << " tubeStage " << tubeStage 
// 		      << " ldist " << layDist << std::endl;
	    
	    m_seggeoft0y1[m_nseg] = firstTubeMl0.y();
	    m_seggeoft0z1[m_nseg] = firstTubeMl0.z();
	    m_seggeoft1y1[m_nseg] = firstTubeMl1.y();
	    m_seggeoft1z1[m_nseg] = firstTubeMl1.z();
	  }
	}

	// --------


	m_segx[m_nseg] = plpos.x();
	m_segy[m_nseg] = plpos.y();
	m_segz[m_nseg] = plpos.z();
	m_segdx[m_nseg] = pldir.x();
	m_segdy[m_nseg] = pldir.y();
	m_segdz[m_nseg] = pldir.z();
	m_seggx[m_nseg] = pgpos.x();
	m_seggy[m_nseg] = pgpos.y();
	m_seggz[m_nseg] = pgpos.z();
	m_segdgx[m_nseg] = pgdir.x();
	m_segdgy[m_nseg] = pgdir.y();
	m_segdgz[m_nseg] = pgdir.z();
	m_segpatid[m_nseg] = m_npat - 1;
	++m_nseg;
      }else{
// 	std::cout << " to many segments " << std::endl;
      }
    
      Muon::MuonPatternSegmentMaker::ClusterIt cit = rotRegion.clusters.begin();
      Muon::MuonPatternSegmentMaker::ClusterIt cit_end = rotRegion.clusters.end();
      for( ; cit!=cit_end;++cit ){
	
	if( m_ntrig < 500 ){
	  Identifier id = (*cit)->identify();
	  int clid(-1);
	  int eta = 0;
	  int phi = 0;
	  int name = 0;
	  if( m_rpcIdHelper && m_rpcIdHelper->is_rpc(id) ){
	    if( m_rpcIdHelper->measuresPhi(id) ) clid = 1;
	    else clid = 0;	    
	    eta = m_rpcIdHelper->stationEta(chid);
	    phi = m_rpcIdHelper->stationPhi(chid);
	    name = m_rpcIdHelper->stationName(chid);
	  }else if( m_tgcIdHelper && m_tgcIdHelper->is_tgc(id) ){
	    if( m_tgcIdHelper->isStrip(id) )     clid = 11;
	    else clid = 10;
	    eta = m_tgcIdHelper->stationEta(chid);
	    phi = m_tgcIdHelper->stationPhi(chid);
	    name = m_tgcIdHelper->stationName(chid);
	  }
	  clid += 100*(10000*name+100*eta+phi);

	  Amg::Vector3D gpos = (*cit)->globalPosition();
	  Amg::Vector3D lpos = gToStation*gpos;
	  m_trigerr[m_ntrig] = std::sqrt((*cit)->localCovariance()(0,0));
	  m_trigx[m_ntrig] = lpos.x();
	  m_trigy[m_ntrig] = lpos.y();
	  m_trigz[m_ntrig] = lpos.z();
	  m_triggx[m_ntrig] = gpos.x();
	  m_triggy[m_ntrig] = gpos.y();
	  m_triggz[m_ntrig] = gpos.z();
	  m_trigid[m_ntrig] = clid;
	  m_trigpatid[m_ntrig] = m_npat - 1;
	  m_trigsegid[m_ntrig] = m_nseg - 1;
	  ++m_ntrig;
	}else{
// 	  std::cout << " Too many trigger hits " << std::endl;
	}
      }

      Muon::MuonPatternSegmentMaker::MdtIt mmit = mvec.begin();
      Muon::MuonPatternSegmentMaker::MdtIt mmit_end = mvec.end();
      for( ; mmit!=mmit_end;++mmit){
	
	if( m_nmdt < 500 ){
	  Identifier id = (*mmit)->identify();

	  int mdt_id = -1;

	  if( m_mdtIdHelper ){
	    int ml    = m_mdtIdHelper->multilayer(id);
	    int layer = m_mdtIdHelper->tubeLayer(id);
	    int tube  = m_mdtIdHelper->tube(id);
	    int isBarrel = m_mdtIdHelper->isBarrel(chid);
	    mdt_id = 100000*isBarrel+10000*ml+100*layer+tube;
	  }
	  Amg::Vector3D gpos = (*mmit)->globalPosition();
	  Amg::Vector3D lpos = gToStation*gpos;
	  m_mdtt[m_nmdt] = 0.;
	  m_mdtr[m_nmdt] = (*mmit)->localParameters()[Trk::locR];
	  m_mdtdr[m_nmdt] = sqrt((*mmit)->localCovariance()(Trk::locR,Trk::locR))*errorScaleFactor;
	  m_mdtx[m_nmdt] = lpos.x();
	  m_mdty[m_nmdt] = lpos.y();
	  m_mdtz[m_nmdt] = lpos.z();
	  m_mdtgx[m_nmdt] = gpos.x();
	  m_mdtgy[m_nmdt] = gpos.y();
	  m_mdtgz[m_nmdt] = gpos.z();
	  m_mdtid[m_nmdt] = mdt_id;
	  m_mdtpatid[m_nmdt] = m_npat - 1;
	  m_mdtsegid[m_nmdt] = m_nseg - 1;
	  ++m_nmdt;
	}else{
// 	  std::cout << " to many mdt hits " << std::endl;
	}
      }
    }

  }

}
