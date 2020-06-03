// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         RegSelname.h  
 **
 **   Description:    
 **                   
 **                   
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Thu Apr  5 22:47:41 BST 2007
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 


#ifndef __REGSELNAME_H
#define __REGSELNAME_H

#include <string>




class RegSelName {

public:
  
  RegSelName(const std::string& s="") : m_name(s) { }

  const std::string& getName() const { return m_name; }  

  RegSelName& operator=(const std::string& s) { m_name=s; return *this; }
 
protected:

  std::string m_name;

};



#endif  /* __REGSELNAME_H */










