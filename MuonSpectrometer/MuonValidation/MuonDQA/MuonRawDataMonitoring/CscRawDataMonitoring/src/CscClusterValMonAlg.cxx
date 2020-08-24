/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/*   NAME    : CscClusterValMonAlg.cxx
 *   PACKAGE : MuonRawDataMonitoring/CscRawDataMonitoring
 *   PURPOSE : CSC cluster monitoring
 *   AUTHOR  : Venkatesh Kaushik <venkat.kaushik@cern.ch> (2009-04-27)
 *   
 *   MODIFIED: C.Paraskevopoulos, C.Kitsaki
 */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#include "AthenaMonitoring/AthenaMonManager.h"

// Athena include(s)
#include "MuonPrepRawData/CscClusterStatus.h"
#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "CscRawDataMonitoring/CscClusterValMonAlg.h"

// STL include(s)
#include <bitset>
#include <cmath>

using namespace Muon;

CscClusterValMonAlg::CscClusterValMonAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthMonitorAlgorithm(name,pSvcLocator)
  {
    declareProperty("CSCQmaxCutADC", m_qmaxADCCut = 100);
 
  //trigger aware monitoring
    declareProperty("CSCDoEventSelection",   m_doEvtSel = false );
    declareProperty("CSCEventSelTriggers", m_sampSelTriggers );
  }

StatusCode CscClusterValMonAlg::initialize() {
    

  ATH_MSG_INFO ( "Initializing        : " << name() );
  ATH_MSG_INFO ( "CSCClusterKey       : " << m_cscClusterKey );
  ATH_MSG_INFO ( "CSCPrepRawDataKey   : " << m_cscPRDKey );

  ATH_CHECK(m_idHelperSvc.retrieve());

  ATH_CHECK(m_stripFitter.retrieve());
  ATH_MSG_INFO ( "CSCStripFitter      : " << "Using Fitter with name \"" << m_stripFitter->name() << "\"" );

  if( m_doEvtSel ) ATH_CHECK(m_trigDecTool.retrieve());

  ATH_CHECK(m_cscCalibTool.retrieve());
  ATH_CHECK(m_cscClusterKey.initialize((m_idHelperSvc->hasCSC())));
  ATH_CHECK(m_cscPRDKey.initialize((m_idHelperSvc->hasCSC())));

  return AthMonitorAlgorithm::initialize();
}


StatusCode CscClusterValMonAlg::fillHistograms( const EventContext& ctx ) const {


  StatusCode sc = StatusCode::SUCCESS;

  // check if event passed sample-selection triggers
  //if(m_doEvtSel) { if(!evtSelTriggersPassed()) return sc; }

  bool check = false;

  if(!m_doEvtSel) check = true;
  std::vector<std::string>::const_iterator 
  it = m_sampSelTriggers.begin(), itE = m_sampSelTriggers.end();
  for ( ; it != itE; it++ ) {
    if (m_trigDecTool->isPassed(*it, TrigDefs::eventAccepted)) check = true;
  }

  if(m_doEvtSel) { if(!check) return sc; }
  
  // retrieve cluster / strip collection
  SG::ReadHandle<CscPrepDataContainer> cscCluster(m_cscClusterKey, ctx);
  SG::ReadHandle<CscStripPrepDataContainer> cscStrip(m_cscPRDKey, ctx);

  // we can do (some) monitoring plots with just the cluster
  // ideally we need both the cluster and the strips that make up that cluster  
  //FillCSCClusters(*cscCluster.cptr(), *cscStrip.cptr());

  if(!(cscCluster.isValid())) {
    ATH_MSG_ERROR("evtStore() does not contain csc prd Collection with name "<< m_cscClusterKey);
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG ( " Size of Cluster Collection : " << cscCluster->size() );
  ATH_MSG_DEBUG ( " Size of Strip Collection : " << cscStrip->size() );

  for ( CscPrepDataContainer::const_iterator Icol = cscCluster->begin(); Icol != cscCluster->end(); ++Icol )
  {
    const CscPrepDataCollection& clus = **Icol;

    // arrays to hold cluster-count  
    // 32 chambers and 8 layers (each has one extra - index '0' is not counted)
    int clusCount[33][9], sigclusCount[33][9];
    unsigned int nEtaClusWidthCnt = 0, nPhiClusWidthCnt = 0;
    for(unsigned int kl = 0; kl < 33; kl++ ) {
      for(unsigned int km = 0; km < 9; km++ ) {
        clusCount[kl][km] = 0;
        sigclusCount[kl][km] = 0;
      }
    }
  
    float stripsSum_EA = 0.;
    float stripsSum_EAtest = -50.;
    float stripsSum_EC = 0.;
    float stripsSum_ECtest = -50.;

    ATH_MSG_DEBUG ( " Begin loop over clusters ============================");
    for ( CscPrepDataCollection::const_iterator Itclu = clus.begin(); Itclu != clus.end(); ++Itclu ) 
    {
      const CscPrepData& iClus = **Itclu;
      const std::vector<Identifier>& stripIds = iClus.rdoList();    
      float clu_charge = iClus.charge();
      auto clu_time = Monitored::Scalar<float>("clu_time", (iClus.time()));

      ATH_MSG_DEBUG(" cluster charge = " << clu_charge << "\t cluster time = " << clu_time );

     // unsigned int noStrips = stripIds.size();  // no. of strips in this cluster = stripIds.size()
      auto noStrips = Monitored::Scalar<int> ("noStrips",stripIds.size());
      Identifier clusId = iClus.identify();

      // get the cluster coordinates
      int stationName = m_idHelperSvc->cscIdHelper().stationName(clusId);
      std::string stationString = m_idHelperSvc->cscIdHelper().stationNameString(stationName);
      int chamberType = stationString == "CSS" ? 0 : 1;
      int stationEta  = m_idHelperSvc->cscIdHelper().stationEta(clusId);
      int stationPhi  = m_idHelperSvc->cscIdHelper().stationPhi(clusId);
      int wireLayer = m_idHelperSvc->cscIdHelper().wireLayer(clusId);
      int measuresPhi = m_idHelperSvc->cscIdHelper().measuresPhi(clusId);


      auto x = Monitored::Scalar<float> ("x",iClus.globalPosition().x());
      auto y = Monitored::Scalar<float> ("y",iClus.globalPosition().y());
      auto z = Monitored::Scalar<float> ("z",iClus.globalPosition().z());
      auto r = Monitored::Scalar<float> ("r",std::hypot(x,y));

      fill("CscClusMonitor",z,r);
      fill("CscClusMonitor",y,x);

      // convert to my coordinates
      int sectorNo  = stationEta * (2 * stationPhi - chamberType);   // [-16 -> -1] and [+1 -> +16]
      auto secLayer = Monitored::Scalar<float> ("secLayer", (sectorNo + 0.2 * (wireLayer - 1) + 0.1) );
      int xfac = measuresPhi ? -1 : 1;        // [-1 -> -48] / [+1 -> +192]

      //total cluster width (EA and EC) calculation
       if(secLayer > 0.) { 
          stripsSum_EA = stripsSum_EA + noStrips;
     }
       if(stripsSum_EA > stripsSum_EAtest) {
          stripsSum_EAtest = stripsSum_EA;
     }

       if(secLayer < 0. || secLayer == 0.) { 
          stripsSum_EC = stripsSum_EC + noStrips;
     }
       if(stripsSum_EC > stripsSum_ECtest) {
          stripsSum_ECtest = stripsSum_EC;
     }  

      // compute the indices to store cluster count
      int ns = sectorNo < 0 ? sectorNo*(-1) : sectorNo+16; // [-16 -> -1] shifted to [1 -> 16] and [+1 -> +16] shifted to [+17 -> +32]
      int nl = (measuresPhi ? wireLayer : wireLayer+4);  // [ 1 -> 4] (phi-layers) and [5 -> 8] (eta-layers)

      // increment the cluster-count for this layer
      clusCount[ns][nl]++;

      // indices for ns = [+1 -> +32]; 32 places (index '0' is not counted); allocated 33 places
      // indices for nl = [+1 -> +8]; 8 places (index '0' is not counted); allocated 9 places
      ATH_MSG_DEBUG(" ns = " << ns << "\tm_nl = " << nl << "\tm_sec = " << sectorNo << "\t m_lay= " << wireLayer << "\tmPhi = " << measuresPhi);


      // check the cluster status; probably need to read status info from jobOptions - not done for the moment
      // status = Muon::CscStatusUnspoiled (i.e 0) or Muon::CscStatusSplitUnspoiled (i.e 10) are considered good for precision clusters
      // status = Muon::CscStatusSimple (i.e 1) could be good for non-precision clusters (i.e for phi-layers)
      std::string stat = Muon::toString(iClus.status());
      bool cluster_status = ( (stat == "unspoiled")                 || 
          (stat == "unspoiled with split")                            ||
          (stat == "simple")
          ) ? true : false;

      // Also need at least three strips in an eta-cluster to compute Q_max, Q_left and Q_right
      bool eta_cluster_status = cluster_status && ( noStrips > 2 ) && (measuresPhi == 0); 

      // Also need at least one strip in a phi-cluster to compute Q_max = Q_sum
      bool phi_cluster_status = cluster_status && ( noStrips > 0 ) && (measuresPhi == 1);

      ATH_MSG_DEBUG ( " ClusterStatus eta = " << eta_cluster_status << " ,phi = " << phi_cluster_status);
      ATH_MSG_DEBUG ( " ClusterID (eta:" << stationEta << ",phi:" << stationPhi << ",type:" << chamberType << ", measPhi: " 
          << measuresPhi << ",wire:" << wireLayer << ") = " << secLayer << " status = " 
          << stat << " #of strips = " << noStrips );

      // if cluster is okay get Qmax, Qleft, Qright and Qsum = (Qmax + Qleft + Qright)
      if(eta_cluster_status || phi_cluster_status ) {
        const CscStripPrepDataCollection* pcol(0);
        bool found_id = true;
        std::vector <const CscStripPrepData*> stripVec;
        std::vector <float> fStripIDs;
      //  auto fStripIDs = Monitored::Collection("fStripIDs",fStripIDs);
        float maxStripCharge = 0., maxStipId = 0.;
        int sIdx = 0, mxIdx = 0; // index-counter and index of max strip in the vector of Id's

        auto clus_phi = Monitored::Scalar<int>("clus_phi", (int)measuresPhi);
        auto clus_eta = Monitored::Scalar<int>("clus_eta", (int)(!measuresPhi));
        fill("CscClusMonitor",noStrips, secLayer,clus_phi, clus_eta);

      // fill cluster width (no. of strips per cluster) 
        if(measuresPhi) {
          nPhiClusWidthCnt++;
        } else {
          nEtaClusWidthCnt++;
        }

        // Loop over strip id's vector / strip collection and match the id's from vector with strips in collection
        for ( std::vector<Identifier>::const_iterator sId = stripIds.begin(); sId != stripIds.end(); ++sId, sIdx++ ) {
          Identifier id = *sId; // for strip Id's
          int thisStrip = m_idHelperSvc->cscIdHelper().strip(id);
          auto stripid = Monitored::Scalar<int> ("stripid", thisStrip*xfac);// x-axis fill value
          fStripIDs.push_back(stripid);
          
          fill("CscClusMonitor",stripid, secLayer);

          if(!pcol) {
            const CscStripPrepDataCollection* icol = cscStrip->indexFindPtr(clus.identifyHash());
            if ( icol == nullptr ) {
              found_id = false;
              break;  // could not identify the strips
            } else {
              pcol = icol;
            }
          } // end if !pcol  

          bool found_strip = false;
          float maxsampChVal = 0.;
          if(found_id) {
            for ( CscStripPrepDataCollection::const_iterator istrip= pcol->begin(); istrip != pcol->end(); ++ istrip ) {
              found_strip = ( *istrip )->identify() == id ; 
              if(found_strip) {
                stripVec.push_back(*istrip);
                std::vector<float> samp_charges = ( *istrip )->sampleCharges();
                for(unsigned int i = 0; i < samp_charges.size(); i++ ) {
                  if(samp_charges[i] > maxsampChVal) maxsampChVal = samp_charges[i];
                }
                if(maxsampChVal > maxStripCharge ) {
                  maxStripCharge = maxsampChVal; 
                  maxStipId = stripid;
                  mxIdx = sIdx;
                }
                break; // break from inner loop
              }
            } // end for loop on strip collection
            ATH_MSG_DEBUG ( " " << (found_strip? "FoundStrip " : "NoStripFound ") << " with max sampling = " << maxsampChVal);
          } // end if found_id
        }

         auto fStripIDs_col = Monitored::Collection("fStripIDs_col",fStripIDs);
         ATH_MSG_DEBUG ( " Max Strip charge = " << maxStripCharge  << " and strip Id = " << maxStipId << " and index = " << mxIdx);
         float qmax = 0., qleft = 0., qright = 0., qsum = 0.;
        // if we are here and loop over strips is successful we should have found_id = true
        // and the size of strip-ID-vector == size of strips-vector
        bool size_ids_coll = (noStrips == (int)stripVec.size() ? true : false) ;

        if(found_id && size_ids_coll ) {
          // store results of three strips (Qmax, Qleft, Qright)
          std::vector<ICscStripFitter::Result> res;
          res.reserve(3);
          bool range_check = (mxIdx > -1) && (mxIdx < int(noStrips));

          ATH_MSG_DEBUG ( " Range check = (" << mxIdx  << " > -1 ) && (" << mxIdx << " < " << noStrips << " ) = " << range_check
              << "\t size of vec check " << noStrips << " == " << stripVec.size());

          if( range_check ) {
            // fit Q_left fit
            if(mxIdx-1 >= 0  ) { 
              res[0] = m_stripFitter->fit(*stripVec[mxIdx-1]); 
              qleft = res[0].charge;
              qsum += qleft;
              ATH_MSG_DEBUG ( " Left Strip q +- dq = " << res[0].charge  << " +- " << res[0].dcharge << "\t t +- dt = " 
                  << res[0].time << " +- " <<  res[0].dtime << "\t w +- dw = " << res[0].width << " +- " 
                  << res[0].dwidth << "\t status= " << res[0].status << "\t chisq= " << res[0].chsq);
            }// end if q_left
            // fit Q_max strip
            res[1] = m_stripFitter->fit(*stripVec[mxIdx]);
            qmax = res[1].charge;
            qsum += qmax; 
            ATH_MSG_DEBUG ( " Peak Strip q +- dq = " << res[1].charge  << " +- " << res[1].dcharge << "\t t +- dt = " 
                << res[1].time << " +- " <<  res[1].dtime << "\t w +- dw = " << res[1].width << " +- " 
                << res[1].dwidth << "\t status= " << res[1].status << "\t chisq= " << res[1].chsq);
            // fit Q_right strip
            if(mxIdx+1 < int(noStrips)) {
              res[2] = m_stripFitter->fit(*stripVec[mxIdx+1]);
              qright = res[2].charge;
              qsum += qright;
              ATH_MSG_DEBUG ( " Right Strip q +- dq = " << res[2].charge  << " +- " << res[2].dcharge << "\t t +- dt = " 
                  << res[2].time << " +- " <<  res[2].dtime << "\t w +- dw = " << res[2].width << " +- " 
                  << res[2].dwidth << "\t status= " << res[2].status << "\t chisq= " << res[2].chsq);
            } // end if q_right
          } // end if range_check

          // not used at the moment
          // 1 e = 1.602176487 10^{-19} C = 1.6022 x 10^{-4} fC
          // float m_fCperElectron = 1.6022e-4; // multiply # of electrons by this number to get fC

          float kiloele = 1.0e-3; // multiply # of electrons by this number to get kiloElectrons (1 ke = 1 ADC)


          // Assume 1000 e = 1 ADC for now = 1000 x 1.6022 x 10^{-4} fC = 0.16022 fC
          // convert qmax, qleft, qright into ADC 
          
          auto QmaxADC = Monitored::Scalar<float>("QmaxADC", (qmax * kiloele));
          auto QsumADC = Monitored::Scalar<float>("QsumADC", (qsum * kiloele));
          

          // check if signal or noise
          // QmaxADC > m_qmaxADCCut is signal
          bool signal = QmaxADC > m_qmaxADCCut;

          // fill signal/noise histograms
          auto signal_mon = Monitored::Scalar<int>("signal_mon",(int)signal);
          auto noise_mon = Monitored::Scalar<int>("noise_mon",(int)!(signal));
          for(unsigned int j =0; j < fStripIDs.size(); j++)
          {
            fill("CscClusMonitor",fStripIDs_col, secLayer, signal_mon, noise_mon);
          }
          auto clus_phi = Monitored::Scalar<int>("clus_phi", (int)measuresPhi );
          auto clus_eta = Monitored::Scalar<int>("clus_eta", (int)(!measuresPhi) );
          auto clus_phiSig = Monitored::Scalar<int>("clus_phiSig", (int)measuresPhi && (signal));
          auto clus_etaSig = Monitored::Scalar<int>("clus_etaSig", (int)(!measuresPhi) && (signal));
          auto clus_phiNoise = Monitored::Scalar<int>("clus_phiNoise", (int)measuresPhi && !(signal));
          auto clus_etaNoise = Monitored::Scalar<int>("clus_etaNoise", (int)(!measuresPhi) && !(signal));

          auto sideA = Monitored::Scalar<int>("sideA",(int)(stationEta==1) && (signal));
          auto sideC = Monitored::Scalar<int>("sideC",(int)(stationEta==-1) && (signal));
          fill("CscClusMonitor",secLayer,sideA,sideC);

          if(signal)  sigclusCount[ns][nl]++;

          auto clu_charge_kiloele = Monitored::Scalar<float>("clu_charge_kiloele", (iClus.charge()*kiloele));

          fill("CscClusMonitor",QmaxADC, secLayer, noStrips, secLayer, QsumADC, clu_time, clu_charge_kiloele, clus_phi, clus_eta, clus_phiSig, clus_etaSig, clus_phiNoise, clus_etaNoise, signal_mon, noise_mon, sideA, sideC);

          ATH_MSG_DEBUG ( " End of strip fits " ); 

        } // if found_id
      } // end if cluster_status

      auto stripsSum_EA_mon = Monitored::Scalar<float> ("stripsSum_EA_mon",stripsSum_EA);
      auto stripsSum_EC_mon = Monitored::Scalar<float> ("stripsSum_EC_mon",stripsSum_EC);
      fill("CscClusMonitor",stripsSum_EA_mon, stripsSum_EC_mon);

    } // end for loop over prep-data collection
    ATH_MSG_DEBUG ( " End loop over clusters ============================");
    auto nPhiClusWidthCnt_mon = Monitored::Scalar<int> ("nPhiClusWidthCnt_mon",nPhiClusWidthCnt);
    auto nEtaClusWidthCnt_mon = Monitored::Scalar<int> ("nEtaClusWidthCnt_mon",nEtaClusWidthCnt);
    fill("CscClusMonitor",nPhiClusWidthCnt_mon,nEtaClusWidthCnt_mon);

    // Fill cluster counts
    int numeta = 0, numphi = 0;
    int numetasignal = 0, numphisignal = 0;

    //loop over chambers
    for(int kl = 1; kl < 33; kl++ ) {

      // loop over layers
      int eta_hits[4] = {0,0,0,0};
      bool chamber_empty = true;  
      int sec = kl < 17 ? kl*(-1) : kl; // [1->16](-side)  [17-32] (+side)
      for(int km = 1; km < 9; km++ ) {
        int lay = (km > 4 && km < 9) ? km-4 : km;  // 1,2,3,4 (phi-layers)     5-4, 6-4, 7-4, 8-4 (eta-layers)
        bool mphi = (km > 0 && km < 5) ? true : false; // 1,2,3,4 (phi-layers) 5,6,7,8 (eta-layers)
        std::string wlay = mphi ? "Phi-Layer " : "Eta-Layer: ";
        int count = clusCount[kl][km];
        auto count_mon = Monitored::Scalar<int>("count_mon",count);
        int scount = sigclusCount[kl][km];
        auto scount_mon = Monitored::Scalar<int>("scount_mon",scount);
        auto count_diff = Monitored::Scalar<int>("count_diff",(count-scount));

        auto mphi_true = Monitored::Scalar<int>("mphi_true",(int)mphi && count == 1);
        auto mphi_false = Monitored::Scalar<int>("mphi_false",(int)!(mphi) && count == 1);

        auto scount_phi_true = Monitored::Scalar<int>("scount_phi_true", (int)mphi && count == 1 && scount == 1);
        auto scount_phi_false = Monitored::Scalar<int>("scount_phi_false", (int)mphi && count == 1 && scount == 0);

        auto scount_eta_true = Monitored::Scalar<int>("scount_eta_true", (int)!(mphi) && count == 1 && scount == 1);
        auto scount_eta_false = Monitored::Scalar<int>("scount_eta_false", (int)!(mphi) && count == 1 && scount == 0);

        auto secLayer = Monitored::Scalar<float>("secLayer",(sec + 0.2 * (lay - 1) + 0.1));

        if(count){
          ATH_MSG_DEBUG ("sec[" << sec << "]\t" << wlay << "[" << lay << "] = " << secLayer << "= " << "\tNsig = " << scount << ", Ntot = " << count);
          if(mphi){
            numphi += count;
            if(scount){
              chamber_empty = false;
              numphisignal += scount;
            }
          }
          else{
            numeta += count;
            if(scount){
              eta_hits[lay-1]++;
              chamber_empty = false;
              numetasignal +=scount;
            }
          }
          ATH_MSG_DEBUG ( wlay << "Counts sec: [" << kl-16 << "]\tlayer: [" << km << "] = " << secLayer << "\t = " << count << "\t" << scount);
        }

        fill("CscClusMonitor", count_mon, scount_mon, count_diff, secLayer, mphi_true, mphi_false, scount_phi_true, scount_phi_false, scount_eta_true, scount_eta_false);

      }// end loop over layers

      int segNum_new = -999.;
      if(!chamber_empty){
        std::ostringstream nseglist;
        std::bitset<4> segNum;
        for(unsigned int mm = 0; mm < 4; mm++) {
          bool set = (eta_hits[mm] > 0 ? true : false);
          if(set) segNum.set(mm);
          nseglist << (set ? "1" : "0");
        }
        segNum_new = segNum.to_ulong();
        ATH_MSG_DEBUG("segments= " << nseglist.str() << "\t = " << segNum.to_ulong());  
      }
      auto segNum_mon = Monitored::Scalar<int>("segNum_mon", segNum_new);
      auto sec_mon = Monitored::Scalar<float>("sec_mon",sec+0.3);
      fill("CscClusMonitor", segNum_mon, sec_mon);
    } // end loop over chambers

    ATH_MSG_DEBUG(" numphi = " << numphi << "\t numeta = " << numeta << "\tm_sphi = " << numphisignal << "\t m_seta = " << numetasignal);
    auto numphi_mon = Monitored::Scalar<int>("numphi_mon", numphi);
    auto numeta_mon = Monitored::Scalar<int>("numeta_mon", numeta);
    auto numphi_sig_mon = Monitored::Scalar<int>("numphi_sig_mon", numphisignal);
    auto numeta_sig_mon = Monitored::Scalar<int>("numeta_sig_mon", numetasignal);
    auto numphi_numeta_mon = Monitored::Scalar<int>("numphi_numeta_mon", numphi+numeta);
    auto numphi_numeta_sig_mon = Monitored::Scalar<int>("numphi_numeta_sig_mon", numphisignal+numetasignal);
    auto num_num_noise_mon = Monitored::Scalar<int>("num_num_noise_mon", (numphi-numphisignal)+(numeta-numetasignal));
    auto numphi_diff_mon = Monitored::Scalar<int>("numphi_diff_mon", numphi-numphisignal);
    auto numeta_diff_mon = Monitored::Scalar<int>("numeta_diff_mon", numeta-numetasignal);

    fill("CscClusMonitor",numphi_mon,numeta_mon,numphi_sig_mon,numeta_sig_mon,numphi_numeta_mon,numphi_numeta_sig_mon,num_num_noise_mon,numphi_diff_mon,numeta_diff_mon);

  } // end for loop over prep-data container

  ATH_MSG_DEBUG ( " END EVENT ============================");

  return sc; 

}
