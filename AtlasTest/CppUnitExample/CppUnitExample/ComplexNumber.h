/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CPPUNITEXAMPLE_COMPLEXNUMBER_H
#define CPPUNITEXAMPLE_COMPLEXNUMBER_H
/// Example standalone class to test with CppUnit 
/// borrowed from http://CppUnit.sourceforge.net/cppunit_cookbook.html

class ComplexNumber { 
  friend bool operator ==(const ComplexNumber& a, const ComplexNumber& b);
  friend ComplexNumber operator +(const ComplexNumber& a, const ComplexNumber& b);
  double real, imaginary;
 public:
   ComplexNumber( double r, double i = 0 ) 
     : real(r)
         , imaginary(i) 
   {
   }
 };

 bool operator ==( const ComplexNumber &a, const ComplexNumber &b )
 { 
   return a.real == b.real  &&  a.imaginary == b.imaginary; 
 }
 ComplexNumber operator +( const ComplexNumber &a, const ComplexNumber &b )
 { 
   return ComplexNumber(a.real+b.real,a.imaginary+b.imaginary); 
 }
#endif // CPPUNITEXAMPLE_COMPLEXNUMBER_H 

