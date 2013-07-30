/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GoodRunsLists_TUniqueGRLString_h
#define GoodRunsLists_TUniqueGRLString_h

#include "TString.h"

namespace Root {

  class TUniqueGRLString : public TString {
 
   public:
 
    // access to the single instance of this class (created statically on first request)
    static TUniqueGRLString& instance();

    // =operator
    //TUniqueGRLString& TUniqueGRLString::operator=(const TUniqueGRLString& other);

    void SetTo(const TString& other);
 
   private:
     
    // default constructor (called by instance())
    TUniqueGRLString();
    /// Destructor.  Destroys all modules objects created

    /**
     * Since the instance of this class is static, it will only ever be deleted
     * after main - so no client will ever get a pointer to a deleted object.
     */
    virtual ~TUniqueGRLString();
  };

}

#endif

