/* emacs: this is -*- c++ -*- */
/**
 **
 **   @file         utils.h  
 **
 **                   
 **                   
 **                   
 ** 
 **   @author       M.Sutton  
 **
 **   @date         Thu Jun 23 01:53:36 BST 2005
 **
 **   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **                   
 **                   
 **
 **/


#ifndef READCARDS_UTILS_H
#define READCARDS_UTILS_H

#include <string>
#include <vector>
#include <iostream>


// get tokens off the front (chop) and back (chomp)  
std::string chop(std::string& , const std::string& );
std::string chomp(std::string& , const std::string& );

// chop token off of end of string, leave string unchaged, return the token 
std::string choptoken(std::string& , const std::string& );

// chop token off of end of string, leave string unchaged, return the token 
std::string chomptoken(std::string& , const std::string& );


// removes whitespace
void removespace(std::string& s, const std::string& s2=" \t");

// replaces whitespace
void replace(std::string& s, const std::string& s2=" \t", const std::string& s3="-");

// chops s1 to the first not of s2
std::string chopfirst(std::string& s1, const std::string& s2="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.:/");

// chops s1 from the the last not of s2
std::string choplast(std::string& s1, const std::string& s2="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");

// chops from both ends all of s2 
std::string chopends(std::string& s1, const std::string& s2=" \t");

// functions for manipulating filenames
std::string basename( std::string name );
std::string dirname( std::string name );

// removes colons
void depunctuate(std::string& ); 
  
// helper methods, calculate delta phi form two phi angles
double deltaPhi(double , double );

// checks to see if a particular file can be opened
bool canopen(const std::string& ); 


// converts number into integer string ...
std::string number(const int& i,    const std::string& s="%d");
std::string number(const double& d, const std::string& s="%lf");



#endif  /* READCARDS_UTILS_H */











