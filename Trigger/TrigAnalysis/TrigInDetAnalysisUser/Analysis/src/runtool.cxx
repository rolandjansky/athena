/**
 **     @file    runtool.cxx
 **
 **       @Author.Sutton
 **
 **     @author  mark sutton
 **     @date    Mon 24 Oct 2016 15:25:43 CEST 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


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


