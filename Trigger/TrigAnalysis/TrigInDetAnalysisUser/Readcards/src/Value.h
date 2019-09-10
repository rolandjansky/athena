/* emacs: this is -*- c++ -*- */
/**
 **
 **   @file         Value.h  
 **
 **                   
 **                   
 **                   
 ** 
 **   @author       M.Sutton  
 **
 **   @date         Wed May  4 11:14:42 BST 2005
 **
 **   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **                   
 **                   
 **
 **/


#ifndef READCARDS_VALUE_H
#define READCARDS_VALUE_H


#include <string>
#include <vector>
#include <iostream>



// using namespace std;

/** tag-value pair class.
 ** contains a string tag and a "value", which can be a 
 ** single value, or a vector
 **
 **/

class Value {
  
public:
  
  Value()                           : mTag(""), mVal(0) { } 
  Value(char* s,  std::vector<std::string> v) : mTag(s),  mVal(v) { } 
  Value(char* s,  std::string v)              : mTag(s),  mVal(0) { mVal.push_back(v); } 
  Value(std::string s, std::vector<std::string> v) : mTag(s),  mVal(v) { } 
  Value(std::string s, std::string v)              : mTag(s),  mVal(0) { mVal.push_back(v); } 
  
  const std::string&              Tag() const {  return mTag;  }
  const std::vector<std::string>&  Val() const {  return mVal;  }
  
private:
  
  std::string              mTag;
  //  std::vector<double>  mVal;
  std::vector<std::string> mVal;

};


std::ostream& operator<<(std::ostream& s, const Value& v);


#endif  /* READCARDS_VALUE_H */










