
/**
 **
 **   @file         utils.cxx        
 **
 **                   
 **                   
 **                   
 ** 
 **   @author       M.Sutton    
 **
 **   @date         Thu Jun 23 01:11:43 BST 2005
 **
 **   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **                   
 **                   
 **
 **/



#include <string>
#include <vector>
#include <iostream>
//#include <math.h>
#include <cmath>

#include <stdio.h>

#include "utils.h"

// chop tokens off the end of a string, leave string unchanged
// return choped string
std::string choptoken(std::string& s1, const std::string& s2)
{
  std::string s3 = "";
  std::string::size_type pos = s1.find(s2);
  if ( pos != std::string::npos ) {
    s3 = s1.substr(0, pos+1); 
    s1.erase(0, pos+1);
  }
  return s3;
} 

// chop tokens off the end of a string, leave string unchanged
// return choped string
std::string chomptoken(std::string& s1, const std::string& s2)
{
  std::string s3 = "";
  std::string::size_type pos = s1.find_last_of(s2);
  if ( pos != std::string::npos ) {
    s3 = s1.substr(pos, s1.size());
    s1.erase(pos, s1.size()); 
  }
  return s3;
} 



std::string chopfirst(std::string& s1, const std::string& s2)
{
  std::string s3;
  std::string::size_type pos = s1.find_first_not_of(s2);
  if ( pos != std::string::npos ) {
    s3 = s1.substr(0, pos); 
    s1.erase(0, pos);
  }
  else {
    s3 = s1;
    s1 = "";
  } 
  return s3;
} 

// chop tokens off the front of a string
std::string chopends(std::string& s1, const std::string& s2)
{
  chopfirst(s1, s2);
  choplast(s1, s2);
  return s1;
} 



std::string choplast(std::string& s1, const std::string& s2)
{
  std::string s3 = "";
  std::string::size_type pos = s1.find_last_not_of(s2);
  if ( pos != std::string::npos ) {
    s3 = s1.substr(pos+1, s1.size());
    s1.erase(pos+1, s1.size());
  }
  return s3;
} 


// chop tokens off the front of a string
std::string chop(std::string& s1, const std::string& s2)
{
  std::string::size_type pos = s1.find(s2);
  std::string s3;
  if ( pos == std::string::npos ) {
    s3 = s1;
    s1.erase(0, s1.size());
  }
  else {
    s3 = s1.substr(0, pos); 
    s1.erase(0, pos+s2.size());
  }
  return s3;
} 


// chomp them off the end
std::string chomp(std::string& s1, const std::string& s2)
{
  std::string::size_type pos = s1.find(s2);
  std::string s3;
  if ( pos == std::string::npos ) {
    s3 = s1;
    s1.erase(0,s1.size());    
  }
  else {
    s3 = s1.substr(pos+s2.size(),s1.size());
    s1.erase(pos,s1.size()); 
  } 
  return s3;
} 


// remove strings from a string
void removespace(std::string& s, const std::string& s2) 
{
  std::string::size_type pos;
  while ( (pos = s.find(s2))!=std::string::npos ) {
    s.erase(pos, 1);
  }
} 


// replace from a string
void replace(std::string& s, const std::string& s2, const std::string& s3) 
{
  std::string::size_type pos;
  //  while ( (pos = s.find(" "))!=std::string::npos ) {
  //    s.replace(pos, 1, "-");
  while ( (pos = s.find(s2))!=std::string::npos ) {
    s.replace(pos, 1, s3);
  }
} 


// remove colons from a string
void depunctuate(std::string& s) 
{
  std::string::size_type pos;
  while ( (pos = s.find(":"))!=std::string::npos ) {
    s.erase(pos, 1);
  }
} 
  



// helper method to get difference in phi 
double deltaPhi(double phi1, double phi2)  {
  double delta = fabs(phi1-phi2);
  delta = (delta > M_PI) ? (2.0*M_PI - delta) : delta; 
  return delta;
}



// checks to see if a particular file can be opened
bool canopen(const std::string& s) { 
  FILE* f = fopen(s.c_str(), "r");
  if ( f ) { fclose(f); return true; }
  return false;
}


// converts number into integer string ...
std::string number(const double& d, const std::string& s) { 
  char tmp[512];
  sprintf(tmp, s.c_str(), d);
  return tmp;
}

std::string number(const int& i, const std::string& s) {
  char tmp[512];
  sprintf(tmp, s.c_str(), i);
  return tmp;
} 



std::string dirname( std::string name ) { 
  std::string::size_type pos = name.find_last_of( "/" );
  if ( pos!=std::string::npos ) name = name.substr( 0, pos );
  return name;
}


std::string basename( std::string name ) { 
  std::string::size_type  pos = name.find( "/" );
  while ( pos!=std::string::npos ) {
    name = name.substr( pos+1, name.size()-pos-1 );
    pos  = name.find( "/" );
  } 
  return name;
}
