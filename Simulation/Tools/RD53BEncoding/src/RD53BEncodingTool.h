/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

*/

#ifndef RD53BENCODINGTOOL_H
#define RD53BENCODINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include "Identifier/Identifier.h"

#include "./ChipMap_RD53B.h"
#include "./Stream.h"
#include "./StreamMap.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include <iostream>
#include <fstream>

typedef ChipMap_RD53B ChipMap;

class PixelID;

static const InterfaceID IID_RD53BEncodingTool("RD53BEncodingTool", 1, 0);

class RD53BEncodingTool : public AthAlgTool {
public:
  RD53BEncodingTool(const std::string &type,
                   const std::string &name,
                   const IInterface *parent);
  
  ~RD53BEncodingTool() {};
  
  static const InterfaceID& interfaceID() { return IID_RD53BEncodingTool; };
  
  StatusCode initialize();
  StatusCode finalize();
  
  StatusCode addStream(Identifier id, int chips);
  
  StatusCode bookHistograms(std::vector < std::vector < float > > barrel_z,
                            std::vector < std::vector < float > > endcap_z);
  
  void fillStreams(std::vector<ChipMap>& chip_maps, Identifier id, int event);
  
  void resetStreamsIfNeeded(int event);
  
  void fillDataRates();
  
  void fillDataRates(int event);
  
private:
  
  const static int N_LAYERS=8;
  
  enum Region {
    INVALID_REGION=-1, BARREL, ENDCAP, N_REGIONS
  };
  
  std::map<int, std::string > m_regionLabels {
    {INVALID_REGION, "invalid"}, {BARREL, "barrel"}, {ENDCAP, "endcap"}
  };
  
  void testStream(ChipMap& chipmap, 
                  std::vector <float>& streams, 
                  std::vector <float>& cores,
                  int& chipId, int event);
  
  void fillStreams(ChipMap& chip_map, Identifier id, int chip, int event);
  
  void createStream(ChipMap& map, 
                    std::vector<float>& streams,
                    std::vector<float>& tags,
                    std::vector<float>& addressing,
                    std::vector<float>& totbits,
                    std::vector<float>& data,
                    std::vector<float>& orphans,
                    std::vector<float>& cores,
                    int& streams_per_event,
                    int event, bool test=false);
  
  void fillHistograms(ChipMap chipmap, int streams_per_event, float z, bool isBarrel, int layer);
 
  const PixelID* m_pixIdHelper;
  
  StreamMap m_stream_map;
  
  bool m_addressCompression;
  bool m_bitTreeCompression;
  bool m_suppressToT;
  bool m_enableLinkSharing;
  float m_auroraFactor;
  float m_safetyFactor;
  int m_eventsPerStream;
  int m_testevent;
  
  // this is now a bunch of histograms we need to add
  std::string m_path;
  ServiceHandle<ITHistSvc> m_thistSvc;
  
  // for testing the stream creation
  bool m_testStream;
  std::string m_testFileName;
  int m_testBarrelEndcap;
  int m_testLayerDisc   ;
  int m_testModuleEta   ;
  int m_testModulePhi   ;
  std::vector <float> m_testStreams;
  std::vector <float> m_testCores;  
  std::ofstream m_testChipFile;
  std::ofstream m_testStreamFile; 
  
  bool m_doExpertPlots;  
  bool m_debug;
  
  std::vector < std::vector < float > > m_module_z_layer[N_REGIONS];  
  
  // datarates and stream information  
  TProfile* m_p_datarate[N_LAYERS][N_REGIONS];    
  TProfile* m_p_datarate_w_safety_factor[N_LAYERS][N_REGIONS];    
  TProfile* m_p_streamlength_per_stream[N_LAYERS][N_REGIONS];    
  TProfile* m_p_streamlength_per_stream_incl_prot[N_LAYERS][N_REGIONS];  
  TProfile* m_p_cores_per_stream[N_LAYERS][N_REGIONS];    
  TProfile* m_p_perc_orphans_per_stream[N_LAYERS][N_REGIONS];    
  TProfile* m_p_perc_tot_per_stream[N_LAYERS][N_REGIONS];    
  TProfile* m_p_perc_bittree_per_stream[N_LAYERS][N_REGIONS];    
  TProfile* m_p_perc_tags_per_stream[N_LAYERS][N_REGIONS];    
  TProfile* m_p_perc_addressing_per_stream[N_LAYERS][N_REGIONS];    
  TProfile* m_p_orphans_per_stream[N_LAYERS][N_REGIONS];    
  TProfile* m_p_tot_per_stream[N_LAYERS][N_REGIONS];    
  TProfile* m_p_bittree_per_stream[N_LAYERS][N_REGIONS];  
  TProfile* m_p_tags_per_stream[N_LAYERS][N_REGIONS];    
  TProfile* m_p_addressing_per_stream[N_LAYERS][N_REGIONS];    
  TH2* m_h2_datarate[N_LAYERS][N_REGIONS];  
  TH2* m_h2_streamlength_per_stream[N_LAYERS][N_REGIONS];    
  TH2* m_h2_streamlength_per_stream_incl_prot[N_LAYERS][N_REGIONS];   
  
  // expert histograms
  TProfile* m_p_chip_occupancy[N_LAYERS][N_REGIONS];  
  TProfile* m_p_hits_per_chip[N_LAYERS][N_REGIONS];  
  TProfile* m_p_cores_per_chip[N_LAYERS][N_REGIONS];  
  TProfile* m_p_hits_per_core[N_LAYERS][N_REGIONS];  
  TProfile* m_p_streams_per_event[N_LAYERS][N_REGIONS];
  TH2* m_h2_hits_per_chip[N_LAYERS][N_REGIONS];  
  TH2* m_h2_data_per_core[N_LAYERS][N_REGIONS];  
  TH2* m_h2_bittree_per_core[N_LAYERS][N_REGIONS];  
  TH2* m_h2_tot_per_core[N_LAYERS][N_REGIONS];  
  TH2* m_h2_cores_per_chip[N_LAYERS][N_REGIONS];  
  TH2* m_h2_qrows_per_ccol[N_LAYERS][N_REGIONS];  
  TH2* m_h2_neig_qrows_per_ccol[N_LAYERS][N_REGIONS];
  TH2* m_h2_streams_per_event[N_LAYERS][N_REGIONS];
  
  TH2* m_h2_cores_per_stream[N_LAYERS][N_REGIONS];    
  TH2* m_h2_orphans_per_stream[N_LAYERS][N_REGIONS];    
  TH2* m_h2_tot_per_stream[N_LAYERS][N_REGIONS];    
  TH2* m_h2_bittree_per_stream[N_LAYERS][N_REGIONS];    
  TH2* m_h2_tags_per_stream[N_LAYERS][N_REGIONS];    
  TH2* m_h2_addressing_per_stream[N_LAYERS][N_REGIONS];  
  
};

#endif // RD53BENCODINGTOOL_H
