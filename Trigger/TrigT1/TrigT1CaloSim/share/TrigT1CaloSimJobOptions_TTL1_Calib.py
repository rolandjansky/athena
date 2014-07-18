# Import the configurable algorithms for TrigT1Calo
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.Logging import logging  # loads logger

from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__TriggerTowerMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPMTowerMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JetElementMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EmTauTrigger
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JetTrigger
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EnergyTrigger
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPCMMMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEPCMMMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__ROD
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Tester
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__DumpTriggerObjects

# Get the algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add the required algorithms to the sequence
from TrigT1CaloSim.TrigT1CaloSimConfig import TriggerTowerMaker_TTL1
triggerTowerMaker = TriggerTowerMaker_TTL1( 'TriggerTowerMaker' )
job += [triggerTowerMaker]
   
job += LVL1__CPMTowerMaker( 'CPMTowerMaker' )
job += LVL1__JetElementMaker( 'JetElementMaker' )
job += LVL1__EmTauTrigger( 'EmTauTrigger' )
job += LVL1__JetTrigger( 'JetTrigger' )
job += LVL1__EnergyTrigger( 'EnergyTrigger' )
job += LVL1__ROD( 'ROD' )
job += LVL1__CPCMMMaker( 'CPCMMMaker' )
job += LVL1__JEPCMMMaker( 'JEPCMMMaker' )

from AthenaCommon.AppMgr import ToolSvc
from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1TriggerTowerTool
ToolSvc += LVL1__L1TriggerTowerTool('L1TriggerTowerTool')
ToolSvc.L1TriggerTowerTool.BaselineCorrection=False

# Dynamic Pedestal Correction
job.TriggerTowerMaker.BaselineCorrection = False
if job.TriggerTowerMaker.BaselineCorrection:
    ToolSvc.L1TriggerTowerTool.BaselineCorrection=True

    # obtain dynamic pedestals either from ROOT-File (only <mu>=60) or from Text-File
    useRootFile = True
    if useRootFile:
        if not hasattr(ToolSvc, 'L1DynamicPedestalProviderRoot'):
            from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1DynamicPedestalProviderRoot
            pedestalProvider = LVL1__L1DynamicPedestalProviderRoot('L1DynamicPedestalProviderRoot',
                                                                   FirRefFile='l1calofir-mu60.root')
            ToolSvc += pedestalProvider
    else:  
        if not hasattr(ToolSvc, 'L1DynamicPedestalProviderTxt'):
            from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1DynamicPedestalProviderTxt
            pedestalProvider = LVL1__L1DynamicPedestalProviderTxt('L1DynamicPedestalProviderTxt',
                                                                  InputFileEM='DynamicPedestalCorrectionEM.txt',
                                                                  InputFileHAD='DynamicPedestalCorrectionHAD.txt')
            pedestalProvider.BeamIntensityPattern=inputFileSummary['metadata']['/Digitization/Parameters']['BeamIntensityPattern']
            ToolSvc += pedestalProvider
    ToolSvc.L1TriggerTowerTool.L1DynamicPedestalProvider=pedestalProvider

    if not hasattr(ToolSvc, "LumiBlockMuTool"):
        from LumiBlockComps.LumiBlockCompsConf import LumiBlockMuTool
        ToolSvc += LumiBlockMuTool("LumiBlockMuTool")
    ToolSvc.L1TriggerTowerTool.LumiBlockMuTool = ToolSvc.LumiBlockMuTool

#
job.TriggerTowerMaker.ZeroSuppress=False
job.TriggerTowerMaker.MatchFIR=False

# FIR Filter Coefficients (<mu>=40)
useMatchedElement  = False
useAutocorrElement = True   

# Noise Cuts (<mu>=40)
useNoiseCutOcc100 = True

#...
if useMatchedElement:
    job.TriggerTowerMaker.PulseElements = True
    # Em
    job.TriggerTowerMaker.FilterCoeffsEmElement0=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement1=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement2=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement3=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement4=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement5=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement6=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement7=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement8=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement9=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement10=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement11=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement12=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement13=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement14=[1,4,9,5,1] #15
    job.TriggerTowerMaker.FilterCoeffsEmElement15=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement16=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement17=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement18=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement19=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement20=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement21=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement22=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement23=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement24=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement25=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement26=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement27=[1,4,9,5,1]
    job.TriggerTowerMaker.FilterCoeffsEmElement28=[1,4,9,5,1] # 14
    job.TriggerTowerMaker.FilterCoeffsEmElement29=[0,3,14,4,0]
    job.TriggerTowerMaker.FilterCoeffsEmElement30=[0,3,14,4,0]
    job.TriggerTowerMaker.FilterCoeffsEmElement31=[0,3,14,4,0]
    job.TriggerTowerMaker.FilterCoeffsEmElement32=[0,3,14,4,0] # 4
    # Had
    job.TriggerTowerMaker.FilterCoeffsHadElement0=[1,8,15,10,4]
    job.TriggerTowerMaker.FilterCoeffsHadElement1=[1,8,15,10,4]
    job.TriggerTowerMaker.FilterCoeffsHadElement2=[1,8,15,10,4]
    job.TriggerTowerMaker.FilterCoeffsHadElement3=[1,8,15,10,4]
    job.TriggerTowerMaker.FilterCoeffsHadElement4=[1,8,15,10,4]
    job.TriggerTowerMaker.FilterCoeffsHadElement5=[1,8,15,10,4]
    job.TriggerTowerMaker.FilterCoeffsHadElement6=[1,8,15,10,4]
    job.TriggerTowerMaker.FilterCoeffsHadElement7=[1,8,15,10,4]
    job.TriggerTowerMaker.FilterCoeffsHadElement8=[1,8,15,10,4]
    job.TriggerTowerMaker.FilterCoeffsHadElement9=[1,8,15,10,4]
    job.TriggerTowerMaker.FilterCoeffsHadElement10=[1,8,15,10,4]
    job.TriggerTowerMaker.FilterCoeffsHadElement11=[1,8,15,10,4]
    job.TriggerTowerMaker.FilterCoeffsHadElement12=[1,8,15,10,4]
    job.TriggerTowerMaker.FilterCoeffsHadElement13=[1,8,15,10,4]
    job.TriggerTowerMaker.FilterCoeffsHadElement14=[1,8,15,10,4] # 15
    job.TriggerTowerMaker.FilterCoeffsHadElement15=[0,9,15,11,6]
    job.TriggerTowerMaker.FilterCoeffsHadElement16=[0,9,15,11,6]
    job.TriggerTowerMaker.FilterCoeffsHadElement17=[0,9,15,11,6]
    job.TriggerTowerMaker.FilterCoeffsHadElement18=[0,9,15,11,6]
    job.TriggerTowerMaker.FilterCoeffsHadElement19=[0,9,15,11,6]
    job.TriggerTowerMaker.FilterCoeffsHadElement20=[0,9,15,11,6]
    job.TriggerTowerMaker.FilterCoeffsHadElement21=[0,9,15,11,6]
    job.TriggerTowerMaker.FilterCoeffsHadElement22=[0,9,15,11,6]
    job.TriggerTowerMaker.FilterCoeffsHadElement23=[0,9,15,11,6]
    job.TriggerTowerMaker.FilterCoeffsHadElement24=[0,9,15,11,6]
    job.TriggerTowerMaker.FilterCoeffsHadElement25=[0,9,15,11,6]
    job.TriggerTowerMaker.FilterCoeffsHadElement26=[0,9,15,11,6]
    job.TriggerTowerMaker.FilterCoeffsHadElement27=[0,9,15,11,6]
    job.TriggerTowerMaker.FilterCoeffsHadElement28=[0,9,15,11,6] # 14
    job.TriggerTowerMaker.FilterCoeffsHadElement29=[0,3,11,6,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement30=[0,3,11,6,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement31=[0,3,11,6,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement32=[0,3,11,6,0] #4 
 
if useAutocorrElement:
    job.TriggerTowerMaker.PulseElements = True
    # Em 
    job.TriggerTowerMaker.FilterCoeffsEmElement0=[-2,0,14,3,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement1=[-2,0,14,3,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement2=[-2,-1,15,3,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement3=[-2,-1,15,3,-4]
    job.TriggerTowerMaker.FilterCoeffsEmElement4=[-2,-1,15,3,-4]
    job.TriggerTowerMaker.FilterCoeffsEmElement5=[-2,-1,15,3,-4]
    job.TriggerTowerMaker.FilterCoeffsEmElement6=[-2,-1,15,3,-4]
    job.TriggerTowerMaker.FilterCoeffsEmElement7=[-2,0,14,3,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement8=[-1,0,7,2,-2]
    job.TriggerTowerMaker.FilterCoeffsEmElement9=[-1,0,7,2,-2]
    job.TriggerTowerMaker.FilterCoeffsEmElement10=[-2,0,14,3,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement11=[-2,0,14,3,-4]
    job.TriggerTowerMaker.FilterCoeffsEmElement12=[-3,-1,15,3,-4]
    job.TriggerTowerMaker.FilterCoeffsEmElement13=[-3,-1,15,3,-5]
    job.TriggerTowerMaker.FilterCoeffsEmElement14=[-1,0,7,2,-2] #15
    job.TriggerTowerMaker.FilterCoeffsEmElement15=[-2,-1,8,1,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement16=[-2,-1,8,1,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement17=[-2,-1,8,1,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement18=[-2,-1,8,1,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement19=[-2,-1,8,1,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement20=[-2,-1,8,1,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement21=[-2,-1,8,1,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement22=[-2,-1,8,1,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement23=[-2,-1,8,1,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement24=[-2,-1,8,1,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement25=[-1,-5,12,-2,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement26=[-1,-5,12,-2,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement27=[-1,-5,12,-2,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement28=[-2,-3,10,0,-4] # 14
    job.TriggerTowerMaker.FilterCoeffsEmElement29=[1,-2,8,1,0]
    job.TriggerTowerMaker.FilterCoeffsEmElement30=[3,-6,9,-2,0]
    job.TriggerTowerMaker.FilterCoeffsEmElement31=[3,-7,10,-3,0]
    job.TriggerTowerMaker.FilterCoeffsEmElement32=[3,-7,10,-3,0] # 4  
    # Had
    job.TriggerTowerMaker.FilterCoeffsHadElement0=[-2,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement1=[-2,4,10,5,1]
    job.TriggerTowerMaker.FilterCoeffsHadElement2=[-2,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement3=[-2,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement4=[-2,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement5=[-2,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement6=[-3,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement7=[-3,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement8=[-3,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement9=[-3,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement10=[-3,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement11=[-3,4,10,5,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement12=[-2,2,5,2,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement13=[-2,2,5,2,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement14=[-2,2,5,2,0] # 15
    job.TriggerTowerMaker.FilterCoeffsHadElement15=[-7,3,10,5,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement16=[-2,1,3,1,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement17=[-2,1,3,1,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement18=[-2,1,3,1,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement19=[-2,1,3,1,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement20=[-2,1,3,1,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement21=[-2,1,3,1,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement22=[-2,1,3,1,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement23=[-2,1,3,1,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement24=[-2,1,3,1,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement25=[-10,4,13,2,-4]
    job.TriggerTowerMaker.FilterCoeffsHadElement26=[-11,4,14,2,-5]
    job.TriggerTowerMaker.FilterCoeffsHadElement27=[-2,1,3,1,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement28=[-2,1,3,1,-1] # 14
    job.TriggerTowerMaker.FilterCoeffsHadElement29=[1,-4,8,1,0]      # FCal2-1
    job.TriggerTowerMaker.FilterCoeffsHadElement30=[3,-10,12,-3,0]   # FCal2-2
    job.TriggerTowerMaker.FilterCoeffsHadElement31=[-2,-1,13,5,0]     # FCal3-1
    job.TriggerTowerMaker.FilterCoeffsHadElement32=[0,-6,9,1,0] #4   # FCal3-2

#...
if useNoiseCutOcc100:
    # Em
    job.TriggerTowerMaker.EmReg1Thresh=[6182,6246,6506,6515,6536,6495,6449,5844,5959,5780,5476,5526,5677,5348,4807,5307,5248,5383,5316,5230,5219,5228,5122,4933,4633]
    job.TriggerTowerMaker.EmReg2Thresh=[9316,9034,8981]
    job.TriggerTowerMaker.EmReg3Thresh=[6087]
    job.TriggerTowerMaker.FCAL1Thresh=[4116,11645,28641,26263]
    # Had
    job.TriggerTowerMaker.HadReg1Thresh=[4929,4859,4944,4569,4566,4394,4203,4067,4534,3633,3497,3517,3744,3369,3126,2684,3515,3680,3726,3607,3511,3526,3491,3387,3281]
    job.TriggerTowerMaker.HadReg2Thresh=[5617,5527,5206]
    job.TriggerTowerMaker.HadReg3Thresh=[2941]
    job.TriggerTowerMaker.FCAL23Thresh=[5274,10229,3044,5329]


# Print Debug Info
log = logging.getLogger( "TrigT1CaloSimJobOptions_TTL1_Calib.py" )
log.info( "TriggerTowerMaker Global Job Options" )
log.info( "job.TriggerTowerMaker.BaselineCorrection = "+str(job.TriggerTowerMaker.BaselineCorrection) ) 
log.info( "job.TriggerTowerMaker.ZeroSuppress = "+str(job.TriggerTowerMaker.ZeroSuppress) )
log.info( "job.TriggerTowerMaker.MatchFIR = "+str(job.TriggerTowerMaker.MatchFIR) )
log.info( "useMatchedElement = "+str(useMatchedElement) )
log.info( "useAutocorrElement = "+str(useAutocorrElement) )
log.info( "useNoiseCutOcc100 = "+str(useNoiseCutOcc100) )
log.info( "ToolSvc.L1TriggerTowerTool.BaselineCorrection = "+str(ToolSvc.L1TriggerTowerTool.BaselineCorrection) )
log.info( "ToolSvc.L1TriggerTowerTool.L1DynamicPedestalProvider = "+str(ToolSvc.L1TriggerTowerTool.L1DynamicPedestalProvider) )
log.info( "Em filter coefficients, FCal:" )
log.info( "job.TriggerTowerMaker.FilterCoeffsEmElement29 = "+str(job.TriggerTowerMaker.FilterCoeffsEmElement29) )
log.info( "job.TriggerTowerMaker.FilterCoeffsEmElement30 = "+str(job.TriggerTowerMaker.FilterCoeffsEmElement30) )
log.info( "job.TriggerTowerMaker.FilterCoeffsEmElement31 = "+str(job.TriggerTowerMaker.FilterCoeffsEmElement31) )
log.info( "job.TriggerTowerMaker.FilterCoeffsEmElement32 = "+str(job.TriggerTowerMaker.FilterCoeffsEmElement32) )
log.info( "Had filter coefficients, FCal23:" )
log.info( "job.TriggerTowerMaker.FilterCoeffsHadElement29 = "+str(job.TriggerTowerMaker.FilterCoeffsHadElement29) )
log.info( "job.TriggerTowerMaker.FilterCoeffsHadElement30 = "+str(job.TriggerTowerMaker.FilterCoeffsHadElement30) )
log.info( "job.TriggerTowerMaker.FilterCoeffsHadElement31 = "+str(job.TriggerTowerMaker.FilterCoeffsHadElement31) )
log.info( "job.TriggerTowerMaker.FilterCoeffsHadElement32 = "+str(job.TriggerTowerMaker.FilterCoeffsHadElement32) )
log.info( "Em Noise Cuts" )
log.info( "job.TriggerTowerMaker.EmReg1Thresh = "+str(job.TriggerTowerMaker.EmReg1Thresh) )
log.info( "job.TriggerTowerMaker.EmReg2Thresh = "+str(job.TriggerTowerMaker.EmReg2Thresh) )
log.info( "job.TriggerTowerMaker.EmReg3Thresh = "+str(job.TriggerTowerMaker.EmReg3Thresh) )
log.info( "job.TriggerTowerMaker.FCAL1Thresh = "+str(job.TriggerTowerMaker.FCAL1Thresh) )
log.info( "Had Noise Cuts" )
log.info( "job.TriggerTowerMaker.HadReg1Thresh = "+str(job.TriggerTowerMaker.HadReg1Thresh) )
log.info( "job.TriggerTowerMaker.HadReg2Thresh = "+str(job.TriggerTowerMaker.HadReg2Thresh) )
log.info( "job.TriggerTowerMaker.HadReg3Thresh = "+str(job.TriggerTowerMaker.HadReg3Thresh) )
log.info( "job.TriggerTowerMaker.FCAL23Thresh = "+str(job.TriggerTowerMaker.FCAL23Thresh) )
