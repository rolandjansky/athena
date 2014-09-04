########################################################
########## including out Generator options...
#######################################################

from G4AtlasApps.SimFlags import simFlags
if not simFlags.CosmicFilterVolumeName.statusOn:
    print "Warning CosmicFilterVolumeName not set using default (CaloEntryLayer)"
    simFlags.CosmicFilterVolumeName = "CaloEntryLayer"

#--------------------------------------------------------------
# Setup generator + simulation + trigger
#--------------------------------------------------------------

import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

# make sure we are loading the ParticleProperty service
from PartPropSvc.PartPropSvcConf import PartPropSvc
ServiceMgr += PartPropSvc()

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from CosmicGenerator.CosmicGeneratorConf import CosmicGenerator
job += CosmicGenerator()

if hasattr(simFlags, 'RandomSvc'):
    ##Only for MC12 onwards
    job.CosmicGenerator.AtRndmGenSvc = simFlags.RandomSvc.get_Value()
    simFlags.RandomSeedList.addSeed( "COSMICS", 2040160768, 443921183 )
    
job.CosmicGenerator.emin = 10000;  # default =10000 #10 GeV
job.CosmicGenerator.emax = 5000*1000; # 2 TeV

if simFlags.CosmicFilterVolumeName == "Muon":
    print 'Using muon Volume setup of Cosmic Generator...'
    job.CosmicGenerator.xvert_low =   -301700.
    job.CosmicGenerator.xvert_hig =    298300.
    job.CosmicGenerator.zvert_low =    -300000.
    job.CosmicGenerator.zvert_hig =    300000.
    job.CosmicGenerator.Radius= 20000.
else:
    print 'Using Non-muon Volume setup of Cosmic Generator...'
    job.CosmicGenerator.xvert_low =   -200000.
    job.CosmicGenerator.xvert_hig =    200000.
    job.CosmicGenerator.zvert_low =   -200000.
    job.CosmicGenerator.zvert_hig =    200000.
    job.CosmicGenerator.Radius= 10000. #barrel length ~22m


job.CosmicGenerator.yvert_val =    57300+41000.
job.CosmicGenerator.ctcut     =    0.
job.CosmicGenerator.OptimizeForCavern= True
job.CosmicGenerator.IPx=0.
job.CosmicGenerator.IPy=0.
job.CosmicGenerator.IPz=0.


#special settings from Juerg Beringer
if simFlags.CosmicFilterVolumeName == "Pixel" or simFlags.CosmicFilterVolumeName2 == "Pixel":
    job.CosmicGenerator.Radius= 2000.
    job.CosmicGenerator.doPathLengthCut = True         # Optimization based on box cut in pixel detector plane
    job.CosmicGenerator.energyCutThreshold = 100.      # - margin of error for energy loss calculation (in MeV)
    job.CosmicGenerator.doAimedAtPixelsCut = True      # Optimization based on box cut in pixel detector plane
    job.CosmicGenerator.pixelplane_maxx = 1150.        # - require |x| < value in mm
    job.CosmicGenerator.pixelplane_maxz = 1650.        # - require |y| < value in mm
    job.CosmicGenerator.doReweighting = True           # Whether to use reweighting for cosmic ray generation
    job.CosmicGenerator.rvert_max = 300000.            # - radius in mm for generating primary vertex


#fix for bug: 49362
import sys
ServiceMgr.EventSelector.EventsPerRun = int(2**31 - 1) #sys.maxint on a 32-bit machine

if simFlags.CosmicPtSlice.statusOn:
    include('CosmicGenerator/CosmicSliceConfig.py')

#######################################################
########## End of Cosmic Generator Configuration
#######################################################
