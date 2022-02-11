/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// This class contains the geometry calculations needed to calculate
// an identifier for a given G4Step in the MiniFCAL.

// Aug-2008: M.Fincke

#ifndef LARG4MINIFCAL_MINIFCALASSIGNIDENTIFIER_H
#define LARG4MINIFCAL_MINIFCALASSIGNIDENTIFIER_H

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/Algorithm.h"

#include "G4TrackStatus.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4ThreeVector.hh"

#include "globals.hh"

#include <map>
#include <vector>

typedef std::map<int,int> MapNumToIndex;


// Forward declarations.
class ISvcLocator;
class LArG4Identifier;
class G4Step;
class MsgStream;

namespace LArG4 {

  namespace MiniFCAL {

    enum eMiniFCALAssignIdentifierType { kActive, kInactive, kDead };

    class MiniFCALAssignIdentifier {

    public:

      // Standard implementation of a singleton pattern.
      static MiniFCALAssignIdentifier* GetInstance();
      virtual ~MiniFCALAssignIdentifier(){  ;}

      LArG4Identifier CalculateIdentifier( const G4Step* a_step, const eMiniFCALAssignIdentifierType type = kActive ) const;


    protected:
      MiniFCALAssignIdentifier();
      IMessageSvc* m_msgsvc = nullptr;

    private:
      static MiniFCALAssignIdentifier *m_instance;


      G4double m_halfLength;
      G4double m_absThick;
      G4double m_layThick;

      // Map layer and ring numbers to corresponding indexes in recordsets
      MapNumToIndex m_ringIndexes, m_nWafers;
      std::map<int,G4double> m_ringRouter;
      std::map<int,G4double> m_ringRinner;
      int m_nRings;

      bool m_initialized;
    };

  } // namespace MiniFCAL

} // namespace LArG4

#endif // MiniFCALAssignIdentifier_H
