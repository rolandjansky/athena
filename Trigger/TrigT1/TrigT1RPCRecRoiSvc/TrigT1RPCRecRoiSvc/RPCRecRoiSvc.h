/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1_RPCRECROISVC_H
#define TRIGT1_RPCRECROISVC_H


#include "TrigT1Interfaces/RecMuonRoiSvc.h"

class IRPCgeometrySvc;
class IRPCcablingSvc;
class IMDTcablingSvc;


namespace LVL1RPC {

class RPCRecRoiSvc : public LVL1::RecMuonRoiSvc
{
public:
  RPCRecRoiSvc (const std::string& name, ISvcLocator* svc)
    : LVL1::RecMuonRoiSvc( name, svc ), 
      m_phi(0),
      m_eta(0)
    {};

  ~RPCRecRoiSvc (void) {};

  StatusCode initialize (void); 

  void reconstruct (const unsigned int & roIWord) const;
  double phi (void) const {return m_phi;};
  double eta (void) const {return m_eta;};
  
  bool writeRoiRobMap(const std::string& filename);

private:
  bool etaDimLow (unsigned short int side,unsigned short int sector,
                  unsigned short int roi,float& etaMin, float& etaMax) const;
  bool etaDimHigh(unsigned short int side,unsigned short int sector,
                  unsigned short int roi,float& etaMin, float& etaMax) const;
private:
  mutable double m_phi, m_eta;

  const IRPCgeometrySvc*  m_rPCgeometrySvc;
  const IRPCcablingSvc*   m_rPCcablingSvc;
  const IMDTcablingSvc*   m_mDTcablingSvc;
};

} // end of namespace

#endif 
