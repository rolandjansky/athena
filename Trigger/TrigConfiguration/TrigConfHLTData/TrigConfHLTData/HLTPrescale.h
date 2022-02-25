/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_HLTPrescale
#define TrigConf_HLTPrescale

#include <iosfwd>
#include <string>
#include <map>
#include <vector>
#include <utility>

#include "TrigConfL1Data/TrigConfData.h"

#include <unordered_map>

namespace TrigConf {
   class HLTPrescale;
}

namespace TrigConf {

   std::ostream & operator<<(std::ostream &, const TrigConf::HLTPrescale &);

   class HLTPrescale {
   public:

      // constructor
      HLTPrescale(float prescale=1, float pass_through=-1);
      HLTPrescale(const HLTPrescale& o) = default;
      HLTPrescale(HLTPrescale&& o) noexcept = default;

      // destructor
      ~HLTPrescale();

      // assignment
      HLTPrescale& operator=(const HLTPrescale&) = default;
      HLTPrescale& operator=(HLTPrescale&&) noexcept = default;

      // maps for rerun and stream prescales
      typedef std::unordered_map<std::string, float> PrescaleMap_t;
      typedef PrescaleMap_t::value_type value_type;

      // setters
      HLTPrescale& setPrescale(float prescale) { m_prescale = prescale; return *this; }
      HLTPrescale& setPassThrough(float pass_through) { m_pass_through = pass_through; return *this; }
      HLTPrescale& setRerunPrescale(const std::string& targetName, float ps);
      HLTPrescale& setStreamPrescale(const std::string& streamName, float ps);

      // getters
      float prescale() const { return m_prescale; }
      float pass_through() const { return m_pass_through; }
      std::pair<bool, float> getRerunPrescale(const std::string& targetName) const;
      std::pair<bool, float> getStreamPrescale(const std::string& streamName) const;
      const PrescaleMap_t& getRerunPrescales() const { return m_rerun_prescale; }
      const PrescaleMap_t& getStreamPrescales() const { return m_stream_prescale; }

      bool disabled() const { return m_prescale<0; }

      std::string __str__() const;

   private:
      // data
      float m_prescale;
      float m_pass_through;
      PrescaleMap_t m_rerun_prescale;
      PrescaleMap_t m_stream_prescale;
   };

}

#endif
