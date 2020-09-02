/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/*   NAME    : CscPrdValMonAlg.cxx
 *   PACKAGE : MuonRawDataMonitoring/CscRawDataMonitoring
 *   PURPOSE : CSC PrepRawData (PRD) monitoring
 *   AUTHOR  : Ken Johns, Xiaowen Lei (U.Arizona)
 *
 *   MODIFIED: N. Benekos(Illinois)
 *             V. Kaushik(U.Arizona) : 2009-05-12
 *             J. Veatch(U.Arizona)  : 2012-02-08
 
 *   MODIFIED: C.Kitsaki, C.Paraskevopoulos
 */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#include "AthenaMonitoring/AthenaMonManager.h"

// Athena include(s)
#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "MuonPrepRawData/CscStripPrepData.h"
#include "CscRawDataMonitoring/CscPrdValMonAlg.h"

using namespace Muon;

CscPrdValMonAlg::CscPrdValMonAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthMonitorAlgorithm(name,pSvcLocator)
  {
    declareProperty("NoiseCutADC", m_cscNoiseCut = 50);
    declareProperty("MapYXandRZ", m_mapxyrz = false);
  }


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

  for (CscStripPrepDataContainer::const_iterator it = CscPRD->begin(); it != CscPRD->end(); ++it) {
    const CscStripPrepDataCollection *prd = *it;
    ATH_MSG_DEBUG ( " Size of Collection     : " << prd->size()  );
   // size_t noStrips = prd->size();  // no. of strips in this cluster = m_stripIds.size()
    auto noStrips = Monitored::Scalar<int>("noStrips",prd->size());
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
      std::string stationString = m_idHelperSvc->cscIdHelper().stationNameString(stationName);
      int chamberType = stationString == "CSS" ? 0 : 1;
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


      // y-axis fill value
      // sector# +2 layer 1 maps to +2 + 0.2*(1-1) + 0.1 = +2.1
      // sector# +2 layer 2 maps to +2 + 0.2*(2-1) + 0.1 = +2.3
      // sector# +2 layer 3 maps to +2 + 0.2*(3-1) + 0.1 = +2.5
      // sector# +2 layer 4 maps to +2 + 0.2*(4-1) + 0.1 = +2.7
      auto secLayer = Monitored::Scalar<float>("secLayer", (sectorNo + 0.2 * (wireLayer - 1) + 0.1));
      int xfac = measuresPhi ? -1 : 1;        // [-1 -> -48] / [+1 -> +192]

      // x-axis fill value
      auto spid = Monitored::Scalar<float>("spid", (stripId * xfac) );
      auto measphi = Monitored::Scalar<int>("measphi", (int)measuresPhi);
      auto measeta = Monitored::Scalar<int>("measeta", (int)!(measuresPhi));
     
      fill("CscPrdMonitor",spid, secLayer, noStrips, measphi, measeta);
        
      if(m_mapxyrz) {
          
        auto x = Monitored::Scalar<float>("x",praw.globalPosition().x());
        auto y = Monitored::Scalar<float>("y",praw.globalPosition().y());
        auto z = Monitored::Scalar<float>("z",praw.globalPosition().z());
        auto r = Monitored::Scalar<float>("r",std::hypot(x,y));
    
        fill("CscPrdMonitor",z,r);
        fill("CscPrdMonitor",y,x);
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

      auto signal_mon = Monitored::Scalar<int>("signal_mon", (int)signal);
      auto noise_mon = Monitored::Scalar<int>("noise_mon", (int)!(signal));
      auto clus_phiSig = Monitored::Scalar<int>("clus_phiSig", (int)measuresPhi && (signal));
      auto clus_etaSig = Monitored::Scalar<int>("clus_etaSig", (int)(!measuresPhi) && (signal));
      auto clus_phiNoise = Monitored::Scalar<int>("clus_phiNoise", (int)measuresPhi && !(signal));
      auto clus_etaNoise = Monitored::Scalar<int>("clus_etaNoise", (int)(!measuresPhi) && !(signal));
      auto sideA = Monitored::Scalar<int>("sideA",(int)(stationEta==1) && (signal));
      auto sideC = Monitored::Scalar<int>("sideC",(int)(stationEta==-1) && (signal));

      // increment the signal-cluster count
      if(signal){
        sigclusCount[ns][nl]++;
        measuresPhi ? nPhiClusWidthCnt[wireLayer]++ : nEtaClusWidthCnt[wireLayer]++ ;
      } 

      fill("CscPrdMonitor", spid, secLayer, lumiblock_mon, noStrips, signal_mon, noise_mon, clus_phiSig, clus_etaSig, clus_etaNoise, clus_etaNoise, sideC, sideA);

    } // end for-loop over PRD collection
    ATH_MSG_DEBUG ( " End loop over PRD collection======================" );

    for(size_t lcnt = 1; lcnt < 5; lcnt++ ) {
      int tmp_phiClus = nPhiClusWidthCnt[lcnt];
      int tmp_etaClus = nEtaClusWidthCnt[lcnt];
      auto nPhiClusWidthCnt_mon = Monitored::Scalar<int>("nPhiClusWidthCnt_mon",tmp_phiClus);
      auto nEtaClusWidthCnt_mon = Monitored::Scalar<int>("nEtaClusWidthCnt_mon",tmp_etaClus);
      fill("CscPrdMonitor", nPhiClusWidthCnt_mon, nEtaClusWidthCnt_mon);
     // m_h2csc_prd_eta_vs_phi_cluswidth->Fill(nPhiClusWidthCnt[lcnt],nEtaClusWidthCnt[lcnt]);
    } // end loop over lcnt

    int numeta = 0, numphi = 0;
    int numetasignal = 0, numphisignal = 0;
    int tmp_val = 0;
    for(int kl = 1; kl < 33; kl++ ) {

      for(int km = 1; km < 9; km++ ) {
        int lay = (km > 4 && km < 9) ? km-4 : km;  // 1,2,3,4 (phi-layers)     5-4, 6-4, 7-4, 8-4 (eta-layers)
        bool mphi = (km > 0 && km < 5) ? true : false; // 1,2,3,4 (phi-layers) 5,6,7,8 (eta-layers)
        std::string wlay = mphi ? "Phi-Layer " : "Eta-Layer: ";

        int count = clusCount[kl][km];
        auto count_mon = Monitored::Scalar<int>("count_mon",count);
        int scount = sigclusCount[kl][km];
        auto scount_mon = Monitored::Scalar<int>("scount_mon",scount);

        auto mphi_true = Monitored::Scalar<int>("mphi_true",(int)mphi && count == 1);
        auto mphi_false = Monitored::Scalar<int>("mphi_false",(int)!(mphi) && count == 1 );

        auto scount_phi_true = Monitored::Scalar<int>("scount_phi_true", (int)mphi && count == 1 && scount == 1 );
        auto scount_phi_false = Monitored::Scalar<int>("scount_phi_false", (int)mphi && count == 1 && scount == 0 );

        auto scount_eta_true = Monitored::Scalar<int>("scount_eta_true", (int)!(mphi) && count == 1 && scount == 1 );
        auto scount_eta_false = Monitored::Scalar<int>("scount_eta_false", (int)!(mphi) && count == 1 && scount == 0 );

        auto secLayer = Monitored::Scalar<float>("secLayer",(kl-16 + 0.2 * (lay - 1) + 0.1));

        
        auto tmp_val_mon = Monitored::Scalar<int>("tmp_val_mon", tmp_val);

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
              secLayer << "\t = " << count << "\t" << scount);
        }//end count
        fill("CscPrdMonitor", count_mon, scount_mon, tmp_val_mon, secLayer, mphi_true, mphi_false, scount_phi_false, scount_phi_true, scount_eta_false, scount_eta_true );
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
  
  ATH_MSG_DEBUG ( " End EVENT======================" );

  ATH_MSG_DEBUG( "CscPrdValMonAlg: fillHistograms reports success" );

  return StatusCode::SUCCESS;
} // end CscPrdValMonAlg::fillHistograms()
