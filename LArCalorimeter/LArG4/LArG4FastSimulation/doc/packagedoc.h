/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page LArG4FastSimulation_page The LArG4 Fast Simulation Package

 - This class is the base class for the fast simulation in athena 

 - LArFastShower: the main guiding code for the fast simulation.  Controls application of the
    various types of fast simulation.

 - For each subdetector in {FCAL, FCAL2, EMB, EMEC}, a separate class contains all constants
     for parameterization (ShowerParameters) which inherit from a base class

 - For each subdetector in {HEC, FCAL, FCAL2, EMB, EMEC}, a separate class contains all flags and
     energy cut offs for parameterization, frozen showers, and killing application (ParticleBounds)
     which inherit from a base class 

 - For each subdetector in {FCAL, EMB, EMEC}, a separate class contains special sensitive
    detectors for frozen shower library application (FastSimDedicatedSD) which inherit from
    a base class

 - TestActionTimer: a class for recording simulation time divided by subdetector and by particle type

 - LArG4FastSimulation_setupTimer_jobOptions.py: job options for enabling the TestActionTimer class

 - analyzeTimer.pl: a perl script for analyzing the results of the TestActionTimer in the sim log file

 - LArG4FastSimulation_frozenShowers_jobOptions.py: job option script for setting up frozen showers
    during simulation.  Not necessary any more.

 - LArG4FastSimulation_setup_jobOptions.py: job options script for setting up fast simulation.
    Not necessary any more with current flag construction.

 - For questions or comments: Joe Boudreau (boudreau@pitt.edu) or the Atlas Fast Simulation Group (atlas-shower-param@cern.ch)



--------------------------------
  REQUIREMENTS 
--------------------------------


*/
