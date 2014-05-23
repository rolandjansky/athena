/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TEST_TOYCONVERSIONSVC_H
# define TEST_TOYCONVERSIONSVC_H

//<<<<<< INCLUDES                                                       >>>>>>
#include "GaudiKernel/ConversionSvc.h"
//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< FORWARD DECLARATIONS                                           >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/** @class ToyConversionSvc 
 * @brief  a toy that implements IConversionSvc
 * @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration
 * $Id: ToyConversionSvc.h,v 1.3 2008-03-18 01:36:59 calaf Exp $
 */


class ToyConversionSvc : public ConversionSvc {
public:
  static long int storageType();
  virtual long int repSvcType() const;

  ToyConversionSvc(const std::string &name, ISvcLocator* svc);
  virtual ~ToyConversionSvc();
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // TEST_TOYCONVERSIONSVC_H
