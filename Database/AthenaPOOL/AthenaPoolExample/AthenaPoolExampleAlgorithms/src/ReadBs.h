/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLEALGORITHMS_READBS_H
#define ATHENAPOOLEXAMPLEALGORITHMS_READBS_H

/** @file ReadBs.h
 *  @brief This file contains the class definition for the ReadBs class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ReadBs.h,v 1.2 2009-03-11 20:08:50 gemmeren Exp $
 **/

#include "AthenaBaseComps/AthAlgorithm.h"

class StoreGateSvc;

namespace AthPoolEx {

/** @class AthPoolEx::ReadBs
 *  @brief This class provides an example for reading event data objects from Pool.
 **/
class ReadBs : public AthAlgorithm {
public: // Constructor and Destructor
   /// Standard Service Constructor
   ReadBs(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~ReadBs();

public:
/// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:
   ServiceHandle<StoreGateSvc> p_SGinMeta;
   ServiceHandle<StoreGateSvc> p_SGmeta;
};

} // end namespace AthPoolEx

#endif
