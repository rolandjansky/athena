/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyGenTrackTool_H
#define IMboyGenTrackTool_H

#include "GaudiKernel/IAlgTool.h" 

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

static const InterfaceID IID_IIMboyGenTrackTool("IMboyGenTrackTool", 1, 0);

class MboyGenTrackStore;

  /**
   @class IMboyGenTrackTool

   This class is extracting generated track collection

  @author samusog@cern.ch
  
  */

class IMboyGenTrackTool :virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /** Get generated tracks collections */
   virtual StatusCode GetMboyGenTrackStore(
               MboyGenTrackStore*& pMboyGenTrackStore,
               std::string TrackRecordCollectionLocation
              ) = 0;

};

inline const InterfaceID& IMboyGenTrackTool::interfaceID() { return IID_IIMboyGenTrackTool; }

#endif

