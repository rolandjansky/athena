/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOMAPPINGTOOLS_PPMCOOLORBUILTINMAPPINGTOOL_H
#define TRIGT1CALOMAPPINGTOOLS_PPMCOOLORBUILTINMAPPINGTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigT1CaloToolInterfaces/IL1CaloMappingTool.h"
#include "CxxUtils/CachedValue.h"

class IInterface;
class StatusCode;

namespace LVL1 {

/** PPM crate/module/channel to eta/phi/layer mappings
 *
 *  Uses COOL channel mapping tools when they work, built-in mapping tools otherwise.
 *  
 *
 *  @author Peter Faulkner
 */

class PpmCoolOrBuiltinMappingTool : public extends<AthAlgTool, IL1CaloMappingTool>
{
 public:

   using base_class::base_class;

   virtual StatusCode initialize() override;
   virtual StatusCode finalize() override;

   /// Return eta, phi and layer mapping for given crate/module/channel
   virtual bool mapping(int crate, int module, int channel,
                        double& eta, double& phi, int& layer) const override;
   /// Return crate, module and channel mapping for given eta/phi/layer
   virtual bool mapping(double eta, double phi, int layer,
                        int& crate, int& module, int& channel) const override;

 private:

   /// Check if COOL mappings are working
   bool coolWorks() const;

   /// COOL mapping tool
   ToolHandle<IL1CaloMappingTool> m_coolTool
   { this, "CoolMappingTool", "LVL1::PpmCoolMappingTool/PpmCoolMappingTool" };
   /// Built-in mapping tool
   ToolHandle<IL1CaloMappingTool> m_builtinTool
   { this, "BuiltinMappingTool", "LVL1::PpmMappingTool/PpmMappingTool" };

   /// COOL mapping check done flag
   mutable CxxUtils::CachedValue<bool> m_coolCheck;
};

} // end namespace

#endif
