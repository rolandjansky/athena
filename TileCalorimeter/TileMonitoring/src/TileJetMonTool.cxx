/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TODO:
  - upravit volani book2F, resp nastavit spravne m_path. Pouzit to co
  je zakomentovane v http://alxr.usatlas.bnl.gov/lxr-stb6/source/atlas/TileCalorimeter/TileMonitoring/src/TileFatherMonTool.cxx#077,  http://alxr.usatlas.bnl.gov/lxr-stb6/source/atlas/TileCalorimeter/TileMonitoring/src/TileFatherMonTool.cxx#101
*/
// ********************************************************************
//
// NAME:     TileJetMonTool.cxx
// PACKAGE:  TileMonitoring 
//
// AUTHOR:   Tomas Davidek (Tomas.Davidek@cern.ch)
//           
//
// ********************************************************************

#include "xAODJet/JetContainer.h"
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


#include "TH2F.h"
//#include "TProfile.h"

#include <iostream>
#include <string>



/*---------------------------------------------------------*/
TileJetMonTool::TileJetMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_tileBadChanTool("TileBadChanTool")
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("jetPtMin", m_jetPtMin = 20000.); //Threshold in MeV
  declareProperty("jetEtaMax", m_jetEtaMax = 1.6);
  declareProperty("jetCollectionName", m_jetCollectionName = "AntiKt4EMTopoJets");
  declareProperty("energyChanMin", m_energyChanMin = 2000);
  declareProperty("energyChanMax", m_energyChanMax = 4000);
  declareProperty("do_1dim_histos", m_do_1dim_histos = false);
  m_path = "/Tile/Jet";

  m_partname[0] = "LBA";
  m_partname[1] = "LBC";
  m_partname[2] = "EBA";
  m_partname[3] = "EBC";

}

/*---------------------------------------------------------*/
TileJetMonTool::~TileJetMonTool() {
/*---------------------------------------------------------*/

}


/*---------------------------------------------------------*/
StatusCode TileJetMonTool::initialize() {
/*---------------------------------------------------------*/


  ATH_MSG_INFO("in initialize()");

//=== get TileBadChanTool
  ATH_MSG_DEBUG("TileJetMonTool: Retrieving tile bad channel tool");
  CHECK(m_tileBadChanTool.retrieve());
  ATH_MSG_DEBUG("TileJetMonTool: Retrieved tile bad channel tool");

 return TileFatherMonTool::initialize();
}


/*---------------------------------------------------------*/
StatusCode TileJetMonTool::bookHistograms() {
/*---------------------------------------------------------*/

  return bookTimeHistograms();
}


/*---------------------------------------------------------*/
StatusCode TileJetMonTool::fillHistograms() {
/*---------------------------------------------------------*/


  //  const JetCollection* jetcol;
  const xAOD::JetContainer* jetContainer;
  CHECK(evtStore()->retrieve(jetContainer, m_jetCollectionName));


  fillEvtInfo();
  uint32_t LumiBlock = getLumiBlock();

  ATH_MSG_VERBOSE("TileJetMonTool::fillHistograms(), lumiblock ");


  for (const xAOD::Jet* jet : *jetContainer) {
    if ((jet->pt() > m_jetPtMin) && (fabs(jet->eta()) < m_jetEtaMax)) {
      if (isGoodJet(*jet)) {
        CHECK(fillTimeHistograms(*jet, LumiBlock));
      }
    }
  }

  ATH_MSG_VERBOSE("TileJetMonTool::fillHistograms(), end-of-loops ");
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileJetMonTool::procHistograms() {
/*---------------------------------------------------------*/

  float sum = 0;

  for (int p = 0; p < NPART; ++p) {
    for (const TH2* channelTimeHistogram : m_TileChanTime[p]) {
      if (channelTimeHistogram->GetEntries() != 0)
        ATH_MSG_DEBUG( "Not empty: name/entries => "  
                       << channelTimeHistogram->GetName() 
                       << "/" << channelTimeHistogram->GetEntries());
      
      sum += channelTimeHistogram->GetEntries();
    }
  }


  ATH_MSG_INFO( "Total number of entries in histograms: " << sum );

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileJetMonTool::bookTimeHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in TileJetMonTool::bookTimeHistograms()" );


  // book histograms for all channels, even if some of them remain empty 
  // in the end
  std::string runNumStr = getRunNumStr();
  std::string htitle;
  std::string moduleName;
  char chnum[3]; // must be of length 3, since the last char is supposed to be '\0'
  //  TH2F *hist;
  for (int p = 0; p < NPART; ++p) {
    // first, create the total timing histograms for each partition
    // these histograms have fine time binning
    m_TilePartTime.push_back( book1F( m_partname[p]
				      , m_partname[p], m_partname[p]
				      , 600, -30.0, 30.0));
    if ( p >= 2){
      m_TileEBTime_NoScint.push_back( book1F( m_partname[p]
					      , m_partname[p] + "_NoScint", m_partname[p] + "_NoScint"
					      , 600, -30.0, 30.0));
    }
    for (unsigned int m = 0; m < TileCalibUtils::MAX_DRAWER; ++m) {
      moduleName = TileCalibUtils::getDrawerString(p + 1, m);

      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {

        sprintf(chnum, "%02d", channel);
        htitle = "Time in " + moduleName + "_ch_" + chnum;

        /* TH2F histograms are too much in memory. Trying to book only 10 LB
         and set rebin-bit */
        m_TileChanTime[p].push_back( book2F( m_partname[p]
					     , moduleName + "_ch_" + chnum, moduleName + "_ch_" + chnum
					     , 10, 0, 10, 120, -30.0, 30.0));
        //	m_TileChanTime[p][m*TileCalibUtils::MAX_CHAN+ch]->SetBit(TH1::kCanRebin);
        /*
         m_TileChanTime[p].push_back(bookProfile( m_partname[p],
                                                  moduleName+"_ch_"+chnum,
                                                  moduleName+"_ch_"+chnum,
                                                  1500,0.5,1500.5,-30.0,30.0));
         */

	if (m_do_1dim_histos) {
	  m_TileChanTime1D[p].push_back( book1F( m_partname[p]
						 , moduleName + "_ch_" + chnum + "_1d"
						 , moduleName + "_ch_" + chnum + "_1d"
						 , 600, -30.0, 30.0));
	}
      }
    }
  }

  ATH_MSG_INFO( "All histograms booked " );

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/  

void TileJetMonTool::clearTimeHistograms() {
/*---------------------------------------------------------*/
  m_TilePartTime.clear();
  m_TileEBTime_NoScint.clear();
  for (int p = 0; p < NPART; ++p) {
    m_TileChanTime[p].clear();
    if (m_do_1dim_histos) {
      m_TileChanTime1D[p].clear();
    }
  }
}

/*---------------------------------------------------------*/  
StatusCode TileJetMonTool::fillTimeHistograms(const xAOD::Jet& jet, uint32_t LumiBlock) {
/*---------------------------------------------------------*/  


  ATH_MSG_VERBOSE( "in fillTimeHistograms()" );

  if( jet.numConstituents() == 0) return StatusCode::SUCCESS;

  jet::JetCellAccessor::const_iterator cellIt =  jet::JetCellAccessor::begin(&jet);
  jet::JetCellAccessor::const_iterator cellItE = jet::JetCellAccessor::end(&jet);
     
  for( ;cellIt != cellItE; cellIt++) {

    const CaloCell* cell = (*cellIt);

    if (cell->caloDDE()->getSubCalo() == CaloCell_ID::TILE) { // a Tile Cell

      const TileCell *tilecell = (TileCell*) cell;
      Identifier id = tilecell->ID();
      int part1 = 0;
      int part2 = 0;
      int mod1 = -1;
      int mod2 = -1;
      int chan1 = -1;
      int chan2 = -1;
      int pmt1 = -1;
      int pmt2 = -1;
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
        mod1 = m_tileID->module(id);
        chan1 = m_tileHWID->channel(adc_id);
        pmt1 = m_cabling->channel2hole(part1, chan1);
        bad1 = m_tileBadChanTool->encodeStatus(m_tileBadChanTool->getAdcStatus(adc_id));
      }
      
      // How is it here with partition? D0 spans two partitions....
      // It should be ok to treat it in this way:
      IdentifierHash hash2 = caloDDE->onl2();
      if (hash2 != TileHWID::NOT_VALID_HASH) {
        HWIdentifier adc_id = m_tileHWID->adc_id(hash2, gain2);
        part2 = m_tileHWID->ros(adc_id);
        mod2 = m_tileID->module(id);
        chan2 = m_tileHWID->channel(adc_id);
        pmt2 = m_cabling->channel2hole(part2, chan2);
        bad2 = m_tileBadChanTool->encodeStatus(m_tileBadChanTool->getAdcStatus(adc_id));
      }
      
      float ene1 = (pmt1 > 0) ? tilecell->ene1() : -1;
      float ene2 = (pmt2 > 0) ? tilecell->ene2() : -1;
      
      //        ATH_MSG_INFO( "This is Tile cell " << icell
      //                      << ": pmts " << pmt1 << "," << pmt2
      //                      << ", ene "  << ene1 << "," << ene2 );
      
      
      /*
        Now really fill the histograms
      */
      char sLumiBlock[10];
      sprintf(sLumiBlock, "%d", LumiBlock);
      if (isGoodChannel(part1, mod1, pmt1, bad1, qbit1, ene1)) {
        
        ATH_MSG_DEBUG( "Filling in time for " << TileCalibUtils::getDrawerString(part1, mod1)
                       << ", ch " << chan1
                       << ", LB " << sLumiBlock 
                       << ", time: " << tilecell->time1());
        
        m_TileChanTime[part1 - 1][TileCalibUtils::MAX_CHAN * mod1 + chan1]->Fill(sLumiBlock, tilecell->time1(), 1);
	if (m_do_1dim_histos) {
	  m_TileChanTime1D[part1 - 1][TileCalibUtils::MAX_CHAN * mod1 + chan1]->Fill(tilecell->time1(), 1);
	}
	m_TilePartTime[part1 - 1]->Fill(tilecell->time1());
	if (part1 >= 3 && chan1 != 0 && chan1 != 1 && chan1 != 12 && chan1 != 13) m_TileEBTime_NoScint[part1 - 3]->Fill(tilecell->time1());
      }
      if (isGoodChannel(part2, mod2, pmt2, bad2, qbit2, ene2)) {
        ATH_MSG_DEBUG( "Filling in time for " << TileCalibUtils::getDrawerString(part2, mod2)
                       << ", ch " << chan2
                       << ", LB " << sLumiBlock 
                       << ", time: " << tilecell->time2());

        m_TileChanTime[part2 - 1][TileCalibUtils::MAX_CHAN * mod2 + chan2]->Fill(sLumiBlock, tilecell->time2(), 1);
	if (m_do_1dim_histos) {
	  m_TileChanTime1D[part2 - 1][TileCalibUtils::MAX_CHAN * mod2 + chan2]->Fill(tilecell->time2(), 1);
	}
	m_TilePartTime[part2 - 1]->Fill(tilecell->time2());
	if (part2 >= 3 && chan2 != 0 && chan2 != 1 && chan2 != 12 && chan2 != 13) m_TileEBTime_NoScint[part2 - 3]->Fill(tilecell->time2());
      }
    }
  }
  
  return StatusCode::SUCCESS;
}
/*
 A jak to udelame s celami, ktere jsou ve vice clusterech v jednom jetu ??
 */

bool TileJetMonTool::isGoodChannel(int part, int mod, int pmt, uint32_t bad, unsigned int qbit, float energy) {
  /*
  ATH_MSG_DEBUG( "isGoodChannel: mod " << TileCalibUtils::getDrawerString(part, mod)
                 << ", pmt " << pmt 
                 << ", bad " << bad 
                 << ", qbit " << qbit
                 << ", ene " << energy );
  */

  if ((part <= 0) || (part > NPART)) return false;// invalid partition

  if ((mod < 0) || (mod >= (int) TileCalibUtils::MAX_DRAWER)) return false;  // invalid module number

  if (pmt <= 0) return false;   // non-existing PMT (empty hole)

  if (((qbit & TileCell::MASK_BADCH) != 0) || // masked as bad
      ((qbit & TileCell::MASK_TIME) != TileCell::MASK_TIME) ||  // flagged
      ((qbit & FLAG_OF2) != FLAG_OF2)) // not reco'ed with OF2

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
  if (bad > 1) return false;

  /* Masking on the fly not really needed, since we are reading ESD */
  if ((energy < m_energyChanMin) || (energy > m_energyChanMax)) return false;
  return true;
}

bool TileJetMonTool::isGoodJet(const xAOD::Jet& jet) {
  double hecf = jet.getAttribute<float>(xAOD::JetAttribute::HECFrac);

  //double tileGap3f = tileGap3F(&jet);

  double quality = jet.getAttribute<float>(xAOD::JetAttribute::LArQuality);
  double HecQ = jet.getAttribute<float>(xAOD::JetAttribute::HECQuality);
  double NegE = jet.getAttribute<float>(xAOD::JetAttribute::NegativeE);
  double emf = jet.getAttribute<float>(xAOD::JetAttribute::EMFrac);
  double time = jet.getAttribute<float>(xAOD::JetAttribute::Timing);
  double pt = jet.pt();
  const std::vector<float>& sumPtTrk = jet.getAttribute<std::vector<float> >(xAOD::JetAttribute::SumPtTrkPt1000);
  double chf = (pt != 0 && !sumPtTrk.empty()) ? sumPtTrk[0] / pt : 0;

  double em_eta = jet.jetP4(xAOD::JetEMScaleMomentum).eta();

  double fmax = jet.getAttribute<float>(xAOD::JetAttribute::FracSamplingMax);

  bool isBadJet = isBad(MediumBad, quality, NegE, emf, hecf, time, fmax, em_eta, chf, HecQ);
  return (!isBadJet);
}

/*
  This is copy & paste from JetCaloUtilsFillerTool.cxx. The function is private,
  so it cannot be just included
*/
bool TileJetMonTool::isBad(BadJetCategory criteria, double quality, double NegE, double emf,
    double hecf, double time, double fmax, double eta, double chf, double HecQ) {
  if (criteria == LooseBad || criteria == MediumBad || criteria == TightBad) {
    // HEC spike
    if (hecf > 0.5 && fabs(HecQ) > 0.5) return true;
    if (fabs(NegE) > 60000./*MeV*/) return true;
    // EM coherent noise
    if (emf > 0.95 && fabs(quality) > 0.8 && fabs(eta) < 2.8) return true;
    // Cosmics and Beam background
    if (fabs(time) > 25.) return true;
    if (emf < 0.05 && chf < 0.05 && fabs(eta) < 2.) return true;
    if (emf < 0.05 && fabs(eta) > 2.) return true;
    if (fmax > 0.99 && fabs(eta) < 2) return true;
  }

  if (criteria == MediumBad || criteria == TightBad) {
    // HEC spike
    if (hecf > 1 - fabs(HecQ)) return true;
    // EM coherent noise
    if (emf > 0.9 && fabs(quality) > 0.8 && fabs(eta) < 2.8) return true;
    // Cosmics and Beam background
    if (fabs(time) > 10.) return true;
    if (emf < 0.05 && chf < 0.1 && fabs(eta) < 2.) return true;
    if (emf > 0.95 && chf < 0.05 && fabs(eta) < 2.) return true;
  }

  if (criteria == TightBad) {
    // EM coherent noise
    if (fabs(quality) > 0.95) return true;
    if (emf > 0.98 && fabs(quality) > 0.05) return true;
    // Cosmics and Beam background
    if (emf < 0.1 && chf < 0.2 && fabs(eta) < 2.) return true;
    if (emf < 0.1 && fabs(eta) > 2.) return true;
    if (emf > 0.9 && chf < 0.02 && fabs(eta) < 2.) return true;

  }
  return false;
}
