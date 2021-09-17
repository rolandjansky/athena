/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IRPC_STATUSCONDITIONSSVC_H
#define IRPC_STATUSCONDITIONSSVC_H

#include "GaudiKernel/IInterface.h"
#include <string>
#include <vector>
#include <algorithm>
#include "AthenaKernel/IOVSvcDefs.h"

//#include "MuonCondSvc/MuonHierarchy.h"
#include "MuonCondInterface/IRPCConditionsSvc.h"

class Identifier;
class IdentifierHash;
class StatusCode;

class ATLAS_NOT_THREAD_SAFE IRPC_STATUSConditionsSvc : virtual public IRPCConditionsSvc {
  

public:


  virtual ~IRPC_STATUSConditionsSvc(){}
  // / Retrieve interface ID
  //static const InterfaceID& interfaceID() { return IID_RPC_STATUSConditionsSvc; }

  // map initialization method
  virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS)=0;


/*   virtual const std::vector<Identifier>&  EffPanelId() const=0 ; */
/*   virtual const std::vector<Identifier>&  EffStripId() const=0 ; */
  
/*   virtual const std::map<Identifier,     double>& RPC_EfficiencyMap()=0              ;  */
/*   virtual const std::map<Identifier,     double>& RPC_EfficiencyGapMap()=0           ;  */
/*   virtual const std::map<Identifier,     double>& RPC_MeanClusterSizeMap()=0         ;  */
/*   virtual const std::map<Identifier,     double>& RPC_FracClusterSize1Map()=0        ;  */
/*   virtual const std::map<Identifier,     double>& RPC_FracClusterSize2Map()=0        ;  */
/*   virtual const std::map<Identifier,     std::string>& RPC_DeadStripListMap()=0      ;  */
/*   virtual const std::map<Identifier,      float>& RPC_FracDeadStripMap()=0           ;  */
/*   virtual const std::map<Identifier,        int>& RPC_ProjectedTracksMap()=0         ;   */
/*   virtual const std::map<Identifier,        int>& RPC_DeadStripList()=0              ;   */

  

  //virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID & interfaceID();
 
  //protected:


 private:
  
};

inline const InterfaceID & IRPC_STATUSConditionsSvc::interfaceID(){
  static const InterfaceID IID_RPC_STATUSConditionsSvc("RPC_STATUSConditionsSvc",1,0);
  return IID_RPC_STATUSConditionsSvc;
}
#endif  
