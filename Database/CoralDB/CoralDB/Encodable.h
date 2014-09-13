/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Define a type for strings that need special handling in
// dump/restore.  E.g. dump file format is line-oriented,
// tab-separated.  So tabs and newlines can not be dumped literally
//
// A.Gaponenko, 2007

#ifndef ENCODABLE_H
#define ENCODABLE_H

#include <string>
#include <iostream>

namespace CoralDB {
  
  class Encodable : public std::string {
  public: 
    Encodable() {}
    Encodable(const std::string& s) : std::string(s) {}
  };
  
  // Encodes data into the stream
  std::ostream& operator<<(std::ostream& os, const Encodable& s);
  
  // Reads the stream until (and including) the first newline and decodes it into s.
  // Sets badbit on invalid inputs.
  std::istream& operator>>(std::istream& is, Encodable& s);
  
}

#endif/*ENCODABLE_H*/
  
