/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DetectorPerformance_TRT_ClockNoise_H
#define TRT_DetectorPerformance_TRT_ClockNoise_H

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <utility>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

//#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"

//#include "DataModel/DataVector.h"



namespace InDetDD {
  class TRT_DetectorManager;
}
class ITRT_StrawNeighbourSvc;
namespace InDet {
  class TRT_StrawMap;
}

class ITHistSvc;
class TTree;




// constants for numbers of detector elements, time bins
#define nPhi 32
#define nSide 2
#define nStrawB 1642 // number of straws in a barrel stack
#define nStraw 5482 // number of straws in a barrel + endcap stack
#define nBin 24

// noise periods
#define PERIOD_40MHZ 8
#define PERIOD_80MHZ 4


/////////////////////////////////////////////////////////////////////////////

namespace InDet 
{
  
  class TRT_ClockNoise : public AthAlgorithm {

  public:

    TRT_ClockNoise (const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:

//    const InDetDD::TRT_DetectorManager *m_trtDetMgr;

    ServiceHandle<ITRT_StrawNeighbourSvc> m_trtStrawNeighbourTool;
    ToolHandle<InDet::TRT_StrawMap> m_trtStrawMap;
    ServiceHandle<ITRT_StrawStatusSummarySvc> m_TRTStrawStatusSummarySvc;
    
    // flag to write ntuple
    bool m_print_ntuple;
    
    // output text file
    std::string m_textfile;
    
    // ----------------------------------
    // for clock noise cut:
    // ----------------------------------
    // minimum number of noise hits
    int m_min_noise_hits;
    // minimum asymmetry value
    float m_min_asym;
    // ----------------------------------
    
    // methods to look for clock noise
    int classify_noise(); 
    float find_max_asymmetry(int occ[24], int period, int* phase, float* occ_above_thermal);
    float find_asymmetry(int occ[24], int period, int phase, float* occ_above_thermal);
    
    // implement definition of clock noise in this method
    int clock_noise_cut(int totalocc, float asym);
    
    // discrete fourier transform, to pick out the frequency of noise components
    int DFT(int occ[24], float* comp0mhz, float* comp40mhz, int* phase40mhz, float* comp80mhz, int* phase80mhz);
    
    // find the phase of a noise component, from DFT output
    int find_dft_phase(float re, float im, int period, int offset);

    // convert (0,1) for (C,A) side back to standard (-2,-1,1,2)
    int recover_side(int AorC, int straw);
    
    // array for storing the occupancy for each straw, by LEbin
    // indices are: phi, side (0 for C, 1 for A), straw#, LEbin
    int m_straw_bin_occ[nPhi][nSide][nStraw][nBin]; 
    
    // ---- for output tree ---------------  
    ServiceHandle<ITHistSvc> m_histSvc;
    TTree* m_ntuple;
  
    int    m_leadingEdgeBin; 
    int    m_board;
    int    m_chip;
    int    m_pad;
    int    m_side; 
    int    m_phi;
    int    m_straw; 
    int    m_strawLayer;
    int    m_layer;
    int    m_evtNum;

  };
  
  
} // end namespace 


#endif // MY_TRT_TEMPLATE_MY_ALG_H
