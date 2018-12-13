/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    runtool.cxx         
//   
//
//   @Author.Sutton
// 
//
//   $Id: runtool.cxx, v0.0   Mon 24 Oct 2016 15:25:43 CEST sutt $


#include <string>

#include "lumiParser.h"


int main( int argc, char** argv ) { 

  if ( argc<2 ) return -1;

  std::string file = argv[1];
  
  lumiParser goodrunslist( file ); 
 
  //  std::cout << "\n" << goodrunslist << std::endl;
   
  goodrunslist.sort();

  return 0;
}


