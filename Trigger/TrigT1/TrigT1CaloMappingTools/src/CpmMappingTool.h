/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOMAPPINGTOOLS_CPMMAPPINGTOOL_H
#define TRIGT1CALOMAPPINGTOOLS_CPMMAPPINGTOOL_H

#include <map>
#include <utility>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"

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

class CpmMappingTool : virtual public IL1CaloMappingTool,
                               public AthAlgTool {

 public:

   CpmMappingTool(const std::string& type, const std::string& name,
                                           const IInterface* parent);
   virtual ~CpmMappingTool();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Return eta, phi and layer mapping for given crate/module/channel
   virtual bool mapping(int crate, int module, int channel,
                        double& eta, double& phi, int& layer);
   /// Return crate, module and channel mapping for given eta/phi/layer
   virtual bool mapping(double eta, double phi, int layer,
                        int& crate, int& module, int& channel);

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
