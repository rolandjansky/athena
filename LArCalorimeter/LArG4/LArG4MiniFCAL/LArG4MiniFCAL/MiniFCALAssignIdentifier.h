/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This class contains the geometry calculations needed to calculate
// an identifier for a given G4Step in the MiniFCAL.

// Aug-2008: M.Fincke  

#ifndef MiniFCALAssignIdentifier_H
#define MiniFCALAssignIdentifier_H

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgFactory.h"

#include "G4TrackStatus.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4ThreeVector.hh"
#include "CLHEP/Geometry/Point3D.h"

#include "LArG4Code/LArVG4DetectorParameters.h"
#include "globals.hh"

#include <map>
#include <vector>

typedef std::map<int,int> MapNumToIndex;


// Forward declarations.
class ISvcLocator;
class LArG4Identifier;
class G4Step;
class MsgStream;

class CaloDetDescrManager ;

namespace LArG4 {
  
  namespace MiniFCAL {
    
    enum eMiniFCALAssignIdentifierType { kActive, kInactive, kDead };
    
    class MiniFCALAssignIdentifier {
      
    public:
      
      // Standard implementation of a singleton pattern.
      static MiniFCALAssignIdentifier* GetInstance();
      virtual ~MiniFCALAssignIdentifier(){  ;} 
      
      LArG4Identifier CalculateIdentifier( const G4Step* a_step, const eMiniFCALAssignIdentifierType type = kActive );
      
      
    protected:
      MiniFCALAssignIdentifier();
      IMessageSvc* m_msgsvc;
      
    private:
      static MiniFCALAssignIdentifier *m_instance;
      
      
      /** a handle on Store Gate for access to the Event Store */
      StoreGateSvc* m_storeGate;
      StoreGateSvc* m_detStore;
      
            
      G4double emecZshift;
      G4double halfLength;
      G4double absThick;
      G4double layThick;

      // Map layer and ring numbers to corresponding indexes in recordsets
      MapNumToIndex layerIndexes, ringIndexes, nWafers;
      std::map<int,G4double> ringRouter;
      std::map<int,G4double> ringRinner;
      int nRings;

      bool m_initialized;      
    };
    
  } // namespace MiniFCAL
  
} // namespace LArG4

#endif // MiniFCALAssignIdentifier_H
