# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses


def addTrigFTKAthenaOptions(parser):
    parser.defineArgGroup('Athena', 'General Athena Options')
    parser.add_argument('--asetup', group='Athena', type=trfArgClasses.argFactory(trfArgClasses.argSubstep, runarg=False), nargs='+', metavar='substep:ASETUP',
                        help='asetup command string to be run before this substep is executed')
    return None

def addTrigFTKSimOptions(parser,nsubregions=4):
    parser.defineArgGroup('TrigFTKSim', 'Fast tracker simulation generic options')

    parser.add_argument('--NBanks', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Number of pattern banks', group='TrigFTKSim')
    # Here we set a default value as the merger wants this explicitly
    parser.add_argument('--NSubRegions', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Number of sub-regions', group='TrigFTKSim', default=trfArgClasses.argInt(nsubregions, runarg=True))
    parser.add_argument('--pmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of pmap file', group='TrigFTKSim')
    parser.add_argument('--pmapunused_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of pmapunused file', group='TrigFTKSim')
    parser.add_argument('--pmapcomplete_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of 11L pmap file', group='TrigFTKSim')
    parser.add_argument('--rmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of rmap file', group='TrigFTKSim')

    parser.add_argument('--IBLMode',type=trfArgClasses.argFactory(trfArgClasses.argInt,runarg=True),
                        help='Enalbe the IBL geometry',group='TrigFTKSim')

    parser.add_argument('--StoreAllSS',type=trfArgClasses.argFactory(trfArgClasses.argBool,runarg=True),
                        help='If enabled, store all SS, not just those in roads',group='TrigFTKSim')

    parser.add_argument('--FixEndCapL0',type=trfArgClasses.argFactory(trfArgClasses.argBool,runarg=True),
                        help='Fix EndCap L0 clustering',group='TrigFTKSim')

    parser.add_argument('--UseTSPBank', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='TSP bank utilisation', group='TrigFTKSim')
    parser.add_argument('--UseCompressedBank', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='use compressed bank algorithm', group='TrigFTKSim')

    parser.add_argument('--ReadFTKHits', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                            help='Read FTKHits directly', group='TrigFTKSim')

    # Cannot take maxEvents as an argument from addAthenaArguments() as it will have the wrong
    # default ('first', but we need 'all')
    parser.add_argument('--maxEvents', group='TrigFTKSim',
                        type=trfArgClasses.argFactory(trfArgClasses.argSubstepInt, runarg=True, defaultSubstep='all'),
                        nargs='+', metavar='substep:maxEvents',
                        help='Set maximum events for each processing step (default for this transform is to set for all substeps)')

    parser.add_argument("--DuplicateGanged", group="TrigFTKSim",
                        default=trfArgClasses.argInt(1, runarg=True),
                        help="Duplicate ganged pixels so we don't lose efficiency",
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True))

    parser.add_argument("--GangedPatternReco", group="TrigFTKSim",
                        default=trfArgClasses.argInt(0, runarg=True),
                        help="Pattern recognition to partially remove duplication",
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True))

    ### default here need to be 'all' since we have many steps
    parser.add_argument("--firstEventFTK", "--FirstEventFTK", group="TrigFTKSim",
                        help="the number of the first event to process, specifically for FTK",
                        type=trfArgClasses.argFactory(trfArgClasses.argSubstepInt, runarg=True, defaultSubstep='all'))

    ### default here need to be 'all' since we have many steps
    parser.add_argument("--firstEvent", "--FirstEvent", group="TrigFTKSim",
                        help="the number of the first event to process, specifically not for FTK",
                        type=trfArgClasses.argFactory(trfArgClasses.argSubstepInt, runarg=True, defaultSubstep='all'))


    ### default here need to be 'all' since we have many steps, though for RDO_FTK running we don't want them all (only r2e and r2eFTK)
    parser.add_argument("--skipEvents", "--SkipEvents", nargs='+', group="TrigFTKSim",
                        help="the number of events to skip, needed for RDO_FTK running",
                        type=trfArgClasses.argFactory(trfArgClasses.argSubstepInt, runarg=True, defaultSubstep='all'))

    parser.add_argument('--postExec', type=trfArgClasses.argFactory(trfArgClasses.argSubstepList), nargs='+',
                        metavar='substep:POSTEXEC', group='TrigFTKSim',
                        help='Python code to execute after main job options are included (can be optionally limited to a single substep)')

    parser.add_argument('--postInclude', group = 'TrigFTKSim', type=trfArgClasses.argFactory(trfArgClasses.argSubstepList, splitter=','), nargs='+',
                        metavar='substep:POSTINCLUDE',
                        help='Python configuration fragment to include after main job options (can be optionally limited to a single substep). Will split on commas: frag1.py,frag2.py is understood.')

    parser.add_argument('--preExec', group = 'TrigFTKSim', type=trfArgClasses.argFactory(trfArgClasses.argSubstepList), nargs='+',
                        metavar='substep:PREEXEC',
                        help='Python code to execute before main job options are included (can be optionally limited to a single substep)')

    parser.add_argument('--preInclude', group = 'TrigFTKSim', type=trfArgClasses.argFactory(trfArgClasses.argSubstepList, splitter=','), nargs='+',
                        metavar='substep:PREINCLUDE',
                        help='Python configuration fragment to include before main job options (can be optionally limited to a single substep). Will split on commas: frag1.py,frag2.py is understood.')




    #JDC:
    parser.add_argument('--ConstantsDir', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Directory where input files are kept', group='TrigFTKSim')
    parser.add_argument('--FitConstantsVersion', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Version of fit constants', group='TrigFTKSim')
    parser.add_argument('--PatternsVersion', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Version of patterns', group='TrigFTKSim')
    parser.add_argument('--separateSubRegFitConst', default=0, type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Use separate fit constant for each subregion', group='TrigFTKSim')
    parser.add_argument('--useDBPath', default=0, type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Query the Data Base to get File Paths', group='TrigFTKSim')
    parser.add_argument('--runNum', default=0, type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Run Number', group='TrigFTKSim')
    parser.add_argument('--versionTag', default=0, type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='COOL tag for a different version', group='TrigFTKSim')
    #end JDC

    parser.add_argument('--FTKSetupTag', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Setup the FTK architecture tag, if specific values are also used they have priority', group='TrigFTKSim')

    return None

def addTrigFTKSimRFOptions(parser):
    parser.defineArgGroup('TrigFTKRoadFinder', 'Add FTK road finder options')
    parser.add_argument('--ssmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of ssmap file', group='TrigFTKRoadFinder')
    parser.add_argument('--ssmaptsp_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of ssmaptsp file', group='TrigFTKRoadFinder')
    parser.add_argument('--ssmapunused_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of ssmapunused file', group='TrigFTKRoadFinder')
    parser.add_argument('--bankpatterns', type=trfArgClasses.argFactory(trfArgClasses.argIntList, runarg=True),
                        help='Number of bank patterns', group='TrigFTKRoadFinder', nargs='+')
    parser.add_argument("--patternbankspath",type=trfArgClasses.argFactory(trfArgClasses.argList,runarg=True),
                        help="String format describing the generic path for the bank like: patterns_reg{0}_sub{1}.root.",
                        group="TrigFTKRoadFinder",nargs='+')

    parser.add_argument("--MaxMissingSCTPairs", type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help="Maximum number of missing SCT pairs, allowed in the transition region", group="TrigFTKRoadFinder")
    parser.add_argument('--badmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of badmap file', group='TrigFTKRoadFinder')
    parser.add_argument('--badmap_path_for_hit', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of badmap file for hits', group='TrigFTKRoadFinder')
    parser.add_argument('--UseTSPBank', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='TSP bank utilisation', group='TrigFTKRoadFinder')
    parser.add_argument('--DBBankLevel', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='DBBankLevel', group='TrigFTKRoadFinder')
    parser.add_argument('--TSPSimulationLevel', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='TSPSimulationLevel', group='TrigFTKRoadFinder')
    parser.add_argument('--TSPMinCoverage', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='TSPMinCoverage', group='TrigFTKRoadFinder')

    parser.add_argument('--HWModeSS',type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                         help="Set the SS format: 0 (def) offline, 1 HW-like used in Vertical Slice, 2 HW-like 2015", group="TrigFTKRoadFinder")
    parser.add_argument('--ModuleLUTPath',type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                         help="Global to Local module ID LUT used during the road finding stage", group="TrigFTKRoadFinder")

    parser.add_argument('--ModuleLUTPath2nd',type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                         help="Global to Local module ID LUT used during the SSB", group="TrigFTKRoadFinder")

    parser.add_argument('--DCMatchMethod',type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                         help="Set the DC matching method", group="TrigFTKRoadFinder")

    parser.add_argument("--PixelClusteringMode",type=trfArgClasses.argFactory(trfArgClasses.argInt,runarg=True),
                        help="Set the pixel clustering mode: 0 default, 1 ToT+pixel center",group="TrigFTKRoadFinder")
    parser.add_argument("--Ibl3DRealistic",type=trfArgClasses.argFactory(trfArgClasses.argBool,runarg=True),
                        help="Do the IBL 3D realistic(HWlike) clustering? Default is false",group="TrigFTKRoadFinder")
    parser.add_argument('--SctClustering',type=trfArgClasses.argFactory(trfArgClasses.argInt,runarg=True),
                        help="Set the SCT clustering [def: False]", group="TrigFTKRoadFinder")

    parser.add_argument('--Clustering',type=trfArgClasses.argFactory(trfArgClasses.argBool,runarg=True),
                        help="Perform clustering? Default is true", group="TrigFTKRoadFinder")

    parser.add_argument('--ReadClusters',type=trfArgClasses.argFactory(trfArgClasses.argBool,runarg=True),
                        help="Read Clusters from FTKIP File? Default is false", group="TrigFTKRoadFinder")

    parser.add_argument('--MakeCache', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Enable MakeCache', group='TrigFTKRoadFinder')
    parser.add_argument('--CachePath', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='CachePath', group='TrigFTKRoadFinder')
    parser.add_argument("--CachedBank", type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Interpret the pattern bank has a cache", group="TrigFTKRoadFinder")
    parser.add_argument("--SectorsAsPatterns", type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help="If 1 allows to use a list of sectors as pattern bank, default 0", group="TrigFTKRoadFinder")

    parser.add_argument('--DoRoadFile', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Enable roads file', group='TrigFTKRoadFinder')
    parser.add_argument('--RoadFilesDir', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='RoadFilesDir', group='TrigFTKRoadFinder')
    parser.add_argument('--SaveRoads', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Save roads file', group='TrigFTKRoadFinder')
    parser.add_argument('--SetAMSize',type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='This variable decides how to set the limit on the number of patterns: 0 TSP, 1 or 2 AM (as soon as limit reached, before exceeded)',
                        group='TrigFTKRoadFinder')

    return None


def addTrigFTKSimTFOptions(parser):
    parser.defineArgGroup('TrigFTKTrackFitter', 'Add FTK track fitter options')

    parser.add_argument("--fitconstantspath",type=trfArgClasses.argFactory(trfArgClasses.argList,runarg=True),
                        help="String format describing the generic path for the fitconstants like: patterns_reg{0}.root.",
                        group="TrigFTKTrackFitter",nargs='+')
    parser.add_argument("--sectorspath",type=trfArgClasses.argFactory(trfArgClasses.argList,runarg=True),
                        help="String format describing the generic path for the sectors like: patterns_reg{0}.root.",
                        group="TrigFTKTrackFitter",nargs='+')
    parser.add_argument("--fit711constantspath",type=trfArgClasses.argFactory(trfArgClasses.argList,runarg=True),
                        help="String format describing the generic path for the 2nd stage fit constants like: patterns_reg{0}.root.",
                        group="TrigFTKTrackFitter",nargs='+')

    parser.add_argument('--Save1stStageTrks', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Save 1st stage tracks', group="TrigFTKTrackFitter")

    parser.add_argument('--loadHWConf_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Location of HW configuration file', group='TrigFTKTrackFitter')
    parser.add_argument('--HWNDiff', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help="Hit Warrior threshold", group='TrigFTKTrackFitter')
    parser.add_argument('--HitWarrior', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help="Hit Warrior type: 0 none, 1 local, 2 global (def)", group='TrigFTKTrackFitter')
    parser.add_argument('--HitWarriorMerger', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help="Hit Warrior Merger (merge) type", group='TrigFTKTrackFitter')

    parser.add_argument('--SecondStageFit', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Enables the second stage fitter", group='TrigFTKTrackFitter')

    parser.add_argument('--PrintSSBConstants', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Print SSB Constants in firmware-style", group='TrigFTKTrackFitter')

    parser.add_argument('--dTIBL', type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True),
                        help="dT for IBL temperature shifts in SSB fit.", group='TrigFTKTrackFitter')


    parser.add_argument('--doAuxFW', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Enables firmware-style constants", group='TrigFTKTrackFitter')
    parser.add_argument('--MaxNcomb', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help="Limit on the number of combinations per road", group='TrigFTKTrackFitter')
    parser.add_argument('--MaxNhitsPerPlane', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help="limit the number of hits per plane per road", group='TrigFTKTrackFitter')

    parser.add_argument("--Save1stStageTrks", type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Save the tracks after the first stage", group='TrigFTKTrackFitter')

    parser.add_argument('--TRACKFITTER_MODE', type=trfArgClasses.argFactory(trfArgClasses.argIntList, runarg=True),
                        help='track fitter mode', group='TrigFTKTrackFitter', nargs='+')
    parser.add_argument('--SSFMultiConnection', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Flag to enable the multi-connections mode in the SSF', group='TrigFTKTrackFitter')
    parser.add_argument('--SSFNConnections', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Maximum number of connections in the SSF', group='TrigFTKTrackFitter')
    parser.add_argument('--SSFAllowExtraMiss', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Allowing extra missed layer in the transition region', group='TrigFTKTrackFitter')
    parser.add_argument('--SSFTRDefn', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='How the transition region is defined: 0=moduleID of last SCT layer, 1=by eta of 1st stage track', group='TrigFTKSim')
    parser.add_argument('--SSFTRMinEta', type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True),
                        help='when SSFTRDefn=1 (by eta), the min eta', group='TrigFTKTrackFitter')
    parser.add_argument('--SSFTRMaxEta', type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True),
                        help='when SSFTRDefn=1 (by eta), the max eta', group='TrigFTKTrackFitter')

    return None


def addTrigFTKSimMergeOptions(parser):
    parser.defineArgGroup('TrigFTKMerge', 'Fast tracker simulation merge options')
    parser.add_argument('--FTKUnmergedInputPath',
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Unmerged Input file path', group='TrigFTKMerge')

    parser.add_argument('--SaveTruthTree',
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Save truth tree in merged output', group='TrigFTKMerge')

    parser.add_argument('--SaveOfflineTree',
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Save offline cluster and track tree in merged output', group='TrigFTKMerge')

    parser.add_argument('--OfflineTreeName',
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Tree name for offline clusters and tracks in merged output', group='TrigFTKMerge')

    parser.add_argument('--FTKForceAllInput',
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Force all Input files to be present', group='TrigFTKMerge')

    parser.add_argument('--FTKDoGrid',
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Use the naming for the grid input', group='TrigFTKMerge')

    parser.add_argument('--MergeRoads',
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Merge roads', group='TrigFTKMerge')

    parser.add_argument('--MergeRoadsDetailed',
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Merge roads at Detailed level', group='TrigFTKMerge')
    return None

if __name__ == '__main__':
    print "TrigFTKSim transform parser manipulator module"
