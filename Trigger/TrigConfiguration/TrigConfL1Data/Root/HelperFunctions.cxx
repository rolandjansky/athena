/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <iostream>
#include <sstream>
#include <cassert>
#include <cstdlib>

#include "TrigConfL1Data/HelperFunctions.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/TriggerItemNode.h"
#include "TrigConfL1Data/TriggerThreshold.h"

#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"

using namespace std;
using namespace TrigConf;



std::vector<std::string>
TrigConf::split(const std::string& line, const std::string& del) {
   std::vector<std::string> res;
   boost::split(res, line, boost::is_any_of(del));
   return res;
}

// helper method: removing all spaces at beginning and end of a string
void
TrigConf::strip(std::string& str) {
  std::string::size_type pos = str.find_last_not_of(' ');
  if(pos != std::string::npos) {
    str.erase(pos + 1);
    pos = str.find_first_not_of(' ');
    if(pos != std::string::npos) str.erase(0, pos);
  }
  else str.erase(str.begin(), str.end());
}

// helper method: remove all spaces in string
void TrigConf::removeAllSpaces(std::string& str) {
   std::string::size_type pos;
   while ((pos = str.find_first_of(' ')) != std::string::npos) {
      str.erase(pos,1);
   }
}


// helper method: replace tabs by single space
void TrigConf::replaceTabs(std::string& str) {
  std::string::size_type old_pos = 0, pos = str.find('\t',0);
  while ( pos != std::string::npos) {
    str.replace(pos,1," ");
    old_pos = pos;
    pos = str.find('\t',old_pos);
  }
}


// helper function: all chars to lower chars
void
TrigConf::toLower(std::string& s) {
  for(size_t i=0; i<s.size(); i++) s[i] += (s[i]>='A'&&s[i]<='Z')?'a'-'A':0;
}



namespace {

   TrigConf::TriggerItemNode *
   parseToken(const std::string& givenlogic,
              std::string::size_type& pos,
              const std::vector<std::string>& conditions,
              const std::vector<TrigConf::TriggerThreshold*>& thrs);


   TrigConf::TriggerItemNode *
   parseExpr(const std::string& expr,
             const std::vector<std::string>& conditions,
             const std::vector<TrigConf::TriggerThreshold*>& thrs)
   {


      std::string::size_type pos  = 0;
      std::string::size_type last = expr.size();

      TrigConf::TriggerItemNode * newNode = 0;

      while(pos!=last) {

         TrigConf::TriggerItemNode * nodeForToken = parseToken(expr, pos, conditions, thrs);

         // deal with the end of the expression
         if(pos == last) {
            if(newNode==0)
               return nodeForToken; // single token
            newNode->addChild(nodeForToken);
            return newNode;
         }


         // determine the &/| type from the next character
         TriggerItemNode::NodeType typeFromChar;
         if(expr[pos]=='&' || expr[pos]=='|') {
            typeFromChar = TriggerItemNode::typeFromChar(expr[pos]);
         } else {
            throw std::logic_error(string("Unexpected character '") + expr[pos] + "' in expression '" + expr
                                   + "' at position" + boost::lexical_cast<std::string,int>(pos) + " [b]");
         }


         if(newNode==0) {
            // create Node upon first encounter of & or |
            newNode = new TriggerItemNode(typeFromChar);
         } else {
            // check that current character matches the type of the node
            if(newNode->type() != typeFromChar) {
               throw std::logic_error(string("Unexpected character '") + expr[pos] + "' in expression '" + expr 
                                      + "' at position" + boost::lexical_cast<std::string,int>(pos) + " [c] Expected "
                                      + TriggerItemNode::typeAsString(newNode->type()) );
            }
         }

         newNode->addChild(nodeForToken);
         pos++; // move beyond the &/|
      }
      
      return newNode;
   }

   std::string getSubExpr(const std::string &exp, const std::string::size_type begin) {
      std::string::size_type pos = begin;
      std::string::size_type last = exp.size();

      uint32_t openBrackets = 0;
      while(pos!=last) {
         char cc = exp[pos];
         if(cc=='(') openBrackets++;
         if(cc==')') openBrackets--;
         if(openBrackets==0 && (cc=='&' or cc=='|') ) break;
         pos++;
      }
      if (openBrackets>0) {
         std::cout << "ERROR: No matching closing bracket in '" << exp << "'" << std::endl;
         assert(0);
      }
      std::string se(exp, begin, pos-begin);
      // std::cout << "Subexp of '" << exp << "'[" << begin<<"] returns " << se << std::endl;
      return se;
   }


   TrigConf::TriggerItemNode *
   parseToken(const std::string& logic,
              std::string::size_type& pos,
              const std::vector<std::string>& conditions,
              const std::vector<TrigConf::TriggerThreshold*>& thrs) {

      // logic should be the definition of a node
      //
      // this means it needs to start with a number, '(', or '!' 

      //std::cout << "parse  '" << givenlogic << "'" << std::endl;
      //for(uint32_t i =0; i<conditions.size(); i++) std::cout << conditions[i] << std::endl;
      //std::cout << thrs.size() << ")"  << std::endl;


      //std::string::size_type last = logic.size();

      TriggerItemNode* thisNode(0);
      // tokenize

      // need three tokens to build a TriggerItemNode
      char cur = logic[pos];
      switch(cur) {
      case '!':
         {
            pos++;
            thisNode = new TriggerItemNode(TrigConf::TriggerItemNode::NOT);
            std::string se = getSubExpr(logic,pos);
            thisNode->addChild( parse(se,conditions,thrs) );
            pos += se.size();
            break;
         }
      case '(':
         {
            std::string se = getSubExpr(logic,pos);
            pos += se.size();
            // should have outer parentheses (...)
            std::string senop(se, 1, se.size()-2);         
            thisNode = parse(senop,conditions,thrs);
            break;
         }
      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
         {
            std::string se = getSubExpr(logic,pos);
            pos += se.size();
            uint32_t condIdx = boost::lexical_cast<uint32_t, std::string>(se);
            thisNode = buildObjNode(condIdx, conditions, thrs);
            break;
         }
      default:
         {
            std::string errMsg = "Unexpected character '";
            errMsg += logic[pos];
            errMsg += "' in expression '" + logic + "' at position" + boost::lexical_cast<std::string,int>(pos) + " [a]";
            throw std::logic_error(errMsg);
         }
         break;
      }


      return thisNode;
   }
}



TrigConf::TriggerItemNode *
TrigConf::buildObjNode(uint32_t condIdx,
                       const vector<string>& conditions, 
                       const vector<TrigConf::TriggerThreshold*>& thrs) {

   TriggerItemNode* newNode = new TriggerItemNode(TriggerItemNode::OBJ);
   
   try {
      vector<string> condDef = split(conditions[condIdx-1], ",");

      if(condDef.size()==1) { // internal trigger (condDef contains internal trigger name)

         newNode->setInternalTrigger( condDef[0] );

      } else { // threshold (condDef contains multiplicity, condName, and threshold name)
         // set multiplicity
         newNode->setMultiplicity(boost::lexical_cast<int,std::string>(condDef[0]));
         // find trigger threshold in list of all thresholds and set it in the TriggerItemNode
         std::string& name = condDef[2];
         if(thrs.size()>0) {
            std::vector<TrigConf::TriggerThreshold*>::const_iterator thrIt = thrs.begin();
            for(;thrIt!=thrs.end(); thrIt++ ) {
               if((*thrIt)->name()==name) {
                  newNode->setTriggerThreshold(*thrIt);
                  break;
               }
            }
         } else {
            newNode->setThresholdName(name);
         }
      }
   }
   catch(const std::exception& e) {
      std::cout << "Exeption caught in buildObjNode for " << conditions[condIdx-1] << ": " << e.what() << std::endl;
      throw;
   }
   return newNode;
}


TrigConf::TriggerItemNode *
TrigConf::parse(const std::string& givenlogic,
                const std::vector<std::string>& conditions,
                const std::vector<TrigConf::TriggerThreshold*>& thrs) {

   // balance expression
   std::string expr = insertParenthesis(givenlogic);

   // parse
   TriggerItemNode* topNode = parseExpr(expr, conditions, thrs);

   // wrap in an AND
   if( topNode->type() != TriggerItemNode::AND) {
      TriggerItemNode * newNode = new TriggerItemNode(TriggerItemNode::AND);
      newNode->addChild(topNode);
      topNode = newNode;
   }

   return topNode;
}






// pos: position of the current opening bracket
std::string::size_type
TrigConf::findClosingBracket(std::string::size_type pos,
                             const std::string& logic) {

   pos++;
   uint32_t openBrackets = 1;
   std::string::size_type last = logic.size();
   while(openBrackets>0 && pos!=last) {
      if(logic[pos]==')') openBrackets--;
      if(logic[pos]=='(') openBrackets++;
      pos++;
   }
   if (openBrackets>0) {
      std::string errMsg = "No matching closing bracket in '";
      errMsg += logic;
      errMsg += "'";
      throw std::logic_error(errMsg);
   }
   return pos-1;
}

std::string
TrigConf::insertParenthesis(const std::string& givenlogic) {
   std::string logicWithPars(givenlogic);
   size_t last = givenlogic.size()-1;   
   bool leadingAnd(false);
   for(size_t pos = 0;pos<last;pos++) {
      char c = logicWithPars[pos];
      if(c=='(') { pos=findClosingBracket(pos,givenlogic); continue; }
      if(c=='&') leadingAnd=true;
      if(c=='|') {
         if(leadingAnd) {
            logicWithPars.insert(pos,")");
            logicWithPars.insert(0,"(");
         }
         break;
      }
   }
   return logicWithPars;
}



uint32_t
TrigConf::bin2uint(const std::string& binary) {
   uint32_t value(0);
   for(char c: binary) {
      value <<= 1;
      if(c=='1') value += 1; 
   }
   return value;
}


std::string
TrigConf::uint2bin(uint32_t value, uint16_t width) {
   stringstream ss;
   for(uint32_t mask = 1 << (width-1); mask>0; mask>>=1)
      ss << ( (value & mask) == 0 ? '0' : '1' );

   return ss.str();
}
