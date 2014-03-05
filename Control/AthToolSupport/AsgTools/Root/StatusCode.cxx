// StatusCode.cxx

#include "AsgTools/StatusCode.h"

//**********************************************************************

StatusCode::StatusCode(int rstat) : m_code(rstat==1) { }
  
//**********************************************************************

bool StatusCode::isSuccess() const {
  return (m_code == SUCCESS);
}

//**********************************************************************

bool StatusCode::isFailure() const {
  return !isSuccess();
}

//**********************************************************************

StatusCode& StatusCode::operator=(int code) {
  m_code = code == 1;
  return *this;
}

//**********************************************************************
