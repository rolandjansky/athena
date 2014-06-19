# $Id: xAODElectronReader_jobOptions.py 583111 2014-02-13 17:31:49Z christos $

# Set up the reading of the input xAOD:
#FNAME = "/afs/cern.ch/work/j/jwalder/data/forTrigger/mc12_14TeV.208214.Pythia8B_AU2_CTEQ6L1_bb_Jpsi_mu5p5mu5p5.recon.RDO.e2238_s1682_s1691_r4845_tid01332753_00/RDO.01332753._000009.pool.root.1"
FNAME = "/afs/cern.ch/work/j/jwalder/data/forTrigger/mc12_8TeV.208010.Pythia8B_AU2_CTEQ6L1_pp_Jpsimu6p5mu6p5.merge.AOD.e1966_a188_a180_r3549_tid01353548_00/AOD.01353548._001293.pool.root.1"

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the xAOD reader algorithm(s):

from xAODTrigBphysCnv.xAODTrigBphysCnvConf import xAODMaker__TrigEFBphysCnvAlg as efalg
from xAODTrigBphysCnv.xAODTrigBphysCnvConf import xAODMaker__TrigL2BphysCnvAlg as l2alg
from xAODTrigBphysCnv.xAODTrigBphysCnvConf import xAODMaker__TrigxAODBphysReaderAlg as xAODReader


#alg = xAODReader__ElectronReader()
#alg.OutputLevel = DEBUG
#theJob += alg

AOD_containers_EF = ['HLT_EFBMuMuFex']
AOD_containers_L2 = ['HLT_L2BMuMuFex','HLT_L2DiMuXFex']

for obj in AOD_containers_EF:
    alg = efalg(name='TrigEFBphysCnvAlg_'+obj,
		AODKey=obj,
                xAODKey = 'xAOD'+obj,
                OutputLevel = VERBOSE)
    theJob += alg


for obj in AOD_containers_L2:
    alg = l2alg(name='TrigL2BphysCnvAlg_'+obj,
		AODKey=obj,
                xAODKey = 'xAOD'+obj,
                OutputLevel = VERBOSE)
    theJob += alg

for obj in AOD_containers_L2+AOD_containers_EF:
    alg = xAODReader(name='BPhysReader_'+obj,
                     OutputLevel = VERBOSE,
                     xAODKey = 'xAOD'+obj
                     )
    theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
