/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetCosmicsEventPhaseTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef InDetInDetFixedWindowTrackTimeTool_h
#define InDetInDetFixedWindowTrackTimeTool_h

#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"

#include "TrkTrack/Track.h"
#include "TrkSegment/TrackSegment.h"

#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"
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
      
      /** if m_useNewEP = True, set averageT0 != 0 */
      void beginRun();
      
      /** finds event phase of a track from the leading edge */
      double findPhase(const Trk::Track *track);
      
      /** finds event phase of a segment from the leading edge */
      double findPhase(const Trk::Segment *segment);
      
      /** finds event phase of a track from the trailing edge */
      double findPhaseFromTE(const Trk::Track *track);
      
    private:
      
      double m_averageT0; // subtract average T0 in EP calculation (new April 2009)
      
      ServiceHandle<ITRT_CalDbSvc> m_trtconddbsvc ;//!< TRT Calibration DB tool
      
      bool m_gett0;
      bool m_useNewEP;
      double m_globalOffset; 

      double m_windowCenter;
      double m_windowSize;
    }; 
}

#endif 
