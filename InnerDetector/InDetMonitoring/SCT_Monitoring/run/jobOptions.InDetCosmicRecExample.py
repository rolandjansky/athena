# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#####################################################
#                                                   #
# Reconstruction for cosmic ID run                  #
# Author: Ola K. Oye                                #
#                                                   #
#####################################################

include ("RecExCommon/RecExCommon_flags.py")

####### Detectors ########
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.pixel_setOff()
DetFlags.LVL1_setOff()

####### Reconstruction ###

#Turn off all reconstruction for now
makeSimpleTracks = False
doxKalman = False
doiPatRec = False
doEmCluster = False
doCaloCluster = False
doMoore = False
doMuonbox = False
doBtagging = False
doEgamma = False
doJetRec = False
doTauRec = False
doMuonID = False
doStaco = False
doMuonAlgs = False
doTileMuID = False
doMissingET = False
doJiveXML = False

#doDumpMC=True

# number of event to process
EvtMax=-1

#Input file
PoolRDOInput=["InDetCosmicDig.pool.root"]

# main jobOption, using standard RecExCommon
include ("RecExCommon/RecExCommon_topOptions.py")

# Run JiveXML only on ID
include( "JiveXML/JiveXML_jobOptionBase.py" )
include( "JiveXML/DataTypes_InDet.py" ) 

# set correct event collection for JiveXML
EventData2XML.McEventCollection="GEN_EVENT"
