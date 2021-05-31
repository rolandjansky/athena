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

from TrigNavTools.NavConverterConfig import createNavConverterAlg
alg = createNavConverterAlg("TrigNavCnv")
alg.HLTConfigSvc = svcMgr.xAODConfigSvc
alg.doPrint = False

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

topSequence += alg
StreamAOD.ItemList += ["xAOD::TrigCompositeContainer#HLTNav_All", "xAOD::TrigCompositeAuxContainer#HLTNav_AllAux.",
                        "xAOD::TrigCompositeContainer#HLTNav_Summary", "xAOD::TrigCompositeAuxContainer#HLTNav_SummaryAux."]
