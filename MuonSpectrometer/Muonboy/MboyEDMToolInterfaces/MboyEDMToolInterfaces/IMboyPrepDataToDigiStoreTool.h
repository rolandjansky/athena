/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyPrepDataToDigiStoreTool_H
#define IMboyPrepDataToDigiStoreTool_H

#include "GaudiKernel/IAlgTool.h" 

namespace Trk {
  class  PrepRawData;
}

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

class MuonRecDigiStoreAth;

  /**
   @class IMboyPrepDataToDigiStoreTool

   This class is filling a MuonRecDigiStoreAth with PrepRawData

  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IIMboyPrepDataToDigiStoreTool("IMboyPrepDataToDigiStoreTool", 1, 0);

class IMboyPrepDataToDigiStoreTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /** Fill with PrepRawData vectors */
   virtual void FillMuonRecDigiStoreWithVectorOfPrepData_Base(
                                          const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData,
                                          MuonRecDigiStoreAth* pMuonRecDigiStoreAth
                                          ) = 0 ;

   /** Fill with PrepRawData vectors */
   virtual void FillMuonRecDigiStoreWithPrepData_Base(
                                          const Trk::PrepRawData* pPrepData,
                                          MuonRecDigiStoreAth* pMuonRecDigiStoreAth
                                          ) = 0 ;

};

inline const InterfaceID& IMboyPrepDataToDigiStoreTool::interfaceID() { return IID_IIMboyPrepDataToDigiStoreTool; }

#endif

