// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VarHandle.h 726663 2016-02-28 01:22:15Z ssnyder $
#ifndef TRIGROOTANALYSIS_VARHANDLE_H
#define TRIGROOTANALYSIS_VARHANDLE_H

// ROOT include(s):
#include <TString.h>
#include <TDataType.h>

// Forward declaration(s):
class TObject;
class TTree;
class TBranch;

namespace D3PDReaderPriv {

   /**
    *  @short Base class for the different kind of VarHandle specializations
    *
    *         This class is used to keep a list of all the VarHandle members of
    *         a D3PDObject class. It makes some operations much easier.
    *
    * $Revision: 726663 $
    * $Date: 2016-02-28 02:22:15 +0100 (Sun, 28 Feb 2016) $
    */
   class VarHandleBase {

   protected:
      /// Custom enumeration describing the availability of the branch
      enum BranchAvailability {
         UNKNOWN = 0, ///< The input TTree has not yet been checked
         AVAILABLE = 1, ///< The input branch is available
         UNAVAILABLE = 2 ///< The input branch is not available
      };

   public:
      /// Constructor specifying all the needed parameters
      VarHandleBase( ::TObject* parent = 0, const char* name = "",
                     const ::Long64_t* master = 0 );
      /// The destructor is actually only useful in the specialized class...
      virtual ~VarHandleBase() {}

      /// Get a pointer to the parent of the object
      ::TObject* GetParent() const;
      /// Set the pointer to the parent of the object
      void SetParent( ::TObject* parent );

      /// Get the name of the branch handled by this class
      const char* GetName() const;
      /// Set the name of the branch handled by this class
      void SetName( const char* name );

      /// Get the type name of the branch handled by this object
      const char* GetType() const;
      /// Set the type name of the branch handled by this object
      void SetType( const char* type );

      /// Get a pointer to the master entry variable
      const ::Long64_t* GetMaster() const;
      /// Set the pointer to the master entry variable
      void SetMaster( const ::Long64_t* master );

      /// Connect the object to an input tree
      virtual void ReadFrom( ::TTree* tree ) = 0;
      /// Connect the object to an output tree
      virtual ::TBranch* WriteTo( ::TTree* tree ) = 0;

      /// Check if this variable is "active" at the moment
      ::Bool_t IsActive() const;
      /// Set the "activity level" of the variable
      void SetActive( ::Bool_t active = kTRUE );

      /// Check if the variable is available in the input
      virtual ::Bool_t IsAvailable() const;

      /// Read in the current entry from the branch
      virtual void ReadCurrentEntry() const = 0;

      /// "Clear" the variable of its contents
      virtual void Clear() = 0;

   protected:
      /// Connect the variable to the branch
      ::Bool_t ConnectVariable( void* var, ::TClass* realClass,
                                EDataType dtype, Bool_t isptr ) const;
      /// Update the variable to the current entry in the D3PD
      void UpdateBranch() const;
      /// Switch to a new tree in the statistics gathering
      void UpdateStat( ::TBranch* br ) const;
      /// Translate the typeid() type name to something ROOT understands
      const char* RootType( const char* typeid_type ) const;
      /// Translate the typeid() type name to a huma-readable ROOT type name
      const char* RootCppType( const char* typeid_type ) const;

      const ::Long64_t* m_master; ///< Pointer to the current entry number
      ::TObject* m_parent; ///< Pointer to the parent D3PDObject
      ::Bool_t m_fromInput; ///< Flag showing if the variable is read from an input TTree
      ::TTree* m_inTree; ///< The input TTree
      mutable ::TBranch* m_inBranch; /// The input branch belonging to this variable
      mutable BranchAvailability m_available; ///< Availability of the branch

   private:
      ::TString m_name; ///< Name of the branch to handle
      ::Bool_t m_active; ///< Flag telling if the variable can be written to the output

      ::TString m_type; ///< Variable type
      mutable std::vector< ::Long64_t > m_entriesRead; ///< Number of read entries for each tree
      mutable std::vector< ::Float_t > m_branchSize; ///< Unzipped entry size for each tree
      mutable std::vector< ::Float_t > m_zippedSize; ///< Zipped entry size for each tree

   }; // class VarHandleBase

   /**
    *  @short Class responsible for reading primitive types from the D3PD
    *
    *         This class is used by all the D3PDReader classes to physically
    *         handle the branches of the input TTree.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 726663 $
    * $Date: 2016-02-28 02:22:15 +0100 (Sun, 28 Feb 2016) $
    */
   template< typename Type >
   class VarHandle : public VarHandleBase {

   public:
      /// Convenience typedef of the used variable
      typedef Type& result_type;
      /// Convenience typedef of the used variable (const version)
      typedef const Type& const_result_type;

      /// Constructor specifying all the needed parameters
      VarHandle( ::TObject* parent = 0, const char* name = "",
                 const ::Long64_t* master = 0 );
      /// The destructor is actually only useful in the specialized class...
      ~VarHandle();

      /// Connect the object to an input tree
      virtual void ReadFrom( ::TTree* tree );
      /// Connect the object to an output tree
      virtual ::TBranch* WriteTo( ::TTree* tree );

      /// Operator used to access the branch itself
      result_type operator()();
      /// Operator used to access the branch itself (constant version)
      const_result_type operator()() const;

      /// Read in the current entry from the branch
      virtual void ReadCurrentEntry() const;

      /// "Clear" the variable of its contents
      virtual void Clear();

   private:
      mutable Type m_variable; ///< The variable in memory

   }; // class VarHandle

   /**
    *  @short Class responsible for reading STL objects from the D3PD
    *
    *         This specialization of the template class makes it possible to
    *         handle branches describing primitive types and branches describing
    *         STL collections a little differently.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 726663 $
    * $Date: 2016-02-28 02:22:15 +0100 (Sun, 28 Feb 2016) $
    */
   template< typename Type >
   class VarHandle< Type* > : public VarHandleBase {

   public:
      /// Convenience typedef of the used variable
      typedef Type* result_type;
      /// Convenience typedef of the used variable (const version)
      typedef const Type* const_result_type;

      /// Constructor specifying all the needed parameters
      VarHandle( ::TObject* parent = 0, const char* name = "",
                 const ::Long64_t* master = 0 );
      /// This destructor actually does something...
      ~VarHandle();

      /// Connect the object to an input tree
      virtual void ReadFrom( ::TTree* tree );
      /// Connect the object to an output tree
      virtual ::TBranch* WriteTo( ::TTree* tree );

      /// Operator used to access the branch itself
      result_type operator()();
      /// Operator used to access the branch itself (constant version)
      const_result_type operator()() const;

      /// Read in the current entry from the branch
      virtual void ReadCurrentEntry() const;

      /// "Clear" the variable of its contents
      virtual void Clear();

   private:
      mutable Type* m_variable; ///< The variable in memory
   }; // class VarHandle

} // namespace D3PDReaderPriv

// Include the implementation:
#ifndef __CINT__
#include "VarHandle.icc"
#endif // __CINT__

#endif // TRIGROOTANALYSIS_VARHANDLE_H
