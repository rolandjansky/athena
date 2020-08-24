/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : MdtVsTgcRawDataValAlg
// Authors:  A. Ishikawa(Kobe), M.King(Kobe)
// Jun. 2008`
//
// DESCRIPTION:
// Subject: correlation btw MDT hits vs TGC RoI -->Offline Muon Data Quality
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MdtVsTgcRawDataMonitoring/MdtVsTgcRawDataValAlg.h"

#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/TgcRdoIdHash.h"
#include "MuonReadoutGeometry/TgcReadoutParams.h"
#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "TrkSegment/SegmentCollection.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include <inttypes.h>
#include <sstream>
#include <algorithm>
#include <fstream>

/////////////////////////////////////////////////////////////////////////////

MdtVsTgcRawDataValAlg::MdtVsTgcRawDataValAlg( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent )
{
  // Declare the properties 
  declareProperty("CheckCabling",     m_checkCabling=false);
  declareProperty("TgcLv1File",       m_tgclv1file=true);    
  declareProperty("ChamberName",      m_chamberName="XXX");
  declareProperty("StationSize",      m_StationSize="XXX");
  declareProperty("Sector",           m_sector=0); 
  declareProperty("Side",             m_side=0); 
  declareProperty("LastEvent",        m_lastEvent=0);
  declareProperty("CosmicStation",    m_cosmicStation=0);
  declareProperty("MdtAdcCut",        m_MdtAdcCut=50);
  declareProperty("MdtTdcCut",        m_MdtTdcCut=1600);
  
  // initialize class members
   
  for(int ac=0; ac<2; ac++){
	m_mvt_cutspassed[ac] = 0;
	for(int jMDT=0; jMDT<4; jMDT++){
		m_mdt_segmmap[ac][jMDT] = 0;
		for(int sMDT=0; sMDT<4; sMDT++){
			m_mdt_segmposdirsag[ac][jMDT][sMDT] = 0;
			for(int iREPT=0; iREPT<4; iREPT++){
				m_mdt_segmmatchsag[ac][jMDT][sMDT][iREPT] = 0;
				m_mdt_trackdirdirsag[ac][jMDT][sMDT][iREPT] = 0;
				m_mdt_trackchecksag[ac][jMDT][sMDT][iREPT][0] = 0;
				m_mdt_trackchecksag[ac][jMDT][sMDT][iREPT][1] = 0;
			}
		}
	}
	for(int WS=0; WS<2; WS++){
		for(int EffNDE=0; EffNDE<4; EffNDE++){
			m_eff_stationmapbase[ac][WS][EffNDE] = 0;
			m_eff_stationmapmid[ac][WS][EffNDE] = 0;
			m_eff_stationmap[ac][WS][EffNDE] = 0;
		}
	}
  }

  // Initialize to zero
  for(int i=0;i<2;i++)// AC
    for(int jTGC=0;jTGC<4;jTGC++)// TGC Station
      for(int f=0;f<2;f++)// FE
        for(int k=0;k<2;k++)// WireStrip
          for(int x=0;x<4;x++){
            m_mvt_extrprdsag[i][jTGC][f][k][x]=0;
            m_mvt_extrprdsag2[i][jTGC][f][k][x]=0;
          }
  for(int k=0;k<2;k++)
    for(int i=0;i<2;i++)
      for(int x=0;x<4;x++){
        m_tgc_prdcompsag[i][k][x]=0;
      }

}

MdtVsTgcRawDataValAlg::~MdtVsTgcRawDataValAlg(){
  ATH_MSG_INFO( " deleting MdtVsTgcRawDataValAlg "  );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
MdtVsTgcRawDataValAlg::initialize(){
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  ATH_MSG_INFO( "in initializing MdtVsTgcRawDataValAlg"  );
  // MuonDetectorManager from the conditions store
  ATH_CHECK(m_DetectorManagerKey.initialize());
  ATH_CHECK(m_idHelperSvc.retrieve());
 
  //MDT z position
  //Name MultiLayer TubeLayer z
  //17 1 1 14142.5
  //17 1 2 14168.5
  //17 1 3 14194.5
  //17 2 1 14394.6
  //17 2 2 14420.6
  //17 2 3 14446.6
  //18 1 1 13726.5
  //18 1 2 13752.5
  //18 1 3 13778.5
  //18 2 1 13978.6
  //18 2 2 14004.6
  //18 2 3 14030.6

  // Retrieve the MuonDetectorManager
  const MuonGM::MuonDetectorManager* MuonDetMgrDS=nullptr;
  ATH_CHECK( detStore()->retrieve(MuonDetMgrDS) );
  ATH_MSG_DEBUG( " Found the MuonDetectorManager from detector store. "  );
  prepareTREarray(MuonDetMgrDS);
  ATH_CHECK(m_tgc_PrepDataContainerName.initialize());
  ATH_CHECK(m_tgc_CoinContainerName.initialize());
  ATH_CHECK(m_mdt_PrepDataContainerName.initialize());
  ATH_CHECK(m_mdt_SegmentCollectionName.initialize());
  return StatusCode::SUCCESS;
}


/*----------------------------------------------------------------------------------*/
StatusCode MdtVsTgcRawDataValAlg::bookHistogramsRecurrent(){
/*----------------------------------------------------------------------------------*/
  ATH_MSG_DEBUG( "TGC RawData Monitoring Histograms being booked"  );
  
  //declare a group of histograms
  std::string generic_path_mdtvstgclv1 = "Muon/MuonRawDataMonitoring/MDTvsTGC";
  //MonGroup mdtvstgclv1_expert( this, generic_path_mdtvstgclv1+"/Global", expert, run );
  MonGroup mdtvstgclv1_expert_a( this, generic_path_mdtvstgclv1+"/TGCEA", run, ATTRIB_UNMANAGED );
  MonGroup mdtvstgclv1_expert_c( this, generic_path_mdtvstgclv1+"/TGCEC", run, ATTRIB_UNMANAGED );
  
  if(newRunFlag()){
    ATH_MSG_INFO( "MdtVsTgc RawData Monitoring : begin of run"  );
    
    ATH_CHECK( bookmaphists(mdtvstgclv1_expert_a, mdtvstgclv1_expert_c) );
    ATH_CHECK( bookeffhists(mdtvstgclv1_expert_a, mdtvstgclv1_expert_c) );
  }//isNewRun
  return StatusCode::SUCCESS;
}


/*----------------------------------------------------------------------------------*/
StatusCode MdtVsTgcRawDataValAlg::fillHistograms(){
/*----------------------------------------------------------------------------------*/
  ATH_MSG_DEBUG( "MdtVsTgcRawDataValAlg::TGC RawData Monitoring Histograms being filled"  );

  //TGC PRD
  SG::ReadHandle<Muon::TgcPrepDataContainer> tgc_prd_container(m_tgc_PrepDataContainerName);
  
  //TGC Coincidence
  SG::ReadHandle<Muon::TgcCoinDataContainer> tgc_coin_container(m_tgc_CoinContainerName);

  ATH_MSG_DEBUG( "size of tgc container is " << tgc_coin_container -> size()  );
  
  //MDT PRD
  SG::ReadHandle<Muon::MdtPrepDataContainer> mdt_prd_container(m_mdt_PrepDataContainerName);

  SG::ReadHandle<xAOD::MuonSegmentContainer> mdt_segment_collection(m_mdt_SegmentCollectionName) ;
  tgceffcalc(mdt_segment_collection.cptr(), tgc_prd_container.cptr());
  maphists(mdt_segment_collection.cptr(), tgc_prd_container.cptr());

  //only analyze nSL==1
  int nSL = numberOfSL(tgc_coin_container.cptr());

  if(nSL==1){
    //fill MDT hit vs TGC RoI
    correlation(mdt_prd_container.cptr(), tgc_coin_container.cptr());
  }

 
  return StatusCode::SUCCESS;  // statuscode check
} 
/*----------------------------------------------------------------------------------*/
StatusCode MdtVsTgcRawDataValAlg::procHistograms(){
/*----------------------------------------------------------------------------------*/
 
  ATH_MSG_DEBUG( "MdtVsTgcRawDataValAlg finalize()"  );
  if(endOfRunFlag()){
    tgceffcalcfinalize();
    maphistsfinalize();
  }
  return StatusCode::SUCCESS;
}
