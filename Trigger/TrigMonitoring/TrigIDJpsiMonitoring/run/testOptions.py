# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#set DB by hand
#ConfForMC = False
ConfForMC = True

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
#GlobalFlags.DetGeo.set_atlas()
if (ConfForMC):
    #GlobalFlags.DataSource.set_geant4()
    globalflags.DetGeo.set_Value_and_Lock('geant4')
else :
    #GlobalFlags.DataSource.set_data()
    globalflags.DataSource.set_Value_and_Lock('data')
from IOVDbSvc.CondDB import conddb

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#athenaCommonFlags.FilesInput = [ "/afs/cern.ch/work/c/cgutscho/triggerFiles/data12_8TeV.00209084.physics_Muons.merge.AOD.f473_m1213._lb0072._0001.1" ]

#athenaCommonFlags.FilesInput = [ "/afs/cern.ch/work/c/cgutscho/triggerFiles/data12_8TeV.00211620.physics_Egamma.merge.AOD.f479_m1228._lb0917._0001.1" ]

# Zmm MC:

#athenaCommonFlags.FilesInput = [ "/afs/cern.ch/user/c/cgutscho/workspace/triggerFiles/mc10_14TeV.106047.PythiaZmumu_no_filter.recon.AOD.e662_s1107_d459_r2037.AOD.root.1" ]

# Muon data12 skim:

"""
athenaCommonFlags.FilesInput = [
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00200.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00250.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00201.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00251.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00202.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00252.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00203.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00253.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00204.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00254.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00205.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00255.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00206.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00256.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00207.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00257.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00208.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00258.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00209.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00259.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00210.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00260.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00211.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00261.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00212.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00262.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00213.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00263.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00214.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00264.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00215.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00265.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00216.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00266.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00217.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00267.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00218.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00268.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00219.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00269.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00220.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00270.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00221.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00271.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00222.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00272.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00223.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00273.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00224.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00274.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00225.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00275.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00226.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00276.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00227.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00277.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00228.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00278.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00229.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00279.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00230.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00280.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00231.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00281.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00232.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00282.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00233.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00283.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00234.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00284.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00235.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00285.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00236.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00286.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00237.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00287.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00238.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00288.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00239.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00289.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00240.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00290.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00241.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00291.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00242.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00292.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00243.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00293.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00244.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00294.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00245.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00295.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00246.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00296.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00247.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00297.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00248.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00298.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00249.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkim/user.cgutscho.008429._00299.Ztomumu0.AOD.pool.root"
]
"""

# Muon data12 inefficient events:

"""
athenaCommonFlags.FilesInput = [
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkimIneffMu/user.cgutscho.008821._00001.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkimIneffMu/user.cgutscho.008821._00008.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkimIneffMu/user.cgutscho.008821._00002.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkimIneffMu/user.cgutscho.008821._00009.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkimIneffMu/user.cgutscho.008821._00003.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkimIneffMu/user.cgutscho.008821._00010.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkimIneffMu/user.cgutscho.008821._00004.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkimIneffMu/user.cgutscho.008821._00011.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkimIneffMu/user.cgutscho.008821._00005.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkimIneffMu/user.cgutscho.008821._00012.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkimIneffMu/user.cgutscho.008821._00006.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkimIneffMu/user.cgutscho.008821._00013.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkimIneffMu/user.cgutscho.008821._00007.Ztomumu0.AOD.pool.root",
  "/afs/cern.ch/work/c/cgutscho/trigger/mergeSkimIneffMu/user.cgutscho.008821._00014.Ztomumu0.AOD.pool.root",
]
"""

# Egamma data12 files:

"""
athenaCommonFlags.FilesInput = [
"/afs/cern.ch/work/c/cgutscho/triggerFiles/data12_8TeV.00211620.physics_Egamma.merge.AOD.f479_m1228._lb0813._0001.1",
"/afs/cern.ch/work/c/cgutscho/triggerFiles/data12_8TeV.00211620.physics_Egamma.merge.AOD.f479_m1228._lb0898._0001.1",
"/afs/cern.ch/work/c/cgutscho/triggerFiles/data12_8TeV.00211620.physics_Egamma.merge.AOD.f479_m1228._lb0917._0001.1",
"/afs/cern.ch/work/c/cgutscho/triggerFiles/data12_8TeV.00211937.physics_Egamma.merge.AOD.f479_m1228._lb0623._0001.1",
"/afs/cern.ch/work/c/cgutscho/triggerFiles/data12_8TeV.00212144.physics_Egamma.merge.AOD.f479_m1228._lb0296._0001.1",
"/afs/cern.ch/work/c/cgutscho/triggerFiles/data12_8TeV.00212172.physics_Egamma.merge.AOD.f479_m1228._lb0503._0001.1",
"/afs/cern.ch/work/c/cgutscho/triggerFiles/data12_8TeV.00211620.physics_Egamma.merge.AOD.f479_m1228._lb0837._0001.1",
"/afs/cern.ch/work/c/cgutscho/triggerFiles/data12_8TeV.00211620.physics_Egamma.merge.AOD.f479_m1228._lb0838._0001.1",
"/afs/cern.ch/work/c/cgutscho/triggerFiles/data12_8TeV.00211620.physics_Egamma.merge.AOD.f479_m1228._lb0839._0001.1",
"/afs/cern.ch/work/c/cgutscho/triggerFiles/data12_8TeV.00211620.physics_Egamma.merge.AOD.f479_m1228._lb0842._0001.1",
"/afs/cern.ch/work/c/cgutscho/triggerFiles/data12_8TeV.00211620.physics_Egamma.merge.AOD.f479_m1228._lb0843._0001.1",
"/afs/cern.ch/work/c/cgutscho/triggerFiles/data12_8TeV.00211620.physics_Egamma.merge.AOD.f479_m1228._lb0844._0001.1",
"/afs/cern.ch/work/c/cgutscho/triggerFiles/data12_8TeV.00211620.physics_Egamma.merge.AOD.f479_m1228._lb0840-lb0841._0001.1",
]
"""

#mydir = "/afs/cern.ch/work/c/cgutscho/triggerFiles/temp/mc10_14TeV.106047.PythiaZmumu_no_filter.recon.AOD.e662_s1107_d459_r2037_tid285233_00/"
"""
athenaCommonFlags.FilesInput = [ 
  mydir + "AOD.285233._000010.pool.root.1",
  mydir + "AOD.285233._000014.pool.root.1",
  mydir + "AOD.285233._000018.pool.root.1",
  mydir + "AOD.285233._000012.pool.root.1",
  mydir + "AOD.285233._000015.pool.root.1",
  mydir + "AOD.285233._000019.pool.root.1",
  mydir + "AOD.285233._000013.pool.root.1",
  mydir + "AOD.285233._000016.pool.root.1",
]
"""

# Athena-Aware NTuple making Tools
#CBNTAthenaAware = True
#include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
#include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")
#athenaCommonFlags.PoolInputQuery.set_Value_and_Lock("") # Needed for TAG jobs (as is)

from RecExConfig.RecFlags import rec
rec.doTrigger.set_Value_and_Lock(False) # leave false; nothing to do with trigger analysis
# include your algorithm job options here
rec.doESD.set_Value_and_Lock(False) # leave false; nothing to do with trigger analysis
rec.doAOD.set_Value_and_Lock(False)
rec.doFloatingPointException.set_Value_and_Lock(True)
rec.UserAlgs.set_Value_and_Lock("RunJpsimumu.py")
#rec.UserAlgs.set_Value_and_Lock("InDetBeamSpotReader_jobOptions.py")

from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotReader as InDetBeamSpotReader

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += InDetBeamSpotReader()

# Jiri's suggestions:
#topSequence.InTimeAntiKt4TruthJetsFixIt.Enable=False;
#topSequence.InTimeAntiKt4TruthJetsSwapper.Enable=False;
#topSequence.OutOfTimeAntiKt4TruthJetsFixIt.Enable=False;
#topSequence.OutOfTimeAntiKt4TruthJetsSwapper.Enable=False;
        

from IOVDbSvc.CondDB import conddb

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
#conddb.Folders+=[ "INDET"   + "/Indet/Beampos" + "<tag>nominal</tag>"]
IOVDbSvc = Service("IOVDbSvc")
# IOVDbSvc.overrideTags += ["<prefix>/Indet/Beampos</prefix> <tag>nominal</tag>"]
# IOVDbSvc.overrideTags += ["<prefix>/Indet/Beampos</prefix>"]
# IOVDbSvc.dbConnection  = "impl=cool;techno=sqlite;schema=beamspot.db;X:TESTCOOL"
#IOVDbSvc.Folders += ["<dbConnection>impl=cool;techno=sqlite;schema=beamspot.db;X:BEAMSPOT</dbConnection> /Indet/Beampos<tag>nominal</tag>"]
#IOVDbSvc.Folders += ["<dbConnection>sqlite://;schema=beamspot.db;dbname=BEAMSPOT</dbConnection> /Indet/Beampos<tag>nominal</tag>"]


# Output log setting; this is for the framework in general
# You may over-ride this in your job options for your algorithm
rec.OutputLevel.set_Value_and_Lock(WARNING);
#rec.OutputLevel.set_Value_and_Lock(DEBUG);
# Write settings; keep all of these to false.
# Control the writing of your own n-tuple in the alg's job options
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doHist.set_Value_and_Lock (False)
###beam spot!!!
#include("InDetBeamSpotReader_jobOptions.py")
###
# main jobOption - must always be included
#include("RecJobTransforms/UseOracle.py") # DB access
include("RecExCommon/RecExCommon_topOptions.py")
# Following 3 lines needed for TAG jobs (as is)
#svcMgr.EventSelector.RefName= "StreamAOD"
#svcMgr.EventSelector.CollectionType="ExplicitROOT"
#svcMgr.EventSelector.Query = ""
#svcMgr.EventSelector.SkipEvents = 15400
theApp.EvtMax = -1 # number of event to process
#theApp.EvtMax = 500 # number of event to process
# Stops writing of monitoring ntuples (big files)
#from PerfMonComps.PerfMonFlags import jobproperties as jp
#jp.PerfMonFlags.doMonitoring = False
#jp.PerfMonFlags.doFastMon = False
