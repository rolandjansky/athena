// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IJOBIDSVC_H
#define ATHENAKERNEL_IJOBIDSVC_H

#include "GaudiKernel/IService.h"
#include "GaudiKernel/StatusCode.h"

#include <string>
#include "uuid/uuid.h"

namespace Athena {
class PackageInfo;
}

/** @class IJobIDSvc
 * @brief  interface to JobIDSvc
 * @author Yushu Yao <yyao@lbl.gov> - ATLAS Collaboration
 *$Id: IJobIDSvc.h,v 1.3 2009-04-10 11:52:37 binet Exp $	
 */

typedef uuid_t JobID_t;
typedef unsigned char* PJobID_t;

class IJobIDSvc : virtual public IService {
public:

	virtual ~IJobIDSvc();
	
	/// Get current JobID
	virtual PJobID_t getJobID() const = 0;
	virtual std::string toString() const = 0;
	
	/// Gaudi boilerplate
	static const InterfaceID& interfaceID();
};


inline
const InterfaceID& IJobIDSvc::interfaceID() {
	static const InterfaceID IID("IJobIDSvc", 1, 0);
	return IID;
}

#endif // ATHENAKERNEL_IJOBIDSVC_H
