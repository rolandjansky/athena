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
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonPrepRawData/CscClusterStatus.h"
#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "CscRawDataMonitoring/CscClusterValMonAlg.h"

// ROOT include(s)
#include "TH1F.h"
#include "TH2F.h"
#include "TClass.h"

// STL include(s)
#include <bitset>
#include <math.h>

using namespace Muon;

CscClusterValMonAlg::CscClusterValMonAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthMonitorAlgorithm(name,pSvcLocator),
  m_stripFitter("CalibCscStripFitter/CalibCscStripFitter"),
  m_cscCalibTool("CscCalibTool/CscCalibTool"),
  m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool")
  {
    declareProperty("CSCQmaxCutADC", m_qmaxADCCut = 100);
    declareProperty("CSCStripFitter", m_stripFitter);
    declareProperty("CSCCalibTool", m_cscCalibTool);

  //trigger aware monitoring
    declareProperty("CSCTrigDecisionTool", m_trigDec );
    declareProperty("CSCDoEventSelection",   m_doEvtSel = false );
    declareProperty("CSCEventSelTriggers", m_sampSelTriggers );
  }



CscClusterValMonAlg::~CscClusterValMonAlg() {}


StatusCode CscClusterValMonAlg::initialize() {
    

  ATH_MSG_INFO ( "Initializing        : " << name() );
  ATH_MSG_INFO ( "CSCClusterKey       : " << m_cscClusterKey );
  ATH_MSG_INFO ( "CSCPrepRawDataKey   : " << m_cscPRDKey );

  StatusCode sc;
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_MSG_DEBUG ("CSCIdHelper         : " << "Using CscIdhelper " );

  ATH_CHECK(m_stripFitter.retrieve());
  ATH_MSG_INFO ( "CSCStripFitter      : " << "Using Fitter with name \"" << m_stripFitter->name() << "\"" );

  //ATH_CHECK(m_cscCalibTool.retrieve());

  if( m_doEvtSel ) ATH_CHECK(m_trigDec.retrieve());

  ATH_CHECK(m_cscCalibTool.retrieve());
  ATH_CHECK(m_cscClusterKey.initialize((m_idHelperSvc->hasCSC())));
  ATH_CHECK(m_cscPRDKey.initialize((m_idHelperSvc->hasCSC())));

  return AthMonitorAlgorithm::initialize();
}


StatusCode CscClusterValMonAlg::fillHistograms( const EventContext& ctx ) const {


  StatusCode sc = StatusCode::SUCCESS;

  // check if event passed sample-selection triggers
  //if(m_doEvtSel) { if(!evtSelTriggersPassed()) return sc; }

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
      float clu_time = iClus.time();

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
      auto r = Monitored::Scalar<float> ("r",sqrt(x*x + y*y));

      fill("CscClusMonitor",z,r);
      fill("CscClusMonitor",y,x);

      // convert to my coordinates
      int sectorNo  = stationEta * (2 * stationPhi - chamberType);   // [-16 -> -1] and [+1 -> +16]
      auto secLayer = Monitored::Scalar<float> ("secLayer", (sectorNo + 0.2 * (wireLayer - 1) + 0.1) );
      auto secLayerPhi = Monitored::Scalar<float> ("secLayerPhi", (sectorNo + 0.2 * (wireLayer - 1) + 0.1) );
      auto secLayerEta = Monitored::Scalar<float> ("secLayerEta", (sectorNo + 0.2 * (wireLayer - 1) + 0.1) );
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
        float maxStripCharge = 0., maxStipId = 0.;
        int sIdx = 0, mxIdx = 0; // index-counter and index of max strip in the vector of Id's


      // fill cluster width (no. of strips per cluster) 
        if(measuresPhi) {
          fill("CscClusMonitor",noStrips,secLayerPhi);  // fill phi-cluster width
          nPhiClusWidthCnt++;
        } else {
          fill("CscClusMonitor",noStrips,secLayerEta);  // fill eta-cluster width
          nEtaClusWidthCnt++;
        }

        // Loop over strip id's vector / strip collection and match the id's from vector with strips in collection
        for ( std::vector<Identifier>::const_iterator sId = stripIds.begin(); sId != stripIds.end(); ++sId, sIdx++ ) {
          Identifier id = *sId; // for strip Id's
          int thisStrip = m_idHelperSvc->cscIdHelper().strip(id);
          auto stripid = Monitored::Scalar<int> ("stripid", thisStrip*xfac);
        //  float stripid = thisStrip * xfac;         // x-axis fill value
          fStripIDs.push_back(stripid);
          fill("CscClusMonitor",stripid, secLayer);

          if(!pcol) {
            CscStripPrepDataContainer::const_iterator icol = cscStrip->indexFind(clus.identifyHash());
            if ( icol == cscStrip->end() ) {
              found_id = false;
              break;  // could not identify the strips
            } else {
              pcol = *icol;
              if(!pcol) found_id = false;
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
            //if(stripVec.size()>0) res[0] = m_stripFitter->fit(*stripVec[mxIdx-1]);
            
            if(mxIdx-1 >= 0  ) {
             
            //  res[0] = m_stripFitter->fit(*stripVec[mxIdx-1]);  //python crashes 
            }
          }

        }
      }
    }
  }

  return sc; 

}


