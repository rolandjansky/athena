/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**


@mainpage

This package is used by Calorimeter simulation with Calibration Hits.

The key classes of this package are:

1. CalibrationDefaultProcessing. This class provides a "default behavior" for all energy deposits that are not made in a volume that's been made sensitive for calibration studies.

2. EscapedEnergyRegistry. This class keeps track of which types of volumes use which VEscapedEnergyProcessing objects.

3. SimulationEnergies. This class implements the calculations requires to categorize the energies deposited during the simulation.  It's intended for use in calibration studies.

4. VEscapedEnergyProcessing. This interface "hides" the different escaped-energy processing required by the different detectors in the simulation.

--------------------------------
  REQUIREMENTS 
--------------------------------

@include requirements
@htmlinclude used_packages.html 

*/
