# Creation: David Cote, November 2008

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

streamName = primDPD.WriteRandomCommStream.StreamName
fileName   = buildFileName( primDPD.WriteRandomCommStream )
if primDPD.isVirtual() == False:
    dpd=MSMgr.NewPoolStream( streamName, fileName ) 
    pass
if primDPD.isVirtual() == True:
    dpd=MSMgr.NewVirtualStream( streamName, fileName ) 
    pass    

from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
excludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,excludeList)

#event skimming
dpd.SetPrescale(10)

