/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRPhiCoincidenceMatrix_hh
#define TGCRPhiCoincidenceMatrix_hh

#include "TrigT1TGC/TGCSSCControllerOut.h"
#include "TrigT1TGC/TGCArguments.h"

namespace LVL1TGC {
class BigWheelCoincidenceLUT;
}

namespace LVL1TGCTrigger {

class TGCSectorLogic;
class TGCRPhiCoincidenceOut;

class TGCRPhiCoincidenceMatrix {
 public:
  void setSSCId(int SSCIdIn){ m_SSCId=SSCIdIn;};
  void setSideId(int sideIdIn){ m_sideId=sideIdIn;};
  void setCoincidenceLUT(std::shared_ptr<const LVL1TGC::BigWheelCoincidenceLUT> lut);
  void inputR(int rIn, int dRIn, int ptRIn);
  void inputPhi(int phiIn, int dPhiIn, int ptPhiIn);
  void clear();
  TGCRPhiCoincidenceOut* doCoincidence();

  TGCRPhiCoincidenceMatrix(const TGCArguments*, const  TGCSectorLogic*  sL=0);
  virtual ~TGCRPhiCoincidenceMatrix();

  TGCRPhiCoincidenceMatrix(const TGCRPhiCoincidenceMatrix& right) = default;
  TGCRPhiCoincidenceMatrix& operator=(const TGCRPhiCoincidenceMatrix& right);
  
  const TGCArguments* tgcArgs() const { return m_tgcArgs; }
  
 private:
  enum {MaxNPhiHit = 2}; 
    
  const  TGCSectorLogic*  m_sectorLogic; 

  TGCRPhiCoincidenceOut* m_matrixOut;
  std::shared_ptr<const LVL1TGC::BigWheelCoincidenceLUT> m_lut;

  int m_nPhiHit;
  int m_SSCId;
  int m_r;
  int m_dR;
  int m_ptR;
  int m_phi[MaxNPhiHit];
  int m_dPhi[MaxNPhiHit];
  int m_ptPhi[MaxNPhiHit];
  int m_sideId;

  const TGCArguments* m_tgcArgs;
};

}   // LVL1TGCTrigger namespace

#endif // TGCRPhiCoincidenceMatrix_hh

