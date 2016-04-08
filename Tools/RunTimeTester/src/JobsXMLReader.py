#! /usr/local/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Part of the parser for the unfied test configuration file.
Handles jobs for  the RTT part of the file,
Returns a list of JobDescriptors

If there is no extension to the jobOptions file name,
'.py' is understood. Thus, to use the old '.txt' option files, supply
the options file with the .txt extension attatched to the job options file name.

Python Modules used:
      xml.dom.minidom
      sys
      os.path
      string

RTT Modules used:
      Descriptor
      AthenaJobDescriptor
      findPakages

Usage:
     
"""

import xml.dom.minidom, sys, os.path, string, imp, copy

# from Descriptor               import Descriptor # now use factory to create
# from AthenaJobDescriptor      import AthenaJobDescriptor # now use factory to create
# from ReleaseJobDescriptor     import ReleaseJobDescriptor # now use factory to create
from findPackages             import findPackages
from RTTSException            import RTTInputError
from RTTSException            import RTTCodingError
from formatCollection         import formatCollection
from exc2string2              import exc2string2
from findRTThome              import findRTThome
from copy                     import deepcopy
from Factory_DataSet          import DataSetFactory, DQ2DataSet
from XMLTools                 import getText, getElementNodes, nodeToString
from JobGroupsParser          import handleUserTestTag,list2dict,handleKeepFileTag, handleAuxFileTag
from Evaluate                 import Evaluate

import logging
logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------
class UseToPackage:
    def __init__(self, releasePath):
        self.releasePath = releasePath

    def convert(self, use):
        pack = None
        tkns = use.split()
        
        # if len(tkns) == 5:
        #     pack = os.path.join(self.releasePath, tkns[3], tkns[1], tkns[2])
        # if len(tkns) == 4:
        #     pack = os.path.join(self.releasePath, tkns[1], tkns[2])

        # example of a use:
        # 'use AMI AMI-02-00-07 Database/Bookkeeping (/afs/cern.ch/atlas/software/dist/10.3.0/)
        if len(tkns) == 5:
            pack = os.path.join(tkns[3], tkns[1], tkns[2])
        if len(tkns) == 4:
            pack = os.path.join(tkns[2])

        return pack




# from MethodTracer        import MetaTracer
# __metaclass__ = MetaTracer


class TryIt:
    def __init__(self, job, dict):
        self.job = job
        
        self.dict = dict
        self.error = False
        self.messages = []

    def runFn(self, fn, tag):
        try:
            fn(self.job.getElementsByTagName(tag), self.dict)
        except:
            self.error = True
            m = 'Error building descriptor in method %s\nTraceback:%s' % (fn.__name__, exc2string2())
            self.messages.append(m)

    def runXPathFn(self, fn, xpath):
        try:
            fn(Evaluate(xpath, self.job), self.dict)
        except:
            self.error = True
            m = 'Error building descriptor in method %s\nTraceback:%s' % (fn.__name__, exc2string2())
            self.messages.append(m)
        
    def handleSimpleTextTag(self, (tagName, dictKey) ):
        nodes = self.job.getElementsByTagName(tagName)
        if nodes: self.dict[dictKey] = string.strip(getText(nodes[0].childNodes))

    def handleSimpleBoolTag(self, tagName):
        nodes = self.job.getElementsByTagName(tagName)
        if nodes: self.dict[tagName] = True


class SimpleTagHandler:
    """Add an entry to the arg dict with tagname as key, when you know
    you want their is only one element with name = tagname"""

    def __init__(self, dict, logger):
        self.dict = dict

    def doit(self, node, tagName):
        print node.nodeName, tagName
        try:
            nodes = node.getElementsByTagName(tagName)
            if not nodes: return
            node = nodes[0]
            self.dict[tagName] = getText(node.childNodes)

        except:
            m  = """Could not retrieve text for tagname %s, ignoring
            Traceback:\n %s""" % (tagName, exc2string2())
            logger.error(m)

from Tracer import Tracer
                    

#def updateJobTransformCmdWithDataSets(datasetsDict, transformCmd):
#    """grep the command for logical data set names and replace them with the
#    physical data set path.
#    """
#    for dsType in datasetsDict.keys():
#        dsInstances = datasetsDict[dsType]
#        for dsInst in dsInstances:
#            physicalName = dsInst.physical
#            logicalName  = dsInst.logical
#            if logicalName:
#                transformCmd = transformCmd.replace(logicalName,physicalName)
#    return transformCmd
    

class JobsXMLReader:
    def __init__(self, parentElement, rttPilotJobs, packageName, dataSetCatalog, chainStoreMaker, logger, descFactory):
        # parentElement = <jobList>
        logger.debug('JobsXMLReader init')
        self.rttPilotJobs       = rttPilotJobs
        self.packageName        = packageName
        self.dataSetCatalog     = dataSetCatalog
        self.chainStoreMaker    = chainStoreMaker
        self.chainStoreName     = None # will be filled in when processing Chain tags
        self.descriptors        = []
        self.classificationTags = None # the "apply to all jobs" classification
        self.descFactory        = descFactory
        
        self.makeJobDescriptors(parentElement)
        
        
    def getDescriptors(self): return self.descriptors
        
    # -------------------------------------------------------------------

    def makeJobDescriptors(self, parentElement):
        # store the list of global classification tags that will be
        # applied to (currently) athena jobs (in theory 0 or 1 tag will be in the list)
        # Jobs can locally provide their own tag where:
        #      local empty classification tag ---> no CPC for that job
        #      local non-empty classification tag ---> local CPC for that job
        self.classificationTags = Evaluate('classification', parentElement)
            
        athenaTags              = Evaluate('athena',        parentElement)
        jobTransformTags        = Evaluate('jobTransform',  parentElement)
        chainTags               = Evaluate('chain',         parentElement)
        pseudoTags              = Evaluate('pseudo',        parentElement) # make dummy descs for testing

        nChainedAthenaJobs = 0
        for n in chainTags:
            nChainedAthenaJobs += len (n.getElementsByTagName('athena'))

        nChainedJobTransforms = 0
        for n in chainTags:
            nChainedJobTransforms += len (n.getElementsByTagName('jobTransform'))
        
        logger.debug('no of athena jobs:              %d' % len(athenaTags))
        logger.debug('no of job transform jobs:       %d' % len(jobTransformTags))
        logger.debug('no of job chains:               %d' % len(chainTags))
        logger.debug('no of chained athena jobs:      %d' % nChainedAthenaJobs)
        logger.debug('no of chained job transforms:   %d' % nChainedJobTransforms)
        logger.debug('no of pseudo chains:            %d' % len(pseudoTags))

        descriptors = []
        if self.rttPilotJobs:
            descriptors  += self.handleAthenaJobs   (athenaTags,   'AthenaPilot')
        descriptors  += self.handleAthenaJobs       (athenaTags,   'Athena')

        descriptors += self.handleJobTransformJobs (jobTransformTags)
        descriptors += self.handleChains           (chainTags)
        descriptors += self.handlePseudoJobs       (pseudoTags)

        logger.debug('no of descriptors:  %d' % len(descriptors))
    
        self.descriptors = descriptors

    # ####################################################################################
    #
    # Handlers for jobs tags that are not specifics to a type of job
    #
    # ####################################################################################
    
    def handleQueues(self, queues, dict):
        logger.debug('queues ...'+str(queues))
        if queues:
            dict['suggestedQueue']  =string.strip(getText(queues[0].childNodes))
              

            
    # ####################################################################################
    #
    # Handlers for jobs that set attributes in Athena job descriptors
    #
    # ####################################################################################
    

    def handleAthenaJobs(self, jobs, factoryParameter):
        """loops over all athena tags to make athena descriptors"""
    
        jobList = []

        for job in jobs:
            try:
                jobList.append(self.handleAthenaJob(job, factoryParameter))
            except Exception, e:
                logger.error('Error processing athena tags for descriptor type %s, skipping job\nTraceback:\n%s' % (factoryParameter,
                                                                                                                    exc2string2()))
        return jobList


    def handleAthenaJob(self, job, factoryParameter):
        """
        Returns a list of job descriptors and pilot job descriptors
        """
        
        logger.debug("Athena job  "+str(job))

        argDict = {}
        tryIt = TryIt(job, argDict)

        names = {
            'commandLineFlags': 'commandLineFlags',
            'commandLineString': 'commandLineString',        
            'confederation':    'confederation',
            'queue':            'suggestedQueue',
            # 'mode':             'mode',
            'doc':              'jobDocString',
            'doclink':           'jobDocURL',
            'displayName':      'jobDisplayName',
            'athenaFlavour':     'athenaFlavour',
            }

        [tryIt.handleSimpleTextTag(item) for item in names.items()]

        names = ['numberedOnly', 'limitedNightlies', 'athenaCommonFlags']
        [tryIt.handleSimpleBoolTag(tag) for tag in names]

        # more complicated stuff
        tryIt.runFn(self.handleAthenaGroups,            "group")
        tryIt.runFn(self.handleAthenaPreconfigOptions,  "preconfigOptions")
        tryIt.runFn(self.handleAthenaOptions,           "options")
        tryIt.runFn(self.handleFurtherAthenaOptions,    "furtherOptions")
        tryIt.runFn(self.handleAthenaDataSets_OldStyle, "dataset")
        tryIt.runXPathFn(self.handleAthenaDataSets,     "dataset_info")
        tryIt.runFn(self.handleDQ2DataSets,             'dq2')
        tryIt.runFn(self.handleAction,                  "action")
        tryIt.runFn(self.handleTest,                    "test")
        tryIt.runFn(self.handleClassification,          "classification")
        tryIt.runFn(self.handleCASTORSvcClass,          "castorSvcClass")        
        tryIt.runFn(self.handleBatchWallTime,           "batchWallTime")        
        tryIt.runXPathFn(self.handleChainDataSets,      "chaindataset_info")
        
        #chaindatasets = job.getElementsByTagName('chaindataset_info')
        #self.handleChainDataSets(chaindatasets, argDict)

        keepFileEls = job.getElementsByTagName('keepFilePattern')
        argDict['keepFilePatterns'] = [handleKeepFileTag(tag) for tag in keepFileEls]

        auxFileEls = job.getElementsByTagName('auxFilePattern')
        argDict['auxFilePatterns'] = [handleAuxFileTag(tag) for tag in auxFileEls]

        # rationalise multiple job options
        argDict['jobOptions'].extend(argDict['furtherJobOptions'])
        del argDict['furtherJobOptions']
        
        argDict['hashString'] = hash(self.packageName+nodeToString(job))


        # get the long term trend analysis (CoolDozer) id if it exists
        argDict['trendId'] = job.getAttribute('trendId')
        
        jd = self.descFactory.create(factoryParameter, argDict)

        if tryIt.error:
            jd.constructionError = True
            jd.errorMessages = tryIt.messages


        return jd

    # ####################################################################
    # Extract the values of the athena child tags
    # Expected tags are:
    #          <group>       - mandatory
    #          <package>     - mandatory
    #          <options>     - mandatory
    #          <queue>       - mandatory
    #          <dataset>     - optional
    # ###################################################################

    def handleAthenaGroups(self, groups, dict):
        # KLUDGE: Athena added to job group to make it the same as the
        #         name of job group definition version
        
        logger.debug('groups '+str(groups))
        if groups:
            groupName = string.strip(getText(groups[0].childNodes))
            if not groupName.startswith('Athena'): groupName = 'Athena'+groupName
            dict['jobGroup'] = groupName

    # -------------------------------------------------------------------------------------

    def handleBatchWallTime(self, walltime, dict):
        if not walltime: return
        dict['suggestedBatchWallTime'] = getText(walltime[0].childNodes)

    # -------------------------------------------------------------------------------------

    def handleClassification(self, classification, dict):
        # self.classificationTags = list of global level <classification> tags
        # classification = list of local-to-this-job level <classification> tags            
        def validateCPCvalue(tagName, tagValues):
            legalValues = {'displayClass'     : ['OfflineValidation', 'OnlineValidation', 'Development'],
                           'displayProcess'   : ['EvGen', 'Simul', 'Digi', 'Reco', 'Core'],
                           'displayComponent' : ['Athena-Core',
                                                 'Det-InDet', 'Det-Muon', 'Det-LAr', 'Det-TileCal', 'Det-Combined',
                                                 'Trig-Electron', 'Trig-Photon', 'Trig-Tau', 'Trig-ETMiss',
                                                 'Trig-Bphys', 'Trig-Btag', 'Trig-Cosmics', 'Trig-Common', 'Trig-Other',
                                                 'Perf-Egamma', 'Perf-CombMuon', 'Perf-JetEtmiss', 'Perf-Tau',
                                                 'Perf-TrkVtx', 'Perf-Btag', 'Phys-Top', 'Phys-Higgs', 'Phys-Susy',
                                                 'Phys-StandardModel', 'Phys-Exotics', 'Phys-B', 'Phys-HeavyIon']
                           }

            for tagValue in tagValues: 
                if tagValue not in legalValues[tagName]: 
                    m  = "Invalid CPC tag name/value: %s, %s\n" % (tagName, tagValue)
                    m += "Legal values are: %s" % str(legalValues[tagName])
                    raise RTTInputError(m)

        def extractChildren(parentNode):
            childTags = {}
            for tag in ['displayClass', 'displayProcess', 'displayComponent']:
                values = [Evaluate('text()', v) for v in Evaluate(tag, parentNode)]
                validateCPCvalue(tag, values)
                childTags[tag] = values
            return childTags

        def addChildrenToDict(parentNode, dict):
            dict.update(extractChildren(parentNode))

        if not classification: # no local job, use global if exists
            if not self.classificationTags or Evaluate('none', self.classificationTags[0]):
                return
            else:
                addChildrenToDict(self.classificationTags[0], dict)
        else:
            # there exists a local CPC tag for this job
            # empty version? ==> switch off the global CPC tag if it exists
            if Evaluate('none', classification[0]):
                return
            # non-empt y version
            else:
                addChildrenToDict(classification[0], dict)
    
    # -------------------------------------------------------------------------------------

    def handleAthenaPreconfigOptions(self, preconfigs, dict):
        dict['preconfigJobOptions'] = []
        if preconfigs:
            dict['preconfigJobOptions'] = [string.strip(
                self.fixAthenaJobOptions(getText(preconfigs[0].childNodes)))]

    # -------------------------------------------------------------------------------------
    
    def handleAthenaOptions(self, options, dict):
        logger.debug('options '+str(options))
        dict['jobOptions'] = []
        if options:
            dict['jobOptions'] = [string.strip(
                self.fixAthenaJobOptions(getText(options[0].childNodes)))]

    # -------------------------------------------------------------------------------------
    
    def handleFurtherAthenaOptions(self, furtherOptions, dict):
        # there can be only one joboptions tag.
        # these tags allow chaining of athena jobs. Would have been
        # better had jobOptions been a list from the begining.
        
        logger.debug("furtherOptions" +str(furtherOptions))
        dict['furtherJobOptions']  =  []
        if furtherOptions:
            tags =  furtherOptions[0].getElementsByTagName("jobOptionItem")
            dict['furtherJobOptions']  =  [getText(jot.childNodes)
                                           for jot in tags]

    # -------------------------------------------------------------------------------------
        
    def getPhysicalDataset(self, logicalDataSetName):
        
        if not self.dataSetCatalog.has_key(logicalDataSetName):
            msg =  'logical data set name not known to  catalog %s' % logicalDataSetName
            raise RTTInputError(msg)

        resDataSetName = self.dataSetCatalog.get(logicalDataSetName)
        if not resDataSetName:
            msg = 'The logical data set name:\n' + str(logicalDataSetName) + '\n maps to no physical data set in the catalog!'
            #  msg =  'physical data set name not available %s' % resDataSetName
            raise RTTInputError(msg)
    
        return resDataSetName

    # -------------------------------------------------------------------------------------

    def handleDQ2DataSets(self, dq2Tags, dict):
        logger.debug('handleDQ2Datasets start')
        
        dsObjs = []
        type   = ''
        for tag in dq2Tags:
            dsName, dq2files, type = self.handleDQ2Tag(tag)
            physicalFiles             = self.getDQ2PhysicalFiles(dsName, dq2files)
            dsFactory  = DataSetFactory(logger)
            for fn in physicalFiles:
                dsInstance = dsFactory.create(type, fn, fn, 'dq2') # fn = physical = logical
                dsObjs.append(dsInstance)
            
        if not dict.has_key('datasets'):
            dict['datasets'] = {}
        dict['datasets'].setdefault(type,[]).extend(dsObjs)
        logger.debug('handleDQ2Datasets end')
        
            
    def handleDQ2Tag(self, tag):
        siteTag   = tag.getElementsByTagName('dq2site')
        dq2site   = getText(siteTag[0].childNodes)
        dsTag     = tag.getElementsByTagName('dq2dataset')
        dq2dsName = getText(dsTag[0].childNodes)
        typeTag   = tag.getElementsByTagName('datatype')
        type = getText(typeTag[0].childNodes)
        fileTags  = tag.getElementsByTagName('dq2file')
        dq2files  = []
        for t in fileTags:dq2files.append(getText(t.childNodes))
        return (dq2dsName, dq2files, type)


    def getDQ2PhysicalFiles(self, dsName, dq2files):

        physFiles = []
        for file in dq2files:
            file = os.path.join(dsName, file)
            if not self.dataSetCatalog.has_key(file):
                msg =  'logical data set name not known to  catalog %s' % file
                raise RTTInputError(msg)
            physF = self.dataSetCatalog.get(file)
            if not physF:
                msg  = 'The logical data set name:\n' + str(file)
                msg += '\n maps to no physical data set in the catalog!'
                #  msg =  'physical data set name not available %s' % resDataSetName
                raise RTTInputError(msg)
            physFiles.append(physF)
            
        return physFiles
    #--------------------------------------------------------------------------------------

    def handleAthenaDataSets_OldStyle(self, datasets, dict):
        """
        Old dataset declaration: <dataset>/blah/blah</dataset>
        Old style means dc2 data.
        """

        if not datasets: return
        
        logger.debug('Processing ' + str(len(datasets)) + ' old style <dataset> tags')

        datasetInstanceList = []
        
        datasetType   = 'dc2'
        datasetRepeat = 1

        for dsElementNode in datasets:

            logicalDataSetName = string.strip(getText(dsElementNode.childNodes))
            physDataSetName = self.getPhysicalDataset(logicalDataSetName)

            dsFactory = DataSetFactory(logger)            
            datasetInstance = dsFactory.create(datasetType,physDataSetName,physDataSetName, 'standard') # physical = logical
            datasetInstanceList.append(datasetInstance)

        if not dict.has_key('datasets'):
            dict['datasets'] = {}
        dict['datasets'].setdefault(datasetType,[]).extend(datasetInstanceList)
            
    # -------------------------------------------------------------------------------------
                
    def handleAthenaDataSets(self, datasets, dict):
        """New dataset declaration:
              <dataset_info>
                 <dc2/>
                 <datasetName>/castor/blah/blah</dataset>
                 <datasetRepeat>5</datasetRepeat>
              </dataset_info>

              The repeat tag is optional (defaults to 1). Known data type tags, currently: <dc2/> and <bytestream/>.
        """

        if not datasets: return
        
        datasetsDict = {}

        logger.debug('Processing ' + str(len(datasets)) + ' <dataset_info> tags')
        [self.handleAthenaDataSet(datasetNode, datasetsDict) for datasetNode in datasets]

        if not dict.has_key('datasets'):
            dict['datasets'] = {}

        [dict['datasets'].setdefault(datasetType,[]).extend(datasetsDict[datasetType]) for datasetType in datasetsDict.keys()]
        
    # -------------------------------------------------------------------------------------
    
    def handleAthenaDataSet(self, node, dict):
        datasetType   = getElementNodes(node.childNodes)[0].tagName
        datasetName   = getText(node.getElementsByTagName('datasetName')[0].childNodes)                
        datasetRepeat = self.getDatasetRepeat(node)            

        physDataSetName = self.getPhysicalDataset(datasetName)
        
        dsFactory = DataSetFactory(logger)
        dataSetInstance = dsFactory.create(datasetType,physDataSetName,physDataSetName,'standard') # physical = logical
        [dict.setdefault(datasetType,[]).append(dataSetInstance) for i in range(datasetRepeat)]
        
        logger.info('handleAthenaDataset no of data sets %d' % (len(dict)))

    # -------------------------------------------------------------------------------------

    def getDatasetRepeat(self,datasetTag):

        datasetRepeat = 1 # default
        
        repeatTags = datasetTag.getElementsByTagName('datasetRepeat')
        if not repeatTags: return datasetRepeat
        
        datasetRepeat = getText(repeatTags[0].childNodes)
            
        try:
            datasetRepeat = int(datasetRepeat)
        except:
            msg  = 'The tag <datasetRepeat> must take a non-zero positive integer as content. Instead, found: ' + str(datasetRepeat) + '\n'
            msg += 'This Athena job will now be rejected.\n'
            logger.error(msg)
            raise RTTInputError(msg)

        return datasetRepeat
        
    # -------------------------------------------------------------------------------------
    
    def fixAthenaJobOptions(self, jo):
        base,ext = os.path.splitext(jo)
    
        if ext =='' :jo=jo+'.py'
        return jo

    # -------------------------------------------------------------------------------------
                
    def handleChainDataSets(self, datasetNodes, dict):
        """New dataset declaration:
              <chaindataset_info>
                 <dc2/>
                 <chaindatasetName>/castor/blah/blah</dataset>
                 <datasetRepeat>5</datasetRepeat>
                 <dataset_info>... </datasetinfo> <!-- fall back file -->
              </chaindataset_info>

              The repeat tag is optional (defaults to 1). Known data type tags, currently: <dc2/> and <bytestream/>.
        """

        if not datasetNodes: return
        
        chainDataSets = []

        logger.debug('Processing ' + str(len(datasetNodes)) + ' <chaindataset_info> tags')

        [chainDataSets.append(self.handleChainDataSet(node)) for node in datasetNodes]


        dict['chainStoreFiles'] = chainDataSets

    # -------------------------------------------------------------------------------------
    
    def handleChainDataSet(self, node):
        """
        Returns a tuple (datsetInstance, datasetDictionary)
        the datasetInstancer represents the chain file to be used.
        Its existence is not checked (this allows deferral: 
        previous chain jobs may reate the dataset).
        The dataset dictionary represents the fallback file(s).
        It may have a number of entries of form 
        'datasetType': [dataset instances]'
        """
        
        def getFallBackFiles(logical):
            """
            PS 11/02/09
            All fallback datasets are AthenaDatasets
            return a dictionary of fallback datasets of the form
            {'datasetType': [dataset objects]}
            """
            fallBackFileNodes = Evaluate('dataset_info', node)
            logger.debug('%d fallback file node(s) found' % len(fallBackFileNodes))
            tempDict = {}

            # obtain a dict of form {'datasets': {dataset dictionary}}
            self.handleAthenaDataSets(fallBackFileNodes, tempDict)

            if len(tempDict)==0: logger.warning('No valid fallback files found')
            dictionary = tempDict['datasets'] # get the inner dictionary
            for dsInsts in dictionary.values(): # why?
                for ds in dsInsts:
                    logger.info('Fallback file found, name %s' % str(ds))
                    ds.logical = logical
                    ds.origin = 'fallback'
            return dictionary


        # ---- Method proper starts here ----

        datasetType     = getElementNodes(node.childNodes)[0].tagName
        datasetName     = Evaluate('chaindatasetName/text()', node) # logical name

        #if not self.chainStoreArea:
        #    msg = 'Trying to use a chain file but no self.chainStoreArea\n'
        #    msg = 'Perhaps using a chainfile outside of a chain.'
        #    msg = 'Chain file name = %s' % datasetName
        #    logger.error(msg)
        #    useFallBackFile(localDict, datasetName)
        #    dict['datasets'] = localDict['datasets']
        #    return

        #physDataSetName = os.path.join(self.chainStoreArea, datasetName)
        #if not (os.path.exists(physDataSetName)):
        #    msg = 'chain file %s does not exist' % physDataSetName
        #    logger.error(msg)
        #    logger.error(msg)
        #     useFallBackFile(localDict, datasetName)
        #    dict['datasets'] = localDict['datasets']
        #    return

        physicalDatasetName = 'unknown'
        datasetInstance = DataSetFactory(logger).create(datasetType, physicalDatasetName, datasetName, 'chainstore')
        fallBackFileDict = getFallBackFiles(datasetName)
        
        return (datasetInstance, fallBackFileDict)
        # dict.setdefault('datasets', {})
        # dataSetInstance = DataSetFactory(logger).create(datasetType, physDataSetName, datasetName)
        # dict['datasets'].setdefault(datasetType,[]).append(dataSetInstance)

    # -------------------------------------------------------------------------------------
    

    def handleCASTORSvcClass(self, svcClass, dict):
        if not svcClass: return        
        dict['castorSvcClass'] = Evaluate('text()', svcClass[0]) # ignore all but first one
        
    def handleAction(self, actions, dict):
        if not actions: return
        dict['actions'] = [handleUserTestTag(action) for action in actions]
            
    def handleTest(self, tests, dict):
        if not tests: return
        dict['tests'] = [handleUserTestTag(test) for test in tests]

    def handleJobTransformJobs(self, jobs):
        jobList=[]
        for job in jobs:
            try:
                jobList.append(self.handleJobTransformJob(job))
            except:
                m = 'Error creating a JobTransform Descriptor. Traceback\n%s' % exc2string2()
                logger.error(m)
                raise RTTCodingError(m)
        return jobList
                    
    def handleJobTransformJob(self, job):

        names = {
            'group'               : 'jobGroup',
            'doc'                 : 'jobDocString',
            'queue'               : 'suggestedQueue',
            'jobTransformCmd'     : 'jobTransformCmd',
            'jobTransformJobName' : 'jobTransformJobName'
            }
        
        argDict = {}

        tryIt = TryIt(job, argDict)
        [tryIt.handleSimpleTextTag(item) for item in names.items()]

        names = ['numberedOnly', 'limitedNightlies']
        [tryIt.handleSimpleBoolTag(tag) for tag in names]

        tryIt.runFn(self.handleClassification, "classification")
        tryIt.runFn(self.handleTest,           "test")
        tryIt.runFn(self.handleAction,         "action")
        tryIt.runFn(self.handleCASTORSvcClass, "castorSvcClass")        
        tryIt.runFn(self.handleBatchWallTime,  "batchWallTime")

        keepFileEls = job.getElementsByTagName('keepFilePattern')
        argDict['keepFilePatterns'] = [handleKeepFileTag(tag) for tag in keepFileEls]

        auxFileEls = job.getElementsByTagName('auxFilePattern')
        argDict['auxFilePatterns'] = [handleAuxFileTag(tag) for tag in auxFileEls]

        # grab the datasets if there are any
        # Note: re-using the below method even though it's badly named.
        # It should be changed to a more generic name.
        # self.handleAthenaDataSets(job.getElementsByTagName('dataset_info'),dict)
        tryIt.runXPathFn(self.handleAthenaDataSets,          "dataset_info")
        argDict['hashString'] = hash(self.packageName+nodeToString(job))

        # get the long term trend analysis (CoolDozer) id if it exists
        argDict['trendId'] = job.getAttribute('trendId')

        localError    = False
        localErrorMsg = []
        tryIt.runXPathFn(self.handleChainDataSets, "chaindataset_info")
        
        jd = self.descFactory.create('JobTransform', argDict)

        if tryIt.error:
            jd.constructionError = True
            jd.errorMessages.extend(tryIt.messages)
        if localError:
            jd.constructionError = True
            jd.errorMessages.extend(localErrorMsg)
            
        return jd

    def handleChains(self, jobs):
        jobList=[]
        for job in jobs:
            jobList.append(self.handleChain(job))
        return jobList
    
    def handleChain(self, node):
        # DTD guarantees there is one of <parallel>, <sequential>
        # or chainElement
        chainName = Evaluate('chainName/text()', node).replace(' ', '_') # protect against whitespace
        self.chainStoreArea = self.chainStoreMaker.chainStorePath(chainName)

        nodes = Evaluate('sequential', node)
        if nodes:
            desc = self.handleSequentialNode(nodes[0])
            return desc

        nodes = Evaluate('parallel', node)
        if nodes:
            desc = self.handleParallelNode(nodes[0])
            return desc

        nodes = Evaluate('ChainElement', node)
        if nodes:
            desc = self.handleChainElementNode(nodes[0])
            return desc

    def handleSequentialNode(self, node):
        desc = self.handleContainerNode('Sequential', node)
        return desc
    
    def handleParallelNode(self, node):
        desc = self.handleContainerNode('Parallel', node)
        return desc

    def handleChainElementNode(self, node):
        jd = self.descFactory.create('Chain')
        # DTD gaurantees 1 of  athena, jobTransforms or pseudo jobs only

        desc = None
        nodes = Evaluate('athena', node)
        if nodes:
            desc = self.handleAthenaJob(nodes[0], 'Athena')
            jd.addDescriptor(desc)

        if not desc:
            nodes = Evaluate('jobTransform', node)
            if  nodes:
                desc = self.handleJobTransformJob(nodes[0])
                jd.addDescriptor(desc)

        if not desc:
            nodes = Evaluate('pseudo', node)
            if  nodes:
                desc = self.handlePseudoJob(nodes[0])
                jd.addDescriptor(desc)

        desc.chainStoreArea = self.chainStoreArea

        outFileNodes = Evaluate('chainfileout', node)

        jd.setOutFiles([Evaluate('text()', n) for n in outFileNodes])
        return jd

    def handleChainFileOutNodes(self, nodes):
        return [Evaluate('text()', n) for n in nodes] 
        
    

    def handleContainerNode(self, descName, node):

        #descName is a class name starts with uppercase 
    
        jd = self.descFactory.create(descName)

        # keys are xml node names (lower case)
        handlers = {
            'sequential':   self.handleSequentialNode,
            'parallel':     self.handleParallelNode,
            'chainElement': self.handleChainElementNode
            }

        for cnode in node.childNodes:
            nodeName = cnode.nodeName
            logger.info('%s Descriptor  checking child node %s' % (descName, nodeName))
            if nodeName not in handlers.keys(): continue
            logger.info('%s Descriptor  handling child node %s' % (descName, nodeName))
            desc = handlers[nodeName](cnode)
            desc.chainStoreArea = self.chainStoreArea
            jd.addDescriptor(desc)
            logger.info('%s Descriptor  adding child descriptor %s' % (descName, desc.__class__.__name__))

        jd.chainStoreArea = self.chainStoreArea
        return jd

    def handlePseudoJobs(self, jobs):
        jobList=[]
        for job in jobs:
            desc = self.handlePseudoJob(job)
            jobList.append(desc)
        return jobList
    
    def handlePseudoJob(self, node):

        desc = self.handleAthenaJob(Evaluate('athena', node)[0], 'Pseudo') # pseudo has a single an athena node
        return desc
        #argDict = {}


class JobsXMLReaderATN:
    def __init__(self, ATNElement, rttPilotJobs, packageName, dataSetCatalog, chainStoreMaker, logger, descFactory):

        self.descFactory = descFctory
        self.logger      = logger
        self.descriptors = []

        self.makeJobDescriptors(ATNElement)
        
    def getDescriptors(self): return self.descriptors

    def makeJobDescriptors(self, element):
        # get the ATN TEST elements
        testElements = Evaluate('TEST', element)
        testElements = [t for t in testElements if t.getAttribute('type') == 'athena']
        
        if testElements:
            self.descriptors.append(self.makeAthenaJobDescriptor(testElements))

    def makeAthenaJobDescriptor(self, elements):

        # The following line gets the text from the options_atn tag.
        # This, conforming with other ATLAS usage, is in the form
        # package/optionsfile name - if you give this to athena, it
        # will find it. Currently, in the RTT, we find the full path
        # to the options files ourselves - which lets up pick up
        # options from the runpath.
        # here, we extract the fill name part for processing in the RTT way.

        def getOptionsAndScripts(elem):
            jobOptionsEls = Evaluate('options_atn', elem)
            prescriptEls  = Evaluate('prescript', elem)

            jobOptions = [getText(e.childNodes) for e in jobOptionsEls]
            prescripts = [getText(e.childNodes) for e in prescriptEls]
            return (jobOptions,prescripts)
            
        optionsAndScripts = [getOptionsAndScripts(e) for e in elements]
        jobOptions = []
        [jobOptions.extend(jo[0]) for jo in optionsAndScripts]

        prescripts = []
        [prescripts.extend(ps[1]) for ps in optionsAndScripts]


        argDict = {
            'suggestedQueue'    : 'short',
            'jobGroup'          : 'AthenaATN',      
            'optionsAndScripts' : optionsAndScripts,
            'jobOptions'        : jobOptions,
            'prescripts'        : prescripts,
            'rttATNJob'         : True,
            'atnName'           : elements[0].getAttribute('name')
            }


        desc = self.descFactory.create('AthenaATN',
                                       argDict)
        logger.debug('JobsXMLReader::JobsXMLReaderATN::makeAthenaJobDescriptor %s' % desc.identifiedName)
        # logger.debug(str(desc))
        return desc
        
        
        
        
if __name__ == "__main__":
    pass
