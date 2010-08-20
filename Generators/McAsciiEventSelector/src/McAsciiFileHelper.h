///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McAsciiFileHelper.h 
// Header file for McAsciiFileHelper (utils)
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCASCIIEVENTSELECTOR_MCASCIIFILEHELPER_H 
#define MCASCIIEVENTSELECTOR_MCASCIIFILEHELPER_H 

// STL includes
#include <string>

// Gaudi includes

// Forward declaration
class MsgStream;

namespace McAsciiFileHelper
{ 

  std::size_t evtMax( const std::string& name, MsgStream& msg );

} // namespace McAsciiFileHelper

#endif //> MCASCIIEVENTSELECTOR_MCASCIIFILEHELPER_H
