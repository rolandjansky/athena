#!/bin/sh
#/** @file post.sh
# @brief sh script that check the return code of an executable and compares 
# its output with a reference (if available).
# @param test_name 
#
# @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration.
# $Id: post_check.sh,v 1.32 2009-05-06 18:10:12 ssnyder Exp $
# **/

PP="adummythatshouldntmatch"

# Timestamps.
PP="$PP"'|^... ... [ 0-9][0-9] [0-9]{2}:[0-9]{2}:[0-9]{2} ....? 2[0-9]{3}$'

# Package versions.
PP="$PP"'|\w+-[[:digit:]]{2}-[[:digit:]]{2}-[[:digit:]]{2}'

# This gets written if the module cache gets rebuilt.
PP="$PP"'|^==> New TileCablingService created$'

# CLIDsvc output depends on module cache, existing files?
PP="$PP"'|^ClassIDSvc *INFO'

# File catalog stuff.
PP="$PP"'|^Domain.ROOT_All.:'
PP="$PP"'|XMLFileCatalog'

# Ignore all INFO messages --- too many differences otherwise.
PP="$PP"'|^ *[^ ]+ +INFO |^Py:.* ::? +INFO'

# Transient DB server timouts.
PP="$PP"'|^(error|warn) .* (Server signalled payload error|Server has signalled Global Error|Trying reload cache on proxy)'


PP="$PP"'|Read module info for'
PP="$PP"'|Warning in <TEnvRec::ChangeValue>: duplicate entry <Library.*> for level 0; ignored'
PP="$PP"'|Py:PropertyProxy  WARNING Found .* duplicates among the .* genConfDb files :'
PP="$PP"'|Py:PropertyProxy  WARNING --------------------------------------------------'
PP="$PP"'|Py:PropertyProxy  WARNING   -<component name>: <module> - . <duplicates> .'
PP="$PP"'|Py:PropertyProxy  WARNING   -.* - '
PP="$PP"'|^ *Welcome to ApplicationMgr'
PP="$PP"'|^ *running on .* on '
PP="$PP"'|No duplicates have been found: that.s good'
PP="$PP"'|^Py:ConfigurableDb +(WARNING|ERROR)'
PP="$PP"'|^Domain.ROOT_All. +Info'
PP="$PP"'|Info Database being retired'
PP="$PP"'|ImplicitCollection +Info Opened the implicit collection'
PP="$PP"'|^RootDBase.open Success'
PP="$PP"'|INFO Time User'
PP="$PP"'|^RalSessionMgr Info'
PP="$PP"'|^RelationalDatabase Info'
PP="$PP"'|at 0x[0-9a-f]+'
PP="$PP"'|msgSvc 0x[0-9a-f]+'
PP="$PP"'|iprop tool = 0x[0-9a-f]+'
PP="$PP"'|converter=0x[0-9a-f]+'
PP="$PP"'|singleton created from scratch'
PP="$PP"'|Preloading tcmalloc'
PP="$PP"'|TCMALLOCDIR not defined'
PP="$PP"'|^CORAL/.*'
PP="$PP"'|bmagatlas05_test2.data'
PP="$PP"'|^Py:TriggerFlags.py'
PP="$PP"'|^Data source lookup using'
PP="$PP"'|^EventInfoMgtInit: Got release'
PP="$PP"'|^PluginManager Info'
PP="$PP"'|^MagFieldFactory'
PP="$PP"'|ToolSvc/ToolSvc'
PP="$PP"'|^ *algs:'
PP="$PP"'|EventCounter/EventCounter'
PP="$PP"'|default: '
PP="$PP"'|^TEnvRec::ChangeValue:0: RuntimeWarning: duplicate entry'
PP="$PP"'|^ +/-+> ## .+ ##'
PP="$PP"'|^ +dcs :'
PP="$PP"'|^ +detdescr :'
PP="$PP"'|^ +[a-zA-Z0-9]+ : +(--|ON)'
PP="$PP"'|DeprecationWarning.* takes no parameters'
PP="$PP"'|^ *newobj = object.__new__'
PP="$PP"'|INFO MVFVxContainerCnv'
PP="$PP"'|Private AlgTool InDet::InDetTestBLayerTool/InDetTestBLayer'
PP="$PP"'|^ +BSOLREAD. reading file'
PP="$PP"'|^\|( \|)*-.* = '
PP="$PP"'|file found at'
PP="$PP"'|[Ss]unday conditions'
PP="$PP"'|^ +0x[0-9a-f]+  [0-9]+  [:_A-Za-z0-9]+\[[/a-zA-Z0-9:_]+\]$'
PP="$PP"'|^ +->  0x[0-9a-f]+  [0-9a-zA-Z:_<>]+\[[0x[0-9a-f]+\]\+[0-9a-f]+$'
PP="$PP"'|^-+$'
PP="$PP"'|^PMonSD'
PP="$PP"'|read from .* timed out|Request .* on chan .* failed|Trying next proxy|Trying direct connect to server|Trying next server|server error'
PP="$PP"'|^CINTClassBuilder.*possible entries are in use'
PP="$PP"'|^PDF files:'
PP="$PP"'|^ +\|-?- .* ='
PP="$PP"'|^ +\|-?x .* ='
PP="$PP"'|^.Entries :'
PP="$PP"'|Trying soft reload cache on proxy'
PP="$PP"'|^Initializing the TMVA: |^Loading Cuts from: |^Initializing TMVA '
PP="$PP"'|/referencefiles/'
PP="$PP"'|^X connection to .* broken'

# Lines above here are probably always needed.
# But we can try to clear out the stuff below when we rebase to a new release.

PP="$PP"'|^CommonLikelihood: Creating Instance'
PP="$PP"'|ATLF'
PP="$PP"'|^Py:CaloDepthToolshowerdefault.* INFO '
PP="$PP"'|no dictionary for class DataHeader_p[12]|no dictionary for class DataHeaderElement_p[12]|no dictionary for class PoolToken_p1'
PP="$PP"'|^DsoDb'
PP="$PP"'|Public AlgTool CaloFillRectangularCluster'
PP="$PP"'|^ToolSvc.SCT_LayerBuilder +WARNING|^ ToolSvc.SCT_LayerBuilder +. WARNING'
PP="$PP"'|Histograms saving not required'
PP="$PP"'|^objKeyStore:| CfgItemList/KeyStore| CfgKeyStore/KeyStore|^. . $|^. . . $|^. . . . $'
PP="$PP"'|/TrigSteering/TrigSteeringConfig.py'
PP="$PP"'|Unable to locate catalogue|^ PoolSvc.* WARNING'


# Remove this once InDetTestBLayer is fixed.
PP="$PP"'|could not get pixel module design|^ ToolSvc.InDetRecTestBLayerTool.*WARNING'

# These warnings went away as of InDetTruthAlgs-00-08-04.
PP="$PP"'|No valid proxy for object TrackTruthCollection|^ StoreGateSvc.* WARNING'

PP="$PP"'|^Py:CaloDepthToolcscopt2.* INFO '

# Can remove once we have TriggerMenuAnalysis-00-02-84 in all builds.
PP="$PP"'|^Found new type of HLT feature: TrigInDetTrackCollection_TrigL2SiTrackFinder_eGamma'

# Verbosity removed in MuonIdHelper-00-08-20.
PP="$PP"'|^MuonIdHelper::init_neighbors'

PP="$PP"'|^SCT_ID +DEBUG|HepPDT Version|^RalSessionMgr Warning Use COOL_DISABLE_CORALCONNECTIONPOOLCLEANUP|^  Z range|^  R range|^ solefittorsimtilta|^ToolSvc.BunchCrossingTool +WARNING|^ToolSvc.CaloNoiseToolDefault +WARNING|^ *$|^DbSession Info|^MGM::MuonDetectorManager::updateAlignment +WARNING|PixelClusterErrorData Initialize|^done|mtst|into pool catalog|problem running pool_insertFileToCatalog'

PP="$PP"'|^DbSession Info +Open +DbSession|^ failed to import Digitization.DigitizationFlags'

PP="$PP"'|^Found new type of HLT feature:'
PP="$PP"'|^CombLinks: FeatureIndex is invalid'
PP="$PP"'|setting up COOL for TileCal conditions data'

# new tau code not in AtlasProduction.
PP="$PP"'|WARNING The list of features to extracted is zero|^ StreamNTUP_PHYSICSD3PDMaker.tau_Filler.tau_Filler_PanTauDetails. WARNING'

PP="$PP"'|^Initializing TMVA fudge'

PP="$PP"'|^Py:AODFix +WARNING rec.doApplyAODFix is locked|^Py:AODFix +WARNING malformed AthenaVersion'

# pixel dd spews this puke in 7.3
PP="$PP"'|^subdet |^part |^barrel_endcap |^layer or disk |^phi_module |^eta_module |^phi_index |^eta_index |^indet |^sct1 |^bec1 |^lay_disk0 |^phi_mod0 |^eta_mod1 |^side0 |^strip0 |^PixelID::initLevelsFromDict - found levels|^bec_eta_mod |^pixel |^bec |^lay_disk |^bec_shift |^phi_mod |^eta_mod |^phi_mod_shift |^eta_mod_shift |^lay_disk_shift '

# more 7.3...
PP="$PP"'|no dictionary for class Trk::VertexOnTrack_p1 is available'


# roofit puke
PP="$PP"'|Developed by Wouter Verkerke and David Kirkby|NIKHEF, University of California|roofit'

PP="$PP"'|Files larger than 10GB are disallowed by ATLAS policy|They should only be produced for private use or in special cases|^ AthenaPoolCnvSvc *. WARNING'

#fastjet
PP="$PP"'|FastJet release|clustering using|external jet-finder|Cacciari|lease cite|^#'

PP="$PP"'|^  Add to|building structure|trying to build from|MuonVersion|agdd2geoVersion is empty'
PP="$PP"'|RootDbase: all good'

# More changes in devval.
PP="$PP"'|.Br .* Stream.*_ref|.Br .* Token|.Baskets :|^[*].[.]+[*]$'
PP="$PP"'|^ asking for outer radius'
PP="$PP"'|sTGCMicromegas|BCM Lucid'
PP="$PP"'|you are recording an object with key|There is already a persistent version|^ DetectorStore .* WARNING'

# optional scell tree
PP="$PP"'|Tree .*:CollectionTree: CollectionTree|Tree compression factor|RunNumber : RunNumber|EventNumber : EventNumber|^[*]+$|no dictionary for class AttributeListLayout'

# jet changes
PP="$PP"'|WARNING: VoronoiDiagramGenerator: two|^ AntiKt4LCTopoJetsMomCalc .*WARNING|^AntiKt4LCTopoJetsMomCalc +WARNING cannot retrieve|^StoreGateSvc +WARNING retrieve.* AntiKt4LCTopoJets|^Using NEW FastJet Interface'

# tau changes
PP="$PP"'|^tauRec jobProperties do not require PanTau'

# Due to MuonCombinedTimingTools-00-00-13
PP="$PP"'|Folder /TRT/Calib/RT is requested|Folder /TRT/Calib/T0 is requested|^ IOVDbSvc .* WARNING'

PP="$PP"'|Failed to find triggerStream from inputFileSummary'

PP="$PP"'|nsec setup for SCT/TRT|bunch spacing is|bunchSpacing could not be auto configured|^...1034h$'

PP="$PP"'|^GEOPIXELSERVICES|ChronoAuditor.*WARNING'
PP="$PP"'|^Py:AODSelect_setupOptions WARNING No TruthParticleContainer found|^Py:JetD3PDObject  WARNING JetMomentsFromClusters|^Py:JetMomentCalcConfig WARNING OriginCorrection'
kPP="$PP"'|^RootClassLoader Info Failed to load dictionary for native class: "short int"'

PP="$PP"'|CscCalibTools/CscCalibTool_jobOptions|^with:|^from AthenaCommon.CfgGetter import getPublicTool|^getPublicTool|This is for OffLine'

PP="$PP"'|^WARNING:component shape not Box nor Trapezoid|^ ?AtlasDetectorID::|^AtlasDetectorIDHelper::|^ ===>|^  version in|^  VERTEMP|^  attention temp|^  InfosChain|^  Version number|^  grid size|^  zgp|^  rgp|^  fgp|  Reading completed|^  Solenoid grid|is requested but no data|AlgTool TrackInCaloTools|Failed to load dictionary|^  [- ][0-9].[0-9]+( [-0-9. ]+)?$|^0x[0-9a-f]+ |please be patient'

PP="$PP"'|^name:|^ FileMgr.*WARNING|^FileMgr +WARNING At finalize'

PP="$PP"'|^ Composition|TinyXML Parser|^evt no'

# pantau
PP="$PP"'|^featureList.py:|^PanTau_Main_TopOptions|^Py:CombinedRec_config WARNING could not set up Pantau|^ -?[0-9]+[.][0-9]+,$|^ .[IFV].,$|^ .F.. .Default:|^ -?[0-9]+,$|^ .(Basic_|Eflow|ImpactParams_|JetShape_|AbsSumCharge|RecoModeLoose|NEF|NCharged|SumCharge)[^ ]*,$|^ .Eflow[^ ]+ .Default:|^ -0.[0-9]+. .Default: ...$'

PP="$PP"'|^PixelID +WARNING|^ DBM PixelID:|^PixelGeoModel did not build|^ PixelID.*WARNING'

PP="$PP"'|^Py:JobPropertyContainer:: WARNING|USHAPE|Will skip this envelope volume|^ ?AtlasGeometry.* WARNING'

# 19.x.x...
PP="$PP"'|load MagFieldTool|: Defining |Start of Reco Config|^CscSegments=|^MuonPatterns=|^MuonSegment.*=|^MuonTrack.*=|^PhiPatterns=|End of Reco Config|PerfMon domains|^::: domain|^IOVDbSvc +WARNING Tag .* substituted|^AtlasEnvelopeSvcDefinitionSvc +WARNING|LayerMaterialMap of size|^ AtlasEnvelopeSvcDefinitionSvc.* WARNING|^.*CnvAlg.* WARNING No .* with key .* found. Do nothing.|^ .*CnvAlg.* WARNING|^ToolSvc.PixelLayerBuilder +WARNING|^ ToolSvc.PixelLayerBuilder .* WARNING|^ ToolSvc.CaloNoiseToolDefault.* WARNING|^turning off forward muons|already in TClassTable|already loaded$'

PP="$PP"'|TClassTable::Add|^Note.* already loaded|^DataLink version|^ElementLink version|Using split mode on a class'

PP="$PP"'|^Warning .* is already in '

PP="$PP"'|has a NULL data object ptr'

PP="$PP"'|no dictionary for class|^loading|open Success'

PP="$PP"'|^warn .*frontier'

PP="$PP"'|^SetGeometryVersion.py obtained'
PP="$PP"'|WARNING Unable to register callback|WARNING Unable to locate catalog'
PP="$PP"'|^ImplicitCollection Info|^GeoModelSvc +WARNING.*does not match|^RootCollection Info|^RootCollectionSchemaEditor (Info|Warning)'
PP="$PP"'|^Warning in material CabCoolEcD3'
PP="$PP"'|has no streamer or dictionary'
PP="$PP"'|^RootDatabase.open Always'
PP="$PP"'|No valid proxy for object /Generation/Parameters'

# Messages that differ between tests run in AtlasCore vs AtlasEvent.
PP="$PP"'|^RalDatabaseSvc Info|^CoralApplication Info|^TRT_GeoModel +WARNING|Xtomo'

# ubsan
PP="$PP"'|bits/regex.h:1545'

PP="$PP"'|Cannot convert TrigRNNOutput'


test=$1
if [ -z "$testStatus" ]; then
    echo "post.sh> Warning: athena exit status is not available "
else 
    # check exit status
    joblog=${test}.log
    if [ "$testStatus" = 0 ]; then
	reflog=../../share/${test}.ref
        if [ ! -r $reflog ]; then
	  reflog=../share/${test}.ref
        fi

        # If we can't find the reference file, maybe it's located outside
        # the repo.  With the switch to git, we have to fall back
        # to versioning these files by hand.
        # ATLAS_REFERENCE_TAG should be a string of the form PACKAGE/VERSION.
        # We first look for it in DATAPATH.  If we don't find it,
        # we then look under ATLAS_REFERENCE_DATA, which falls back
        # to an afs path if it's not found.
        if [ \( ! -r $reflog \) -a "$ATLAS_REFERENCE_TAG" != "" ]; then
            # Look for the file in DATAPATH.
            # We have to look for the directory, not the file itself,
            # since get_files is hardcoded not to look more than two
            # levels down.
            get_files -data -symlink $ATLAS_REFERENCE_TAG > /dev/null
            reflog=`basename $ATLAS_REFERENCE_TAG`/${test}.ref
            if [ ! -r $reflog ]; then
                testdata=$ATLAS_REFERENCE_DATA
                if [ "$testdata" = "" ]; then
                    testdata=/afs/cern.ch/atlas/maxidisk/d33/referencefiles
                fi
                reflog=$testdata/$ATLAS_REFERENCE_TAG/${test}.ref
            fi
        fi

	if [ -r $reflog ]; then
            jobdiff=${joblog}-todiff.bz2
            refdiff=`basename ${reflog}`-todiff.bz2
            sed 's/H1Topo/Topo/g' < $joblog | egrep -v "$PP" | bzip2 -9 > $jobdiff
            sed 's/H1Topo/Topo/g' < $reflog | egrep -v "$PP" | bzip2 -9 > $refdiff
            bzdiff -a -u -w $jobdiff $refdiff
	    diffStatus=$?
	    if [ $diffStatus != 0 ] ; then
		echo "post.sh> ERROR: $joblog and $reflog differ"
                exit 1
	    fi
	else
	    tail $joblog
	    echo "post.sh> WARNING: reference output $reflog not available "
	    echo  " post.sh> Please check ${PWD}/$joblog"
            exit 1
	fi
    else
	tail --lines=500 $joblog
	echo  "post.sh> ERROR: Athena exited abnormally! Exit code: $testStatus "
	echo  " post.sh> Please check ${PWD}/$joblog"
    fi
fi

# Check output for ERROR/FATAL
joblog=${test}.log

exit $testStatus

