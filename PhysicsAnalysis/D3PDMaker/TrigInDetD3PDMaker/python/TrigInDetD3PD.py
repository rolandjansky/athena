# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigInDetD3PD.py 340544 2011-01-18 12:51:04Z krasznaa $

# Import the D3PDObject(s):
from TrigInDetD3PDMaker.TrigInDetTrackD3PDObject import TrigInDetTrackD3PDObject
from TrigInDetD3PDMaker.TrigVertexD3PDObject     import TrigVertexD3PDObject

# Define the function adding TrigInDet information to a D3PD:
def TrigInDetD3PD( stream, level = 10 ):

    stream += TrigInDetTrackD3PDObject( level )
    stream += TrigVertexD3PDObject( level )
    return
