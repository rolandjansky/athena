# -*- coding: iso-8859-1 -*-
#######################################################
# SIMPLE JOB CONFIGURATION FOR MUON ANALYSES
# SPECIFY THE FOLLOWING JOB PARAMETERS
#######################################################

if 'EvtMax' not in dir():
    EvtMax              = -1              # NUMBER OF EVENTS TO PROCESS
if 'skipEvt' not in dir():
    skipEvt             = 0               # NUMBER OF EVENTS TO SKIP
if 'input_type' not in dir():
    input_type          = "data"          # data, geant4
if 'input_format' not in dir():
    input_format        = "AOD"           # AOD, ESD
if 'filter_cells' not in dir():
    filter_cells        = True            # FILTER CELLS TRUE/FALSE
if 'filterConfig' not in dir():
    filterConfig        = 0               # FILTER CELLS BASED ON PROXIMITY TO MUONS (0) OR TRACKS (1)
if 'filter_tracks' not in dir():
    filter_tracks       = True            # FILTER TRACKS BASED ON TRACKPARTICLECANDIDATE
if 'include_lar' not in dir():
    include_lar         = False           # INCLUDE LIQUID ARGON
if 'beam_type' not in dir():
    beam_type           = "collisions"    # BEAMTYPE: cosmics, collisions
if 'muon_type' not in dir():
    muon_type    = 0                      # MUON TRACK TYPE: 0 (COMBINED), 1 (ID), 2 (MS), 3 (EXTRAPOLATED MS), 4 (EXTRAPOLATED ID)
if 'monitor_performance' not in dir():
    monitor_performance = False           # WHETHER (MEMORY) MONITORING SHOULD BE ENABLED
if 'levelOfDetails' not in dir():
    levelOfDetails      = 10              # LEVEL OF DETAILS (1-9)
if "outputFile" not in dir():
    outputFile          = "output.root"   # OUTPUT FILE

if "inputFiles" not in dir():             # INPUT FILES
    if 'inputFile' not in dir() or inputFile=="":
        inputFiles = ['AOD.pool.root']
    else:
        if inputFile.endswith(".py"):
            include(inputFile)
        else:
            inputFiles = [inputFile]

####################
# LEVEL OF DETAILS #
####################
#
# 0 METADATA (IF ANY)
# 1 BASIC EVENT INFORMATION
# 2 MUON KINEMATICS
# 3 ELABORATE POSITIONING AND EVENT INFORMATION
# 4 ISOLATION, VERTEXING, TRIGGER, TOTAL PATH LENGTH
# 5 BASIC MISSING ENERGY, PMT INFORMATION
# 6 CALORIMETRIC MISSING ENERGY
# 7 ELABORATE MISSING ENERGY, TRUTH INFORMATION, ACTIVE PATH LENGTH
# 8 INVARIANT MASS
# 9-255 NO NEW DETAILS
#

#######################################################
# ATHENA CONFIGURATION FILE(S)
#######################################################
include("TileD3PDMaker/configurator_mu.py")
