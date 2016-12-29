
include.block("LArROD/LArConfigureCablingSCFolder.py")

# in the future, the cabling might be geometry dependent
print "Preparing SuperCell maps"

tagForCabling='LARIdentifierOflOnOffIdMap_SC-000'

from IOVDbSvc.CondDB import conddb
if ( conddb.isMC and not conddb.folderRequested('/LAR/IdentifierOfl/OnOffIdMap_SC') ) :
   conddb.addFolder("LAR_OFL","<tag>"+tagForCabling+"</tag>/LAR/IdentifierOfl/OnOffIdMap_SC")
else :
   conddb.addMarkup("/LAR/IdentifierOfl/OnOffIdMap_SC","<tag>"+tagForCabling+"</tag>")
