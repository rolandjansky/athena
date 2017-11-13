##=============================================================================
## Name:        PerfDPD_topOptions.py
##
## Authors:     Karsten Koeneke & David Cote (DESY)
## Created:     February 2009
##
## Description: job options for all Performance ESD->DPD.
##
## Usage: athena PrimaryDPDMaker/PerfDPD_topOptions.py 2>&1 | tee log.txt
##        Edit this file to disable DPDs or change the global setup
##
##=============================================================================

#Turn on all Performance DPD
from RecExConfig.RecFlags import rec
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
#rec.DPDMakerScripts.append("PrimaryDPDMaker/DESD_GOODTIME.py")
#rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_EGamma.py")
#rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_Jet.py")
#rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_LargeMet.py") # Need testing with new file
#rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_MinBias.py")
#rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_Muon.py")
#rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_PhotonJet.py")
#rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_SingleElectron.py")
#rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_SingleMuon.py")
#rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_Tracking.py")

#rec.DPDMakerScripts.append("PrimaryDPDMaker/D2ESD_DiPhoton.py")

# Example to check the bookkeeping
#rec.DPDMakerScripts.append("PrimaryDPDMaker/ReadBookkeepingExample.py")
rec.DPDMakerScripts.append("PrimaryDPDMaker/TestDPD.py")


#primDPD.ApplyThinning = False
#primDPD.ApplySlimming = False

if not vars().has_key('UseMCTruth'): UseMCTruth=False
primDPD.UseMCTruth=UseMCTruth


# from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
# include("/data2/magdac/DESD_SGLMU/data12_8TeV.00201494.physics_Muons.merge.AOD.f436_m1121/input.py")
# athenaCommonFlags.FilesInput = testInputFiles


#Global setup
from AthenaCommon.AthenaCommonFlags import jobproperties as jp
#jp.AthenaCommonFlags.FilesInput = ["/afs/cern.ch/user/k/kkoeneke/afsPAT/data/data.physics_Egamma.ESD.pool.root"]
#jp.AthenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-pat/data/ESD/data10_7TeV.00167607.physics_Egamma.merge.ESD.r1774_p327_tid203271_00/ESD.203271._000001.pool.root.1"]
#jp.AthenaCommonFlags.FilesInput = ["/tmp/magdac/data12_8TeV.00209109.physics_Muons.recon.ESD.f473/data12_8TeV.00209109.physics_Muons.recon.ESD.f473._lb0200._SFO-2._0001.1"]
#jp.AthenaCommonFlags.FilesInput = ["/tmp/kkoeneke/data/data12_8TeV.00209864.physics_Muons.merge.AOD.f475_m1223/data12_8TeV.00209864.physics_Muons.merge.AOD.f475_m1223._lb0153._0001.1"]

#jp.AthenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/data11_7TeV/AOD/v83_m1095/data11_7TeV.00191715.physics_Egamma.merge.AOD.v83_m1095/data11_7TeV.00191715.physics_Egamma.merge.AOD.v83_m1095._lb0076._0001.1__DQ2-1332317564"]

#jp.AthenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/data11_7TeV/AOD/v83_m1095/data11_7TeV.00191715.physics_Muons.merge.AOD.v83_m1095/data11_7TeV.00191715.physics_Muons.merge.AOD.v83_m1095._lb0077._0001.1"]
#myPath="/eos/atlas/atlascerngroupdisk/proj-pat/data/ESD/data11_7TeV.00191715.physics_Egamma.recon.ESD.v83/"
#myPath="/eos/atlas/atlascerngroupdisk/proj-pat/data/ESD/data11_7TeV.00191715.physics_Muons.recon.ESD.v83/"

myPath = os.getenv("DataMC")+"/mc/xAOD/valid2.117050.PowhegPythia_P2011C_ttbar.digit.AOD.e2657_s1933_s1964_r5534/*"
#myPath = "/atlas/scratch0/kkoeneke/DataMC/mc/xAOD/valid2.147807.PowhegPythia8_AU2CT10_Zmumu.digit.AOD.e2657_s1933_s1964_r5493_tid01466650_00/"
from glob import glob
jp.AthenaCommonFlags.FilesInput = glob(myPath)




if not vars().has_key('SKIPEVENTS'): SKIPEVENTS = 0
jp.AthenaCommonFlags.SkipEvents = SKIPEVENTS
if not vars().has_key('EVTMAX'): EVTMAX = -1
jp.AthenaCommonFlags.EvtMax = EVTMAX


# Turn off most of RecExCommon... (optional)
rec.doCBNT       = False
rec.doWriteAOD   = False
rec.doAOD        = False
rec.doWriteTAG   = False
rec.doHist       = False
rec.doDPD        = True
rec.doFileMetaData        = False

#the job starts here!
# include ("RecExCommon/RecExCommon_topOptions.py")
include ("RecExCommon/AnalysisCommon_topOptions.py")
