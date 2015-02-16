/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLEALGORITHMS_REWRITEDATA_H
#define ATHENAPOOLEXAMPLEALGORITHMS_REWRITEDATA_H

/** @file ReWriteData.h
 *  @brief This file contains the class definition for the ReWriteData class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ReWriteData.h,v 1.1 2008-12-10 21:28:11 gemmeren Exp $
 **/

#include "AthenaBaseComps/AthAlgorithm.h"

namespace AthPoolEx {

/** @class AthPoolEx::ReWriteData
 *  @brief This class provides an example for reading and writing data objects from/to Pool.
 **/
class ReWriteData : public AthAlgorithm {
public: // Constructor and Destructor
   /// Standard Service Constructor
   ReWriteData(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~ReWriteData();

public:
/// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();
};

} // end AthPoolEx namespace

#endif
