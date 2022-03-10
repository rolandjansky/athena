/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/AthenaMonManager.h"

// Athena include(s)
#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "MuonPrepRawData/CscStripPrepData.h"
#include "CscPrdValMonAlg.h"

using namespace Muon;

namespace {
  struct MonStruct1 {
    std::vector<float> spid;
    std::vector<float> secLayer;
    std::vector<int> lumiblock_mon;
    std::vector<int> noStrips;
    std::vector<int> signal_mon;
    std::vector<int> noise_mon;
    std::vector<int> clus_phiSig; 
    std::vector<int> clus_etaSig;
    std::vector<int> clus_etaNoise; 
    std::vector<int> clus_phiNoise;
    std::vector<int> sideC; 
    std::vector<int> sideA;
    std::vector<int> measphi; 
    std::vector<int> measeta;
  };

  struct MonStruct2 {
    std::vector<int> count_mon;
    std::vector<int> scount_mon;
    std::vector<float> tmp_val_mon;
    std::vector<float> secLayer;
    std::vector<int> mphi_true;
    std::vector<int> mphi_false;
    std::vector<int> scount_phi_false;
    std::vector<int> scount_phi_true;
    std::vector<int> scount_eta_false;
    std::vector<int> scount_eta_true;
  };
}

CscPrdValMonAlg::CscPrdValMonAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthMonitorAlgorithm(name,pSvcLocator) 
  { }
  
StatusCode CscPrdValMonAlg::initialize() {
  ATH_MSG_INFO( "CscPrdValMonAlg: in initialize" );
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_stripFitter.retrieve());
  ATH_MSG_INFO( "CscPrdValMonAlg " << name() << ": retrieved " << m_stripFitter );
  ATH_CHECK(m_cscPrdKey.initialize());

  return AthMonitorAlgorithm::initialize();
  
}

//
// fillHistograms ----------------------------------------------------------------
//
StatusCode CscPrdValMonAlg::fillHistograms( const EventContext& ctx ) const  {

  int lumiblock = -1;
  SG::ReadHandle<xAOD::EventInfo> evt(m_EventInfoKey, ctx);
  lumiblock = evt->lumiBlock();
  auto lumiblock_mon = Monitored::Scalar<int>("lumiblock_mon",lumiblock);
  if(lumiblock < 0 ) return StatusCode::FAILURE;
  

  // Part 1: Get the messaging service, print where you are
  ATH_MSG_DEBUG( "CscPrdValMonAlg: in fillHistograms" );

  SG::ReadHandle<CscStripPrepDataContainer> CscPRD(m_cscPrdKey, ctx);

  // ==============================================================================
  // Field           Range               Notes
  // ==============================================================================
  // StationName     unsigned integer    maps to "CSS", "CSL", etc.
  // StationEta      [-1,1]              -1 for backward, 1 for forward endcap
  // StationPhi      [1,8]               increases with Phi
  // Technology      [1]                 maps to "CSC"
  // ChamberLayer    [1,2]               increases with |Z|
  // WireLayer       [1,4]               increases with |Z|
  // MeasuresPhi     [0,1]               0 if measures R, 1 if measures Phi
  // Strip           [1,n]               increases with R   for MeasuresPhi=0
  //                                     increases with Phi for MeasuresPhi=1
  // ==============================================================================



  // Begin Event ==================================================
  ATH_MSG_DEBUG ( " BEGIN  EVENT ========================================== "  );
  ATH_MSG_DEBUG(" Size of PRD Container  : " << CscPRD->size());

  MonStruct1 monstruct1;
  MonStruct2 monstruct2;

  for (CscStripPrepDataContainer::const_iterator it = CscPRD->begin(); it != CscPRD->end(); ++it) {
    const CscStripPrepDataCollection *prd = *it;
    ATH_MSG_DEBUG ( " Size of Collection     : " << prd->size()  );
    size_t nEtaClusWidthCnt[5], nPhiClusWidthCnt[5];    // cluster position in each phi-layer
    int clusCount[33][9], sigclusCount[33][9];
    for(size_t kl = 0; kl < 33; kl++ ) {
      for(size_t km = 0; km < 9; km++ ) {
        if(kl == 0 && km < 5) {
          nEtaClusWidthCnt[km] = 0;
          nPhiClusWidthCnt[km] = 0;
        }
        clusCount[kl][km] = 0;
        sigclusCount[kl][km] = 0;
      } // end loop over km
    } // end loop over kl

    // loop over PRD-clusters
    // Loop over strip id's vector -- this is just one strip even though its a vector of ID's
    ATH_MSG_DEBUG ( " BEGIN Loop over Strips ========================================== "  );
    for (CscStripPrepDataCollection::const_iterator ic = (*it)->begin(); ic != (*it)->end(); ++ic) { // for-loop over PRD collection
      const CscStripPrepData& praw = **ic;

      // Identify the PRD cluster
      Identifier prawId = praw.identify();
      int stationName = m_idHelperSvc->cscIdHelper().stationName(prawId);
      int chamberType = m_idHelperSvc->cscIdHelper().stationNameIndex("CSS") == stationName ? 0 : 1;
      int stationEta  = m_idHelperSvc->cscIdHelper().stationEta(prawId);
      int stationPhi  = m_idHelperSvc->cscIdHelper().stationPhi(prawId);
      int wireLayer   = m_idHelperSvc->cscIdHelper().wireLayer(prawId);
      int measuresPhi = m_idHelperSvc->cscIdHelper().measuresPhi(prawId);
      int stripId     = m_idHelperSvc->cscIdHelper().strip(prawId);

      int sectorNo  = stationEta * (2 * stationPhi - chamberType);

      // compute the indices to store cluster count
      int ns = sectorNo < 0 ? sectorNo*(-1) : sectorNo+16; // [-16 -> -1] shifted to [1 -> 16] and [+1 -> +16] shifted to [+17 -> +32]
      int nl = (measuresPhi ? wireLayer : wireLayer+4);  // [ 1 -> 4] (phi-layers) and [5 -> 8] (eta-layers)

      clusCount[ns][nl]++;

      // indices for ns = [+1 -> +32]; 32 places (index '0' is not counted); allocated 33 places
      // indices for nl = [+1 -> +8]; 8 places (index '0' is not counted); allocated 9 places
      ATH_MSG_DEBUG(" ns = " << ns << "\tm_nl = " << nl << "\tm_sec = " << sectorNo << "\t m_lay= " 
          << wireLayer << "\t strip = " << stripId << "\tmPhi = " << measuresPhi);


      monstruct1.noStrips.push_back(prd->size());
      // y-axis fill value
      // sector# +2 layer 1 maps to +2 + 0.2*(1-1) + 0.1 = +2.1
      // sector# +2 layer 2 maps to +2 + 0.2*(2-1) + 0.1 = +2.3
      // sector# +2 layer 3 maps to +2 + 0.2*(3-1) + 0.1 = +2.5
      // sector# +2 layer 4 maps to +2 + 0.2*(4-1) + 0.1 = +2.7
      monstruct1.secLayer.push_back((sectorNo + 0.2 * (wireLayer - 1) + 0.1));
      int xfac = measuresPhi ? -1 : 1;        // [-1 -> -48] / [+1 -> +192]

      // x-axis fill value
      monstruct1.spid.push_back((stripId * xfac));
      monstruct1.measphi.push_back((int)measuresPhi);
      monstruct1.measeta.push_back((int)!(measuresPhi));
        
      if(m_mapxyrz) {
          
        auto x = Monitored::Scalar<float>("x",praw.globalPosition().x());
        auto y = Monitored::Scalar<float>("y",praw.globalPosition().y());
        auto z = Monitored::Scalar<float>("z",praw.globalPosition().z());
        auto r = Monitored::Scalar<float>("r",std::hypot(x,y));
    
        fill("CscPrdMonitor",z,r,y,x);
        ATH_MSG_DEBUG(" prd x = " << x << "\t y = " << y << "\t z = " << z );
      } // end if(m_mapxyrz)
     
      // Fit this strip and get Charge (in units of: # of electrons)
      ICscStripFitter::Result res;
      res = m_stripFitter->fit(praw);

      ATH_MSG_DEBUG ( "Strip q +- dq = " << res.charge  << " +- " << res.dcharge << "\t t +- dt = "
          << res.time << " +- " <<  res.dtime << "\t w +- dw = " << res.width << " +- "
          << res.dwidth << "\t status= " << res.status << "\t chisq= " << res.chsq);
   
      // determine of the cluster is a noise/signal cluster Max_Delta_ADC > NoiseCut
      float kiloele = 1.0e-3; // multiply # of electrons by this number to get kiloElectrons (1 ke = 1 ADC)
      float qstripADC = res.charge * kiloele;

      // By default res.status = -1
      // if strip fit is success res.status = 0
      // If fit fails use the peak sample. In this case res.status = 1

      bool signal = ((qstripADC > m_cscNoiseCut) && (res.status >= 0)) ? true : false;



      monstruct1.signal_mon.push_back((int)signal);
      monstruct1.noise_mon.push_back((int)!(signal));
      monstruct1.clus_phiSig.push_back((int)measuresPhi && (signal));
      monstruct1.clus_etaSig.push_back((int)(!measuresPhi) && (signal));
      monstruct1.clus_phiNoise.push_back((int)measuresPhi && !(signal));
      monstruct1.clus_etaNoise.push_back((int)(!measuresPhi) && !(signal));
      monstruct1.sideA.push_back((int)(stationEta==1) && (signal));
      monstruct1.sideC.push_back((int)(stationEta==-1) && (signal));

      // increment the signal-cluster count
      if(signal){
        sigclusCount[ns][nl]++;
        measuresPhi ? nPhiClusWidthCnt[wireLayer]++ : nEtaClusWidthCnt[wireLayer]++ ;
      } 

    } // end for-loop over PRD collection
    ATH_MSG_DEBUG ( " End loop over PRD collection======================" );

    for(size_t lcnt = 1; lcnt < 5; lcnt++ ) {
      auto nPhiClusWidthCnt_mon = Monitored::Scalar<int>("nPhiClusWidthCnt_mon",nPhiClusWidthCnt[lcnt]);
      auto nEtaClusWidthCnt_mon = Monitored::Scalar<int>("nEtaClusWidthCnt_mon",nEtaClusWidthCnt[lcnt]);
      fill("CscPrdMonitor", nPhiClusWidthCnt_mon, nEtaClusWidthCnt_mon);  
    } // end loop over lcnt
    

    int numeta = 0, numphi = 0;
    int numetasignal = 0, numphisignal = 0;
    
    for(int kl = 1; kl < 33; kl++ ) {
      float tmp_val = 0;

      for(int km = 1; km < 9; km++ ) {
        int lay = (km > 4 && km < 9) ? km-4 : km;  // 1,2,3,4 (phi-layers)     5-4, 6-4, 7-4, 8-4 (eta-layers)
        bool mphi = (km > 0 && km < 5) ? true : false; // 1,2,3,4 (phi-layers) 5,6,7,8 (eta-layers)
        std::string wlay = mphi ? "Phi-Layer " : "Eta-Layer: ";

        int count = clusCount[kl][km];
        int scount = sigclusCount[kl][km];

        monstruct2.count_mon.push_back(count);
        monstruct2.scount_mon.push_back(scount);        
        monstruct2.mphi_true.push_back((int)mphi && count);
        monstruct2.mphi_false.push_back((int)!(mphi) && count);
        monstruct2.scount_phi_true.push_back((int)mphi && count && scount);
        monstruct2.scount_phi_false.push_back((int)mphi && count && !scount);
        monstruct2.scount_eta_true.push_back((int)!(mphi) && count && scount);
        monstruct2.scount_eta_false.push_back((int)!(mphi) && count && !scount);
        monstruct2.secLayer.push_back((kl-16 + 0.2 * (lay - 1) + 0.1));

        if(count) {
          if(mphi){
            numphi += count;
            if(scount){
              numphisignal += scount;
              tmp_val = count - scount;
            } else tmp_val = count;
          } else{
            numeta += count;
            if(scount){
              numetasignal += scount;
              tmp_val = count - scount;
            } else tmp_val = count;
          }
          ATH_MSG_DEBUG ( wlay << "Counts sec: [" << kl-16 << "]\tlayer: [" << km << "] = " <<
              monstruct2.secLayer.back() << "\t = " << monstruct2.count_mon.back() 
              << "\t" << monstruct2.scount_mon.back());
        }//end count
        monstruct2.tmp_val_mon.push_back(tmp_val);
      } //end for km
    } //end for kl

    auto numphi_mon = Monitored::Scalar<int>("numphi_mon", numphi);
    auto numeta_mon = Monitored::Scalar<int>("numeta_mon", numeta);
    auto numphi_sig_mon = Monitored::Scalar<int>("numphi_sig_mon", numphisignal);
    auto numeta_sig_mon = Monitored::Scalar<int>("numeta_sig_mon", numetasignal);
    auto numphi_diff_mon = Monitored::Scalar<int>("numphi_diff_mon", numphi-numphisignal);
    auto numeta_diff_mon = Monitored::Scalar<int>("numeta_diff_mon", numeta-numetasignal);

    fill("CscPrdMonitor", numphi_mon, numeta_mon, numphi_sig_mon, numeta_sig_mon, numphi_diff_mon, numeta_diff_mon );

  } // end for-loop over container
  
auto noStrips = Monitored::Collection("noStrips",monstruct1.noStrips);
auto secLayer1 = Monitored::Collection("secLayer", monstruct1.secLayer);
auto spid = Monitored::Collection("spid", monstruct1.spid);
auto measphi = Monitored::Collection("measphi", monstruct1.measphi);
auto measeta = Monitored::Collection("measeta", monstruct1.measeta);
auto signal_mon = Monitored::Collection("signal_mon", monstruct1.signal_mon);
auto noise_mon = Monitored::Collection("noise_mon", monstruct1.noise_mon);
auto clus_phiSig = Monitored::Collection("clus_phiSig", monstruct1.clus_phiSig);
auto clus_etaSig = Monitored::Collection("clus_etaSig", monstruct1.clus_etaSig);
auto clus_phiNoise = Monitored::Collection("clus_phiNoise", monstruct1.clus_phiNoise);
auto clus_etaNoise = Monitored::Collection("clus_etaNoise", monstruct1.clus_etaNoise);
auto sideA = Monitored::Collection("sideA",monstruct1.sideA);
auto sideC = Monitored::Collection("sideC",monstruct1.sideC);
fill("CscPrdMonitor", spid, secLayer1, lumiblock_mon, noStrips, signal_mon, noise_mon, clus_phiSig, clus_etaSig, clus_etaNoise, clus_phiNoise, sideC, sideA, measphi, measeta );

auto count_mon = Monitored::Collection("count_mon", monstruct2.count_mon);
auto scount_mon = Monitored::Collection("scount_mon", monstruct2.scount_mon);
auto mphi_true = Monitored::Collection("mphi_true", monstruct2.mphi_true);
auto mphi_false = Monitored::Collection("mphi_false", monstruct2.mphi_false);
auto scount_phi_true = Monitored::Collection("scount_phi_true", monstruct2.scount_phi_true);
auto scount_phi_false = Monitored::Collection("scount_phi_false", monstruct2.scount_phi_false);
auto scount_eta_true = Monitored::Collection("scount_eta_true", monstruct2.scount_eta_true);
auto scount_eta_false = Monitored::Collection("scount_eta_false", monstruct2.scount_eta_false);
auto secLayer = Monitored::Collection("secLayer", monstruct2.secLayer);  
auto tmp_val_mon = Monitored::Collection("tmp_val_mon", monstruct2.tmp_val_mon);
fill("CscPrdMonitor", count_mon, scount_mon, tmp_val_mon, secLayer, mphi_true, mphi_false, scount_phi_false, scount_phi_true, scount_eta_false, scount_eta_true );


  ATH_MSG_DEBUG ( " End EVENT======================" );

  ATH_MSG_DEBUG( "CscPrdValMonAlg: fillHistograms reports success" );

  return StatusCode::SUCCESS;
} // end CscPrdValMonAlg::fillHistograms()
