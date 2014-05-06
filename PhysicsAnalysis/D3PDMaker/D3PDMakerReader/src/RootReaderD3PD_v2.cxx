/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RootReaderD3PD_v2.cxx 516932 2012-09-10 09:29:08Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "RootReaderD3PD_v2.h"
#include "CodeGenerator_v2.h"

namespace D3PD {

   RootReaderD3PD_v2::RootReaderD3PD_v2() {

   }

   StatusCode RootReaderD3PD_v2::createReader( const std::string& classname,
                                               const std::string& dir ) const {

      //
      // Create the source files:
      //
      CHECK( Version2::writeD3PDObjectBase( dir ) );
      CHECK( Version2::writeVarHandle( dir ) );
      if( m_metadata.container() ) {
         CHECK( Version2::writeVarProxy( dir ) );
      }
      CHECK( Version2::writeUserD3PDObject( dir ) );
      CHECK( Version2::writeD3PDReadStats( dir ) );
      CHECK( Version2::writeD3PDPerfStats( dir ) );
      CHECK( Version2::writeUtils( dir ) );
      CHECK( Version2::writeHeader( classname, dir, m_metadata ) );
      CHECK( Version2::writeSource( classname, dir, m_metadata ) );

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
