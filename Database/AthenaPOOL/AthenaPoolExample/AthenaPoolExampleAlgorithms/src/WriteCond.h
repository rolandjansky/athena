/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLEALGORITHMS_WRITECOND_H
#define ATHENAPOOLEXAMPLEALGORITHMS_WRITECOND_H

/** @file WriteCond.h
 *  @brief This file contains the class definition for the WriteCond class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: WriteCond.h,v 1.1 2008-12-10 21:28:11 gemmeren Exp $
 **/

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaPoolExampleData/ExampleHitContainer.h"
#include "StoreGate/ReadHandleKey.h"

namespace AthPoolEx {

/** @class AthPoolEx::WriteCond
 *  @brief This class provides an example for writing conditions data objects to Pool.
 **/
class WriteCond : public AthReentrantAlgorithm {
public: // Constructor and Destructor
   /// Standard Service Constructor
   WriteCond(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~WriteCond();

public:
/// Gaudi Service Interface method implementations:
   virtual StatusCode initialize() override;
   virtual StatusCode execute (const EventContext& ctx) const override;
   virtual StatusCode stop() override;

private:
   StringProperty m_conditionName;
   DoubleProperty m_weight;
   DoubleProperty m_offset;

   SG::ReadHandleKey<ExampleHitContainer> m_exampleHitKey { this, "ExampleHitKey", "MyHits" };
};

} // end AthPoolEx namespace

#endif
