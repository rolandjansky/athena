# Creation: David Cote, November 2008

if primDPD.ApplySkimming():
    from PrimaryDPDMaker.IdProjFilter import IdProjFilter
    idpFilter=IdProjFilter(name="IDProjFilter")
    topSequence += idpFilter

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

streamName = primDPD.WriteIDProjCommStream.StreamName
fileName   = buildFileName( primDPD.WriteIDProjCommStream )
if primDPD.isVirtual() == False:
    idp=MSMgr.NewPoolStream( streamName, fileName ) 
    pass
if primDPD.isVirtual() == True:
    idp=MSMgr.NewVirtualStream( streamName, fileName ) 
    pass    
if primDPD.ApplySkimming():
    idp.AddAcceptAlgs("IDProjFilter")

from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
excludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName,excludeList )


