# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# Set Flags for Running the Trigger in readBS Mode
# includes Trigger_topOptions.py to do the real work.
# Reads from file RawEvent.re (file must already exits
# run athena with TrigWriteBS_Flags.py to produce it)
# 
# use:
# athena.py -bs testEFIDreadBS_Flags.py
#==============================================================

#onlyID=True
doTrigger=True
AllAlgs=False

readBS=True
# Set detector description
DetDescrVersion = "ATLAS-DC3-02"

doCBNT=False
doWriteAOD=False
doWriteESD=False
doWriteTAG=False
doAOD=False # make AOD objects
doESD=False # make ESD objects
doTAG=False

#useROOTNtuple=True # False : hbook ntuple

# Set Truth flags
doTruth=False    # needs to be true if creating Fake RoI 

include ("TriggerRelease/TriggerFlags.py")
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False

TriggerFlags.JetSlice.unsetAll()
TriggerFlags.BjetSlice.unsetAll()
TriggerFlags.BphysicsSlice.unsetAll()
TriggerFlags.TauSlice.unsetAll()
TriggerFlags.MuonSlice.unsetAll()
#TriggerFlags.EgammaSlice.setL2()
TriggerFlags.EgammaSlice.setEF()

#TriggerFlags.doHLTpersistency=False
#TriggerFlags.fakeLVL1=False
#TriggerFlags.doLVL1=False
#TriggerFlags.readLVL1Calo=True
#TriggerFlags.doNtuple=True
#TriggerFlags.useOfflineSpacePoints=False
#TriggerFlags.readLVL1configFromXML=True
#TriggerFlags.doHypo=False
#TriggerFlags.doID=True
#TriggerFlags.doCalo=True
#TriggerFlags.doEF=True
#TriggerFlags.doMuon=False

#TriggerFlags.EgammaSlice.doL2Calo=True
#TriggerFlags.EgammaSlice=TriggerSliceFlags(False,True,False,True,False,False) #L2TRK,L2Calo,L2mu,EFtrk,EFCalo,EFmuon

TriggerFlags.doMuon=False
TriggerFlags.doHypo=False

triggerMenu.disableAll()
triggerMenu.enableSignature("e10")
TriggerFlags.EgammaSlice.setFlag("doIDSCAN",False)
TriggerFlags.EgammaSlice.setFlag("doSiTrack",False)
TriggerFlags.EgammaSlice.setFlag("doTRTxK", False)

#-------------------------------------------------------------
# End of setting flags
#-------------------------------------------------------------

include( "TriggerRelease/TrigReadBS_topOptions.py" )

MessageSvc.OutputLevel = DEBUG
#NovaCnvSvc.OutputLevel = INFO
MessageSvc.debugLimit = 10000000

# For BS files produced with DetDescrVersion "ATLAS-DC3-01" or "ATLAS-DC3-02"
# uncomment the following line (but don't do that if using a file produced
# by re-running LVL1 with an offline release 12 or higher).
# Algorithm("Lvl1Conversion_L2").useOldRoIWordFormat=True # fix to read data with old RoiWord format
