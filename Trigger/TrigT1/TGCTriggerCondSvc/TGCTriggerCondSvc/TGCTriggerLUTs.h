/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCTRIGGERLUTS_H
#define TGCTRIGGERLUTS_H

#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/CLASS_DEF.h"

#include <string>
#include <map>
#include <fstream>


class TGCTriggerLUTs {

  friend class TGCTriggerCondAlg;
  friend class TGCTriggerBWCWReader;

 public:

  enum {CW_BW=0, CW_EIFI=1, CW_TILE=2, CW_NSW=3, CW_BIS78=4, WHICH_INNER=5, HOTRoI=6, LUT_NUM}; 
  enum {N_PT_THRESH=15,
        N_SIDE=2,
        N_OCTANT=8,
        N_MODULETYPE=12};
  enum {DR_offset=-15,
        DPhi_offset=-7};


  TGCTriggerLUTs();
  virtual ~TGCTriggerLUTs();

  std::string getData(int cwtype, std::string file) const;
  std::string getData(int cwtype, int channel) const;
  std::string getFile(int cwtype, int channel) const;
  std::string getVersion(int cwtype, int channel = 0) const;
  std::string getType(int cwtype, int channel = 0) const;

  std::map<int, std::map<int, std::map<int, char> > > getReadMapBw(int side, int octantId) const;

  private:

    std::map<std::string, std::string> m_datamap[LUT_NUM];
    std::vector<std::string> m_data[LUT_NUM];
    std::vector<std::string> m_file[LUT_NUM];
    std::vector<std::string> m_version[LUT_NUM];
    std::vector<std::string> m_type[LUT_NUM];
  
    std::map<int, std::map<int, std::map<int, char> > > m_mapDB_bw[N_SIDE][N_OCTANT]; //for Run3 <RoI(&type),<R,<Phi,pT(char)> > >

};


CLASS_DEF(TGCTriggerLUTs, 206388330, 1)

#include "AthenaKernel/CondCont.h"
CLASS_DEF(CondCont<TGCTriggerLUTs>, 123453230, 1)

#endif // TGCTRIGGERDATA_H
