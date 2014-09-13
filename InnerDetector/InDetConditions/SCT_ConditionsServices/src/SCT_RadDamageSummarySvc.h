/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RADDAMAGESUMMARYSVC_H
#define SCT_RADDAMAGESUMMARYSVC_H

#include "SCT_ConditionsServices/ISCT_RadDamageSummarySvc.h"

#include <string>

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "CLHEP/Vector/ThreeVector.h"

class Identifier;
class IdentifierHash;
class SCT_ID;
// class ISCT_RamoPotentialSvc;
class ISCT_ChargeTrappingSvc;
class StoreGateSvc;


// ----------------------------------------------------------------------------- //
// -- Summary service for radiation damage related services                         
//    Provides access to SCT_ChargeTrappingSvc                                        
//                                                                               
//    primary author for Pixels: Carolina Deluca <carolina.deluca.silberberg@cern.ch> 
//    author for SCT: Peter Vankov <peter.vankov@cern.ch>,
//                    Marco Filipuzzi <marco.filipuzzi@cern.ch>                 
//    date: 2012-05-18                                                              
// ----------------------------------------------------------------------------- //

class SCT_RadDamageSummarySvc: public AthService, virtual public ISCT_RadDamageSummarySvc{

 public:
  static const InterfaceID& interfaceID();
  
  SCT_RadDamageSummarySvc(const std::string& name, ISvcLocator* sl);
  virtual ~SCT_RadDamageSummarySvc();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface); 
 
  // virtual double RamoPotential(const IdentifierHash & elementHash, const Hep3Vector & pos);
  virtual double ChargeTrappingProbability(const IdentifierHash & elementHash, const double & zpos);
  virtual double TrappingConstant(const IdentifierHash & elementHash, const double & zpos);
  virtual double ElectricField(const IdentifierHash & elementHash, const double & zpos);
  virtual double TrappingTime(const IdentifierHash & elementHash, const double & zpos);
  virtual double TimeToElectrode(const IdentifierHash & elementHash, const double & zpos);
  virtual double TrappingPositionZ(const IdentifierHash & elementHash, const double & zpos);
  virtual double HoleDriftMobility(const IdentifierHash & elementHash, const double & zpos); 
  bool doCTrap(const IdentifierHash & elementHash,  const double & zpos); 
  virtual void HoleTransport(double & x0, double & y0, double & xfin, double & yfin, double & Q_m2, double & Q_m1, double & Q_00, double & Q_p1, double & Q_p2 )const;
  virtual void InitPotentialValue();

 private:
  ServiceHandle< StoreGateSvc > m_detStore;
  
  const SCT_ID* m_SCT_ID;

  // ServiceHandle< IRamoPotentialSvc > m_ramoPotentialSvc;
  ServiceHandle< ISCT_ChargeTrappingSvc > m_chargeTrappingSvc;
  
  bool m_useTrapping;
  mutable bool m_doCTrap;
  // bool m_useRamo;

};

inline const InterfaceID& SCT_RadDamageSummarySvc::interfaceID(){

  static const InterfaceID ISCT_RadDamageSummarySvc("SCT_RadDamageSummarySvc", 1, 0); 
  return ISCT_RadDamageSummarySvc::interfaceID();
}

#endif

