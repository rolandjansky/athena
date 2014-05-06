// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Variable.h 348546 2011-03-01 15:09:56Z krasznaa $
#ifndef D3PDMAKERREADER_VARIABLE_H
#define D3PDMAKERREADER_VARIABLE_H

// STL include(s):
#include <string>

namespace D3PD {

   /**
    *  @short Structure describing one D3PD variable
    *
    *         This structure is used to provide information to the code generator
    *         functions about a single D3PD variable.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 348546 $
    * $Date: 2011-03-01 16:09:56 +0100 (Tue, 01 Mar 2011) $
    */
   struct Variable {

      std::string type; ///< Full type name of the variable
      std::string fullname; ///< Full name of the variable in the D3PD
      std::string doc; ///< Variable documentation string
      mutable std::string name; ///< Variable name without prefix
      mutable std::string varname; ///< Variable name without prefix and whitespaces
      bool primitive; ///< Flag showing whether variable is a primitive

   }; // struct Variable

} // namespace D3PD

#endif // D3PDMAKERREADER_VARIABLE_H
