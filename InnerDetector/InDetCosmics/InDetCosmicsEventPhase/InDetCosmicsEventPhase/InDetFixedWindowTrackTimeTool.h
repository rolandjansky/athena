/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetCosmicsEventPhaseTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef InDetInDetFixedWindowTrackTimeTool_h
#define InDetInDetFixedWindowTrackTimeTool_h

#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrkTrack/Track.h"
#include "TrkSegment/TrackSegment.h"
#include "TRT_ConditionsData/AverageT0.h"
#include "TRT_ConditionsServices/ITRT_CalDbTool.h"
#include "InDetCosmicsEventPhase/IInDetCosmicsEventPhaseTool.h"

class AtlasDetectorID;
class Identifier;

namespace InDet 
{

  /** @class InDetFixedWindowTrackTimeTool
  @author  Brett Jackson <brett.david.jackson@cern.ch>
  
  This tool is essentially copy and pasted from the TRTCosmicsEventPhase algorithm.
  */  
  
  class InDetFixedWindowTrackTimeTool : virtual public IInDetCosmicsEventPhaseTool, public AthAlgTool
    {
    public:
      InDetFixedWindowTrackTimeTool(const std::string&,const std::string&,const IInterface*);
      
      /** default destructor */
      virtual ~InDetFixedWindowTrackTimeTool ();
      
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
      
      SG::ReadCondHandleKey<TRTCond::AverageT0> m_T0ReadKey{this,"T0ReadKey","AverageT0","Average T0 in-key"};            
      ToolHandle<ITRT_CalDbTool> m_caldbtool ;//!< TRT Calibration DB tool
      
      double m_globalOffset; 
      bool m_useNewEP;
      double m_windowCenter;
      double m_windowSize;
    }; 
}

#endif 
