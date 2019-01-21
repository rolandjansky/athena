/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCTileMuCoincidenceMap_hh
#define TGCTileMuCoincidenceMap_hh

#include <string>

#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/ReadCondHandle.h"
#include "MuonCondSvc/TGCTriggerData.h"

namespace LVL1TGCTrigger {
 
class TGCTileMuCoincidenceMap {
public:

  TGCTileMuCoincidenceMap(const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey,
                          const std::string& version="NA");
  virtual ~TGCTileMuCoincidenceMap();

  TGCTileMuCoincidenceMap(const TGCTileMuCoincidenceMap& right);
  TGCTileMuCoincidenceMap& operator=(const TGCTileMuCoincidenceMap& right);

  enum TileMuMask {
    TM_NA    =  0,
    TM_D6_L  =  1,
    TM_D6_H  =  5,
    TM_D56_L =  3,
    TM_D56_H =  7
  };

  enum {N_Input_TileMuModule=4};
  
  int                         getMask(const int module, 
				      const int ssc, 
				      const int sec,
				      const int side ) const;

  int                         getFlagPT(const int pt,
					const int ssc,
					const int sec,
					const int side )  const;

  int                         getFlagROI(const int roi,
					 const int ssc,
					 const int sec,
					 const int side )  const;

  const std::string&          getVersion() const;

  void                        dumpMap() const;

  bool readMap();  

protected:
  enum {N_Side=2};
  enum {N_EndcapSector=48};
  enum {N_Endcap_SSC=19};
  enum {N_PT_THRESH=6};
  enum {N_ROI_IN_SSC=8};

private:
  int m_flagPT[N_PT_THRESH][N_Endcap_SSC][N_EndcapSector][N_Side]; 
  // 1 use; 0: not use; -1: not used for Trigger

  int m_flagROI[N_ROI_IN_SSC][N_Endcap_SSC][N_EndcapSector][N_Side]; 
  // 1 use; 0: not use; -1: not used for Trigger

  int m_map[N_Input_TileMuModule][N_Endcap_SSC][N_EndcapSector][N_Side];    

  std::string m_verName;
  const SG::ReadCondHandleKey<TGCTriggerData>& m_readCondKey;
};


inline  
 const std::string& TGCTileMuCoincidenceMap::getVersion() const
{
  return m_verName;
}

} //end of namespace bracket

#endif // TGCTileMuCoincidenceMap_hh


