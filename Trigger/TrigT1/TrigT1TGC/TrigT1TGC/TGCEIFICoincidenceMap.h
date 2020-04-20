/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1TGC_TGCEIFICoincidenceMap_hh
#define TrigT1TGC_TGCEIFICoincidenceMap_hh

#include "TrigT1TGC/TGCInnerTrackletSlot.hh"
#include "TrigT1TGC/TGCArguments.hh"

#include "StoreGate/ReadCondHandle.h"
#include "MuonCondSvc/TGCTriggerData.h"

#include "AthenaKernel/MsgStreamMember.h"

#include <string>
#include <bitset>

namespace LVL1TGCTrigger {
 
class TGCEIFICoincidenceMap
{
 public:

  TGCEIFICoincidenceMap(TGCArguments*, const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey);
  TGCEIFICoincidenceMap(TGCArguments*, const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey,
                         const std::string& version, int sideId=0);
  virtual ~TGCEIFICoincidenceMap();

  TGCEIFICoincidenceMap(const TGCEIFICoincidenceMap& right);
  TGCEIFICoincidenceMap& operator = (const TGCEIFICoincidenceMap& right);

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

  TGCArguments* tgcArgs() const;

  /** Declaring the standard message stream
   *  Returns a reference to the default message stream
   */
  MsgStream& msg(const MSG::Level lvl) const;
  /** Test method to evaluate the verbosity level */
  bool msgLvl(const MSG::Level lvl) const;
  /** set message level to propagate from the parent class */
  void setMessageLevel(const MSG::Level lvl) const;

 protected:
  enum {N_EndcapSector=48};
  enum {N_Input_InnerSector=4};
  enum {N_Endcap_SSC=19};
  enum {N_PT_THRESH=6};
  enum {N_ROI_IN_SSC=8};

 private:
  // The flagPT is a flag whether each pT is applied the coincidence or not.
  // In each bit (of N_PT_THRESH bits), 1 applies the coincidence, and 0 is not.
  // The LSB is the lowest pT threshold, and MSB is the highest pT threshold.
  std::bitset<N_PT_THRESH> m_flagPT[N_Endcap_SSC][N_EndcapSector];

  int m_flagROI[N_ROI_IN_SSC][N_Endcap_SSC][N_EndcapSector]; 
  // 1 use; 0: not use; -1: not used for Trigger

  TGCInnerTrackletSlot m_map[N_Input_InnerSector][N_Endcap_SSC][N_EndcapSector];    

  std::string m_verName;
  int m_side; 
  bool m_fullCW;

  TGCArguments* m_tgcArgs;

  const SG::ReadCondHandleKey<TGCTriggerData>& m_readCondKey;

  //Declaring private message stream member.
  mutable Athena::MsgStreamMember m_msg;
};

inline TGCArguments* TGCEIFICoincidenceMap::tgcArgs() const
{
  return m_tgcArgs;
}

inline const std::string& TGCEIFICoincidenceMap::getVersion() const
{
  return m_verName;
}

inline int TGCEIFICoincidenceMap::getSideId() const
{
  return m_side;
}

inline bool TGCEIFICoincidenceMap::isFullCW() const
{
  return m_fullCW;
}

inline void TGCEIFICoincidenceMap::setFullCW(bool val)
{
  m_fullCW = val;
}

inline
 const TGCInnerTrackletSlot* TGCEIFICoincidenceMap::getInnerTrackletMask(const int input, 
									 const int ssc, 
									 const int sec) const
{
  if ((input<0)||(input>=N_Input_InnerSector)) return 0;
  if ((ssc<0)||(ssc>=N_Endcap_SSC)) return 0;
  if ((sec<0)||(sec>=N_EndcapSector)) return 0;

  return  &(m_map[input][ssc][sec]);    
}

inline MsgStream& TGCEIFICoincidenceMap::msg(const MSG::Level lvl) const { return m_msg << lvl; }

inline bool TGCEIFICoincidenceMap::msgLvl(const MSG::Level lvl) const
{
  return (m_msg.get().level() <= lvl) ? true : false;
}

inline void TGCEIFICoincidenceMap::setMessageLevel(const MSG::Level lvl) const { m_msg.get().setLevel(lvl); }

} //end of namespace bracket

#endif


