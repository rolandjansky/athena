/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyGetPrepDataFromEventTool_H
#define IMboyGetPrepDataFromEventTool_H

#include "GaudiKernel/IAlgTool.h" 


namespace Trk {
  class  PrepRawData;
}

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

  /**
   @class IMboyGetPrepDataFromEventTool

   This class is extracting PrepRawData vectors
   
  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IIMboyGetPrepDataFromEventTool("IMboyGetPrepDataFromEventTool", 1, 0);

class IMboyGetPrepDataFromEventTool:virtual public IAlgTool {

public:
///////////////////////////////////

  static const InterfaceID& interfaceID();

   /** Collect PrepRawData vectors */
  virtual StatusCode CollectPrepData      ( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData ) = 0;
  virtual StatusCode CollectPrepDataMDT   ( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData ) = 0;
  virtual StatusCode CollectPrepDataRPC   ( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData ) = 0;
  virtual StatusCode CollectPrepDataTGC   ( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData ) = 0;
  virtual StatusCode CollectPrepDataCSC   ( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData ) = 0;
  virtual StatusCode CollectPrepDataCSCClu( std::vector<const Trk::PrepRawData*>& VectorOfpPrepData ) = 0;

};

inline const InterfaceID& IMboyGetPrepDataFromEventTool::interfaceID() { return IID_IIMboyGetPrepDataFromEventTool; }

#endif

