/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <stdint.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <stdexcept>
#include <inttypes.h>
#include <sys/types.h>

namespace TrigConf {

   class TriggerThreshold;
   class TriggerItemNode;

   // like PERL split
   std::vector<std::string>
   split(const std::string& line, const std::string& del=" ");

   // like PERL strip
   void strip(std::string& str);

   void replaceTabs(std::string& str);

   void toLower(std::string&);

   void removeAllSpaces(std::string&);

   uint32_t bin2uint(const std::string& binary);
   
   std::string uint2bin(uint32_t uinteger, uint16_t width);



   // builds a tree of TriggerItemNode's and returns top node
   //
   // input is:
   //
   // logic string like "!(1&2)|3"
   TrigConf::TriggerItemNode*
   parse(const std::string& logic,
         const std::vector<std::string>& conditions,
         const std::vector<TrigConf::TriggerThreshold*>& thrs);

   // returns position of corresponding ')' parenthesis
   // 
   // opening parenthisis is at pos (or before)
   std::string::size_type
   findClosingBracket(std::string::size_type pos,
                      const std::string& logic);
   
   // builds an TriggerItemNode(OBJ), set's the TriggerThreshold, multiplicity, and InternalTrigger (if applicable)
   //
   // condition contains threshold name and multiplicity, or name of internal trigger
   // threshold is looked for in thrs vector
   TrigConf::TriggerItemNode*
   buildObjNode(uint32_t condIdx,
                const std::vector<std::string>& conditions, 
                const std::vector<TrigConf::TriggerThreshold*>& thrs);

   std::string insertParenthesis(const std::string& givenlogic);

   
}
