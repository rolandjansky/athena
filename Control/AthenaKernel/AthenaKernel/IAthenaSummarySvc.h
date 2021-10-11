/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IATHENASUMMARYSVC_H
#define ATHENAKERNEL_IATHENASUMMARYSVC_H

#include "GaudiKernel/IService.h"
#include "GaudiKernel/StatusCode.h"

#include <string>

/** @class IAthenaSummarySvc
 * @brief Abstract produces summary of Athena stuff 
 * @author Charles Leggett
 */  

class IAthenaSummarySvc : virtual public IService {

public:
  DeclareInterfaceID(IAthenaSummarySvc, 1, 0);

  virtual ~IAthenaSummarySvc();

  virtual StatusCode createSummary() = 0;
  virtual void setStatus(int) = 0;
  virtual void addListener(const std::string&) = 0;
  virtual void addSummary(const std::string&, const std::string&) = 0;
  virtual const std::string& getOutputFile() const = 0;
};

#endif
