/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAROOTSHAREDWRITERSVC_H
#define ATHENAROOTSHAREDWRITERSVC_H

/** @file AthenaRootSharedWriterSvc.h
 *  @brief This file contains the class definition for the AthenaRootSharedWriterSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IAthenaSharedWriterSvc.h"

#include "GaudiKernel/IConversionSvc.h"

#include "THashTable.h"

class TServerSocket;
class TMonitor;

/** @class AthenaRootSharedWriterSvc
 *  @brief This class provides an example for writing event data objects to Pool.
 **/
class AthenaRootSharedWriterSvc : public AthService, virtual public IAthenaSharedWriterSvc {
   // Allow the factory class access to the constructor
   friend class SvcFactory<AthenaRootSharedWriterSvc>;

public: // Constructor and Destructor
   /// Standard Service Constructor
   AthenaRootSharedWriterSvc(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~AthenaRootSharedWriterSvc() = default;

public:
/// Gaudi Service Interface method implementations:
   virtual StatusCode initialize() override;
   virtual StatusCode start() override;
   virtual StatusCode stop() override;
   virtual StatusCode finalize() override;
   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

private:
   ServiceHandle<IConversionSvc> m_cnvSvc{this,"AthenaPoolCnvSvc","AthenaPoolCnvSvc"};

   TServerSocket* m_rootServerSocket;
   TMonitor* m_rootMonitor;
   THashTable m_rootMergers;
   int m_rootClientIndex;
   int m_rootClientCount;
};

#endif
