/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_HLTSequenceList
#define TrigConf_HLTSequenceList

#include <iosfwd>
#include <string>
#include <vector>
#include <map>

#include "TrigConfHLTData/HLTSequence.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>

namespace TrigConf {

   class HLTSequenceList;
   class DiffStruct;
   class HLTSequence;

   std::ostream & operator<<(std::ostream &, const HLTSequenceList &);

   struct seq_name_hash{};
   struct seq_hash_id{};

   typedef boost::multi_index::multi_index_container<HLTSequence*, boost::multi_index::indexed_by<
      boost::multi_index::ordered_unique< boost::multi_index::identity<HLTSequence> >,
      boost::multi_index::ordered_unique< boost::multi_index::tag<seq_hash_id>, boost::multi_index::const_mem_fun<HLTSequence, unsigned int, &HLTSequence::hashId> >,
      boost::multi_index::hashed_unique< boost::multi_index::tag<seq_name_hash>, boost::multi_index::const_mem_fun<TrigConfData, const std::string&, &TrigConfData::name> >
      >
   > HLTSequenceContainer;


   /**@brief list of HLT sequences*/
   class HLTSequenceList : public HLTSequenceContainer {
   public:

      /**@brief default constructor*/
      HLTSequenceList();
    
      /**@brief constructor with configuration data
       *
       * @param HLTSequenceList list of HLT sequences
       */
      HLTSequenceList( std::vector<HLTSequence*>& HLTSequenceList );

      /**@brief destructor*/
      ~HLTSequenceList();

      /**@brief adds an HLTSequence to the menu
       *
       * @param sequence sequence to be added
       */
      void addHLTSequence( HLTSequence * sequence);

      void clear();

//       std::vector<HLTSequence*>& sequences() { return m_HLTSequenceList; }             ///< accessor to the list of sequences
//       const std::vector<HLTSequence*>& sequences() const { return m_HLTSequenceList; } ///< const accessor to the list of sequences
      
      /**@brief print the sequence list*/
      void print(const std::string& indent="", unsigned int detail=1) const;

      bool hasTE(const std::string& name);

      /**@brief counts the number of sequences in the menu
       *
       * @returns the sequences with given output TE (0 if not found)
       */
      HLTSequence* getSequence(unsigned int id) const;
      HLTSequence* getSequence(const std::string& tename) const;

      /**@brief find name from ID in the map m_IdToLabel
       *
       * @param id trigger element ID
       * @param label reference to be set with the trigger element name that has been found
       * @returns @c true if ID has been found
       */
      bool getLabel(unsigned int hashId, std::string & label);

      /**@brief find ID from name in the map m_IdToLabel
       *
       * @param label the trigger element name that has been found
       * @param id reference to the trigger element ID that has been found
       * @returns @c true if name has been found
       */
      bool getHashIdFromLabel(const char * label, unsigned int & hashId);

      DiffStruct* compareTo(const HLTSequenceList* o) const;

   private:

      friend std::ostream & operator<<(std::ostream &, const HLTSequenceList &);
   };

}

#endif 
  
