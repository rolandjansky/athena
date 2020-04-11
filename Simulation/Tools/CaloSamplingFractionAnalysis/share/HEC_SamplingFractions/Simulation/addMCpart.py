from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.TruthSGKey.set_Value_and_Lock('TruthEvent') 
from TruthD3PDAnalysis.GenObjectsFilterTool import *
from TruthD3PDMaker.GenVertexD3PDObject import GenVertexD3PDObject
alg += GenVertexD3PDObject( 1, filter = AllTrackFilterTool() )
pfilter=TruthTrackFilterTool()
pfilter.PtMin=-1.
pfilter.EtaMax=-1.
pfilter.SelectTruthTracks=False
from TruthD3PDMaker.GenParticleD3PDObject import GenParticleD3PDObject
alg += GenParticleD3PDObject( 1, filter = pfilter )
#from TruthD3PDMaker.GenEventD3PDObject import GenEventD3PDObject
#alg += GenEventD3PDObject( 0, filter = AllTrackFilterTool() )
