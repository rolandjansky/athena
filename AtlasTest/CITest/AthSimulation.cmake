# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# CI test definitions for the AthSimulation project
# --> README.md before you modify this file
#

atlas_add_citest( DuplicateClass
   SCRIPT python -c 'import ROOT'
   PROPERTIES FAIL_REGULAR_EXPRESSION "class .* is already in" )

atlas_add_citest( G4ExHive
   SCRIPT athena.py --threads=4 --evtMax=50 G4AtlasApps/jobOptions.G4AtlasMT.py
   PROPERTIES PROCESSORS 4 )

atlas_add_citest( SimulationRun2FullSim
   SCRIPT RunWorkflowTests_Run2.py --CI -s -w FullSim -e '--maxEvents 10'
   LOG_IGNORE_PATTERN "WARNING FPE INVALID" )  # ignore FPEs from Geant4
