/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __TCALCULATORTOOLBASE__
#define __TCALCULATORTOOLBASE__

/**
   @class TCalculatorToolBase
   @brief Bsse class for tools to retrieve efficiencies, scale factors,... for objects in pure ROOT

   @author Karsten Koeneke (CERN)
   @date   October 2011
*/

// ROOT includes
#include "TString.h"

// Include the return object
#include "TResult.h"


namespace Root {
  class [[deprecated("do not use for multi-threaded code")]] TCalculatorToolBase
  {

  public: 
    /** Standard constructor */
    TCalculatorToolBase(const char* name="TCalculatorToolBase") :
      m_name(name),
      m_result( Form("%s_TResult",name) )
    {
    }
    
    /** Standard destructor */
    virtual ~TCalculatorToolBase(){}
  

    // Main methods
  public:
    /** Initialize this class */
    virtual int initialize() = 0;

    /** Finalize this class; everything that should be done after the event loop should go here */
    virtual int finalize() = 0;


    /** Get the name of the class instance */
    inline const char* getName() const { return m_name; }


    /** Method to get the plain TResult */
    inline const Root::TResult& getTResult( ) const { return m_result; } 


#ifdef ROOTCORE 
    /** For convenient PROOF support */
    ClassDef(TCalculatorToolBase,1); 
#endif 


    // Protected members
  protected:
    /** The name of the class instance */
    TString m_name;

    /** The return TResult object */
    mutable TResult m_result;


  }; // End: class definition


} // End: namespace Root

#endif
