/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_CRC_COMBINE_H
#define ATHENAKERNEL_CRC_COMBINE_H 1

#include <string>
#include <boost/crc.hpp>
/// using crc32 for architecture independence in combining the seeds
inline uint32_t crc_combine(uint32_t seed, uint32_t v) {
  boost::crc_32_type crf;
  crf.process_bytes(&seed,sizeof(uint32_t));
  crf.process_bytes(&v,sizeof(uint32_t));
  return crf.checksum();
}
/// using crc32 for architecture independence in combining the seeds
inline uint32_t crc_combine(uint32_t seed, const std::string& v) {
  boost::crc_32_type crf;
  crf.process_bytes(&seed,sizeof(uint32_t));
  crf.process_bytes(v.c_str(),v.size());
  return crf.checksum();
}
#endif
