/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCTRIGGERDATA_H
#define TGCTRIGGERDATA_H

#include "SGTools/BaseInfo.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <string>
#include <map>
#include <fstream>

class TGCTriggerData {

  friend class TGCTriggerDbAlg;

  public:

    enum {CW_BW=0, CW_EIFI=1, CW_TILE=2, CW_NUM=3}; 
    enum {N_PT_THRESH=6,
          N_SIDE=2,
          N_OCTANT=8,
          N_ENDCAP_SSC=19,
          N_ENDCAP_SECTOR=48,
          N_ROI_IN_SSC=8,
          N_EIFI_INPUT=4,
          N_EIFI_REGION=2,
          N_EIFI_READOUT=2,
          N_EIFI_TRIGBIT=4,
          N_TILE_INPUT=4};

    TGCTriggerData();
    virtual ~TGCTriggerData();

    std::string getData(int cwtype, std::string file) const;
    std::string getData(int cwtype, int channel) const;
    std::string getFile(int cwtype, int channel) const;
    std::string getVersion(int cwtype, int channel = 0) const;
    std::string getType(int cwtype, int channel = 0) const;
    bool isActive(int cwtype, int channel = 0) const;

    std::map<int, std::map<int,int> > getReadMapBw(int side, int octantId, int pt) const;

    bool getTrigBitEifi(int side, int slot, int ssc, int sectorId, int reg, int read, int bit) const;  
    int getFlagPtEifi(int side, int pt, int ssc, int sectorId) const;  
    int getFlagRoiEifi(int side, int roi, int ssc, int sectorId) const;  

    int getTrigMaskTile(int slot, int ssc, int sectorId, int side) const;  
    int getFlagPtTile(int pt, int ssc, int sectorId, int side) const;  
    int getFlagRoiTile(int roi, int ssc, int sectorId, int side) const;  

  private:

    std::map<std::string, std::string> m_datamap[CW_NUM];
    std::vector<std::string> m_data[CW_NUM];
    std::vector<std::string> m_file[CW_NUM];
    std::vector<std::string> m_version[CW_NUM];
    std::vector<std::string> m_type[CW_NUM];
    std::vector<bool>        m_active[CW_NUM];
  
    std::map<int, std::map<int,int> > m_readmap_bw[N_SIDE][N_OCTANT][N_PT_THRESH];

    bool m_trigbit_eifi[N_SIDE][N_EIFI_INPUT][N_ENDCAP_SSC][N_ENDCAP_SECTOR][N_EIFI_REGION][N_EIFI_READOUT][N_EIFI_TRIGBIT];
    int  m_flagpt_eifi[N_SIDE][N_PT_THRESH][N_ENDCAP_SSC][N_ENDCAP_SECTOR];
    int  m_flagroi_eifi[N_SIDE][N_ROI_IN_SSC][N_ENDCAP_SSC][N_ENDCAP_SECTOR];

    int m_trigbit_tile[N_TILE_INPUT][N_ENDCAP_SSC][N_ENDCAP_SECTOR][N_SIDE]; 
    int m_flagpt_tile[N_PT_THRESH][N_ENDCAP_SSC][N_ENDCAP_SECTOR][N_SIDE];
    int m_flagroi_tile[N_ROI_IN_SSC][N_ENDCAP_SSC][N_ENDCAP_SECTOR][N_SIDE];

};

CLASS_DEF(TGCTriggerData, 72345188, 1)

#include "AthenaKernel/CondCont.h"
CLASS_DEF(CondCont<TGCTriggerData>, 96649668, 1)

#endif // TGCTRIGGERDATA_H
