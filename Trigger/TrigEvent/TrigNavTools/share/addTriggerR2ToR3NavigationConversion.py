#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

# this is postinclude to schedule trigger navigation conversion in transform
# to run:
"""
Reco_tf.py \
--AMI=q431 \
--athenaopts='--threads=1' \
--maxEvents=100 \
--outputAODFile=myAODfromESD.pool.root --inputESDFile=myESD.pool.root --imf False --postInclude addTriggerR2ToR3NavigationConversion.py
"""


from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
TriggerConfigGetter("ReadPool")


print(svcMgr.getAllChildren())

from TrigNavTools.TrigNavToolsConf import Run2ToRun3TrigNavConverter
alg = Run2ToRun3TrigNavConverter("TrigNavCnv", OutputLevel=2, HLTConfigSvc=svcMgr.xAODConfigSvc)
alg.doPrint = False

# examples of collections
alg.Collections = ["xAOD::MuonContainer","xAOD::L2StandAloneMuonContainer"]
    
# examples of chains
alg.Chains = ["HLT_mu4"]
alg.Rois = ["forID","forID1","forID2","forMS","forSA","forTB","forMT","forCB"]

# need to insert algoritm before the outputAODFile

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

topSequence += alg
StreamAOD.ItemList += ["xAOD::TrigCompositeContainer#HLTNav_All", "xAOD::TrigCompositeAuxContainer#HLTNav_AllAux.",
                        "xAOD::TrigCompositeContainer#HLTNav_Summary", "xAOD::TrigCompositeAuxContainer#HLTNav_SummaryAux."]
