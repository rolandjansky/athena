
/*
 *  SCT_CablingUtilities.cxx
 *  SCT_Cabling
 *
 *  Created by sroe on 14/11/2008.
 *  Copyright 2008 CERN. All rights reserved.
 *
 */
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib> // to include 'getenv'

namespace SCT_Cabling{
  
  ///Convert a string (decimal or hex) to an int; -1 indicates an error
  int stringToInt(const std::string &hexOrDecString){
    std::istringstream istrm(hexOrDecString);
    int result(-1);
    //enable exceptions for a badly formatted string
    istrm.exceptions(std::ios_base::badbit|std::ios_base::failbit);
    //do conversion if the string is not empty
    if (not hexOrDecString.empty()){
      try{
        bool numberIsHex = (hexOrDecString.find("x") not_eq std::string::npos) or (hexOrDecString.find("X") not_eq std::string::npos);
        if(numberIsHex){
          istrm>>std::hex>>result;
        } else {
          istrm>>std::dec>>result;
        }
      } catch (const std::ios_base::failure&){ //bad conversion to int
        result=-1;
        //need to throw here
        std::cerr<<"stringToInt failed to convert a string to an integer in InDetCabling/SCT_CablingUtilities"<<std::endl;
        throw(std::ios_base::failure("stringToInt failure in SCT_CablingUtilities"));
      }
    }
    return result;
  }
  
  ///Check range and give error if out of range
  bool inRange(const int value, const int lowerBound, const int upperBound, const std::string & valueName){
    bool bad= ( (value > upperBound) or (value<lowerBound) );
    if (bad){
      std::string name = valueName.empty()?"The value":valueName;
      std::cerr<<name<<" = "<<value<<" is out of the allowable range."<<std::endl;
    }
    return (not bad);
  } 
  
  ///calculate link, normal and swapped (from Kondo)
  int calculateLink(const int MURorder, const int ModID, const int theSide,const bool isSwapped = false){
    return isSwapped?(MURorder*12 + (5 - ModID) * 2 + (theSide)):(MURorder*12 + ModID * 2 + theSide);
  }
  
  //End of utility functions
  
  namespace CoveritySafe{
    ///get an environment variable
    std::string getenv(const std::string & variableName){
      std::string result("");
      const char * pChar=std::getenv(variableName.c_str());
      if (pChar) result=pChar;
      return result;
    } 
  }
}


