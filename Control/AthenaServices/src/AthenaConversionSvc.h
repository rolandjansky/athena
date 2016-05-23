/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_ATHENACONVERSIONSVC_H
#define ATHENASERVICES_ATHENACONVERSIONSVC_H
/** @file AthenaConversionSvc.h
    @brief Base class for all ATLAS conversion services. 

  * $Id: AthenaConversionSvc.h,v 1.2 2007-06-16 00:55:22 calaf Exp $
*/

// Include files
#ifndef _CPP_VECTOR
 #include <vector>
#endif
#ifndef _CPP_ALGORITHM
 #include <algorithm>
#endif
#ifndef _CPP_FUNCTIONAL
 #include <functional>
#endif
#ifndef GAUDIKERNEL_KERNEL_H
 #include "GaudiKernel/Kernel.h"
#endif
#ifndef GAUDIKERNEL_CONVERSIONSVC_H
 #include "GaudiKernel/ConversionSvc.h"
#endif

template <class TYPE,class ARG> class SvcFactory2;

/** @class AthenaConversionSvc 
    @brief Base class for all ATLAS conversion services. 

    @details It manages a set of Converters that
    are dedicated to read and write specific data objects.
    Currently it is just a dummy forwarding to Gaudi ConversionSvc base class.
    FIXME shouldn't it be a simple typedef?
*/

class AthenaConversionSvc  : public ConversionSvc
{
  friend class SvcFactory2<AthenaConversionSvc,unsigned char>;

public:

  /// Initialize the service.
  virtual StatusCode initialize();

  /// stop the service.
  virtual StatusCode finalize();

  /// Standard Constructor
  AthenaConversionSvc(const std::string& name, 
		      ISvcLocator* svc, 
		      unsigned char type);

protected:
  /// Standard Destructor
  virtual ~AthenaConversionSvc();

};
#endif // ATHENASERVICES_ATHENACONVERSIONSVC_H



