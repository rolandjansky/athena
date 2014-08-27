from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BFieldFlags import jobproperties
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.DetFlags import DetFlags

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs as recAlgsFlags
from RecExConfig.RecConfFlags import recConfFlags

# in Standalone mode, don't allow any configuration errors
athenaCommonFlags.AllowIgnoreConfigError = False

from MuonRecExample.MuonRecFlags import muonRecFlags,mooreFlags,muonboyFlags
from MuonRecExample.DataFiles import DataInfo,DataFilesInfoMap
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags

# configure all flags to run all Subset + Muon Combined
import MuonCombinedRecExample.MuonCombinedRecOnlySetup


# Run on CSC data or not
muonRecFlags.doCSCs=True

#--------------------------------------------------------------------------------
# Input setup
#--------------------------------------------------------------------------------
globalflags.DataSource.set_Value_and_Lock('data')
jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")
globalflags.InputFormat.set_Value_and_Lock('bytestream')


################################################################################
# Data 2009 without CSCs
muonRecFlags.doCSCs = False
athenaCommonFlags.BSRDOInput.set_Value_and_Lock(['/castor/cern.ch/grid/atlas/DAQ/2009/00121080/physics_RPCwBeam/data09_cos.00121080.physics_RPCwBeam.daq.RAW._lb0000._SFO-5._0605.data'])
#athenaCommonFlags.BSRDOInput.set_Value_and_Lock(['/castor/cern.ch/grid/atlas/DAQ/2009/00121064/physics_RPCwBeam/data09_cos.00121064.physics_RPCwBeam.daq.RAW._lb0000._SFO-5._0002.data'])
# set the following to override the auto-config
#globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-08-00-00')
#jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
#jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
#jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
#globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-REPC-002-03')
# Use auto-config with caution! It is tuned to Summer 2009 reprocessing.
rec.triggerStream = "RPCwBeam"
rec.AutoConfiguration = ['FieldAndGeo', 'ConditionsTag']
#rec.AutoConfiguration = ['TriggerStream','FieldAndGeo', 'ConditionsTag'] # ConditionsTag requires TriggerStream, which is broken

################################################################################
## # Data 2008 with CSCs
## muonRecFlags.doCSCs=True
## athenaCommonFlags.BSRDOInput.set_Value_and_Lock([
##         '/castor/cern.ch/grid/atlas/DAQ/2008/91897/physics_TGCwBeam/daq.ATLAS.0091897.physics.TGCwBeam.LB0001.SFO-1._0001.data'])
## globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEONF-04-00-00')
## jobproperties.BField.solenoidOn.set_Value_and_Lock(False)
## jobproperties.BField.barrelToroidOn.set_Value_and_Lock(False)
## jobproperties.BField.endcapToroidOn.set_Value_and_Lock(False)
## globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-REPC-001-02')


################################################################################
## # Data 2008 without CSCs
## muonRecFlags.doCSCs=False
## athenaCommonFlags.BSRDOInput.set_Value_and_Lock([
##     '/castor/cern.ch/grid/atlas/atlasdatadisk/data08_cos/RAW/data08_cos.00091060.physics_RPCwBeam.daq.RAW.o4/daq.ATLAS.0091060.physics.RPCwBeam.LB0032.SFO-1._0013.data'])
##     #        '/castor/cern.ch/grid/atlas/atlasdatadisk/data08_cos/RAW/data08_cos.00091060.physics_RPCwBeam.daq.RAW.o4/daq.ATLAS.0091060.physics.RPCwBeam.LB0001.SFO-1._0001.data'])
##     #        '/castor/cern.ch/grid/atlas/DAQ/2008/87863/physics_BPTX/daq.NoTag.0087863.physics.BPTX.LB0000.SFO-1._0001.data'])
## # WARNING: the files on that atlsdatadisk may disappear after some time
## # WARNING: the STAGE_SVCCLASS below is only needed for files on atlasdatadisk
## if athenaCommonFlags.BSRDOInput()[0].startswith("/castor/cern.ch/grid/atlas/atlasdatadisk/"):
##    import os
##    os.environ["STAGE_SVCCLASS"]='atldata'
## globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEONF-04-00-00')
## jobproperties.BField.solenoidOn.set_Value_and_Lock(False)
## jobproperties.BField.barrelToroidOn.set_Value_and_Lock(False)
## jobproperties.BField.endcapToroidOn.set_Value_and_Lock(False)
## globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-REPC-001-03')



#--------------------------------------------------------------------------------
# Output setup
#--------------------------------------------------------------------------------
rec.doTruth=False
#rec.doCBNT=False
#rec.doWriteESD = True
#rec.doWriteAOD = True
#muonRecFlags.doCalibNtuple = False

#--------------------------------------------------------------------------------
# Reco setup
#--------------------------------------------------------------------------------

## Muon Standalone Reco
#muonRecFlags.doMuonboy = False  # switch on or off Muonboy (if off, then MuTag/Staco switched off too)
#muonRecFlags.doMoore = False    # switch on or off Moore   (if off, then Muid* switched off too)

## Switch on/off groups of algorithms that go into the same MuonCollection
#muonCombinedRecFlags.doStacoMuonCollection = False
#muonCombinedRecFlags.doMuidMuonCollection = False
#muonCombinedRecFlags.doCaloMuonCollection = False
#muonCombinedRecFlags.doMuGirlLowBetaMuonCollection = False

## Algs for the StacoMuonCollection
#muonCombinedRecFlags.doMboyCombined = False # don't extrapolate Muonboy standalone tracks to the IP
#muonCombinedRecFlags.doStaco = False # don't run Staco (nor MuTag)
#muonCombinedRecFlags.doMuTag = False # don't run MuTag

## Algs for MuidMuonCollection
#muonCombinedRecFlags.doMuidStandalone = False
#muonCombinedRecFlags.doExtrapolateMuonToIP = False # don't extrapolate Moore standalone tracks to the IP
#muonCombinedRecFlags.doMuidCombined = False
#muonCombinedRecFlags.doCombinedMuonFit = False
#muonCombinedRecFlags.doStacoMoore = False  # run Staco on Moore+InDet tracks (runs independent of Muonboy+Staco)
#muonCombinedRecFlags.doMuGirl = False
#muonCombinedRecFlags.doMuTagMoore = False # run MuTag on Moore segments (and veto Moore tracks?)
#muonCombinedRecFlags.doMuTagIMO = False
#muonCombinedRecFlags.doMuidVertexAssociator = False

## Algs for CaloMuonCollection
#muonCombinedRecFlags.doCaloTrkMuId = False # don't run CaloTrkMuId algorithm

## Algs for MuGirlLowBetaMuonCollection
#muonCombinedRecFlags.doMuGirlLowBeta = False

## Other Algs
#muonCombinedRecFlags.doLArMuId = False # don't run LArMuid algorithm




#rec.doTrigger = True


#--------------------------------------------------------------------------------
# General setup
#--------------------------------------------------------------------------------
rec.doNameAuditor = True
#muonRecFlags.doVP1 = True    # Decide whether to run Virtual Point1 graphical event display

# Read geometry alignment corrections from DB
#muonRecFlags.useAlignmentCorrections = True


muonCombinedRecFlags.Commissioning = True

# backwards compat
rec.Commissioning = muonCombinedRecFlags.Commissioning()

##### no more flags after this line #####
try:
    include("MuonRecExample/MuonRec_topOptions.py")
    ###### put any user finetuning after this line #####
#    topSequence.MergeMuonCollections.OutputLevel = DEBUG
#    topSequence.MakeAODMuons.OutputLevel = DEBUG
#    topSequence.MakeAODMuons.doMuonCleaning = True

    ###### put any user finetuning before this line #####
except:
    # print the stacktrace (saving could fail, and would then obscure the real problem)
    import traceback
    print traceback.format_exc().rstrip()
    
    # always write config so far for debugging
    from AthenaCommon.ConfigurationShelve import saveToAscii
    saveToAscii("config.txt")
    # add DetFlags
    from MuonRecExample.MuonRecUtils import dumpDetFlags
    dumpDetFlags("config.txt")
    # but still exit with error
    import sys
    sys.exit(10)
else:
    # and write config to include user changes after topOptions
    from AthenaCommon.ConfigurationShelve import saveToAscii
    saveToAscii("config.txt")
    # add DetFlags
    from MuonRecExample.MuonRecUtils import dumpDetFlags
    dumpDetFlags("config.txt")
