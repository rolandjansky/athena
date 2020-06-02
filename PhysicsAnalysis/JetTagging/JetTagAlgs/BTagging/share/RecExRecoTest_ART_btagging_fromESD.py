# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#This file is to run standard reconstruction + btagging on an ESD file

from RecExConfig.RecFlags import rec
rec.doTrigger.set_Value_and_Lock(False)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]

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
