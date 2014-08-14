##-----------------------------------------------------------------------------
## Name: PrimaryDPD_PrescaledESDStream.py
##
## Author: Eric Feng (Chicago)
## Major update: David Cote (DESY), Feb 2009
##
## $Id: PerfDPD_PrescaledESD.py,v 1.1 2009-03-31 12:09:39 kkoeneke Exp $
##-----------------------------------------------------------------------------


from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags_PrescaledESDStream import primPrescaledESDDPD

streamName = primDPD.WritePrescaledESDStream.StreamName
fileName   = buildFileName( primDPD.WritePrescaledESDStream )
if primDPD.isVirtual() == False:
    prescaledESD=MSMgr.NewPoolStream( streamName,fileName )
    pass
if primDPD.isVirtual() == True:
    prescaledESD=MSMgr.NewVirtualStream( streamName,fileName )
    pass
prescaledESD.AddItem("TrigElectronContainer#*")
prescaledESD.AddOtherAlgsToBookkeep( desdEventSkimmingFilterNamesList )

if primDPD.ApplySkimming():
    factor=primPrescaledESDDPD.PrescaleFactor()
    prescaledESD.SetPrescale(factor)

from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
excludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName,excludeList )




  
