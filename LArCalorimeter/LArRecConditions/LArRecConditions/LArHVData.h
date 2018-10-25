/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
   LArHVData() { }
   ~LArHVData () { } 

  /** @brief struct for HV and weight pair 
   */
  struct HV_t {
    float hv;
    float weight; 
  }; 

  /** @brief  struct for Current and weight pair 
   */
  struct CURRENT_t {
    float current;
    float weight; 
  }; 

  const std::set<Identifier>& getUpdatedCells();

  /** brief  Given a Offline Readout ID, return values of HV and Weight 
   */
  StatusCode getHV(const Identifier& id, std::vector< HV_t > & v  ) const ; 

  /**  Given a Offline Readout ID, return values of Current and Weight 
   */
  StatusCode getCurrent(const Identifier& id, std::vector< CURRENT_t > & v  ) const ; 

  typedef std::map<Identifier, std::vector<HV_t> > hvMap;
  typedef std::map<Identifier, std::vector<CURRENT_t> > currMap;

  private:
  hvMap m_voltage;
  currMap  m_current;
  std::set<Identifier> m_updatedCells;
};

inline 
const std::set<Identifier>&  LArHVData::getUpdatedCells() { 
   return m_updatedCells;
}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArHVData, 94850034, 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<LArHVData>, 35533232, 1) 

#endif
