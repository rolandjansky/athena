/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "AthenaMonitoring/AthenaMonManager.h"

#include "CscRawDataMonitoring/CSCSegmValMonAlg.h"

#include "MuonPrepRawData/CscClusterStatus.h"
#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonPrepRawData/CscPrepData.h"

// Track
#include "TrkTrack/TrackCollection.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkSurfaces/PlaneSurface.h"

#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"

// STL
#include <sstream>
#include <utility>
#include "GaudiKernel/SystemOfUnits.h"

//________________________________________________________________________________________________________
CSCSegmValMonAlg::CSCSegmValMonAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthMonitorAlgorithm(name,pSvcLocator) 
  { }


//________________________________________________________________________________________________________
StatusCode CSCSegmValMonAlg::initialize() {

  ATH_MSG_INFO( "in CSCSegmValMonAlg::init()" );
  if( m_doEvtSel ) ATH_CHECK(m_trigDecTool.retrieve());

  if(m_sampSelTriggers.empty() && m_doEvtSel) {
    ATH_MSG_WARNING("Event selection triggers not specified. Switching off trigger-aware monitoring");
    m_doEvtSel = false;
  }

  StoreGateSvc* detStore = nullptr;
  ATH_CHECK(service("DetectorStore", detStore));
  ATH_CHECK(m_edmHelperSvc.retrieve());
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_segmKey.initialize());

  return AthMonitorAlgorithm::initialize();
}  

//
// fillHistograms-----------------------------------------------------------------------------------------
//

StatusCode CSCSegmValMonAlg::fillHistograms(const EventContext& ctx) const{

  ATH_MSG_DEBUG( "in CSCSegmValMonAlg::fillHistograms()                             " );

  if( m_environment == AthMonitorAlgorithm::Environment_t::tier0 || m_environment == AthMonitorAlgorithm::Environment_t::tier0ESD ||  m_environment == AthMonitorAlgorithm::Environment_t::user ) {

    // if required, check if event passed sample-selection triggers
    if(m_doEvtSel && !evtSelTriggersPassed()) return StatusCode::SUCCESS; 

    // Segment Cluster counter
    int segmClustCount[33];

    // arrays to hold cluster-count
    // 32 chambers and 8 layers (each has one extra - index '0' is not counted)
    int clusCount[33][9];
    for(unsigned int kl = 0; kl < 33; kl++ ) {
      for(unsigned int cm3 = 0; cm3 < 9; cm3++ ){
        clusCount[kl][cm3] = 0;
      }
    }

    SG::ReadHandle<Trk::SegmentCollection> segments(m_segmKey, ctx);

    if ( segments->empty() ){
      ATH_MSG_DEBUG( "      Segm Collection is Empty, done...    ");
      return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG( "      Segm Collection size         " );
    ATH_MSG_DEBUG( "Number of segms found: " << segments->size() );

    if(segments->size() > 0){
      ATH_MSG_DEBUG(   "Number of segms in event is: " << segments->size()                               );
      ATH_MSG_DEBUG(  "This algorithm is designed to operate for single segm / event only"             );
      ATH_MSG_DEBUG(  "Processing only the first segm in the segm collection"                         );
    }

    int segnum = 0; 
    int layerindex = 0; 

    //Loop over segms
    for (Trk::SegmentCollection::const_iterator s = segments->begin(); s != segments->end(); ++s) {

      //Get segm
      const Muon::MuonSegment *segm=dynamic_cast<const Muon::MuonSegment*>(*s);

      if (!segm){
        ATH_MSG_WARNING( "no pointer to segm!!!" );
        continue;
      }

      ATH_MSG_DEBUG( "Looking at segment id" );
      // Skip segment if there are no csc hits
      if ( cscHits(segm)<1 ) continue;

      // Get contained measurements
      const std::vector<const Trk::MeasurementBase*> meas = segm->containedMeasurements();

      // Skip segment if no measurements
      // Number of clusters on segment
      auto n_clust = Monitored::Scalar<int>("n_clust", meas.size());

      if ( n_clust < 2 ) continue;

      // Analyze segment if it is a csc segment
      if ( isCscSegment(segm) ){
        
        segnum++;
        
        // Initialize cluster counter
        for(int sect = 0; sect < 33; sect++) {
          segmClustCount[sect] = 0;
          for(unsigned int ilay = 0; ilay < 9; ilay++ ){
            clusCount[sect][ilay] = 0;
          }
        }

        // Print out segment information
        const Trk::FitQuality* fq = segm->fitQuality();
        double chi2 = 999.;
        int ndof = -1;
        if( fq ) {
          chi2 = fq->chiSquared();
          ndof = fq->numberDoF();
          ATH_MSG_DEBUG( "Chi2 " << chi2 );
          ATH_MSG_DEBUG( "Ndof " << ndof );
        }

        // cut on segment angle
        float segm_ly = segm->localParameters()[Trk::locY];
        float segm_ayz = segm->localDirection().angleYZ();
        segm_ayz -= M_PI/2.;
        float segm_cut = m_segmSlope;
        bool segmAngle_cut = segmSlopeCut(segm_ly, segm_ayz, segm_cut);
        ATH_MSG_DEBUG(" local_pos: " << segm_ly << "\tangle_yz: " << segm_ayz << "\tcut: " << segm_cut << "\t pass = " << segmAngle_cut );

        ATH_MSG_DEBUG( "R " << segm->globalPosition().perp() );
        ATH_MSG_DEBUG( "Z " << segm->globalPosition().z() );
        ATH_MSG_DEBUG( "Phi " << segm->globalPosition().phi() );
        ATH_MSG_DEBUG( "Eta " << segm->globalPosition().eta() );
        ATH_MSG_DEBUG( "Dir Phi " << segm->globalDirection().phi() );
        ATH_MSG_DEBUG( "Dir Eta " << segm->globalDirection().eta() ); 

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
  
        // identify the segment location
        const Trk::MeasurementBase* rio = meas.at(0);
        Identifier segmId = m_edmHelperSvc->getIdentifier(*rio);

        int segm_stationPhi  = m_idHelperSvc->cscIdHelper().stationPhi(segmId);
        int segm_stationEta  = m_idHelperSvc->cscIdHelper().stationEta(segmId);
        int segm_stationName = m_idHelperSvc->cscIdHelper().stationName(segmId);
        std::string segm_stationString = m_idHelperSvc->cscIdHelper().stationNameString(segm_stationName);
        int segm_chamberType = Muon::MuonStationIndex::CSS == segm_stationName ? 0 : 1;
        auto segm_sectorNo = Monitored::Scalar<int>("segm_sectorNo", (segm_stationEta * (2 * segm_stationPhi - segm_chamberType)) ); // [-16 -> -1] and [+1 -> +16]
        int segm_isec = segm_sectorNo < 0 ? segm_sectorNo*(-1) : segm_sectorNo+16; // [-16 -> -1] shifted to [1 -> 16] and [+1 -> +16] shifted to [+17 -> +32]
        ATH_MSG_DEBUG(" sgsec = " << segm_isec << "\tsec = " << segm_sectorNo);
        auto sideA = Monitored::Scalar<int>("sideA", (int) (segm_stationEta==1));
        auto sideC = Monitored::Scalar<int>("sideC",(int)(segm_stationEta==-1));
        fill("CscSegmMonitor",n_clust,segm_sectorNo,sideA,sideC);

        float clus_kiloele = 1.0e-3; // multiply # of electrons by this number to get kiloElectrons (1 ke = 1 ADC)
        int eta_clus_count[2][2] = {{0, 0},{0, 0}}, phi_clus_count[2][2] = {{0, 0},{0, 0}}; // no. of prec/trans hits per segment
        float eta_clus_qsum[2][5] = {{-1., -1.}, {-1., -1., -1., -1., -1.}},  phi_clus_qsum[2][5] = {{-1., -1.}, {-1., -1., -1., -1., -1.}}; // qsum over each prec/trans. layer on segment
        float eta_clus_time[2][5] = {{-1., -1.}, {-1., -1., -1., -1., -1.}},  phi_clus_time[2][5] = {{-1., -1.}, {-1., -1., -1., -1., -1.}}; // time over each prec/trans. layer on segment
        int eta_clus_use[2][5] = {{0, 0},{0,0,0,0,0}}, phi_clus_use[2][5] = {{0,0}, {0,0,0,0,0}};

        layerindex = 0;

        for( auto& hit : segm->containedMeasurements()) {
          const Muon::CscClusterOnTrack* clust_rot = dynamic_cast<const Muon::CscClusterOnTrack*>(hit);
          if ( clust_rot ) {
            Identifier clusId = m_edmHelperSvc->getIdentifier(*clust_rot);

            // get the cluster coordinates
            int clus_stationName = m_idHelperSvc->cscIdHelper().stationName(clusId);
            int clus_chamberType = Muon::MuonStationIndex::CSS == clus_stationName ? 0 : 1;
            int clus_stationEta  = m_idHelperSvc->cscIdHelper().stationEta(clusId);
            int clus_stationPhi  = m_idHelperSvc->cscIdHelper().stationPhi(clusId);
            int clus_wireLayer = m_idHelperSvc->cscIdHelper().wireLayer(clusId);
            int clus_measuresPhi = m_idHelperSvc->cscIdHelper().measuresPhi(clusId);

            // convert to my coordinates
            int clus_sectorNo  = clus_stationEta * (2 * clus_stationPhi - clus_chamberType);   // [-16 -> -1] and [+1 -> +16]
            auto clus_secLayer = Monitored::Scalar<float>("clus_secLayer", clus_sectorNo + 0.2 * (clus_wireLayer - 1) + 0.1);
            int clus_isec = clus_sectorNo < 0 ? clus_sectorNo*(-1) : clus_sectorNo+16; // [-16 -> -1] shifted to [1 -> 16] and [+1 -> +16] shifted to [+17 -> +32]
            int clus_ilay = (clus_measuresPhi ? clus_wireLayer : clus_wireLayer+4);

            // check the cluster status; probably need to read status info from jobOptions - not done for the moment
            Muon::CscClusterStatus status = clust_rot->status();
            auto status_mon = Monitored::Scalar<int>("status_mon",(int)clust_rot->status());
            auto sideA_phi0 = Monitored::Scalar<int>("sideA_phi0",(int) (segm_stationEta==1)&& (int)(clus_measuresPhi==0));
            auto sideC_phi0 = Monitored::Scalar<int>("sideC_phi0",(int) (segm_stationEta==-1) && (int)(clus_measuresPhi == 0));
            auto sideA_phi = Monitored::Scalar<int>("sideA_phi", (int) (segm_stationEta==1) && (int)(clus_measuresPhi!=0));
            auto sideC_phi = Monitored::Scalar<int>("sideC_phi", (int) (segm_stationEta==-1) && (int)(clus_measuresPhi!=0));

            fill("CscSegmMonitor",status_mon, sideA_phi0, sideC_phi0, sideA_phi, sideC_phi);

            std::string clus_stat = Muon::toString(status);
            bool clus_status = ( (int(status)==Muon::CscStatusUnspoiled) || (int(status)==Muon::CscStatusSplitUnspoiled) || (int(status)==Muon::CscStatusSimple)) ? true : false;
            bool clus_stat_eff = ( (int(status) >= 0 && int(status) < 8 ) || (int(status) > 8 && int(status) < 18) ) ? true : false;

            // get cluster 
            const Muon::CscPrepData* theClus = clust_rot->prepRawData();
            float clus_qsum = 0, clus_time = -1.;

            if(theClus){
              clus_qsum  = theClus->charge() * clus_kiloele;
              clus_time = theClus->time();

              if(clus_measuresPhi == 0){
                if(clus_stationEta == 1) eta_clus_count[0][0]++;
                else eta_clus_count[1][0]++;
              } else{
                if(clus_stationEta == 1) phi_clus_count[0][0]++;
                else phi_clus_count[1][0]++;
              }
            }

            auto clus_qsum_mon = Monitored::Scalar<float>("clus_qsum_mon",clus_qsum);
            auto clus_time_mon = Monitored::Scalar<float>("clus_time_mon",clus_time);

            // get no. of strips per cluster
            unsigned int clus_noStrips = theClus->rdoList().size();

            //need at least three strips in an eta-cluster
            bool clus_eta_status = clus_status && ( clus_noStrips > 2 ) && (clus_measuresPhi == 0);
            bool clus_eta_eff = clus_stat_eff && ( clus_noStrips > 2 ) && (clus_measuresPhi == 0);
            if(clus_eta_eff){
              if(clus_stationEta == 1) eta_clus_count[0][1]++;
              else phi_clus_count[1][1]++;
            }
            if(clus_eta_status){
              if(clus_stationEta == 1){
                eta_clus_qsum[0][clus_wireLayer] = clus_qsum;
                eta_clus_time[0][clus_wireLayer] = clus_time;
                eta_clus_use[0][clus_wireLayer] = 1;
              }
              else{
                eta_clus_qsum[1][clus_wireLayer] = clus_qsum;
                eta_clus_time[1][clus_wireLayer] = clus_time;
                eta_clus_use[1][clus_wireLayer] = 1;
              }
            }

            // need at least one strip in a phi-cluster
            bool clus_phi_status = clus_status && ( clus_noStrips > 0 ) && (clus_measuresPhi == 1);
            if(clus_phi_status) {
              if(clus_stationEta==1){
                phi_clus_qsum[0][clus_wireLayer] = clus_qsum;
                phi_clus_time[0][clus_wireLayer] = clus_time;
                phi_clus_use[0][clus_wireLayer] = 1;
                phi_clus_count[0][1]++;
              } else {
                phi_clus_qsum[1][clus_wireLayer] = clus_qsum;
                phi_clus_time[1][clus_wireLayer] = clus_time;
                phi_clus_use[1][clus_wireLayer] = 1;
                phi_clus_count[1][1]++;
              }
            }


            auto checkStatusEtaA = Monitored::Scalar<int>("checkStatusEtaA", (int)clus_eta_status && (int)(clus_stationEta==1));
            auto checkStatusEtaC = Monitored::Scalar<int>("checkStatusEtaC", (int)clus_eta_status && (int)(clus_stationEta==-1));
            auto checkTimeEtaA = Monitored::Scalar<int>("checkTimeEtaA", (int)clus_eta_status && (int)(clus_stationEta==1) && (int) (std::abs(clus_time) <= 200));
            auto checkTimeEtaC = Monitored::Scalar<int>("checkTimeEtaC", (int)clus_eta_status && (int)(clus_stationEta==-1) && (int) (std::abs(clus_time) <= 200));
          
            auto checkStatusPhiA = Monitored::Scalar<int>("checkStatusPhiA", (int)clus_phi_status && (int)(clus_stationEta==1));
            auto checkStatusPhiC = Monitored::Scalar<int>("checkStatusPhiC", (int)clus_phi_status && (int)(clus_stationEta==-1));
            auto checkTimePhiA = Monitored::Scalar<int>("checkTimePhiA", (int)clus_phi_status && (int)(clus_stationEta==1) && (int) (std::abs(clus_time) <= 200));
            auto checkTimePhiC = Monitored::Scalar<int>("checkTimePhiC", (int)clus_phi_status && (int)(clus_stationEta==-1) && (int) (std::abs(clus_time) <= 200));
            fill("CscSegmMonitor", clus_qsum_mon, clus_secLayer, clus_time_mon, checkStatusEtaA, checkStatusEtaC, checkTimeEtaA, checkTimeEtaC, checkStatusPhiA, checkStatusPhiC, checkTimePhiA, checkTimePhiC );

            // increment the cluster-count for this layer
            if(clus_eta_status || clus_phi_status) clusCount[clus_isec][clus_ilay]++;

            // increment segment cluster count
            if(clus_eta_status) segmClustCount[clus_isec]++;

            if(clus_eta_status) layerindex+=clus_wireLayer;

            ATH_MSG_DEBUG("status = " << clus_stat << "\tcharge = " << clus_qsum << "\ttime= " << clus_time << "\tnstrips = " << clus_noStrips);


          } // if clust_rot

        } // for loop over clusters

        auto tmp_etaClusA = Monitored::Scalar<int>("tmp_etaClusA", eta_clus_count[0][0]);
        auto tmp_etaClusGoodA = Monitored::Scalar<int>("tmp_etaClusGoodA", eta_clus_count[0][1]);

        auto tmp_phiClusA = Monitored::Scalar<int>("tmp_phiClusA", phi_clus_count[0][0]);
        auto tmp_phiClusGoodA = Monitored::Scalar<int>("tmp_phiClusGoodA", phi_clus_count[0][1]);

        auto checkClusEtaA = Monitored::Scalar<int>("checkClusEtaA", (int)eta_clus_count[0][0] > 0);
        auto checkClusEtaGoodA = Monitored::Scalar<int>("checkClusEtaGoodA", (int)eta_clus_count[0][1] > 0);

        auto checkClusPhiA = Monitored::Scalar<int>("checkClusPhiA", (int)phi_clus_count[0][0] > 0);
        auto checkClusPhiGoodA = Monitored::Scalar<int>("checkClusPhiGoodA", (int)phi_clus_count[0][1] > 0);

        auto tmp_etaClusC = Monitored::Scalar<int>("tmp_etaClusC", eta_clus_count[1][0]);
        auto tmp_etaClusGoodC = Monitored::Scalar<int>("tmp_etaClusGoodC", eta_clus_count[1][1]);

        auto tmp_phiClusC = Monitored::Scalar<int>("tmp_phiClusC", phi_clus_count[1][0]);
        auto tmp_phiClusGoodC = Monitored::Scalar<int>("tmp_phiClusGoodC", phi_clus_count[1][1]);

        auto checkClusEtaC = Monitored::Scalar<int>("checkClusEtaC", (int)eta_clus_count[1][0] > 0);
        auto checkClusEtaGoodC = Monitored::Scalar<int>("checkClusEtaGoodC", (int)eta_clus_count[1][1] > 0);

        auto checkClusPhiC = Monitored::Scalar<int>("checkClusPhiC", (int)phi_clus_count[1][0] > 0);
        auto checkClusPhiGoodC = Monitored::Scalar<int>("checkClusPhiGoodC", (int)phi_clus_count[1][1] > 0);

        fill("CscSegmMonitor", tmp_etaClusA, checkClusEtaA, tmp_etaClusGoodA, checkClusEtaGoodA, tmp_phiClusA, tmp_phiClusGoodA, checkClusPhiA, checkClusPhiGoodA, tmp_etaClusC, checkClusEtaC, tmp_etaClusGoodC, checkClusEtaGoodC, tmp_phiClusC, tmp_phiClusGoodC, checkClusPhiC, checkClusPhiGoodC);

        // Fill number of 3 and 4 cluster segment histogram
        std::vector<int> tmp_layerIndexA;
        std::vector<int> tmp_isectA;
        std::vector<int> tmp_layerIndexC;
        std::vector<int> tmp_isectC;

        for( int isect = 1; isect < 17; isect++) {

          if(segmClustCount[isect+16] > 2 ) {
            tmp_layerIndexA.push_back(layerindex-5);
            tmp_isectA.push_back(isect);
          }

          if(segmClustCount[isect+16] > 3){           
            for(int i=1; i<=4; i++) {
              tmp_layerIndexA.push_back(i);
              tmp_isectA.push_back(isect);
            }
          }
          
          if(segmClustCount[isect] > 2) {
            tmp_layerIndexC.push_back(layerindex-5);
            tmp_isectC.push_back(-1.*isect);
          }
          

          if(segmClustCount[isect] > 3) {
            for(int i=1; i<=4; i++) {
              tmp_layerIndexC.push_back(i);
              tmp_isectC.push_back(-1.*isect);
            }
          }
        }

        auto tmp_layerIndexA_mon = Monitored::Collection("tmp_layerIndexA_mon", tmp_layerIndexA);
        auto tmp_isectA_mon = Monitored::Collection("tmp_isectA_mon", tmp_isectA);
        auto tmp_layerIndexC_mon = Monitored::Collection("tmp_layerIndexC_mon", tmp_layerIndexA);
        auto tmp_isectC_mon = Monitored::Collection("tmp_isectC_mon", tmp_isectA);
        fill("CscSegmMonitor", tmp_layerIndexA_mon, tmp_isectA_mon, tmp_layerIndexC_mon, tmp_isectC_mon);

        float eta_clus_qsum_tot = 0., phi_clus_qsum_tot = 0.; // total qsum over all prec. trans. layers on segment

        for(unsigned int i=0; i < 2; i++){
          eta_clus_qsum_tot = 0; phi_clus_qsum_tot = 0.;

          for(unsigned int j = 1; j <5; j++) {
            if(i==0){
              auto etaClusQSumA = Monitored::Scalar<float>("etaClusQSumA", eta_clus_qsum[i][j]);
              auto phiClusQSumA = Monitored::Scalar<float>("phiClusQSumA", phi_clus_qsum[i][j]);
              fill("CscSegmMonitor", etaClusQSumA, phiClusQSumA);

              if(std::abs(eta_clus_time[i][j]) <= 200) {
                auto etaTimeClusA = Monitored::Scalar<float>("etaTimeClusA",eta_clus_time[i][j]);
                fill("CscSegmMonitor", etaTimeClusA);
              }

              if(std::abs(phi_clus_time[i][j]) <= 200) {
                auto phiTimeClusA = Monitored::Scalar<float>("phiTimeClusA",phi_clus_time[i][j]);
                fill("CscSegmMonitor", phiTimeClusA);
              }
            }

            if(i==1){
              auto etaClusQSumC = Monitored::Scalar<float>("etaClusQSumC", eta_clus_qsum[i][j]);
              auto phiClusQSumC = Monitored::Scalar<float>("phiClusQSumC", phi_clus_qsum[i][j]);
              fill("CscSegmMonitor", etaClusQSumC, phiClusQSumC);

              if(std::abs(eta_clus_time[i][j]) <= 200) {
                auto etaTimeClusC = Monitored::Scalar<float>("etaTimeClusC",eta_clus_time[i][j]);
                fill("CscSegmMonitor", etaTimeClusC);
              }
            
              if(std::abs(phi_clus_time[i][j]) <= 200) {
                auto phiTimeClusC = Monitored::Scalar<float>("phiTimeClusC",phi_clus_time[i][j]);
                fill("CscSegmMonitor", phiTimeClusC);
              }
            }

                   
            if(phi_clus_use[i][j] && eta_clus_use[i][j]){
              eta_clus_qsum_tot += eta_clus_qsum[i][j];
              phi_clus_qsum_tot += phi_clus_qsum[i][j];

              if(i==0){
                auto etaQSumGoodClusA = Monitored::Scalar<float>("etaQSumGoodClusA",eta_clus_qsum[i][j]);
                auto phiQSumGoodClusA = Monitored::Scalar<float>("phiQSumGoodClusA",phi_clus_qsum[i][j]);
                fill("CscSegmMonitor", etaQSumGoodClusA, phiQSumGoodClusA); 
                if(std::abs(eta_clus_time[i][j]) <= 200){
                  auto etaTimeGoodClusA = Monitored::Scalar<float>("etaTimeGoodClusA",eta_clus_time[i][j]);
                  fill("CscSegmMonitor", etaTimeGoodClusA); 
                }
              
                if(std::abs(phi_clus_time[i][j]) <= 200) {
                  auto phiTimeGoodClusA = Monitored::Scalar<float>("phiTimeGoodClusA",phi_clus_time[i][j]);
                  fill("CscSegmMonitor", phiTimeGoodClusA); 
                }
              }

              if(i==1){
                auto etaQSumGoodClusC = Monitored::Scalar<float>("etaQSumGoodClusC",eta_clus_qsum[i][j]);
                auto phiQSumGoodClusC = Monitored::Scalar<float>("phiQSumGoodClusC",phi_clus_qsum[i][j]);
                fill("CscSegmMonitor", etaQSumGoodClusC, phiQSumGoodClusC);  
                if(std::abs(eta_clus_time[i][j]) <= 200){
                  auto etaTimeGoodClusC = Monitored::Scalar<float>("etaTimeGoodClusC",eta_clus_time[i][j]);
                  fill("CscSegmMonitor", etaTimeGoodClusC);
                } 

                if(std::abs(phi_clus_time[i][j]) <= 200) {
                  auto phiTimeGoodClusC = Monitored::Scalar<float>("phiTimeGoodClusC",phi_clus_time[i][j]);
                  fill("CscSegmMonitor", phiTimeGoodClusC); 
                }
              }
            }
          }
              
          if(i==0) {
            auto etaQSumTot0 = Monitored::Scalar<float>("etaQSumTot0", eta_clus_qsum_tot);
            auto phiQSumTot0 = Monitored::Scalar<float>("phiQSumTot0", phi_clus_qsum_tot);
            fill("CscSegmMonitor", etaQSumTot0, phiQSumTot0);
          }   
        
          if(i==1) {
            auto etaQSumTot = Monitored::Scalar<float>("etaQSumTot", eta_clus_qsum_tot);
            auto phiQSumTot = Monitored::Scalar<float>("phiQSumTot", phi_clus_qsum_tot);
            fill("CscSegmMonitor", etaQSumTot, phiQSumTot); 
          } 
        }    
      } // if is csc segment
    } // loop over segms
  } // environment if

  return StatusCode::SUCCESS;
}

//________________________________________________________________________________________________________
bool CSCSegmValMonAlg::evtSelTriggersPassed() const {

  if(!m_doEvtSel) return true;
  
  for (const auto& trig : m_sampSelTriggers) {
    if(m_trigDecTool->isPassed(trig, TrigDefs::eventAccepted)) {
      return true;
    }
  }
  return false;

} // end evtSelTriggersPassed 

//________________________________________________________________________________________________________
bool CSCSegmValMonAlg::isCscSegment( const Muon::MuonSegment* seg ) const {
  bool isCsc(false);

  std::vector<const Trk::MeasurementBase*> mbs = seg->containedMeasurements();
  for( unsigned int i = 0; i< mbs.size(); ++i){

    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*> (mbs[i]);
    if (!rot){
      const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (mbs[i]);
      if (crot) rot  = &(crot->rioOnTrack(0));
    }  
    if( !rot ) {
      continue;
    }
    if( m_idHelperSvc->isCsc( rot->identify() ) ) isCsc=true;
  }

  return isCsc;
}


//________________________________________________________________________________________________________
unsigned int CSCSegmValMonAlg::cscHits( const Muon::MuonSegment* seg ) const {
  unsigned int nrHits(0);

  std::vector<const Trk::MeasurementBase*> mbs = seg->containedMeasurements();
  for( unsigned int i = 0; i< mbs.size(); ++i){

    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*> (mbs[i]);
    if (!rot){
      const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (mbs[i]);
      if (crot) rot  = &(crot->rioOnTrack(0));
    }  
    if( !rot ) {
      continue;
    }
    if( m_idHelperSvc->isCsc( rot->identify() ) ) ++nrHits;
  }

  return nrHits ;
}

//________________________________________________________________________________________________________
bool CSCSegmValMonAlg::segmSlopeCut(const float csc_x, const float csc_ax, const float cut ) const {
  float s0 = csc_x;
  float s1 = -std::tan(csc_ax);
  float s1corr = s1 - 0.000119 * s0;
  bool good_segm = std::abs(s1corr)<cut ? true : false;
  return good_segm;
}

