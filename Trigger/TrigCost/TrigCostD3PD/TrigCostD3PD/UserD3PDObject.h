// Dear emacs, this is -*- c++ -*--

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CodeGenerator_v2_constants.h 539451 2013-03-01 08:33:45Z krasznaa $
#ifndef D3PDREADER_UserD3PDObject_H
#define D3PDREADER_UserD3PDObject_H

// STL include(s):
#include <map>
#include <vector>

// ROOT include(s):
#include <TNamed.h>
#include <TString.h>

// Local include(s):
#include "D3PDObjectBase.h"
//#include "D3PDReadStats.h"
#include "VarHandle.h"
#include "VarProxy.h"

// Forward declaration(s):
class TTree;

namespace D3PDReader {

   // Forward declaration(s):
   class UserD3PDObject;

   /**
    *  @short Special class for handling user variables
    *
    *         This proxy class can be used to handle user-defined vector
    *         variables in a fancy way. It is also used by all of the regular
    *         proxy classes as base class, so they could handle user defined
    *         variables as well. (For instance for adding a new property to
    *         electrons in the analysis.)
    *
    *         The class doesn't inherit from TObject on purpose, in order to
    *         avoid ending up with a diamond in the inheritance structure.
    *         (Python *really* doesn't like that...)
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 539451 $
    * $Date: 2013-03-01 09:33:45 +0100 (Fri, 01 Mar 2013) $
    */
   class UserD3PDObjectElement : public ::TNamed {

      friend class UserD3PDObject;

   protected:
      /// Constructor only visible to children and UserD3PDObject
      UserD3PDObjectElement( size_t index, const UserD3PDObject& parent );

   public:
      /// Copy constructor
      UserD3PDObjectElement( const UserD3PDObjectElement& parent );
      /// Destructor, cleaning up the created objects
      virtual ~UserD3PDObjectElement();

      /// Index of the object inside its container
      virtual size_t GetIndex() const;

      /// Access a variable
      template< typename T >
      VarProxy< T >& Variable( const TString& name );
      /// Access a variable (constant version)
      template< typename T >
      const VarProxy< T >& Variable( const TString& name ) const;

   private:
      /// Reference to the parent of this object
      const UserD3PDObject* fParent;
      /// The index of this object inside the parent container
      size_t fIndex;
      /// Book-keeper of the VarProxy members
      mutable std::map< ::TString, VarProxyBase* > fProxies;

      UserD3PDObjectElement& operator=( const UserD3PDObjectElement& parent );

      ClassDef( UserD3PDObjectElement, 0 )

   }; // class UserD3PDObjectElement

   /**
    *  @short Special class for handling user variables
    *
    *         This class is used to handle user defined variables. It can be
    *         used on its own to read/write any sort of variable, but it also
    *         acts as a base class for all the other D3PDObject classes, to
    *         make it possible to decorate the information stored by them.
    *
    *         While the class fully implements the D3PDObjectBase interface,
    *         it doesn't inherit from it, in order to avoid creating a diamond
    *         in the inheritance structure. Python is not able to handle such
    *         an inheritance structure correctly.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 539451 $
    * $Date: 2013-03-01 09:33:45 +0100 (Fri, 01 Mar 2013) $
    */
   class UserD3PDObject : public D3PDObjectBase {

   public:
      /// Constructor used when reading from a TTree
      UserD3PDObject( const ::Long64_t& master, const char* prefix = "" );
      /// Constructor when the object is only used for writing data out
      UserD3PDObject( const char* prefix = "" );
      /// Destructor, cleaning up the created objects
      virtual ~UserD3PDObject();

      /// Get the currently configured prefix value
      virtual const char* GetPrefix() const;
      /// Set the prefix for the variables
      virtual void SetPrefix( const char* prefix );

      /// Connect the object to an input TTree
      virtual void ReadFrom( ::TTree* tree );
      /// Connect the object to an output TTree
      virtual void WriteTo( ::TTree* tree );

      /// Turn (selected) branches either on or off
      virtual void SetActive( ::Bool_t active = kTRUE,
                              const ::TString& pattern = ".*" );
      /// Read in all the variables that we need to write out as well
      virtual void ReadAllActive();

      /// Get the D3PD reading statistics
      //virtual D3PDReadStats GetStatistics() const;

      /// Set the contents of the object according to another object
      UserD3PDObject& Set( const UserD3PDObject& parent );

      /// Clear the container. Useful when writing new branches.
      void Clear( Option_t* option = "" );
      /// Add one element to an output collection
      UserD3PDObject& Add( const UserD3PDObjectElement& el );

      /// Access a proxy class describing one element of the container
      UserD3PDObjectElement& operator[]( size_t index );
      /// Access a proxy class describing one element of the container (constant version)
      const UserD3PDObjectElement& operator[]( size_t index ) const;
      /// Add one element to an output collection
      UserD3PDObject& operator+=( const UserD3PDObjectElement& el );

      /// Declare a new variable for writing
      template< typename T >
      void DeclareVariable( const TString& name );
      /// Access a variable
      template< typename T >
      VarHandle< T >& Variable( const TString& name );
      /// Access a variable (constant version)
      template< typename T >
      const VarHandle< T >& Variable( const TString& name ) const;

   private:
      const ::Long64_t* fMaster; ///< Pointer to the master entry number
      ::TString fPrefix; ///< Prefix to the branch names
      ///< Internal list of proxy objects
      mutable std::vector< UserD3PDObjectElement* > fProxies;
      /// Book-keeper of the VarHandle members
      mutable std::map< ::TString, VarHandleBase* > fHandles;
      /// Book-keeper of VarHandle objects needed for technical reasons
      mutable std::vector< VarHandleBase* > fExtraHandles;
      /// Flag specifying if object is used for D3PD reading
      const ::Bool_t fFromInput;
      /// TTree that is being read currently
      ::TTree* fInTree;

      ClassDef( UserD3PDObject, 0 )

   }; // class UserD3PDObject

} // namespace D3PDReader

// Include the template implementation:
#ifndef __CINT__
#include "UserD3PDObject.icc"
#endif // __CINT__

#endif // D3PDREADER_UserD3PDObject_H
