/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IATHENASUMMARYSVC_H
#define ATHENAKERNEL_IATHENASUMMARYSVC_H

#ifndef GAUDIKERNEL_ISERVICE_H
 #include "GaudiKernel/IService.h"
#endif
#ifndef KERNEL_STATUSCODES_H
 #include "GaudiKernel/StatusCode.h"
#endif
#ifndef GAUDIKERNEL_CLASSID_H
 #include "GaudiKernel/ClassID.h"
#endif

#include <string>

/** @class IAthenaSummarySvc
 * @brief Abstract produces summary of Athena stuff 
 * @author Charles Leggett
 */  

class IAthenaSummarySvc : virtual public IService {

public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID();

  virtual ~IAthenaSummarySvc();

  virtual StatusCode createSummary() = 0;
  virtual void setStatus(int) = 0;
  virtual void addListener(const std::string&) = 0;
  virtual void addSummary(const std::string&, const std::string&) = 0;
  virtual const std::string& getOutputFile() const = 0;


};

inline
const InterfaceID& 
IAthenaSummarySvc::interfaceID() {
    static const InterfaceID IID("IAthenaSummarySvc", 1, 0);
    return IID;
}

#endif
