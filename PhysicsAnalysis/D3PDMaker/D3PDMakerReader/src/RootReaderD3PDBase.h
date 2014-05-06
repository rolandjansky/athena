// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RootReaderD3PDBase.h 462802 2011-10-12 16:06:24Z krasznaa $
#ifndef D3PDMAKERREADER_ROOTREADERD3PDBASE_H
#define D3PDMAKERREADER_ROOTREADERD3PDBASE_H

// STL include(s):
#include <string>
#include <set>
#include <vector>

// D3PD include(s):
#include "D3PDMakerUtils/ObjectMetadata.h"

// Local include(s):
#include "IReaderD3PD.h"

namespace D3PD {

   /**
    *  @short Base class for all ROOT-based code generators
    *
    *         To be able to simply create multiple different code generators, this
    *         class collects the functionality that all classes need to implement in
    *         the same way. Classes inheriting from this one only need to implement
    *         the <code>createReader(...)</code> function. (With as complicated
    *         code as they'd like...)
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 462802 $
    * $Date: 2011-10-12 18:06:24 +0200 (Wed, 12 Oct 2011) $
    */
   class RootReaderD3PDBase : public IReaderD3PD {

   public:
      /// Quite empty constructor
      RootReaderD3PDBase();

      /// Main function for adding a variable to the D3PD
      virtual StatusCode addVariable( const std::string& name,
                                      const std::type_info& ti,
                                      void*& ptr,
                                      const std::string& docstring = "",
                                      const void* defval = 0 );
      /// Function currently not used by the D3PDMaker code
      virtual StatusCode addDimensionedVariable( const std::string& name,
                                                 const std::type_info& ti,
                                                 void*& ptr,
                                                 const std::string& dim,
                                                 const std::string& docstring = "",
                                                 const void* defval = 0 );

      /// Function adding metadata to the D3PD. Not implemented here!
      virtual StatusCode addMetadata( const std::string& key,
                                      const void* obj,
                                      const std::type_info& ti );

      /// Function capturing the current value of the D3PD variables. Not implemented here!
      virtual StatusCode capture();
      /// Function clearing the D3PD variables. Not implemented here!
      virtual StatusCode clear();
      /// Function currently not used by the D3PDMaker code
      virtual StatusCode redim( const Dim_t* ptr );

      /// Set the "isCollection" parameter of the object
      virtual void setIsContainer( bool isContainer );
      /// Get the "isCollection" parameter of the object
      virtual bool isContainer() const;

      /// Set the common prefix of the variables
      virtual void setPrefix( const std::string& prefix );
      /// Get the common prefix of the variables
      virtual const std::string& prefix() const;

   protected:
      /// Function adding STL include statements to the header when needed
      void addSTLHeader( std::ostream& out, const char* name ) const;

      ObjectMetadata m_metadata; ///< Object holding the information about the variables

   }; // class RootReaderD3PDBase

} // namespace D3PD

#endif // D3PDMAKERREADER_ROOTREADERD3PDBASE_H
