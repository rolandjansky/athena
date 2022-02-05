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
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "PersistentDataModel/DataHeader.h"
#include "AthenaPoolExampleData/ExampleTrackContainer.h"
#include "AthenaPoolExampleData/ExampleHitContainer.h"
#include "StoreGate/ReadHandleKey.h"

class DataHeader;
class StoreGateSvc;

namespace AthPoolEx {

/** @class AthPoolEx::ReadData
 *  @brief This class provides an example for reading event data objects from Pool.
 **/
class ReadData : public AthReentrantAlgorithm {
public: // Constructor and Destructor
   /// Standard Service Constructor
   ReadData(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~ReadData();

public:
/// Gaudi Service Interface method implementations:
   virtual StatusCode initialize() override;
   virtual StatusCode execute (const EventContext& ctx) const override;
   virtual StatusCode finalize() override;

private:
   ServiceHandle<StoreGateSvc> p_SGinMeta;
   ServiceHandle<StoreGateSvc> p_SGmeta;
   SG::ReadHandleKey<DataHeader> m_dataHeaderKey { this, "DataHeaderKey", "EventSelector" };
   SG::ReadHandleKey<ExampleTrackContainer> m_exampleTrackKey { this, "ExampleTrackKey", "MyTracks" };
   SG::ReadHandleKey<ExampleHitContainer> m_exampleHitKey { this, "ExampleHitKey", "MyHits" };
};

} // end namespace AthPoolEx

#endif
