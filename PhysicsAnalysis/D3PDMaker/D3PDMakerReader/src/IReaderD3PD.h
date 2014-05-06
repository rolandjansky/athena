// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IReaderD3PD.h 462802 2011-10-12 16:06:24Z krasznaa $
#ifndef D3PDMAKERREADER_IREADERD3PD_H
#define D3PDMAKERREADER_IREADERD3PD_H

// D3PDMaker include(s):
#include "D3PDMakerInterfaces/ID3PD.h"

namespace D3PD {

   /**
    *  @short Common interface for the code generator D3PD classes
    *
    *         To make D3PD::ReaderAlg independent of the concrete generator
    *         implementation, all the code generator D3PD classes should implement
    *         this interface. It's a very simple expansion of the D3PD::ID3PD
    *         interface...
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 462802 $
    * $Date: 2011-10-12 18:06:24 +0200 (Wed, 12 Oct 2011) $
    */
   class IReaderD3PD : public ID3PD {

   public:
      /// To make vtable happy...
      virtual ~IReaderD3PD() {}

      /// Set the "isCollection" parameter of the object
      virtual void setIsContainer( bool isContainer ) = 0;
      /// Get the "isCollection" parameter of the object
      virtual bool isContainer() const = 0;

      /// Set the common prefix of the variables
      virtual void setPrefix( const std::string& prefix ) = 0;
      /// Get the common prefix of the variables
      virtual const std::string& prefix() const = 0;

      /// Function creating the D3PDReader code
      /**
       * This function should be implemented in the child classes to generate
       * the D3PDReader source files. It's quite a bit influenced by the currently
       * only implementation (D3PD::RootReaderD3PD), but I believe it could be used
       * for other backends later on as well.
       *
       * @param classname Name of the class to be generated
       * @param dir Directory where the source(s) should be put
       * @param prefix Common prefix to the declared variables
       */
      virtual StatusCode createReader( const std::string& classname,
                                       const std::string& dir = "./" ) const = 0;

   }; // class IReaderD3PD

} // namespace D3PD

#endif // D3PDMAKERREADER_IREADERD3PD_H
