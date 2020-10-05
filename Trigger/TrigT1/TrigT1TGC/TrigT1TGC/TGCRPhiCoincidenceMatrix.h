/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRPhiCoincidenceMatrix_hh
#define TGCRPhiCoincidenceMatrix_hh

#include "TrigT1TGC/TGCSSCControllerOut.h"
#include "TrigT1TGC/TGCRPhiCoincidenceMap.h"
#include "TrigT1TGC/TGCArguments.h"

namespace LVL1TGCTrigger {

class TGCSectorLogic;
class TGCRPhiCoincidenceOut;

const int NumberOfPtLevel = 6;

class TGCRPhiCoincidenceMatrix {
public:
  void setSSCId(int SSCIdIn){ m_SSCId=SSCIdIn;};
  void setRPhiMap(const TGCRPhiCoincidenceMap* map);
  void inputR(int rIn, int dRIn, int ptRIn);
  void inputPhi(int phiIn, int dPhiIn, int ptPhiIn);
  void clear();
  TGCRPhiCoincidenceOut* doCoincidence();

  TGCRPhiCoincidenceMatrix(const TGCArguments*, const  TGCSectorLogic*  sL=0);
  virtual ~TGCRPhiCoincidenceMatrix();

  TGCRPhiCoincidenceMatrix(const TGCRPhiCoincidenceMatrix& right);
  TGCRPhiCoincidenceMatrix& operator=(const TGCRPhiCoincidenceMatrix& right);
  
  const TGCArguments* tgcArgs() const { return m_tgcArgs; }
  
private:

  enum {MaxNPhiHit = 2}; 
    
  const  TGCSectorLogic*  m_sectorLogic; 

  TGCRPhiCoincidenceOut* m_matrixOut;
  const TGCRPhiCoincidenceMap* m_map;

  int m_nPhiHit;
  int m_SSCId;
  int m_r;
  int m_dR;
  int m_ptR;
  int m_phi[MaxNPhiHit];
  int m_dPhi[MaxNPhiHit];
  int m_ptPhi[MaxNPhiHit];

  const TGCArguments* m_tgcArgs;
};

} //end of namespace bracket

#endif // TGCRPhiCoincidenceMatrix_hh

