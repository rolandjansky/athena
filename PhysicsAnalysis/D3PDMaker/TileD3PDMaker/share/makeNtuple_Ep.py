# -*- coding: iso-8859-1 -*-
#######################################################
# SIMPLE JOB CONFIGURATION FOR E OVER P ANALYSES
# SPECIFY THE FOLLOWING JOB PARAMETERS
#######################################################

if 'EvtMax' not in dir():
    EvtMax              = -1              # NUMBER OF EVENTS TO PROCESS
if 'skipEvt' not in dir():
    skipEvt             = 0               # NUMBER OF EVENTS TO SKIP
if 'input_type' not in dir():
    input_type          = "data"          # data, geant4
if 'input_format' not in dir():
    input_format        = "ESD"           # AOD, ESD
if 'filter_cells' not in dir():
    filter_cells        = True            # FILTER CELLS BASED ON PROXIMITY TO MUONS
if 'filter_tracks' not in dir():
    filter_tracks       = True            # FILTER TRACKS BASED ON TRACKPARTICLECANDIDATE
if 'filterConfig' not in dir():
    filterConfig        = 1               # FILTER CELLS BASED ON PROXIMITY TO MUONS (0) OR TRACKS (1)
if 'include_lar' not in dir():
    include_lar         = False           # INCLUDE LIQUID ARGON
if 'monitor_performance' not in dir():
    monitor_performance = False           # WHETHER (MEMORY) MONITORING SHOULD BE ENABLED
if 'levelOfDetails' not in dir():
    levelOfDetails      = 10              # LEVEL OF DETAILS (1-9)
if "outputFile" not in dir():
    outputFile          = "output.root"   # OUTPUT FILE
if "cutSizeOfCone" not in dir():
    cutSizeOfCone       = 0.4             # SIZE OF CONE OF INTEREST AROUND TRACK
if "cutP" not in dir():
    cutP                = 3000            # MOMENTUM CUT FOR TRACKS
if "cutEta" not in dir():
    cutEta              = 2.4             # ETA CUT FOR TRACKS


if 'standalone_muons' not in dir():
    standalone_muons    = False           # WHETHER STANDALONE MUONS (MUON SPECTROMETER) SHOULD BE USE IF NO COMBINED MUON IS FOUND
if 'beam_type' not in dir():
    beam_type           = "collisions"    # BEAMTYPE: cosmics, collisions


if "inputFiles" not in dir():             # INPUT FILES
    if 'inputFile' not in dir() or inputFile=="":
        inputFiles = ['/afs/cern.ch/user/l/lcerdaal/work/public/EoverP/data12_8TeV.00209550.physics_MinBias.merge.DESD_MBIAS.r4065_p1278_tid01069837_00/DESD_MBIAS.01069837._000010.pool.root.1']
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
# 1 BASIC EVENT INFORMATION                                          ### FIXME
# 2 TRACK KINEMATICS                                                 ### FIXME
# 3 ELABORATE POSITIONING AND EVENT INFORMATION                      ### FIXME
# 4 ISOLATION, VERTEXING, TRIGGER, TOTAL PATH LENGTH                 ### FIXME
# 5 BASIC MISSING ENERGY, PMT INFORMATION                            ### FIXME
# 6 CALORIMETRIC MISSING ENERGY                                      ### FIXME
# 7 ELABORATE MISSING ENERGY, TRUTH INFORMATION, ACTIVE PATH LENGTH  ### FIXME
# 8 INVARIANT MASS                                                   ### FIXME
# 9-255 NO NEW DETAILS
#

#######################################################
# ATHENA CONFIGURATION FILE(S)
#######################################################
include("TileD3PDMaker/configurator_Ep.py")
