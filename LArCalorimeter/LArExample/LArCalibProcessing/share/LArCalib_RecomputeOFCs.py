#Example topOptions to re-run OFC computation & picking from the database


#INPUTS:
#RunNumber=180383
#IOVBegin=180383
#IOVEnd=2147483647 
#partition=["EMBA","EMBAPS"]#,"EMBC"]#,"EMBAPS","EMBCPS"]
#gain=[0]#,1,2]

#Technicalities:
#OutputOFCRootFileDir = '.'
#OutputPoolFileDir = '.'

WriteNtuple = False

include ("LArCalibProcessing/LArCalib_Flags.py")

#
# Common LArPhysWaveShifter settings
#

ShiftMode = 3
Nsamplings = 2
TimeShiftGuardRegion = 12

#
# Dphase = 3
#

OFCKey        = "LArOFC"
ShapeKey      = "LArShape"
OFCTreeName   = "OFC"
ShapeTreeName = "SHAPE"

Nsamples   = 5
Dphases    = 3 # 3x1.04 ns binning
Nphases    = 8


#Nsamples   = 5
#Dphases    = 1 # 1.04 ns binning
#Nphases    = 24


isFCAL=False

for p in partition:
    if p.startswith("FCAL"): isFCAL=True

    
#
# PhysCaliTdiff
#
UsePhysCaliTdiff = True
ReadPhysCaliTdiffFromCOOL = True
PhysCaliTdiffLArCalibFolderTag="-gaincorr-01"


#Picking:
doOFCPicker=True
DefaultPhase = 1
FillShapeCorr=False
#TimeOffsetCorrection = 0
TimeOffsetCorrection = 12 
ShapeKeyCorr = "LArShapeCorr"



OutputOFCRootFileName="LArOFCPhys_"+str(RunNumber)+"_"+"_".join(partition)+"_Gain_"+str(gain[0])+".root"
OutputPoolFileName="LArOFCPhys_"+str(RunNumber)+"_"+"_".join(partition)+"_Gain_"+str(gain[0])+".pool.root"
OutputSQLiteFile=OutputPoolFileDir+"/myDB"+str(RunNumber)+"_"+"_".join(partition)+"_Gain_"+str(gain[0])+".db"

#include ( "/afs/cern.ch/user/l/larcalib/w0/DEV/Reconstruction/toolbox/UseLArExtendedSubDetGrouping.py" )
from LArConditionsCommon.LArExtendedSubDetGrouping import LArExtendedSubDetGrouping
LArExtendedSubDetGrouping = LArExtendedSubDetGrouping()
GroupingType="ExtendedSubDetector"
selection = LArExtendedSubDetGrouping.getChannelSelection(partition,gain)
ChannelSelection='<channelSelection>'+selection+'</channelSelection>'
print ChannelSelection
print "CoolChannel Selection for ", partition, " and ",gain, " gain. "

#svcMgr.PoolSvc.ReadCatalog+=["prfile:poolcond/PoolCat_comcond_castor.xml" ]
    
#include ( "LArCalibProcessing/LArCalib_OFC_Phys_TwoFolders_jobOptions.py" )

include ( "LArCalibProcessing/LArCalib_OFC_Phys_jobOptions.py")

svcMgr.MessageSvc.OutputLevel  = INFO
