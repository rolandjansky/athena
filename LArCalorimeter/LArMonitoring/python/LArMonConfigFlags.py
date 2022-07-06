#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##
## @file LArMonitoring/python/LArMonConfigFlags.py
## @purpose Python module to hold flags to configure LArMonitoring
##

""" LArMonConfigFlags
    Python module to hold flags for LAr DQ monitoring

"""

__author__ = "P. Strizenec"
__version__= "$Revision: 0.1 $"
__doc__    = "create LArMonConfigFlags"

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createLArMonConfigFlags():
    acf=AthConfigFlags()
    acf.addFlag('LArMon.LArDigitKey', 'FREE')
    acf.addFlag('LArMon.EventBlockSize', 0)
    # individual algos
    acf.addFlag('LArMon.doLArRawChannelMon', False)
    acf.addFlag('LArMon.doLArCollisionTimeMon', True)
    acf.addFlag('LArMon.doLArAffectedRegions', True)
    acf.addFlag('LArMon.doLArFEBMon', True)
    acf.addFlag('LArMon.doLArHVCorrectionMon', True)
    acf.addFlag('LArMon.doLArCosmicsMon', False)
    acf.addFlag('LArMon.doLArCoverage', True)
    acf.addFlag('LArMon.doLArDigitMon', True)
    acf.addFlag('LArMon.doLArNoisyROMon', True)
    acf.addFlag('LArMon.doLArRODMon', True)
    acf.addFlag('LArMon.doLArNoiseCorrelationMon', False)
    # and individual parameters
    acf.addFlag('LArMon.doLArRawMonitorSignal', False)
    return acf
