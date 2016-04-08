/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOMAPPINGTOOLS_PPMCOOLORBUILTINMAPPINGTOOL_H
#define TRIGT1CALOMAPPINGTOOLS_PPMCOOLORBUILTINMAPPINGTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"

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

class PpmCoolOrBuiltinMappingTool : virtual public IL1CaloMappingTool,
                                            public AthAlgTool {

 public:

   PpmCoolOrBuiltinMappingTool(const std::string& type, const std::string& name,
                                                        const IInterface* parent);
   virtual ~PpmCoolOrBuiltinMappingTool();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Return eta, phi and layer mapping for given crate/module/channel
   virtual bool mapping(int crate, int module, int channel,
                        double& eta, double& phi, int& layer);
   /// Return crate, module and channel mapping for given eta/phi/layer
   virtual bool mapping(double eta, double phi, int layer,
                        int& crate, int& module, int& channel);

 private:

   /// Check if COOL mappings are working
   bool coolWorks();

   /// COOL mapping tool
   ToolHandle<IL1CaloMappingTool> m_coolTool;
   /// Built-in mapping tool
   ToolHandle<IL1CaloMappingTool> m_builtinTool;

   /// COOL mapping check done flag
   bool m_coolCheckDone;
   /// COOL mapping check result
   bool m_coolCheckResult;

};

} // end namespace

#endif
