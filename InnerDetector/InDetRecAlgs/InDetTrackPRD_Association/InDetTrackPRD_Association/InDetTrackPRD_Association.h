/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef InDetTrackPRD_Association_H
#define InDetTrackPRD_Association_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"

namespace InDet {


  // Class-algorithm for track accotiation with PRD 
  // 
  class InDetTrackPRD_Association : public AthAlgorithm 
    {
    
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard Algotithm methods
      ///////////////////////////////////////////////////////////////////

      InDetTrackPRD_Association(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~InDetTrackPRD_Association() {}
      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();

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
      int                                   m_tracksPRD [10];
      int                                   m_tracksPRDn[10];
      int                                   m_nprint        ;
      bool                                  m_istracks  [10];

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out) const;
    };
  MsgStream&    operator << (MsgStream&   ,const InDetTrackPRD_Association&);
  std::ostream& operator << (std::ostream&,const InDetTrackPRD_Association&); 
}
#endif // InDetTrackPRD_Association_H
