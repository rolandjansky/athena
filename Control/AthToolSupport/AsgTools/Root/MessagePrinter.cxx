/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgTools/MessagePrinter.h>

#include <iomanip>
#include <iostream>

//
// method implementations
//

namespace asg
{
   MessagePrinter ::
   MessagePrinter (unsigned val_sourceWidth)
     : m_sourceWidth (val_sourceWidth)
   {
   }



  void MessagePrinter ::
  print (MSG::Level reqlvl, const std::string& name,
         const std::string& text)
  {
    // A stream to construct the printed message in. In order not to have to
    // manipulate std::cout directly.
    std::ostringstream output;
    output << std::setiosflags( std::ios::left );

    // add the tool name, shortening it as necessary
    if( name.size() > m_sourceWidth )
    {
      output << name.substr( 0, m_sourceWidth - 3 );
      output << "...";
    } else
    {
      output << std::setw( m_sourceWidth ) << name;
    }

    // add the level of the message
    output << std::setw( 7 ) << MSG::name( reqlvl );

    // Print the message payload:
    std::cout << output.str() << " " << text << std::endl;
  }
}
