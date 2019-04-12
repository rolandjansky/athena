#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
def CaloLUMIBCIDToolCfg( flags, name='CaloLumiBCIDToolDefault' ):
    acc = ComponentAccumulator()
    from CaloTools.CaloToolsConf import CaloLumiBCIDTool
    from IOVDbSvc.IOVDbSvcConfig import addFolders

    if flags.Input.isMC == False:
      from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
      theLumiTool = LuminosityToolDefault()
      acc.addPublicTool( theLumiTool )

          
      if flags.Common.isOnline:
          acc.merge(addFolders(flags, ['/LAR/LArPileup/LArPileupShape<key>LArShape32</key>', 
                                       '/LAR/LArPileup/LArPileupAverage'], 'LAR_ONL'))
      else:
          acc.merge(addFolders(flags, ['/LAR/ElecCalibOfl/LArPileupShape<key>LArShape32</key>',
                                       '/LAR/ElecCalibOfl/LArPileupAverage'], 'LAR_OFL'))
      theTool = CaloLumiBCIDTool(name, 
                                 isMC=False, 
                                 LumiTool=theLumiTool, keyShape='LArShape32')

    else:
      from LArRecUtils.LArOFCToolDefault import LArOFCToolDefault
      theOFCTool = LArOFCToolDefault()
    
      acc.addPublicTool( theOFCTool )
      from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
      theBunchCrossingTool = BunchCrossingTool()
      
      acc.merge(addFolders(flags, ['/LAR/ElecCalibMC/Shape',
                                   '/LAR/ElecCalibMC/LArPileupAverage'], 'LAR_OFL'))
      theTool = CaloLumiBCIDTool(name,
                                 isMC=True,
                                 LArOFCTool = theOFCTool, BunchCrossingTool = theBunchCrossingTool)
    acc.addPublicTool( theTool )
    
    return acc
