#**************   AOD list  ************************************************
from BTagging.BTaggingFlags import BTaggingFlags
BTaggingAODList =  BTaggingFlags.btaggingAODList

print "List of containers"
print BTaggingAODList
if len(BTaggingAODList) == 0:
    BTaggingAODList += ["xAOD::BTaggingAuxContainer#*"]
    BTaggingAODList += ["xAOD::BTaggingContainer#*"]
    BTaggingAODList += ["xAOD::BTagVertexContainer#*"]
    BTaggingAODList += ["xAOD::BTagVertexAuxContainer#*"]
    BTaggingAODList += ["xAOD::VertexContainer#BTagging*"]
    BTaggingAODList += ["xAOD::VertexAuxContainer#BTagging*"]
    
print BTaggingAODList
