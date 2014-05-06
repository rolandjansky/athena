// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RootObjectMetadata.h 530460 2012-12-13 18:27:24Z krasznaa $
#ifndef D3PDMAKERREADER_ROOTOBJECTMETADATA_H
#define D3PDMAKERREADER_ROOTOBJECTMETADATA_H

// STL include(s):
#include <string>

// D3PD include(s):
#include "D3PDMakerUtils/ObjectMetadata.h"

namespace D3PD {

   /**
    *  @short Extension of the ObjectMetadata class for reading D3PD files
    *
    *         The base D3PD::ObjectMetadata class needs to get some extra
    *         features for interpreting the metadata found in D3PD ROOT files
    *         correctly. Instead of putting all the frunctionality into the
    *         base class, it is only put here.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 530460 $
    * $Date: 2012-12-13 19:27:24 +0100 (Thu, 13 Dec 2012) $
    */
   class RootObjectMetadata : public ObjectMetadata {

   public:
      /// Default constructor
      RootObjectMetadata();
      /// Copy constructor
      RootObjectMetadata( const RootObjectMetadata& parent );

      /// Make the implementation from ObjectMetadata visible
      using ObjectMetadata::addVariable;

      /// Function adding a variable, when the exact type_info is not available
      StatusCode addVariable( const std::string& name,
                              const std::string& type,
                              const std::string& docstring );

      /// Remove possibly overlooked prefixes in the variable names
      StatusCode checkPrefixes();

      /// Function guessing if an object with a given name is object metadata
      static bool isObjectMetadata( const std::string& name );

   private:
      // Avoid coverity warning.
      RootObjectMetadata& operator=( const RootObjectMetadata& );

   }; // class RootObjectMetadata

} // namespace D3PD

#endif // D3PDMAKERREADER_ROOTOBJECTMETADATA_H
