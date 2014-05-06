// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RootReaderD3PD_v1.h 462802 2011-10-12 16:06:24Z krasznaa $
#ifndef D3PDMAKERREADER_ROOTREADERD3PD_V1_H
#define D3PDMAKERREADER_ROOTREADERD3PD_V1_H

// Local include(s):
#include "RootReaderD3PDBase.h"

namespace D3PD {

   /**
    *  @short D3PD class used for generating ROOT D3PDReader classes
    *
    *         Version 1 of the D3PDReader implementation.
    *
    * @suthor Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 462802 $
    * $Date: 2011-10-12 18:06:24 +0200 (Wed, 12 Oct 2011) $
    */
   class RootReaderD3PD_v1 : public RootReaderD3PDBase {

   public:
      /// Quite empty constructor
      RootReaderD3PD_v1();

      /// Function creating the D3PDReader C++ code
      StatusCode createReader( const std::string& classname,
                               const std::string& dir = "./" ) const;

   private:
      /// Function creating the D3PDReader C++ header
      StatusCode writeHeader( const std::string& classname,
                              const std::string& dir ) const;
      /// Function creatig the D3PDReader C++ source
      StatusCode writeSource( const std::string& classname,
                              const std::string& dir ) const;

   }; // class RootReaderD3PD_v1

} // namespace D3PD

#endif // D3PDMAKERREADER_ROOTREADERD3PD_V1_H
