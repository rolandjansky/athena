/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_ReadCalibDataUtilities_h
#define SCT_ReadCalibDataUtilities_h

// Include STL stuff
#include <limits>
#include <string>

// Include boost stuff
#include "boost/array.hpp"
#include "boost/tokenizer.hpp"
#include "boost/lexical_cast.hpp"

#include "SCT_ConditionsServices/SCT_ConditionsParameters.h"

namespace SCT_ReadCalibChipUtilities {
  template <typename C> 
    bool fillFromString(const std::string& source, C& userContainer) {
    if (source.empty()) return false;
    typedef typename C::value_type V_t;
    V_t errVal{std::numeric_limits<V_t>::has_quiet_NaN ? (std::numeric_limits<V_t>::quiet_NaN()) : 0};
    boost::char_separator<char> sep{" "};
    typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;
    Tokenizer tok{source, sep};
    bool noNan{true};
    const Tokenizer::iterator end{tok.end()};
    int j{0};
    for (Tokenizer::iterator i{tok.begin()}; i!=end; ++i) {
      try {
        userContainer[j]=boost::lexical_cast<V_t>(*i);
      } catch (const boost::bad_lexical_cast&) {
        userContainer[j]=errVal;
        noNan=false;
      }
      ++j;
    }
    return noNan;
  }

  const boost::array<std::string, SCT_ConditionsServices::N_NPTGAIN> nPtGainDbParameterNames{
    {"gainByChip", "gainRMSByChip", "offsetByChip", "offsetRMSByChip", "noiseByChip", "noiseRMSByChip"}
};
  const boost::array<std::string, SCT_ConditionsServices::N_NPTGAIN> nPtGainParameterNames{
    {"GainByChip", "GainRMSByChip", "OffsetByChip", "OffsetRMSByChip", "NoiseByChip", "NoiseRMSByChip"}
};
  const boost::array<std::string, SCT_ConditionsServices::N_NOISEOCC> noiseOccDbParameterNames{
  {"occupancyByChip", "occupancyRMSByChip", "offsetByChip", "noiseByChip"}
};
  const boost::array<std::string, SCT_ConditionsServices::N_NOISEOCC> noiseOccParameterNames{
  {"OccupancyByChip", "OccupancyRMSByChip", "OffsetByChip", "NoiseByChip"}
};

  const std::string nPtGainFolder{"/SCT/DAQ/Calibration/ChipGain"};
  const std::string noiseOccFolder{"/SCT/DAQ/Calibration/ChipNoise"};
  
}//end of anon namespace

#endif // SCT_ReadCalibDataUtilities_h
