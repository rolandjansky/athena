/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/LogicParser.h"

#include <boost/tokenizer.hpp>

TrigConf::LogicParser::LogicParser()
{}

std::shared_ptr<TrigConf::Logic>
TrigConf::LogicParser::parse(const std::string & expr) {
   std::unique_ptr<TrigConf::Logic> node( nullptr );
   try {
      auto exprTok = tokenize(expr);
      node = buildTree(exprTok);
      node->setExpression(expr);
   } catch (LogicParsingException & ex) {
      std::cerr << "Error in expression " << expr << ":" << ex.msg() << std::endl;
      throw;
      return nullptr;
   }
   return std::shared_ptr<TrigConf::Logic>(std::move(node));
}

std::vector<std::string>
TrigConf::LogicParser::tokenize(const std::string & expr) const {
   std::vector<std::string> tokens;
   // build tokens with separators ()&|! and <space>. Keeps all separators except <space> in the list of tokens 
   for ( auto & tok : boost::tokenizer<boost::char_separator<char> > (expr, boost::char_separator<char>(" ", "()&|!")) ) {
      tokens.emplace_back(tok);
   }
   return tokens;
}



std::unique_ptr<TrigConf::Logic>
TrigConf::LogicParser::buildTree(const std::vector<std::string> & tokExpr) const {

   size_t front = 0;
   size_t back = tokExpr.size()-1;
   auto logic = buildNode(tokExpr,front,back);
   return logic;
}


std::unique_ptr<TrigConf::Logic>
TrigConf::LogicParser::buildNode(const std::vector<std::string> & tokExpr, size_t front, size_t back) const {

   auto logicLeft = findSubExpr(tokExpr,front, back);

   if(front >= back) {
      return logicLeft;
   }

   std::string token = tokExpr[front];
   if( token!="&" && token != "|" ) {
      throw LogicParsingException(std::string("Did expect a & or | here, but got a ") + token + ".");
   }
   front++;

   LogicOPS * logic = token=="&" ? static_cast<LogicOPS*>(new LogicAND(std::move(logicLeft))) : static_cast<LogicOPS*>(new LogicOR(std::move(logicLeft)));

   auto logicRight = buildNode(tokExpr, front, back);

   if(logicRight->nodeType() == logic->nodeType()) {
      auto subLogics = static_cast<LogicOPS*>(logicRight.get())->takeSubLogics();
      for(auto && sublogic : std::move(subLogics)) {
         logic->addSubLogic(std::move(sublogic));
      }
   } else {
      logic->addSubLogic(std::move(logicRight));
   }

   return std::unique_ptr<TrigConf::Logic>( logic );
}



/**
 *
 * @param front location where the subexpr is to be looked for. @c
 * front will point to the next position to parse from at the end of the function
 */
std::unique_ptr<TrigConf::Logic>
TrigConf::LogicParser::findSubExpr(const std::vector<std::string> & tokExpr, size_t & front, const size_t back) const {

   std::string token = tokExpr[front];

   auto logic = std::unique_ptr<TrigConf::Logic>( nullptr );

   if(token=="!") {
      front++; // to eat the '!'
      token = tokExpr[front];
      if(token == "(") {
         logic = findSubExpr(tokExpr,front, back);
      } else {
         front++; // eat this token
         logic = std::unique_ptr<TrigConf::Logic>(new LogicLeaf(token));
      }
      logic->setNegate();
   } else if(token=="(") {
      size_t parEnd = front;
      while(tokExpr[++parEnd] != ")");
      logic = buildNode(tokExpr,front+1,parEnd-1);
      front = parEnd + 1;
   } else {
      logic = std::unique_ptr<TrigConf::Logic>(new LogicLeaf(token));
      ++front;
   }
   return logic;
}

