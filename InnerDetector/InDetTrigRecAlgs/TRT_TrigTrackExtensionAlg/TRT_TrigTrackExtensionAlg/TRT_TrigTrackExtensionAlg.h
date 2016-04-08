/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
// filename: TRT_TrigTrackExtensionAlg.h
//
// author: Patricia Conde Muino 13/09/2005
//         Patricia.Conde.Muino@cern.ch
//
// Description:  Trigger top algorithm extend the tracks to the TRT. Based on
//              TRT_TrackExtensionAlg but working as an 
//              HLT algorithm (it runs once per ROI and it reconstructs only 
//              the region of interest indicated in the job options).
//
// Modified :
//
// -------------------------------
// ATLAS Collaboration
*/
////////////////////////////////////////////////////////////////////////////

#ifndef TRT_TRIGTRACKEXTENSIONALG_H
#define TRT_TRIGTRACKEXTENSIONALG_H

#include <string>
#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"

class ITRT_TrackExtensionTool;
class MsgStream;

namespace InDet {

  class ITRT_TrackExtensionTool;

  class TRT_TrigTrackExtensionAlg : public HLT::FexAlgo 

  {
    
    ///////////////////////////////////////////////////////////////////
    //!< Public methods:
    ///////////////////////////////////////////////////////////////////
    
  public:
    
    ///////////////////////////////////////////////////////////////////
    //!< Standard Algotithm methods
    ///////////////////////////////////////////////////////////////////
    
    TRT_TrigTrackExtensionAlg(const std::string &name, 
			      ISvcLocator *pSvcLocator);
    virtual ~TRT_TrigTrackExtensionAlg() {};
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
    HLT::ErrorCode hltFinalize();
    
    ///////////////////////////////////////////////////////////////////
    //!< Print internal algorithm parameters and status
    ///////////////////////////////////////////////////////////////////
    
    MsgStream&    dump     (MsgStream&    out) const;
    std::ostream& dump     (std::ostream& out) const;
    
  protected:
    
    ///////////////////////////////////////////////////////////////////
    //!< Protected data 
    ///////////////////////////////////////////////////////////////////
    
    int                      m_outputlevel           ;  //!< Print level for debug
    int                      m_nprint                ;  //!< Kind of  print    
    int                      m_nTracks               ;  //!< Number input  tracks
    int                      m_nTracksExtended       ;  //!< Number output tracks
    int                      m_nTracksTotal          ;  // Number input  tracks
    int                      m_nTracksExtendedTotal  ;  // Number output tracks
    ToolHandle<ITRT_TrackExtensionTool> m_trtExtension ;  // TRT track extension tool
        
    ///////////////////////////////////////////////////////////////////
    //!< Protected methods
    ///////////////////////////////////////////////////////////////////
    
    MsgStream&    dumpConditions(MsgStream&    out) const;
    MsgStream&    dumpEvent     (MsgStream&    out) const;
    
  };
  
  MsgStream&    operator << (MsgStream&   ,const TRT_TrigTrackExtensionAlg&);
  std::ostream& operator << (std::ostream&,const TRT_TrigTrackExtensionAlg&); 
}
#endif // TRT_TRIGTRACKEXTENSIONALG_H
