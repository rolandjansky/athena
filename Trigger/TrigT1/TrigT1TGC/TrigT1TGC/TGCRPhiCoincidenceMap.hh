/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRPhiCoincidenceMap_hh
#define TGCRPhiCoincidenceMap_hh

#include <vector>
#include <map>
#include <string>

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonCondSvc/TGCTriggerData.h"

namespace LVL1TGCTrigger {

class TGCRPhiCoincidenceMap {
public:
  bool test(int octantId, int moduleId, int subsector, 
	    int type, int pt, 
	    int dr, int dphi) const;

  int   getMapType(int hlwire, int hlstrip) const;
  const std::string& getVersion() const;
  int   getSideId() const;
  int   getOctantId() const;
  bool  isFullCW() const;
  void  setFullCW( bool val);

  TGCRPhiCoincidenceMap(const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey,
                        const std::string& version,
			int   sideId=0, int octantId=0);

  virtual ~TGCRPhiCoincidenceMap();

  // copy and assignment operator
  TGCRPhiCoincidenceMap(const TGCRPhiCoincidenceMap& right);
  TGCRPhiCoincidenceMap& operator=(const TGCRPhiCoincidenceMap& right);

  bool readMap();  

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
  enum {DR_offset=-15, DPhi_offset=-7};

private:
  std::map<int, std::map<int,int> > m_mapDB[N_PT_THRESH];

  int m_numberOfDR,m_numberOfDPhi;
  std::string m_verName;
  int m_side;
  int m_octant;
  bool m_fullCW;

  const SG::ReadCondHandleKey<TGCTriggerData>& m_readCondKey;
};


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


