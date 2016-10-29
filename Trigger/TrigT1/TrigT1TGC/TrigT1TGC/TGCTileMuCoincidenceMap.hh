/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCTileMuCoincidenceMap_hh
#define TGCTileMuCoincidenceMap_hh

#include <string>

#include "GaudiKernel/ToolHandle.h"

class ITGCTriggerDbTool;

namespace LVL1TGCTrigger {
 
class TGCTileMuCoincidenceMap {
public:

  TGCTileMuCoincidenceMap(const std::string& version="NA");
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
  int flagPT[N_PT_THRESH][N_Endcap_SSC][N_EndcapSector][N_Side]; 
  // 1 use; 0: not use; -1: not used for Trigger

  int flagROI[N_ROI_IN_SSC][N_Endcap_SSC][N_EndcapSector][N_Side]; 
  // 1 use; 0: not use; -1: not used for Trigger

  int map[N_Input_TileMuModule][N_Endcap_SSC][N_EndcapSector][N_Side];    

  std::string m_verName;
  ToolHandle<ITGCTriggerDbTool> m_condDbTool;
};


inline  
 const std::string& TGCTileMuCoincidenceMap::getVersion() const
{
  return m_verName;
}

inline
 int  TGCTileMuCoincidenceMap::getMask(const int module, 
				       const int ssc, 
				       const int sec,
				       const int side ) const
{
  if ((module<0)||(module>=N_Input_TileMuModule)) return TM_NA;
  if ((ssc<0)||(ssc>=N_Endcap_SSC)) return TM_NA;
  if ((sec<0)||(sec>=N_EndcapSector)) return TM_NA;
  if ((side<0)||(side>=N_Side)) return TM_NA;
  return  map[module][ssc][sec][side];
}

inline
 int  TGCTileMuCoincidenceMap::getFlagPT(const int pt, 
					 const int ssc, 
					 const int sec,
					 const int side)  const
{
  if ((pt<=0)||(pt>N_PT_THRESH)) return -1;
  if ((ssc<0)||(ssc>=N_Endcap_SSC)) return -1;
  if ((sec<0)||(sec>=N_EndcapSector)) return -1;
  if ((side<0)||(side>=N_Side)) return -1;
  
  return  flagPT[pt-1][ssc][sec][side]; 
}

inline
 int  TGCTileMuCoincidenceMap::getFlagROI(const int roi, 
					  const int ssc, 
					  const int sec,
					  const int side)  const
{
  if ((roi<0)||(roi>=N_ROI_IN_SSC)) return -1;
  if ((ssc<0)||(ssc>=N_Endcap_SSC)) return 0;
  if ((sec<0)||(sec>=N_EndcapSector)) return -1;
  if ((side<0)||(side>=N_Side)) return -1;
  
  return  flagROI[roi][ssc][sec][side]; 
}


} //end of namespace bracket

#endif // TGCTileMuCoincidenceMap_hh


