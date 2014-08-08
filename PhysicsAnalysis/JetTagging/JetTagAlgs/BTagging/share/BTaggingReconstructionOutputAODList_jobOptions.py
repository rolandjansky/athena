#**************   AOD list  ************************************************
from BTagging.BTaggingFlags import BTaggingFlags
BTaggingAODList =  BTaggingFlags.btaggingAODList

if len(BTaggingAODList) == 0:
    BTaggingAODList += ["xAOD::BTaggingAuxContainer_v1#*"]
    BTaggingAODList += ["xAOD::BTaggingContainer_v1#*"]
