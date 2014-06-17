AllAlgs = False # if false, all algorithms are switched off by defaults 
#DetDescrVersion = 'ATLAS-CSC-01-00-00'
#DetDescrVersion = 'ATLAS-CSC-01-01-00'
#DetDescrVersion = "ATLAS-CSC-01-02-00"
#DetDescrVersion = "ATLAS-CSC-02-01-00"
#DetDescrVersion = "ATLAS-CSC-02-00-00"
#DetDescrVersion = "ATLAS-CommNF-05-00-00"
#DetDescrVersion = "ATLAS-GEO-00-00-00"
#DetDescrVersion = "ATLAS-GEO-01-00-00"
#DetDescrVersion = "ATLAS-GEO-02-00-00"
#DetDescrVersion = "ATLAS-GEO-02-00-00"
#DetDescrVersion = "ATLAS-GEO-03-00-00"
#DetDescrVersion = "ATLAS-GEO-04-00-00"
#DetDescrVersion = "ATLAS-TEST"
DetDescrVersion = "ATLAS-GEO-08-00-00"
#DetDescrVersion = "ATLAS-GEONTF-05-00-00"

#
doWriteESD=False # uncomment if do not write ESD
doWriteAOD=False # uncomment if do not write AOD
doAOD=False # uncomment if do not run AOD making algorithms
doWriteTAG=False # uncomment if do not write TAG
#
doMissingET=False
#
doMoore=False
doMuonIDStandAlone=False
doMuonIDCombined=False
doMuGirl=False
doStaco=False
doMuTag=False
#
doCBNT=False
doTrigger = False # for example do not run trigger simulation
doESD=False # uncomment if do not run ESD making algorithms
doWriteESD=False # uncomment if do not write ESD
doAOD=False # uncomment if do not run AOD making algorithms
doWriteAOD=False # uncomment if do not write AOD
doWriteTAG=False # uncomment if do not write TAG
# DetFlags modifications are best set here (uncomment RecExCommon_flags first)
include ("RecExCommon/RecExCommon_flags.py")
# switch off ID, calo, or muons
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

PoolRDOInput=["/afs/cern.ch/atlas/project/muon/data/csc12/misal1_csc11.005145.PythiaZmumu.digit.RDO.v12003101_tid003501._00001.pool.root.1"]

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

#--------------------------------------------------------------
# MagField
#  Ignore la diff entre tag simu et Tag reco
#GeoModelSvc.IgnoreTagDifference = True
ServiceMgr.GeoModelSvc.IgnoreTagDifference = True

#--------------------------------------------------------------
# BFieldAth
#--------------------------------------------------------------
include( "BFieldAth/BFieldAth_jobOptions.py" )
###MagFieldAthenaSvc.NameOfTheSource = "BYPYTH"  
###MagFieldAthenaSvc.dx = 0.001  
###MagFieldAthenaSvc.NameOfTheBfieldFile = "bmagatlas05_test2_10250Amp.data" ;
###MagFieldAthenaSvc.NameOfTheBfieldFile = "bmagatlas05_test2_NoSolenoid.data" ;
###MagFieldAthenaSvc.NameOfTheBfieldFile = "bmagatlas04_test1_WithNoToroid_OnlySolAndIron.data" ;
###MagFieldAthenaSvc.SetupCOOL=True
###MagFieldAthenaSvc.NameOfTheSource = "COOL"  
#-----------------------------------------------------------------
# use DCS only if not running online
ServiceMgr.MagFieldAthenaSvc.NameOfTheSource='COOL'
from BFValidation.BFValidationConf import FastField
topSequence += FastField( "MyFastField" )

