/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARHVDATA_H
#define LARHVDATA_H

// Includes for Gaudi
#include <vector>
#include <map>
#include <set>

class Identifier;
class StatusCode;

class LArHVData {
 
  friend class LArHVCondAlg; //The conditions alg filling this object 

  public: 
  LArHVData() = default;
  ~LArHVData () = default;

  /** @brief struct for HV, current and weight 
   */
  struct HV_t {
  HV_t(float ihv, float icurr, float iweight) : 
    hv(ihv),current(icurr),weight(iweight) {};
    float hv;
    float current;
    float weight; 
  }; 

  const std::set<Identifier>& getUpdatedCells() const;

  /** brief  Given a Offline Readout ID, return values of HV and Weight 
   */
  const std::vector<LArHVData::HV_t> & getHV(const Identifier& id) const;

  typedef std::map<Identifier, std::vector<HV_t> > hvMap;

  private:
  hvMap m_voltage;
  std::set<Identifier> m_updatedCells;

  const std::vector<LArHVData::HV_t> m_empty {};

};

inline 
const std::set<Identifier>&  LArHVData::getUpdatedCells() const { 
   return m_updatedCells;
}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArHVData, 94850034, 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<LArHVData>, 35533232, 1) 

#endif
