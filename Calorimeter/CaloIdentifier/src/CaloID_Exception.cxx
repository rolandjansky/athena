/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloIdentifier/CaloID_Exception.h"
#include "CxxUtils/StrFormat.h"

using CxxUtils::strformat;


CaloID_Exception::CaloID_Exception() : 
m_message("No error message") , m_code( 999 ) 
{  }

CaloID_Exception::CaloID_Exception(const std::string&  lMessage , int lCode) : 
m_message ( lMessage ) , m_code ( lCode ) 
{ }

void CaloID_Exception::message(const std::string& lMessage) 
{ m_message = lMessage ;}

std::string CaloID_Exception::message() const 
{ return m_message;}

void CaloID_Exception::code(int lCode) 
{ m_code = lCode ;}

int CaloID_Exception::code() const 
{ return m_code;}

CaloID_Exception::operator std::string () 

{
  return
    strformat ("CaloID_Exception - Error code: %d  \n ", this->code()) +
    this->message();
}

