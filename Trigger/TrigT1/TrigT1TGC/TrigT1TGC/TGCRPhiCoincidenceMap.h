/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRPhiCoincidenceMap_hh
#define TGCRPhiCoincidenceMap_hh

#include <vector>
#include <map>
#include <string>

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonCondSvc/TGCTriggerData.h"
#include "TrigT1TGC/TGCArguments.h"

namespace LVL1TGCTrigger {

class TGCRPhiCoincidenceMap {
public:
  bool test(int octantId, int moduleId, int subsector, 
            int type, int pt, 
            int dr, int dphi) const;// for Run2

  int test_Run3(int octantId, int moduleId, int subsector, 
                int type, int dr, int dphi) const; //for Run3


  int   getMapType(int hlwire, int hlstrip) const;
  const std::string& getVersion() const;
  int   getSideId() const;
  int   getOctantId() const;
  bool  isFullCW() const;
  void  setFullCW( bool val);

  TGCRPhiCoincidenceMap(TGCArguments*,
                        const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey,
                        const std::string& version,
                        int   sideId=0, int octantId=0);

  virtual ~TGCRPhiCoincidenceMap();

  // copy and assignment operator
  TGCRPhiCoincidenceMap(const TGCRPhiCoincidenceMap& right);
  TGCRPhiCoincidenceMap& operator=(const TGCRPhiCoincidenceMap& right);

  bool readMap(); // for Run2
  bool readMap_Run3();// for Run3

  const TGCArguments* tgcArgs() const;

private: // hide default constructor
  TGCRPhiCoincidenceMap() = delete;

protected:
  bool checkVersion();
  int PHIPOS(int iphi, int type) const;
  int SUBSECTORADD(int ssid, int modid, int phimod2, int type) const;
  int getMODID(int addr) const;
  int getSSID(int addr) const;
  int getTYPE(int addr) const;
  int getTYPE(int lDR, int hDR, int lDPhi, int hDPhi ) const;
 
  enum {TMap_HH=0, TMap_HL, TMap_LH, TMap_LL, N_TMap};
  enum {N_PT_THRESH=6};
  enum {NumberOfCoincidenceType=4};
  enum {NumberOfModuleType=12};
  enum {DR_offset=-15, DPhi_offset=-7};

 //converter from char to pTthre Number. the sign mean muon sign.
  std::map<char, int> m_pTdef={{'X',0},
              {'A',1},{'B',2},{'C',3},{'D',4},{'E',5},{'F',6},{'G',7},{'H',8},{'I',9},{'J',10},{'K',11},{'L',12},{'M',13},{'N',14},{'O',15},
              {'a',-1},{'b',-2},{'c',-3},{'d',-4},{'e',-5},{'f',-6},{'g',-7},{'h',-8},{'i',-9},{'j',-10},{'k',-11},{'l',-12},{'m',-13},{'n',-14},{'o',-15} };


private:
  std::map<int, std::map<int,int> > m_mapDB[N_PT_THRESH];//for Run2 [ptLevel]< RoI(&type),<RNumber,RWindow> >
  std::map<int, std::map<int, std::map<int, char> > > m_mapDB_Run3;//for Run3 <RoI(&type),<R,<Phi,pT(char)> > >



  int m_numberOfDR,m_numberOfDPhi;
  std::string m_verName;
  int m_side;
  int m_octant;
  bool m_fullCW;
  
  TGCArguments* m_tgcArgs;

  const SG::ReadCondHandleKey<TGCTriggerData>& m_readCondKey;
};

inline 
const TGCArguments* TGCRPhiCoincidenceMap::tgcArgs() const {
  return m_tgcArgs;
}

inline  
 const std::string& TGCRPhiCoincidenceMap::getVersion() const
{
  return m_verName;
}

inline   
 int TGCRPhiCoincidenceMap::getSideId() const
{
  return m_side;
}

inline   
 int TGCRPhiCoincidenceMap::getOctantId() const
{
  return m_octant;
}

inline   
 bool TGCRPhiCoincidenceMap::isFullCW() const
{
  return m_fullCW;
}

inline   
 void TGCRPhiCoincidenceMap::setFullCW( bool val )
{
  m_fullCW = val;
}

inline
 int TGCRPhiCoincidenceMap::getTYPE(int lDR, int hDR, int lDPhi, int hDPhi ) const
 {
   int type = -1;
   if ( (lDR==-15) && (hDR==15) && (lDPhi==-7) && (hDPhi==7))      type = TMap_HH;
   else if ( (lDR==-15) && (hDR==15) && (lDPhi==-3) && (hDPhi==3)) type = TMap_HL;
   else if ( (lDR==-7) && (hDR==7) && (lDPhi==-7) && (hDPhi==7))   type = TMap_LH;
   else if ( (lDR==-7) && (hDR==7) && (lDPhi==-3) && (hDPhi==3))   type = TMap_LL; 
   return type; 
}

inline
 int  TGCRPhiCoincidenceMap::getMapType(int hlwire, int hlstrip) const
 { return (N_TMap - 2*hlwire - hlstrip - 1) ; }


inline 
 int TGCRPhiCoincidenceMap::PHIPOS(int iphi, int ) const
 { 
   return ( iphi - DPhi_offset ); 
 }

inline 
 int TGCRPhiCoincidenceMap::SUBSECTORADD(int ssid, int modid, int phimod2, int type) const 
 { return (ssid+(modid<<8)+(phimod2<<12) + (type<<16) ); }

inline 
 int TGCRPhiCoincidenceMap::getMODID(int addr) const 
 { return ((addr>>8)&0x000f); }

inline 
 int TGCRPhiCoincidenceMap::getSSID(int addr) const
 { return (addr&0x00ff); }

inline 
 int TGCRPhiCoincidenceMap::getTYPE(int addr) const
 { return ((addr>>16)&0x0003); }


} //end of namespace bracket

#endif // TGCRPhiCoincidenceMap_hh


