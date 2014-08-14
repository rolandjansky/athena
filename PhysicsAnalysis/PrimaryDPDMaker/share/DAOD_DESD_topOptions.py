##=============================================================================
## Name:        DAOD_DESD_topOptions.py
##
## Author:     Eric Feng (Chicago)  <Eric.Feng@cern.ch>
## Created:    August 2009
##
## Description: Job options to produce skimmed DPD (AOD or ESD) from MC.
##
## Usage: athena -s -c "EvtMax=100" PrimaryDPDMaker/DAOD_DESD_topOptions.py | tee log 2>&1
##
## $Id$
##=============================================================================

#include ("real_data_all_files.py")
#Turn on skimmed AOD streams 
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
#primDPD.WriteDAOD_EGLOOSEStream           = True
#primDPD.WriteDAOD_ELLOOSEStream           = True
#primDPD.WriteDAOD_MUStream                = True
#primDPD.WriteDESD_CTLFWJETStream          = True
#primDPD.WriteDESD_FWJETStream             = True
#primDPD.WriteDAOD_ZEEGAMMAStream    = True
primDPD.WriteDAOD_ZMUMUGAMMAStream    = True

#Enable MC truth
primDPD.UseMCTruth = True

#Global setup
#from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as af
#af.FilesInput.set_Value_and_Lock(file_list)
af.FilesInput=['/tmp/kliu/data12_8TeV.00200804.physics_Muons.merge.AOD.f431_m1111/data12_8TeV.00200804.physics_Muons.merge.AOD.f431_m1111._lb0211-lb0226._0001.1']
#af.FilesInput=['/tmp/kliu/data12_8TeV.00200804.physics_Egamma.merge.AOD.f431_m1111/data12_8TeV.00200804.physics_Egamma.merge.AOD.f431_m1111._lb0214-lb0226._0001.1']
#af.FilesInput=["mc11_7TeV.106047.PythiaZmumu_no_filter.merge.AOD.e815_s1272_s1274_r3043_r2993_tid645508_00/*"]
#af.FilesInput=['/afs/cern.ch/user/g/gencomm/w0/cote/data09_cos.00121512.physics_L1Calo.daq.RAW._lb0000._SFO-1._0001.data']
#af.FilesInput=['/afs/cern.ch/user/k/kkoeneke/scratch0/data/AOD/mc08.106050.PythiaZee_1Lepton.recon.AOD.e347_s462_r541_tid028686/AOD.028686._19800.pool.root.1']
#af.FilesInput = ['/mnt/data/efeng/sm/mc08.105814.JF70_pythia_jet_filter.recon.ESD.e372_s462_r617_tid041887/ESD.041887._02964.pool.root.1' ]
#af.FilesInput = ['/afs/physik.uni-freiburg.de/home/kuehn/recon4/valid1.105200.T1_McAtNlo_Jimmy.recon.AOD.e380_s494_r729_tid073737/AOD.073737._000127.pool.root.1' ]
#af.FilesInput = ['/afs/cern.ch/user/s/skuehn/testarea/test/rel_1/Reconstruction/RecExample/RecExCommon/run/AOD.pool.root' ]

#af.FilesInput = ['/mnt/data/efeng/sm/mc08.105802.JF17_pythia_jet_filter.recon.AOD.e347_s462_r617_tid041623/AOD.041623._02863.pool.root.1' ]
#af.FilesInput = [ '/mnt/data/efeng/sm/mc08.105807.JF35_pythia_jet_filter.recon.AOD.e359_s462_r635_tid046324/AOD.046324._03100.pool.root.1' ]
#af.FilesInput = ['/mnt/data/efeng/sm/mc08.105814.JF70_pythia_jet_filter.recon.AOD.e372_s462_r635_tid047114/AOD.047114._02730.pool.root.1']
#JF35 files:  AOD.046324._03111.pool.root.1, AOD.046324._03140.pool.root.1, AOD.046324._03226.pool.root.1, AOD.046324._03634.pool.root.1, AOD.046324._03761.pool.root.1, AOD.046324._03842.pool.root.1, AOD.046324._03884.pool.root.1, AOD.046324._03906.pool.root.1, AOD.046324._03984.pool.root.1
#af.FilesInput = ['/mnt/data/efeng/sm/mc08.105814.JF70_pythia_jet_filter.recon.ESD.e372_s462_r617_tid041887/ESD.041887._02964.pool.root.1' ]

#af.FilesInput = [ '/afs/cern.ch/user/s/skuehn/testarea/testprod/rel_1/Reconstruction/RecExample/RecExCommon/run/AOD.pool.root' ]

#for cf in runArgs.jobConfig:
#    include(cf)
#DetDescrVersion = runArgs.geometryVersion

#af.SkipEvents.set_Value_and_Lock(0)
if not vars().has_key('EvtMax'): EvtMax = -1 
af.EvtMax=EvtMax

from RecExConfig.RecFlags import rec
rec.AutoConfiguration=['everything']

rec.doDPD = True
#rec.DPDMakerScripts.append("PrimaryDPDMaker/PrimaryDPDMaker.py")
#rec.DPDMakerScripts.append("PrimaryDPDMaker/DAOD_EGLOOSE.py")
#rec.DPDMakerScripts.append("PrimaryDPDMaker/DAOD_MU.py")
#rec.DPDMakerScripts.append("PrimaryDPDMaker/DAOD_ELLOOSE.py")
#rec.DPDMakerScripts.append("PrimaryDPDMaker/DESD_CTLFWJET.py")
#rec.DPDMakerScripts.append("PrimaryDPDMaker/DESD_FWJET.py")
#rec.DPDMakerScripts.append("PrimaryDPDMaker/DAOD_ZEEGAMMA.py")
rec.DPDMakerScripts.append("PrimaryDPDMaker/DAOD_ZMUMUGAMMA.py")
rec.doWriteESD = False
rec.doWriteAOD = False
rec.doWriteTAG = False
rec.doHist = False
rec.doCBNT = False
#rec.doPerfMon = False

# The job starts here!
include ("RecExCommon/RecExCommon_topOptions.py")

##Final debugging options...
#ServiceMgr.StoreGateSvc.Dump = True     # Prints the content of StoreGate (EventStore) 
#ServiceMgr.MetaDataStore.Dump = True    # Prints output metadata store content
#ServiceMgr.InputMetaDataStore.Dump=True # Prints input metadata store content
