//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDIGITIZATION_LARHITLIST_H
#define LARDIGITIZATION_LARHITLIST_H
#include <vector>
class LArHitList {

  typedef std::vector< std::pair<float,float> > LARLIST;

private:
  bool m_inWindows;
  LARLIST m_list;

public:
   LArHitList(){};
  ~LArHitList(){};

   void AddHit(const float energy, const float time) {
     m_list.emplace_back(energy,time);
   }

   const LARLIST& getData() const { return m_list;}
   inline bool inWindows() const {return m_inWindows;};
   inline void setInWindows() {m_inWindows=true;};
};
#endif
