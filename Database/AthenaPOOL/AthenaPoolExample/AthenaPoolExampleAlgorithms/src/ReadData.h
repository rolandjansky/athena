/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLEALGORITHMS_READDATA_H
#define ATHENAPOOLEXAMPLEALGORITHMS_READDATA_H

/** @file AthenaPoolExampleAlgorithms/src/ReadData.h
 *  @brief This file contains the class definition for the ReadData class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ReadData.h,v 1.2 2009-03-11 20:08:50 gemmeren Exp $
 **/

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class StoreGateSvc;

namespace AthPoolEx {

/** @class AthPoolEx::ReadData
 *  @brief This class provides an example for reading event data objects from Pool.
 **/
class ReadData : public AthAlgorithm {
public: // Constructor and Destructor
   /// Standard Service Constructor
   ReadData(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~ReadData();

public:
/// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:
   ServiceHandle<StoreGateSvc> p_SGinMeta;
   ServiceHandle<StoreGateSvc> p_SGtagMeta;
   ServiceHandle<StoreGateSvc> p_SGmeta;
};

} // end namespace AthPoolEx

#endif
