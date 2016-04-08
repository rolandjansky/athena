// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETANALYSISEDM_VARHANDLE_H
#define JETANALYSISEDM_VARHANDLE_H

// ROOT include(s):
#include <TString.h>
#include <TObject.h>
#include <TDataType.h>

/////////////////////////////////////////////////////////////////////
/// This file Handle object to variable stored in TTrees.
///  The purpose of VarHandle is to provide a uniform interface to access variable
///  independently of their type.
///  Based on D3PDReader VarHandle.
///  Features :
///    - automatically read-in variable when accessed (on-demand)
///    - act as a view on a variable : does not perform SetBranchAddress if a TBranch already as an address, 
///      thus does not "steal" the adress from other part of the code. 
///    - inversely  resists to  SetBranchAddress which could redirect 
///       the branch reading to an other variable. 
///    - allows writing to an output file.
///
///  VarHandle are called "input variable" if they read their data from a TTree (in this case their m_inputTree!=NULL)
///  VarHandle are called "output variable" if they write their data to a TTree (in this case their m_outputVar=true)
///////////////////////////////////////////////////////////////////




// Forward declaration(s):
class TTree;
class TBranch;

namespace JetAnalysisEDM {
  /// only there to provide a log object.
  struct VarHandleLog : public TObject {
    VarHandleLog(): TObject(){}
    virtual ~VarHandleLog(){}
    static VarHandleLog vlog;
    ClassDef( VarHandleLog,0)
  };
  
  

  typedef std::string string_t;


   /**
    *  @short Base class for the different kind of VarHandle specializations
    *
    *
    */
  class VarHandleBase {     
        
  public:

    
    /// Constructor specifying all the needed parameters
    VarHandleBase( const string_t name = "",
                   const ::Long64_t* master = 0 );
    /// The destructor is actually only useful in the specialized class...
    virtual ~VarHandleBase() {}


    /// Get the name of the branch handled by this class
    string_t getName() const;
    /// Set the name of the branch handled by this class
    void setName( const string_t name );

    /// Get the type name of the branch handled by this object
    string_t getType() const;
    /// Set the type name of the branch handled by this object
    void setType( const string_t type );

    /// Get a pointer to the master entry variable
    const ::Long64_t* getMaster() const;
    /// Set the pointer to the master entry variable
    void setMaster( const ::Long64_t* master );

    /// Connect the object to an input tree
    virtual void readFrom( ::TTree* tree ) = 0;
    /// Connect the object to an output tree
    virtual ::TBranch* writeTo( ::TTree* tree, bool forceOutputVar=true ) = 0;

    /// Check if this variable is "active" at the moment
    ::Bool_t outputVar() const;
    /// Set the "activity level" of the variable
    void setOutputVar( ::Bool_t active = kTRUE );

    /// Returns true if the variable can be connected to an input tree or to an output tree
    //virtual ::Bool_t IsAvailable() const;

    /// Read in the current entry from the branch
    virtual void readCurrentEntry() const = 0;

    /// "clear" the variable of its contents
    virtual void clear() = 0;

    /// create the actual variable, own it.
    virtual void build() const =0;
    /// Create a clone
    virtual VarHandleBase* clone( const string_t name = "",
                                 const ::Long64_t* master = 0, ::TTree* inputTree = 0) {return NULL;}

    bool fromInput(){return (m_inputTree!=NULL);}
    bool hasInputBranch(){ return fInBranch!=NULL;}

     
  protected:
    /// Connect the variable to the branch
    ::Bool_t ConnectVariable( void* var, ::TClass* realClass,
                              EDataType dtype, Bool_t isptr ) const;
    /// Update the variable to the current entry in the D3PD
    void UpdateBranch() const;
    /// Switch to a new tree in the statistics gathering
    //void UpdateStat( ::TBranch* br ) const;
    /// Translate the typeid() type name to something ROOT understands
    string_t RootType( const string_t typeid_type ) const;
    /// Translate the typeid() type name to a huma-readable ROOT type name
    string_t RootCppType( const string_t typeid_type ) const;


    const ::Long64_t* fMaster; ///< Pointer to the current entry number
     
    ::TTree* m_inputTree; ///< The input TTree. NULL if this variable is not an input variable.
    mutable ::TBranch* fInBranch; /// The input branch belonging to this variable


    string_t fName; ///< Name of the branch to handle
    ::Bool_t m_outputVar; ///< Flag telling if the variable can be written to the output
    ::TBranch* fOutBranch; /// The input branch belonging to this variable
     
    string_t fType; ///< Variable type
    bool m_branchNotFoundFirstTime;
     
  protected:
    mutable ::Long64_t m_currentEntry;
    mutable  TTree* m_currentTree;
    mutable int m_currentTreeIndex;

  }; // class VarHandleBase


  
  // ************************************************************************************************
  /**
   *  @short Class responsible for reading primitive types from the D3PD
   * 
   *   This class allows accessing data in a TTree in a uniform way.
   *    supported types are c++ built-in types and std::vector<> of these types.
   * 
   * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>, P-A Delsart
   *
   */
  template< typename Type >
  class VarHandle : public VarHandleBase {

  public:

    template <typename T> friend class VarHelper;
    /// Convenience typedef of the used variable
    typedef Type& result_type;
    /// Convenience typedef of the used variable (const version)
    typedef const Type& const_result_type;

    /// Constructor specifying all the needed parameters
    VarHandle( const string_t name = "",
               const ::Long64_t* master = 0, ::TTree* inputTree = 0 );
    /// The destructor is actually only useful in the specialized class...
    ~VarHandle();

    /// Create a clone
    virtual VarHandleBase* clone( const string_t name = "",
                                  const ::Long64_t* master = 0, ::TTree* inputTree = 0) ;

    virtual void setParameters( const string_t name = "",const ::Long64_t* master = 0, ::TTree* inputTree = 0);

     
    /// Connect the object to an input tree
    virtual void readFrom( ::TTree* tree );
    /// Connect the object to an output tree. (if forceOutputVar==false, connect only if outputVar()==true)
    virtual ::TBranch* writeTo( ::TTree* tree, bool forceOutputVar=true );

     


    /// Operator used to access the branch itself
    result_type  operator()();
    /// Operator used to access the branch itself (constant version)
    const_result_type  operator()() const;


    /// Read in the current entry from the branch
    virtual void readCurrentEntry() const;

    /// "clear" the variable of its contents
    virtual void clear();
     
    ::Long64_t lastReadEntry() const;


    virtual void build()const ;
     
    void resize(size_t n) const ;

    const_result_type  uncheckedAccess() const {return *fVariable;}
    result_type  uncheckedAccess()  {return *fVariable;}

    Type * rawPointer(){return fVariable;}
     
    static result_type dummyValue();
     
  private:
    mutable bool fOwnVariable;
    mutable Type * fVariable; ///< The variable in memory
     
    mutable Type * m_createdVar; /// The location of the variable we created (might differ from fVariable, if the latter is changed)

    /// unconditionnaly re-connect  internal variable to the TBranch
    bool connectToTBranch();
     
  }; // class VarHandle

  struct VarNotFound {
    VarNotFound( string_t vname ) :name(vname){};
    string_t name;
  };


  
  
} // namespace JetAnalysisEDM

// Include the implementation:
#ifndef __CINT__
#include "VarHandle.icc"
#endif // __CINT__

#endif // D3PDREADER_VARHANDLE_H
