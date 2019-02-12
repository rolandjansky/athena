/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IJOBIDSVC_H
#define ATHENAKERNEL_IJOBIDSVC_H

#include "GaudiKernel/IService.h"
#include "GaudiKernel/StatusCode.h"

#include <string>
#include <uuid/uuid.h>

/**
 * @class IJobIDSvc
 * @brief  interface to JobIDSvc
 * @author Yushu Yao <yyao@lbl.gov> - ATLAS Collaboration
 */

typedef uuid_t JobID_t;
typedef unsigned char* PJobID_t;

class IJobIDSvc : virtual public IService {
public:
  DeclareInterfaceID(IJobIDSvc, 1, 0);
  virtual ~IJobIDSvc();

  /// Get current JobID
  virtual PJobID_t getJobID() const = 0;
  virtual std::string toString() const = 0;
};

#endif // ATHENAKERNEL_IJOBIDSVC_H
