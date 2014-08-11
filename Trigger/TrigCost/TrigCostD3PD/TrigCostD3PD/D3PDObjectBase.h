// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CodeGenerator_v2_constants.h 539451 2013-03-01 08:33:45Z krasznaa $
#ifndef D3PDREADER_D3PDOBJECTBASE_H
#define D3PDREADER_D3PDOBJECTBASE_H

// ROOT include(s):
#include <TNamed.h>
#include <TString.h>

// Local include(s):
//#include "D3PDReadStats.h"

// Forward declaration(s):
class TTree;

namespace D3PDReader {

   /**
    *  @short Common base class for all main D3PDReader classes
    *
    *         To make the classes a bit easier to handle in "smart" code
    *         they inherit from a common base that defines all of their
    *         common features.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 539451 $
    * $Date: 2013-03-01 09:33:45 +0100 (Fri, 01 Mar 2013) $
    */
   class D3PDObjectBase : public ::TNamed {

   public:
      /// Default constructor
      D3PDObjectBase() : ::TNamed() {}

      /// Get the currently configured prefix value
      virtual const char* GetPrefix() const = 0;
      /// Set the prefix for the variables
      virtual void SetPrefix( const char* prefix ) = 0;

      /// Connect the object to an input TTree
      virtual void ReadFrom( ::TTree* tree ) = 0;
      /// Connect the object to an output TTree
      virtual void WriteTo( ::TTree* tree ) = 0;

      /// Turn (selected) branches either on or off
      virtual void SetActive( ::Bool_t active = kTRUE,
                              const ::TString& pattern = ".*" ) = 0;

      /// Read in all the variables that we need to write out as well
      virtual void ReadAllActive() = 0;

      /// Get the D3PD reading statistics
      //virtual D3PDReadStats GetStatistics() const = 0;

      ClassDef( D3PDObjectBase, 0 )

   }; // class D3PDObjectBase

} // namespace D3PDReader

#endif // D3PDREADER_D3PDOBJECTBASE_H
