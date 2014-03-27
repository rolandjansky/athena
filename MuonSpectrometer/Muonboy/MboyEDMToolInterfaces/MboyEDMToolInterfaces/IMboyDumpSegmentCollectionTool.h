/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyDumpSegmentCollectionTool_H
#define IMboyDumpSegmentCollectionTool_H

#include "GaudiKernel/IAlgTool.h" 

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

  /**
   @class IMboyDumpSegmentCollectionTool

   This class is dumping Trk::Segment collections
   
  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IIMboyDumpSegmentCollectionTool("IMboyDumpSegmentCollectionTool", 1, 0);

class IMboyDumpSegmentCollectionTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /**Dump collection */
   virtual StatusCode DoDump(
                     std::string m_SegmentCollectionLocation,
                     std::string m_TrackCollectionLocation,
		     std::ofstream* pOutFile
		    ) = 0;

   /**Dump collection */
   virtual StatusCode DoDump(
                     std::string m_SegmentCollectionLocation,
		     std::ofstream* pOutFile
		    ) = 0;

};

inline const InterfaceID& IMboyDumpSegmentCollectionTool::interfaceID() { return IID_IIMboyDumpSegmentCollectionTool; }

#endif

