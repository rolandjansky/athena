# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# Job for testing the basic behaviour of xAODMaker::DynVarFixerAlg.
#

# Read the input file using very basic Athena I/O.
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [
   "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data18_13TeV.00364292.physics_Main.merge.AOD.f1002_m2037._lb0163._0006.1" ]

# Access the algoritgm sequence.
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Set up the algorithm.
from xAODCoreCnv.xAODCoreCnvConf import xAODMaker__DynVarFixerAlg
theJob += xAODMaker__DynVarFixerAlg( "DynVarFixer",
             Containers = [ "InDetTrackParticlesAux.", "CombinedMuonTrackParticlesAux.",
                            "BTagging_AntiKt4EMTopoAux.", "MuonsAux." ],
             OutputLevel = VERBOSE )

# Some extra setting(s).
theApp.EvtMax = 50
