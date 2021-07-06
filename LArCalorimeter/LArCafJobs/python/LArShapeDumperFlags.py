# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

_triggersToCheck = [ 'EF_g[0-9].*',  'EF_.*_larcalib', 'EF_e[0-9].*', 'EF_j[0-9].*', 'EF_fj[0-9].*' ]

def addShapeDumpFlags(flags):
    
    flags.Input.isMC=False

    flags.addFlag("LArShapeDump.digitsKey","FREE")
    flags.addFlag("LArShapeDump.noiseSignifCut",3)
    flags.addFlag("LArShapeDump.doShape",True)
    flags.addFlag("LArShapeDump.doStream",False)
    flags.addFlag("LArShapeDump.doTrigger",True)
    flags.addFlag("LArShapeDump.doOFCIter",lambda prev: prev.LAr.ROD.forceIter)
    flags.addFlag("LArShapeDump.prescale",1)
    flags.addFlag("LArShapeDump.triggerNames",_triggersToCheck)
    flags.addFlag("LArShapeDump.caloType","EMHECFCAL")
    flags.addFlag("LArShapeDump.dumpChannelInfos",False)
    flags.addFlag("LArShapeDump.outputNtup","out.root")
    flags.addFlag("LArShapeDump.HECNoiseNtup","")
    



