#
# Example job options to run Frozen Showers in FastSimulation
#
# $Id: LArG4FastSimulation_frozenShowers_jobOptions.py 448395 2011-07-12 18:28:47Z gsedov $
#

if not 'showerLibInputFormat' in dir():
    showerLibInputFormat = 'pool'

print "ShowerLib input format is",showerLibInputFormat

if (showerLibInputFormat == 'pool'):
    
    # needed setups for non-standard job
    include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py") 
    theApp.Dlls   += [ "EventSelectorAthenaPool" ]

    # set up shower lib service
    include("LArG4ShowerLibSvc/LArG4ShowerLibSvc_testread_joboptions.py")

elif (showerLibInputFormat == 'txt') :
    # set up shower lib
    include("LArG4ShowerLibSvc/LArG4ShowerLibSvc_testread_textfile_joboptions.py")

else:
    print "Unknown argument for showerLibInputFormat", showerLibInputFormat

#
# change parameters for shower lib
#

# this is already done in LArG4FastSimulation/LArG4FastSimulation_setup_jobOptions.py
#from LArG4FastSimSvc.LArG4FastSimSvcConf import LArG4EMBFastSimSvc,LArG4EMECFastSimSvc,LArG4FCALFastSimSvc,LArG4FCAL2FastSimSvc,LArG4HECFastSimSvc

# LArG4EMBFastSimSvc.EFlagToParametrise(False)
# LArG4EMBFastSimSvc.EMinE(1000)
# LArG4EMBFastSimSvc.EMaxE(5000000)
# LArG4EMBFastSimSvc.EFlagToKill(True)
# LArG4EMBFastSimSvc.EEtoKill(10) 
# LArG4EMBFastSimSvc.EFlagShowerLib(True)
# LArG4EMBFastSimSvc.EMinEneShowerLib(10) 
# LArG4EMBFastSimSvc.EMaxEneShowerLib(1000) 

# LArG4EMECFastSimSvc.EFlagToParametrise(False)
# LArG4EMECFastSimSvc.EMinE(1000)
# LArG4EMECFastSimSvc.EMaxE(5000000)
# LArG4EMECFastSimSvc.EFlagToKill(True)
# LArG4EMECFastSimSvc.EEtoKill(10) 
# LArG4EMECFastSimSvc.EFlagShowerLib(True)
# LArG4EMECFastSimSvc.EMinEneShowerLib(10) 
# LArG4EMECFastSimSvc.EMaxEneShowerLib(1000) 

# LArG4FCALFastSimSvc.EFlagShowerLib(False)
# LArG4FCAL2FastSimSvc.EFlagShowerLib(False)
# LArG4HECFastSimSvc.EFlagShowerLib(False)
