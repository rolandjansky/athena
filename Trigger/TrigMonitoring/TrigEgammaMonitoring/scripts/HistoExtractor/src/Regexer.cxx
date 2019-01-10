/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "Regexer.h"

#include <iostream>
#include<boost/algorithm/string.hpp>

Regexer::Regexer(std::string incl, std::string excl)
{
  doIncl = doExcl = false;
  if(incl!=""){
    doIncl = true;
    expr_incl = boost::regex(incl.c_str());
  }
  if(excl!=""){
    doIncl = true;
    expr_excl = boost::regex(excl.c_str());
  }

}

Regexer::~Regexer(void)
{
}

vector<string> Regexer::SplitPathIntoDirs(string path){

  vector<string> dirs;
  boost::split(dirs, path, boost::is_any_of("/"));
  return(dirs);


}

bool Regexer::Match(std::string input){
 
  //skip if we don't have anything to check
  if(!doIncl && !doExcl){ return(true); }

  bool result = true;
  std::string::const_iterator begin = input.begin();
  std::string::const_iterator end = input.end();
  boost::match_results<std::string::const_iterator> match;
  if(doIncl){ result &= regex_search(begin,end,match,expr_incl); }
  if(doExcl){ result &= !(regex_search(begin,end,match,expr_excl)); }
  return(result);
}

std::string Regexer::ExtractRunNumber(std::string input){
  std::string runnumber = "";
  //set up regular expression search (digits enclosed by dots)
  std::string::const_iterator begin = input.begin();
  std::string::const_iterator end = input.end();
  //regex: dot followed by (as many as possible) zeros , number sequence (at least 6 digits) , trailing dot
  boost::regex expression("(\\.[0]*)([0-9]*)(\\.)");
  boost::match_results<std::string::const_iterator> match;
  bool success = regex_search(begin, end, match, expression, boost::match_default);
  if(success){ runnumber = match[2]; }
  return(runnumber);
}

