/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DetectorPerformance_TRT_DetectorTiming_H
#define TRT_DetectorPerformance_TRT_DetectorTiming_H

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

//#include "DataModel/DataVector.h"

#include "Identifier/Identifier.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetRawData/TRT_RDO_Container.h"


namespace InDetDD {
  class TRT_DetectorManager;
}
class ITRT_CalDbSvc;
class ITRT_StrawNeighbourSvc;
namespace InDet {
  class TRT_StrawMap;
}


class ITHistSvc;
class TTree;



/////////////////////////////////////////////////////////////////////////////

class TRTCalDbTool ;

namespace InDet 
{
  
  class TRT_DetectorTiming : public AthAlgorithm {

  public:

    TRT_DetectorTiming (const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

    class hitParams;
  
  private:

//    const InDetDD::TRT_DetectorManager *m_trtDetMgr;

    int m_numTrtOnlyTracks;
    int m_numTrtSctTracks;  

    ServiceHandle<ITRT_CalDbSvc> m_trtCondDbTool ;//!< TRT Calibration DB tool
    ServiceHandle<ITRT_StrawNeighbourSvc> m_trtStrawNeighbourTool;
    ToolHandle<InDet::TRT_StrawMap> m_trtStrawMap;
 
    ServiceHandle<ITHistSvc> m_histSvc;
    TTree* m_ntuple;
  
    double m_correctedLeadingEdge;  
    double m_timeCor; 
    double m_timeOverThreshold; 
    int    m_trailingEdgeBin;
    int    m_leadingEdgeBin; 
    int    m_wrongSide;
    int    m_board;
    double m_locR; 
    double m_timeCorChip;
    int    m_chip;
    int    m_side; 
    double m_t0;
    int    m_phi;
    int    m_straw; 
    int    m_strawLayer;
    int    m_layer;
    int    m_nTrtBarHits; 
    int    m_nTrtEcHits;
    int    m_nTrtHitsBarrelA; 
    int    m_nTrtHitsBarrelC; 
    int    m_nTrtHitsEcA; 
    int    m_nTrtHitsEcC; 
    int    m_nSctHits; 
    int    m_nPixHits; 
    double m_theta;
    double m_eta;
    double m_chi2Ndf; 
    double m_phi0; 
    double m_z0; 
    double m_d0; 
    int    m_tubeHit;
    double m_locRPat;

    
  };
  
 // struct to hold all relevant params for a given hit
 // -- don't need eventPhase since that is constant for an event
  class TRT_DetectorTiming::hitParams {
  public:
    double x;
    double y;
    int    leadingEdgeBin;
    int    trailingEdgeBin;
    double leadingEdge;
    double correctedLeadingEdge;
    int    side;
    int    wrongSide;
    double timeOverThreshold;
    int    board;  
    double locR;
    double localPos;
    double timeCorChip;
    int    chip;
    double t0;
    int    phi;
    int    straw;
    int    strawLayer;
    int    layer;
    int    tubeHit;
    double locRPat;
  };

} // end namespace 


#endif // MY_TRT_TEMPLATE_MY_ALG_H
