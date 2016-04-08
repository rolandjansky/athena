/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SerialNumber_h
#define SCT_SerialNumber_h
/**
 * @file SCT_SerialNumber.h
 * Header file for a utility class to hold the serial number
 **/

#include <string>
//#include <ostream>

/**
 * SCT_SerialNumber is a class to hold a serial number and 
 * provide check on validity, and conversions between the different formats.
 * In general, an SCT serial number is a unique 14 digit code assigned to each module
 * and found on the module as a bar code. It always starts with 20220
 **/

class SCT_SerialNumber{
public:
  ///Default constructor produces an invalid serial number
  SCT_SerialNumber();
  
  /**
   *  Construct from string of the full number
   **/
  SCT_SerialNumber(const std::string & snString);
  
  /** 
   * Construct from full number (unsigned), which has to be a long long 
   **/
  SCT_SerialNumber(const unsigned long long fullSerialNumber);
  
  /** 
   * Construct from full number (signed), which has to be a long long 
   **/
  SCT_SerialNumber(const long long fullSerialNumber);
  
  
  /** 
   * Construct from unsigned int, which can only hold the truncated serial number 
   **/
  SCT_SerialNumber(const unsigned int truncatedSerialNumber);
  
  /** 
   * Construct from int, which can only hold the truncated serial number 
   **/
  SCT_SerialNumber(const int truncatedSerialNumber);
  
  //@name conversions to other forms
  //@{
  ///Full serial number as a string  
  std::string str() const;  
  ///truncated serial number as unsigned int
  unsigned int to_uint() const;
  ///full serial number as long long
  unsigned long long to_ulonglong() const;
  ///overload cast to uint
  operator unsigned int() const;
  //@}
  
  
  /** Cursory check on whether the serial number is well formed
   * N.B. this is deliberately not an 'is_valid' method since a check on validity would require 
   * more comprehensive validation.
   **/
  bool isWellFormed() const;
  
  ///until I make a better test, is_valid returns only whether its well formed
  bool is_valid() const{ return isWellFormed(); }
  
private:
  /// The truncated number holds the integer part after the '20220' of the serial number
  unsigned int m_truncatedNumber;
};

#endif


