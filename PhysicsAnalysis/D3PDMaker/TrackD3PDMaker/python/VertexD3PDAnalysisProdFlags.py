# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file TrackD3PDMaker/python/VertexGroupD3PDProdFlags.py
## @brief Setup Vertex Group D3PD (VTXD3PD) in D3PDMakerConfig/D3PDProdFlags.py
## @author Simone Pagan Griso
## @date Mar, 2012
##
## Any objects here with a name of the form Write*D3PD that derive
## from JobProperty are added to D3PDProdFlags.
##

from AthenaCommon.JobProperties import JobProperty
from TrackD3PDMaker.VertexD3PDAnalysisKeys import VertexD3PDAnalysisKeys

class WriteIDVTXD3PD (JobProperty):
     """Produce the Vertex Group (IDVTX) D3PD."""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamNTUP_IDVTX'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "TrackD3PDMaker/VertexGroupD3PD_prodJobOFragment.py"
     TreeNames = [VertexD3PDAnalysisKeys.D3PDTreeName()]
     SubSteps = ['a2d', 'e2d']

class WriteIDVTXLUMID3PD (JobProperty):
     """Produce the Vertex Luminosiry (IDVTXLUMI) D3PD."""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamNTUP_IDVTXLUMI'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "TrackD3PDMaker/VertexLumiD3PD_prodJobOFragment.py"
     TreeNames = [VertexD3PDAnalysisKeys.D3PDTreeName()]
     SubSteps = ['a2d', 'e2d']
