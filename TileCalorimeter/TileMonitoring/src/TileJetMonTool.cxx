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

#include "TileMonitoring/TileJetMonTool.h"

#include <iostream>
#include <string>

#include "Navigation/NavigationToken.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloSamplingHelper.h"
#include "CaloGeoHelpers/CaloSampling.h" 
#include "JetEvent/Jet.h"
#include "JetEvent/JetConstituentIterator.h"
#include "JetEvent/JetCollection.h"
//#include "JetUtils/JetCollectionHelper.h"
//#include "JetUtils/JetCaloHelper.h"
#include "JetUtils/JetCaloQualityUtils.h"
// #include "src/JetCaloUtilsFillerTool.h"
/* this was an attempt to exploit isBad() from JetCaloUtilsFillerTool, but
   then I discovered it is private. So need to define it here anyway... */

#include "TileEvent/TileCell.h"
#include "TileEvent/TileContainer.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/ITileBadChanTool.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/errorcheck.h"


#include "TH2F.h"
//#include "TProfile.h"

// copy of auxillary functions from JetD3PDMaker/src/JetCaloUtilsFillerTool.cxx

namespace {


  double hecF(const Jet* jet) {
    double e_hec = jet->getShape("energy_" + CaloSampling::getSamplingName(CaloSampling::HEC0))
        + jet->getShape("energy_" + CaloSampling::getSamplingName(CaloSampling::HEC1))
        + jet->getShape("energy_" + CaloSampling::getSamplingName(CaloSampling::HEC2))
        + jet->getShape("energy_" + CaloSampling::getSamplingName(CaloSampling::HEC3));

    double e_jet = jet->getRawE();

    if (e_jet != 0)
      return e_hec / e_jet;
    else
      return -999;
  }

#if 0
  double tileGap3F(const Jet* jet) {
    double e_tileGap3 = jet->getShape(
        "energy_" + CaloSampling::getSamplingName(CaloSampling::TileGap3));
    double e_jet = jet->getRawE();

    if (e_jet != 0)
      return e_tileGap3 / e_jet;
    else
      return -999;
  }
#endif


  double jetEMFraction(const Jet * jet) {
    std::string base = "energy_";
    unsigned int nsamp = CaloSampling::getNumberOfSamplings();
    double e_HAD = 0;
    double e_EM = 0;

    for (unsigned int i = 0; i < nsamp; i++) {
      double e = jet->getShape(base + CaloSampling::getSamplingName(i));
      if (CaloSamplingHelper::isEMSampling((CaloSampling::CaloSample) (i)))
        e_EM += e;
      else if (CaloSamplingHelper::isHADSampling((CaloSampling::CaloSample) (i))) e_HAD += e;
    }

    if ((e_EM == 0) || ((e_EM + e_HAD) == 0)) return 0.;
    return (e_EM / (e_EM + e_HAD));
  }



  double fracSamplingMax(const Jet* jet, int& SamplingMax) {
    std::string base = "energy_";
    double fracSamplingMax = -999999999.;
    double sumE_samplings = 0.;
    unsigned int nsamp = CaloSampling::getNumberOfSamplings();
    for (unsigned int i = 0; i < nsamp; i++) {
      double e = jet->getShape(base + CaloSampling::getSamplingName(i));
      sumE_samplings += e;
      if (e > fracSamplingMax) {
        fracSamplingMax = e;
        SamplingMax = i;
      }
    }

    if (sumE_samplings != 0)
      fracSamplingMax /= sumE_samplings;
    else
      fracSamplingMax = 0;

    return fracSamplingMax;
  }


#if 0
  bool isUgly(const Jet* jet) {

    double fcor = jet->getMoment("BCH_CORR_CELL");
    double tileGap3f = tileGap3F(jet);

    if (fcor > 0.5) return true;
    if (tileGap3f > 0.5) return true;

    return false;
  }
#endif


}


/*---------------------------------------------------------*/
TileJetMonTool::TileJetMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent), m_tileBadChanTool("TileBadChanTool")
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("jetPtMin",m_jetPtMin=20000.); //Threshold in MeV
  declareProperty("jetEtaMax",m_jetEtaMax=1.6);
  declareProperty("jetCollectionName",m_jetCollectionName="AntiKt4Topo");
  declareProperty("energyChanMin",m_energyChanMin=2000);
  declareProperty("energyChanMax",m_energyChanMax=4000);
  declareProperty("histoStreamName",m_THistSvcStreamName="/EXPERT");

  /* see 
 http://alxr.usatlas.bnl.gov/lxr-stb6/source/atlas/TileCalorimeter/TileMonitoring/src/TileFatherMonTool.cxx#101
  */
  m_path = "Tile"; //ROOT File directory.
  m_stem = m_THistSvcStreamName; // + m_path;


  m_name = name;

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

  CHECK(TileFatherMonTool::initialize());

  return bookTimeHistograms();
}

/*---------------------------------------------------------*/
StatusCode TileJetMonTool::fillHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG("in TileJetMonTool::fillHistograms()");

  const JetCollection* jetcol;
  CHECK(evtStore()->retrieve(jetcol, m_jetCollectionName));

  ATH_MSG_DEBUG( "TileJetMonTool::fillHistograms retrieval of jet collection " << m_jetCollectionName << " succeeded.");

  fillEvtInfo();
  uint32_t LumiBlock = getLumiBlock();

  ATH_MSG_DEBUG("TileJetMonTool::fillHistograms(), lumiblock ");

  JetCollection::const_iterator it;
  for (it = jetcol->begin(); it != jetcol->end(); ++it) {
    const Jet *jet = *it;
    if ((jet->pt() > m_jetPtMin) && (fabs(jet->eta()) < m_jetEtaMax)) {
      if (isGoodJet(*jet)) {
        CHECK(fillTimeHistograms(*jet, LumiBlock));
      }
    }
  }

  ATH_MSG_DEBUG("TileJetMonTool::fillHistograms(), end-of-loops ");
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileJetMonTool::finalize() {
/*---------------------------------------------------------*/

  float sum = 0;
  for (int p = 0; p < NPART; ++p) {
    for (int m = 0; m < NMOD; ++m) {
      for (int ch = 0; ch < NPMT; ++ch) {
        if (m_TileChanTime[p][m * NPMT + ch] != NULL) {
          sum += m_TileChanTime[p][m * NPMT + ch]->GetEntries();
        }
      }
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
  std::string hname, htitle;
  char modnum[3], chnum[3]; // must be of length 3, since the last char is supposed to be '\0'
  //  TH2F *hist;
  for (int p = 0; p < NPART; ++p) {
    for (int m = 0; m < NMOD; ++m) {
      sprintf(modnum, "%02d", m + 1);
      for (int ch = 0; ch < NPMT; ++ch) {
        sprintf(chnum, "%02d", ch);
        hname = m_path + "/" + m_partname[p] + modnum + "_ch_" + chnum;
        htitle = "Time in " + m_partname[p] + modnum + "_ch_" + chnum;
        if ((p == 0) && (m == 0)) {
          ATH_MSG_DEBUG( "Booking histo " << hname  );
        }
        /* TH2F histograms are too much in memory. Trying to book only 10 LB
         and set rebin-bit */
        m_TileChanTime[p].push_back( book2F(m_stem + "/" + m_path + "/" + m_partname[p]
                                            , m_partname[p] + modnum + "_ch_" + chnum, m_partname[p] + modnum + "_ch_" + chnum
                                            , 10, 0, 10, 120, -30.0, 30.0));
        //	m_TileChanTime[p][m*NPMT+ch]->SetBit(TH1::kCanRebin);
        /*
         m_TileChanTime[p].push_back(bookProfile(m_stem + "/" + m_path + "/" + m_partname[p],
         m_partname[p]+modnum+"_ch_"+chnum,
         m_partname[p]+modnum+"_ch_"+chnum,
         1500,0.5,1500.5,-30.0,30.0));
         */
      }
    }
  }

  ATH_MSG_INFO( "All histograms booked " );

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/  

void TileJetMonTool::clearTimeHistograms() {
/*---------------------------------------------------------*/

  for (int p = 0; p < NPART; ++p) {
    m_TileChanTime[p].clear();
  }
}

/*---------------------------------------------------------*/  
StatusCode TileJetMonTool::fillTimeHistograms(const Jet& jet, uint32_t LumiBlock) {
/*---------------------------------------------------------*/  


  ATH_MSG_DEBUG( "in fillTimeHistograms()" );

  
  /* 
     Code from PhysicsAnalysis/D3PDMaker/JetD3PDMaker/src/JetTileCellFillerTool.cxx
  */

  // NavigationToken<CaloCell,double> cellToken;
  NavigationToken<CaloCluster, double> ClusToken;
  jet.fillToken(ClusToken, double(1.0));

  //long icluster, icell;
  //icluster = 0;
  typedef NavigationToken<CaloCluster, double>::const_iterator Clus_cItr;
  typedef CaloCluster::cell_iterator Cell_Itr;
  Clus_cItr ClusEnd = ClusToken.end();
  for (Clus_cItr it = ClusToken.begin(); it != ClusEnd; ++it) {  // loop over clusters
    const CaloCluster *cluster = *it;

    /*
    ATH_MSG_INFO( "fillTimeHistograms, cluster " << icluster
                  << ", eta = " << it->eta() );
     icluster++;
     */
    Cell_Itr cBegin = cluster->cell_begin();
    Cell_Itr cEnd = cluster->cell_end();
    //icell = 0;
    for (Cell_Itr itr = cBegin; itr < cEnd; ++itr) { // loop over cells

      /*
      ATH_MSG_INFO(  "fillTimeHistograms, cell " << icell );
      icell++;
       */
      const CaloCell *cell = *itr;
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
        long gain1 = tilecell->gain1();
        long gain2 = tilecell->gain2();
        long qbit1 = tilecell->qbit1();
        long qbit2 = tilecell->qbit2();

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

//          ATH_MSG_INFO( "Filling in time for " << m_partname[part1-1] << mod1
//                        << ", ch " << chan1
//                        << ", LB " << sLumiBlock );

          m_TileChanTime[part1 - 1][mod1 * NPMT + chan1]->Fill(sLumiBlock, tilecell->time1(), 1);
        }
        if (isGoodChannel(part2, mod2, pmt2, bad2, qbit2, ene2)) {
          m_TileChanTime[part2 - 1][mod2 * NPMT + chan2]->Fill(sLumiBlock, tilecell->time2(), 1);
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}
/*
 A jak to udelame s celami, ktere jsou ve vice clusterech v jednom jetu ??
 */

bool TileJetMonTool::isGoodChannel(int part, int mod, int pmt, uint32_t bad, long qbit, float energy) {
  /*
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "isGoodChannel: part " << part << ", mod " << mod+1
   << ", pmt " << pmt << ", bad " << bad << ", qbit " << qbit
   << ", ene " << energy << endreq;
   */
  if ((part <= 0) || (part > NPART)) // invalid partition
    return false;
  if ((mod < 0) || (mod >= NMOD))   // invalid module number
    return false;
  if (pmt <= 0)                     // non-existing PMT (empty hole)
    return false;
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

bool TileJetMonTool::isGoodJet(const Jet &jet) {
  double hecf = hecF(&jet);
  //double tileGap3f = tileGap3F(&jet);

  double quality = jet.getMoment("LArQuality");
  double HecQ = jet.getMoment("HECQuality");
  double NegE = jet.getMoment("NegativeE");
  double emf = jetEMFraction(&jet);
  double time = jet.getMoment("Timing");
  double pt = jet.pt();
  double chf = pt != 0 ? jet.getMoment("sumPtTrk") / pt : 0;

  double em_eta = jet.eta(P4SignalState::JETEMSCALE);

  int SamplingMax = CaloSampling::Unknown;
  double fmax = fracSamplingMax(&jet, SamplingMax);

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
