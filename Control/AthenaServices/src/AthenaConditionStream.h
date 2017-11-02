/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_ATHENACONDITIONSTREAM_H
#define ATHENASERVICES_ATHENACONDITIONSTREAM_H

// framework includes:
#include "AthenaOutputStream.h"

/** @class AthenaConditionStream
 * @brief this class is OBSOLETE. Use AthenaOutputStream with
 * writeOnExecute=false and writeOnFinalize=true instead.
 *    out stream specialized for condition data. Write data at finalize
 *        only
 * 
 * @author srinir@bnl.gov
 * $Id: AthenaConditionStream.h,v 1.2 2008-09-24 18:36:04 binet Exp $
 */

class AthenaConditionStream : public AthenaOutputStream
{

public:
  /// Standard algorithm Constructor
  AthenaConditionStream(const std::string& name, ISvcLocator* pSvcLocator); 
  /// Standard Destructor
  virtual ~AthenaConditionStream();

  // Initialize OutputStream
  //virtual StatusCode initialize();
  /// Terminate OutputStream
  virtual StatusCode finalize();
  /// Working entry point
  virtual StatusCode execute();
};

#endif // ATHENASERVICES_ATHENACONDITIONSTREAM_H
