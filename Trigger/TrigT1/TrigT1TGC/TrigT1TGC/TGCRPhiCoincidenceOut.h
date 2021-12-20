/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRPhiCoincidenceOut_hh
#define TGCRPhiCoincidenceOut_hh

namespace LVL1TGCTrigger {


class TGCRPhiCoincidenceOut {
 public:
  TGCRPhiCoincidenceOut();
  ~TGCRPhiCoincidenceOut() = default;

  int  getPhi() const { return m_phi; }
  int  getR() const { return m_r; }
  int  getIdSSC() const { return m_idSSC; }
  int  getDR() const { return m_dR; }
  int  getDPhi() const { return m_dPhi; }
  bool getInnerVeto() const { return m_innerVeto; }

  int getCharge() const { return m_charge; }
  bool getCoincidenceType() const { return m_coincidenceTypeFlag; }
  bool getGoodMFFlag() const { return m_goodMFFlag; }
  bool getInnerCoincidenceFlag() const { return m_innerCoincidenceFlag; }

  int  getpT() const { return m_pT; }
  int  getRoI() const{ return m_RoI; }

  void setIdSSC(int idSSCIn){ m_idSSC = idSSCIn;};
  void setpT(int pTIn){ m_pT=pTIn;};
  void setR(int rIn){ m_r=rIn;};
  void setPhi(int phiIn){ m_phi=phiIn;};
  void setDR(int drIn) { m_dR = drIn; };
  void setDPhi(int dphiIn) { m_dPhi = dphiIn; };
  void setInnerVeto(bool vetoIn) { m_innerVeto = vetoIn; };
  void setRoI(int RoIIn) {m_RoI=RoIIn; };

  void setCharge(int chargeIn){m_charge=chargeIn;};
  void setCoincidenceType(int CoincidenceTypeIn){m_coincidenceTypeFlag=CoincidenceTypeIn;};
  void setGoodMFFlag(bool goodMFFlagIn){m_goodMFFlag=goodMFFlagIn;};
  void setInnerCoincidenceFlag(bool InnerCoincidenceFlagIn){m_innerCoincidenceFlag=InnerCoincidenceFlagIn;};

  void print() const;
  void clear();

  bool isSuperior(const TGCRPhiCoincidenceOut* right) const;

 private:
  int m_idSSC;
  int m_pT;
  int m_phi;
  int m_r;
  int m_dR;
  int m_dPhi;
  bool m_innerVeto;
  int m_RoI;
  int m_charge;
  bool m_coincidenceTypeFlag;
  bool m_goodMFFlag;
  bool m_innerCoincidenceFlag;
};


}  // namespace LVL1TGCTrigger

#endif // TGCRPhiCoincidenceOut_hh
