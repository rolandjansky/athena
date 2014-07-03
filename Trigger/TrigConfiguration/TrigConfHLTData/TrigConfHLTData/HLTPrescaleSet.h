/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_HLTPrescaleSet
#define TrigConf_HLTPrescaleSet

#include <iosfwd>
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <stdint.h>

#include "TrigConfHLTData/HLTPrescale.h"
#include "TrigConfHLTData/HLTLevel.h"
#include "TrigConfL1Data/TrigConfData.h"

#include <boost/unordered_map.hpp>


namespace TrigConf {
   class HLTPrescaleSet;
}

namespace TrigConf {

   std::ostream & operator<<(std::ostream &, const TrigConf::HLTPrescaleSet &);

   /**@brief HLT chain configuration information*/
   class HLTPrescaleSet : public TrigConfData {
   public:

      typedef boost::unordered_map<unsigned int, HLTPrescale> ScalingMap_t;

      /**@brief default constructor*/
      HLTPrescaleSet();

      /**@brief destructor*/
      ~HLTPrescaleSet();

      HLTPrescale& setPrescale(const HLTPrescale& sc, unsigned int chain_counter, HLTLevel level = HLT);

      const HLTPrescale& getPrescale(unsigned int chain_counter, HLTLevel level = HLT) const;

      bool hasPrescale( unsigned int counter, TrigConf::HLTLevel level = HLT ) const;

      HLTPrescale& thePrescale(unsigned int chain_counter, HLTLevel level);

      const ScalingMap_t& getPrescales(HLTLevel level) const { return m_scalers[static_cast<unsigned int>(level)]; }

      size_t size(HLTLevel level=HLT) const { return m_scalers[static_cast<unsigned int>(level)].size(); }

      void reset();

      /**@brief print the prescale set*/
      void print(const std::string& indent="", unsigned int detail=1) const;

      /**@brief sets the IOV of the prescale set*/
      void setIOV(uint32_t start_run, uint32_t start_lb, uint32_t end_run, uint32_t end_lb );
      void setIOV(uint64_t start, uint64_t end );

      /**@brief gets the IOV of the prescale set*/
      void getIOV(uint32_t& start_run, uint32_t& start_lb, uint32_t& end_run, uint32_t& end_lb ) const;

      /**@brief checks if run/lb are in the current IOV*/
      bool isValid(uint32_t run, uint32_t lb ) const;

      std::string __str__() const;

   private:
      std::vector<ScalingMap_t>   m_scalers;

      uint64_t  m_iovstart;
      uint64_t  m_iovend;

      friend std::ostream & operator<<(std::ostream &, const TrigConf::HLTPrescaleSet &);
   };

}

#endif
