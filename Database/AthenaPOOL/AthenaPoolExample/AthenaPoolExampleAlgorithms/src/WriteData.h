/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLEALGORITHMS_WRITEDATA_H
#define ATHENAPOOLEXAMPLEALGORITHMS_WRITEDATA_H

/** @file AthenaPoolExampleAlgorithms/src/WriteData.h
 *  @brief This file contains the class definition for the WriteData class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: WriteData.h,v 1.1 2008-12-10 21:28:11 gemmeren Exp $
 **/

#include "AthenaBaseComps/AthAlgorithm.h"

namespace AthPoolEx {

/** @class AthPoolEx::WriteData
 *  @brief This class provides an example for writing event data objects to Pool.
 **/
class WriteData : public AthAlgorithm {
public: // Constructor and Destructor
   /// Standard Service Constructor
   WriteData(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~WriteData();

public:
/// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();
};

} // end AthPoolEx namespace

#endif
