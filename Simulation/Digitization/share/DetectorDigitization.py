from __future__ import print_function
##############################################################
#
# Job options file to configure:
#        Digitization
#        LVL1 Simulation
#        ByteStream conversion
#
# Author: Davide Costanzo
# Contacts:
#      Inner Detector:   Davide Costanzo
#      LArCalorimeter:   Guillaume Unal
#      TileCalorimter:   Sasha Solodkov
#      MuonSpectrometer: Daniela Rebuzzi, Ketevi Assamagam
#      LVL1 Simulation:  Tadashi Maeno
#      ByteStream:       Hong Ma
#
#==============================================================
#

# Set up PileupMergeSvc used by subdetectors
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon import CfgGetter

# Set up ComTimeRec for cosmics digitization
from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Beam.beamType == "cosmics" :
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    from CommissionRec.CommissionRecConf import ComTimeRec
    comTimeRec = ComTimeRec("ComTimeRec")
    topSequence += comTimeRec

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Convert old legacy EventInfo if needed
from Digitization.DigitizationFlags import digitizationFlags
if 'LegacyEventInfo' in digitizationFlags.experimentalDigi() and \
    not (DetFlags.pileup.any_on() or digitizationFlags.doXingByXingPileUp()):
    from xAODEventInfoCnv.xAODEventInfoCnvAlgDefault import xAODEventInfoCnvAlgDefault
    xAODEventInfoCnvAlgDefault (sequence = job)

# Beam spot
include( "Digitization/BeamSpot.py" )

# Configure main algorithm
job += CfgGetter.getAlgorithm(digitizationFlags.digiSteeringConf.get_Value(), tryDefaultConfigurable=True)
if 'doFastPixelDigi' in digitizationFlags.experimentalDigi() or 'doFastSCT_Digi' in digitizationFlags.experimentalDigi() or 'doFastTRT_Digi' in digitizationFlags.experimentalDigi():
    print ("WARNING  Setting doFastPixelDigi ,doFastSCT_Digi or doFastTRT_Digi in digitizationFlags.experimentalDigi no longer overrides digitizationFlags.digiSteeringConf.")
elif 'doSplitDigi' in digitizationFlags.experimentalDigi():
    print ("WARNING  Setting doSplitDigi in digitizationFlags.experimentalDigi no longer overrides digitizationFlags.digiSteeringConf. Use --digiSteeringConf 'Split' on the command-line instead.")


# MC Truth info
#if DetFlags.Truth_on():
#    include( "Digitization/TruthDigitization.py" )

# Forward Detectors
#if DetFlags.Forward_on():
#    include( "Digitization/FwdDetDigitization.py" )

# Inner Detector
if DetFlags.ID_on():
    include( "Digitization/InDetDigitization.py" )

# Calorimeters
if DetFlags.Calo_on():
    include( "Digitization/CaloDigitization.py" )

# Muon Digitization
if DetFlags.Muon_on():
    include( "Digitization/MuonDigitization.py" )

# LVL1 trigger simulation
#if DetFlags.digitize.LVL1_on():
include( "Digitization/LVL1Digitization.py" )
