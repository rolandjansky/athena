/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRPhiCoincidenceMatrix_hh
#define TGCRPhiCoincidenceMatrix_hh

#include "TrigT1TGC/TGCSSCControllerOut.hh"
#include "TrigT1TGC/TGCRPhiCoincidenceMap.hh"

namespace LVL1TGCTrigger {

class TGCSectorLogic;
class TGCRPhiCoincidenceOut;

const int NumberOfPtLevel = 6;

class TGCRPhiCoincidenceMatrix {
public:
  void setSSCId(int SSCIdIn){ SSCId=SSCIdIn;};
  void setRPhiMap(const TGCRPhiCoincidenceMap* map);
  void inputR(int rIn, int dRIn, int ptRIn);
  void inputPhi(int phiIn, int dPhiIn, int ptPhiIn);
  void clear();
  TGCRPhiCoincidenceOut* doCoincidence();

  TGCRPhiCoincidenceMatrix(const  TGCSectorLogic*  sL=0);
  virtual ~TGCRPhiCoincidenceMatrix();

  TGCRPhiCoincidenceMatrix(const TGCRPhiCoincidenceMatrix& right);
  TGCRPhiCoincidenceMatrix& operator=(const TGCRPhiCoincidenceMatrix& right);

private:

  enum {MaxNPhiHit = 2}; 
    
  const  TGCSectorLogic*  sectorLogic; 

  TGCRPhiCoincidenceOut* matrixOut;
  const TGCRPhiCoincidenceMap* map;

  int nPhiHit;
  int SSCId;
  int r;
  int dR;
  int ptR;
  int phi[MaxNPhiHit];
  int dPhi[MaxNPhiHit];
  int ptPhi[MaxNPhiHit];
};

} //end of namespace bracket

#endif // TGCRPhiCoincidenceMatrix_hh

