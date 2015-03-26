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

  m_tree->Branch("nmdt",&nmdt,"nmdt/I");
  m_tree->Branch("mdtt",&mdtt,"mdtt[nmdt]/F");
  m_tree->Branch("mdtr",&mdtr,"mdtr[nmdt]/F");
  m_tree->Branch("mdtdr",&mdtdr,"mdtdr[nmdt]/F");
  m_tree->Branch("mdtx",&mdtx,"mdtx[nmdt]/F");
  m_tree->Branch("mdty",&mdty,"mdty[nmdt]/F");
  m_tree->Branch("mdtz",&mdtz,"mdtz[nmdt]/F");
  m_tree->Branch("mdtgx",&mdtgx,"mdtgx[nmdt]/F");
  m_tree->Branch("mdtgy",&mdtgy,"mdtgy[nmdt]/F");
  m_tree->Branch("mdtgz",&mdtgz,"mdtgz[nmdt]/F");
  m_tree->Branch("mdtid",&mdtid,"mdtid[nmdt]/I");
  m_tree->Branch("mdtpatid",&mdtpatid,"mdtpatid[nmdt]/I");
  m_tree->Branch("mdtsegid",&mdtsegid,"mdtsegid[nmdt]/I");

  m_tree->Branch("ntrig",&ntrig,"ntrig/I");
  m_tree->Branch("trigerr",&trigerr,"trigerr[ntrig]/F");
  m_tree->Branch("trigx",&trigx,"trigx[ntrig]/F");
  m_tree->Branch("trigy",&trigy,"trigy[ntrig]/F");
  m_tree->Branch("trigz",&trigz,"trigz[ntrig]/F");
  m_tree->Branch("triggx",&triggx,"triggx[ntrig]/F");
  m_tree->Branch("triggy",&triggy,"triggy[ntrig]/F");
  m_tree->Branch("triggz",&triggz,"triggz[ntrig]/F");
  m_tree->Branch("trigid",&trigid,"trigud[ntrig]/I");
  m_tree->Branch("trigpatid",&trigpatid,"trigsegid[ntrig]/I");
  m_tree->Branch("trigsegid",&trigsegid,"trigsegid[ntrig]/I");

  m_tree->Branch("nseg",&nseg,"nseg/I");
  m_tree->Branch("segid",&segid,"segid[nseg]/I");
  m_tree->Branch("seggeo",&seggeo,"seggeo[nseg]/I");
  m_tree->Branch("seggeotd",&seggeotd,"seggeotd[nseg]/F");
  m_tree->Branch("seggeots",&seggeots,"seggeots[nseg]/F");
  m_tree->Branch("seggeold",&seggeold,"seggeold[nseg]/F");
  m_tree->Branch("seggeoft0y",&seggeoft0y,"seggeoft0y[nseg]/F");
  m_tree->Branch("seggeoft0z",&seggeoft0z,"seggeoft0z[nseg]/F");
  m_tree->Branch("seggeoft1y",&seggeoft1y,"seggeoft1y[nseg]/F");
  m_tree->Branch("seggeoft1z",&seggeoft1z,"seggeoft1z[nseg]/F");
  m_tree->Branch("segid1",&segid1,"segid1[nseg]/I");
  m_tree->Branch("seggeo1",&seggeo1,"seggeo1[nseg]/I");
  m_tree->Branch("seggeotd1",&seggeotd1,"seggeotd1[nseg]/F");
  m_tree->Branch("seggeots1",&seggeots1,"seggeots1[nseg]/F");
  m_tree->Branch("seggeold1",&seggeold1,"seggeold1[nseg]/F");
  m_tree->Branch("seggeoft0y1",&seggeoft0y1,"seggeoft0y1[nseg]/F");
  m_tree->Branch("seggeoft0z1",&seggeoft0z1,"seggeoft0z1[nseg]/F");
  m_tree->Branch("seggeoft1y1",&seggeoft1y1,"seggeoft1y1[nseg]/F");
  m_tree->Branch("seggeoft1z1",&seggeoft1z1,"seggeoft1z1[nseg]/F");
  m_tree->Branch("segx",&segx,"segx[nseg]/F");
  m_tree->Branch("segy",&segy,"segy[nseg]/F");
  m_tree->Branch("segz",&segz,"segz[nseg]/F");
  m_tree->Branch("seggx",&seggx,"seggx[nseg]/F");
  m_tree->Branch("seggy",&seggy,"seggy[nseg]/F");
  m_tree->Branch("seggz",&seggz,"seggz[nseg]/F");
  m_tree->Branch("segdx",&segdx,"segdx[nseg]/F");
  m_tree->Branch("segdy",&segdy,"segdy[nseg]/F");
  m_tree->Branch("segdz",&segdz,"segdz[nseg]/F");
  m_tree->Branch("segdgx",&segdgx,"segdgx[nseg]/F");
  m_tree->Branch("segdgy",&segdgy,"segdgy[nseg]/F");
  m_tree->Branch("segdgz",&segdgz,"segdgz[nseg]/F");
  m_tree->Branch("segpatid",&segpatid,"segpatid[nseg]/I");

  m_tree->Branch("npat",&npat,"npat/I");
  m_tree->Branch("patgx",&patgx,"patgx[npat]/F");
  m_tree->Branch("patgy",&patgy,"patgy[npat]/F");
  m_tree->Branch("patgz",&patgz,"patgz[npat]/F");
  m_tree->Branch("patdgx",&patdgx,"patdgx[npat]/F");
  m_tree->Branch("patdgy",&patdgy,"patdgy[npat]/F");
  m_tree->Branch("patdgz",&patdgz,"patdgz[npat]/F");
  nmdt = 0;
  ntrig = 0;
  nseg = 0;
  npat = 0;
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
  nmdt = 0;
  ntrig = 0;
  nseg = 0;
  npat = 0;
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

  if( npat < 10 ){
     patgx[npat] = pgpos.x();
     patgy[npat] = pgpos.y();
     patgz[npat] = pgpos.z();
     patdgx[npat] = pgdir.x();
     patdgy[npat] = pgdir.y();
     patdgz[npat] = pgdir.z();    
     ++npat;
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

      
      if( nseg < 50 ){
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
	    const MuonGM::MdtReadoutElement* detEl2 = 0;
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
	segid[nseg] = seg_id;
	seggeo[nseg] = seg_geo;
	seggeotd[nseg] = tubeDist;
	seggeots[nseg] = tubeStage;
	seggeold[nseg] = layDist;
//  	std::cout << " - tubeDist " << tubeDist
// 		  << " tubeStage " << tubeStage 
// 		  << " ldist " << layDist << std::endl;
	
	seggeoft0y[nseg] = firstTubeMl0.y();
	seggeoft0z[nseg] = firstTubeMl0.z();
	seggeoft1y[nseg] = firstTubeMl1.y();
	seggeoft1z[nseg] = firstTubeMl1.z();
	
	//  ----------------- check if hits from more than one chamber are present
	segid1[nseg] = -1;
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
		const MuonGM::MdtReadoutElement* detEl2 = 0;
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
	    segid1[nseg] = seg_id;
	    seggeo1[nseg] = seg_geo;
	    seggeotd1[nseg] = tubeDist;
	    seggeots1[nseg] = tubeStage;
	    seggeold1[nseg] = layDist;
// 	    std::cout << " - tubeDist " << tubeDist
// 		      << " tubeStage " << tubeStage 
// 		      << " ldist " << layDist << std::endl;
	    
	    seggeoft0y1[nseg] = firstTubeMl0.y();
	    seggeoft0z1[nseg] = firstTubeMl0.z();
	    seggeoft1y1[nseg] = firstTubeMl1.y();
	    seggeoft1z1[nseg] = firstTubeMl1.z();
	  }
	}

	// --------


	segx[nseg] = plpos.x();
	segy[nseg] = plpos.y();
	segz[nseg] = plpos.z();
	segdx[nseg] = pldir.x();
	segdy[nseg] = pldir.y();
	segdz[nseg] = pldir.z();
	seggx[nseg] = pgpos.x();
	seggy[nseg] = pgpos.y();
	seggz[nseg] = pgpos.z();
	segdgx[nseg] = pgdir.x();
	segdgy[nseg] = pgdir.y();
	segdgz[nseg] = pgdir.z();
	segpatid[nseg] = npat-1;
	++nseg;
      }else{
// 	std::cout << " to many segments " << std::endl;
      }
    
      Muon::MuonPatternSegmentMaker::ClusterIt cit = rotRegion.clusters.begin();
      Muon::MuonPatternSegmentMaker::ClusterIt cit_end = rotRegion.clusters.end();
      for( ; cit!=cit_end;++cit ){
	
	if( ntrig < 500 ){
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
	  trigerr[ntrig] = std::sqrt((*cit)->localCovariance()(0,0));
	  trigx[ntrig] = lpos.x();
	  trigy[ntrig] = lpos.y();
	  trigz[ntrig] = lpos.z();
	  triggx[ntrig] = gpos.x();
	  triggy[ntrig] = gpos.y();
	  triggz[ntrig] = gpos.z();
	  trigid[ntrig] = clid;
	  trigpatid[ntrig] = npat - 1;
	  trigsegid[ntrig] = nseg - 1;
	  ++ntrig;
	}else{
// 	  std::cout << " Too many trigger hits " << std::endl;
	}
      }

      Muon::MuonPatternSegmentMaker::MdtIt mmit = mvec.begin();
      Muon::MuonPatternSegmentMaker::MdtIt mmit_end = mvec.end();
      for( ; mmit!=mmit_end;++mmit){
	
	if( nmdt < 500 ){
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
	  mdtt[nmdt] = 0.;
	  mdtr[nmdt] = (*mmit)->localParameters()[Trk::locR];
	  mdtdr[nmdt] = sqrt((*mmit)->localCovariance()(Trk::locR,Trk::locR))*errorScaleFactor;
	  mdtx[nmdt] = lpos.x();
	  mdty[nmdt] = lpos.y();
	  mdtz[nmdt] = lpos.z();
	  mdtgx[nmdt] = gpos.x();
	  mdtgy[nmdt] = gpos.y();
	  mdtgz[nmdt] = gpos.z();
	  mdtid[nmdt] = mdt_id;
	  mdtpatid[nmdt] = npat -1;
	  mdtsegid[nmdt] = nseg -1;
	  ++nmdt;
	}else{
// 	  std::cout << " to many mdt hits " << std::endl;
	}
      }
    }

  }

}
