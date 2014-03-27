/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyDumpTrackCollectionTool_H
#define IMboyDumpTrackCollectionTool_H

#include "GaudiKernel/IAlgTool.h" 

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

  /**
   @class IMboyDumpTrackCollectionTool

   This class is dumping Trk::Track collections
   
  @author samusog@cern.ch
   
  */

static const InterfaceID IID_IIMboyDumpTrackCollectionTool("IMboyDumpTrackCollectionTool", 1, 0);

class IMboyDumpTrackCollectionTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /**Dump collection */
   virtual StatusCode DoDump(
                     std::string TrackCollectionLocation,
		     std::ofstream* pOutFile
		    ) = 0 ;

};

inline const InterfaceID& IMboyDumpTrackCollectionTool::interfaceID() { return IID_IIMboyDumpTrackCollectionTool; }

#endif

