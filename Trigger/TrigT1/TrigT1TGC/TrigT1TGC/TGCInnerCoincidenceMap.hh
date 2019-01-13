/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCInnerCoincidenceMap_hh
#define TGCInnerCoincidenceMap_hh

#include <vector>
#include <string>
#include "TrigT1TGC/TGCInnerTrackletSlot.hh"

#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/ReadCondHandle.h"
#include "MuonCondSvc/TGCTriggerData.h"

namespace LVL1TGCTrigger {
 
class TGCInnerCoincidenceMap {
public:

  TGCInnerCoincidenceMap(const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey);
  TGCInnerCoincidenceMap(const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey,
                         const std::string& version,int   sideId=0);
  virtual ~TGCInnerCoincidenceMap();

  TGCInnerCoincidenceMap(const TGCInnerCoincidenceMap& right);
  TGCInnerCoincidenceMap& operator=(const TGCInnerCoincidenceMap& right);

  const TGCInnerTrackletSlot* getInnerTrackletMask(const int input, 
						   const int ssc, 
						   const int sec) const;
  int                         getFlagPT(const int pt,
					const int ssc,
					const int sec)  const;

  int                         getFlagROI(const int roi,
					const int ssc,
					const int sec)  const;

  int                         getTriggerBit(const int slot,
                                            const int ssc,
                                            const int sec,
                                            const int reg,
                                            const int read,
                                            const int bit) const;
  

  const std::string&          getVersion() const;
  int                         getSideId() const;
  bool                        isFullCW() const;
  void                        setFullCW( bool val);

  bool readMap();  
  void                        dumpMap() const;

protected:
 
  enum {N_EndcapSector=48};
  enum {N_Input_InnerSector=4};
  enum {N_Endcap_SSC=19};
  enum {N_PT_THRESH=6};
  enum {N_ROI_IN_SSC=8};

private:
  int flagPT[N_PT_THRESH][N_Endcap_SSC][N_EndcapSector]; 
  // 1 use; 0: not use; -1: not used for Trigger

  int flagROI[N_ROI_IN_SSC][N_Endcap_SSC][N_EndcapSector]; 
  // 1 use; 0: not use; -1: not used for Trigger

  TGCInnerTrackletSlot map[N_Input_InnerSector][N_Endcap_SSC][N_EndcapSector];    

  std::string m_verName;
  int m_side; 
  bool m_fullCW;

  const SG::ReadCondHandleKey<TGCTriggerData>& m_readCondKey;
};


inline  
 const std::string& TGCInnerCoincidenceMap::getVersion() const
{
  return m_verName;
}

inline   
 int TGCInnerCoincidenceMap::getSideId() const
{
  return m_side;
}

inline   
 bool TGCInnerCoincidenceMap::isFullCW() const
{
  return m_fullCW;
}

inline   
 void TGCInnerCoincidenceMap::setFullCW( bool val )
{
  m_fullCW = val;
}

inline
 const TGCInnerTrackletSlot* TGCInnerCoincidenceMap::getInnerTrackletMask(const int input, 
									  const int ssc, 
									  const int sec) const
{
  if ((input<0)||(input>=N_Input_InnerSector)) return 0;
  if ((ssc<0)||(ssc>=N_Endcap_SSC)) return 0;
  if ((sec<0)||(sec>=N_EndcapSector)) return 0;

  return  &(map[input][ssc][sec]);    
}


} //end of namespace bracket

#endif // TGCInnerCoincidenceMap_hh


