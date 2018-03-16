#######################
## Reco utils ##
#######################

# print available memory resource
import resource
softas, hardas = resource.getrlimit( resource.RLIMIT_AS )
logRecoUtils = logging.getLogger( 'RecoUtils' )
logRecoUtils.info( 'Memory address space VSize soft limit : %s hard limit: %s ',softas == -1L and 'unlimited' or str(softas),  hardas == -1L and 'unlimited' or str(hardas))
softas, hardas = resource.getrlimit( resource.RLIMIT_RSS )
logRecoUtils.info( 'Memory resident state RSS soft limit : %s hard limit: %s ',softas == -1L and 'unlimited' or str(softas),  hardas == -1L and 'unlimited' or str(hardas))

# make IOV calls in reproducible order
Service("AthenaSealSvc").OutputLevel=WARNING
Service('IOVSvc').sortKeys=True

############
# Auditors

theAuditorSvc = svcMgr.AuditorSvc
theApp.AuditAlgorithms=True  
theApp.AuditServices=True
theApp.AuditTools=True  

from AthenaCommon import CfgMgr

# change to true to abort on ER ROR
theAuditorSvc += CfgMgr.AlgErrorAuditor(Throw=False)
try:
    if rec.abortOnErrorMessage():
        CfgMgr.AlgErrorAuditor(Throw=True)
except Exception:
    logRecoUtils.warning('Could not set abort on Error Message')
    pass

theAuditorSvc += CfgMgr.ChronoAuditor()


if rec.doNameAuditor():
    # CfgMgr is the recommended accessor to configurable
    from AthenaCommon import CfgMgr
    theAuditorSvc += CfgMgr.NameAuditor()


if rec.doDetailedAuditor():  
    from AthenaCommon import CfgMgr
    theAuditorSvc += CfgMgr.MemStatAuditor()

    # Disable AthDsoLogger as it cannot work with any 
    # multi-threaded libraries, such as the new xrootd
    # client.
    # See https://indico.cern.ch/event/363955/

    # trace memory increase through component library loading (note that this includes library loaded in cascade)
    # extract largest one
    #grep AthDsoLogger athenaddl.log | awk {'print $9,$11'\;} | tr ")" " " | sort -n | tail
    # sum all lib
    #grep AthDsoLogger athenaddl.log | awk {'print $9,$11'\;} | tr ")" " " | awk ' {sum += $1} END {print sum}'
#    try:
#        import AthenaServices.AthDsoLogger as ADL
#        dso = ADL.DsoLogger()
#    except Exception:
#        treatException ("Could not load AthDsoLogger")

# if both true, prefer full memory auditor
if rec.doMemoryAuditor():  
    try:
        from AthenaAuditors.AthenaAuditorsConf import AthMemoryAuditor
        theAuditorSvc += AthMemoryAuditor(MaxStacktracesPerAlg=20,
                                          DefaultStacktraceDepth=50,
                                          StacktraceDepthPerAlg=["StreamESD 100"])
    except:
        print "WARNING: MemoryAuditor not available in this release" 
else:
    if rec.doFastMemoryAuditor():  
        try:
            from AthenaAuditors.AthenaAuditorsConf import AthMemoryAuditor
            theAuditorSvc += AthMemoryAuditor(DefaultStacktraceDepth=0)
        except:
            print "WARNING: fast MemoryAuditor not available in this release" 

try:
    from AthenaAuditors.AthenaAuditorsConf import FPEAuditor
    theAuditorSvc += FPEAuditor()
except:
    logRecoUtils.warning('Could not import FPEAuditor. Normal with 15.6.X.Y releases.')


from RecExConfig.RecoFunctions import OutputFileName
OutFileName=OutputFileName(rec.OutputSuffix())

try:
    if rec.doSGAuditor():
        
        # build SGOutFileName
        SGOutFileName="SG.out"
        if rec.doCBNT:
            SGOutFileName="cbnt_"+SGOutFileName

        SGOutFileName=OutFileName+SGOutFileName   


        from AthenaCommon import CfgMgr
        svcMgr += CfgMgr.SGAudSvc()
        theApp.CreateSvc += [ svcMgr.SGAudSvc.getFullName() ]
        svcMgr.SGAudSvc.OutFileName = SGOutFileName

        del SGOutFileName
except Exception:
    treatException("Could not load SGAudSvc" )

######################
# Performance Monitor

if rec.doPerfMon() :
    try:
        # see https://twiki.cern.ch/twiki/bin/view/Atlas/PerfMonComps
        # and https://twiki.cern.ch/twiki/bin/view/Atlas/PerfMonSD
        from PerfMonComps.PerfMonFlags import jobproperties
        jobproperties.PerfMonFlags.OutputFile = "ntuple_"+OutFileName+".root"
        jobproperties.PerfMonFlags.doMonitoring = True
        jobproperties.PerfMonFlags.doFastMon = not rec.doDetailedPerfMon()
        jobproperties.PerfMonFlags.doDetailedMonitoring = rec.doDetailedPerfMon()
        jobproperties.PerfMonFlags.doSemiDetailedMonitoring = rec.doSemiDetailedPerfMon()
        include( "PerfMonComps/PerfMonSvc_jobOptions.py" )

    except Exception:
        treatException("Could not load PerfMon" )
        

#######################
# ChronoStatSvc

#disable
#if hasattr(svcMgr, 'ChronoStatSvc'):
#    svcMgr.ChronoStatSvc.AsciiStatsOutputFile = "chrono_"+OutFileName+".txt"
    
#################
# Time limit
if rec.doTimeLimit():
    print "WARNING doTimeLimit functionality disabled because LSFTimeKeeperSvc was not maintained. Volunteer ?"
##     try:
##         include ("LSFTimeKeeper/LSFTimeKeeperOptions.py")
##         LSFTimeKeeperSvc= Service("LSFTimeKeeperSvc")
##         LSFTimeKeeperSvc.OutputLevel=DEBUG
##     except Exception:
##         treatException("could not load LSFTimeKeeperOptions.py ")

###############
# Output file options
# rather use default athenapool value
if rec.doPersistencyOptimization() and hasattr(svcMgr, 'AthenaPoolCnvSvc'):
    svcMgr.AthenaPoolCnvSvc.MaxFileSizes = [ "12000000000" ]
    svcMgr.AthenaPoolCnvSvc.CommitInterval = 1000000000

    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "TREE_BRANCH_OFFSETTAB_LEN ='100'" ]
    
    if rec.doWriteRDO():
        from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
        if athenaCommonFlags.UseLZMA():
            ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; COMPRESSION_ALGORITHM = '2'" ]
            ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; COMPRESSION_LEVEL = '1'" ]
        else:
            ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; COMPRESSION_ALGORITHM = '1'" ]
            ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; COMPRESSION_LEVEL = '4'" ]
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '1'" ]
        # Switch on splitting for the 4 largest container (default off)
        ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; ContainerName = 'TTree=CollectionTree(InDetSimDataCollection_p1/PixelSDO_Map)'; CONTAINER_SPLITLEVEL = '99'" ]
        ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; ContainerName = 'TTree=CollectionTree(InDetSimDataCollection_p1/SCT_SDO_Map)'; CONTAINER_SPLITLEVEL = '99'" ]
        ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; ContainerName = 'TTree=CollectionTree(InDetSimDataCollection_p1/TRT_SDO_Map)'; CONTAINER_SPLITLEVEL = '99'" ]
        ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; ContainerName = 'TTree=CollectionTree(LArRawChannelContainer_p4/LArRawChannels)'; CONTAINER_SPLITLEVEL = '99'" ]

    if rec.doWriteESD():
        from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
        if athenaCommonFlags.UseLZMA():
            svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolESDOutput() + "'; COMPRESSION_ALGORITHM = '2'" ]
            svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolESDOutput() + "'; COMPRESSION_LEVEL = '1'" ]
        else:
            svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolESDOutput() + "'; COMPRESSION_ALGORITHM = '1'" ]
            svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolESDOutput() + "'; COMPRESSION_LEVEL = '4'" ]
        # Optimize Basket Sizes to store data for 10 entries/events
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolESDOutput() + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '10'" ]
        # Switch on splitting for the largest container (default off)
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolESDOutput() + "'; ContainerName = 'TTree=CollectionTree(Trk::TrackCollection_tlp3/Tracks)'; CONTAINER_SPLITLEVEL = '99'" ]
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolESDOutput() + "'; ContainerName = 'TTree=CollectionTree(Trk::TrackCollection_tlp3/CombinedInDetTracks)'; CONTAINER_SPLITLEVEL = '99'" ]
        # Increase basket size for largest auxiliary containers and switch off basket optimization
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolESDOutput() + "'; ContainerName = 'CollectionTreeAnalysis::JetTagInfo_tlp3'; BRANCH_BASKET_SIZE = '256000'" ]
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolESDOutput() + "'; ContainerName = 'CollectionTreeAnalysis::JetTagInfo_tlp3'; TREE_AUTO_FLUSH = '0'" ]
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "ContainerName = 'CollectionTreeAnalysis::JetTagInfo_tlp3'; BRANCH_COMPRESSION_LEVEL = '1'" ]
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolESDOutput() + "'; ContainerName = 'CollectionTreeInDet::Track_tlp1'; BRANCH_BASKET_SIZE = '256000'" ]
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolESDOutput() + "'; ContainerName = 'CollectionTreeInDet::Track_tlp1'; TREE_AUTO_FLUSH = '0'" ]
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "ContainerName = 'CollectionTreeInDet::Track_tlp1'; BRANCH_COMPRESSION_LEVEL = '1'" ]
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolESDOutput() + "'; ContainerName = 'CollectionTreeTPCnv::MuonMeasurements_tlp2'; TREE_AUTO_FLUSH = '0'" ]
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "ContainerName = 'CollectionTreeTPCnv::MuonMeasurements_tlp2'; BRANCH_COMPRESSION_LEVEL = '1'" ]
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolESDOutput() + "'; ContainerName = 'CollectionTreeMuonCaloEnergyContainer_tlp1'; TREE_AUTO_FLUSH = '0'" ]
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "ContainerName = 'CollectionTreeMuonCaloEnergyContainer_tlp1'; BRANCH_COMPRESSION_LEVEL = '1'" ]

    if rec.doWriteAOD():
        from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
        if athenaCommonFlags.UseLZMA():
            svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolAODOutput() + "'; COMPRESSION_ALGORITHM = '2'" ]
            svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolAODOutput() + "'; COMPRESSION_LEVEL = '1'" ]
        else:
            svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolAODOutput() + "'; COMPRESSION_ALGORITHM = '1'" ]
            svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolAODOutput() + "'; COMPRESSION_LEVEL = '5'" ]
        # Optimize Basket Sizes to store data for 100 entries/events
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolAODOutput() + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '100';"]

	# Switch on splitting for the 2-3 largest container (default off)
        if rec.doTruth():
            svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolAODOutput() + "'; ContainerName = 'TTree=CollectionTree(TruthParticlesAux.)'; CONTAINER_SPLITLEVEL = '99'" ]

        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolAODOutput() + "'; ContainerName = 'TTree=CollectionTree(InDetTrackParticlesAux.)'; CONTAINER_SPLITLEVEL = '99'" ]
        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolAODOutput() + "'; ContainerName = 'TTree=CollectionTree(CaloCalTopoClustersAux.)'; CONTAINER_SPLITLEVEL = '99'" ]

        svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolAODOutput() + "'; ContainerName = 'CollectionTreeInDet::Track_tlp2'; TREE_AUTO_FLUSH = '0'" ]
        # Base the xAOD branch names just on the SG keys:
        StreamAOD.WritingTool.SubLevelBranchName = "<key>"

###############
# Verbose level (2=DEBUG, 3=INFO, 4=WAR NING, 5=ERR OR, 6=FATAL )
# MN: check if a specific Reco output level was set in job properties,
#  - don't overwrite blindly with the default
if not rec.OutputLevel.isDefault():
    ServiceMgr.MessageSvc.OutputLevel = rec.OutputLevel()
#increase the number of letter reserved to the alg/tool name from 18 to 30
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M" 
#ServiceMgr.MessageSvc.defaultLimit = 9999999  # all messages
ServiceMgr.MessageSvc.useColors = False
ServiceMgr.MessageSvc.defaultLimit=500

# write out a list of all Storegate collection with their keys and
# lock/unlock state. Very useful for debugging purpose
ServiceMgr.StoreGateSvc.Dump = rec.doDumpTES()
ServiceMgr.DetectorStore.Dump = rec.doDumpTDS()

# print one line for every object manipulation, incluing deletion at
# the end of the event or the end of the job
# StoreGateSvc.OutputLevel=VERBOSE
# DetectorStore.OutputLevel=VERBOSE

# show summary of WAR NING ER ROR and FA TAL messages
ServiceMgr.MessageSvc.showStats=True
ServiceMgr.MessageSvc.statLevel=WARNING
ServiceMgr.MessageSvc.enableSuppression = True

# if no truth and some alg still want to access it, zillions of HepMcParticleLink
# WAR NING need to be suppressed
if not rec.doTruth():
    ServiceMgr.MessageSvc.setError +=  [ "HepMcParticleLink"]

if jobproperties.Beam.beamType() == 'cosmics':
    # fixme - this is a tempory fix to get rid of lots of WAR NINGs - should be fixed in the real code
    # ToolSvc.MCTBFitter     WAR NING Pseudomeasurement not sorted correctly w.r.t. other hits
    for a in ToolSvc.getChildren():
        if a.getName()=="MCTBFitter":
            ToolSvc.MCTBFitter.OutputLevel=ERROR
        if a.getName()=="MCTBSLFitter":
            ToolSvc.MCTBSLFitter.OutputLevel=ERROR

    # to remove ER ROR message
    from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
    if not athenaCommonFlags.isOnline():
        include("InDetBeamSpotService/BeamCondSvc.py")

    # get rid of event loop WAR NINGS like:
    #  MuTagAllStations                WAR NING p_VxContainer not found at VxPrimaryCandidate
    #  MuTagStatIMO                    WAR NING p_VxContainer not found at VxPrimaryCandidate
    if hasattr(topSequence,"MuTagMasterIMO"):
        topSequence.MuTagMasterIMO.OutputLevel=ERROR
        blah = topSequence.MuTagMasterIMO
        if hasattr(blah,"MuTagAllStations"):
            topSequence.MuTagMasterIMO.MuTagAllStations.OutputLevel=ERROR
        if hasattr(blah,"MuTagStatIMO"):
            topSequence.MuTagMasterIMO.MuTagStatIMO.OutputLevel=ERROR

