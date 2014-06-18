/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SGTOOLS_CALLBACKID_H
#define SGTOOLS_CALLBACKID_H

/*****************************************************************************
 *
 *  CallBackID.h
 *  IOVSvc
 *
 *  Author: Charles Leggett
 *  $Id: CallBackID.h,v 1.3 2005-11-08 22:01:30 ssnyder Exp $
 *
 *  Provides means to identify a callback function.
 *
 *****************************************************************************/

#ifndef ATHENAKERNEL_IOVSVCDEFS_H
 #include "AthenaKernel/IOVSvcDefs.h"
#endif

class CallBackID {
public:
  CallBackID():m_offset(0),p_obj(0){};

  template<typename T>
  CallBackID(StatusCode (T::*updF)(IOVSVC_CALLBACK_ARGS), const T *obj) {
    set(updF,obj);
  }

  template<typename T>
  void set(StatusCode (T::*updF)(IOVSVC_CALLBACK_ARGS), const T *obj);

  std::string name() const { return m_name; }
  std::string objName() const { return m_objName; }
  int offset() const { return m_offset; }
  const void* ptr() const { return p_obj; }

  bool operator==(const CallBackID& cb) const {
    return ( cb.m_offset == m_offset && cb.p_obj == p_obj );
  }
  bool operator<(const CallBackID& cb) const {
    if ( p_obj != cb.p_obj ) {
      return ( p_obj < cb.p_obj ) ;
    } else {
      return ( m_offset < cb.m_offset );
    }
  }

private:
  long m_offset;
  const void* p_obj;

  std::string m_name;
  std::string m_objName;

};

#ifndef SGTOOLS_CALLBACKID_ICC
 #include "SGTools/CallBackID.icc"
#endif

#endif
