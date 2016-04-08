# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
File for parsiing the job group information in the RTT part of the unified
test configuration file
"""

import sys,imp, types, os.path

import xml.dom.minidom
from JobGroupKit        import JobGroupKit
from Checkers           import SimpleFileDiffer, SimpleLogChecker, ChainedJobsLogChecker
from Checkers           import HistogramDiffer
from formatCollection   import formatCollection
from validateXMLFile    import Validator
from UserTestDescriptor import UserTestDescriptor
from Evaluate           import Evaluate

import childrenToDescendants

# --------------------------------------------------------
import logging
logger = logging.getLogger('rtt')
# --------------------------------------------------------
def makeKits(element):
    "Get the list of groups"
    groups = Evaluate('jobGroup', element)
    kits = [handleGroup(group) for group in groups]
    return kits

def handleGroup(group):
    parent = group.getAttribute('parent')
    if parent == '': parent = None
    name = group.getAttribute('name')

    jg = JobGroupKit(name, parent)

    logCheckerTags    = group.getElementsByTagName('logChecker')
    logChecker        = handleLogCheckerTags(logCheckerTags)

    checkerTags       = group.getElementsByTagName('checker')
    checkers          = [handleCheckerTag(tag) for tag in checkerTags]

    keepFileTags      = group.getElementsByTagName('keepFilePattern')
    keepFiles         = [handleKeepFileTag(tag) for tag in keepFileTags]

    auxFileTags       = group.getElementsByTagName('auxFilePattern')
    auxFiles          = [handleAuxFileTag(tag) for tag in auxFileTags]

    testTags          = group.getElementsByTagName('test')
    tests             = [handleUserTestTag(tag) for tag in testTags]

    actionTags        = group.getElementsByTagName('action')
    actions           = [handleUserTestTag(tag) for tag in actionTags]

    # monActionTags     = group.getElementsByTagName('monAction')
    # monActions        = [handleUserTestTag(tag) for tag in monActionTags]
    
    jg.extendItem('keepFilePatterns', keepFiles)
    jg.extendItem('checkers', checkers)
    jg.extendItem('auxFilePatterns', auxFiles)
    jg.logChecker = logChecker

    jg.extendItem('tests', tests)
    jg.extendItem('actions', actions)
    # jg.extendItem('monActions', monActions)
    
    return jg

# ---------------------------------------------------------------
# log checkers - need special treatment as must be the first checker!
# ---------------------------------------------------------------

def handleLogCheckerTags(tags):
    if len(tags) > 1:
        logger.error('Did not get a unique log checker tag')
        return None
    if len(tags) == 0: return

    tag = tags[0]
    checkerClass = tag.getAttribute('class')
    if checkerClass not in logCheckerDispatcher:
        logger.error('Log Checker not in dispatch table: '+checkerClass)
        return None

    return logCheckerDispatcher[checkerClass](tag)

    
def makeSimpleLogChecker(tag):
    searchStringTags  = tag.getElementsByTagName('searchString')
    searchStrings     = [getText(sst.childNodes) for sst in searchStringTags]

    vetoStringTags    = tag.getElementsByTagName('vetoString')
    vetoStrings       = [getText(vst.childNodes) for vst in vetoStringTags]

    mustFindAllSearch = {'True':True, 'False':False}.get(Evaluate('mustFindAllSearch/text()', tag), True)
    return SimpleLogChecker(searchStrings, vetoStrings, mustFindAllSearch)

def makeChainedJobsLogChecker(tag):
    searchStringTags = tag.getElementsByTagName('searchString')
    searchStrings    = [getText(sst.childNodes) for sst in searchStringTags]

    vetoStringTags   = tag.getElementsByTagName('vetoString')
    vetoStrings      = [getText(vst.childNodes) for vst in vetoStringTags]

    return ChainedJobsLogChecker(searchStrings, vetoStrings)

# ---------------------------------------------------------------
# Checker object creation
# ---------------------------------------------------------------

def handleCheckerTag(tag):
    checkerClass = tag.getAttribute('class')
    if checkerClass not in checkerDispatcher:
        logger.error('Checker not in dispatch table: '+checkerClass)
        return
    
    return checkerDispatcher[checkerClass](tag)


def makeSimpleFileDiffer(tag):
    return SimpleFileDiffer()

# ---------------------------------------------------------------
# Handle keep file pattern tag
# ---------------------------------------------------------------

def handleKeepFileTag(tag):
    """Returns a dictionary of the keep file pattern string, info string and display colour attached."""
    infoString   =  tag.getAttribute('info')
    displayColor =  tag.getAttribute('displayColor')

    keepString = getText(tag.childNodes)

    return {'keepFileString':keepString,
            'infoString':infoString,
            'displayColor':displayColor}

# ---------------------------------------------------------------
# Handle aux file pattern tag
# ---------------------------------------------------------------
def handleAuxFileTag(tag):
    return getText(tag.childNodes)
    
#---------------------------------------------------------------
#Instantiate User Test
#---------------------------------------------------------------
def list2dict(keyList=[], valueList=[]):
    dict ={}
    iterator =0
    while iterator < len(keyList):
        dict[keyList[iterator]]=valueList[iterator]
        iterator +=1

    return dict

#-------------------------------------------------------------------------
#Instantiate external class
#-------------------------------------------------------------------------
def handleUserTestTag(test):
    """Instanstiate an installed test or action"""

    userModuleTag    =  test.getElementsByTagName('modulename')
    userModuleName   = [getText(umt.childNodes) for umt in userModuleTag]

    tags         = test.getElementsByTagName('testname')
    userTestName = [getText(utt.childNodes) for utt in tags]

    # initialise an arguments dictionary
    testArgDict  = {}

    # fill a dictionary with argument name-value pairs
    tags         = test.getElementsByTagName('arg')
    [testArgDict.update(handleTestArgTag(tag)) for tag in tags]
    
    # fill a dictionary with argument name - [list of values] pairs
    tags         = test.getElementsByTagName('listarg')
    [testArgDict.update(handleTestListArgTag(tag)) for tag in tags]

    # get the position attribute from the tag
    position = test.getAttribute('position').strip()

    # run the action/test in the atlas environment
    runInAtlasEnv = test.getAttribute('runInAtlasEnv').strip()

    testDescriptor   = UserTestDescriptor(runInAtlasEnv,
                                          position,
                                          userModuleName[0],
                                          userTestName[0],
                                          testArgDict)

    return testDescriptor

#-------------------------------------------------------------------------

def handleTestArgTag(tag):
    name = Evaluate('argname/text()', tag)
    values = [Evaluate('text()', v) for v in Evaluate('argvalue', tag)]
    if len(values)==1: values=values[0]
    return {name: values}

#-------------------------------------------------------------------------

def handleTestListArgTag(tag):
    argNameTag   = (tag.getElementsByTagName('listargname'))[0]
    name         = getText(argNameTag.childNodes)
    
    argValueTags = (tag.getElementsByTagName('listargvalue'))
    value        = [getText(tag.childNodes) for tag in argValueTags]
    
    return {name: value}

#---------------------------------------------------------------------------

def makeHistogramDiffer(tag):
    compTypeTag = tag.getElementsByTagName('comparisonType')
    compType    = compTypeTag[0] # accept only the first one if we have several of these tags (should be only one)

    hd = HistogramDiffer(compType)
    return hd

# ---------------------------------------------------------------
# Extract text from tag
# ---------------------------------------------------------------

def getText(nodelist):
    rc = ""
    for node in nodelist:
        if node.nodeType == node.TEXT_NODE:
            rc = rc + str(node.data)
    return rc.strip()

# ---------------------------------------------------------------
# Dispatcher dictionaries
# ---------------------------------------------------------------

# SimpleLogChecker can be a log checker or a normal checker
checkerDispatcher = {
    'SimpleFileDiffer':         makeSimpleFileDiffer,
    'SimpleLogChecker':         makeSimpleLogChecker,
    'HistogramDiffer':          makeHistogramDiffer
    }

logCheckerDispatcher = {
    'SimpleLogChecker':      makeSimpleLogChecker,
    'ChainedJobsLogChecker': makeChainedJobsLogChecker
    }

logCheckers = {}

def linkKits(kits):
    # build a dictionary where each kit has its children listed.
    # Keys are kits. values are lists of child kits.
    # The kit with group Top has no parent. All leaf kits (no children)
    # are in the dictionary.
    linkTable = {}
    [linkTable.setdefault(parentKit(kit,kits), []).append(kit) for kit in kits]
    [linkTable.setdefault(kit,[]) for kit in kits]

    # As the 'Top' kit has parent 'None',
    # a spurious entry for kit with 'None' appears in the table. Remove it here.
    if None in linkTable.keys():
        if len(linkTable[None]) == 1 and (linkTable[None][0]).group == u'Top':
            del linkTable[None]
        else:
            logger.error('linkTable error: parentless  kit found in table')
            # logger.error('None entry: '+linkTable[None])
            children = reduce(lambda x,y:x+y,
                              [kit.group for kit in linkTable[None]])
            logger.error('Parent=None, Children= '+str(children))

    # sanity checks:
    [logger.error('kit for group '+kit.group+' not in link table') for kit in kits if kit not in linkTable]
    
    # make the link table go from
    # {parent: [direct children]} to {parent:[all decendants]}
    childrenToDescendants.link(linkTable)
    # dumpLinkTable(linkTable)

    # add the parents contents to the children
    # this may duplicate material:
    # if grandma gives to ma and granddaughter
    # then ma gives to grandaughter, then grandaughter
    # will have duplicate information. In actual fact
    # we dont know the order of the linkTable keys, so
    # we do not know the degree of duplication.
    # take care of duplicates in the kit.add method
    for parent in linkTable.keys():
        if not parent: continue
        for child in linkTable[parent]:
            child.add(parent)

    for kit in kits: kit.setLogChecker(kits)
            
def parentKit(kit, kits):
    for k in kits:
        if k.group == kit.parent: return k
    return None

def dumpLinkTable(table):
    labelDict = {}
    for key in table.keys():
        childLabels = []
        for child in table[key]:
            childLabels.append(child.group)
        if key == None:
            print key, childLabels
        else:
            print key.group, childLabels
        
    formatCollection(labelDict)

def makeTopLevelJobGroupKits(fileName):
    v = Validator(fileName)
    v.validate() # exception if invalid
        
    logger.info('JobGroupParser making kits - begin')
    groups = Evaluate('rtt/jobGroups', v._dom.documentElement)
    if not groups: return []        
    kits = makeKits(groups[0])
    logger.info('JobGroupParser making kits - end. Made %d kit(s).' % len(kits))
    return kits

if __name__ == '__main__':
    pass
