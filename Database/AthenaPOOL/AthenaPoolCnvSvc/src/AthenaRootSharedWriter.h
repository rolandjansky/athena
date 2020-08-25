/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAROOTSHAREDWRITER_H
#define ATHENAROOTSHAREDWRITER_H

/** @file AthenaRootSharedWriter.h
 *  @brief This file contains the class definition for the AthenaRootSharedWriter class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaBaseComps/AthFilterAlgorithm.h"

class IConversionSvc;

class TServerSocket;
class TMonitor;

/** @class AthenaRootSharedWriter
 *  @brief This class provides an example for writing event data objects to Pool.
 **/
class AthenaRootSharedWriter : public AthFilterAlgorithm {
public: // Constructor and Destructor
   /// Standard Service Constructor
   AthenaRootSharedWriter(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~AthenaRootSharedWriter();

public:
/// Gaudi Service Interface method implementations:
   virtual StatusCode initialize() override;
   virtual StatusCode execute () override;
   virtual StatusCode finalize() override;

private: // properties
   IntegerProperty m_numberOfClients{this,"NumberOfClients",1};
   IntegerProperty m_autoSend{this,"AutoSend",-1};

private:
   IConversionSvc* m_cnvSvc;

   TServerSocket* m_rootServerSocket;
   TMonitor* m_rootMonitor;
};

#endif
