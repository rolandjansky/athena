/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyDigiEmptyLoop_H
#define MboyDigiEmptyLoop_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

/////////////////////////////////////////////////////////////////////////////
#include "MuonboyAth/MuonboyAthMisc.h"

class IMboyGetPrepDataFromEventTool;

  /**
   @class MboyDigiEmptyLoop

   This class is a top algo the only function of which is
   to loop on PrepRawaData containers before the reconstruction
   it self,in order to not count the corresponding time in the
   reconstruction time

  @author samusog@cern.ch
  
  */

class MboyDigiEmptyLoop:public AthAlgorithm {
public:
    MboyDigiEmptyLoop(const std::string& name, ISvcLocator* pSvcLocator);

public:
///////////////////////////////////

   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:
///////////////////////////////////

   int m_SwitchOff ;//!< Switch Off option

   ToolHandle< IMboyGetPrepDataFromEventTool > p_IMboyGetPrepDataFromEventTool ;//!< Pointer On IMboyGetPrepDataFromEventTool

};

#endif

