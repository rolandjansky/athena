/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOMAPPINGTOOLS_CPMMAPPINGTOOL_H
#define TRIGT1CALOMAPPINGTOOLS_CPMMAPPINGTOOL_H

#include <map>
#include <utility>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigT1CaloToolInterfaces/IL1CaloMappingTool.h"

class IInterface;
class StatusCode;

namespace LVL1 {


/** CPM crate/module/channel to eta/phi/layer mappings
 *
 *  Layer indicates core/overlap
 *
 *  Source: "ATLAS Level-1 Calorimeter Trigger: Cluster Processor
 *           Module, Project Specification" version 2.03
 *
 *  @author Peter Faulkner
 */

class CpmMappingTool : public extends<AthAlgTool, IL1CaloMappingTool>
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

   static const int s_crates   = 4;
   static const int s_modules  = 14;
   static const int s_channels = 80;

   static const int s_etaBinsPerRow = 4;

   static const double s_phiGran;
   static const double s_etaGran;

};

} // end namespace

#endif
