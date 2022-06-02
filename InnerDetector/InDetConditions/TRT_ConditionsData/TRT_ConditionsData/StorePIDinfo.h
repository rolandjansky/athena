/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef INDETTRT_STOREPIDINFO
#define INDETTRT_STOREPIDINFO

///////////////////////////////////////////////////////////////////
// StorePIDinfo.h , (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#include <vector>
#include "GaudiKernel/StatusCode.h"

class StorePIDinfo{
  public:
    //defaults (rule of 5)
    StorePIDinfo() = default;
    StorePIDinfo(const StorePIDinfo&) = default;
    StorePIDinfo(StorePIDinfo&&) = default;
    StorePIDinfo& operator=(const StorePIDinfo&) = default;
    StorePIDinfo& operator=(StorePIDinfo&&) = default;
    ~StorePIDinfo() = default;

    StorePIDinfo(int nbins, float min, float max, const std::vector<float>& values);
    void update(int nbins, float min, float max, const std::vector<float>& values);
    void push_back(float value);
    StatusCode check(int gas, int detpart) const;
    float GetValue(float input) const { return m_values.at(GetBin(input)); }
    float GetBinValue(int bin) const { return m_values.at(bin); }
    int GetBin(float input) const;

  private:
    unsigned int m_nbins = 0;
    float m_min = -9999.9;
    float m_max = 10000000 * 2;
    std::vector<float> m_values;
};
#endif

