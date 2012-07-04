/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "CLIDSvc/CLASS_DEF.h"

class LArHitContainer:public AthenaHitsVector<LArHit>

/** @brief Hit collection */
{
public: 

  LArHitContainer (std::string collectionName="DefaultCollectionName" ); 

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

class StoredLArHitContainers

/** @brief store pointers to the different hit collections */
{
 public:
  StoredLArHitContainers():
    embHitCollection(0),
    emecHitCollection(0),
    fcalHitCollection(0),
    hecHitCollection(0),
    miniFcalHitCollection(0)
    {}

  /** pointer to EM barrel hit collection */
  LArHitContainer* embHitCollection;
  /** pointer to EMEC hit collection */
  LArHitContainer* emecHitCollection;
  /** pointer to FCAL hit collection */
  LArHitContainer* fcalHitCollection;
  /** pointer to HEC hit collection */
  LArHitContainer* hecHitCollection;
  /** pointer to Mini FCAL hit collection */
  LArHitContainer* miniFcalHitCollection;
};

CLASS_DEF( StoredLArHitContainers , 1265102271 , 1 )

#endif     // LArSimEvent_LArHitContainer_h
