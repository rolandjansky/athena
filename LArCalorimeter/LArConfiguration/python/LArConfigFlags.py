# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaConfiguration.Enums import FlagEnum
from AthenaCommon.Logging import logging

class RawChannelSource(FlagEnum):
     Input="Input"           # read from the input-file, bytestream or RDO
     Calculated="Calculated" #re-computed by the offline LArRawChannelBuilder
     Both="Both"             # overwrite the digits computed


def createLArConfigFlags():
    lcf=AthConfigFlags()

    lcf.addFlag("LAr.doAlign",lambda prevFlags : prevFlags.GeoModel.Layout=="atlas")
    lcf.addFlag("LAr.doHVCorr",lambda prevFlags : not prevFlags.Input.isMC)
    lcf.addFlag("LAr.doCellEmMisCalib",lambda prevFlags : prevFlags.Input.isMC)

    lcf.addFlag("LAr.RawChannelSource",_determineRawChannelSource,enum=RawChannelSource)

    lcf.addFlag("LAr.doCellNoiseMasking",True)
    lcf.addFlag("LAr.doCellSporadicNoiseMasking",True)
    lcf.addFlag("LAr.doBadFebMasking",lambda prevFlags : not prevFlags.Input.isMC)

    # Include MC shape folder
    lcf.addFlag("LAr.UseMCShape", True)
    # Name of sqlite file containing Electronic Calibration values
    lcf.addFlag("LAr.ElecCalibSqlite", "")
    # Load Electronic Calibration constants
    lcf.addFlag("LAr.LoadElecCalib", True)
    # Folder name for Optimal Filtering coefficients (empty means default)
    lcf.addFlag("LAr.OFCShapeFolder", "")
    # Load conditions with this `run-number' string
    lcf.addFlag("LAr.ForceIOVRunNumber", "")
    # Include Shape folder
    lcf.addFlag("LAr.UseShape", True)
    # DataBase server string
    lcf.addFlag("LAr.DBConnection", "")

    # Do pileup OFC optimization
    lcf.addFlag("LAr.ROD.DoOFCPileupOptimization",False)
    # Number of collisions to optimize OFC for pileup
    lcf.addFlag("LAr.ROD.NumberOfCollisions",0.)
    # Number of samples in LAr digitization + ROD emulation
    lcf.addFlag("LAr.ROD.nSamples", 5)
    # Index of first sample in LAr digitization + ROD emulation
    lcf.addFlag("LAr.ROD.FirstSample", 0)
    # Force using the highest gain autocorrelation function
    # when doing OFC optimization
    lcf.addFlag("LAr.ROD.UseHighestGainAutoCorr", False)
    # Flag not to use pileup noise neither average constrain in EMB and EMEC-OW,
    # and both pileup noise and average constrain everywhere else
    lcf.addFlag("LAr.ROD.DoOFCMixedOptimization", False)
    ### option to use average constraint in OFC computation
    ### 0 = not use Delta, 1 = only EMECIW/HEC/FCAL, 2 = all , 3 = only EMECIW
    lcf.addFlag("LAr.ROD.UseDelta", 0)
    # Force using the iterative OFC procedure
    lcf.addFlag("LAr.ROD.forceIter",False)
    # NN based energy reconstruction
    lcf.addFlag("LAr.ROD.NNRawChannelBuilding", False)
    lcf.addFlag("LAr.ROD.nnJson", "")
    lcf.addFlag("LAr.ROD.nnOutputNode", "")
    lcf.addFlag("LAr.ROD.nnInputNode", "")

    ##NoisyRO flags
    # cell quality cut
    lcf.addFlag("LAr.NoisyRO.CellQuality", 4000)
    # Number of channels above quality cut
    lcf.addFlag("LAr.NoisyRO.BadChanPerFEB", 30)
    # Number of Bad FEBs per partition cut
    lcf.addFlag("LAr.NoisyRO.BadFEBCut", 5)
    # Number of channels to declare MNB-Loose
    lcf.addFlag("LAr.NoisyRO.MNBLooseCut", 5)
    # Number of channels to declare MNB-Tight
    lcf.addFlag("LAr.NoisyRO.MNBTightCut", 17)
    # Number of channels to declare MNB-Tight wir PS veto
    lcf.addFlag("LAr.NoisyRO.MNBTight_PsVetoCut", [13,3])

    return lcf


_lArRunInfo=None

def _getLArRunInfo(prevFlags):
    log = logging.getLogger('LArConfigFlags.getLArRunInfo')
    global _lArRunInfo #Cache of lar run info
    if _lArRunInfo is None:
        from LArConditionsCommon.LArRunFormat import getLArFormatForRun
        runnbr=prevFlags.Input.RunNumber[0] #If more than one run, assume config for first run is valid for all runs
        dbStr="COOLONL_LAR/"+prevFlags.IOVDb.DatabaseInstance
        _lArRunInfo=getLArFormatForRun(run=runnbr,connstring=dbStr)
        log.info("Got LArRunInfo for run %d",runnbr)
    return _lArRunInfo


def _determineRawChannelSource(prevFlags):
    log = logging.getLogger('LArConfigFlags.determineRawChannelSource')
    if prevFlags.Input.isMC or prevFlags.Overlay.DataOverlay:
        return RawChannelSource.Input

    lri=_getLArRunInfo(prevFlags)
    #runType: 0=RawData, 1=RawDataResult, 2=Result
    if lri is None or lri.runType is None:
        log.warning("WARNING do not have LArRunInfo !")
        return RawChannelSource.Both
    log.info("runType %d",lri.runType())
    if (lri.runType()==0):
        return RawChannelSource.Calculated #Have only digits in bytestream
    elif (lri.runType()==1):
        return RawChannelSource.Both       #Have both, digits and raw-channels in bytestream
    elif (lri.runType()==2):
        return RawChannelSource.Input      #Have only raw-channels in bytestream
    else:
        log.warning("Unknown LAr run type %i",lri.runType())
        return RawChannelSource.Both
