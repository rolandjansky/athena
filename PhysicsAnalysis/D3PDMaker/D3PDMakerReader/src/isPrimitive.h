// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: isPrimitive.h 462802 2011-10-12 16:06:24Z krasznaa $
#ifndef D3PDMAKERREADER_ISPRIMITIVE_H
#define D3PDMAKERREADER_ISPRIMITIVE_H

// STL include(s):
#include <string>

namespace D3PD {

   /// Function deciding if a given type name is a primitive or not
   bool isPrimitive( const std::string& type );

} // namespace D3PD

#endif // D3PDMAKERREADER_ISPRIMITIVE_H
