#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
#

# Handle additonal moments for e/gamma ART
TypeForExisting = "xAOD::CaloClusterAuxContainer"
KeyForExisting = "CaloCalTopoClustersAux"
ExistingList = ""
AdditionalEgamma = ".LATERAL.ENG_FRAC_EM"
for CollectionsList in CILMergeAOD(TypeForExisting):
    if KeyForExisting in CollectionsList:
        ExistingList = CollectionsList
        break

if ExistingList:
    AdditionalEgamma = ".LATERAL.ENG_FRAC_EM"
    CILMergeAOD.removeItem(ExistingList)
    CILMergeAOD.add(ExistingList+AdditionalEgamma)
    StreamAOD.ItemList = CILMergeAOD()
    print("Modified list")
    print(StreamAOD.ItemList)
