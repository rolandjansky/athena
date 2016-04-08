/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1_RPCRECROISVC_H
#define TRIGT1_RPCRECROISVC_H


#include "TrigT1Interfaces/RecMuonRoiSvc.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"

//class IRPCgeometrySvc;
class Identifier;
class IRPCcablingSvc;
class IMDTcablingSvc;


namespace LVL1RPC {

class RPCRecRoiSvc : public LVL1::RecMuonRoiSvc
{
public:
    RPCRecRoiSvc (const std::string& name, ISvcLocator* svc)
        : LVL1::RecMuonRoiSvc( name, svc ), 
          m_phi(0),
          m_eta(0),
          m_phiMin(0),
          m_phiMax(0),
          m_etaMin(0),
          m_etaMax(0),
          m_side(0),
          m_sector(0),
          m_roi(0),
          m_MuonMgr(0),
          m_rPCcablingSvc(0)
        {};
    
  ~RPCRecRoiSvc (void) {};

  StatusCode initialize (void); 

  void reconstruct (const unsigned int & roIWord) const;
  double phi (void) const {return m_phi;};
  double eta (void) const {return m_eta;};
  double phiMin (void) const {return m_phiMin;};
  double phiMax (void) const {return m_phiMax;};
  double etaMin (void) const {return m_etaMin;};
  double etaMax (void) const {return m_etaMax;};
  double side (void) const {return m_side;};
  double sector (void) const {return m_sector;};
  double roi (void) const {return m_roi;};
            
    
  bool dumpRoiMap(const std::string& filename);
    
  void RoIsize(const unsigned int & roIWord,
               double & etaMin_lowHigh, double & etaMax_lowHigh, double & phiMin_lowHigh, double & phiMax_lowHigh) const; //!< calculate RoI edges TAKING INTO ACCOUNT BENDING !!!
    
    // RoI edges for Low-pt and High-pt confirm planes 
    bool etaDimLow (double& etaMin, double& etaMax) const;
    bool etaDimHigh(double& etaMin, double& etaMax) const;


private:
        
    

    mutable double m_phi, m_eta;
    mutable double m_phiMin, m_phiMax, m_etaMin, m_etaMax;
    mutable unsigned short int m_side, m_sector, m_roi;
    
  const MuonGM::MuonDetectorManager * m_MuonMgr;
  const IRPCcablingSvc*   m_rPCcablingSvc;
    //  const IMDTcablingSvc*   m_mDTcablingSvc;
};

} // end of namespace

#endif 
