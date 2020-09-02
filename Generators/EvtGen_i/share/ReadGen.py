###############################################################################
#
# ReadGen.py
# Author: Pavel Reznicek (Pavel.Reznicek@cern.ch)
# Reads the Hep-MC record and eventually dumps it to screen or an ntuple.
# Supports b-quark chains ntuple storage and CBNT.
# Allows also to rerun EvtGen (inclusive) over the MC collection.
#
###############################################################################

#------------------------------------------------------------------------------
# Global job-driving options
#------------------------------------------------------------------------------

# Dump Hep-MC on the screen
if not 'doPrintMC'     in locals(): doPrintMC     = True
# Create B-chains ntuple
if not 'doBphysNtuple' in locals(): doBphysNtuple = True
# Create CBNT
if not 'doCBNT'        in locals(): doCBNT        = True
# Rerun EvtGen
if not 'doEvtGen'      in locals(): doEvtGen      = False

# Messages level for the analysis algorithm
if not 'outputLevel'   in locals(): outputLevel   = INFO
#maxEvents     = MYNOEVENTS
if not 'maxEvents'     in locals(): maxEvents     = 1000
#skipEvents    = MYSKIP
if not 'skipEvents'    in locals(): skipEvents    = 0
# Mostly for the POOL convertors and configurables
if not 'usedRelease'   in locals(): usedRelease   = 16
# Hep-MC event key in the data file (GEN_EVENT, GEN_AOD, TruthEvent)
if not 'mcEventKey'    in locals(): mcEventKey    = "GEN_EVENT"
# Input datafile(s)
if not 'inputData'     in locals():
  inputData = [ "evgen.pool.root" ]

#------------------------------------------------------------------------------
# General Application Configuration options
#------------------------------------------------------------------------------

if usedRelease >= 13:
  from AthenaCommon.AppMgr      import theApp
  from AthenaCommon.AppMgr      import ServiceMgr
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()

#------------------------------------------------------------------------------
# To read out Athena POOL files
#------------------------------------------------------------------------------

if usedRelease >= 13:
  import AthenaPoolCnvSvc.ReadAthenaPool
else:
  include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

#------------------------------------------------------------------------------
# Particle properties
#------------------------------------------------------------------------------

include( "PartPropSvc/PartPropSvc.py" )

#------------------------------------------------------------------------------
# The POOL converters
#------------------------------------------------------------------------------

if usedRelease <= 11:
  include( "ParticleEventAthenaPool/AOD_PoolCnv_jobOptions.py" )

if usedRelease >= 12:
  include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
  include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py" )
  include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py" )
  include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

#------------------------------------------------------------------------------
# Output level threshold: 1=VERBOSE 2=DEBUG 3=INFO 4=WARNING 5=ERROR 6=FATAL
#------------------------------------------------------------------------------

if usedRelease >= 13:
  if not hasattr( ServiceMgr , "MessageSvc" ):
    from GaudiSvc.GaudiSvcConf import MessageSvc
    ServiceMgr += MessageSvc()
  MessageSvc = ServiceMgr.MessageSvc
else:
  MessageSvc = Service( "MessageSvc" )

MessageSvc.OutputLevel  = outputLevel
MessageSvc.defaultLimit = 9999999 # all messages

#------------------------------------------------------------------------------
# Event related parameters
#------------------------------------------------------------------------------

if usedRelease >= 13:
  # The import and registration with ServiceMgr is done in AthenaPoolCnvSvc.ReadAthenaPool
  EventSelector = ServiceMgr.EventSelector
else:
  EventSelector = Service( "EventSelector" )

EventSelector.InputCollections = inputData

#------------------------------------------------------------------------------
# Number of events to be processed and skipped
#------------------------------------------------------------------------------

theApp.EvtMax            = maxEvents
EventSelector.SkipEvents = skipEvents

#------------------------------------------------------------------------------
# Rerun EvtGen on the input data file
#------------------------------------------------------------------------------

if doEvtGen:

  # Copy the orignal events collection and make it writeable
  if usedRelease >= 13:
    from HepMCTools.HepMCToolsConf import CopyMcCollection
    topSequence     += CopyMcCollection()
    CopyMcCollection = topSequence.CopyMcCollection
  else:
    theApp.Dlls     += [ "HepMCTools" ]
    theApp.TopAlg   += [ "CopyMcCollection" ]
    CopyMcCollection = Algorithm( "CopyMcCollection" )

  # Replace the collection in place
  CopyMcCollection.oldCollectionKey    = mcEventKey
  CopyMcCollection.newCollectionKey    = mcEventKey
  CopyMcCollection.deleteOldCollection = True

  # Run EvtGen on that new copy of the collection
  # TODO - translate particle codes service
  if usedRelease >= 13:
    from EvtGen_i.EvtGen_iConf import EvtInclusiveDecay
    topSequence      += EvtInclusiveDecay()
    EvtInclusiveDecay = topSequence.EvtInclusiveDecay
  else:
    theApp.Dlls      += [ "EvtGen_i" ]
    theApp.TopAlg    += [ "EvtInclusiveDecay" ]
    EvtInclusiveDecay = Algorithm( "EvtInclusiveDecay" )

#------------------------------------------------------------------------------
# Dump Hep-MC to the screen (or log file)
#------------------------------------------------------------------------------

if doPrintMC:

  if usedRelease >= 13:
    from TruthExamples.TruthExamplesConf import PrintMC
    topSequence += PrintMC()
    PrintMC = topSequence.PrintMC
  else:
    theApp.Dlls  += [ "TruthExamples" ]
    theApp.TopAlg += [ "PrintMC" ]
    PrintMC = Algorithm( "PrintMC" )

  # Hep-MC event key in pool file
  PrintMC.McEventKey    = mcEventKey
  # Do you want output at all? TRUE/FALSE
  PrintMC.VerboseOutput = True
  # Event print style Vertex(traditional)/Barcode(barcode ordered)
  PrintMC.PrintStyle    = "Barcode"
  # Vertex infomration
  PrintMC.VertexInfo    = False
  # First and last event to print, if no last events => job end are printed
  PrintMC.FirstEvent    = 0
  PrintMC.LastEvent     = 100000000

#------------------------------------------------------------------------------
# Store the b-quark chains
#------------------------------------------------------------------------------

if doBphysNtuple:

  if usedRelease >= 13:
    from GeneratorFilters.GeneratorFiltersConf import BSignalFilter
    topSequence  += BSignalFilter()
    BSignalFilter = topSequence.BSignalFilter
  else:
    theApp.Dlls   += [ "GeneratorFilters" ]
    theApp.TopAlg += [ "BSignalFilter" ]
    BSignalFilter  = Algorithm( "BSignalFilter" )

  # Hep-MC event key in pool file
  BSignalFilter.McEventKey   = mcEventKey
  # No. events to be written to b-chains ntuple
  BSignalFilter.SignaltoNtup = maxEvents
  # Store also the b-quarks properties
  BSignalFilter.StoreBQuarks = True

#------------------------------------------------------------------------------
# CBNT configuration
#------------------------------------------------------------------------------

if doCBNT:

  if usedRelease >= 12:
    include( "CBNT_Athena/CBNT_AthenaAware_jobOptions.py" )
    CBNT_AthenaAware.TreeName = "t3333"
  else:
    include( "CBNT_Athena/CBNT_Athena_jobOptions.py" )
    CBNT_Athena.NtupleLocID = "/FILE1/CBNT/t3333"
  include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )
  include( "RecExCommon/CBNT_Truth_jobOptions.py" )
  if usedRelease >= 13:
    CBNT_Truth = theCBNTAA_Truth

  # Maximum number of particles in the ntuple
  CBNT_Truth.MaxNbParticles = 10000
  # Maximum number of vertices in the ntuple
  CBNT_Truth.MaxNbVertices  = 10000
  CBNT_Truth.McEventsName   = mcEventKey
  # Save ALL particles
  All.Enable = True

#------------------------------------------------------------------------------
# Ntuple configuration
#------------------------------------------------------------------------------

if doBphysNtuple or doCBNT:

  if usedRelease >= 12:

    if usedRelease >= 13:
      if not hasattr( ServiceMgr, "THistSvc" ):
        from GaudiSvc.GaudiSvcConf import THistSvc
        ServiceMgr += THistSvc()
      THistSvc = ServiceMgr.THistSvc
    else:
      theApp.Dlls += [ "AnalysisTools" ]
      THistSvc     = Service ( "THistSvc" )
    THistSvc.Output = [ "AANT DATAFILE='ntuple.root' TYP='ROOT' OPT='NEW'" ]

    if doBphysNtuple:
      if usedRelease >= 13:
        if not hasattr( topSequence, "AANTStreamBchains" ):
          if usedRelease >= 16:
            from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
          else:
            from AnalysisTools.AnalysisToolsConf import AANTupleStream
          topSequence += AANTupleStream( "AANTStreamBchains" )
        AANTStreamBchains = getattr( topSequence, "AANTStreamBchains" )
      else:
        theApp.TopAlg += [ "AANTupleStream/AANTStreamBchains" ]
        AANTStreamBchains = Algorithm( "AANTStreamBchains" )
      AANTStreamBchains.ExtraRefNames        = [ "StreamESD", "Stream1" ]
      AANTStreamBchains.WriteInputDataHeader = True
      AANTStreamBchains.OutputName           = "ntuple.root"

    if doCBNT:
      if usedRelease >= 13:
        if not hasattr( topSequence, "AANTStreamCBNT" ):
          if usedRelease >= 16:
            from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
          else:
            from AnalysisTools.AnalysisToolsConf import AANTupleStream
          topSequence += AANTupleStream( "AANTStreamCBNT" )
        AANTStreamCBNT = getattr( topSequence, "AANTStreamCBNT" )
      else:
        theApp.TopAlg += [ "AANTupleStream/AANTStreamCBNT" ]
        AANTStreamCBNT = Algorithm( "AANTStreamCBNT" )
      AANTStreamCBNT.ExtraRefNames        = [ "StreamESD", "Stream1" ]
      AANTStreamCBNT.WriteInputDataHeader = True
      AANTStreamCBNT.OutputName           = "ntuple.root"
      AANTStreamCBNT.TreeName             = "t3333"

  else:

    HistogramPersistencySvc            = Service( "HistogramPersistencySvc" )
    HistogramPersistencySvc.OutputFile = "histo.root"
    theApp.Dlls                       += [ "RootHistCnv" ]
    theApp.HistogramPersistency        = "ROOT"
    #theApp.Dlls                       += [ "HbookCnv" ]
    #theApp.HistogramPersistency        = "HBOOK"
    #HbookHistSvc                       = Service( "HbookHistSvc")
    #HbookHistSvc.NPAWC                 = 1500000
    NTupleSvc                          = Service( "NTupleSvc" )
    NTupleSvc.Output                   = [ "FILE1 DATAFILE='ntuple.root' TYP='ROOT' OPT='NEW'" ]

###############################################################################
#
# End of ReadGen.py job-options
#
###############################################################################
