# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# These functions have been split of from thr Tools module to avoid
# circular module imports
#

import os, time, types
from os.path       import isdir, join
from stat          import *
from popen2        import Popen4
from RTTSException import RTTCodingError
from RTTDateTime   import RTTDateTime

# -------------------------------------------------------------------------
import logging
logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------

## relPath 
# class to construct relative path between two absolute paths       
class relPath:
    """Author: K.Ciba"""

    ## c'tor
    # @param self "Me, myself and Irene"
    # @brief it is empty, as it should be
    def __init__(self):
        pass

    ## split path to dir chunks
    # @param self "Me, myself and Irene"
    # @param path a path chunk
    # @param rest a list with tails chunks
    def pathSplit( self, path, rest=[] ):
        ( head , tail ) = os.path.split( path )
        if len(head) < 1: return [tail]+ rest
        if len(tail) < 1: return [head]+ rest
        return self.pathSplit( head, [tail] + rest )

    ## finds common chunk in two paths
    # @param self "Me, myself ad Irene"
    # @param l1 1st path
    # @param l2 2nd path
    # @param common a list of common chunks
    def commonPath( self,  l1, l2, common=[] ):
        if len(l1) < 1: return (common, l1, l2)
        if len(l2) < 1: return (common, l1, l2)
        if l1[0] != l2[0]: return (common, l1, l2)
        return self.commonPath( l1[1:], l2[1:], common+[l1[0]])

    ## finds relative path between path1 and path2
    # @param self "Me, myself and Irene"
    # @param p1 1st path
    # @param p2 2nd path
    # @return string with relative path between p1 and p2
    def __call__( self, p1, p2 ):
        if ( p1 == p2 ): return "./"
        ( common, l1, l2 ) = self.commonPath( self.pathSplit( p1 ), self.pathSplit( p2 ) )
        p = []
        if len(l1) > 0:
            p = [ '../' * len(l1) ]
        p = p + l2
        return os.path.join( *p )


def toRelPath(p1,p2):
    r = relPath()
    return r(p1,p2)

def deSpamifyEmailAddress(address):
    return address.replace('NOSPAM','')

def isValidReleaseNumber(rel):
    """Is the release string passed a valid numbered release i.e.
    of the form X.Y.Z"""

    splitRel = rel.split('.')
    if not ( (len(splitRel) == 3) or (len(splitRel) == 4) ):
        return False
    else:
        for thing in splitRel:
            # if len(thing) != 1 or not thing in string.digits:
            #    return False
            try:
                int(thing)
            except Exception, inst:
                return False # not necesarily an error - just not a valid release number

    return True

def releaseNumberComponents(relString):
    """return the integer compoenents of a relase string"""

    bad = (None, None, None)
    if not isValidReleaseNumber(relString): return bad
    
    splitRel = relString.split('.')
    if len(splitRel) != 3:
        return bad
    else:
        rel=[]
        for thing in splitRel:
            # if len(thing) != 1 or not thing in string.digits:
            #    return False
            try:
                rel.append(int(thing))
            except Exception, inst:
                return bad # not necesarily an error - just not a valid release number

    return (rel[0], rel[1], rel[2])


#----------------------------------------------------

def popenCmd(cmd):
    po = Popen4(cmd)

    out = []
        
    while po.poll() == -1:
        out.extend(po.fromchild.readlines())
        
    del po
    return out

#----------------------------------------------------

def fixSortBranches(branches):
    """Imposed 'sorting'."""
    
    theOrder = ['N.0.X','10.0.X','N.X.0','Numbered']
    
    theLen = len(branches)
    done = False
    while not done:
        changedOrder = False
        i = 0
        while i < theLen - 1:
            curr = branches[i]
            next = branches[i+1]
            if theOrder.index(curr) > theOrder.index(next):
                branches[i],branches[i+1] = branches[i+1],branches[i]
                changedOrder = True

            i = i + 1 

        done = (changedOrder==False)
        
    return branches
    
# ----------------------------------------------------------------------

def extractReleaseFromNightlyLink(theLink):    
    """Extract the release from the stamp file (which is a soft link)"""
    theLink = str(theLink) # might be an RTTpath instance
    if os.path.islink(theLink):
        return os.readlink(theLink)
    
    return theLink
# ----------------------------------------------------

def getFileOrDirTimeStamp(fileOrDir):
    """Returns a three-member list [hours,mins,secs] representing the time stamp of a given file or directory"""

    fileOrDirTime = []
    
    if os.path.exists(fileOrDir):
        return RTTDateTime(os.lstat(fileOrDir)[ST_ATIME])
    else:
        return RTTDateTime()
# ----------------------------------------------------

def getFileOrDirDateStamp(fileOrDir):
    """Returns a three-member list [year,month,day] representing the date stamp of a given file or directory"""

    fileOrDirDate = []
    if os.path.exists(fileOrDir):
        return RTTDateTime(os.lstat(fileOrDir)[ST_ATIME])
    else:
        return RTTDateTime()


#----------------------------------------------------


# def getAllResultDirs(resBase, legalBranches, legalBuilds, legalRunTypes):
def getAllResultDirs(resBase, legals):
    """Returns a list of all valid results directories below 'resBase'."""


    
    # get all valid results dirs below resBase
    relDict = makeReleaseDictionary(resBase,legals)
   
    # sort the build keys reverse alphabetically
    builds = relDict.keys()
    builds.sort()
    builds.reverse()    
    
    finalList = []

    for build in builds:        
        
        branches = relDict[build].keys()
        branches = fixSortBranches(branches)
        
        #        branches.sort() #alphabetically
        
        for branch in branches:
            runTypes = relDict[build][branch]
            
            for runType in runTypes:

                releases = relDict[build][branch][runType]

                for release in releases:
                    finalList.append({'build':build,'branch':branch,'runType':runType,'release':release}) 
    
    return finalList
    
# ----------------------------------------------------
# ----------------------------------------------------
        
def jobStatusFromFile(job, resPath, statusFile=""):

    numbEntriesPerLine = 7 # number of entries we normally have in a given line
    
    if statusFile == "":
        statusFile = join(resPath, "status.txt")

    if os.path.exists(statusFile):
        ifile = open(statusFile,"r")

        linesInFile = ifile.readlines()
        ifile.close()
        
        linesInFile.reverse()

        
        matchesFound = []
        for line in linesInFile:
        
            if line.find(job) != -1:
                tokens = line.split()
                # Previously each line in the status.txt log file had the form:
                #      Job  Status  Date  Time
                # now they each have the form:
                #     Job  Status  Date  Time  KitOrReleaseRun  Platform  NICOSdate
                # We need to take this into account here.                

                numbEntriesFoundInLine = len(tokens)
                while numbEntriesFoundInLine < numbEntriesPerLine:
                    tokens.append('n/a')
                    numbEntriesFoundInLine += 1

                returnList = []
                [returnList.append(thing) for thing in tokens[1:]]
                returnTuple = tuple(returnList)

                time.sleep(3)
                return returnTuple

    return ("n/a",)*(numbEntriesPerLine-1)

#----------------------------------------------------

def makeReleaseDictionary(resBase, legals):
    """Searches for all valid results directories below 'resBase',
    and builds a dictionary representing them, keyed on build whose
    values themselves are dictionaries. These element dictionaries
    are keyed on branch and have values which are lists of releases sorted
    such that nightly releases are first, in alphabetical order, then numbered
    releases in reverse numerical order. Returns the final dictionary.
    """
    
    relDict = {}

    lBuilds   = legals.vals('build')
    lBranches = legals.vals('branch')
    lRunTypes = legals.vals('runType')
    
    for buildDir in os.listdir(resBase):
        if not buildDir in lBuilds or not isdir(join(resBase, buildDir)):
            continue 
        
        resBaseBuild = join(resBase,buildDir)

        relDict[buildDir] = {}
            
        for branchDir in os.listdir(resBaseBuild):            
            if not branchDir in lBranches or not isdir(join(resBaseBuild,
                                                            branchDir)):
                continue
            
            resBaseBuildBranch = join(resBaseBuild,branchDir)

            relDict[buildDir][branchDir] = {}

            for runTypeDir in os.listdir(resBaseBuildBranch):
                if not runTypeDir in lRunTypes: continue
                if not isdir(join(resBaseBuildBranch, runTypeDir)): continue

                releasesInDir = os.listdir(join(resBaseBuildBranch,runTypeDir))
                relDict[buildDir][branchDir][runTypeDir] = sortReleases(releasesInDir, legals)

    return relDict

#----------------------------------------------------

def sortReleases(rels, legals):
    """Given a list of releases, returns a list of sorted releases, nightlies first in alphabetical order,
    followed in reverse numerical order by numbered releases."""

    sorted = []
    numbered = [rel for rel in rels if isValidReleaseNumber(rel)]
    nightly  = [rel for rel in rels if legals.isNightlyInRelease(rel)]
    # sort the two lists
    nightly.sort()
    numbered = numericalSort(numbered)
    
    sorted.extend(nightly)
    sorted.extend(numbered)

    return sorted

#----------------------------------------------------

def numericalSort(rels):
    """Takes a list of numbered releases and returns the list sorted in decreasing numerical order."""
    if len(rels)== 0 or len(rels)==1: return rels

    lists = {'1':[],'2':[]}
    for thing in rels:
        if len(thing.split('.')[0]) == 1:
            lists['1'].append(thing)
        else:
            lists['2'].append(thing)
        
    lists['1'].sort()
    lists['1'].reverse()
    lists['2'].sort()
    lists['2'].reverse()
    finalList = lists['2']
    finalList.extend(lists['1'])
    return finalList

def searchPaths(paths, file):
    found = False
    for p in paths:
        tryPath = os.path.join(p, file)
        if os.path.exists(tryPath): return p
    return None


#----------------------------------------------------

def setProtectionFile(path, perm):
    if not os.path.exists(path): return
    os.chmod(path, fixSBit(path, perm))

#----------------------------------------------------

def fixSBit(directory, perm):
    # kluge. The 2000 bit cnnot be set for afs
    if not directory.startswith('/afs'):return perm
    # if directory.startswith('/afs'):return perm

    
    operm = oct(perm)

    
    if len(operm)<5: return perm # not enough digits to require correction

    
    bit11set = ['2', '3', '6', '7']
    if operm[-4:-3] in bit11set: perm = perm-02000
    return perm

#----------------------------------------------------

def convertToSeconds(timeString):
    """Takes a time string of format HH:MM:SS, and returns a
    string representing an integer number of seconds."""
    fields = timeString.split(':')
    if len(timeString)!=8 or len(fields)!=3: return ''
    try:
        hours = int(fields[0])
        mins  = int(fields[1])
        secs  = int(fields[2])
    except:
        return ''

    timeInSecs = (hours*3600) + (60*mins) + secs
    return str(timeInSecs)

    
    # ----------------------------------------------------------------------

def escapeTheCommand(cmd):
    
    toBeEscaped = ['(',')','"']
    
    for char in toBeEscaped:
        pieces = cmd.split(char)
        
        cmd = pieces[0]
        for piece in pieces[1:]:
            cmd += '\\'+char+piece

    return cmd
    
def isType(instance, name):
    """name is a string. Determnes if class with name name is
    in the class heirarchy of an instance"""
    
    if not (type(name) is types.StringType):
        m = 'isType: argument passed is of type %s, should be of type StringType' % name.__class__.__name__
        raise RTTCodingError(m)

    name = name.strip()
    
    if instance.__class__.__name__ == name: return True

    bases = [b.__name__ for b in instance.__class__.__bases__]
    if name in bases: return True
    return False
            
            
if __name__ == '__main__':
    thePath = '/afs/cern.ch/atlas/project/RTT/data/brinick/Results/rel_4/dev/build/i686-slc4-gcc34-opt/offline/AthExHelloWorld/AthenaHelloWorld/HelloWorldOptions/0/testDebug.log'
    cmd = 'md5sum %s' % thePath
    print popenCmd(cmd)
