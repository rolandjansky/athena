/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyDumpMboyStoreTool_H
#define IMboyDumpMboyStoreTool_H

#include "GaudiKernel/IAlgTool.h" 

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthToolInterfaces/IMboyAthToolMisc.h"

  /**
   @class IMboyDumpMboyStoreTool
   
  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IIMboyDumpMboyStoreTool("IMboyDumpMboyStoreTool", 1, 0);

class IMboyDumpMboyStoreTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /**Dump Mboy Store */
   virtual StatusCode dumpMboyStore(
                                    std::string SegmentStoreLocation              , 
                                    std::string TrackStoreLocation                ,
                                    std::ofstream*  pOutMuonboySegmentStore       ,
                                    std::ofstream*  pOutMuonboySegmentStoreChecks ,
                                    std::ofstream*  pOutMuonboyTrackStore         ,
                                    std::ofstream*  pOutMuonboyTrackStoreChecks  
                                   ) = 0 ;
                                    
};

inline const InterfaceID& IMboyDumpMboyStoreTool::interfaceID() { return IID_IIMboyDumpMboyStoreTool; }

#endif

