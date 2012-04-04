/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloCalibrationHitContainer
// 09-Feb-2004 William Seligman

// This class exists to provides two features that an
// AthenaHitsVector<CaloCalibrationHit> does not provide on its own:

// - a CLID for StoreGate

// - a std::string method that can be used to examine the contents of
// the container.

#ifndef CaloSimEvent_CaloCalibrationHitContainer_h
#define CaloSimEvent_CaloCalibrationHitContainer_h

#include "HitManagement/AthenaHitsVector.h"
#include "CaloSimEvent/CaloCalibrationHit.h"

#include "CLIDSvc/CLASS_DEF.h"

class CaloCalibrationHitContainer:public AthenaHitsVector<CaloCalibrationHit>
{
public:

  /** Constructor of CaloCalibrationHitContainer */
  CaloCalibrationHitContainer (std::string collectionName="DefaultCollectionName" );

  /** Destructor */
  virtual ~CaloCalibrationHitContainer()  ;

  /**
     Returns a string containing the description of this <br>
     CaloCalibrationHitContainer with a dump of all the hits
     that it contains<br>
     Can be used in printouts <br>
  */
  virtual operator std::string () const;

};

CLASS_DEF (CaloCalibrationHitContainer, 1312841250 , 1 )

class StoredLArCalibHitContainers
/** @brief store pointers to the different hit collections */
{
 public:
  /** Constructor */
  StoredLArCalibHitContainers():
    activeHitCollection(0),
    inactiveHitCollection(0),
    deadHitCollection(0)
    {}

  /** Active calibration Hits */
  CaloCalibrationHitContainer* activeHitCollection;

  /** Inactive calibration Hits */
  CaloCalibrationHitContainer* inactiveHitCollection;

  /** Dead calibration Hits */
  CaloCalibrationHitContainer* deadHitCollection;
};

CLASS_DEF (StoredLArCalibHitContainers, 1074460253, 1)

#endif     // CaloSimEvent_CaloCalibrationHitContainer_h
