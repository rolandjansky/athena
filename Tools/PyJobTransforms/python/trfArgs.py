# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package PyJobTransforms.trfArgs
#  @brief Standard arguments supported by trf infrastructure
#  @author atlas-comp-transforms-dev@cern.ch
#  @version $Id: trfArgs.py 636448 2014-12-17 11:40:15Z graemes $

import logging
msg = logging.getLogger(__name__)

import PyJobTransforms.trfArgClasses as trfArgClasses
from PyJobTransforms.trfArgClasses import argFactory

from PyJobTransforms.trfLogger import stdLogLevels

## Add standard transform arguments to an argparse ArgumentParser
def addStandardTrfArgs(parser):
    parser.add_argument('--verbose', '--debug', action='store_true', help='Set transform loglevel to DEBUG')
    parser.add_argument('--loglevel', choices=stdLogLevels.keys(), help='Set transform logging level')
    parser.add_argument('--argdict', metavar='FILE', help='File containing pickled argument dictionary')
    parser.add_argument('--argJSON', '--argjson', metavar='FILE', help='File containing JSON serialised argument dictionary')
    parser.add_argument('--dumpargs', action='store_true', help='Dump transform arguments and exit')
    parser.add_argument('--showGraph', action='store_true', help='Show multi-step transform graph, then exit')
    parser.add_argument('--showPath', action='store_true', help='Show execution path only, then exit')
    parser.add_argument('--showSteps', action='store_true', help='Show list of executor steps only, then exit')
    parser.add_argument('--dumpPickle', metavar='FILE', help='Interpret command line arguments and write them out as a pickle file')
    parser.add_argument('--dumpJSON', metavar='FILE', help='Interpret command line arguments and write them out as a JSON file')
    parser.add_argument('--orphanKiller', action='store_true', help="Kill all orphaned children at the end of a job (that is, sharing the transform's pgid, but with ppid=1)." 
                                                                    "Beware, this is potentially dangerous in a a batch environment")
    parser.add_argument('--reportName', type=argFactory(trfArgClasses.argString, runarg=False), 
                        help='Base name for job reports (default name is "jobReport" for most reports, but "metadata" for classic prodsys XML)')
    parser.add_argument('--reportType', type=argFactory(trfArgClasses.argList, runarg=False), nargs='+', metavar='TYPE',
                        help='Job reports to produce: valid values are "text", "json", "classic", "pilotPickle" and "gpickle"')    
    parser.add_argument('--execOnly', action='store_true',
                        help='Exec the first substep only, replacing the transform process (no job reports and the return code will be from the substep process)')
    parser.add_argument('--env', type=argFactory(trfArgClasses.argSubstepList, runarg=False), metavar='substep:KEY=VALUE', nargs='+',
                        help='Explicitly set environment variables for an executor (default is all substeps).'
                        ' N.B. this setting is passed to the shell, so reference to shell variables is allowed, e.g.'
                        ' KEY=VALUE:$KEY')
    parser.add_argument('--imf', type=argFactory(trfArgClasses.argSubstepBool, runarg=False), metavar="substep:BOOL", nargs='+',
                        help='Manually include/exclude the Intel IMF maths library '
                        '(otherwise this is disabled for base releases < 17.7, enabled otherwise)')
    parser.add_argument('--tcmalloc', type=argFactory(trfArgClasses.argSubstepBool, runarg=False), metavar="substep:BOOL", nargs='+',
                        help='Switch preload of the tcmalloc library (disabled by default)')
    parser.add_argument('--steering', type=argFactory(trfArgClasses.argSubstepSteering, runarg=False), nargs='+', metavar='substep:{in/out}{+-}DATA',
                        help='Steer the transform by manipulating the execution graph before the execution path is calculated. '
                        'Format is substep:{in,out}{+-}DATA,{in,out}{+-}DATA,... to modify the substep\'s input/output ' 
                        ' by adding/removing a data type. e.g. RAWtoESD:in-RDO,in+RDO_TRIG would remove RDO and add '
                        'RDO_TRIG to the list of valid input datatypes for the RAWtoESD substep.')
    addMetadataArguments(parser)


## Options related to running athena in general
# TODO: Some way to mask certain options (perExec, e.g.)
## @brief Add standard athena options
#  @param parser trfArgParser object
#  @param maxEventsDefaultSubstep Special option which can change the default substep for maxEvents (needed by
#  some special transforms).
def addAthenaArguments(parser, maxEventsDefaultSubstep='first', addValgrind=True):
    parser.defineArgGroup('Athena', 'General Athena Options')
    parser.add_argument('--athenaopts', group = 'Athena', type=argFactory(trfArgClasses.argList, splitter=' ', runarg=False), metavar='OPT1 OPT2 OPT3', 
                        help='Extra options to pass to athena. Will split on spaces. Options starting with "-" must be given as --athenaopts=\'--opt1 --opt2[=foo] ...\'') 
    parser.add_argument('--command', '-c', group = 'Athena', type=argFactory(trfArgClasses.argString, runarg=False), metavar='COMMAND', 
                        help='Run %(metavar)s before all else')
    parser.add_argument('--athena', group = 'Athena', type=argFactory(trfArgClasses.argString, runarg=False), metavar='ATHENA',
                        help='Use %(metavar)s as the athena executable')
    parser.add_argument('--preExec', group = 'Athena', type=argFactory(trfArgClasses.argSubstepList), nargs='+',
                        metavar='substep:PREEXEC',
                        help='Python code to execute before main job options are included (can be optionally limited to a single substep)')
    parser.add_argument('--preInclude', group = 'Athena', type=argFactory(trfArgClasses.argSubstepList, splitter=','), nargs='+',
                        metavar='substep:PREINCLUDE',
                        help='Python configuration fragment to include before main job options (can be optionally limited to a single substep). Will split on commas: frag1.py,frag2.py is understood.')
    parser.add_argument('--postExec', group = 'Athena', type=argFactory(trfArgClasses.argSubstepList), nargs='+',
                        metavar='substep:POSTEXEC',
                        help='Python code to execute after main job options are included (can be optionally limited to a single substep)')
    parser.add_argument('--postInclude', group = 'Athena', type=argFactory(trfArgClasses.argSubstepList, splitter=','), nargs='+',
                        metavar='substep:POSTINCLUDE',
                        help='Python configuration fragment to include after main job options (can be optionally limited to a single substep). Will split on commas: frag1.py,frag2.py is understood.')
    parser.add_argument('--maxEvents', group='Athena', type=argFactory(trfArgClasses.argSubstepInt, defaultSubstep=maxEventsDefaultSubstep), 
                        nargs='+', metavar='substep:maxEvents',
                        help='Set maximum events for each processing step (default substep is "{0}")'.format(maxEventsDefaultSubstep))
    parser.add_argument('--skipEvents', group='Athena', type=argFactory(trfArgClasses.argSubstepInt, defaultSubstep='first'), 
                        help='Number of events to skip over in the first processing step')
    parser.add_argument('--asetup', group='Athena', type=argFactory(trfArgClasses.argSubstep, runarg=False), nargs='+', metavar='substep:ASETUP',
                        help='asetup command string to be run before this substep is executed')
    parser.add_argument('--eventAcceptanceEfficiency', type=trfArgClasses.argFactory(trfArgClasses.argSubstepFloat, min=0.0, max=1.0, runarg=False),
                        help='Allowed "efficiency" for processing events - used to ensure output file has enough events (default 1.0)')
    parser.add_argument('--athenaMPMergeTargetSize', '--mts', type=trfArgClasses.argFactory(trfArgClasses.argKeyFloatValueList, runarg=False),
                        metavar='dataType:targetSizeInMegaBytes', nargs='+', group='Athena',
                        help='Set the target merge size for an AthenaMP output file type (give size in MB). '
                        'Note that the special value 0 means do not merge this output file; negative values mean '
                        'always merge to a single file. Note that the datatype "ALL" will be used as a default '
                        'for all datatypes not explicitly given their own value.')
    if addValgrind:
        addValgrindArguments(parser)

## @brief Add Valgrind options
def addValgrindArguments(parser):
    parser.defineArgGroup(
        'Valgrind',
        'General Valgrind Options'
    )
    parser.add_argument(
        '--valgrind',
        group = 'Valgrind',
        type = argFactory(
            trfArgClasses.argBool,
            runarg = False
        ),
        metavar = "substep:BOOL",
        help = 'Enable Valgrind'
    )
    parser.add_argument(
        '--valgrindbasicopts',
        group = 'Valgrind',
        type = argFactory(
            trfArgClasses.argList,
            splitter = ',',
            runarg = False
        ),
        metavar = 'OPT1,OPT2,OPT3', 
        help = 'Basic options passed to Valgrind when running Athena. ' +
        'Options starting with "-" must be given as ' +
        '--valgrindopts=\'--opt1=foo,--opt2=bar,...\''
    )
    parser.add_argument(
        '--valgrindextraopts',
        group = 'Valgrind',
        type = argFactory(
            trfArgClasses.argList,
            splitter = ',',
            runarg = False
        ),
        metavar = 'OPT1,OPT2,OPT3', 
        help = 'Extra options passed to Valgrind when running Athena. ' +
        'Options starting with "-" must be given as ' +
        '--valgrindopts=\'--opt1=foo,--opt2=bar,...\''
    )

## @brief Options related to the setup of the ATLAS detector (used in simulation and digitisation
#  as well as reconstruction)
#  @param parser trfArgParser object
def addDetectorArguments(parser):
    parser.defineArgGroup('Detector', 'General detector configuration options, for simulation and reconstruction')
    parser.add_argument('--DBRelease', group = 'Detector', type=argFactory(trfArgClasses.argSubstep, runarg=False), metavar='substep:DBRelease',  nargs='+',
                        help='Use DBRelease instead of ORACLE. Give either a DBRelease tarball file (e.g., DBRelease-21.7.1.tar.gz) or cvmfs DBRelease directory (e.g., 21.7.1 or current')
    parser.add_argument('--conditionsTag', group='Detector', type=argFactory(trfArgClasses.argSubstep), metavar='substep:CondTag',  nargs='+',
                        help='Conditions tag to set')
    parser.add_argument('--geometryVersion', group='Detector', type=argFactory(trfArgClasses.argSubstep), metavar='substep:GeoVersion',  nargs='+',
                        help='ATLAS geometry version tag')
    parser.add_argument('--beamType', group='Detector', type=argFactory(trfArgClasses.argString), 
                        help='Manual beam type setting')
    parser.add_argument('--runNumber', '--RunNumber', group='Detector', type=argFactory(trfArgClasses.argInt), 
                        help='Manual run number setting')

## @brief Options for passing metadata into the transform
#  @param parser trfArgParser object
def addMetadataArguments(parser):
    parser.defineArgGroup('Metadata', 'Metadata arguments that will be passed into the transform')
    parser.add_argument('--AMIConfig', '--amiConfig', '--AMI', type=argFactory(trfArgClasses.argString, runarg=False), help='Configure transform with AMI tag', group="Metadata")
    parser.add_argument('--AMITag', '--amiConfigTag', '--AMIConfigTag', type=argFactory(trfArgClasses.argString), metavar='TAG', group="Metadata",
                        help='AMI tag from which this job was defined - this option simply writes the '
                        'relevant AMI tag value into the output metadata, it does not configure the job (use --AMIConfig for that)')
    parser.add_argument('--taskid', type=argFactory(trfArgClasses.argString, runarg=False), help="Task identification number", group="Metadata")
    parser.add_argument('--jobid', type=argFactory(trfArgClasses.argString, runarg=False), help="Job identification number", group="Metadata")
    parser.add_argument('--attempt', type=argFactory(trfArgClasses.argString, runarg=False), help="Job attempt number", group="Metadata")


## @brief Add primary DPD arguments
# @detailed Get the list of current primary DPDs and add then to the parser
# optionally only some DPDs may be added, using the @c pick list. This function
# uses the @c silent decorator to supress useless messages from ROOT
# @param parser Argument parser object to add arguments to
# @param pick Optional list of DPD types to add (use short names, e.g., @c DESDM_MUON)
# @param transform Transform object. DPD data types will be added to the correct executor (by name or substep)
# @param multipleOK If the @c multipleOK flag should be set for this argument
#@silent
def addPrimaryDPDArguments(parser, pick = None, transform = None, multipleOK=False):
    parser.defineArgGroup('Primary DPDs', 'Primary DPD File Options')
    # list* really gives just a list of DPD names
    try:
        from PrimaryDPDMaker.PrimaryDPDFlags import listRAWtoDPD,listESDtoDPD,listAODtoDPD
        for substep, dpdList in [(['r2e'], listRAWtoDPD), (['e2d'], listESDtoDPD), (['a2d'], listAODtoDPD)]:
            for dpdName in [ dpd.replace('Stream', '') for dpd in dpdList ]:
                msg.debug('Handling {0}'.format(dpdName))
                if pick == None or dpdName in pick:
                    # Need to decide which file type we actually have here
                    dpdType = dpdName.split('_')[0]
                    if 'RAW' in dpdType:
                        parser.add_argument('--output' + dpdName + 'File', 
                                            type=argFactory(trfArgClasses.argBSFile, multipleOK=multipleOK, type='RAW'), 
                                            group = 'Primary DPDs', metavar=dpdName.upper(), 
                                            help='DPD {0} output {1} file'.format(dpdType, dpdName))
                    elif 'AOD' in dpdType:
                        parser.add_argument('--output' + dpdName + 'File', 
                                            type=argFactory(trfArgClasses.argPOOLFile, multipleOK=multipleOK, type='AOD'), 
                                            group = 'Primary DPDs', metavar=dpdName.upper(), 
                                            help='DPD {0} output {1} file'.format(dpdType, dpdName))
                    elif 'ESD' in dpdType:
                        parser.add_argument('--output' + dpdName + 'File', 
                                            type=argFactory(trfArgClasses.argPOOLFile, multipleOK=multipleOK, type='ESD'), 
                                            group = 'Primary DPDs', metavar=dpdName.upper(), 
                                            help='DPD {0} output {1} file'.format(dpdType, dpdName))
                    else:
                        msg.warning('Unrecognised primary DPD type: {0}'.format(dpdName))
                    # Automatically add DPD as output data arguments of their relevant executors
                    if transform:
                        for executor in transform.executors:
                            if hasattr(executor, 'substep') and executor.substep in substep:
                                executor.outDataUpdate([dpdName])
                            if executor.name in substep:
                                executor.outDataUpdate([dpdName])

    except ImportError:
        msg.warning('PrimaryDPDFlags not available - cannot add primary DPD arguments')
            
## @brief Add top DAOD arguments
# @detailed Get the list of current top ADODs and add then to the parser
# optionally only some DPDs may be added, using the @c pick list.
# @param @c parser Argument parser object to add arguments to
# @param @c pick Optional list of DPD types to add (use short names, e.g., @c D2AODM_TOPJET)
def addTopPhysDAODArguments(parser, pick = None):
    parser.defineArgGroup('Top DAODs', 'Top Physics DAOD File Options')
    # TopPhysAllDAODs is a list of JobProperty type objects
    try:
        from TopPhysD2PDMaker.TopPhysD2PDFlags import TopPhysAllDAODs
        for dpdWriter in TopPhysAllDAODs:
            dpdName = dpdWriter.StreamName.replace('Stream', '')
            if pick == None or dpdName in pick: 
                parser.add_argument('--output' + dpdName + 'File', 
                                      type=argFactory(trfArgClasses.argFile, substep=['a2d']), group='Top DAODs',
                                      metavar=dpdName.upper(), help='Top ADOD output %s file (substep [a2d])' % (dpdName,))
    except ImportError:
        msg.warning('TopPhysD2PDFlags not available - cannot add D2PD arguments')

        
## @brief Add D3PD arguments
# @detailed Get the list of current D3PDs and add then to the parser
# optionally only some DPDs may be added, using the @c pick list.
# @param parser Argument parser object to add arguments to
# @param pick Optional list of DPD types to add (use short names, e.g., @c NTUP_EGAMMA)
# @param transform Transform object. DPD data types will be added to the correct executor (by name or substep)
# @param multipleOK If the @c multipleOK flag should be set for this argument
# @param addD3PDMRGtypes Instead of normal output types for D3PDs, add @em input NTUPs and
# @em output merge NTUPs
def addD3PDArguments(parser, pick = None, transform = None, multipleOK=False, addD3PDMRGtypes = False):
    parser.defineArgGroup('D3PD NTUPs', 'D3PD File Options')
    # listAllKnownD3PD is a list of D3PD JobProperty type objects
    try:
        from D3PDMakerConfig.D3PDProdFlags import listAllKnownD3PD
        for dpdWriter in listAllKnownD3PD:
            dpdName = dpdWriter.StreamName.replace('Stream', '')
           
            if pick == None or dpdName in pick: 
                if addD3PDMRGtypes:
                    parser.add_argument('--input' + dpdName + 'File', 
                                        type=argFactory(trfArgClasses.argNTUPFile, treeNames=dpdWriter.TreeNames, io='input'),
                                        group='D3PD NTUPs',
                                        metavar=dpdName.upper(), help='D3PD input {0} file )'.format(dpdName), nargs='+')
                    parser.add_argument('--output' + dpdName + '_MRGFile', 
                                        type=argFactory(trfArgClasses.argNTUPFile, treeNames=dpdWriter.TreeNames),
                                        group='D3PD NTUPs',
                                        metavar=dpdName.upper(), help='D3PD merged output {0} file )'.format(dpdName))
                else:
                    parser.add_argument('--output' + dpdName + 'File', 
                                        type=argFactory(trfArgClasses.argNTUPFile, treeNames=dpdWriter.TreeNames, multipleOK=multipleOK),
                                        group='D3PD NTUPs', metavar=dpdName.upper(),
                                        help='D3PD output {0} file (can be made in substeps {1})'.format(dpdName, ','.join(dpdWriter.SubSteps)))
                    # Automatically add D3PDs as data arguments of their relevant executors
                    if transform:
                        for executor in transform.executors:
                            if hasattr(executor, 'substep') and executor.substep in dpdWriter.SubSteps:
                                executor.outDataUpdate([dpdName])
                            if executor.name in dpdWriter.SubSteps:
                                executor.outDataUpdate([dpdName])
       
    except ImportError:
        msg.warning('D3PDProdFlags not available - cannot add D3PD arguments')


## @brief Add reduction framework output file argument
# @detailed This is the base name for the NTUP output streams. It will get appended
# to it the names of the specific reduced types.
# @param parser Argument parser object to add arguments to
# @param transform Transform object, to add reduced object to (maybe don't need that here?)
def addReductionArguments(parser, transform = None):
    parser.defineArgGroup('Reduction', 'Reduced Filetype Options')
    parser.add_argument('--inputNTUP_COMMONFile', 
                        type=argFactory(trfArgClasses.argNTUPFile, io='input', treeNames=['physics']),
                        group='Reduction', help='NTUP common input file', nargs='+')
    parser.add_argument('--outputDNTUPFile', 
                        type=argFactory(trfArgClasses.argNTUPFile, treeNames=None),
                        group='Reduction', help='Reduced NTUP output filename base')
    parser.add_argument('--outputDAODFile', 
                        type=argFactory(trfArgClasses.argPOOLFile, io='output', type='aod'),
                        help='Output reduced AOD filename base', group='Reduction')
    parser.add_argument('--reductionConf',
                        type=argFactory(trfArgClasses.argList), nargs='+', 
                        help='Configuration of reduced stream outputs', group='Reduction')
    parser.add_argument('--passThrough', type=argFactory(trfArgClasses.argBool), metavar="True/False",
                        help='Run the derivation framework in a pass-through mode, needed for some MC samples. Needs to be implemented in derivation JOs')


## @brief Simple class to store information about extra DPD filetypes
#  @detailed Implement this type of information as a class allowing for
#  extensions to be easily added and for some convenient heuristics
#  in the constructor 
class dpdType(object):

    ## @brief Class constructor for dpdType
    #  @param name The name of this datatype (e.g., @c DRAW_ZEE, @c NTUP_SCT)
    #  @param type The argFile.type (should be the major datatype, e.g. @c BS, @c ESD, @c AOD, etc.)
    #  @param substeps The substeps or executor names where this data can be made
    #  @param argclass The argument class to be used for this data
    #  @param treeNames For DPD types only, the tree(s) used for event counting (if @c None then 
    #  no event counting can be done.
    def __init__(self, name, type = None, substeps = [], argclass = None, treeNames = None):
        self._name = name
        
        ## @note Not very clear how useful this actually is, but we
        #  implement some heuristics here to avoid having to set
        #  the argFile.type always by hand
        if type is None:
            if 'RAW' in name:
                self._type = 'bs'
            elif 'ESD' in name:
                self._type = 'esd'
            elif 'AOD' in name:
                self._type = 'aod'
            elif 'NTUP' in name:
                self._type = 'ntup'
        else:
            self._type = type
            
        ## @note If not given explictly apply some heuristics, watch out for this
        #  if your data is made in a non-standard step
        if substeps == []:
            if 'RAW' in name:
                self._substeps = ['RAWtoESD']
            elif 'ESD' in name:
                self._substeps = ['RAWtoESD']
            elif 'AOD' in name:
                self._substeps = ['ESDtoAOD']
            elif 'NTUP' in name:
                self._substeps = ['ESDtoDPD', 'AODtoDPD']
        else:
            self._substeps = substeps
            
        ## @note Similarly if no explcit class is given we guess, YMMV
        if argclass is None:
            if 'RAW' in name:
                self._argclass = trfArgClasses.argBSFile
            elif 'ESD' in name:
                self._argclass = trfArgClasses.argPOOLFile
            elif 'AOD' in name:
                self._argclass = trfArgClasses.argPOOLFile
            elif 'NTUP' in name:
                self._argclass = trfArgClasses.argNTUPFile
        else:
            self._argclass = argclass
                
        self._treeNames = treeNames
            
    @property
    def name(self):
        return self._name
    
    @property
    def type(self):
        return self._type
    
    @property 
    def substeps(self):
        return self._substeps

    @property 
    def argclass(self):
        return self._argclass

    @property 
    def treeNames(self):
        return self._treeNames


## @brief Add additional DPD arguments
# @detailed Manually add DPDs that, for some reason, are not in any of the automated
# lists parsed by the companion functions above.
# @param parser Argument parser object to add arguments to
# @param pick Optional list of DPD types to add (use short names, e.g., @c NTUP_EGAMMA)
# @param transform Transform object. DPD data types will be added to the correct executor (by name or substep)
# @param multipleOK If the @c multipleOK flag should be set for this argument
def addExtraDPDTypes(parser, pick=None, transform=None, multipleOK=False):
    parser.defineArgGroup('Additional DPDs', 'Extra DPD file types')
    
    # Note on deprecated arguments c.f. Reco_trf:
    # TAG_COMM - doesn't produce any output so seems to be deprecated
    # {DESD,DAOD}_{ZEE,ZMUMU,WENU} are in fact just aliases for ESD or AOD
    
    # Build up a simple list of extra DPD objects
    extraDPDs = []
    extraDPDs.append(dpdType('NTUP_SCT', substeps=['r2e']))
    extraDPDs.append(dpdType('NTUP_MUFAST', substeps=['r2e']))
    extraDPDs.append(dpdType('NTUP_MUONCALIB', substeps=['r2e'], treeNames=['PatternNtupleMaker/Segments']))
    extraDPDs.append(dpdType('NTUP_TRKVALID', substeps=['r2e']))
    extraDPDs.append(dpdType('NTUP_FASTMON', substeps=['a2t','a2d','e2a']))
    extraDPDs.append(dpdType('NTUP_LARNOISE', substeps=['e2d'], treeNames=['CollectionTree']))
    extraDPDs.append(dpdType('NTUP_WZ', substeps=['e2d'], treeNames=['physics']))
    extraDPDs.append(dpdType('NTUP_TRT', substeps=['e2d'], treeNames=['MyCollectionTree']))
    extraDPDs.append(dpdType('NTUP_MCP', substeps=['e2d'], treeNames=['CollectionTree']))
    extraDPDs.append(dpdType('NTUP_HECNOISE', substeps=['e2d'], treeNames=['HECNoise']))
    extraDPDs.append(dpdType('NTUP_ENHBIAS', substeps=['e2d','e2a'], treeNames=['vertices']))
    extraDPDs.append(dpdType('NTUP_TRUTH', substeps=['a2d'], treeNames=['truth']))
    extraDPDs.append(dpdType('NTUP_SUSYTRUTH', substeps=['a2d'], treeNames=['truth']))
    extraDPDs.append(dpdType('NTUP_HIGHMULT', substeps=['e2a'], treeNames=['MinBiasTree']))
    extraDPDs.append(dpdType('NTUP_PROMPTPHOT', substeps=['e2d', 'a2d'], treeNames=["PAUReco","HggUserData"]))
    
    extraDPDs.append(dpdType('DESDM_BEAMSPOT'))
    extraDPDs.append(dpdType('DAOD_HSG2'))
    
    for dpd in extraDPDs:
        if pick == None or dpd.name in pick:
            msg.debug('Adding DPD {0} ({1}, {2}, {3}, {4})'.format(dpd.name, dpd.type, dpd.substeps, dpd.treeNames, dpd.argclass))
            # NTUPs are a bit special as they can take a treeName to count events
            if issubclass(dpd.argclass, trfArgClasses.argNTUPFile):
                parser.add_argument('--output' + dpd.name + 'File', 
                                    type=argFactory(dpd.argclass, multipleOK=multipleOK, type=dpd.type, treeNames=dpd.treeNames), 
                                    group = 'Additional DPDs', metavar=dpd.name.upper(), 
                                    help='DPD output {0} file'.format(dpd.name))
            else:
                parser.add_argument('--output' + dpd.name + 'File', 
                                    type=argFactory(dpd.argclass, multipleOK=multipleOK, type=dpd.type), 
                                    group = 'Additional DPDs', metavar=dpd.name.upper(), 
                                    help='DPD output {0} file'.format(dpd.name))
            if transform:
                for executor in transform.executors:
                    if hasattr(executor, 'substep') and executor.substep in dpd.substeps:
                        executor.outDataUpdate([dpd.name])
                    if executor.name in dpd.substeps:
                        executor.outDataUpdate([dpd.name])
    

def addFileValidationArguments(parser):
    parser.defineArgGroup('File Validation', 'Standard file validation switches')
    parser.add_argument('--skipFileValidation', '--omitFileValidation', action='store_true', 
                        group='File Validation', help='Skip both input and output file validation (warning - do not use this option in production jobs!)')
    parser.add_argument('--skipInputFileValidation', '--omitInputFileValidation', action='store_true', 
                        group='File Validation', help='Skip input file validation (warning - do not use this option in production jobs!)')
    parser.add_argument('--skipOutputFileValidation', '--omitOutputFileValidation', action='store_true', 
                        group='File Validation', help='Skip output file validation (warning - do not use this option in production jobs!)')
    parser.add_argument('--parallelFileValidation', type = argFactory(trfArgClasses.argBool),
                        group='File Validation', help='Parallelise file validation')

def addParallelJobProcessorArguments(parser):
    parser.defineArgGroup('Parallel Job Processor', 'Parallel Job Processor arguments')
    parser.add_argument('----parallelProcessPool', group='pool', type=argFactory(trfArgClasses.argInt, runarg=False), help='Number of processes in pool requested (int)')

def addValidationArguments(parser):
    parser.defineArgGroup('Validation', 'Standard job validation switches')
    parser.add_argument('--ignoreFiles', '--ignoreFilters', group='Validation', type=argFactory(trfArgClasses.argList, splitter=',', runarg=False), 
                        help='Files containing error patterns to be ignored during logfile scans (will split on commas; use "None" to disable the standard "atlas_error_mask.db")', nargs='+')
    parser.add_argument('--ignorePatterns', group='Validation', type=argFactory(trfArgClasses.argList, splitter=',', runarg=False), 
                        help='Regexp error patterns to be ignored during logfile scans (will be applied as a search against the whole logfile line)', nargs='+')
    parser.add_argument('--ignoreErrors', type=argFactory(trfArgClasses.argBool, runarg=False), metavar="BOOL", group='Validation',
                        help='Ignore ERROR lines in logfiles (use with care this can mask serious problems; --ignorePatterns is prefered)')
    parser.add_argument('--checkEventCount', type=trfArgClasses.argFactory(trfArgClasses.argSubstepBool, defaultSubstep = 'all', runarg=False),
                        help='Enable check of output events against input events (default: True)', group='Validation',
                        metavar="BOOL")

## @brief Add trigger related arguments
def addTriggerArguments(parser, addTrigFilter=True):
    parser.defineArgGroup('Trigger', 'Trigger Related Options')
    parser.add_argument('--triggerConfig',
                        type=argFactory(trfArgClasses.argSubstep, defaultSubstep="RAWtoESD", separator='='), 
                        metavar='substep=triggerConf',
                        help='Trigger configuration string (substep aware argument - default is to run trigger in RAWtoESD step, '
                        'use syntax SUBSTEP=TRIGCONF if you want to run trigger somewhere else). '
                        'N.B. This argument uses EQUALS (=) to separate the substep name from the value.', 
                        group='Trigger')
    if addTrigFilter:
        parser.add_argument('--trigFilterList',
                            type=argFactory(trfArgClasses.argList), nargs="+",
                            help='Trigger filter list (multiple values can be given separately or split on commas; only understood in RAWtoESD)', 
                            group='Trigger')

## Tea for two and two for tea... these arguments are used for testing
def addTeaArguments(parser):
    parser.defineArgGroup('Tea', 'Tea Making Options (for testing)')
    parser.add_argument('--cupsOfTea', group='Tea', type=argFactory(trfArgClasses.argInt), help='Number of cups of tea requested (int)')
    parser.add_argument('--teaType', group='Tea', type=argFactory(trfArgClasses.argString), help='Requested flavour of tea (string)')
    parser.add_argument('--mugVolume', group='Tea', type=argFactory(trfArgClasses.argFloat), help='How large a cup to use (float)')
    parser.add_argument('--drinkers', group='Tea', nargs='+', type=argFactory(trfArgClasses.argList), help='Who is drinking tea (list)')

## @brief This method adds the current valid list of D3PDs to two lists
def listKnownD3PDs():
    inputD3PDList = []
    outputD3PDList = []
    from D3PDMakerConfig.D3PDProdFlags import listAllKnownD3PD
    for dpdWriter in listAllKnownD3PD:
        dpdName = dpdWriter.StreamName.replace('Stream', '')
        inputD3PDList.append(dpdName)
        outputD3PDList.append(dpdName+'_MRG')
    return inputD3PDList, outputD3PDList
   
