/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetTrigTrackPRD_Association_H
#define InDetTrigTrackPRD_Association_H

#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"

//!< Trigger specific stuff
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigInterfaces/FexAlgo.h"


namespace InDet {


  // Class-algorithm for track accotiation with PRD 
  // 
  class InDetTrigTrackPRD_Association : public HLT::FexAlgo 
    {
    
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard Algotithm methods
      ///////////////////////////////////////////////////////////////////

      InDetTrigTrackPRD_Association(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~InDetTrigTrackPRD_Association() {}
      HLT::ErrorCode hltInitialize();
      HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);    
      HLT::ErrorCode hltFinalize();      

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump     (MsgStream&    out) const;
      std::ostream& dump     (std::ostream& out) const;

    protected:

      ///////////////////////////////////////////////////////////////////
      // Protected data 
      ///////////////////////////////////////////////////////////////////
     
      std::vector<std::string>              m_tracksName    ; // Name of track collections       
      ToolHandle<Trk::IPRD_AssociationTool> m_assoTool      ; // Assotiation tool
      int                                   m_outputlevel   ;
      int                                   m_tracksPRD[10];
      int                                   m_tracksPRDn[10];
      int                                   m_nprint;
      bool                                  m_istracks[10];

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out) const;
    };
  MsgStream&    operator << (MsgStream&   ,const InDetTrigTrackPRD_Association&);
  std::ostream& operator << (std::ostream&,const InDetTrigTrackPRD_Association&); 
}
#endif // InDetTrigTrackPRD_Association_H
