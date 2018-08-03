/////////////////////////////////////////////////////////////////
// PixelOfflineMonitoring.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkInDet/PixelOfflineMonitoring.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODTracking/TrackParticle.h"

#include <vector>
#include <string>

#include "TLorentzVector.h"

DerivationFramework::PixelOfflineMonitoring::PixelOfflineMonitoring(const std::string& t, const std::string& n, const IInterface* p) : 
  AthAlgTool(t,n,p),
  m_containerName("InDetTrackParticles")
{
  declareInterface<DerivationFramework::ISkimmingTool>(this);
  declareProperty("ContainerName", m_containerName="InDetTrackParticles");
}

DerivationFramework::PixelOfflineMonitoring::~PixelOfflineMonitoring() {
}  

StatusCode DerivationFramework::PixelOfflineMonitoring::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::PixelOfflineMonitoring::finalize()
{
  return StatusCode::SUCCESS;
}

bool DerivationFramework::PixelOfflineMonitoring::eventPassesFilter() const {

  const xAOD::TrackParticleContainer* tracks=0;
  CHECK(evtStore()->retrieve(tracks, m_containerName));

  // Check the event contains tracks
  unsigned int nTracks = tracks->size();
  if (nTracks==0) return StatusCode::SUCCESS;

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
      std::vector<float> moduleBiasVoltage;
      std::vector<float> moduleTemperature;
      std::vector<float> moduleLorentzShift;

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
            bool checkOverflow = false;
            if ((*clus_itr)->isAvailable<std::vector<int>>("rdo_phi_pixel_index")) {
              for (int i=0; i<(int)(*clus_itr)->auxdata<std::vector<int>>("rdo_phi_pixel_index").size(); i++) {

                int phi = (*clus_itr)->auxdata<std::vector<int>>("rdo_phi_pixel_index")[i];
                if (phi<5)   { checkEdge=true; }
                if (phi>320) { checkEdge=true; }

                int eta = (*clus_itr)->auxdata<std::vector<int>>("rdo_eta_pixel_index")[i];
                if ((*clus_itr)->auxdata<int>("layer")==0 && (*clus_itr)->auxdata<int>("bec")==0) {  // IBL
                  if (eta<5)  { checkEdge=true; }
                  if (eta>70) { checkEdge=true; }
                }
                else {
                  if (eta<5)  { checkEdge=true; }
                  if (eta>140) { checkEdge=true; }
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
      static SG::AuxElement::Decorator<std::vector<float>> ModuleBiasVoltage("ModuleBiasVoltage");
      static SG::AuxElement::Decorator<std::vector<float>> ModuleTemperature("ModuleTemperature");
      static SG::AuxElement::Decorator<std::vector<float>> ModuleLorentzShift("ModuleLorentzShift");
      d0err(*tp)             = (*trk)->definingParametersCovMatrixVec().at(0);
      z0err(*tp)             = (*trk)->definingParametersCovMatrixVec().at(2);
      HoleIndex(*tp)         = holeIndex;
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
      ClusterIsolation10x2(*tp) = clusterIsolation10x2;
      ClusterIsolation20x4(*tp) = clusterIsolation20x4;
      NumTotalClustersPerModule(*tp) = numTotalClustersPerModule;
      NumTotalPixelsPerModule(*tp)   = numTotalPixelsPerModule;
      ModuleBiasVoltage(*tp)  = moduleBiasVoltage;
      ModuleTemperature(*tp)  = moduleTemperature;
      ModuleLorentzShift(*tp) = moduleLorentzShift;

      PixelMonitoringTrack->push_back(tp);
    }
  }

  bool pass=true;
  return pass;
}  

void DerivationFramework::PixelOfflineMonitoring::GetLayerEtaPhiFromId(uint64_t id,int *barrelEC, int *layer, int *eta, int *phi) const {
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
