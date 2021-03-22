# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#This file is to run standard reconstruction + btagging on an ESD file

from RecExConfig.RecFlags import rec
rec.doTrigger.set_Value_and_Lock(False)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PFlowTests/mc16_13TeV/mc16_13TeV.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.recon.ESD.e3569_s3170_r12310_r12253_r12310/ESD.23850840._000295.pool.root.1"]

doDumpProperties=True

#Turn of TAG
rec.doWriteTAG.set_Value_and_Lock(False)

#Disables PFO Thnning - these cotnainers do not exist, unless you run jet finding
from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False)

athenaCommonFlags.EvtMax=-1

#Run btagging 
UserAlgs = ["BTagging/btaggingAlgs.py"]

include ("RecExCommon/RecExCommon_topOptions.py")
from SGComps import AddressRemappingSvc
AddressRemappingSvc.addInputRename ("xAOD::TrackParticleContainer","InDetTrackParticles.TrackCompatibility","InDetTrackParticles.TrackCompatibility_rename")
