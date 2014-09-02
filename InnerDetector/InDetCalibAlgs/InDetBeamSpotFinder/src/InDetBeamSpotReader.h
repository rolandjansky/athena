/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_INDETBEAMSPOTREADER_H
#define INDET_INDETBEAMSPOTREADER_H
//////////////////////////////
// Author jwalder@cern.ch
// Simple Alogrithm to read the current beamspot value
//////////////////////////////


#include <string>
#include <vector>
#include <map>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h" 

class IBeamCondSvc;

namespace InDet {
  /** An example class of how to read beamspot position.
      Currently there must be a valid beamspot entry for the IOV of data that is 
      being read. Otherwise an error will occur.
   */
  class InDetBeamSpotReader : public AthAlgorithm {
    
  public:
    //Standard constructor and methods
    InDetBeamSpotReader  (const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    
  
  private:
    ServiceHandle<IToolSvc> m_toolSvc;
    ServiceHandle<IBeamCondSvc> m_beamSpotSvc;
  };
    
}//end namespace 

#endif

