/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAROOTSERIALIZESVC_H
#define ATHENAROOTSERIALIZESVC_H

/** @file AthenaRootSerializeSvc.h
 *  @brief This file contains the class definition for the AthenaRootSerializeSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IAthenaSerializeSvc.h"

/** @class AthenaRootSerializeSvc
 *  @brief This class provides the AthenaSerializeSvc using ROOT
 **/
class AthenaRootSerializeSvc : public ::AthService, virtual public IAthenaSerializeSvc {
   // Allow the factory class access to the constructor
   friend class SvcFactory<AthenaRootSerializeSvc>;

public: 
   /// Standard Service Constructor
   AthenaRootSerializeSvc(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~AthenaRootSerializeSvc();

   /// Gaudi Service Interface method implementations:
   StatusCode initialize() override;
   StatusCode finalize() override;
   StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

   void* serialize(const void* object, const std::string& name, size_t& nbytes) const override;
   void* serialize(const void* object, const Guid& id, size_t& nbytes) const override;
   void* serialize(const void* object, const RootType& cltype, size_t& nbytes) const override;

   void* deserialize(void* buffer, size_t& nbytes, const std::string& name) const override;
   void* deserialize(void* buffer, size_t& nbytes, const Guid& id) const override;
   void* deserialize(void* buffer, size_t& nbytes, const RootType& cltype) const override;
};

#endif
