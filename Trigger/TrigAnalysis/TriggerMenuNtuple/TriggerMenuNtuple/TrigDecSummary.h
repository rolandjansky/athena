/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigDecSummary_h__
#define __TrigDecSummary_h__
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
  std::vector<unsigned int> mL1_TBP;
  std::vector<unsigned int> mL1_TAP;
  std::vector<unsigned int> mL1_TAV;

  std::vector<unsigned int> mL2_Raw;
  std::vector<unsigned int> mL2_PT;

  std::vector<unsigned int> mEF_Raw;
  std::vector<unsigned int> mEF_PT;

};

#endif // __TrigDecSummary_h__
