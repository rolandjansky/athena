/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "common_fcn.h"

#include <cstdlib>
#include <cctype>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;


// list of argument passed at the begin
vector<string> __cline_argv;


/** initialize the environment */
void common_fcn_init(int argc,char *argv[])
{
  for (int i=0;i!=argc;++i)
    __cline_argv.push_back(argv[i]);
}

/** this function replace the sub-string "tmpl" in "orstring" with
    the value "val". "tmpl" content is not parsed but simply replaced. the
    "orstring" is modified by the function This is different from replace_ids
    in that, it will search with the tag prefix'ed. (e.g. search and replace
    <tag><tmpl> with <tag><val>) */
void replace_ids_with_tag(string &orstring, const string &tmpl, string &tag, int val)
{

  unsigned int spos = orstring.rfind(tag+tmpl);
  if (spos!=string::npos) {
    ostringstream vstr;
    vstr << val;
    orstring.replace(spos,tmpl.size()+tag.size(),tag+vstr.str());
  }
}


/** this function replace the sub-string "tmpl" in "orstring" with
    the value "val". "tmpl" content is not parsed but simply
    replaced. the "orstring" is modified by the function */
void replace_ids(string &orstring, const string &tmpl, int val)
{

  unsigned int spos = orstring.rfind(tmpl);
  if (spos!=string::npos) {
    ostringstream vstr;
    vstr << val;
    orstring.replace(spos,tmpl.size(),vstr.str());
  }
}


/** This function looks for sub-string like $N, where N is a number.
    This is replaced by the command like argument in the Nth position. 
    The original string is changed by the method. */
void replace_args(string &orstring)
{
  unsigned int spos(0);

  do {
    int loc = orstring.find('$',spos);
    if (loc==string::npos) break;

    // scan the following characters to see there the sub-string ends
    unsigned int slen = orstring.size();
    unsigned int ic=loc+1; // start from the nex char
    for (;ic!=slen;++ic) {
      // false if the char is not a digit
      if (!isdigit(orstring[ic])) break;
    }
    
    // numeric position len
    if (ic-loc>1) {
      // get the $N numeric values
      unsigned int nval = atoi(orstring.substr(loc+1,ic-1).c_str());
      // replace the variable with the command line argument
      if (nval<__cline_argv.size() && nval>=0) {
	orstring.replace(loc,ic-loc,__cline_argv[nval]);
      }
      else {
	cerr << "!!! requested command line argument (" << nval << ") is out of range" << endl;
      }
    }

    // move spos after the variable
    spos = ic;
  } while(spos<orstring.size());
    
}


istream& replace_getline(istream &sin, string &line)
{
  istream &res = getline(sin,line);
  replace_args(line);

  return res;
}
