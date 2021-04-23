#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

    
    
def createPrecisionCaloTool( name, info, OutputLevel=0 ):
    
    from TrigEgammaEmulationToolMT.TrigEgammaEmulationToolMTConf import Trig__TrigEgammaEmulationPrecisionCaloHypoTool
    from AthenaCommon.SystemOfUnits import GeV

    cpart = info['chainParts'][0]
    sel = cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']
    etaBins = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    etthr = int(cpart['threshold'])
    #chain = info['chainName']


    def same( val ):
        return [val]*( len(etaBins) - 1 )

    # Configure Precision Calo
    PrecisionCaloTool = Trig__TrigEgammaEmulationPrecisionCaloHypoTool(
                                name                   = name,
                                dETACLUSTERthr         = 0.1,
                                dPHICLUSTERthr         = 0.1,
                                ET2Thr                 = same( 90 * GeV ),
                                ETthr                  = same( etthr * GeV),
                                EtaBins                = etaBins,
                                OutputLevel            = OutputLevel,
                            )

    if sel == 'etcut':
        PrecisionCaloTool.dETACLUSTERthr = 9999
        PrecisionCaloTool.dPHICLUSTERthr = 9999
        PrecisionCaloTool.ETthr = same( (etthr - 3)*GeV ) 
    elif sel == 'nocut':
        PrecisionCaloTool.AcceptAll = True

    return PrecisionCaloTool




