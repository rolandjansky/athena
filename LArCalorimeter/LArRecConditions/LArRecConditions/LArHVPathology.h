/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHVPATHOLOGY_H
#define LARHVPATHOLOGY_H

#include "Identifier/Identifier.h"
#include <vector>
#include <map>

#include "LArRecConditions/LArHVPathologiesDb.h"

class LArHVPathology
{
 public:

  LArHVPathology() {}
  LArHVPathology(const LArHVPathology* in);
  LArHVPathology(const LArHVPathologiesDb* in);
  virtual ~LArHVPathology() {}

  const std::vector<LArHVPathologiesDb::LArHVElectPathologyDb>& getPathology() const {
    return m_v;
  }
  std::map<std::pair<Identifier, unsigned int>, std::vector<unsigned short> >&getElecMap() {
    return m_electIndMap;
  }

 private:
  std::vector<LArHVPathologiesDb::LArHVElectPathologyDb> m_v;
  std::map<std::pair<Identifier, unsigned int>, std::vector<unsigned short> > m_electIndMap;

};

inline LArHVPathology::LArHVPathology(const LArHVPathology* in) {
   if(in) {
      for(unsigned i=0;i<in->m_v.size();++i) m_v.push_back(in->m_v[i]);
      for(const auto &p : in->m_electIndMap) m_electIndMap.emplace(std::make_pair(p.first, p.second));
   }
}

inline LArHVPathology::LArHVPathology(const LArHVPathologiesDb* in) {
   if(in) {
      for(unsigned i=0;i<in->m_v.size();++i) m_v.push_back(in->m_v[i]);
   }
}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArHVPathology, 180546408, 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<LArHVPathology>, 24283336, 1) 

#endif
