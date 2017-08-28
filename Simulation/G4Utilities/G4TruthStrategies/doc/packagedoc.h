/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page G4TruthStrategies_page G4TruthStrategies
@author Andrea Dell'Acqua (dellacqu@mail.cern.ch)

@section G4TruthStrategies_G4TruthStrategies Introduction

This class provides all the truth strategies to be used in the simulation.  These strategies decide when a vertex will be saved to the truth tree.  Each is constructed and included in G4AtlasApps, where it is also assigned to the appropriate region of the detector.  Cuts for the strategies are all configurable from job options.

@section G4TruthStrategies_G4TruthStrategies Class Overview

The truth strategies included in this package are:

 - BremsstrahlungStrategy : For saving hard Brems (in the ID)
 - CaloCatchAllStrategy : Currently empty
 - ComptonStrategy : For saving hard compton scatters (in the ID)
 - ConversionStrategy : For saving photon conversions (in the ID)
 - DecayStrategy : For saving particle decays (in the ID)
 - HadronicInteractionStrategy : For saving hadronic interactions (in the ID)
 - IonizationStrategy : For saving hard ionizations (in the ID)
 - MuonCatchAllStrategy : For saving hard brems from muons (in the calorimetry)



*/
