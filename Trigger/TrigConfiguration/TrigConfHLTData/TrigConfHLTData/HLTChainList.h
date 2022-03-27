/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_HLTChainList
#define TrigConf_HLTChainList

#include <string>
#include <iosfwd>
#include <vector>

#include "TrigConfHLTData/HLTChain.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/composite_key.hpp>

/* @brief class to hold the chains
 *
 * The chains are owned by this collection
 */
namespace TrigConf {

   class HLTChainList;
   class CTPConfig;
   class HLTPrescaleSet;
   class DiffStruct;

   std::ostream & operator<<(std::ostream &, const TrigConf::HLTChainList &);

   // dummy classes to work as indeces for boost::multi_index structure
   struct name{};
   struct name_hash{};
   struct counter{};

   typedef boost::multi_index::multi_index_container<HLTChain*, boost::multi_index::indexed_by<
      boost::multi_index::ordered_unique< boost::multi_index::identity<HLTChain> >,
      boost::multi_index::ordered_unique< boost::multi_index::tag<counter>,
                                          boost::multi_index::composite_key< 
         HLTChain*,
         boost::multi_index::const_mem_fun<HLTChain,int,&HLTChain::chain_counter>,
         boost::multi_index::const_mem_fun<HLTChain,HLTLevel,&HLTChain::level_enum>
         >
      >,
      boost::multi_index::ordered_unique< boost::multi_index::tag<name>, boost::multi_index::const_mem_fun<HLTChain, const std::string&, &HLTChain::chain_name> >,
      boost::multi_index::hashed_unique< boost::multi_index::tag<name_hash>, boost::multi_index::const_mem_fun<HLTChain,const std::string&, &HLTChain::chain_name> >,
      boost::multi_index::random_access<>
      >
   > HLTChainContainer;


   /**@brief list of all HLT chains in a trigger menu*/
   class HLTChainList : public HLTChainContainer {
   public:
      
      /**@brief default constructor*/
      HLTChainList();	       

      /**@brief constructor with configuration data
       *
       * @param chainList list of all HLT chains
       */
      HLTChainList( const std::vector<HLTChain*>& chainList );	       

      /**@brief default destructor*/
      ~HLTChainList();

      /**@brief adds an HLTChain to the menu
       *
       * @param ch chain to be added
       */
      bool addHLTChain(HLTChain* ch);

      /**@brief access the chain by name
       * returns null-pointer if chain not found 
       */
      HLTChain* chain( const std::string& chainname ) const;

      /**@brief access the chain by level and counter
       *
       * returns null-pointer if chain not found 
       */
      HLTChain* chain( unsigned int counter, HLTLevel level ) const;

      /**@brief accessors to list of chains, for backward compatibility, use HLTChainList directly*/
      //std::vector<HLTChain*> chains() const;
      
      /**@brief print the chain list*/
      void print(const std::string& indent="", unsigned int detail=1) const;

      DiffStruct* compareTo(const HLTChainList* o) const;

      /**@brief set prescales of all chains*/
      void applyPrescaleSet(const HLTPrescaleSet* pss);

      /**@brief extract prescales from all chains*/
      HLTPrescaleSet* extractPrescaleSet() const;

      void setL2LowerChainCounter(const CTPConfig* ctpcfg);
 
      void setEFLowerChainCounter();

      // delete all chains and clear the container
      void clear();
     
      friend std::ostream & operator<<(std::ostream & o, const TrigConf::HLTChainList & c);
   };

}

#endif 
  
