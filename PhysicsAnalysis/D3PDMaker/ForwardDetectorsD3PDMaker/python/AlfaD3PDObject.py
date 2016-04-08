# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from   D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
import ForwardDetectorsD3PDMaker

AlfaD3PDObject = make_SG_D3PDObject( "ALFA_CLinkEvent",	"ALFA_CLinkEvent", "alfa_", "AlfaD3PDObject" )
AlfaD3PDObject.defineBlock( 0, 'ALFA_CLinkEvent_EH', ForwardDetectorsD3PDMaker.AlfaEventHeaderFillerTool )
AlfaD3PDObject.defineBlock( 0, 'ALFA_CLinkEvent_TD', ForwardDetectorsD3PDMaker.AlfaTrackingDataFillerTool )

AlfaSimHitD3PDObject=make_SG_D3PDObject("AtlasHitsVector<ALFA_Hit>","ALFA_HitCollection","alfa_simhit_", "AlfaSimHitD3PDObject" )
AlfaSimHitD3PDObject.defineBlock(0, 'ALFA_HitCollection', ForwardDetectorsD3PDMaker.AlfaHitCollectionFillerTool)
