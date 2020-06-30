/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_MuonHitTestToolBase_h
#define G4AT_MuonHitTestToolBase_h

#include "SimTestToolBase.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/Identifier.h"
#include "HitManagement/HitIdHelper.h"

namespace MuonGM {
  class MuonDetectorManager;
}

class MuonHitTestToolBase : public SimTestToolBase {

public:
  MuonHitTestToolBase(const std::string& type, const std::string& name, const IInterface* parent);


  StatusCode initialize();

protected:
  StatusCode executeCheckEventInfo();
  StatusCode executeFillHistos(const Amg::Vector3D &);
  StatusCode executeFillHistosSectors_Wedge1(const Amg::Vector3D &, std::string);
  StatusCode executeFillHistosSectors_Wedge2(const Amg::Vector3D &, std::string);
  StatusCode executeFillHistos_sTGc(const Amg::Vector3D &, std::string);

protected:
  std::string m_detname;
  const MuonGM::MuonDetectorManager* m_pMuonMgr;

  /// MDT barrel eta cut, applicable to the MDT 2D cross section plot
  double m_BarrelEtaCut;

  // general
  TH1 *m_muonevnt, *m_muonrun;
  TH1 *m_muoneta, *m_muontheta, *m_muonphi;
  TH1 *m_muonzResid, *m_muonphiResid;
  TH2 *m_muondetBarrel, *m_muonlongView;
  // specialised
  TH1 *m_eta, *m_theta, *m_phi;
  TH1 *m_zResid, *m_phiResid;
  TH2 *m_detBarrel, *m_longView;


  // helper variables
  Amg::Vector3D m_direction;
};

#endif // MuonHitTestToolBase_h
