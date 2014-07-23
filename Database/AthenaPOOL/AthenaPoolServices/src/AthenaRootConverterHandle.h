/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLSERVICES_ATHENAROOTCONVERTERHANDLE_H
#define ATHENAPOOLSERVICES_ATHENAROOTCONVERTERHANDLE_H

/** @file AthenaRootConverterHandle.h
 *  @brief his file contains the class definition for AthenaRootConverter class.
 *  @author Marcin Nowak
 *  $Id: 
 **/

#include "AthenaPoolServices/T_AthenaRootConverter.h"

#include "TROOT.h"
#include "TBuffer.h"
#include "TClass.h"


class AthenaRootConverterHandle {
public:
  AthenaRootConverterHandle(T_AthenaRootConverterBase *converter);
  
  virtual ~AthenaRootConverterHandle();
  
  virtual void ReadBuffer(TBuffer& b, void* obj, Version_t version, UInt_t R__s, UInt_t R__c);

  const std::string&	ClassName() { return m_converter->ClassName(); }
  unsigned		StreamerChecksum() { return m_converter->StreamerChecksum(); }
  
protected:
  TClass			*m_oldClass;
  void				*m_oldObj;
  T_AthenaRootConverterBase 	*m_converter;
};


#endif


