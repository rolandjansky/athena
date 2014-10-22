#**************   AOD list  ************************************************
from BTagging.BTaggingFlags import BTaggingFlags
BTaggingAODList =  BTaggingFlags.btaggingAODList

print "List of containers"
print BTaggingAODList
if len(BTaggingAODList) == 0:
    BTaggingAODList += ["xAOD::BTaggingAuxContainer_v1#*"]
    BTaggingAODList += ["xAOD::BTaggingContainer_v1#*"]
    BTaggingAODList += ["xAOD::BTagVertexContainer_v1#*"]
    BTaggingAODList += ["xAOD::BTagVertexAuxContainer_v1#*"]
    BTaggingAODList += ["xAOD::VertexContainer_v1#BTagging*"]
    BTaggingAODList += ["xAOD::VertexAuxContainer_v1#BTagging*"]
    
print BTaggingAODList
