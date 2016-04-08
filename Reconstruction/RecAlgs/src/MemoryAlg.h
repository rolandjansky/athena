/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECALGS_MEMORYALG_H
#define RECALGS_MEMORYALG_H

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>
#include <malloc.h>

/** @class MemoryAlg
    
Several of the memory algs can be scheduled.
Each one saves memory consumption (VMem, RSS and shared pages from /proc/<pid>/statm)
measured in bytes to the vector.

@author Rolf Seuster <seuster AT cern.ch>
*/

class MemoryAlg : public AthAlgorithm
{
 public:
  
  /** Standard Athena-Algorithm Constructor */
  MemoryAlg(const std::string& name, ISvcLocator* pSvcLocator);
  
  /** Default Destructor */
  ~MemoryAlg() {};
  
  /** standard Athena-Algorithm method */
  StatusCode          initialize();
  /** standard Athena-Algorithm method */
  StatusCode          execute();
  /** standard Athena-Algorithm method */
  StatusCode          finalize()
  { return StatusCode::SUCCESS; };
  
 private:
  
  /** member variables for algorithm properties: */
  std::string  m_memoryObjOutputName;

  unsigned long m_pagesize;
  };

#endif 
