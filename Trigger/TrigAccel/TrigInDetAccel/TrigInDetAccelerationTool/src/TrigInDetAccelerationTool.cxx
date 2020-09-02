/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetAccelerationTool.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h" 
#include "AthenaBaseComps/AthCheckMacros.h"
#include "TrigAccelEvent/TrigInDetAccelEDM.h"

TrigInDetAccelerationTool::TrigInDetAccelerationTool(const std::string& t, 
						     const std::string& n,
						     const IInterface*  p ): AthAlgTool(t,n,p),
									     m_accelSvc("TrigInDetAccelerationSvc",this->name()) {

  declareInterface< ITrigInDetAccelerationTool >( this );
}

StatusCode TrigInDetAccelerationTool::initialize() {

  ATH_CHECK(m_accelSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode TrigInDetAccelerationTool::finalize() {  
  return StatusCode::SUCCESS;
}

size_t TrigInDetAccelerationTool::exportSeedMakingJob(const TrigCombinatorialSettings& tcs, const IRoiDescriptor* roi, const std::vector<TrigSiSpacePointBase>& vsp, TrigAccel::DATA_EXPORT_BUFFER& output) const {

  typedef struct SpIndexPair {
  public:

    struct compareZ {
    public:
      bool operator()(const std::pair<int, const TrigSiSpacePointBase*>& p1, const std::pair<int, const TrigSiSpacePointBase*>& p2) {
        return p1.second->z() < p2.second->z();
      }
    };
    
    struct compareR {
    public:
      bool operator()(const std::pair<int, const TrigSiSpacePointBase*>& p1, const std::pair<int, const TrigSiSpacePointBase*>& p2) {
        return p1.second->r() < p2.second->r();
      }
    };

  } SP_INDEX_PAIR;
  

  //0. get InDet geometry information

  const std::vector<short>& pixelLayers = m_accelSvc->getLayerInformation(1);
  const std::vector<short>& sctLayers   = m_accelSvc->getLayerInformation(2);
  const std::vector<short>& layerTypes  = m_accelSvc->getLayerInformation(0);
  

  //1. check buffer size

  size_t dataTypeSize = sizeof(TrigAccel::SEED_MAKING_JOB);
  const size_t bufferOffset = 256;
  size_t totalSize = bufferOffset+dataTypeSize;//make room for the header
  if(!output.fit(totalSize)) output.reallocate(totalSize);

  TrigAccel::SEED_MAKING_JOB* pJ = reinterpret_cast<TrigAccel::SEED_MAKING_JOB*>(output.m_buffer+bufferOffset);

  memset(pJ,0,dataTypeSize);

  TrigAccel::SEED_FINDER_SETTINGS& sfs = pJ->m_settings; 

  sfs.m_maxBarrelPix = tcs.m_maxBarrelPix;
  sfs.m_minEndcapPix = tcs.m_minEndcapPix;
  sfs.m_maxEndcapPix = tcs.m_maxEndcapPix;
  sfs.m_maxSiliconLayer = tcs.m_maxSiliconLayer;
  
  sfs.m_magFieldZ = tcs.m_magFieldZ;

  sfs.m_tripletD0Max = tcs.m_tripletD0Max;
  sfs.m_tripletD0_PPS_Max = tcs.m_tripletD0_PPS_Max;
  sfs.m_tripletPtMin = tcs.m_tripletPtMin;
  sfs.m_tripletDoPSS = tcs.m_tripletDoPSS ? 1 : 0;
  sfs.m_doubletFilterRZ = tcs.m_doubletFilterRZ ? 1 : 0;
  sfs.m_nMaxPhiSlice = tcs.m_nMaxPhiSlice;
  sfs.m_maxTripletBufferLength = tcs.m_maxTripletBufferLength;
  sfs.m_isFullScan = 1;
  
  sfs.m_zedMinus = roi->zedMinus();
  sfs.m_zedPlus =  roi->zedPlus();

  TrigAccel::SPACEPOINT_STORAGE& sps = pJ->m_data;

  unsigned int nSP = vsp.size();
  if(nSP >= TrigAccel::MAX_NUMBER_SPACEPOINTS) {
    nSP = TrigAccel::MAX_NUMBER_SPACEPOINTS-1;
    ATH_MSG_WARNING("MAX_NUMBER_SPACEPOINTS exceeded, exported data truncated ...");
  }

  //2. arrange spacepoints into phi/Layer bins

  double phiSliceWidth = 2*M_PI/tcs.m_nMaxPhiSlice;
  int nLayers = (int) layerTypes.size();
  int nSlices = (int) tcs.m_nMaxPhiSlice;
  
  std::vector<std::vector<std::pair<int, const TrigSiSpacePointBase*> > > phiLArray;
  phiLArray.resize(nLayers*nSlices);
  
  for(unsigned int i=0;i<nSP;i++) {
    const TrigSiSpacePointBase& sp  = vsp[i];
    const std::pair<IdentifierHash, IdentifierHash>& els = sp.offlineSpacePoint()->elementIdList();

    IdentifierHash hashId = els.first;
    short layerId = -1;
    if(sp.isPixel()) {
      layerId = pixelLayers[hashId];
    } else {
      layerId = sctLayers[hashId];
    }
    int phiIdx = (sp.phi()+M_PI)/phiSliceWidth; 
    if (phiIdx >= tcs.m_nMaxPhiSlice) { 
      phiIdx %= tcs.m_nMaxPhiSlice; 
    } 
    else if (phiIdx < 0) { 
      phiIdx += tcs.m_nMaxPhiSlice; 
      phiIdx %= tcs.m_nMaxPhiSlice; 
    } 

    std::vector<std::pair<int, const TrigSiSpacePointBase*> >& v = phiLArray[layerId + phiIdx*nLayers];
    v.push_back(std::pair<int, const TrigSiSpacePointBase*>(i,&sp));//storing the original index of spacepoint
  }

  //sorting spacepoints in accordance with non-ref coordinate 
  int layerIdx=0;
  for(std::vector<short>::const_iterator it = layerTypes.begin();it!=layerTypes.end();++it, layerIdx++) {
    short barrel_ec = (*it);//0-barrel, !=0 - endcap
    for(int slice = 0;slice<nSlices;slice++) {
      std::vector<std::pair<int, const TrigSiSpacePointBase*> >& v = phiLArray[layerIdx + slice*nLayers];
      if(barrel_ec==0) std::sort(v.begin(), v.end(), SP_INDEX_PAIR::compareZ());
      else std::sort(v.begin(), v.end(), SP_INDEX_PAIR::compareR());
    }
  }

  sps.m_nSpacepoints = nSP;
  sps.m_nPhiSlices = nSlices;
  sps.m_nLayers = nLayers;

  int spIdx=0;
  for(int slice = 0;slice<nSlices;slice++) {
    for(int layer = 0;layer<nLayers;layer++) {
      int layerStart = spIdx;
      std::vector<std::pair<int, const TrigSiSpacePointBase*> >& v = phiLArray[layer + slice*nLayers];
      for(std::vector<std::pair<int, const TrigSiSpacePointBase*> >::iterator it = v.begin();it!=v.end();++it) {
        const TrigSiSpacePointBase* sp  = (*it).second;
        sps.m_index[spIdx] = (*it).first;
        sps.m_type[spIdx] = sp->isPixel() ? 1 : 2;
        sps.m_x[spIdx] = sp->x();
        sps.m_y[spIdx] = sp->y();
        sps.m_z[spIdx] = sp->z();
        sps.m_r[spIdx] = sp->r();
        sps.m_phi[spIdx] = sp->phi();
        sps.m_covR[spIdx] = sp->dr()*sp->dr();
        sps.m_covZ[spIdx] = sp->dz()*sp->dz();
        spIdx++;
      }
      int layerEnd = spIdx;
      sps.m_phiSlices[slice].m_layerBegin[layer] = layerStart;
      sps.m_phiSlices[slice].m_layerEnd[layer] = layerEnd;
    }
  }
  
  return totalSize;

}
