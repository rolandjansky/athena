/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkInDet/PixelNtupleMaker.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODTracking/TrackParticle.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTracking/VertexContainer.h"

#include <vector>
#include <string>

#include "TLorentzVector.h"

DerivationFramework::PixelNtupleMaker::PixelNtupleMaker(const std::string& t, const std::string& n, const IInterface* p) : 
  AthAlgTool(t,n,p),
  m_containerName("InDetTrackParticles")
{
  declareInterface<DerivationFramework::ISkimmingTool>(this);
  declareProperty("ContainerName", m_containerName="InDetTrackParticles");
}

DerivationFramework::PixelNtupleMaker::~PixelNtupleMaker() {
}  

StatusCode DerivationFramework::PixelNtupleMaker::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::PixelNtupleMaker::finalize()
{
  return StatusCode::SUCCESS;
}

bool DerivationFramework::PixelNtupleMaker::eventPassesFilter() const {

  const xAOD::TrackParticleContainer* tracks=0;
  CHECK(evtStore()->retrieve(tracks, m_containerName));

  // Check the event contains tracks
  unsigned int nTracks = tracks->size();
  if (nTracks==0) { return StatusCode::SUCCESS; }

  bool isMC = false;
  const xAOD::EventInfo *eventInfo = nullptr;
  CHECK(evtStore()->retrieve(eventInfo, "EventInfo"));
  if (eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) { isMC=true; } // can do something with this later

  const xAOD::TruthParticleContainer* truthPtc = 0;
  if (isMC) { CHECK(evtStore()->retrieve(truthPtc,"TruthParticles")); }

  const xAOD::TrackMeasurementValidationContainer* pixClustersOrig = 0;
  CHECK(evtStore()->retrieve(pixClustersOrig,"PixelClusters"));

  std::pair< xAOD::TrackMeasurementValidationContainer*, xAOD::ShallowAuxContainer* > pixClustersPair = xAOD::shallowCopyContainer( *pixClustersOrig );
  xAOD::TrackMeasurementValidationContainer* pixClusters =  pixClustersPair.first;

  xAOD::TrackParticleContainer* PixelMonitoringTrack = new xAOD::TrackParticleContainer();
  xAOD::TrackParticleAuxContainer* PixelMonitoringTrackAux = new xAOD::TrackParticleAuxContainer();
  PixelMonitoringTrack->setStore(PixelMonitoringTrackAux);
  CHECK(evtStore()->record(PixelMonitoringTrack,"PixelMonitoringTrack"));
  CHECK(evtStore()->record(PixelMonitoringTrackAux,"PixelMonitoringTrackAux."));

  std::vector<float> tmpCov(15,0.);
  static SG::AuxElement::ConstAccessor<MeasurementsOnTrack>  acc_MeasurementsOnTrack("msosLink");
  for (xAOD::TrackParticleContainer::const_iterator trk=tracks->begin(); trk!=tracks->end(); trk++) {

    bool passTrack = true;
    if ((*trk)->pt()<1000.0) {
      uint8_t getInt = 0;
      (*trk)->summaryValue(getInt,xAOD::numberOfPixelHits); int nPixHits = getInt;
      (*trk)->summaryValue(getInt,xAOD::numberOfSCTHits);   int nSCTHits = getInt;
      if (nPixHits<4) { passTrack=false; }
      if (nSCTHits<1) { passTrack=false; }
    }

    if (passTrack) {
      xAOD::TrackParticle* tp = new xAOD::TrackParticle();
      tp->makePrivateStore(**trk);
      tp->setDefiningParametersCovMatrixVec(tmpCov);

      std::vector<int> holeIndex;
      std::vector<int> outlierIndex;
      std::vector<int> clusterLayer;
      std::vector<int> clusterBEC;
      std::vector<int> clusterModulePhi;
      std::vector<int> clusterModuleEta;
      std::vector<float> clusterCharge;
      std::vector<int> clusterToT;
      std::vector<int> clusterL1A;
      std::vector<int> clusterIsSplit;
      std::vector<int> clusterSize;
      std::vector<int> clusterSizePhi;
      std::vector<int> clusterSizeZ;
      std::vector<bool> isEdge;
      std::vector<bool> isGanged;
      std::vector<bool> isOverflow;
      std::vector<float> trackPhi;
      std::vector<float> trackTheta;
      std::vector<float> trackX;
      std::vector<float> trackY;
      std::vector<float> localX;
      std::vector<float> localY;
      std::vector<float> globalX;
      std::vector<float> globalY;
      std::vector<float> globalZ;
      std::vector<float> unbiasedResidualX;
      std::vector<float> unbiasedResidualY;
      std::vector<float> unbiasedPullX;
      std::vector<float> unbiasedPullY;
      std::vector<int> clusterIsolation10x2;
      std::vector<int> clusterIsolation20x4;
      std::vector<int> numTotalClustersPerModule;
      std::vector<int> numTotalPixelsPerModule;
      std::vector<float> moduleBiasVoltage;
      std::vector<float> moduleTemperature;
      std::vector<float> moduleLorentzShift;

      std::vector<std::vector<int>> rdoToT;
      std::vector<std::vector<float>> rdoCharge;
      std::vector<std::vector<int>> rdoPhi;
      std::vector<std::vector<int>> rdoEta;

      std::vector<std::vector<int>> clusterTruthPdgId;
      std::vector<std::vector<int>> siHitPdgId;
      std::vector<std::vector<int>> siHitBarcode;
      std::vector<std::vector<float>> siHitStartPosX;
      std::vector<std::vector<float>> siHitStartPosY;
      std::vector<std::vector<float>> siHitEndPosX;
      std::vector<std::vector<float>> siHitEndPosY;
      std::vector<std::vector<float>> siHitEnergyDeposit;

      const MeasurementsOnTrack& measurementsOnTrack = acc_MeasurementsOnTrack(*(*trk));
      for (MeasurementsOnTrackIter msos_iter=measurementsOnTrack.begin(); msos_iter!=measurementsOnTrack.end(); ++msos_iter) {  
        if (!(*msos_iter).isValid()) { continue; }
        const xAOD::TrackStateValidation* msos = *(*msos_iter); 

        if (msos->detType()==1) { // its a pixel 
          if (msos->type()==6) { 
            holeIndex.push_back(msos->detElementId());
          }
          if (msos->type()==5) { 
            outlierIndex.push_back(msos->detElementId());
          }

          if (msos->trackMeasurementValidationLink().isValid() && *(msos->trackMeasurementValidationLink())) {
            const xAOD::TrackMeasurementValidation* msosClus =  *(msos->trackMeasurementValidationLink());        

            for (xAOD::TrackMeasurementValidationContainer::iterator clus_itr=(pixClusters)->begin(); clus_itr!=(pixClusters)->end(); ++clus_itr) {
              if ((*clus_itr)->identifier()!=(msosClus)->identifier()) { continue; }
              if ((*clus_itr)->auxdata<float>("charge")!=(msosClus)->auxdata<float>("charge")) { continue; }

              clusterLayer.push_back((*clus_itr)->auxdata<int>("layer"));
              clusterBEC.push_back((*clus_itr)->auxdata<int>("bec"));
              clusterModulePhi.push_back((*clus_itr)->auxdata<int>("phi_module"));
              clusterModuleEta.push_back((*clus_itr)->auxdata<int>("eta_module"));
              clusterCharge.push_back((*clus_itr)->auxdata<float>("charge"));
              clusterToT.push_back((*clus_itr)->auxdata<int>("ToT"));
              clusterL1A.push_back((*clus_itr)->auxdata<int>("LVL1A"));
              clusterIsSplit.push_back((*clus_itr)->auxdata<int>("isSplit"));
              clusterSize.push_back((*clus_itr)->auxdata<int>("nRDO"));
              clusterSizePhi.push_back((*clus_itr)->auxdata<int>("sizePhi"));
              clusterSizeZ.push_back((*clus_itr)->auxdata<int>("sizeZ"));
              trackPhi.push_back(msos->localPhi());
              trackTheta.push_back(msos->localTheta());
              trackX.push_back(msos->localX());
              trackY.push_back(msos->localY());
              localX.push_back((*clus_itr)->localX());
              localY.push_back((*clus_itr)->localY());
              globalX.push_back((*clus_itr)->globalX());
              globalY.push_back((*clus_itr)->globalY());
              globalZ.push_back((*clus_itr)->globalZ());
              unbiasedResidualX.push_back(msos->unbiasedResidualX());
              unbiasedResidualY.push_back(msos->unbiasedResidualY());
              unbiasedPullX.push_back(msos->auxdata<float>("unbiasedPullX"));
              unbiasedPullY.push_back(msos->auxdata<float>("unbiasedPullY"));
              moduleBiasVoltage.push_back((*clus_itr)->auxdata<float>("BiasVoltage"));
              moduleTemperature.push_back((*clus_itr)->auxdata<float>("Temperature"));
              moduleLorentzShift.push_back((*clus_itr)->auxdata<float>("LorentzShift"));

              // cluster isolation   IBL:50x250um, PIXEL:50x400um
              //    - isolation region 10x2 = 500x500um for IBL,  500x800um for PIXEL
              int numNeighborCluster10x2 = 0;
              int numNeighborCluster20x4 = 0;
              int nTotalClustersPerModule = 0;
              int nTotalPixelsPerModule = 0;
              for (xAOD::TrackMeasurementValidationContainer::iterator clus_neighbor=(pixClusters)->begin(); clus_neighbor!=(pixClusters)->end(); ++clus_neighbor) {
                if ((*clus_neighbor)->auxdata<int>("layer")==(*clus_itr)->auxdata<int>("layer")
                    && (*clus_neighbor)->auxdata<int>("bec")==(*clus_itr)->auxdata<int>("bec")
                    && (*clus_neighbor)->auxdata<int>("phi_module")==(*clus_itr)->auxdata<int>("phi_module")
                    && (*clus_neighbor)->auxdata<int>("eta_module")==(*clus_itr)->auxdata<int>("eta_module")) {
                  float deltaX = std::abs((*clus_neighbor)->localX()-(*clus_itr)->localX());
                  float deltaY = std::abs((*clus_neighbor)->localY()-(*clus_itr)->localY());
                  nTotalClustersPerModule++;
                  nTotalPixelsPerModule += (*clus_neighbor)->auxdata<int>("nRDO");
                  if (deltaX>0.0 && deltaY>0.0) {
                    if ((*clus_itr)->auxdata<int>("layer")==0 && (*clus_itr)->auxdata<int>("bec")==0) {  // IBL
                      if (deltaX<0.500 && deltaY<0.500) { numNeighborCluster10x2++; }
                      if (deltaX<1.000 && deltaY<1.000) { numNeighborCluster20x4++; }
                    }
                    else {
                      if (deltaX<0.500 && deltaY<0.800) { numNeighborCluster10x2++; }
                      if (deltaX<1.000 && deltaY<1.600) { numNeighborCluster20x4++; }
                    }
                  }
                }
              }
              clusterIsolation10x2.push_back(numNeighborCluster10x2);
              clusterIsolation20x4.push_back(numNeighborCluster20x4);
              numTotalClustersPerModule.push_back(nTotalClustersPerModule);
              numTotalPixelsPerModule.push_back(nTotalPixelsPerModule);

              // is edge pixel?
              // contain overlflow hit?
              bool checkEdge = false;
              bool checkGanged = false;
              bool checkOverflow = false;
              if ((*clus_itr)->isAvailable<std::vector<int>>("rdo_phi_pixel_index")) {
                for (int i=0; i<(int)(*clus_itr)->auxdata<std::vector<int>>("rdo_phi_pixel_index").size(); i++) {

                  int phi = (*clus_itr)->auxdata<std::vector<int>>("rdo_phi_pixel_index")[i];
                  if (phi<5)   { checkEdge=true; }
                  if (phi>320) { checkEdge=true; }

                  int eta = (*clus_itr)->auxdata<std::vector<int>>("rdo_eta_pixel_index")[i];
                  if ((*clus_itr)->auxdata<int>("layer")==0 && (*clus_itr)->auxdata<int>("bec")==0) {  // IBL
                    if ((*clus_itr)->auxdata<int>("eta_module")>-7 && (*clus_itr)->auxdata<int>("eta_module")<6) { // IBL Planar                                                       
                      if (eta<5)   { checkEdge=true; }
                      if (eta>154) { checkEdge=true; }
                    }
                    else {  // IBL 3D
                      if (eta<5)  { checkEdge=true; }
                      if (eta>74) { checkEdge=true; }
                    }
                  }
                  else {
                    if (eta<5)   { checkEdge=true; }
                    if (eta>154) { checkEdge=true; }
                  }

                  int tot = (*clus_itr)->auxdata<std::vector<int>>("rdo_tot")[i];
                  if ((*clus_itr)->auxdata<int>("layer")==0 && (*clus_itr)->auxdata<int>("bec")==0) {  // IBL
                    if (tot==16) { checkOverflow=true; }
                  }
                  else if ((*clus_itr)->auxdata<int>("layer")==1 && (*clus_itr)->auxdata<int>("bec")==0) {  // b-layer
                    if (tot==150) { checkOverflow=true; }
                  }
                  else {
                    if (tot==255) { checkOverflow=true; }
                  }


                  // check ganged pixels
                  if ((*clus_itr)->auxdata<int>("layer")==0 && (*clus_itr)->auxdata<int>("bec")==0) {  // IBL
                    // nothing
                  }
                  else { 
                    int columnsPerFE = 18;   // number of columns per FEI3 (18x160)
                    int rowsFGanged  =153;   // first ganged pixel row for FEI3
                    int rowsLGanged  =159;   // last ganged pixel row for FEI3

                    int chkGangedID = 0;
                    int col = eta%columnsPerFE;
                    int row = phi;
                    if (col>0 && col<columnsPerFE-1) {
                      if (row>=rowsFGanged-1 && row<=rowsLGanged) {
                        chkGangedID = (row-rowsFGanged+1)%2+1; // 1 inter ganged pixel; 2 ganged pixel
                      }
                    } 
                    else if (col==0 || col==columnsPerFE-1) {
                      if (row>=rowsFGanged-1) {
                        chkGangedID = 2; // treat long ganged (3) as ganged 
                      }
                      else {
                        chkGangedID = 1; // long
                      }
                    }
                    if (chkGangedID>0) { checkGanged=true; }
                  }
                }
              }
              isEdge.push_back(checkEdge);
              isGanged.push_back(checkGanged);
              isOverflow.push_back(checkOverflow);

              // rdo information 
              std::vector<int> tmpToT;
              std::vector<float> tmpCharge;
              std::vector<int> tmpPhi;
              std::vector<int> tmpEta;
              if ((*trk)->pt()>2000.0) {
                if ((*clus_itr)->isAvailable<std::vector<int>>("rdo_phi_pixel_index")) {
                  for (int i=0; i<(int)(*clus_itr)->auxdata<std::vector<int>>("rdo_phi_pixel_index").size(); i++) {
                    int phi = (*clus_itr)->auxdata<std::vector<int>>("rdo_phi_pixel_index")[i];
                    int eta = (*clus_itr)->auxdata<std::vector<int>>("rdo_eta_pixel_index")[i];
                    int tot = (*clus_itr)->auxdata<std::vector<int>>("rdo_tot")[i];
                    float charge = (*clus_itr)->auxdata<std::vector<float>>("rdo_charge")[i];

                    tmpToT.push_back(tot);
                    tmpCharge.push_back(charge);
                    tmpPhi.push_back(phi);
                    tmpEta.push_back(eta);
                  }
                }
              }
              rdoToT.push_back(tmpToT);
              rdoCharge.push_back(tmpCharge);
              rdoPhi.push_back(tmpPhi);
              rdoEta.push_back(tmpEta);

              // truth information
              std::vector<int> tmpTruthBarcode;
              std::vector<int> tmpTruthPdg;
              if (isMC) {
                if ((*clus_itr)->isAvailable<std::vector<int>>("truth_barcode")) {
                  tmpTruthBarcode = (*clus_itr)->auxdata<std::vector<int>>("truth_barcode");

                  for (int i=0; i<(int)tmpTruthBarcode.size(); i++) {
                    int pdgmatch = 0;
                    for (xAOD::TruthParticleContainer::const_iterator tr_it=truthPtc->begin(); tr_it!=truthPtc->end(); tr_it++) {
                      if ((*tr_it)->barcode()==tmpTruthBarcode.at(i)) {
                        pdgmatch = (*tr_it)->pdgId();
                        if (TMath::Abs(tmpTruthBarcode.at(i))>200000) {
                          if (pdgmatch>0) { pdgmatch+=200000; }
                          else            { pdgmatch-=200000; }
                        }
                        break;
                      }
                    }
                    tmpTruthPdg.push_back(pdgmatch);
                  }
                }
              }
              clusterTruthPdgId.push_back(tmpTruthPdg);

              if (isMC && (*clus_itr)->isAvailable<std::vector<int>>("sihit_barcode")) {
                static SG::AuxElement::ConstAccessor<std::vector<int>>   acc_sihit_barcode("sihit_barcode");
                static SG::AuxElement::ConstAccessor<std::vector<int>>   acc_sihit_pdgid("sihit_pdgid");
                static SG::AuxElement::ConstAccessor<std::vector<float>> acc_sihit_startPosX("sihit_startPosX");
                static SG::AuxElement::ConstAccessor<std::vector<float>> acc_sihit_startPosY("sihit_startPosY");
                static SG::AuxElement::ConstAccessor<std::vector<float>> acc_sihit_endPosX("sihit_endPosX");
                static SG::AuxElement::ConstAccessor<std::vector<float>> acc_sihit_endPosY("sihit_endPosY");
                static SG::AuxElement::ConstAccessor<std::vector<float>> acc_sihit_energyDeposit("sihit_energyDeposit");

                const std::vector<int>   &sihit_barcode       = acc_sihit_barcode(*(*clus_itr));
                const std::vector<int>   &sihit_pdgid         = acc_sihit_pdgid(*(*clus_itr));
                const std::vector<float> &sihit_startPosX     = acc_sihit_startPosX(*(*clus_itr));
                const std::vector<float> &sihit_startPosY     = acc_sihit_startPosY(*(*clus_itr));
                const std::vector<float> &sihit_endPosX       = acc_sihit_endPosX(*(*clus_itr));
                const std::vector<float> &sihit_endPosY       = acc_sihit_endPosY(*(*clus_itr));
                const std::vector<float> &sihit_energyDeposit = acc_sihit_energyDeposit(*(*clus_itr));

                siHitPdgId.push_back(sihit_pdgid);
                siHitBarcode.push_back(sihit_barcode);
                siHitStartPosX.push_back(sihit_startPosX);
                siHitStartPosY.push_back(sihit_startPosY);
                siHitEndPosX.push_back(sihit_endPosX);
                siHitEndPosY.push_back(sihit_endPosY);
                siHitEnergyDeposit.push_back(sihit_energyDeposit);
              }
              break;
            }
          }
        }
      }

      static SG::AuxElement::Decorator<float> d0err("d0err");
      static SG::AuxElement::Decorator<float> z0err("z0err");
      static SG::AuxElement::Decorator<float> qOverPerr("qOverPerr");
      static SG::AuxElement::Decorator<std::vector<int>>   HoleIndex("HoleIndex");
      static SG::AuxElement::Decorator<std::vector<int>>   OutlierIndex("OutlierIndex");
      static SG::AuxElement::Decorator<std::vector<int>>   ClusterLayer("ClusterLayer");
      static SG::AuxElement::Decorator<std::vector<int>>   ClusterBEC("ClusterBEC");
      static SG::AuxElement::Decorator<std::vector<int>>   ClusterModulePhi("ClusterModulePhi");
      static SG::AuxElement::Decorator<std::vector<int>>   ClusterModuleEta("ClusterModuleEta");
      static SG::AuxElement::Decorator<std::vector<float>> ClusterCharge("ClusterCharge");
      static SG::AuxElement::Decorator<std::vector<int>>   ClusterToT("ClusterToT");
      static SG::AuxElement::Decorator<std::vector<int>>   ClusterL1A("ClusterL1A");
      static SG::AuxElement::Decorator<std::vector<int>>   ClusterIsSplit("ClusterIsSplit");
      static SG::AuxElement::Decorator<std::vector<int>>   ClusterSize("ClusterSize");
      static SG::AuxElement::Decorator<std::vector<int>>   ClusterSizePhi("ClusterSizePhi");
      static SG::AuxElement::Decorator<std::vector<int>>   ClusterSizeZ("ClusterSizeZ");
      static SG::AuxElement::Decorator<std::vector<bool>>  ClusterIsEdge("ClusterIsEdge");
      static SG::AuxElement::Decorator<std::vector<bool>>  ClusterIsGanged("ClusterIsGanged");
      static SG::AuxElement::Decorator<std::vector<bool>>  ClusterIsOverflow("ClusterIsOverflow");
      static SG::AuxElement::Decorator<std::vector<float>> TrackLocalPhi("TrackLocalPhi");
      static SG::AuxElement::Decorator<std::vector<float>> TrackLocalTheta("TrackLocalTheta");
      static SG::AuxElement::Decorator<std::vector<float>> TrackLocalX("TrackLocalX");
      static SG::AuxElement::Decorator<std::vector<float>> TrackLocalY("TrackLocalY");
      static SG::AuxElement::Decorator<std::vector<float>> ClusterLocalX("ClusterLocalX");
      static SG::AuxElement::Decorator<std::vector<float>> ClusterLocalY("ClusterLocalY");
      static SG::AuxElement::Decorator<std::vector<float>> ClusterGlobalX("ClusterGlobalX");
      static SG::AuxElement::Decorator<std::vector<float>> ClusterGlobalY("ClusterGlobalY");
      static SG::AuxElement::Decorator<std::vector<float>> ClusterGlobalZ("ClusterGlobalZ");
      static SG::AuxElement::Decorator<std::vector<float>> UnbiasedResidualX("UnbiasedResidualX");
      static SG::AuxElement::Decorator<std::vector<float>> UnbiasedResidualY("UnbiasedResidualY");
      static SG::AuxElement::Decorator<std::vector<float>> UnbiasedPullX("UnbiasedPullX");
      static SG::AuxElement::Decorator<std::vector<float>> UnbiasedPullY("UnbiasedPullY");
      static SG::AuxElement::Decorator<std::vector<int>>   ClusterIsolation10x2("ClusterIsolation10x2");
      static SG::AuxElement::Decorator<std::vector<int>>   ClusterIsolation20x4("ClusterIsolation20x4");
      static SG::AuxElement::Decorator<std::vector<int>>   NumTotalClustersPerModule("NumTotalClustersPerModule");
      static SG::AuxElement::Decorator<std::vector<int>>   NumTotalPixelsPerModule("NumTotalPixelsPerModule");
      static SG::AuxElement::Decorator<std::vector<float>> ModuleBiasVoltage("ModuleBiasVoltage");
      static SG::AuxElement::Decorator<std::vector<float>> ModuleTemperature("ModuleTemperature");
      static SG::AuxElement::Decorator<std::vector<float>> ModuleLorentzShift("ModuleLorentzShift");
      static SG::AuxElement::Decorator<std::vector<std::vector<int>>>   RdoToT("RdoToT");
      static SG::AuxElement::Decorator<std::vector<std::vector<float>>> RdoCharge("RdoCharge");
      static SG::AuxElement::Decorator<std::vector<std::vector<int>>>   RdoPhi("RdoPhi");
      static SG::AuxElement::Decorator<std::vector<std::vector<int>>>   RdoEta("RdoEta");
      static SG::AuxElement::Decorator<std::vector<std::vector<int>>> ClusterTruthPdgId("ClusterTruthPdgId");
      static SG::AuxElement::Decorator<std::vector<std::vector<int>>> SiHitPdgId("SiHitPdgId");
      static SG::AuxElement::Decorator<std::vector<std::vector<int>>> SiHitBarcode("SiHitBarcode");
      static SG::AuxElement::Decorator<std::vector<std::vector<float>>> SiHitStartPosX("SiHitStartPosX");
      static SG::AuxElement::Decorator<std::vector<std::vector<float>>> SiHitStartPosY("SiHitStartPosY");
      static SG::AuxElement::Decorator<std::vector<std::vector<float>>> SiHitEndPosX("SiHitEndPosX");
      static SG::AuxElement::Decorator<std::vector<std::vector<float>>> SiHitEndPosY("SiHitEndPosY");
      static SG::AuxElement::Decorator<std::vector<std::vector<float>>> SiHitEnergyDeposit("SiHitEnergyDeposit");

      d0err(*tp)             = (*trk)->definingParametersCovMatrixVec().at(0);
      z0err(*tp)             = (*trk)->definingParametersCovMatrixVec().at(2);
      qOverPerr(*tp)         = TMath::Sqrt((*trk)->definingParametersCovMatrix()(4,4));

      HoleIndex(*tp)         = holeIndex;
      OutlierIndex(*tp)      = outlierIndex;
      ClusterLayer(*tp)      = clusterLayer;
      ClusterBEC(*tp)        = clusterBEC;
      ClusterModulePhi(*tp)  = clusterModulePhi;
      ClusterModuleEta(*tp)  = clusterModuleEta;
      ClusterCharge(*tp)     = clusterCharge;
      ClusterToT(*tp)        = clusterToT;
      ClusterL1A(*tp)        = clusterL1A;
      ClusterIsSplit(*tp)    = clusterIsSplit;
      ClusterSize(*tp)       = clusterSize;
      ClusterSizePhi(*tp)    = clusterSizePhi;
      ClusterSizeZ(*tp)      = clusterSizeZ;
      ClusterIsEdge(*tp)     = isEdge;
      ClusterIsGanged(*tp)   = isGanged;
      ClusterIsOverflow(*tp) = isOverflow;
      TrackLocalPhi(*tp)     = trackPhi;
      TrackLocalTheta(*tp)   = trackTheta;
      TrackLocalX(*tp)       = trackX;
      TrackLocalY(*tp)       = trackY;
      ClusterLocalX(*tp)     = localX;
      ClusterLocalY(*tp)     = localY;
      ClusterGlobalX(*tp)    = globalX;
      ClusterGlobalY(*tp)    = globalY;
      ClusterGlobalZ(*tp)    = globalZ;
      UnbiasedResidualX(*tp) = unbiasedResidualX;
      UnbiasedResidualY(*tp) = unbiasedResidualY;
      UnbiasedPullX(*tp)     = unbiasedPullX;
      UnbiasedPullY(*tp)     = unbiasedPullY;
      ClusterIsolation10x2(*tp) = clusterIsolation10x2;
      ClusterIsolation20x4(*tp) = clusterIsolation20x4;
      NumTotalClustersPerModule(*tp) = numTotalClustersPerModule;
      NumTotalPixelsPerModule(*tp)   = numTotalPixelsPerModule;
      ModuleBiasVoltage(*tp)  = moduleBiasVoltage;
      ModuleTemperature(*tp)  = moduleTemperature;
      ModuleLorentzShift(*tp) = moduleLorentzShift;
      RdoToT(*tp)    = rdoToT;
      RdoCharge(*tp) = rdoCharge;
      RdoPhi(*tp)    = rdoPhi;
      RdoEta(*tp)    = rdoEta;

      ClusterTruthPdgId(*tp)  = clusterTruthPdgId;
      SiHitPdgId(*tp)         = siHitPdgId;
      SiHitBarcode(*tp)       = siHitBarcode;
      SiHitStartPosX(*tp)     = siHitStartPosX;
      SiHitStartPosY(*tp)     = siHitStartPosY;
      SiHitEndPosX(*tp)       = siHitEndPosX;
      SiHitEndPosY(*tp)       = siHitEndPosY;
      SiHitEnergyDeposit(*tp) = siHitEnergyDeposit;

      PixelMonitoringTrack->push_back(tp);
    }
  }
  bool pass=true;
  return pass;
}  

void DerivationFramework::PixelNtupleMaker::GetLayerEtaPhiFromId(uint64_t id,int *barrelEC, int *layer, int *eta, int *phi) const {
  *eta =(id>>43) % 0x20 - 10;
  *phi =(id>>43) % 0x800 / 0x20;
  int layer_index = ((id>>43) / 0x800) & 0xF;

  //A possibility is to use a bit by bit AND: layer_index & 0xF

  switch (layer_index) {
    case 4:
      *barrelEC=-2;
      *layer = 0;
      break;
    case 5:
      *barrelEC=-2;
      *layer = 1;
      break;
    case 6:
      *barrelEC=-2;
      *layer = 2;
      break;
    case 8:
      *barrelEC=0;
      *layer=0;
      break;
    case 9:
      *layer=1;
      *barrelEC=0;
      break;
    case 10:
      *layer=2;
      *barrelEC=0;
      break;
    case 11:
      *barrelEC=0;
      *layer =3;
      break;
    case 12:
      *barrelEC=2;
      *layer=0;
      break;
    case 13:
      *layer =1;
      *barrelEC=2;
      break;
    case 14:
      *layer=2;
      *barrelEC=2;
      break;
  }
  return;
}
