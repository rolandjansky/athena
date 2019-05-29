/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetCosmicsEventPhaseTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef InDetInDetSlidingWindowTrackTimeTool_h
#define InDetInDetSlidingWindowTrackTimeTool_h

#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TRT_ConditionsData/AverageT0.h"

#include "TRT_ConditionsServices/ITRT_CalDbTool.h"
#include "InDetCosmicsEventPhase/IInDetCosmicsEventPhaseTool.h"

class AtlasDetectorID;
class Identifier;
namespace Trk{
  class Track;
  class Segment;
}

namespace InDet 
{

  /** @class InDetSlidingWindowTrackTimeTool
  @author  Brett Jackson <brett.david.jackson@cern.ch>
  
  This tool is essentially copy and pasted from the TRTCosmicsEventPhase algorithm.
  */  
  
  class InDetSlidingWindowTrackTimeTool : virtual public IInDetCosmicsEventPhaseTool, public AthAlgTool
    {
    public:
      InDetSlidingWindowTrackTimeTool(const std::string&,const std::string&,const IInterface*);
      
      /** default destructor */
      virtual ~InDetSlidingWindowTrackTimeTool ();
      
      /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
      /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      
      /** finds event phase of a track from the leading edge */
      double findPhase(const Trk::Track *track) const;
      
      /** finds event phase of a segment from the leading edge */
      double findPhase(const Trk::Segment *segment) const;
      
      /** finds event phase of a track from the trailing edge */
      double findPhaseFromTE(const Trk::Track *track) const;
      
    private:
      
      
      ToolHandle<ITRT_CalDbTool> m_caldbtool ;//!< TRT Calibration DB tool
      SG::ReadCondHandleKey<TRTCond::AverageT0> m_T0ReadKey{this,"T0ReadKey","AverageT0","Average T0 in-key"};            
      double m_globalOffset; 
      bool m_useNewEP;
      int m_nIterations;
      double m_windowSize;
    }; 
}

#endif 
