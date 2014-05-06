// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RootReaderD3PD_v2.h 462802 2011-10-12 16:06:24Z krasznaa $
#ifndef D3PDMAKERREADER_ROOTREADERD3PD_V2_H
#define D3PDMAKERREADER_ROOTREADERD3PD_V2_H

// Local include(s):
#include "RootReaderD3PDBase.h"

namespace D3PD {

   /**
    *  @short D3PD class used for generating ROOT D3PDReader classes
    *
    *         Version 2 of the D3PDReader implementation.
    *
    * @suthor Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 462802 $
    * $Date: 2011-10-12 18:06:24 +0200 (Wed, 12 Oct 2011) $$
    */
   class RootReaderD3PD_v2 : public RootReaderD3PDBase {

   public:
      /// Constructor specifying whether it's a container's D3PD
      RootReaderD3PD_v2();

      /// Function creating the D3PDReader C++ code
      StatusCode createReader( const std::string& classname,
                               const std::string& dir = "./" ) const;

   }; // class RootReaderD3PD_v2

} // namespace D3PD

#endif // D3PDMAKERREADER_ROOTREADERD3PD_V2_H
