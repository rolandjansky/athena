// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_HLTPrescaleSetCollection
#define TrigConf_HLTPrescaleSetCollection

#include <iosfwd>
#include <string>
#include <list>
#include <vector>
#include <utility>
#include <exception>

#include <boost/thread/recursive_mutex.hpp>

namespace TrigConf {
   class HLTPrescaleSet;
   class HLTPrescaleSetCollection;

   std::ostream & operator<<(std::ostream &, const TrigConf::HLTPrescaleSetCollection &);

   /**@brief HLT chain configuration information*/
   class HLTPrescaleSetCollection {
   public:

      struct cont {
         cont(unsigned int _lb, unsigned int _psk, HLTPrescaleSet* _pss = 0) :
            lb(_lb), psk(_psk), pss(_pss) {};
         unsigned int    lb;
         unsigned int    psk;
         HLTPrescaleSet* pss;
      };

      /**@brief default constructor*/
      HLTPrescaleSetCollection();

      /**@brief destructor*/
      ~HLTPrescaleSetCollection();

      /** @brief get prescale set for lumiblock and set the internal
          current lumiblock counter
       **/
      HLTPrescaleSet* thePrescaleSet(unsigned int lumiblock) const;

      const HLTPrescaleSet* prescaleSet(unsigned int lumiblock) const;

      /** @brief access current prescale set (for current lumiblock) **/
      HLTPrescaleSet* thePrescaleSet() const { return m_currentPSS; }

      const HLTPrescaleSet* prescaleSet() const { return m_currentPSS; }


      /** @brief number of prescale sets */
      size_t size() const { return m_prescaleSets.size(); }

      /**@brief Add prescale set for this lumiblock number
       *
       * deletes all prescale sets
       */
      HLTPrescaleSet* setPrescaleSet( HLTPrescaleSet* pss );

      /**@brief Add prescale set for this lumiblock number */
      HLTPrescaleSet* addPrescaleSet( unsigned int lumiblock, HLTPrescaleSet* pss );

      /**@brief Check if prescale set with this lumiblock and prescale keys exists */
      bool contains(unsigned int lumiblock, unsigned int psk );

      /**@brief Deletes all prescale sets */
      void clear();

      std::list<cont>& sets() { return m_prescaleSets; }

      const std::list<cont>& sets() const { return m_prescaleSets; }

      std::vector<std::pair<unsigned int, unsigned int> > prescale_keys() const;

      void set_prescale_key_to_load(unsigned int);

      void set_prescale_keys_to_load(const std::vector<std::pair<unsigned int, unsigned int> >&);

      void setCurrentToFirstIfUnset();

      /**@brief Print the prescale set*/
      void print(const std::string& indent="", unsigned int detail=1) const;

      std::string __str__() const;

   private:

      HLTPrescaleSetCollection & operator=( const HLTPrescaleSetCollection & );

      HLTPrescaleSetCollection( const HLTPrescaleSetCollection & );

      HLTPrescaleSet* addPrescaleSet( const cont& add_psinfo );

      mutable unsigned int m_currentLB;
      mutable HLTPrescaleSet* m_currentPSS;

      std::list<cont> m_prescaleSets;         //!< all prescale sets with start lb      

      mutable boost::recursive_mutex m_prescaleSetCollection_mutex;  //!< Mutex for m_prescaleSetCollection
     
      friend std::ostream & operator<<(std::ostream &, const HLTPrescaleSetCollection &);

   };

}

#endif
