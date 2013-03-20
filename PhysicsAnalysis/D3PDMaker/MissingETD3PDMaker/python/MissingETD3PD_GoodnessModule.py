# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from RecExConfig.RecFlags import rec
from MissingETD3PDMaker.MissingETD3PDMakerFlags import *
from MissingETD3PDMaker.MissingETD3PDGoodnessD3PDObject   import *
	
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## config file in case one only wishes to add the met goodness module
		
def METPhysicsD3PDGoodness (file,
	                level = 4,
	                tuplename = 'METPhysicsD3PD',
	                seq = topSequence,
	                D3PDSvc = 'D3PD::RootD3PDSvc'):
	
    alg = D3PDMakerCoreComps.MakerAlg(tuplename, seq,
                                      file = file, D3PDSvc = D3PDSvc)

    ## goodness filler 
    alg += MissingETGoodnessD3PDObject (level, allow_missing = True) 
    #Level 4 Objects ---- MetPerf Cleaning Variables
    
    alg += JetsInfoMETD3PDObject  (level, allow_missing = True) 
    
    return alg

