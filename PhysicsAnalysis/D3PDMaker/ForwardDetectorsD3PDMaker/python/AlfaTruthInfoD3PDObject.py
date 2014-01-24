# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from   D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
import ForwardDetectorsD3PDMaker

AlfaTruthInfoD3PDObject = make_SG_D3PDObject( "McEventCollection",	"TruthEvent", "alfamc_", "AlfaTruthInfoD3PDObject" )

AlfaTruthInfoD3PDObject.defineBlock( 0, 'ALFA_McEventCollection', ForwardDetectorsD3PDMaker.AlfaTruthInfoFillerTool )
