/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONF_LOGICPARSER_H
#define TRIGCONF_LOGICPARSER_H

#include "TrigConfData/Logic.h"

#include <memory>

namespace TrigConf {

   /** @brief turns a logical expression into a structure that can be evaluated 
    * 
    * The logical expression can consist of word-tokens and ! & | ( ) and <space>
    * The word-tokens can contain letters and digits, the can be followed by an "[<number>]"
    *
    * The usage is that the parser is presented with a logic-expression, it returns a shared_ptr<Logic>
    * That Logic evaluate(state) function can be given a map of word-tokens mapped to true or false:
    @code
    TrigConf::LogicParser p;
    
    std::map<std::string,bool> state {
       {"EM3", true},
       {"MU6", false},
       {"MU20", true}
    };

    auto logic = p.parse("EM3 & !(MU6|MU20)");
    bool passed = logic->evaluate(state);  // false

    p.parse("!EM3 | (MU6 & MU20)")->evaluate(state);  // false
    @code

    * It is also possible to evaluate expressions that require a certain minimum multiplicity for each word-token
    * In that case a map of word-tokens mapped to an integer has to be presented to the Logic::evaluate function
    @code
    std::map<std::string,unsigned int> multiplicity {
      {"EM3", 2},
      {"XE30", 1},
      {"XE50", 0},
      {"MU20", 1},
      {"RNDM0", 1},
      {"EM18VHI", 1},
      {"TOPO_300INVM9999-AJ30s6-AJ20s6", 1},
    };

    auto logic = p.parse("MU20[x1]&XE30[x1]");
    bool passed = logic->evaluate(state);  // true
    @code
    *
    * This second use case is to evaluation the trigger items in terms
    * of their threshold multiplicity requirements
    */


   class LogicParser {
   public:
      LogicParser();

      std::shared_ptr<TrigConf::Logic> parse(const std::string &);

   private:
      std::vector<std::string> tokenize(const std::string & expr) const;

      std::unique_ptr<TrigConf::Logic> buildTree(const std::vector<std::string> & tokExpr) const;

      std::unique_ptr<TrigConf::Logic> buildNode(const std::vector<std::string> & tokExpr, size_t front, size_t back) const;

      std::unique_ptr<TrigConf::Logic> findSubExpr(const std::vector<std::string> & tokExpr, size_t & front, size_t back) const;

   };

}

#endif
