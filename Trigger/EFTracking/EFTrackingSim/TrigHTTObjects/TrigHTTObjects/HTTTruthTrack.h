/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHTTOBJECTS_HTTTRUTHTRACK_H
#define TRIGHTTOBJECTS_HTTTRUTHTRACK_H

#include <TObject.h>
#include <TMath.h>

#include "TrigHTTObjects/HTTTrackPars.h"

class HTTTruthTrack : public TObject {
public:
  HTTTruthTrack();

  void setD0(double v) { m_d0 = v; }
  void setZ0(double v) { m_z0 = v; }

  void setVtxX(double v) { m_vtx_x = v; }
  void setVtxY(double v) { m_vtx_y = v; }
  void setVtxZ(double v) { m_vtx_z = v; }
  void setPX(double v) { m_px = v; }
  void setPY(double v) { m_py = v; }
  void setPZ(double v) { m_pz = v; }

  void setQ(int v) { m_q = v; }

  void setPDGCode(int v) { m_pdgcode = v; }
  void setBarcode(int v) { m_barcode = v; }
  void setEventIndex(int v) { m_evtindex = v; }
  void setBarcodeFracOffline(double v) { m_barcode_frac_offline = v; }


  double getPt() const { return TMath::Sqrt(m_px * m_px + m_py * m_py); }
  double getEta() const { return TMath::ASinH(m_pz / TMath::Sqrt(m_px * m_px + m_py * m_py)); }
  double getPhi() const { return TMath::ATan2(m_py, m_px); }
  double getD0() const { return m_d0; }
  double getZ0() const { return m_z0; }

  double getVtxX() const { return m_vtx_x; }
  double getVtxY() const { return m_vtx_y; }
  double getVtxZ() const { return m_vtx_z; }
  double getIP() const { return TMath::Sqrt(m_vtx_x * m_vtx_x + m_vtx_y * m_vtx_y); }
  double getPX() const { return m_px; }
  double getPY() const { return m_py; }
  double getPZ() const { return m_pz; }

  int getQ() const { return m_q; }
  double getHalfInvPt() const { return static_cast<double>(m_q) / (2 * getPt()); }
  double getQOverPt() const { return static_cast<double>(m_q) / getPt(); }

  int getPDGCode() const { return m_pdgcode; }
  int getBarcode() const { return m_barcode; }
  int getEventIndex() const { return m_evtindex; }
  double getBarcodeFracOffline() const { return m_barcode_frac_offline; }


  HTTTrackPars getPars() const {
    HTTTrackPars pars;
    pars.qOverPt = getQ() / getPt();
    pars.eta = getEta();
    pars.phi = getPhi();
    pars.d0 = getD0();
    pars.z0 = getZ0();

    return pars;
  }


private:
  double m_d0;
  double m_z0;

  double m_vtx_x;
  double m_vtx_y;
  double m_vtx_z;
  double m_px; // MeV
  double m_py;
  double m_pz;

  int m_q;

  int m_pdgcode;
  int m_barcode;
  int m_evtindex;
  double m_barcode_frac_offline; // largest "matching fraction" with any "good"
                                // xAOD::TruthParticle, corresponding to the
                                // particle with m_barcode


  ClassDef(HTTTruthTrack, 3)
};

#endif // TRIGHTTOBJECTS_HTTTRUTHTRACK_H
