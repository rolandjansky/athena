/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  FileParse
//===============================================================================
//  Description: Class designed to parse input file header information and print 
//               error messages if an unexpected format is found.
//-------------------------------------------------------------------------------
//  Mireia Dosil, January 2001
//-------------------------------------------------------------------------------
//  Changes: K. Karr, 02.01.2002: Changed name of class from Builder to FileParse 
//           and made some minor modifications in error handling.
/////////////////////////////////////////////////////////////////////////////////

#ifndef FILE_PARSE_H
#define FILE_PARSE_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>


class FileParse
{
public: 

  static bool expect( std::ifstream &f, std::string msg, std::string fname );
  
  static bool expectInt( std::ifstream &f, std::string msg, std::string fname, int &val );
  
  static bool expectInt( std::ifstream &f, std::string msg, std::string fname, int &val, int val2 );
};

#endif
