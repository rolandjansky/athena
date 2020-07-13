####################################################
# import the run jobProperties, change behavior if needed
from FatrasExample.FatrasJobProperties import FatrasFlags
from FatrasExample.FatrasOutputLevels import FatrasOutputLevelFlags

####################################################
# (-) Preset : 
#     override FatrasFlags if jobproperties are there
#
from AthenaCommon.JobProperties import jobproperties
if (hasattr(jobproperties,"FastSimulation")) :
    FatrasFlags.SimulationID.set_Value(jobproperties.FastSimulation.doFatrasID())
    FatrasFlags.SimulationCalo.set_Value(jobproperties.FastSimulation.doFatrasMuon())
    FatrasFlags.SimulationMS.set_Value(jobproperties.FastSimulation.doFatrasMuon())
else :
   from FastSimulationConfig.FastSimulationFlags import jobproperties 
   jobproperties.FastSimulation.doFatrasID   = DetFlags.ID_on()
   jobproperties.FastSimulation.doFatrasMuon = DetFlags.Muon_on()
    
if FatrasOutputLevelFlags.FatrasSteering() < 1:
   jobproperties.print_JobProperties()

####################################################
# (0) Create a new Job
#
include ('FatrasExample/FatrasJob.py')
fJob = FatrasJob('FatrasTestJob')

####################################################
# (1) The run setup
# 
#  Default Arguments are :
#
#   FatrasFlags.SingleTrackSimulation  = False
#   FatrasFlags.Simulation             = True
#   FatrasFlags.Refitting              = True
#   FatrasFlags.Reconstruction         = True
#   FatrasFlags.HitPostProcessing      = True
#   FatrasFlags.ExitStateCreation      = True
#   FatrasFlags.BuildGeometry          = False
#   FatrasFlags.GenericGeometry        = ''
#   FatrasFlags.WriteESD               = False
#   FatrasFlags.WriteAOD               = False
#   FatrasFlags.WriteJiveXML           = False

if FatrasFlags.InputGeometry.statusOn \
   and not FatrasFlags.EmulateReadoutGeometry():
   FatrasFlags.RefittingID.set_Value( True)
   FatrasFlags.ReconstructionID.set_Value( False)

####################################################
# (2) The event setup
# 
# import the key jobProperites (incorporates collection names)
from FatrasExample.FatrasKeys import FatrasKeyFlags
####################################################
# (3) The Physics list 
#
# import the jobProperties, change physics list
from FatrasExample.FatrasPhysicsList import FatrasPhysicsListFlags
#  Default Arguments are:
#
#   FatrasPhysicsListFlags.MultipleScattering = True
#   FatrasPhysicsListFlags.EnergyLoss         = True
#   FatrasPhysicsListFlags.Bremstrahlung      = True
#   FatrasPhysicsListFlags.PairProduction     = True
#   FatrasPhysicsListFlags.ParticleDecay      = True 
#
####################################################
# (4) The message stream output
# 
# import the jobProperties, change output level if needed
from FatrasExample.FatrasOutputLevels import FatrasOutputLevelFlags
#  Default Arguments are:
# 
#   FatrasOutputLevelFlags.GeometryBuilding   = 3
#   FatrasOutputLevelFlags.FatrasAlgs         = 3
#   FatrasOutputLevelFlags.FatrasTools        = 3
#   FatrasOutputLevelFlags.ExtrapolationTools = 3
#   FatrasOutputLevelFlags.FittingTools       = 3
#
####################################################
# (5) Fatras Validation
#
# import the jobProperties and steer the output file
from FatrasExample.FatrasValidation import FatrasValidationFlags 
#  Default Arguments are:
# 
#   FatrasValidationFlags.OutputFileName     = "FatrasOutput"
#   FatrasValidationFlags.MaterialEffects    = False 
#   FatrasValidationFlags.Bremstrahlung      = True
#   FatrasValidationFlags.PairProduction     = True
#   FatrasValidationFlags.ParticleDecay      = False 
#   FatrasValidationFlags.Simulation         = True
#   FatrasValidationFlags.Refitting          = True
#   FatrasValidationFlags.TrkNtuple          = True
#   FatrasValidationFlags.TrackPlotter       = True
#   FatrasValidationFlags.McNtuple           = True
#
if FatrasValidationFlags.OutputFileName.statusOn :
   # check whether the output file is needed
   from FatrasExample.FatrasValidation import FatrasValidationFlags
   # iterate over the flags
   FatrasOutputFile = False
   for k,v in  FatrasValidationFlags.__dict__.items() :
      # one is enough to need the output file
      if not FatrasOutputFile :
         try : 
           FlagValue = v()
           if FlagValue is True :
              FatrasOutputFile = True
         except TypeError :
           pass

   # include the THistSvc and associate the stream /val/
   from AthenaCommon.AppMgr import ServiceMgr
   if FatrasOutputFile :
      if not hasattr(ServiceMgr, 'THistSvc'):
         from GaudiSvc.GaudiSvcConf import THistSvc
         ServiceMgr += THistSvc()
      ServiceMgr.THistSvc.Output += [ FatrasValidationFlags.ValidationStream()+" DATAFILE='" +FatrasValidationFlags.OutputFileName() +"' TYPE='ROOT' OPT='RECREATE'" ]
      theApp.HistogramPersistency = 'ROOT'

 
####################################################
# (6) Fatras Geometry
# 
include('FatrasExample/FatrasGeometry.py')
fGeometry = FatrasGeometry(fatrasJob=fJob)

####################################################
# (7) Conditions Setup
#
include('FatrasExample/FatrasConditions.py')
fConditions = FatrasConditions(fatrasJob=fJob)

####################################################
# (8) Fatras Extrapolation 
# 
include ('FatrasExample/FatrasExtrapolation.py')
fExtrapolation = FatrasExtrapolation(fatrasJob=fJob)
#####################################################
# (9) Fatras Fitting
#
#  Default Arguments are:
#
#      fatrasJob  = None 
#      fitterType = 'kalman' #[chi2,dna,daf]
#
fFitting  = None
if FatrasFlags.RefittingID() or FatrasFlags.RefittingMS():
   include ('FatrasExample/FatrasFitting.py')
   fFitting = FatrasFitting(fatrasJob=fJob, fitterType='kalman')
#####################################################
# (10) Fatras Tools
# 
include ('FatrasExample/FatrasTools.py')
fTools = FatrasTools(fatrasJob=fJob)
#####################################################
# (11) The primary simulation
#
include ('FatrasExample/FatrasPrimarySimulation.py')
fSim = FatrasPrimarySimulation(fatrasJob=fJob)

#####################################################
# (12) Fatras Algs
# 
include ('FatrasExample/FatrasAlgs.py')
fAlgs = FatrasAlgs(fatrasJob=fJob)

#####################################################
# (13) FastCaloSim - usually configured through Reco_trf.py
# 
if FatrasFlags.FastCaloSim() and DetFlags.Calo_on() :
  # set FastCaloSim to true 
  from CaloRec.CaloCellFlags import jobproperties
  jobproperties.CaloCellFlags.doFastCaloSim.set_Value(True)
 
  # include the CaloRec jobOptions
  include( 'CaloRec/CaloRec_jobOptions.py' )

  # FastCaloSim produced tons of warnings
  ToolSvc.FastShowerCellBuilderTool.OutputLevel = ERROR

#####################################################
# (14) reconstruction chain
# 
include ('InDetSLHC_Example/FatrasReconstruction.py')
fReco = FatrasReconstruction(fatrasJob=fJob)
            
############################################################
# (15) additional validation - if needed
if FatrasFlags.ReconstructionID() :
  # check whether the ValidationNtupleWriter has been set up  
  if 'TrkValNtupleWriter' in dir() :
     # get the keys from outside
     from FatrasExample.FatrasKeys import FatrasKeyFlags
     from FatrasExample.FatrasValidation import FatrasValidationFlags     
     # add the simulation track collection
     if FatrasFlags.SimulationID() and FatrasValidationFlags.Simulation() :         
         # add the simulation track collection
         TrkValNtupleWriter.DoTruth               = True
         TrkValNtupleWriter.TrackSelectorTool     = ToolSvc.FatrasDummyTrackSelectorTool
         TrkValNtupleWriter.TrackCollection      += [ FatrasKeyFlags.TrackCollection() ]
         TrkValNtupleWriter.TrackTruthCollection += [ FatrasKeyFlags.TrackCollection()+'Truth' ]
     # add the simulation track collection
     if FatrasFlags.RefittingID() and FatrasValidationFlags.Refitting() :
         # add the simulation track collection
         TrkValNtupleWriter.TrackCollection      += [ FatrasKeyFlags.RefittedTrackCollection() ]
         TrkValNtupleWriter.TrackTruthCollection += [ FatrasKeyFlags.RefittedTrackCollection()+'Truth' ]

if not FatrasFlags.ReconstructionID() :
   if (InDetFlags.doNtupleCreation() or InDetFlags.doPhysValMon()):

    if 'TrkValNtupleWriter' in dir() :
     # get the keys from outside
     from FatrasExample.FatrasKeys import FatrasKeyFlags
     from FatrasExample.FatrasValidation import FatrasValidationFlags     
     # add the simulation track collection
    if FatrasFlags.SimulationID() and FatrasValidationFlags.Simulation() :         
         # add the simulation track collection
         TrkValNtupleWriter.DoTruth               = True
         TrkValNtupleWriter.TrackSelectorTool     = ToolSvc.FatrasDummyTrackSelectorTool
         TrkValNtupleWriter.TrackCollection      += [ FatrasKeyFlags.TrackCollection() ]
         TrkValNtupleWriter.TrackTruthCollection += [ FatrasKeyFlags.TrackCollection()+'Truth' ]
     # add the simulation track collection
    if FatrasFlags.RefittingID() and FatrasValidationFlags.Refitting() :
         # add the simulation track collection
         TrkValNtupleWriter.TrackCollection      += [ FatrasKeyFlags.RefittedTrackCollection() ]
         TrkValNtupleWriter.TrackTruthCollection += [ FatrasKeyFlags.RefittedTrackCollection()+'Truth' ]
         
      
####################################################
# File Output
include ('FatrasExample/FatrasOutput.py')

####################################################
# this is the place to modify the previous (external) settings
# (a) D3PD in case of single track simulation 
if FatrasFlags.SingleTrackSimulation() :
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.doTrkD3PD() :
        topSequence.D3PD.WriteInputDataHeader = False
        topSequence.D3PD.ExistDataHeader      = False

#from AthenaCommon.AppMgr import ServiceMgr
#ServiceMgr.StoreGateSvc.Dump = True

####################################################
# Auditors
if FatrasFlags.RunAuditors():
   # --- load AuditorSvc
   theAuditorSvc = ServiceMgr.AuditorSvc
   # --- write out summary of the memory usage
   #   | number of events to be skip to detect memory leak
   #   | 20 is default. May need to be made larger for complete jobs.
   theAuditorSvc.Auditors += [ 'ChronoAuditor']
   ServiceMgr.ChronoStatSvc.NumberOfSkippedEventsForMemStat = 5
   # --- memory auditors
   theAuditorSvc.Auditors += [ 'MemStatAuditor' ]
   MemStatAuditor         = theAuditorSvc.auditor( 'MemStatAuditor' )
   MemStatAuditor.OutputLevel = WARNING
   # --- write out a short message upon entering or leaving each algorithm
   theAuditorSvc.Auditors += [ "NameAuditor" ]  
   #
   theApp.AuditTools      = True
   theApp.AuditAlgorithms = True
   theApp.AuditServices   = True

# ------------------------------------------------------------
# Event Display
# ------------------------------------------------------------
if FatrasFlags.doVP1():
    # PerfMon seems always installed... Remove it the hard way because it kills VP1
    del ServiceMgr.PerfMonSvc
    theApp.ExtSvc    = [ s for s in theApp.ExtSvc if 'PerfMonSvc' not in s ]
    theApp.CreateSvc = [ s for s in theApp.CreateSvc if 'PerfMonSvc' not in s ]
    # now add VP1
    from VP1Algs.VP1AlgsConf import VP1Alg
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += VP1Alg()
    
    
if FatrasOutputLevelFlags.SuppressRandSvcOutput() :
  # temporarily: AtRanluxGenSvc produces tons of INFO messages
  from AthenaServices.AthenaServicesConf import AtRanluxGenSvc 
  ServiceMgr += AtRanluxGenSvc()
  ServiceMgr.AtRanluxGenSvc.OutputLevel = ERROR
  
  from AthenaServices.AthenaServicesConf import AtRndmGenSvc 
  ServiceMgr += AtRndmGenSvc()
  ServiceMgr.AtRndmGenSvc.OutputLevel = ERROR
  
