/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CollectionUtilities/UtilityFuncs.h
 * @brief prototypes for utility POOL collection funcs
 * @author Jack.Cranshaw@cern.ch
 * $Id: UtilityFuncs.h 495625 2012-04-13 13:54:17Z mnowak $ 
 */
#include "CollectionBase/ICollectionDescription.h"

#include <string>
#include <map>
#include <set>
#include <vector>

namespace pool {

   template <class KEY, class PAYLOAD>
   std::vector<PAYLOAD> vectorizeMapPayload(const std::map<KEY,PAYLOAD> m)
   {
      std::vector<PAYLOAD> outputs;
      for (typename std::map<KEY,PAYLOAD>::const_iterator j = m.begin(); j != m.end(); ++j) {
         outputs.push_back(j->second);
      }
      return outputs;
   }

   // Funcs for manipulating CollectionMetadata
   std::string getKey(const std::string key, const std::string encoded);
   std::string getGroup(const std::string encoded);
   std::set<std::string> findGroups(const pool::ICollectionDescription& desc);

   // Funcs for manipulating CollectionDescriptions
   void dumpAttributes(const pool::ICollectionDescription& description);
   void dumpTokens(const pool::ICollectionDescription& description);
   void dumpGroups(const pool::ICollectionDescription& description);
   void dumpOverlapEval(const pool::ICollectionDescription& desc0,
                        const pool::ICollectionDescription& desc1,
                        const std::vector< std::pair<std::string,std::string> >& spec0,
                        const std::vector< std::pair<std::string,std::string> >& spec1,
                        const std::string spectype);

   // Formatting
   std::pair<unsigned int,unsigned int>
   getColumnWidths(const pool::ICollectionDescription& description, bool tokens);

   void Tokenize(const std::string& str,
                 std::vector<std::string>& tokens,
                 const std::string& delimiters = " ");

}

