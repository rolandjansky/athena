/* emacs: this is -*- c++ -*- */
/**
 **
 **   @file         tagname.h  
 **
 **                 noddy struct to add integer tokens to a tag   
 **                   
 **                   
 ** 
 **   @author       M.Sutton  
 **
 **   @date         Wed Jun  9 19:44:29 BST 2004
 **
 **   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **                   
 **                   
 **
 **/


#ifndef RESPLOT_TAGNAME_H
#define RESPLOT_TAGNAME_H


#include <string>


struct tagname
{
public:
  tagname(const std::string& base, int i) {
    char   sbase[512];
    sprintf(sbase, "%s[%03d]", base.c_str(), i);
    m_string = sbase;
  }


  tagname(const char* base, int i) {
    char   sbase[512];
    sprintf(sbase, "%s[%03d]", base, i);
    m_string = sbase;
  }

  tagname(const char* base, char* addition) {
    char   sbase[512];
    sprintf(sbase, "%s%s", base, addition);
    m_string = sbase;
  }

  tagname& add(const char* s) {
    char   sbase[512];
    sprintf(sbase, "%s%s", m_string.c_str(), s);
    m_string = sbase;
    return (*this);
  }

  tagname& add(const std::string& s) {
    char   sbase[512];
    sprintf(sbase, "%s%s", m_string.c_str(), s.c_str() );
    m_string = sbase;
    return (*this);
  }

  const char* c_str()     { return m_string.c_str(); }
  operator const char*()  { return m_string.c_str(); }
  operator std::string()  { return m_string; }

private:

  std::string m_string;

};


#endif  /* RESPLOT_TAGNAME_H */










