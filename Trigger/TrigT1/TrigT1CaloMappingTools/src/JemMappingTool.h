/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOMAPPINGTOOLS_JEMMAPPINGTOOL_H
#define TRIGT1CALOMAPPINGTOOLS_JEMMAPPINGTOOL_H

#include <map>
#include <utility>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigT1CaloToolInterfaces/IL1CaloMappingTool.h"

class IInterface;
class StatusCode;

namespace LVL1 {


/** JEM crate/module/channel to eta/phi/layer mappings
 *
 *  Layer indicates core/overlap
 *
 *  Source: "ATLAS Level-1 Calorimeter Trigger: Jet/Energy Processor
 *           Module, Project Specification" version 1.0
 *  The remappings at extreme eta are from a private communication
 *  from Steve Hillier.
 *
 *  @author Peter Faulkner
 */

class JemMappingTool : public extends<AthAlgTool, IL1CaloMappingTool>
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

   static const int s_crates   = 2;
   static const int s_modules  = 16;
   static const int s_channels = 44;

   static const int s_modulesPerQuadrant = 8;
   static const int s_extremeNegModule   = 0;
   static const int s_extremePosModule   = 7;
   static const int s_etaBinsPerRow      = 4;

   static const double s_phiGran;
   static const double s_etaGran;

   //  Extreme eta data vectors
   std::vector<double> m_etasNegative;
   std::vector<double> m_granNegative;
   std::vector<double> m_etasPositive;
   std::vector<double> m_granPositive;

   //  Extreme eta remapping vectors
   std::vector<int>    m_negChans;
   std::vector<int>    m_posChans;

};

} // end namespace

#endif
