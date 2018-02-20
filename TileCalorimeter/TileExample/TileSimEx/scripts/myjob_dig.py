# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#==============================================================
#
# Configuration of Job options file for Digitization
#
# The original copy of this job option is ../share/jobOptions_TileTB_Dig.py,
# This skeleton is modified to loop over particle types, beam energies,
# and physics lists in sub_jobs.sha to setup the output file names.
#
# This job option will call ../share/jobOptions_Tile_Dig.py,
#==============================================================

TileTB=True
doCBNT=True

if not 'doHitNtuple' in dir():
    doHitNtuple = True

if not 'doTileNtuple' in dir():
    doTileNtuple = True

if not 'doRDO' in dir():
    doRDO = False

# Setup FileSuffix for the outpu files
# EBEAMENERGY, PARTICLENAME, PHYSICSLIST will be replaced in sub_jobs.sh.
if not 'FileSuffix' in dir():
    FileSuffix='90-EBEAMENERGY-PARTICLENAME_PHYSICSLIST'

# TileCal standalone setup with 2 barrels and 2 ext.barrels on top is the default setup in this simulation.
if not 'Geo' in dir():
    Geo = '2B2EB'
# Call Job options file for Digitization
include("TileSimEx/jobOptions_Tile_Dig.py")

