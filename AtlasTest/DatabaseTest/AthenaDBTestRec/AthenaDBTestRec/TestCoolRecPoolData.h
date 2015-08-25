/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENADBTESTREC_TESTCOOLRECPOOLDATA_H
#define ATHENADBTESTREC_TESTCOOLRECPOOLDATA_H

// TestCoolRecPoolData.h - test class for conditions data stored as POOL obj
// Richard Hawkings, started 15/8/06

#include<vector>
#include<string>
#include "CLIDSvc/CLASS_DEF.h"

class TestCoolRecPoolData {
 public:
  TestCoolRecPoolData();
  TestCoolRecPoolData(const int run, const int chan, const std::string folder,
		      const int size);
  ~TestCoolRecPoolData();
  // get methods
  int run() const;
  int chan() const;
  const std::string folder() const;
  // data access
  int size() const;
  std::vector<float>::iterator dbegin();
  std::vector<float>::iterator dend();
  std::vector<float>::const_iterator cdbegin() const;
  std::vector<float>::const_iterator cdend() const;

 private:
  int m_run;
  int m_chan;
  std::string m_folder;
  std::vector<float> m_data;
};

inline int TestCoolRecPoolData::run() const { return m_run;}
inline int TestCoolRecPoolData::chan() const { return m_chan;}
inline const std::string TestCoolRecPoolData::folder() const { return m_folder;}
inline int TestCoolRecPoolData::size() const { return m_data.size();}
inline std::vector<float>::iterator TestCoolRecPoolData::dbegin()
{ return m_data.begin(); }
inline std::vector<float>::iterator TestCoolRecPoolData::dend()
{ return m_data.end(); }
inline std::vector<float>::const_iterator TestCoolRecPoolData::cdbegin() const
{ return m_data.begin(); }
inline std::vector<float>::const_iterator TestCoolRecPoolData::cdend() const
{ return m_data.end(); }

CLASS_DEF( TestCoolRecPoolData , 113918902 , 1 )

#endif // ATHENADBTESTREC_TESTCOOLRECPOOLDATA_H
