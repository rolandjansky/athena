/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkInDet/PixelNtupleMaker.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODTracking/TrackParticle.h"

#include <vector>
#include <string>

#include "TLorentzVector.h"

DerivationFramework::PixelNtupleMaker::PixelNtupleMaker(const std::string& t, const std::string& n, const IInterface* p) : 
  AthAlgTool(t,n,p)
{
  declareInterface<DerivationFramework::ISkimmingTool>(this);
}

DerivationFramework::PixelNtupleMaker::~PixelNtupleMaker() {
}

StatusCode DerivationFramework::PixelNtupleMaker::initialize() {
  ATH_CHECK(m_containerKey.initialize());
  ATH_CHECK(m_measurementContainerKey.initialize());
  ATH_CHECK(m_monitoringTracks.initialize());
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::PixelNtupleMaker::finalize()
{
  return StatusCode::SUCCESS;
}

bool DerivationFramework::PixelNtupleMaker::eventPassesFilter() const {
  const EventContext& ctx = Gaudi::Hive::currentContext();

  SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_containerKey,ctx);
//  ATH_CHECK(evtStore()->retrieve(tracks, m_containerName));
  if (!tracks.isValid()) { return false; }

  // Check the event contains tracks
  unsigned int nTracks = tracks->size();
//  if (nTracks==0) return StatusCode::SUCCESS;
  if (nTracks==0) { return false; }

  SG::ReadHandle<xAOD::TrackMeasurementValidationContainer> pixClusters(m_measurementContainerKey,ctx);
  if (!pixClusters.isValid()) { return false; }

  SG::WriteHandle<xAOD::TrackParticleContainer> PixelMonitoringTrack(m_monitoringTracks,ctx);
  if (PixelMonitoringTrack.record(std::make_unique<xAOD::TrackParticleContainer>(),
                                  std::make_unique<xAOD::TrackParticleAuxContainer>()).isFailure())  {
     return false;
  }

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
      std::vector<int> clusterIsolation10x2;
      std::vector<int> clusterIsolation20x4;
      std::vector<int> numTotalClustersPerModule;
      std::vector<int> numTotalPixelsPerModule;
      std::vector<float> moduleLorentzShift;

      std::vector<std::vector<int>> rdoToT;
      std::vector<std::vector<float>> rdoCharge;
      std::vector<std::vector<int>> rdoPhi;
      std::vector<std::vector<int>> rdoEta;

      const MeasurementsOnTrack& measurementsOnTrack = acc_MeasurementsOnTrack(*(*trk));
      for (MeasurementsOnTrackIter msos_iter=measurementsOnTrack.begin(); msos_iter!=measurementsOnTrack.end(); ++msos_iter) {  
        if (!(*msos_iter).isValid()) { continue; }
        const xAOD::TrackStateValidation* msos = *(*msos_iter); 
        if (!msos->trackMeasurementValidationLink().isValid()) { continue; }
        if (!(*(msos->trackMeasurementValidationLink())))      { continue; }
        if (msos->detType()==1) { // its a pixel 
          if (msos->type()==6) { 
            holeIndex.push_back(msos->detElementId());
            continue; 
          } // not a hole
          const xAOD::TrackMeasurementValidation* msosClus =  *(msos->trackMeasurementValidationLink());        

          for (xAOD::TrackMeasurementValidationContainer::const_iterator clus_itr=pixClusters->begin(); clus_itr!=pixClusters->end(); ++clus_itr) {
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
            moduleLorentzShift.push_back((*clus_itr)->auxdata<float>("LorentzShift"));

            // cluster isolation   IBL:50x250um, PIXEL:50x400um
            //    - isolation region 10x2 = 500x500um for IBL,  500x800um for PIXEL
            int numNeighborCluster10x2 = 0;
            int numNeighborCluster20x4 = 0;
            int nTotalClustersPerModule = 0;
            int nTotalPixelsPerModule = 0;
            for (xAOD::TrackMeasurementValidationContainer::const_iterator clus_neighbor=pixClusters->begin(); clus_neighbor!=pixClusters->end(); ++clus_neighbor) {
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
              }
            }
            isEdge.push_back(checkEdge);
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

            break;
          }
        }
      }

      static SG::AuxElement::Decorator<float> d0err("d0err");
      static SG::AuxElement::Decorator<float> z0err("z0err");
      static SG::AuxElement::Decorator<std::vector<int>>   HoleIndex("HoleIndex");
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
      static SG::AuxElement::Decorator<std::vector<int>>   ClusterIsolation10x2("ClusterIsolation10x2");
      static SG::AuxElement::Decorator<std::vector<int>>   ClusterIsolation20x4("ClusterIsolation20x4");
      static SG::AuxElement::Decorator<std::vector<int>>   NumTotalClustersPerModule("NumTotalClustersPerModule");
      static SG::AuxElement::Decorator<std::vector<int>>   NumTotalPixelsPerModule("NumTotalPixelsPerModule");
      static SG::AuxElement::Decorator<std::vector<float>> ModuleLorentzShift("ModuleLorentzShift");
      static SG::AuxElement::Decorator<std::vector<std::vector<int>>>   RdoToT("RdoToT");
      static SG::AuxElement::Decorator<std::vector<std::vector<float>>> RdoCharge("RdoCharge");
      static SG::AuxElement::Decorator<std::vector<std::vector<int>>>   RdoPhi("RdoPhi");
      static SG::AuxElement::Decorator<std::vector<std::vector<int>>>   RdoEta("RdoEta");
      d0err(*tp)             = (*trk)->definingParametersCovMatrixVec().at(0);
      z0err(*tp)             = (*trk)->definingParametersCovMatrixVec().at(2);
      HoleIndex(*tp)         = std::move(holeIndex);
      ClusterLayer(*tp)      = std::move(clusterLayer);
      ClusterBEC(*tp)        = std::move(clusterBEC);
      ClusterModulePhi(*tp)  = std::move(clusterModulePhi);
      ClusterModuleEta(*tp)  = std::move(clusterModuleEta);
      ClusterCharge(*tp)     = std::move(clusterCharge);
      ClusterToT(*tp)        = std::move(clusterToT);
      ClusterL1A(*tp)        = std::move(clusterL1A);
      ClusterIsSplit(*tp)    = std::move(clusterIsSplit);
      ClusterSize(*tp)       = std::move(clusterSize);
      ClusterSizePhi(*tp)    = std::move(clusterSizePhi);
      ClusterSizeZ(*tp)      = std::move(clusterSizeZ);
      ClusterIsEdge(*tp)     = std::move(isEdge);
      ClusterIsOverflow(*tp) = std::move(isOverflow);
      TrackLocalPhi(*tp)     = std::move(trackPhi);
      TrackLocalTheta(*tp)   = std::move(trackTheta);
      TrackLocalX(*tp)       = std::move(trackX);
      TrackLocalY(*tp)       = std::move(trackY);
      ClusterLocalX(*tp)     = std::move(localX);
      ClusterLocalY(*tp)     = std::move(localY);
      ClusterGlobalX(*tp)    = std::move(globalX);
      ClusterGlobalY(*tp)    = std::move(globalY);
      ClusterGlobalZ(*tp)    = std::move(globalZ);
      UnbiasedResidualX(*tp) = std::move(unbiasedResidualX);
      UnbiasedResidualY(*tp) = std::move(unbiasedResidualY);
      ClusterIsolation10x2(*tp) = std::move(clusterIsolation10x2);
      ClusterIsolation20x4(*tp) = std::move(clusterIsolation20x4);
      NumTotalClustersPerModule(*tp) = std::move(numTotalClustersPerModule);
      NumTotalPixelsPerModule(*tp)   = std::move(numTotalPixelsPerModule);
      ModuleLorentzShift(*tp) = std::move(moduleLorentzShift);
      RdoToT(*tp)    = std::move(rdoToT);
      RdoCharge(*tp) = std::move(rdoCharge);
      RdoPhi(*tp)    = std::move(rdoPhi);
      RdoEta(*tp)    = std::move(rdoEta);

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
