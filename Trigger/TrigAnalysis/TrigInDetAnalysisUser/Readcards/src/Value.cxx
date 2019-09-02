
/**
 **
 **   @file         Value.cxx        
 **
 **                   
 **                   
 **                   
 ** 
 **   @author       M.Sutton    
 **
 **   @date         Fri May 11 17:29:29 BST 2007
 **
 **   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **                   
 **                   
 **
 **/


#include "Value.h"


std::ostream& operator<<(std::ostream& s, const Value& v) { 

  const std::vector<std::string>& vals = v.Val();
  s << v.Tag() << "\t= ";
  if ( vals.size()>1 ) { 
    s << "{ ";
    if ( vals.size()>12 ) s<< "\t"; 
    for ( unsigned i=0 ; i<vals.size()-1 ; i++ ) {
      if ( i && i%12==0 ) s << "\n\t\t";
      s << vals[i] << ", ";
    }
  }
  if ( vals.size()>0 )  s << vals[vals.size()-1];
  if ( vals.size()>12 ) s << "  ";
  if ( vals.size()>1 )  s << " }";
  
  return s;
}










