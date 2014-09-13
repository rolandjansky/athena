/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBBLOBOBJS_TILEBCHWORDS_H
#define TILECALIBBLOBOBJS_TILEBCHWORDS_H

#include "TileCalibBlobObjs/Exception.h"
#include <stdint.h>
#include <vector>

class TileBchWords{
 public:
  TileBchWords() : m_words(std::vector<uint32_t>(2,0)) {}
  ~TileBchWords() {}
  uint32_t  getChnWord() const {return m_words[0];}
  uint32_t  getAdcWord() const {return m_words[1];}
  const uint32_t& operator[](unsigned int idx) const;
  uint32_t&       operator[](unsigned int idx);
 private:
  std::vector<uint32_t> m_words;
};

//
//_____________________________________________________
inline uint32_t&
TileBchWords::operator[](unsigned int idx)
{
  if(idx>1) throw TileCalib::IndexOutOfRange("TileBchWords::operator[]", idx, 1);
  return m_words[idx];
}

//
//_____________________________________________________
inline const uint32_t&
TileBchWords::operator[](unsigned int idx) const
{
  if(idx>1) throw TileCalib::IndexOutOfRange("TileBchWords::operator[]", idx, 1);
  return m_words[idx];
}

#endif
