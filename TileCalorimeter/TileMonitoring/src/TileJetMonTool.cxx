/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileJetMonTool.cxx
// PACKAGE:  TileMonitoring 
//
// AUTHOR:   Tomas Davidek (Tomas.Davidek@cern.ch)
//           
//
// Changes 07.07.2015
// - by default, only histograms with channel time and cell time per partition
//   are created and filled. 2D histograms per channel, 1D histograms per 
//   channel and energy difference histograms are optionally created
//   (see switches do_1dim_histos, do_2dim_histos, do_enediff_histos)
// - improvement in detecting the special channels (C10 special, E1 merged, E4')
//   implemented as suggested by Sasha
// - avoid duplicate entries when a cell/channel is part of more clusters
//   (such a cell appears multiple times in the list of cells associated to
//   the given jet)
// ********************************************************************

#include "xAODJet/JetContainer.h"
#include <xAODCore/ShallowCopy.h>
#include "JetUtils/JetCaloQualityUtils.h"
#include "JetUtils/JetCellAccessor.h"

#include "CaloGeoHelpers/CaloSampling.h" 
#include "CaloIdentifier/TileID.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloSamplingHelper.h"

#include "TileMonitoring/TileJetMonTool.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileEvent/TileCell.h"
#include "TileEvent/TileContainer.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
// added by TD
#include "TileIdentifier/TileFragHash.h"
#include "xAODEventInfo/EventInfo.h"

#include "JetMomentTools/JetVertexTaggerTool.h"

//#include "CaloIdentifier/Tile_Base_ID.h"
//#include "TH2F.h"
//#include "TH1F.h"
//#include "TProfile.h"

#include <iostream>
#include <string>

using xAOD::EventInfo;

/*---------------------------------------------------------*/
TileJetMonTool::TileJetMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_tileBadChanTool("TileBadChanTool")
  , m_jvt("JVT")
  , m_cleaningTool("MyCleaningTool")
  , m_ECTool("ECTool")
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("jetPtMin", m_jetPtMin = 20000.); //Threshold in MeV
  declareProperty("jetEtaMax", m_jetEtaMax = 1.6);
  declareProperty("jetCollectionName", m_jetCollectionName = "AntiKt4EMTopoJets");
  declareProperty("energyChanMin", m_energyChanMin = 2000);
  declareProperty("energyChanMax", m_energyChanMax = 4000);
  //  declareProperty("averageMinEvt", m_averageMinEvt = 10);
  declareProperty("do_1dim_histos", m_do_1dim_histos = false);
  declareProperty("do_2dim_histos", m_do_2dim_histos = false);
  declareProperty("do_enediff_histos", m_do_enediff_histos = false);
  declareProperty("enediff_threshold", m_enediff_threshold = 2000);
  declareProperty("do_energy_profiles", m_do_energy_profiles = true);
  declareProperty("do_event_cleaning",m_do_event_cleaning = true);
  declareProperty("do_jet_cleaning",m_do_jet_cleaning = true);
  declareProperty("useJVTTool",m_jvt);
  declareProperty("useJetCleaning",m_cleaningTool);
  declareProperty("useEventCleaning",m_ECTool);
  declareProperty("JvtDecorator",m_JvtDecorator = "passJvt");
  declareProperty("OrDecorator",m_OrDecorator = "passOR");
  declareProperty("jet_tracking_eta_limit",m_jet_tracking_eta_limit = 2.4);
  declareProperty("jet_JVT_threshold",m_jet_jvt_threshold = 0.59);
  declareProperty("jet_JVT_pTmax",m_jet_jvt_ptmax = 120000);
  m_path = "/Tile/Jet";

  m_partname[0] = "LBA";
  m_partname[1] = "LBC";
  m_partname[2] = "EBA";
  m_partname[3] = "EBC";

  m_first_event = true;
}
/*---------------------------------------------------------*/


/*---------------------------------------------------------*/
TileJetMonTool::~TileJetMonTool() {
/*---------------------------------------------------------*/

}


/*---------------------------------------------------------*/
StatusCode TileJetMonTool::initialize() {
/*---------------------------------------------------------*/

  ATH_MSG_INFO("in initialize()");
  ATH_MSG_INFO("value of m_do_jet_cleaning: " << m_do_jet_cleaning);

//=== get TileBadChanTool
  ATH_MSG_DEBUG("TileJetMonTool: Retrieving tile bad channel tool");
  CHECK(m_tileBadChanTool.retrieve());
  ATH_MSG_DEBUG("TileJetMonTool: Retrieved tile bad channel tool");

  if (m_do_jet_cleaning) {
    ATH_MSG_DEBUG("TileJetMonTool: initializing JVT updater");
    CHECK(m_jvt.retrieve());
    ATH_MSG_DEBUG("TileJetMonTool: initialized JVT updater");
    
    ATH_MSG_DEBUG("TileJetMonTool: initializing JetCleaningTool");
    CHECK(m_cleaningTool.retrieve());
    CHECK(m_ECTool.retrieve());
    ATH_MSG_DEBUG("TileJetMonTool: initialized JetCleaningTool");
  } else {
    m_jvt.disable();
    m_cleaningTool.disable();
    m_ECTool.disable();
  }

  return TileFatherMonTool::initialize();
}


/*---------------------------------------------------------*/
StatusCode TileJetMonTool::bookHistograms() {
/*---------------------------------------------------------*/

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileJetMonTool::fillHistograms() {
/*---------------------------------------------------------*/

  //  const JetCollection* jetcol;
  if (! isGoodEvent()) {
    ATH_MSG_DEBUG("TileJetMonTool::fillHistograms(), event skipped ");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("TileJetMonTool::fillHistograms(), event accepted ");
  }

  const xAOD::JetContainer* jetContainer = evtStore()->tryConstRetrieve<xAOD::JetContainer>(m_jetCollectionName);
  //  CHECK(evtStore()->retrieve(jetContainer, m_jetCollectionName));
  if (!jetContainer) return StatusCode::SUCCESS;

  fillEvtInfo();
  if (m_first_event) {
    m_first_event = false;
    if (bookTimeHistograms() != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("Something wrong when booking histograms.");
    }
  }	
  uint32_t LumiBlock = getLumiBlock();

  ATH_MSG_VERBOSE("TileJetMonTool::fillHistograms(), lumiblock " << LumiBlock);

  m_usedCells.clear();

  int ijet = 0;
  for (const xAOD::Jet* jet : *jetContainer) {
    if ((jet->pt() > m_jetPtMin) && (fabs(jet->eta()) < m_jetEtaMax)) {
      if (isGoodJet(*jet)) {
        ATH_MSG_DEBUG("TileJetMonTool::fillHistograms, jet " << ijet
			                << ", eta " << jet->eta()
			                << ", phi " << jet->phi()
			                << ", constituents " << jet->numConstituents());
        CHECK(fillTimeHistograms(*jet, LumiBlock));
      } else {
        ATH_MSG_DEBUG("TileJetMonTool::fillHistogram, BAD jet " << ijet
                      << ", eta " << jet->eta()
                      << ", phi " << jet->phi()
                      << ", constituents " << jet->numConstituents());
      }
    }
    ijet++;
  }

  ATH_MSG_VERBOSE("TileJetMonTool::fillHistograms(), end-of-loops ");
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileJetMonTool::procHistograms() {
/*---------------------------------------------------------*/

  float sum_channel = 0;
  for (const TH1* channelTimeHistogram : m_TilePartTime) {
    if (channelTimeHistogram->GetEntries() != 0) {
      sum_channel += channelTimeHistogram->GetEntries();
    }
  }

  float sum_cell_hg = 0;
  for (int p = 0; p < NPART; ++p) {
    for (const TH1* cellTimeHistogram : m_TilePartCellTimeHG[p]) {
      if (cellTimeHistogram->GetEntries() != 0) {
        sum_cell_hg += cellTimeHistogram->GetEntries();
      }
    }
  }
  float sum_cell_lg = 0;
  for (int p = 0; p < NPART; ++p) {
    for (const TH1* cellTimeHistogram : m_TilePartCellTimeLG[p]) {
      if (cellTimeHistogram->GetEntries() != 0) {
        sum_cell_lg += cellTimeHistogram->GetEntries();
      }
    }
  }
  ATH_MSG_INFO( "Total number of entries in histograms: channels "  << sum_channel
                << ", cells HG " << sum_cell_hg
                << ", cells LG " << sum_cell_lg);
	  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileJetMonTool::bookTimeHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in TileJetMonTool::bookTimeHistograms()" );

  // Input for axis labels as in TileCellMonTool.cxx
  std::vector<std::string> cellchLabel[NPART];
  std::vector<std::string> moduleLabel[NPART];
  for (int part = 0; part < NPART; ++part) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      if (drawer % 2 == 1) {
        moduleLabel[part].push_back(" ");
      } else {
        moduleLabel[part].push_back(TileCalibUtils::getDrawerString(part + 1, drawer));
      }
    }

    for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
      std::string cell_name = getCellName(part + 1, channel);
      if (cell_name.empty()) cell_name += "ch";
      else cell_name += "_ch";
      cellchLabel[part].push_back(cell_name + std::to_string(channel));
    }

  }


  // book histograms for all channels, even if some of them remain empty 
  // in the end
  std::string runNumStr = getRunNumStr();
  std::string htitle;
  std::string moduleName;
  char chnum[3]; // must be of length 3, since the last char is supposed to be '\0'
  char indexnum[3]; // must be of length 3, since the last char is supposed to be '\0'
  // energy upper limits of the cell-time histograms
  m_cell_ene_hg_up.push_back(500);
  m_cell_ene_hg_up.push_back(1000);
  m_cell_ene_hg_up.push_back(2000);
  m_cell_ene_hg_up.push_back(4000);
  m_cell_ene_hg_up.push_back(6000);
  m_cell_ene_hg_up.push_back(8000);
  m_cell_ene_hg_up.push_back(10000);
  m_cell_ene_hg_up.push_back(13000);
  m_cell_ene_hg_up.push_back(16000);
  m_cell_ene_hg_up.push_back(20000);

  m_cell_ene_lg_up.push_back(25000);
  m_cell_ene_lg_up.push_back(30000);
  m_cell_ene_lg_up.push_back(40000);
  m_cell_ene_lg_up.push_back(50000);
  m_cell_ene_lg_up.push_back(65000);
  m_cell_ene_lg_up.push_back(80000);

  /*
  float *ene_hg = (float*)malloc((m_cell_ene_hg_up.size()+2)*sizeof(float));
  ene_hg[0] = -1000;
  for (unsigned int i = 0; i <= m_cell_ene_hg_up.size(); ++i) {
    ene_hg[i+1] = m_cell_ene_hg_up[i];
  }
  ene_hg[m_cell_ene_hg_up.size()+1] = 2*m_cell_ene_hg_up[m_cell_ene_hg_up.size()-1];
  float *ene_lg = (float*)malloc((m_cell_ene_lg_up.size()+2)*sizeof(float));
  ene_lg[0] = 0;
  for (unsigned int i = 0; i <= m_cell_ene_lg_up.size(); ++i) {
    ene_lg[i+1] = m_cell_ene_lg_up[i];
  }
  ene_lg[m_cell_ene_lg_up.size()+1] = 2*m_cell_ene_lg_up[m_cell_ene_lg_up.size()-1];
  */
  //  TH2F *hist;
  for (int p = 0; p < NPART; ++p) {
    // first, create the total timing histograms for each partition
    // these histograms have fine time binning
    m_TilePartTime.push_back( book1F( "ChanTime/" + m_partname[p],
				      m_partname[p], m_partname[p],
				      600, -30.0, 30.0));
    if ( p >= 2) {
      m_TileEBTime_NoScint.push_back( book1F( "ChanTime/" + m_partname[p],
					      m_partname[p] + "_NoScint", m_partname[p] + "_NoScint",
					      600, -30.0, 30.0));
    }
    // now cell-time. Don't need cells without scintillators in EBA/EBA,
    // since these are single pmt cells
    Float_t xmin, xmax;
    for (unsigned int i = 0; i <= m_cell_ene_hg_up.size(); ++i) {
      sprintf(indexnum, "%02u", i);
      m_TilePartCellTimeHG[p].push_back( book1F("CellTime/" + m_partname[p]
                                        , "Cell_time_" + m_partname[p] + "_hg_slice_" + indexnum
                                        , "Cell_time_" + m_partname[p] + "_hg_slice_" + indexnum, 600, -30.0, 30.0));

      if (! m_do_energy_profiles) {
	//	xmin = ene_hg[i]; xmax = ene_hg[i+1];
	xmin = (i == 0) ? -1000 : m_cell_ene_hg_up[i - 1];
	xmax = (i < m_cell_ene_hg_up.size()) ? m_cell_ene_hg_up[i] : 2 * m_cell_ene_hg_up[m_cell_ene_hg_up.size() - 1];

	m_TilePartCellEneHG[p].push_back( book1F("CellTime/" + m_partname[p]
						 , "Cell_ene_" + m_partname[p] + "_hg_slice_" + indexnum
						 , "Cell_ene_" + m_partname[p] + "_hg_slice_" + indexnum, 100, xmin, xmax));
      }
    }

    for (unsigned int i = 0; i <= m_cell_ene_lg_up.size(); ++i) {
      sprintf(indexnum, "%02u", i);
      m_TilePartCellTimeLG[p].push_back( book1F("CellTime/" + m_partname[p]
                                                , "Cell_time_" + m_partname[p] + "_lg_slice_" + indexnum
                                                , "Cell_time_" + m_partname[p] + "_lg_slice_" + indexnum, 600
                                                , -30.0, 30.0));

      if (! m_do_energy_profiles) {
	//	xmin = ene_lg[i]; xmax = ene_lg[i+1];
	xmin = (i == 0) ? 0 : m_cell_ene_lg_up[i - 1];
	xmax = (i < m_cell_ene_lg_up.size()) ? m_cell_ene_lg_up[i] : 2 * m_cell_ene_lg_up[m_cell_ene_lg_up.size() - 1];
	m_TilePartCellEneLG[p].push_back( book1F("CellTime/" + m_partname[p]
						 , "Cell_ene_" + m_partname[p] + "_lg_slice_" + indexnum
						 , "Cell_ene_" + m_partname[p] + "_lg_slice_" + indexnum, 100, xmin, xmax));
      }
    }
    
    if (m_do_energy_profiles) {
      m_TilePartCellEneHGProf.push_back( bookProfile("CellTime/" + m_partname[p],
						     "Cell_ene_" + m_partname[p] + "_hg_prof",
						     "Cell_ene_" + m_partname[p] + "_hg_prof",
						     m_cell_ene_hg_up.size()+1,
						     -0.5,m_cell_ene_hg_up.size()+0.5));
      m_TilePartCellEneLGProf.push_back( bookProfile("CellTime/" + m_partname[p],
						     "Cell_ene_" + m_partname[p] + "_lg_prof",
						     "Cell_ene_" + m_partname[p] + "_lg_prof",
						     m_cell_ene_lg_up.size()+1,
						     -0.5,m_cell_ene_lg_up.size()+0.5));
    }      

    for (unsigned int m = 0; m < TileCalibUtils::MAX_DRAWER; ++m) {
      moduleName = TileCalibUtils::getDrawerString(p + 1, m);
      
      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
	
        sprintf(chnum, "%02u", channel);
	if (m_do_2dim_histos) {
	  htitle = "Time in " + moduleName + "_ch_" + chnum + " vs lumiblock";
	  /* TH2F histograms are too much in memory. Trying to book only 10 LB
	     and set rebin-bit */
	  m_TileChanTime[p].push_back( book2F( "ChanTime/" + m_partname[p],
					       moduleName + "_ch_" + chnum + "_2d", 
					       htitle,
					       10, 0, 10, 120, -30.0, 30.0));
	  //	m_TileChanTime[p][m*TileCalibUtils::MAX_CHAN+ch]->SetBit(TH1::kCanRebin);
	  /*
	    m_TileChanTime[p].push_back(bookProfile( m_partname[p],
	    moduleName+"_ch_"+chnum,
	    moduleName+"_ch_"+chnum,
	    1500,0.5,1500.5,-30.0,30.0));
	  */
	}
	if (m_do_1dim_histos) {
	  htitle = "Time in " + moduleName + "_ch_" + chnum;
	  m_TileChanTime1D[p].push_back( book1F( "ChanTime/" + m_partname[p],
						 moduleName + "_ch_" + chnum + "_1d",
						 htitle,
						 600, -30.0, 30.0));
	}
	if ((m_do_enediff_histos) && (channel%2 == 0)) {
	  // each cell has one odd and one even channel
	  m_TileEneDiff_LG[p].push_back( book1F( "Enediff/" + m_partname[p],
						 moduleName + "_enediff_LG_ch1_" + chnum,
						 moduleName + "_enediff_LG_ch1_" + chnum,
						 
						 100,-1,1));
	  m_TileEneDiff_HG[p].push_back( book1F( "Enediff/" + m_partname[p],
						 moduleName + "_enediff_HG_ch1_" + chnum,
						 moduleName + "_enediff_HG_ch1_" + chnum,
						 
						 100,-1,1));
	}
      }
    } // end-of-loop over modules
    // average time DQ "temperature" plots, per partition
    m_TilePartTimeDQ.push_back( bookProfile2D( "DQ/",
					       "tileJetChanTime"+m_partname[p], 
					       "Run " + runNumStr + " Partition " +
					       m_partname[p] + ": Average time with jets",
					       64, 0.5, 64.5, 48, -0.5, 47.5,
					       -80,80));
    SetBinLabel(m_TilePartTimeDQ[p]->GetXaxis(), moduleLabel[p]);
    SetBinLabel(m_TilePartTimeDQ[p]->GetYaxis(), cellchLabel[p]);
    m_TilePartTimeDQ[p]->SetZTitle("Average Channel Time (ns)");
    m_TilePartTimeDQ[p]->SetOption("COLZ");    
  } // end-of-loop over partitions
  // delete ene_hg;
  //  delete ene_lg;
  ATH_MSG_INFO( "All histograms booked " );

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/  

void TileJetMonTool::clearTimeHistograms() {
/*---------------------------------------------------------*/
  m_TilePartTime.clear();
  m_TilePartTimeDQ.clear();
  m_TileEBTime_NoScint.clear();
  if (m_do_energy_profiles) {
    m_TilePartCellEneHGProf.clear();
    m_TilePartCellEneLGProf.clear();
  }

  for (int p = 0; p < NPART; ++p) {
    m_TilePartCellTimeHG[p].clear();
    m_TilePartCellTimeLG[p].clear();
    if (! m_do_energy_profiles) {
      m_TilePartCellEneHG[p].clear();
      m_TilePartCellEneLG[p].clear();
    }
    if (m_do_2dim_histos) {
      m_TileChanTime[p].clear();
    }
    if (m_do_1dim_histos) {
      m_TileChanTime1D[p].clear();
    }
    if (m_do_enediff_histos) {
      m_TileEneDiff_LG[p].clear();
      m_TileEneDiff_HG[p].clear();
    }
  }
}

/*---------------------------------------------------------*/  
StatusCode TileJetMonTool::fillTimeHistograms(const xAOD::Jet& jet, uint32_t LumiBlock) {
/*---------------------------------------------------------*/  


  ATH_MSG_VERBOSE( "in fillTimeHistograms()" );

  if( jet.numConstituents() == 0) return StatusCode::SUCCESS;
  /*
  // My original idea was to loop over clusters, then over cells, 
  // and for each cell use the calo cluster weight.
  // First, I can't get it compiled
  // Second, Sasha thinks it is easier to keep the list of cells
  // already used and compare. Using std::set instead of std::vector
  // since std:set::find() should be faster than std::vector::find().
  xAOD::JetConstituentVector cluster_vec = jet.getConstituents();
  xAOD::JetConstituentVector::const_iterator clusterIt = cluster_vec.begin();
  xAOD::JetConstituentVector::const_iterator clusterItE = cluster_vec.end();

  for( ; clusterIt != clusterItE; clusterIt++) {
    const CaloCluster *cluster = (*clusterIt);
    const CaloCluster::cell_iterator cellIt = cluster->cell_begin();
    const CaloCluster::cell_iterator cellItE = cluster->cell_end();
    for( ; cellIt != cellItE; ++cellIt) {
      const CaloCell* cell = (*cellIt);
      double weight = cluster->getCellWeight(cell);
    }
  }
  */
  /*
  int n_cells_1(0);
  int n_cells_2(0);

  for (const xAOD::JetConstituent* jet_constituent : jet.getConstituents()) {
    if( jet_constituent->type() == xAOD::Type::CaloCluster ){
      const xAOD::CaloCluster* calo_cluster = static_cast<const xAOD::CaloCluster*>(jet_constituent->rawConstituent());
      if (calo_cluster->getCellLinks()) {
        for (const CaloCell* cell : *calo_cluster) {
          ++n_cells_1;
        }
      }
    }
  }


  */


  //  jet::JetCellAccessor::const_iterator cellIt =  jet::JetCellAccessor::begin(&jet);
  //  jet::JetCellAccessor::const_iterator cellItE = jet::JetCellAccessor::end(&jet);
     
  int c_index = -1;
  //  for( ;cellIt != cellItE; cellIt++) {

  for (const xAOD::JetConstituent* jet_constituent : jet.getConstituents()) {
    if( jet_constituent->type() == xAOD::Type::CaloCluster ){
      const xAOD::CaloCluster* calo_cluster = static_cast<const xAOD::CaloCluster*>(jet_constituent->rawConstituent());
      if (calo_cluster->getCellLinks()) {
        for (const CaloCell* cell : *calo_cluster) {
          c_index++;
          if (cell->caloDDE()->getSubCalo() == CaloCell_ID::TILE) { // a Tile Cell
            ATH_MSG_DEBUG("Cell " << c_index << " IS TILECAL !!");
            const TileCell *tilecell = (TileCell*) cell;
            Identifier id = tilecell->ID();
            if (m_usedCells.find(id) == m_usedCells.end()) {
              m_usedCells.insert(id);
            } else {
              continue;
            }
            //      int section= m_tileID->section(id);
            int module = m_tileID->module(id); // ranges 0..63
            int sample = m_tileID->sample(id); // ranges 0..3 (A, BC, D, E)
            int part1 = 0;
            int part2 = 0;
            int chan1 = -1;
            int chan2 = -1;
            uint32_t bad1 = 0;
            uint32_t bad2 = 0;
            int gain1 = tilecell->gain1();
            int gain2 = tilecell->gain2();
            unsigned int qbit1 = tilecell->qbit1();
            unsigned int qbit2 = tilecell->qbit2();
            
            const CaloDetDescrElement * caloDDE = tilecell->caloDDE();
            IdentifierHash hash1 = caloDDE->onl1();
            if (hash1 != TileHWID::NOT_VALID_HASH) {
              HWIdentifier adc_id = m_tileHWID->adc_id(hash1, gain1);
              part1 = m_tileHWID->ros(adc_id);
              chan1 = m_tileHWID->channel(adc_id);
              bad1 = m_tileBadChanTool->encodeStatus(m_tileBadChanTool->getAdcStatus(adc_id));
            }
            
            // How is it here with partition? D0 spans two partitions....
            // It should be ok to treat it in this way:
            IdentifierHash hash2 = caloDDE->onl2();
            if (hash2 != TileHWID::NOT_VALID_HASH) {
              HWIdentifier adc_id = m_tileHWID->adc_id(hash2, gain2);
              part2 = m_tileHWID->ros(adc_id);
              chan2 = m_tileHWID->channel(adc_id);
              bad2 = m_tileBadChanTool->encodeStatus(m_tileBadChanTool->getAdcStatus(adc_id));
            }
            
            //        ATH_MSG_INFO( "This is Tile cell " << icell
            //                      << ": pmts " << pmt1 << "," << pmt2
            //                      << ", ene "  << ene1 << "," << ene2 );
            
            Bool_t is_good1 = isGoodChannel(part1, module, chan1, bad1, qbit1, id);
            Bool_t is_good2 = isGoodChannel(part2, module, chan2, bad2, qbit2, id);
            float ene1 = is_good1 ? tilecell->ene1() : -1;
            float ene2 = is_good2 ? tilecell->ene2() : -1;
            
            ATH_MSG_DEBUG(".... " << TileCalibUtils::getDrawerString(part1, module)
                          << ", ch1 " << chan1
                          << ", ch2 " << chan2
                          << ", qbit " << qbit1 << "/" << qbit2
                          << ", is_bad " << bad1 << "/" << bad2
                          << ", isGood " << is_good1
                          << "/" << is_good2
                          << ", ene " << ene1+ene2);
            /*
              Now really fill the histograms time vs lumiblock and 1dim time
            */
            char sLumiBlock[10];
            sprintf(sLumiBlock, "%d", LumiBlock);
            
            // first channel
            if (is_good1 && (ene1 > m_energyChanMin) && (ene1 < m_energyChanMax)) {
              if (m_do_1dim_histos) {
                m_TileChanTime1D[part1 - 1][TileCalibUtils::MAX_CHAN * module + chan1]->Fill(tilecell->time1(), 1);
              }
              if (m_do_2dim_histos) {
                m_TileChanTime[part1 - 1][TileCalibUtils::MAX_CHAN * module + chan1]->Fill(sLumiBlock,
                    tilecell->time1(), 1);
              }
              // info for DQ histograms
              m_TilePartTimeDQ[part1 - 1]->Fill(module + 1, chan1, tilecell->time1());
              // general histograms, only require non-affected channels
              if (bad1 < 2) {
                m_TilePartTime[part1 - 1]->Fill(tilecell->time1());
                ATH_MSG_DEBUG( "Filling in time1 for " << TileCalibUtils::getDrawerString(part1, module)
                               << ", ch " << chan1
                               << ", ene " << ene1
                               << ", LB " << sLumiBlock 
                               << ", time: " << tilecell->time1());
                if ((part1 >= 3) && (sample < TileID::SAMP_E)) {
                  m_TileEBTime_NoScint[part1 - 3]->Fill(tilecell->time1());
                }
              }
            }
            // second channel
            if (is_good2 && (ene2 > m_energyChanMin) && (ene2 < m_energyChanMax)) {
              if (m_do_1dim_histos) {
                m_TileChanTime1D[part2 - 1][TileCalibUtils::MAX_CHAN * module + chan2]->Fill(tilecell->time2(), 1);
              }
              if (m_do_2dim_histos) {
                m_TileChanTime[part2 - 1][TileCalibUtils::MAX_CHAN * module + chan2]->Fill(sLumiBlock, tilecell->time2(), 1);
              }
              // info for DQ histograms
              m_TilePartTimeDQ[part2 - 1]->Fill(module + 1, chan2, tilecell->time2());
              // general histograms, only require non-affected channels
              if (bad2 < 2) {
                ATH_MSG_DEBUG( "Filling in time2 for " << TileCalibUtils::getDrawerString(part2, module)
                               << ", ch " << chan2
                               << ", ene " << ene2
                               << ", LB " << sLumiBlock 
                               << ", time: " << tilecell->time2()
                               <<" (qbit2 " << qbit2 << ", ch1 " << chan1 << ", ene1 " << ene1 << ", bad1 " << bad1 << ", qbit1 " << qbit1 << ")" );
                m_TilePartTime[part2 - 1]->Fill(tilecell->time2());
                if ((part2 >= 3) && (sample < TileID::SAMP_E)) {
                  m_TileEBTime_NoScint[part2 - 3]->Fill(tilecell->time2());
                }
              }
            }
            /* 
               Now filling the cell-based histograms
            */
            if ((is_good1) && (is_good2)) {
              if (m_do_enediff_histos && (gain1 == gain2) && 
                  (ene1+ene2 > m_enediff_threshold))  { 
                // EneDiff histograms, HG-HG and LG-LG combinations only
                int evenchan = (chan1%2 == 0) ? chan1 : chan2;
                if (gain1 == 0) {
                  m_TileEneDiff_LG[part1-1][(TileCalibUtils::MAX_CHAN * module + evenchan)/2]->Fill((ene1-ene2)/(ene1+ene2));
                } else {
                  m_TileEneDiff_HG[part1-1][(TileCalibUtils::MAX_CHAN * module + evenchan)/2]->Fill((ene1-ene2)/(ene1+ene2));
                }
              }
              if ((bad1 < 2) && (bad2 < 2) && (gain1 == gain2)) {
                // cell-time histograms, only overall, require not affected channels
                if ((part1 <= 2) || (sample != TileID::SAMP_E)) {
                  int index = find_index(gain1,ene1+ene2);
                  ATH_MSG_DEBUG( "Filling in cell-time for " << TileCalibUtils::getDrawerString(part2, module)
                                 << ", ch1 " << chan1  
                                 << ", ch2 " << chan2  
                                 << ", ene " << ene1+ene2
                                 << ", index " << index
                                 << ", time: " << tilecell->time());
                  if (gain1 == 1) {
                    m_TilePartCellTimeHG[part1-1][index]->Fill(tilecell->time());
		    if (m_do_energy_profiles) {
		      m_TilePartCellEneHGProf[part1-1]->Fill((float)index,ene1+ene2);
		    } else {
		      m_TilePartCellEneHG[part1-1][index]->Fill(ene1+ene2);
		    }
                  } else {
                    m_TilePartCellTimeLG[part1-1][index]->Fill(tilecell->time());
		    if (m_do_energy_profiles) {
		      m_TilePartCellEneLGProf[part1-1]->Fill((float)index,ene1+ene2);
		    } else {
		      m_TilePartCellEneLG[part1-1][index]->Fill(ene1+ene2);
		    }
                  }
                }
              }
            }
          } else {
            ATH_MSG_DEBUG("Cell " << c_index << " is NOT Tilecal");
          }
        }
      }
    }
  }

        //  if (n_cells_1 != n_cells_2) ATH_MSG_WARNING("AUUU: " << n_cells_1 << "/" << n_cells_2);

  return StatusCode::SUCCESS;
}

bool TileJetMonTool::isGoodChannel(int part, int mod, int chan, uint32_t bad, unsigned int qbit, Identifier id) {

  if ((part <= 0) || (part > NPART)) return false;// invalid partition

  if ((mod < 0) || (mod >= (int) TileCalibUtils::MAX_DRAWER)) return false;  // invalid module number

  if (m_cabling->channel2hole(part, chan) <= 0) 
    return false; // non-existing PMT (empty hole)

  if (((qbit & TileCell::MASK_BADCH) != 0) || // masked as bad
      ((qbit & TileCell::MASK_TIME) != TileCell::MASK_TIME) ||  // flagged
      ((qbit & TileCell::MASK_ALGO) == TileFragHash::OptFilterDsp)) // in DSP

    /* Sasha navrhuje asi jednodussi:
     mask = TileCell::MASK_BADCH | TileCell:MASK_OVER | TileCell::MASK_TIME
     if ((qbit & mask) != TileCell::MASK_TIME) return false;
     */

    return false;
  /* 
   bad is the status in the DB (see http://alxr.usatlas.bnl.gov/lxr-stb6/source/atlas/TileCalorimeter/TileConditions/src/TileBadChanTool.cxx#390).
   Meaning:
   0 = good, 1 = noisy, 2 = affected, 3 = bad, 4 = otherwise
   */
  if (bad > 2) return false;

  /* 
     Now check for special C10, merged E1, E4'
     C10 spec is ok only if channel = 5 (i.e. pmt=6). The other is pmt=5
     E1 merged and E4' should be dropped if channel = 12 (i.e. pmt=13)
  */
  return ((( chan != 4) && (chan != 12)) || m_cabling->TileGap_connected(id));
}

bool TileJetMonTool::isGoodEvent() {
  /* check for errors in LAr and Tile, see https://twiki.cern.ch/twiki/bin/viewauth/Atlas/DataPreparationCheckListForPhysicsAnalysis
   */
  if (! m_do_event_cleaning) return true;

  ATH_MSG_DEBUG("TileJetMonTool::isGoodEvent()....");
  const EventInfo* eventInfo = nullptr;
  if (! evtStore()->retrieve(eventInfo, "EventInfo").isSuccess()){
    ATH_MSG_ERROR("Cannot retrieve EventInfo object!");
    return false;
  }
  if (eventInfo->errorState(EventInfo::LAr) == EventInfo::Error) return(false);
  if (eventInfo->errorState(EventInfo::Tile) == EventInfo::Error) return(false);

  /* see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/HowToCleanJets2017
  */
  if (! m_do_jet_cleaning) return true;

  const xAOD::JetContainer* jetContainer = nullptr;
  if (! evtStore()->retrieve(jetContainer, "AntiKt4EMTopoJets").isSuccess()){
    ATH_MSG_INFO("Cannot retrieve AntiKt4EMTopoJets. However, returning true.");
    return true;
  }

  auto jetsSC = xAOD::shallowCopyContainer(*jetContainer);
  int ijet=0;
  for (auto jet : *jetsSC.first) {
    ATH_MSG_DEBUG("Jet " << ijet << ", pT " << jet->pt()/1000.0 << " GeV, eta " 
		  << jet->eta());
    jet->auxdata<char>(m_JvtDecorator) = passesJvt(*jet);
    jet->auxdata<char>(m_OrDecorator) = true;
    ATH_MSG_DEBUG("... done with jet " << ijet);
    ijet++;
  }
  bool accept = m_ECTool->acceptEvent(jetsSC.first);
  delete jetsSC.first;
  delete jetsSC.second;
  return accept;
}

bool TileJetMonTool::passesJvt(const xAOD::Jet& jet) {
  if (jet.pt() > m_jetPtMin
      && jet.pt() < m_jet_jvt_ptmax
      && fabs(jet.getAttribute<float>("DetectorEta")) < m_jet_tracking_eta_limit
      && m_jvt->updateJvt(jet) < m_jet_jvt_threshold)
    return false;
  return true;
}

bool TileJetMonTool::isGoodJet(const xAOD::Jet& jet) {
  if (! m_do_jet_cleaning) return true;
  if (jet.pt() < m_jetPtMin) return false;
  if (! passesJvt(jet)) return false;
  if (! m_cleaningTool->keep(jet)) return false;
  return true;
}

unsigned int TileJetMonTool::find_index(const int gain, const float energy) {
  if (gain == 1) {
    for (unsigned int i = 0; i < m_cell_ene_hg_up.size(); ++i) {
      if (energy < m_cell_ene_hg_up[i]) {
        return i;
      }
    }
    return (m_cell_ene_hg_up.size());
  } else {
    for (unsigned int i = 0; i < m_cell_ene_lg_up.size(); ++i) {
      if (energy < m_cell_ene_lg_up[i]) {
        return i;
      }
    }
    return (m_cell_ene_lg_up.size());
  }
}
