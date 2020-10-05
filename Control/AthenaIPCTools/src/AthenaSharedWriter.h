/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASHAREDWRITER_H
#define ATHENASHAREDWRITER_H

/** @file AthenaSharedWriter.h
 *  @brief This file contains the class definition for the AthenaSharedWriter class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "AthenaKernel/IAthenaSharedWriterSvc.h"

#include "GaudiKernel/IConversionSvc.h"

/** @class AthenaSharedWriter
 *  @brief This class provides an example for writing event data objects to Pool.
 **/
class AthenaSharedWriter : public AthFilterAlgorithm {
public: // Constructor and Destructor
   /// Standard Service Constructor
   AthenaSharedWriter(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~AthenaSharedWriter() = default;

public:
/// Gaudi Service Interface method implementations:
   virtual StatusCode initialize() override;
   virtual StatusCode execute () override;
   virtual StatusCode finalize() override;

private: // properties
   IntegerProperty m_numberOfClients{this,"NumberOfClients",1};

private:
   ServiceHandle<IConversionSvc> m_cnvSvc{this,"AthenaPoolCnvSvc","AthenaPoolCnvSvc"};
   ServiceHandle<IAthenaSharedWriterSvc> m_sharedWriterSvc{this,"AthenaRootSharedWriterSvc","AthenaRootSharedWriterSvc"};
};

#endif
