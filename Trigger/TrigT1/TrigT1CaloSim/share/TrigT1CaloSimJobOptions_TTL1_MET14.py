# Import the configurable algorithms for TrigT1Calo
from AthenaCommon.GlobalFlags  import globalflags

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
ToolSvc.L1TriggerTowerTool.BaselineCorrection=True

# Dynamic Pedestal Correction
job.TriggerTowerMaker.BaselineCorrection=True
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
#job.TriggerTowerMaker.AutoCalibrateLUT=True
#job.TriggerTowerMaker.DoNotCalibrate=False
#job.TriggerTowerMaker.PedestalValue=32.
#job.TriggerTowerMaker.PedestalSpread=0.
job.TriggerTowerMaker.MatchFIR=False

# FIR Filter Coefficients (<mu>=60)
useMatchedElement  = False
useAutocorrElement = True 

# Noise Cuts (<mu>=60, occupancy=0.5%)
useNoiseCutOcc050 = True

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
 
#...
if useAutocorrElement:
    job.TriggerTowerMaker.PulseElements = True
    # Em
    job.TriggerTowerMaker.FilterCoeffsEmElement0=[-1,-1,8,1,-2]
    job.TriggerTowerMaker.FilterCoeffsEmElement1=[-1,-1,8,1,-2]
    job.TriggerTowerMaker.FilterCoeffsEmElement2=[-1,-1,8,1,-2]
    job.TriggerTowerMaker.FilterCoeffsEmElement3=[-1,-1,8,1,-2]
    job.TriggerTowerMaker.FilterCoeffsEmElement4=[-1,-1,8,1,-2]
    job.TriggerTowerMaker.FilterCoeffsEmElement5=[-1,-1,8,1,-2]
    job.TriggerTowerMaker.FilterCoeffsEmElement6=[-1,-1,8,1,-2]
    job.TriggerTowerMaker.FilterCoeffsEmElement7=[-1,-1,8,1,-2]
    job.TriggerTowerMaker.FilterCoeffsEmElement8=[-2,-1,15,3,-4]
    job.TriggerTowerMaker.FilterCoeffsEmElement9=[-1,-1,8,1,-2]
    job.TriggerTowerMaker.FilterCoeffsEmElement10=[-1,-1,8,1,-2]
    job.TriggerTowerMaker.FilterCoeffsEmElement11=[-2,-1,8,1,-2]
    job.TriggerTowerMaker.FilterCoeffsEmElement12=[-2,-1,8,1,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement13=[-2,-1,8,1,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement14=[-2,-1,8,1,-3] #15
    job.TriggerTowerMaker.FilterCoeffsEmElement15=[-2,-1,8,1,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement16=[-2,-1,8,1,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement17=[-2,-2,9,0,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement18=[-2,-2,8,1,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement19=[-2,-2,9,0,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement20=[-2,-2,9,0,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement21=[-2,-2,9,0,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement22=[-2,-2,9,0,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement23=[-2,-2,9,0,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement24=[-2,-2,8,1,-3]
    job.TriggerTowerMaker.FilterCoeffsEmElement25=[0,-3,6,-1,-1]
    job.TriggerTowerMaker.FilterCoeffsEmElement26=[0,-3,6,-1,-1]
    job.TriggerTowerMaker.FilterCoeffsEmElement27=[0,-3,6,-1,-1]
    job.TriggerTowerMaker.FilterCoeffsEmElement28=[-1,-3,9,0,-3] # 14
    job.TriggerTowerMaker.FilterCoeffsEmElement29=[1,-3,8,0,0]
    job.TriggerTowerMaker.FilterCoeffsEmElement30=[3,-7,10,-3,0]
    job.TriggerTowerMaker.FilterCoeffsEmElement31=[3,-7,10,-3,0]
    job.TriggerTowerMaker.FilterCoeffsEmElement32=[3,-7,10,-3,0] # 4  
    # Had
    job.TriggerTowerMaker.FilterCoeffsHadElement0=[-2,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement1=[-2,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement2=[-2,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement3=[-3,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement4=[-3,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement5=[-3,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement6=[-3,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement7=[-3,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement8=[-2,2,5,2,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement9=[-3,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement10=[-3,4,10,5,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement11=[-4,4,10,5,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement12=[-2,2,5,2,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement13=[-2,2,5,2,0]
    job.TriggerTowerMaker.FilterCoeffsHadElement14=[-4,3,10,5,-1] # 15
    job.TriggerTowerMaker.FilterCoeffsHadElement15=[-7,3,10,5,-2]
    job.TriggerTowerMaker.FilterCoeffsHadElement16=[-2,1,3,1,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement17=[-2,1,3,1,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement18=[-2,1,3,1,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement19=[-2,1,3,1,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement20=[-4,2,6,2,-2]
    job.TriggerTowerMaker.FilterCoeffsHadElement21=[-4,2,6,2,-2]
    job.TriggerTowerMaker.FilterCoeffsHadElement22=[-4,2,6,2,-2]
    job.TriggerTowerMaker.FilterCoeffsHadElement23=[-2,1,3,1,-1]
    job.TriggerTowerMaker.FilterCoeffsHadElement24=[-4,2,6,2,-2]
    job.TriggerTowerMaker.FilterCoeffsHadElement25=[-11,4,15,1,-5]
    job.TriggerTowerMaker.FilterCoeffsHadElement26=[-12,4,15,1,-5]
    job.TriggerTowerMaker.FilterCoeffsHadElement27=[-11,4,14,2,-6]
    job.TriggerTowerMaker.FilterCoeffsHadElement28=[-8,4,11,5,-3] # 14
    job.TriggerTowerMaker.FilterCoeffsHadElement29=[1,-5,8,1,0]      # FCal2-1
    job.TriggerTowerMaker.FilterCoeffsHadElement30=[3,-10,12,-4,0]   # FCal2-2
    job.TriggerTowerMaker.FilterCoeffsHadElement31=[-1,-1,7,2,0]     # FCal3-1
    job.TriggerTowerMaker.FilterCoeffsHadElement32=[1,-4,5,0,0] #4   # FCal3-2

# ...
if useNoiseCutOcc050:
    # Em
    job.TriggerTowerMaker.EmReg1Thresh=[9645,9732,9774,9781,9827,9752,9569,9032,8760,8600,8634,8530,8380,7629,7105,7019,7322,7907,8135,7527,7541,7557,7504,6935,7258]
    job.TriggerTowerMaker.EmReg2Thresh=[12891,12743,12646]
    job.TriggerTowerMaker.EmReg3Thresh=[8861]
    job.TriggerTowerMaker.FCAL1Thresh=[5616,17760,41266,38060]
    # Had
    job.TriggerTowerMaker.HadReg1Thresh=[6014,5960,5937,5814,5673,5530,5350,5619,5484,4997,4720,4764,4726,4456,4141,3736,4866,5137,5217,5071,4868,4787,4714,4680,4517]
    job.TriggerTowerMaker.HadReg2Thresh=[8263,7674,6470]
    job.TriggerTowerMaker.HadReg3Thresh=[3735]
    job.TriggerTowerMaker.FCAL23Thresh=[8453,13829,4616,7417]

# Print Debug Info
print "IRH::TriggerTowerMaker Global Job Options"
print job.TriggerTowerMaker.BaselineCorrection
print job.TriggerTowerMaker.ZeroSuppress
#print job.TriggerTowerMaker.AutoCalibrateLUT
#print job.TriggerTowerMaker.DoNotCalibrate
#print job.TriggerTowerMaker.PedestalValue
#print job.TriggerTowerMaker.PedestalSpread
print job.TriggerTowerMaker.MatchFIR
print useMatchedElement
print useAutocorrElement
print useNoiseCutOcc050
print ToolSvc.L1TriggerTowerTool.BaselineCorrection
print ToolSvc.L1TriggerTowerTool.L1DynamicPedestalProvider
print "IRH::Em filter coefficients, FCal:"
print job.TriggerTowerMaker.FilterCoeffsEmElement29
print job.TriggerTowerMaker.FilterCoeffsEmElement30
print job.TriggerTowerMaker.FilterCoeffsEmElement31
print job.TriggerTowerMaker.FilterCoeffsEmElement32
print "IRH::Had filter coefficients, FCal23:"
print job.TriggerTowerMaker.FilterCoeffsHadElement29
print job.TriggerTowerMaker.FilterCoeffsHadElement30
print job.TriggerTowerMaker.FilterCoeffsHadElement31
print job.TriggerTowerMaker.FilterCoeffsHadElement32
print "IRH::Em Noise Cuts"
print job.TriggerTowerMaker.EmReg1Thresh
print job.TriggerTowerMaker.EmReg2Thresh
print job.TriggerTowerMaker.EmReg3Thresh
print job.TriggerTowerMaker.FCAL1Thresh
print "IRH::Had Noise Cuts"
print job.TriggerTowerMaker.HadReg1Thresh
print job.TriggerTowerMaker.HadReg2Thresh
print job.TriggerTowerMaker.HadReg3Thresh
print job.TriggerTowerMaker.FCAL23Thresh
