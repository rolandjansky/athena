/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_HLTUtils
#define TrigConf_HLTUtils

#include <string>
#include <inttypes.h>
#include <set>
#include <vector>
#include <mutex>

namespace TrigConf {

   class HLTChainList;
   class HLTChain;
   class HLTSequenceList;
   class HLTSequence;
   class HLTPrescale;
   class HLTFrame;

   typedef uint32_t HLTHash; 
  
   class HLTUtils {

   public:

      /**@brief hash function translating TE names into identifiers*/
      static HLTHash string2hash( const std::string&, const std::string& category="TE" );
      /**@brief hash function translating identifiers into names (via internal dictionary)*/
      static const std::string hash2string( HLTHash, const std::string& category="TE" );
      /**@brief debugging output of internal dictionary*/
      static void hashes2file( const std::string& fileName="hashes2string.txt" );
      /**@brief debugging output of internal dictionary*/
      static void file2hashes( const std::string& fileName="hashes2string.txt" );
      /**@brief Multi-threaded protection for the static hash operations*/
      static std::mutex s_mutex;
      /**@brief In-file identifier*/
      static std::string s_newCategory;


    
      /**
       * @brief returns list of TEids which can be poduced at given level (L2 or EF) in currecnt configuration
       * @warning this method is dooing quite extensive menu inspection so it should not be executed to often ...
       * 
       * @param level L2 or EF name of trigger level
       * @param chainsList list of configuration chains
       * @param sequenceList list of configuration sequences
       * @return list of TE ids which according to the given configuration will be produced at level 
       */    
      static std::vector<unsigned int> allTEsProducedInLevel( const std::string& level, 
                                                              const  HLTChainList& chainList,  const HLTSequenceList& sequenceList);

      /**
       * @brief returns list of TEids which are specified by Chain signatures
       */

      static std::vector<std::string> explicitChainTEs(const HLTChain& ch);



      /**
       * @brief returns list of TEids which are specified by Chain signatures and which are needed by this signatures
       */
      static std::vector<std::string> implicitChainTEs(const HLTChain& ch, const HLTSequenceList& sequenceList);



      /**
       * @brief returns set of input TEs for given set of TEs
       */
      static std::set<std::string> inputTEs( const std::set<std::string>& TEs,
                                             const TrigConf::HLTSequenceList& sequenceList);
    

      static std::set<std::string> allTEsProducedInL2Chain( const TrigConf::HLTChain& chain,
                                                            const TrigConf::HLTSequenceList& sequenceList);
    
      static std::set<std::string> allTEsProducedInEFChain( const TrigConf::HLTChain& chain,
                                                            const TrigConf::HLTSequenceList& sequenceList,
                                                            const std::set<std::string>& l2tes );

      static std::set<std::string> allTEsProducedInL2( const TrigConf::HLTFrame& frame);

      static std::set<std::string> allTEsProducedInEF( const TrigConf::HLTFrame& frame,
                                                       std::set<std::string>* l2tes=0 );
    
      static std::set<std::string> allTEsProduced( const TrigConf::HLTFrame& frame );
    
      static std::string ChainCondenseDisplay( const HLTChain& chain, 
                                               const HLTFrame& frame );

      static std::vector< std::string > splitGroups(const std::string& s);

      static HLTSequence* buildSequence(const std::string& desc);

      static void mergeHLTChainList(  TrigConf::HLTFrame& frame );

      static void mergeHLTChainList2( TrigConf::HLTFrame& frame );

      static void mergeL2EFPrescales( TrigConf::HLTChain* hltchain, 
                                      const TrigConf::HLTPrescale& l2ps, 
                                      const TrigConf::HLTPrescale& efps);
      
   };
   
}

#endif
