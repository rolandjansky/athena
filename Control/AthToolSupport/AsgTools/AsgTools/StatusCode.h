// StatusCode.h

#ifndef asg_StatusCode_H
#define asg_StatusCode_H

// David Adams
// January 2014
//
// Partial reimplementation of Gaudi StatusCode to allow
// ToolHandle::retrieve to have the same syntax in Root as Athena.

#include "AsgTools/AsgToolsConf.h"
#ifdef ASGTOOL_ATHENA
#include "GaudiKernel/StatusCode.h"
#else
#include "AsgTools/Check.h"

class StatusCode {

public:  // enums

  enum {
    FAILURE = 0,
    SUCCESS = 1
  };

public:  // methods

  // Ctor from status code. (0 = failure)
  StatusCode(int rstat =1);

  bool isSuccess() const;

  bool isFailure() const;

  // Success iff code=1, SUCCESS or true
  StatusCode& operator=(int code);

private:  // data

  int m_code;

};

#endif
#endif
