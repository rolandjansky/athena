/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLEALGORITHMS_WRITETAG_H
#define ATHENAPOOLEXAMPLEALGORITHMS_WRITETAG_H

/** @file WriteTag.h
 *  @brief This file contains the class definition for the WriteTag class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: WriteTag.h,v 1.1 2008-12-10 21:28:11 gemmeren Exp $
 **/

#include "AthenaBaseComps/AthAlgorithm.h"

class AthenaAttributeListSpecification;

namespace AthPoolEx {

/** @class AthPoolEx::WriteTag
 *  @brief This class provides an example for writing event data objects to Pool.
 **/
class WriteTag : public AthAlgorithm {
public: // Constructor and Destructor
   /// Standard Service Constructor
   WriteTag(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~WriteTag();

public:
/// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:
   StringProperty m_key;
   IntegerProperty m_magic;
   /// Specification of the event tag metadata schema
   AthenaAttributeListSpecification* m_attribListSpec;
};

} // end AthPoolEx namespace

#endif
