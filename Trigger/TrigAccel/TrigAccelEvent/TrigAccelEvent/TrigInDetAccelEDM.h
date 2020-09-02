/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGACCELEVENT_TRIGINDETACCELEDM_H
#define TRIGACCELEVENT_TRIGINDETACCELEDM_H

#include<cstdint>

namespace TrigAccel {

  //A. GPU-accelerated track seeding
  
  constexpr unsigned int MAX_SILICON_LAYERS           = 50;
  constexpr unsigned int MAX_NUMBER_PIX_MODULES       = 2100;
  constexpr unsigned int MAX_NUMBER_SCT_MODULES       = 8200;
  constexpr unsigned int MAX_NUMBER_SPACEPOINTS       = 100000;
  constexpr unsigned int MAX_PHI_SLICES               = 100;
  constexpr unsigned int MAX_NUMBER_OUTPUT_SEEDS      = 100000;
  
  typedef struct SiliconLayer {
  public:
    int m_subdet;//1 : Pixel, 2 : SCT
    int m_type;//0: barrel, +/-n : endcap
    float m_refCoord;
    int m_nElements;
    float m_minBound, m_maxBound;
    float m_phiBinWidth, m_rzBinWidth;
    int m_nPhiSlices;

  } SILICON_LAYER;

  typedef struct DetectorModel {
  public:
    int m_nLayers;
    int m_nModules;
    SILICON_LAYER m_layers[MAX_SILICON_LAYERS];
    int m_hashArray[MAX_NUMBER_PIX_MODULES+MAX_NUMBER_SCT_MODULES];
    float m_minRZ[MAX_NUMBER_PIX_MODULES+MAX_NUMBER_SCT_MODULES];
    float m_maxRZ[MAX_NUMBER_PIX_MODULES+MAX_NUMBER_SCT_MODULES];
  } DETECTOR_MODEL;

  typedef struct SpacePointLayerRange {
  public:
    int m_layerBegin[MAX_SILICON_LAYERS];
    int m_layerEnd[MAX_SILICON_LAYERS];
  } SPACEPOINT_LAYER_RANGE;
 
  typedef struct SpacePointStorage {
  public:
    int m_nSpacepoints;
    int m_nPhiSlices;
    int m_nLayers;
    int m_index[MAX_NUMBER_SPACEPOINTS];
    int m_type[MAX_NUMBER_SPACEPOINTS];
    float m_x[MAX_NUMBER_SPACEPOINTS];
    float m_y[MAX_NUMBER_SPACEPOINTS];
    float m_z[MAX_NUMBER_SPACEPOINTS];
    float m_r[MAX_NUMBER_SPACEPOINTS];
    float m_phi[MAX_NUMBER_SPACEPOINTS];
    float m_covR[MAX_NUMBER_SPACEPOINTS];
    float m_covZ[MAX_NUMBER_SPACEPOINTS];
    SPACEPOINT_LAYER_RANGE m_phiSlices[MAX_PHI_SLICES];
  } SPACEPOINT_STORAGE;

  typedef struct SeedFinderSettings {
  public:
    unsigned int m_maxBarrelPix, m_minEndcapPix, m_maxEndcapPix, m_maxSiliconLayer; 
    float m_magFieldZ; 
    float m_tripletD0Max; 
    float m_tripletD0_PPS_Max; 
    float m_tripletPtMin; 
    int  m_tripletDoPSS, m_doubletFilterRZ; 
    int m_nMaxPhiSlice; 
    unsigned int m_maxTripletBufferLength; 
    int m_isFullScan;
    float m_zedMinus, m_zedPlus;
    
  } SEED_FINDER_SETTINGS;

  typedef struct SeedMakingJob {
  public:
    SEED_FINDER_SETTINGS m_settings;
    SPACEPOINT_STORAGE m_data;
  } SEED_MAKING_JOB;

  typedef struct OutputSeedStorage {
  public:
    int m_nSeeds;
    int m_nMiddleSps;
    int m_nI, m_nO;
    int m_nCandidates;
    int m_nErrors;
    int m_innerIndex[MAX_NUMBER_OUTPUT_SEEDS];
    int m_middleIndex[MAX_NUMBER_OUTPUT_SEEDS];
    int m_outerIndex[MAX_NUMBER_OUTPUT_SEEDS];
    float m_Q[MAX_NUMBER_OUTPUT_SEEDS];
  } OUTPUT_SEED_STORAGE;

  //B. GPU-accelerated track following
  
  constexpr unsigned int MAX_NUMBER_PIX_HITS          = 100000;
  constexpr unsigned int MAX_NUMBER_SCT_HITS          = 100000;
  constexpr unsigned int MAX_NUMBER_INPUT_SEEDS       = 50000;
  constexpr unsigned int MAX_ROAD_LENGTH              = 64;
  
  typedef struct TrackFinderSettings {
  public:
    unsigned int m_maxBarrelPix, m_minEndcapPix, m_maxEndcapPix, m_maxSiliconLayer; 
    float m_magFieldZ; 
    float m_tripletD0Max; 
    float m_tripletD0_PPS_Max; 
    float m_tripletPtMin; 
    int  m_tripletDoPSS, m_doubletFilterRZ; 
    int m_nMaxPhiSlice; 
    unsigned int m_maxTripletBufferLength; 
    int m_isFullScan;
  } TRACK_FINDER_SETTINGS;
  
  typedef struct SiliconPlane{
  public:
    double m_Ax[3];
    double m_Ay[3];
    double m_Az[3];
    double m_D[3];
    double m_B[3];//magnetic field in the center
    float m_minWidth;
    float m_maxWidth;
    float m_length;
    int m_shape;
  } SILICON_PLANE;

  typedef struct SctGeometryStorage {
  public:
    int m_nModules;
    int m_dead[MAX_NUMBER_SCT_MODULES];
    int m_type[MAX_NUMBER_SCT_MODULES];
    SILICON_PLANE m_geoInfo[MAX_NUMBER_SCT_MODULES];
  } SCT_GEO_STORAGE;

  typedef struct PixelGeometryStorage {
  public:
    int m_nModules;
    int m_dead[MAX_NUMBER_PIX_MODULES];
    int m_type[MAX_NUMBER_PIX_MODULES];
    SILICON_PLANE m_geoInfo[MAX_NUMBER_PIX_MODULES];
  } PIXEL_GEO_STORAGE;

  typedef struct PixelClusterStorage {
  public:
    int m_nModules;
    int m_start[MAX_NUMBER_PIX_MODULES];
    int   m_end[MAX_NUMBER_PIX_MODULES];
    float m_localX[MAX_NUMBER_PIX_HITS];
    float m_localY[MAX_NUMBER_PIX_HITS];
    float m_covXX[MAX_NUMBER_PIX_HITS];
    float m_covXY[MAX_NUMBER_PIX_HITS];
    float m_covYY[MAX_NUMBER_PIX_HITS];
  } PIXEL_CLUSTER_STORAGE;
  
  typedef struct SctClusterStorage {
  public:
    int m_nModules;
    int m_start[MAX_NUMBER_SCT_MODULES];
    int   m_end[MAX_NUMBER_SCT_MODULES];
    float m_localX[MAX_NUMBER_SCT_HITS];
    float m_covXX[MAX_NUMBER_SCT_HITS];
  } SCT_CLUSTER_STORAGE;
  
  typedef struct ProtoTrack {
    int m_nElements;
    int m_nSeedElements;
    float m_initialParams[5];
    int m_planeType[MAX_ROAD_LENGTH];
    int m_planeIndex[MAX_ROAD_LENGTH];
    int m_hitIndex[MAX_ROAD_LENGTH];
    int m_seedPlanes[6];//max = 3 SP x 2 clusters
  } PROTO_TRACK;

  typedef struct ProtoTrackStorage {
  public:
    int m_nSeeds;
    PROTO_TRACK m_tracks[MAX_NUMBER_INPUT_SEEDS];
  } PROTO_TRACK_STORAGE;


  typedef struct TrackSeedStorage {
  public:
    int m_nSeeds;
    int m_planeType[MAX_NUMBER_INPUT_SEEDS];
    int m_planeIdx[MAX_NUMBER_INPUT_SEEDS];
    int m_sp1stPlaneIndices[MAX_NUMBER_INPUT_SEEDS][3];
    int m_sp2ndPlaneIndices[MAX_NUMBER_INPUT_SEEDS][3];
    int m_spClusterIndices[MAX_NUMBER_INPUT_SEEDS][6];
    float m_sp1x[MAX_NUMBER_INPUT_SEEDS];
    float m_sp1y[MAX_NUMBER_INPUT_SEEDS];
    float m_sp1z[MAX_NUMBER_INPUT_SEEDS];
    float m_sp2x[MAX_NUMBER_INPUT_SEEDS];
    float m_sp2y[MAX_NUMBER_INPUT_SEEDS];
    float m_sp2z[MAX_NUMBER_INPUT_SEEDS];
    float m_sp3x[MAX_NUMBER_INPUT_SEEDS];
    float m_sp3y[MAX_NUMBER_INPUT_SEEDS];
    float m_sp3z[MAX_NUMBER_INPUT_SEEDS];

  } TRACK_SEED_STORAGE;

  typedef struct ExtendedTrackStateStruct2 {
    float m_par[10];
    float m_cov[55];
  } EXTENDED_TRACK_STATE_TYPE_2;
  
  typedef struct LocalEstimate {
  public:
    float m_P[5];
    float m_J[5][5];//jacobian to get to the next plane estimate
    float m_path;
  } LOCAL_ESTIMATE;

  typedef struct TrackData {
  public:
    int m_status;
    float m_chi2;
    int m_ndof;
    int m_firstElement;
    int m_lastElement;
    char m_stat[MAX_ROAD_LENGTH];
    char m_validatedPlanes[MAX_ROAD_LENGTH];

    LOCAL_ESTIMATE m_E[MAX_ROAD_LENGTH];
    
    EXTENDED_TRACK_STATE_TYPE_2 m_ets;
    int m_nValidated;
  } TRACK_DATA;

  typedef struct SeededOutput {
  public:
    TRACK_DATA m_data[MAX_NUMBER_INPUT_SEEDS];
  } SEEDED_OUTPUT;

  //C. GPU-accelerated data preparation
  
  constexpr unsigned int MAX_NUMBER_BS_WORDS          = 80000;
  constexpr unsigned int MAX_GANGED_PIXELS            = 4000;
  constexpr unsigned int MAX_GANGED_PIXEL_PER_MODULE  = 50;
  constexpr unsigned int MAX_PIX_BS_HEADERS           = 2048;
  constexpr unsigned int MAX_PIX_HASH                 = 2048;
  constexpr unsigned int MAX_SCT_BS_HEADERS           = 8500;
  constexpr unsigned int MAX_SCT_HASH                 = 8176;
  constexpr unsigned int MAX_BS_ROBF                  = 200;
  constexpr unsigned int MAX_ADJ_HIT_PAIRS            = 1024;
  constexpr unsigned int SCT_MAX_SP_PER_MODULE        = 256;
  constexpr unsigned int MAX_PHI_INDEX                = 100;
  constexpr unsigned int MAX_RZ_INDEX                 = 300;
  constexpr unsigned int MAX_PIX_ROD_INDEX            = 200;
  constexpr unsigned int MAX_PIX_LINK_INDEX           = 8;
  constexpr unsigned int MAX_SCT_ROD_INDEX            = 90;
  constexpr unsigned int MAX_SCT_LINK_INDEX           = 96;
  
  typedef struct SctModuleGeoInfo {
    char m_type;
    float m_phiPitch;
    float m_lorentzShift;
    float m_center[3];
    float m_M[3][3];
    //special meaning for endcap 
    float m_stripLength;
    float m_maxRadius;
    float m_minRadius;
    
  } SCT_MODULE_GEO_INFO;
  
  typedef struct PixelModuleGeoInfo {
    char m_type;
    char m_bLayer;
    float m_phiPitch;
    float m_etaPitchLong;
    float m_etaPitchNormal;
    float m_lorentzShift;
    float m_center[3];
    float m_M[3][3];
    float m_halfWidth;
    float m_halfLength;
  } PIXEL_MODULE_GEO_INFO;

  struct HashQuadruplet {
    unsigned short x,y,z,w;
  };

  typedef struct IdCablingInfo {
  public:
    HashQuadruplet m_pixelRodLinkHashTable[MAX_PIX_ROD_INDEX][MAX_PIX_LINK_INDEX];
    unsigned short m_sctRodLinkHashTable[MAX_SCT_ROD_INDEX][MAX_SCT_LINK_INDEX];
    uint16_t m_pixelModuleInfo[MAX_PIX_HASH];
    uint16_t m_sctModuleInfo[MAX_SCT_HASH];
    PIXEL_MODULE_GEO_INFO m_pixelGeoArray[MAX_PIX_HASH];
    SCT_MODULE_GEO_INFO m_sctGeoArray[MAX_SCT_HASH];
  } ID_CABLING_INFO;

  struct myfloat2 {
    float x,y;
  };

  struct myint4 {
    int x,y,z,w;
  };

  struct myushort4 {
    unsigned short x,y,z,w;
  };

  struct myushort2 {
    unsigned short x,y;
  };

  typedef struct InputByteStreamData {  
    int m_nDataWords;
    uint32_t m_rodIds[MAX_NUMBER_BS_WORDS];
    uint32_t m_words[MAX_NUMBER_BS_WORDS];
    float m_xBeamSpot;
    float m_yBeamSpot; 
  } INPUT_BYTESTREAM_DATA;


  typedef struct DecodedPixelModuleInfo {
    int m_headerPositions;
    int m_trailerPositions;
    int m_gangedStart;
    short m_hashIds;
    unsigned int m_nClusters;
    unsigned int m_nGangedPixels;
  } DECODED_PIX_MODULE_INFO;

  typedef struct DecodedPixelHitInfo {
    unsigned int m_clusterIds;
    unsigned short m_etaIndex;
    unsigned short m_phiIndex;
    unsigned int m_tot;
  } DECODED_PIX_HIT_INFO;

  typedef struct PixelSpacePointType {
    float m_position[3];
    unsigned short m_clusterIdx;
  } PIXEL_SPACEPOINT_TYPE;
  

  typedef struct DecodedPixelData {
  public:
    int m_nHeaders, m_nTrailers;
    int m_nSpacePoints;
    
    int m_gangedPixelsStart;//initial value = m_nDataWords
    int m_nPixels[MAX_PIX_BS_HEADERS];//number of decoded pixels (counting ganged) per module
    int m_clusterStarts[MAX_PIX_BS_HEADERS];//for navigation through cluster and SP storage space
    DECODED_PIX_MODULE_INFO m_modulesInfo[MAX_PIX_BS_HEADERS];
    unsigned short m_moduleInfoWord[MAX_PIX_BS_HEADERS];
    myfloat2 m_clusterPosition[MAX_NUMBER_BS_WORDS+MAX_GANGED_PIXELS];
    unsigned short m_clusterId[MAX_NUMBER_BS_WORDS+MAX_GANGED_PIXELS];
    DECODED_PIX_HIT_INFO m_hitInfo[MAX_NUMBER_BS_WORDS+MAX_GANGED_PIXELS];
    myint4 m_decodedData[MAX_NUMBER_BS_WORDS+MAX_GANGED_PIXELS];
  
    PIXEL_SPACEPOINT_TYPE m_spacePoints[MAX_NUMBER_BS_WORDS+MAX_GANGED_PIXELS]; 
    
    int m_hashToIndex[MAX_PIX_HASH];

  } DECODED_PIXEL_DATA;

  typedef struct DecodedSctHeaderInfo {
    int m_headerPositions;
    int m_rdoEnd;
    short m_hashIds;
    unsigned int m_nClusters;
    bool m_condensedMode;
    
    unsigned int m_nSP;
    unsigned int m_spacePointsStart;
  } DECODED_SCT_HEADER_INFO;
  
  typedef struct SctSpacePointStruct {
    float m_position[3];
    unsigned short m_clusterIdx[2];
  } SCT_SPACEPOINT_TYPE;

  typedef struct DecodedSctData {
    int m_nHeaders;
    int m_nTrailers; 
    int m_nPhiModules;
    unsigned int m_nSpacePoints;
    
    DECODED_SCT_HEADER_INFO m_headersInfo[MAX_SCT_BS_HEADERS];
    unsigned short m_moduleInfoWord[MAX_SCT_BS_HEADERS];
    unsigned short m_headerInfoIndices[MAX_SCT_HASH];//starts from 1 !!!
    unsigned short m_phiModuleIndices[MAX_SCT_BS_HEADERS/2];
    unsigned short m_clusterIds[MAX_NUMBER_BS_WORDS*2];
    float   m_clusterPosition[MAX_NUMBER_BS_WORDS*2];
    SCT_SPACEPOINT_TYPE m_spacePoints[SCT_MAX_SP_PER_MODULE*MAX_SCT_BS_HEADERS/2]; 
    myushort4 m_decodedData[MAX_NUMBER_BS_WORDS*2];
    myushort2 m_clusterInfo[MAX_NUMBER_BS_WORDS*2];
    
  } DECODED_SCT_DATA;
  
}

#endif
