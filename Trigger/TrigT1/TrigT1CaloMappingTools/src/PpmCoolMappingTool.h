/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOMAPPINGTOOLS_PPMCOOLMAPPINGTOOL_H
#define TRIGT1CALOMAPPINGTOOLS_PPMCOOLMAPPINGTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"

class CaloLVL1_ID;
class CaloTriggerTowerService;
class IInterface;
class StatusCode;
class TTOnlineID;

namespace LVL1 {

/** PPM crate/module/channel to eta/phi/layer mappings
 *
 *  Uses COOL channel mapping tools
 *  
 *
 *  @author Peter Faulkner
 */

class PpmCoolMappingTool : virtual public IL1CaloMappingTool,
                                   public IIncidentListener,
                                   public AthAlgTool {

 public:

   PpmCoolMappingTool(const std::string& type, const std::string& name,
                                               const IInterface* parent);
   virtual ~PpmCoolMappingTool();

   virtual StatusCode initialize();
   virtual StatusCode finalize();
   virtual void handle(const Incident&);

   /// Return eta, phi and layer mapping for given crate/module/channel
   virtual bool mapping(int crate, int module, int channel,
                        double& eta, double& phi, int& layer);
   /// Return crate, module and channel mapping for given eta/phi/layer
   virtual bool mapping(double eta, double phi, int layer,
                        int& crate, int& module, int& channel);

 private:

   // Tools and helpers
   ToolHandle<CaloTriggerTowerService> m_ttSvc;
   const CaloLVL1_ID*                  m_lvl1Helper;
   const TTOnlineID*                   m_l1ttonlineHelper;

   /// Mapping lookup table
   std::vector<unsigned int> m_idTable;
   static const int s_maxTableEntries = 8192;

};

} // end namespace

#endif
