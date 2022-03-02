/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_HLTChain
#define TrigConf_HLTChain

#include "TrigConfL1Data/TrigConfData.h"
#include "TrigConfHLTData/HLTPrescale.h"
#include "TrigConfHLTData/HLTLevel.h"

#include <string>
#include <iosfwd>
#include <fstream>
#include <vector>
#include <set>

#include <unordered_map>

namespace TrigConf {
   class HLTChain;
   class HLTSignature;
   class HLTTriggerType;
   class HLTStreamTag;
   class DiffStruct;
}
   
bool HLTChain_lt(const TrigConf::HLTChain* ch1, const TrigConf::HLTChain* ch2);
   
namespace TrigConf {

   std::ostream & operator<<(std::ostream &, const HLTChain &);

   /**@brief HLT chain configuration information*/
   class HLTChain : public TrigConfData {
   public:

      /**@brief default constructor*/
      HLTChain( void );

      /**@brief constructor with configuration data
       *
       * @param chain_name chain name
       * @param chain_counter chain counter
       * @param chain_version chain version
       * @param level trigger level
       * @param lower_chain_name name of lower chain
       * @param lower_chain_counter counter of lower chain
       * @param prescale prescale value of the chain
       * @param pass_through pass_through flag of the chain
       * @param signatureList list of signatures that the chain consists of
       */
      HLTChain( const std::string& chain_name,
                int chain_counter,
                int chain_version,
                const std::string& level,
                const std::string& lower_chain_name,
                int lower_chain_counter,
                std::vector<HLTSignature*>&& signatureList );

      /**@brief copy constructor
       *
       * does a deep copy of everything underneath
       **/
      HLTChain( const HLTChain& ch );

      /**@brief destructor*/
      ~HLTChain();


      // getters
      const std::string&                  chain_name            () const { return name(); }
      int                                 chain_counter         () const { return m_chain_counter; }
      int                                 chain_version         () const { return m_chain_version; }
      const std::string&                  level                 () const { return m_level; }
      HLTLevel                            level_enum            () const { return m_level=="L2"?L2:(m_level=="EF"?EF:HLT); }
      const std::string&                  lower_chain_name      () const { return m_lower_chain_name; }
      int                                 lower_chain_counter   () const { return m_lower_chain_counter; }
      unsigned int                        chain_hash_id         () const { return m_chain_hash_id; }
      unsigned int                        lower_chain_hash_id   () const { return m_lower_chain_hash_id; }
      int                                 EB_after_step         () const { return m_EB_after_step; }
      const std::vector<size_t>           leg_multiplicities    () const { return m_leg_multiplicities; }
      bool                                hasMultipleLowerChains() const;
      const std::vector<int>&             lower_chain_counters  () const;
      std::vector<unsigned int>           lower_chain_hash_ids  () const;


      std::vector<HLTTriggerType*>&       triggerTypeList()       { return m_HLTTriggerTypeList; }
      const std::vector<HLTTriggerType*>& triggerTypeList() const { return m_HLTTriggerTypeList; }

      // setters
      HLTChain& set_chain_name           ( const std::string&  chain_name);
      HLTChain& set_chain_counter        ( int chain_counter) { m_chain_counter = chain_counter; return *this; }
      HLTChain& set_chain_version        ( int chain_version) { m_chain_version = chain_version; return *this; }
      HLTChain& set_level                ( const std::string&  level) { m_level = level; return *this; }
      HLTChain& set_lower_chain_name     ( const std::string&  lower_chain_name);
      HLTChain& set_lower_chain_counter  ( int lower_chain_counter) { m_lower_chain_counter = lower_chain_counter; return *this; }
      HLTChain& set_lower_chain_counters ( const std::vector<int>& low_ccs) { m_lower_chain_counters = low_ccs; return *this; }
      HLTChain& set_signatureList        ( const std::vector<HLTSignature*>&   sigList);
      HLTChain& set_triggerTypeList      ( const std::vector<HLTTriggerType*>& trigList) { m_HLTTriggerTypeList = trigList; return *this; }
      HLTChain& set_groupList            ( const std::set<std::string>& groups) { m_groups = groups; return *this; }
      HLTChain& set_EB_after_step        ( int EB_after_step ) { m_EB_after_step = EB_after_step; return *this; }
      HLTChain& set_leg_multiplicities   ( const std::vector<size_t>& mult ) { m_leg_multiplicities = mult; return *this; }


      // signatures
      std::vector<HLTSignature*>&         signatureList()       { return m_HLTSignatureList; }
      const std::vector<HLTSignature*>&   signatureList() const { return m_HLTSignatureList; }
      const std::vector<HLTSignature*>&   signatures()    const { return m_HLTSignatureList; }
      unsigned int                        lastStep() const;
      void                                shiftStepCounter(int shift);


      // streams
      void addStream(HLTStreamTag*);
      void clearStreams();
      const std::vector<HLTStreamTag*>& streams() const { return m_streams; }
      #ifndef __GCCXML__
      const std::vector<HLTStreamTag*>& streamTagList() const __attribute__ ((deprecated)) { return m_streams; }  //deprecated
      #endif
      const std::vector<HLTStreamTag*>& streams_orig() const { return m_streams_orig; }
      std::pair<bool, float> stream_prescale(const std::string& streamName) const;


      // groups
      const std::set<std::string>& groups() const { return m_groups; }
      #ifndef __GCCXML__
      const std::set<std::string>& groupList() const __attribute__ ((deprecated)) { return m_groups; }
      #endif
      void addGroup(const std::string& group) { m_groups.insert(group); }
      void clearGroups() { m_groups.clear(); }

      // prescales
      HLTChain&              set_prescales( const HLTPrescale& prescales);
      HLTChain&              set_prescale( float prescale)             { m_prescales.setPrescale(prescale); return *this; }
      HLTChain&              set_rerun_prescale( float rerun_prescale) { m_prescales.setRerunPrescale("",rerun_prescale); return *this; }
      HLTChain&              set_pass_through( float pass_through)     { m_prescales.setPassThrough(pass_through); return *this; }

      HLTPrescale&           prescales() { return m_prescales; }
      const HLTPrescale&     prescales() const { return m_prescales; }
      float                  prescale() const { return prescales().prescale(); }
      float                  pass_through() const { return prescales().pass_through(); }
      std::pair<bool, float> rerun_prescale(const std::string& targetName) const { return prescales().getRerunPrescale(targetName); }
      #ifndef __GCCXML__
      float                  rerun_prescale() const __attribute__ ((deprecated)) { return prescales().getRerunPrescale("").second; }
      #endif

      inline bool operator<(const HLTChain& e) const {
         return m_level[0]>e.m_level[0] || 
            (m_level[0]==e.m_level[0] && m_chain_counter<e.m_chain_counter);
      }
      
      /**@brief label the signatures*/
      void createSignatureLabels();

      /**@brief print the chain*/
      void print(const std::string& indent="", unsigned int detail=1) const;

      void writeXML(std::ofstream & xmlfile);

      DiffStruct* compareTo(const HLTChain* o) const;

      // for python
      std::string __str__() const;


   private:


      unsigned int      m_chain_hash_id;       //!< hash value from m_chain_name, this is used to identify the chain in the HLTResult
      int               m_chain_counter;       //!< chain counter
      int               m_chain_version;       //!< chain version
      std::string       m_level;               //!< trigger level
      std::string       m_lower_chain_name;    //!< name of the lower trigger chain (or the LVL1 trigger item)
      int               m_lower_chain_counter; //!< counter of the lower trigger chain (or the ID of the LVL1 trigger item)
      std::vector<int>  m_lower_chain_counters;//!< counters of the lower trigger items if more than 1
      unsigned int      m_lower_chain_hash_id; //!< hash value from m_lower_chain_name, this is used to match to a chain from the previous trigger level
      int               m_EB_after_step;       //!< EB_after_step flag 
      std::vector<size_t> m_leg_multiplicities;//!< Number of objects required per leg. NOTE: Run3 only quantity
      HLTPrescale       m_prescales;

      std::vector<HLTSignature*>                         m_HLTSignatureList;
      std::vector<HLTTriggerType*>                       m_HLTTriggerTypeList;
      std::set<std::string>                              m_groups;
      std::vector<HLTStreamTag*>                         m_streams_orig;
      std::vector<HLTStreamTag*>                         m_streams;
      std::unordered_map<std::string, HLTStreamTag*>   m_streams_map;

      friend std::ostream & operator<<(std::ostream &, const HLTChain &);

   public:
      
      // temporary object to store the merge information
      struct {
         unsigned int l2;
         unsigned int ef;
      } mergeCounter;
   };

}

#endif
