/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// LArHitContainer

// This class exists to provides two features that an
// AthenaHitsVector<LArHit> does not provide on its own:

// - a CLID for StoreGate

// - a std::string method that can be used to examine the contents of
// the container.

#ifndef LArSimEvent_LArHitContainer_h
#define LArSimEvent_LArHitContainer_h

#include "HitManagement/AthenaHitsVector.h"
#include "LArSimEvent/LArHit.h"

#include "AthenaKernel/CLASS_DEF.h"

class LArHitContainer:public AthenaHitsVector<LArHit>

/** @brief Hit collection */
{
public: 

  LArHitContainer (const std::string& collectionName="DefaultCollectionName" ); 

  virtual ~LArHitContainer()  ; 

    /**
     * Returns a string containing the description of this <br>
     * LArHitContainer with a dump of all the hits
     * that it contains<br>
     * <p>
     *
     * Can be used in printouts <br>
     */
    virtual operator std::string () const ;

};

CLASS_DEF (LArHitContainer, 2701 , 1 ) 

#endif     // LArSimEvent_LArHitContainer_h
