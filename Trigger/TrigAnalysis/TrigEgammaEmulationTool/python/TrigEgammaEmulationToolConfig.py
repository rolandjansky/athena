# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************************
# TrigEgammaEmulationTool python configuration
# Author: Joao Victor da Fonseca Pinto <jodafons@cern.ch>
# Contributor: Jorge Andres Lopez <jorge.lopez@cern.ch>
#********************************************************************************

OutputLevel = 0

from AthenaCommon.Logging import logging
logger = logging.getLogger("TrigEgammaEmulationToolConfig")

from AthenaCommon         import CfgMgr
from AthenaCommon.AppMgr  import ToolSvc
from egammaRec.Factories  import ToolFactory,FcnWrapper,AlgFactory, getPropertyValue
import PyUtils.RootUtils as ru
ROOT = ru.import_root()
import cppyy

# Following loads the online selectors
from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf  import AsgElectronIsEMSelector
from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping      import ElectronIsEMMap,electronPIDmenu

#*****************************************************************************
#from TrigEgammaMatchingTool.TrigEgammaMatchingToolConf import Trig__TrigEgammaMatchingTool
#EgammaMatchTool = Trig__TrigEgammaMatchingTool("MatchingTool")
#ToolSvc += EgammaMatchTool
  
#*****************************************************************************
from egammaMVACalib.egammaMVACalibConf import egammaMVATool
mvatool = egammaMVATool("mvatool",folder="egammaMVACalib/v1")
ToolSvc += mvatool

#*****************************************************************************
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
ToolSvc.TrigDecisionTool.TrigDecisionKey='xTrigDecision'

#*****************************************************************************
#from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
#LumiTool = LuminosityToolDefault()
#ToolSvc += LumiTool
#from LumiBlockComps.LuminosityToolDefault import LuminosityToolOnline
#LumiOnlineTool = LuminosityToolOnline("LuminosityToolOnline")
#ToolSvc += LumiOnlineTool

from LumiBlockComps.LuminosityToolDefault import LuminosityToolOnline
LumiOnlineTool = LuminosityToolOnline("OnlLuminosityTool")
if not hasattr(ToolSvc,LumiOnlineTool.getName()):
  ToolSvc += LumiOnlineTool

#*****************************************************************************
# L1Calo
from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaL1SelectorTool
EgammaL1Emulator = ToolFactory( Trig__TrigEgammaL1SelectorTool,
                                name                   = "TrigEgammaL1Emulator",
                                WPNames                =       ['Tight','Medium','Loose'],
                                                      #  [Default, Tight , Medium, Loose ]
                                HadCoreCutMin          = [ 1.0   ,  1.0  ,  1.0  ,  1.0  ],
                                HadCoreCutOff          = [-0.2   , -0,2  , -0.2  , -0.2  ],
                                HadCoreSlope           = [ 1/23. ,  1/23.,  1/23.,  1/23.],
                                EmIsolCutMin           = [ 2.0   ,  1.0  ,  1.0  ,  1.5  ],
                                EmIsolCutOff           = [-1.8   , -2.6  , -2.0  , -1.8  ],
                                EmIsolSlope            = [ 1/8.  ,  1/8. ,  1/8. ,  1/8. ],
                                IsolCutMax             = 50,
                                OutputLevel            = OutputLevel)

#*****************************************************************************
# L2 staff (all Asgs will be imported from the current relase)
from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaL2SelectorTool
from TrigEgammaEmulationTool.TrigEgammaEmulationL2Config import EgammaL2CaloVeryLooseEmulator, EgammaL2CaloLooseEmulator,\
    EgammaL2CaloMediumEmulator, EgammaL2CaloTightEmulator, EgammaL2ElectronEmulator
from TrigEgammaEmulationTool.TrigEgammaEmulationL2Config import EgammaL2RingerVeryLooseEmulator, EgammaL2RingerLooseEmulator,\
    EgammaL2RingerMediumEmulator, EgammaL2RingerTightEmulator


EgammaL2Emulator = ToolFactory(Trig__TrigEgammaL2SelectorTool,
                               name                   = "TrigEgammaL2EmulatorTool",
                               OutputLevel            = OutputLevel,
                               CaloCutIDSelectors     = [ EgammaL2CaloTightEmulator ,
                                                          EgammaL2CaloMediumEmulator,
                                                          EgammaL2CaloLooseEmulator,
                                                          EgammaL2CaloVeryLooseEmulator],
                               CaloRingerSelectors    = [ EgammaL2RingerTightEmulator ,
                                                          EgammaL2RingerMediumEmulator,
                                                          EgammaL2RingerLooseEmulator,
                                                          EgammaL2RingerVeryLooseEmulator],
                               ElectronSelector       =   EgammaL2ElectronEmulator)

#*****************************************************************************
from TrigEgammaEmulationTool.TrigEgammaEmulationEFConfig import EgammaEFCaloDefaultEmulator, EgammaEFElectronDefaultEmulator, \
    EgammaEFPhotonEmulator, EgammaEFElectronNoD0Emulator


from TrigEgammaAnalysisTools.TrigEgammaProbelist import probeListLowMidPtSupportingTriggers, probeListHighPtSupportingTriggers
from TrigEgammaAnalysisTools.TrigEgammaProbelist import probeListLowMidPtPhysicsTriggers, probeListHighPtPhysicsTriggers
supportingTriggerList = probeListLowMidPtSupportingTriggers+probeListHighPtSupportingTriggers

# Update the supporting list with very loose chains. This will be used in data collisin matches
for trig in probeListLowMidPtPhysicsTriggers+probeListHighPtPhysicsTriggers:
  if 'vloose' in trig:  supportingTriggerList.append(trig)

# Emulator tool
# (all Asgs will be imported from the current relase)
from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaEmulationTool
logger.info("Creating TrigEgammaEmulationTool")
TrigEgammaEmulationTool  = ToolFactory( Trig__TrigEgammaEmulationTool, 
                                        name                    = "TrigEgammaEmulationTool",
                                        OutputLevel             = OutputLevel,
                                        TriggerList             = [],
                                        SupportingTriggerList   = supportingTriggerList,
                                        L1SelectorTool          = EgammaL1Emulator,
                                        L2SelectorTool          = EgammaL2Emulator,
                                        EFCaloSelectorTool      = EgammaEFCaloDefaultEmulator,
                                        EFElectronSelectorTools = [ EgammaEFElectronDefaultEmulator,
                                                                    #EgammaEFElectronCutD0DphiDetaEmulator, 
                                                                    EgammaEFElectronNoD0Emulator,
                                                                    #EgammaEFElectronNoDphiResEmulator,
                                                                    #EgammaEFElectronSmoothEmulator,
                                                                  ],
                                        EFPhotonSelectorTools   = [ EgammaEFPhotonEmulator],
                                        DoL2ElectronFex         = True,  # V7 menu
                                        DoRinger                = False, # V7 menu
                                        DoEFCaloPid             = False, # V7 menu
                                        )
                                          
###############################################################################################

