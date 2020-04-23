/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

*/

#ifndef RD53B_ENCODINGALG_H
#define RD53B_ENCODINGALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "../src/ChipMap_RD53B.h"


class RD53BEncodingTool;

typedef ChipMap_RD53B ChipMap;

namespace InDetDD {
  class PixelDetectorManager;
}

class PixelID;


class RD53BEncodingAlg : public AthAlgorithm {

 public:
  RD53BEncodingAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~RD53BEncodingAlg(){}

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:
   
  const static int N_LAYERS=5;
  
  enum LayerType {
    INVALID_LAYER=-1, INNERMOST, NEXT_TO_INNERMOST, OUTER, N_TYPES    
  };
  
  enum Region {
    INVALID_REGION=-1, BARREL, ENDCAP, N_REGIONS
  };
  
  std::map<int, std::string > m_layerTypeLabels {
    {INVALID_LAYER, "invalid"}, {INNERMOST, "innermost"}, {NEXT_TO_INNERMOST, "nextToInnermost"}, {OUTER, "outer"}
  };
  
  std::map<int, std::string > m_regionLabels {
    {INVALID_REGION, "invalid"}, {BARREL, "barrel"}, {ENDCAP, "endcap"}
  };
   
  StatusCode bookHistograms();
  
  StatusCode initializeStreams(const ToolHandleArray< RD53BEncodingTool >& encondingTools);
  
  void fillChipMaps();  
  
  int findChip(int eta, int phi);

  const InDetDD::PixelDetectorManager*           m_pixelManager;                   //!< the Si Detector Manager
  const PixelID*                                 m_pixIdHelper;                    //!< pixel Id Helper 
  
  // this is now a bunch of histograms we need to add
  std::string m_path;
  ServiceHandle<ITHistSvc> m_thistSvc;
   
  // checks the chip numbering and core numbering
  TH2* m_chips[N_REGIONS][N_TYPES];
  TH2* m_cores_on_chip[N_REGIONS][N_TYPES];
  
  ToolHandleArray< RD53BEncodingTool > m_encodingTools;
  
  int m_split_chip_eta;
  int m_split_chip_phi;  
  int m_cols        ;
  int m_rows        ;  
  int m_cols_core   ;
  int m_rows_core   ;
  int m_colsbtwchips;
  int m_rowsbtwchips;
  bool m_use50x50;
  
  int m_event;

};

#endif // RD53B_ENCODINGALG_H

