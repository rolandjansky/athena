/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUNTUPLE_TRIGDECSUMMARY_H
#define TRIGGERMENUNTUPLE_TRIGDECSUMMARY_H
/*
  TrigDecSummary.h
*/
#include <vector>

class TrigDecSummary {
public:
  enum DecType_t {
    kL1_TBP, 
    kL1_TAP, 
    kL1_TAV, 

    kL2_Raw, 
    kL2_PT, 

    kEF_Raw, 
    kEF_PT 

  };

public:
  TrigDecSummary();
  ~TrigDecSummary();

  void setBitStatus(unsigned int bit, DecType_t type, bool status);
  bool bitStatus(unsigned int bit, DecType_t type) const;

  void clear();

protected:
  void add(unsigned int n, std::vector<unsigned int>& v);
  void remove(unsigned int n, std::vector<unsigned int>& v);
  bool bit_status(unsigned int n, const std::vector<unsigned int>& v) const;

private:
  std::vector<unsigned int> m_L1_TBP;
  std::vector<unsigned int> m_L1_TAP;
  std::vector<unsigned int> m_L1_TAV;

  std::vector<unsigned int> m_L2_Raw;
  std::vector<unsigned int> m_L2_PT;

  std::vector<unsigned int> m_EF_Raw;
  std::vector<unsigned int> m_EF_PT;

};

#endif // TRIGGERMENUNTUPLE_TRIGDECSUMMARY_H
