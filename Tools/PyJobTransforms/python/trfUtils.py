# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfUtils
# @brief Transform utility functions
# @author atlas-comp-transforms-dev@cern.ch
# @version $Id: trfUtils.py 785618 2016-11-21 22:03:04Z uworlika $

import os
import os.path as path
import re
import signal
import sys
import tarfile
import time
import uuid
import socket

import multiprocessing
import base64

from datetime import datetime
from subprocess import Popen, STDOUT, PIPE, CalledProcessError
from xml.dom import minidom
from xml.parsers.expat import ExpatError
from xml.etree import ElementTree

from PyJobTransforms.trfExitCodes import trfExit
import PyJobTransforms.trfExceptions as trfExceptions

import logging
msg = logging.getLogger(__name__)


## @brief Find a named file along a colon separated PATH type variable
#  @details Note will also work for finding directories
#  @return Full path to file or @c None is file is not found
def findFile(pathvar, fname):
    # First see if the file already includes a path.
    msg.debug('Finding full path for {fileName} in path {path}'.format(
        fileName = fname,
        path = pathvar
    ))
    if fname.startswith('/'):
        return(fname)
    
    # Split the path.
    pathElements = pathvar.split(':')
    for pathElement in pathElements:
        if path.exists(path.join(pathElement, fname)):
            return(path.join(pathElement, fname))
    
    return(None)


## @brief List all processes and parents and form a dictionary where the 
#  parent key lists all child PIDs
#  @param listMyOrphans If this is @c True, then processes which share the same
#  @c pgid as this process and have parent PID=1 (i.e., init) get added to this process's children,
#  which allows these orphans to be added to the kill list. N.B. this means
#  that orphans have two entries - as child of init and a child of this
#  process
def getAncestry(listMyOrphans = False):
    psCmd = ['ps', 'ax', '-o', 'pid,ppid,pgid,args', '-m']

    try:
        msg.debug('Executing %s' % psCmd)
        p = Popen(psCmd, stdout=PIPE, stderr=PIPE)
        stdout = p.communicate()[0]
        psPID = p.pid
    except OSError, e:
        msg.error('Failed to execute "ps" to get process ancestry: %s' % repr(e))
        raise
    
    childDict = {}
    myPgid = os.getpgrp()
    myPid = os.getpid()
    for line in stdout.split('\n'):
        try:
            (pid, ppid, pgid, cmd) = line.split(None, 3)
            pid = int(pid)
            ppid = int(ppid)
            pgid = int(pgid)
            # Ignore the ps process
            if pid == psPID:
                continue
            if ppid in childDict:
                childDict[ppid].append(pid)
            else:
                childDict[ppid] = [pid]
            if listMyOrphans and ppid == 1 and pgid == myPgid:
                msg.info("Adding PID {0} to list of my children as it seems to be orphaned: {1}".format(pid, cmd))
                if myPid in childDict:
                    childDict[myPid].append(pid)
                else:
                    childDict[myPid] = [pid]
                
        except ValueError:
            # Not a nice line
            pass
    return childDict

## @brief Find all the children of a particular PID (calls itself recursively to descend into each leaf)
#  @note  The list of child PIDs is reversed, so the grandchildren are listed before the children, etc.
#  so signaling left to right is correct
#  @param psTree The process tree returned by @c trfUtils.listChildren(); if None then @c trfUtils.listChildren() is called internally.
#  @param parent The parent process for which to return all the child PIDs
#  @param listOrphans Parameter value to pass to getAncestry() if necessary
#  @return @c children List of child PIDs 
def listChildren(psTree = None, parent = os.getpid(), listOrphans = False):
    '''Take a psTree dictionary and list all children'''
    if psTree == None:
        psTree = getAncestry(listMyOrphans = listOrphans)
    
    msg.debug("List children of %d (%s)" % (parent, psTree.get(parent, [])))
    children = []
    if parent in psTree:
        children.extend(psTree[parent])
        for child in psTree[parent]:
            children.extend(listChildren(psTree, child))
    children.reverse()
    return children


## @brief Kill all PIDs
#  @note Even if this function is used, subprocess objects need to join() with the 
#  child to prevent it becoming a zombie 
#  @param childPIDs Explicit list of PIDs to kill; if absent then listChildren() is called
#  @param sleepTime Time between SIGTERM and SIGKILL
#  @param message Boolean if messages should be printed
#  @param listOrphans Parameter value to pass to getAncestry(), if necessary (beware, killing
#  orphans is dangerous, you may kill "upstream" processes; Caveat Emptor)
def infanticide(childPIDs = None, sleepTime = 3, message = True, listOrphans = False):
    if childPIDs is None:
        childPIDs = listChildren(listOrphans = listOrphans)
        
    if len(childPIDs) > 0 and message:
        msg.info('Killing these child processes: {0}...'.format(childPIDs))
        
    for pid in childPIDs:
        try:
            os.kill(pid, signal.SIGTERM)
        except OSError:
            pass
        
    time.sleep(sleepTime)
        
    for pid in childPIDs:
        try:
            os.kill(pid, signal.SIGKILL)
        except OSError:
            # OSError happens when the process no longer exists - harmless
            pass


def call(args, bufsize=0, executable=None, stdin=None, preexec_fn=None, close_fds=False, shell=False, cwd=None, env=None, universal_newlines=False, startupinfo=None, creationflags=0, message="", logger=msg, loglevel=None, timeout=None, retry=2, timefactor=1.5, sleeptime=10):

    def logProc(p):
        line=p.stdout.readline()
        if line:
            line="%s%s" % (message, line.rstrip())
            if logger is None:
                print line
            else:
                logger.log(loglevel, line)

    def flushProc(p):
        line=p.stdout.readline()
        while line:
            line="%s%s" % (message, line.strip())
            if logger is None:
                print line
            else:
                logger.log(loglevel, line)
            line=p.stdout.readline()

    if loglevel is None: 
        loglevel=logging.DEBUG

    if timeout is None or timeout<=0: # no timeout set
        msg.info('Executing %s...' % args)
        starttime = time.time()
        p=Popen(args=args, bufsize=bufsize, executable=executable, stdin=stdin, stdout=PIPE, stderr=STDOUT, preexec_fn=preexec_fn, close_fds=close_fds, shell=shell, cwd=cwd, env=env, universal_newlines=universal_newlines, startupinfo=startupinfo, creationflags=creationflags)
        while p.poll() is None:
            logProc(p)
        flushProc(p)
        if timeout is not None:    
            msg.info('Executed call within %d s.' % (time.time()-starttime))
        return p.returncode
            
    else: #timeout set
        n=0
        while n<=retry:
            msg.info('Try %i out of %i (time limit %ss) to call %s.' % (n+1, retry+1, timeout, args))
            starttime = time.time()
            endtime=starttime+timeout
            p=Popen(args=args, bufsize=bufsize, executable=executable, stdin=stdin, stdout=PIPE, stderr=STDOUT, preexec_fn=preexec_fn, close_fds=close_fds, shell=shell, cwd=cwd, env=env, universal_newlines=universal_newlines, startupinfo=startupinfo, creationflags=creationflags)
            while p.poll() is None and time.time()<endtime:
                logProc(p)
            if p.poll() is None:
                msg.warning('Timeout limit of %d s reached. Kill subprocess and its children.' % timeout)
                parent=p.pid
                pids=[parent]
                pids.extend(listChildren(parent=parent))
                infanticide(pids)
                msg.info('Checking if something is left in buffer.')
                flushProc(p)
                if n!=retry:
                    msg.info('Going to sleep for %d s.' % sleeptime)                    
                    time.sleep(sleeptime)
                n+=1
                timeout*=timefactor
                sleeptime*=timefactor
            else:
                flushProc(p)
                msg.info('Executed call within %d s.' % (time.time()-starttime))
                return p.returncode

        msg.warning('All %i tries failed!' % n)
        raise Exception


## @brief Return a string with a report of the current athena setup
def asetupReport():
    setupMsg = str()
    eVars = ['AtlasBaseDir', 'AtlasProject', 'AtlasVersion', 'AtlasPatch', 'AtlasPatchVersion', 'CMTCONFIG', 'TestArea']
    if "AtlasProject" in os.environ:
        CMake_Platform = "{0}_PLATFORM".format(os.environ["AtlasProject"])
        if CMake_Platform in os.environ:
            eVars.remove("CMTCONFIG")
            eVars.append(CMake_Platform)
    for eVar in eVars:
        if eVar in os.environ:
            setupMsg += '\t%s=%s\n' % (eVar, os.environ[eVar])
    # Look for patches so that the job can be rerun 
    if 'TestArea' in os.environ and os.access(os.environ['TestArea'], os.R_OK):
        setupMsg += "\n\tPatch packages are:\n"
        try:
            cmd = ['lstags']
            lstagsOut = Popen(cmd, shell = False, stdout = PIPE, stderr = STDOUT, bufsize = 1).communicate()[0]
            setupMsg +=  "\n".join([ "\t\t{0}".format(pkg) for pkg in lstagsOut.split("\n") ])
        except (CalledProcessError, OSError), e:
            setupMsg += 'Execution of lstags failed: {0}'.format(e)
    else:
        setupMsg+= "No readable patch area found"

    return setupMsg.rstrip()


## @brief Test (to the best of our knowledge) if the current release is older
#  than a major, minor version number
#  @details There's no unambiguous reference to the release that encompasses
#  all of the development builds (dev, devval, migs), but almost everything 
#  can be determined from @c AtlasVersion and @c AtlasBaseDir. If neither of 
#  those contain version information then we assume a development build
#  that is @e new by definition (so we return @c False)
#  @param major Major release number
#  @param minor Minor release number (if not specified, will not be matched against)
#  @return Boolean if current release is found to be older  
def releaseIsOlderThan(major, minor=None):
    if 'AtlasVersion' not in os.environ or 'AtlasBaseDir' not in os.environ:
        msg.warning("Could not find 'AtlasVersion' and 'AtlasBaseDir' in the environment - no release match possible")
        return False
    try:
        # First try AtlasVersion, which is clean
        relRegExp = re.compile(r'(?P<major>\d+)\.(?P<minor>\d+)\.(?P<other>.*)')
        relMatch = re.match(relRegExp, os.environ['AtlasVersion'])
        if not relMatch:
            # Now try the final part of AtlasBaseDir
            leafDir = path.basename(os.environ['AtlasBaseDir'])
            relMatch = re.match(relRegExp, leafDir)
            if not relMatch:
                msg.info('No identifiable numbered release found from AtlasVersion or AtlasBaseDir - assuming dev/devval/mig')
                return False

        relmajor = int(relMatch.group('major'))
        relminor = int(relMatch.group('minor'))
        msg.info('Detected release major {0}, minor {1} (.{2}) from environment'.format(relmajor, relminor, relMatch.group('other')))
        
        # Major beats minor, so test this first
        if relmajor < major:
            return True
        if relmajor > major:
            return False
        
        # First case is major equality and don't care about minor
        if minor is None or relminor >= minor:
            return False
        return True

    except Exception, e:
        msg.warning('Exception thrown when attempting to detect athena version ({0}). No release check possible'.format(e))
    return False

## @brief Quote a string array so that it can be echoed back on the command line in a cut 'n' paste safe way
#  @param strArray: Array of strings to quote
#  @details Technique is to first quote any pre-existing single quotes, then single quote all of the array 
#  elements so that the shell sees them as a single argument 
def shQuoteStrings(strArray = sys.argv):
    return [ "'" + qstring.replace("'", "\\'") + "'" for qstring in strArray ]


## @brief Generator to return lines and line count from a file
#  @param filename: Filename to open and deliver lines from
#  @param strip: If lines get stripped before being returned (default @c True)
#  @param removeTimestamp: Removes timestamp from left.(default @c True) Since strings are removed only from left,
#                          this option requires explicit removal of substepName.
#  @param substepName: Removes substepName from left, if it's value is provided. (default @c None)
#  @note This is useful so that multiple parts of code can co-operatively take lines from the file
def lineByLine(filename, strip=True, removeTimestamp=True, substepName=None):
    linecounter = 0
    f = open(filename, 'r')
    for line in f:
        linecounter += 1
        if substepName and isinstance(substepName, str):    # Remove substepName only if caller provides that string.
            line = line.lstrip(substepName)
        if removeTimestamp:
            line = line.lstrip('0123456789:-, ')            # Remove timestamps in both serial and MP mode.
        if strip:
            line = line.strip()
        yield line, linecounter
    f.close()
    

## @brief XML pretty print an ElementTree.ELement object
#  @param element ElementTree.ELement object to print
#  @param indent Indent parameter for minidom toprettyxml method
#  @param poolFileCatalogFormat Whether to reformat the XML as a classic POOLFILECATALOG document
#  @return String with the pretty printed XML version
#  @note This is rather a convoluted way to get the correct DOCTYPE
#        set and there's probably a better way to do it, but as this 
#        is a deprecated way of delivering metadata upstream it's not
#        worth improving at this stage. 
def prettyXML(element, indent = ' ', poolFileCatalogFormat = False):
    # Use minidom for pretty printing
    # See http://broadcast.oreilly.com/2010/03/pymotw-creating-xml-documents.html
    xmlstring = ElementTree.tostring(element, 'utf-8')
    try:
        metadataDoc = minidom.parseString(xmlstring)
    except ExpatError:
        # Getting weird \x00 NULLs on the end of some GUIDs, which minidom.parsestring does not like (is this APR?)
        msg.warning('Error parsing ElementTree string - will try removing hex literals ({0!r})'.format(xmlstring))
        xmlstring = xmlstring.replace('\x00', '')
        metadataDoc = minidom.parseString(xmlstring)
        
    
    if poolFileCatalogFormat is False:
        return metadataDoc.toprettyxml(indent=indent, encoding='UTF-8')
    
    # Now create a new document with the correct doctype for classic POOLFILECATALOG
    # See http://stackoverflow.com/questions/2337285/set-a-dtd-using-minidom-in-python
    imp = minidom.DOMImplementation()
    doctype = imp.createDocumentType(qualifiedName='POOLFILECATALOG', publicId='', systemId='InMemory')
    doc = imp.createDocument(None, 'POOLFILECATALOG', doctype)
    
    # Cut and paste the parsed document into the new one
    # See http://stackoverflow.com/questions/1980380/how-to-render-a-doctype-with-pythons-xml-dom-minidom
    refel = doc.documentElement
    for child in metadataDoc.childNodes:
        if child.nodeType==child.ELEMENT_NODE:
            doc.replaceChild(doc.importNode(child, True), doc.documentElement)
            refel= None
        elif child.nodeType!=child.DOCUMENT_TYPE_NODE:
            doc.insertBefore(doc.importNode(child, True), refel)

    return doc.toprettyxml(indent=indent, encoding='UTF-8')


## @brief Return isoformated 'now' string
#  @details Uses datetime.isoformat method, but suppressing microseconds
def isodate():
    return datetime.now().replace(microsecond=0).isoformat()


## @brief Strip a string down to alpha-numeric characters only
#  @note This is used to force executor names and substep aliases
#   to a form that the substep argument parser will recognise.
#   None is still allowed as this is the default for "unset" in
#   some cases. 
def forceToAlphaNum(string):
    if string == None or string.isalnum():
        return string
    newstring = ''
    for piece in string:
        if piece.isalnum():
            newstring += piece
    msg.warning("String {0} was stripped to alphanumeric characters only: {1}".format(string, newstring))
    return newstring


## @brief Compare metadata for files, but taking into account that GUID can vary
#  @details Compare metadata dictionaries, but allowing for differences in the file_guid property
#  as this is generated randomly for file types without an intrinsic GUID
#  @param metadata1 Filel metadata dictionary
#  @param metadata2 File2 metadata dictionary
#  @param giudCheck How to compare GUIDs. Valid values are:
#  - @c equal GUIDs must be the same
#  - @c valid GUIDs must be valid, but don't have to be the same
#  - @c ignore The file_guid key is ignored
#  @return True if metadata is the same, otherwise False
def cmpMetadata(metadata1, metadata2, guidCheck = 'valid'):
    # First check we have the same files
    allFiles = set(metadata1.keys()) | set(metadata2.keys())
    if len(allFiles) > len(metadata1.keys()) or len(allFiles) > len(metadata2.keys()):
        msg.warning('In metadata comparison file lists are not equal - fails ({0} != {1}'.format(metadata1, metadata2))
        return False
    for fname in allFiles:
        allKeys = set(metadata1[fname].keys()) | set(metadata2[fname].keys())
        if len(allKeys) > len(metadata1[fname].keys()) or len(allFiles) > len(metadata2[fname].keys()):
            msg.warning('In metadata comparison key lists are not equal - fails')
            return False
        for key in allKeys:
            if key is 'file_guid':
                if guidCheck is 'ignore':
                    continue
                elif guidCheck is 'equal':
                    if metadata1[fname]['file_guid'].upper() == metadata2[fname]['file_guid'].upper():
                        continue
                    else:
                        msg.warning('In metadata comparison strict GUID comparison failed.')
                        return False
                elif guidCheck is 'valid':
                    try:
                        uuid.UUID(metadata1[fname]['file_guid'])
                        uuid.UUID(metadata2[fname]['file_guid'])
                        continue
                    except ValueError:
                        msg.warning('In metadata comparison found invalid GUID strings.')
                        return False
            if metadata1[fname][key] != metadata2[fname][key]:
                msg.warning('In metadata comparison found different key values: {0!s} != {1!s}'.format(metadata1[fname][key], metadata2[fname][key]))
    return True                


## @brief Unpack a given tarfile
#  @param filename Tar file to unpck
#  @param directory Directory target for the unpacking
def unpackTarFile(filename, directory="."):
    try:
        tar = tarfile.open(filename)
        tar.extractall(path=directory)
        tar.close()
    except Exception, e:
        errMsg = 'Error encountered while unpacking {0} to {1}: {2}'.format(filename, directory, e)
        msg.error(errMsg)
        raise trfExceptions.TransformSetupException(trfExit.nameToCode('TRF_SETUP'), errMsg)


## @brief Ensure that the DBRelease tarball has been unpacked 
#  @details Extract the dbversion number and look for an unpacked directory.
#  If found then this release is already setup. If not then try to unpack
#  the tarball.
#  @param tarball The tarball file
#  @param dbversion The version number (if not given the look at the tarball name to get it)
#  @throws trfExceptions.TransformSetupException If the DBRelease tarball is unreadable or the version is not understood
#  @return Two element tuple: (@c True if release was unpacked or @c False if release was already unpacked, dbsetup path)
def unpackDBRelease(tarball, dbversion=None):
    if dbversion == None:
        dbdMatch = re.match(r'DBRelease-([\d\.]+)\.tar\.gz', path.basename(tarball))
        if dbdMatch == None:
            raise trfExceptions.TransformSetupException(trfExit.nameToCode('TRF_DBRELEASE_PROBLEM'),
                                                        'Could not find a valid version in the DBRelease tarball: {0}'.format(tarball))
        dbversion = dbdMatch.group(1)
    dbsetup = path.abspath(path.join("DBRelease", dbversion, "setup.py")) 
    if os.access(dbsetup, os.R_OK):
        msg.debug('DBRelease {0} is already unpacked, found {1}'.format(tarball, dbsetup))
        return False, dbsetup
    else:
        msg.debug('Will attempt to unpack DBRelease {0}'.format(tarball))
        unpackTarFile(tarball)
        msg.info('DBRelease {0} was unpacked'.format(tarball))
        if not os.access(dbsetup, os.R_OK):
            raise trfExceptions.TransformSetupException(trfExit.nameToCode('TRF_DBRELEASE_PROBLEM'),
                                                        'DBRelease setup file {0} was not readable, even after unpacking {1}'.format(dbsetup, tarball))
        return True, dbsetup

## @brief Run a DBRelease setup
#  @param setup DMRelease setup script location (absolute or relative path)
#  @return: None
def setupDBRelease(setup):
    try:
        dbdir=path.abspath(path.dirname(setup))
        msg.debug('Will add {0} to sys.path to load DBRelease setup module'.format(dbdir))
        # N.B. We cannot use __import__ because the X.Y.Z directory name is illegal for a python module path
        opath = sys.path
        sys.path.insert(0, dbdir)
        from setup import Setup
        # Instansiate the Setup module, which activates the customisation
        setupObj = Setup(dbdir)
        sys.path = opath
        msg.debug('DBRelease setup module was initialised successfully')        
    except ImportError, e:
        errMsg = 'Import error while trying to load DB Setup module: {0}'.format(e)
        msg.error(errMsg)
        raise trfExceptions.TransformSetupException(trfExit.nameToCode('TRF_DBRELEASE_PROBLEM'), errMsg)
    except Exception, e:
        errMsg = 'Unexpected error while trying to load DB Setup module: {0}'.format(e)
        msg.error(errMsg)
        raise trfExceptions.TransformSetupException(trfExit.nameToCode('TRF_DBRELEASE_PROBLEM'), errMsg)
    
    
## @brief Validate a DBRelease exists on cvmfs and return the path to the setup script
#  @param dbrelease The DBRelease number (X.Y.Z[.A]) or "current"
#  @throws trfExceptions.TransformSetupException If the DBRelease setup is unreadable or the dbrelease parameter is not understood
#  @return Path to setup.py script for this DBRelease
def cvmfsDBReleaseCheck(dbrelease):
    dbsetup = None
    dbdMatch = re.match(r'([\d\.]+|current)$', dbrelease)
    msg.debug('Attempting to setup DBRelease {0} from cvmfs'.format(dbrelease))
    if dbdMatch:
        if 'VO_ATLAS_SW_DIR' in os.environ:
            msg.debug('Found site defined path to ATLAS software: {0}'.format(os.environ['VO_ATLAS_SW_DIR']))
            dbsetup = path.join(os.environ['VO_ATLAS_SW_DIR'], 'database', 'DBRelease', dbrelease, 'setup.py')
            if os.access(dbsetup, os.R_OK):
                return dbsetup
            msg.warning('Site defined path to ATLAS software seems invalid (failed to access {0}). Will also try standard cvmfs path.'.format(dbsetup))
        else:
            msg.debug('Using standard CVMFS path to ATLAS software')

        dbsetup = path.join('/cvmfs/atlas.cern.ch/repo/sw/database/DBRelease', dbrelease, 'setup.py')
        if not os.access(dbsetup, os.R_OK):
            raise trfExceptions.TransformSetupException(trfExit.nameToCode('TRF_DBRELEASE_PROBLEM'),
                                                                'CVMFS DBRelease setup file {0} was not readable'.format(dbsetup))
        msg.debug('Using cvmfs based dbrelease: {0}'.format(path.dirname(dbsetup)))
    else:
        raise trfExceptions.TransformSetupException(trfExit.nameToCode('TRF_DBRELEASE_PROBLEM'),
                                                    'Unable to interpret DBRelease "{0}" as either a tarball or a CVMFS release directory'.format(dbrelease))
    return dbsetup                
 
 
## @brief Dump a list of arguments to the pickle file given in the 'dumpPickle' argument
#  @note  This is a copy of the JSONDump function, but should in fact be deprecated soon
#         so no point in merging the common parts!
#  TODO: Deprecate me!
def pickledDump(argdict):
    if 'dumpPickle' not in argdict:
        return
    
    from PyJobTransforms.trfArgClasses import argument
    theArgumentDictionary = {}
    for k, v in argdict.iteritems():
        if k is 'dumpPickle':
            continue
        if isinstance(v, argument):
            theArgumentDictionary[k] = getattr(v, "dumpvalue", v.value)
        else:
            theArgumentDictionary[k] = v
    with open(argdict['dumpPickle'], 'w') as pickleFile:
        import cPickle as pickle
        pickle.dump(theArgumentDictionary, pickleFile)


## @brief Dump a list of arguments to the JSON file given in the 'dumpJSON' argument
def JSONDump(argdict):
    if 'dumpJSON' not in argdict:
        return
    
    from PyJobTransforms.trfArgClasses import argument
    theArgumentDictionary = {}
    for k, v in argdict.iteritems():
        if k is 'dumpJSON':
            continue
        if isinstance(v, argument):
            theArgumentDictionary[k] = getattr(v, "dumpvalue", v.value)
        else:
            theArgumentDictionary[k] = v
    with open(argdict['dumpJSON'], 'w') as JSONFile:
        import json
        json.dump(theArgumentDictionary, JSONFile, sort_keys=True, indent=2)
        
## @brief Recursively convert unicode to str, useful when we have just loaded something
#  from json (TODO: make the transforms happy with unicode as well as plain str!)
def convertToStr(in_string):
    if isinstance(in_string, dict):
        return dict([(convertToStr(key), convertToStr(value)) for key, value in in_string.iteritems()])
    elif isinstance(in_string, list):
        return [convertToStr(element) for element in in_string]
    elif isinstance(in_string, unicode):
        return in_string.encode('utf-8')
    else:
        return in_string

        
## @brief Convert a command line option to the dictionary key that will be used by argparse
def cliToKey(option):
    return option.lstrip('-').replace('-', '_')


## @brief print in a human-readable way the items of a given object
#  @details This function prints in a human-readable way the items of a given
#  object.
#  @param object to print
def printHR(the_object):
    # dictionary
    if isinstance(the_object, dict):
        for key, value in sorted(the_object.items()):
            print u'{key}: {value}'.format(key = key, value = value)
    # list or tuple
    elif isinstance(the_object, list) or isinstance(the_object, tuple):
        for element in the_object:
            print element
    # other
    else:
        print the_object


## @brief return a URL-safe, base 64-encoded pseudorandom UUID
#  @details This function returns a URL-safe, base 64-encoded pseudorandom
#  Universally Unique IDentifier (UUID).
#  @return string of URL-safe, base 64-encoded pseudorandom UUID
def uniqueIdentifier():
    return str(base64.urlsafe_b64encode(uuid.uuid4().bytes).strip("="))


## @brief return either singular or plural units as appropriate for a given
#  quantity
#  @details This function returns either singular or plural units as appropriate
#  for a given quantity. So, a quantity of 1 would cause the return of singular
#  units and a quantity of 2 would cause the return of plural units.
#  @param quantity the numerical quantity
#  @param unitSingular the string for singular units
#  @param unitSingular the string for plural units
#  @return string of singular or plural units
def units(
    quantity = None,
    unitSingular = "unit",
    unitPlural = "units"
    ):
    if quantity == 1:
        return unitSingular
    else:
        return unitPlural


# @brief returns if the current job is running in interactive environment.
def isInteractiveEnv():
    isInteractiveEnv = False
    # PS1 is for sh, bash; prompt is for tcsh and zsh
    if 'PS1' in os.environ or 'prompt' in os.environ:
        isInteractiveEnv = True
    elif os.isatty(sys.stdout.fileno()) or os.isatty(sys.stdin.fileno()):
        isInteractiveEnv = True

    return isInteractiveEnv


## @brief Job: a set of pieces of information relevant to a given work function
#  @details A Job object is a set of pieces of information relevant to a given
#  work function. A Job object comprises a name, a work function, work function
#  arguments, the work function timeout specification, a
#  multiprocessing.Pool.apply_async() object and, ultimately, a result object.
#  @param name the Job object name
#  @param workFunction the work function object
#  @param workFunctionArguments the work function keyword arguments dictionary
#  @param workFunctionTimeout the work function timeout specification in seconds
class Job(object):

    ## @brief initialisation method
    def __init__(
        self,
        workFunction = None,
        workFunctionKeywordArguments = {},
        workFunctionTimeout = None,
        name = None,
        ):
        self.workFunction = workFunction
        self.workFunctionKeywordArguments = workFunctionKeywordArguments
        self.workFunctionTimeout = workFunctionTimeout
        self.className = self.__class__.__name__
        self.resultGetter = None
        if name == None:
            self._name = uniqueIdentifier()
        else:
            self._name = name
        if self.workFunction == None:
            exceptionMessage = "work function not specified"
            msg.error("{notifier}: exception message: {exceptionMessage}".format(
                notifier = self.className,
                exceptionMessage = exceptionMessage
            ))
            raise trfExceptions.TransformInternalException(
                trfExit.nameToCode('TRF_INTERNAL'),
                exceptionMessage
            )

    @property
    def name(self):
        return self._name

    ## @brief return an object self description string
    #  @details	This method returns an object description string consisting of
    #  a listing of the items of the object self.
    #  @return object description string
    def __str__(self):
        descriptionString = ""
        for key, value in sorted(vars(self).items()):
            descriptionString += str("{key}:{value} ".format(
                key = key,
                value = value)
            )
        return descriptionString

    ## @brief print in a human-readable way the items of the object self
    #  @details This function prints in a human-readable way the items of the
    #  object self.
    def printout(self):
        printHR(vars(self))


## @brief JobGroup: a set of Job objects and pieces of information relevant to a
#  given set of Job objects
#  @details A JobGroup is a set of Job objects and pieces of information relevant
#  to a given set of Job objects. A JobGroup object comprises a name, a list of
#  Job objects, a timeout and, ultimately, an ordered list of result objects.
#  The timeout can be speecified or derived from the summation of the timeout
#  specifications of the set of Job objects.
#  @param name the JobGroup object name
#  @param jobs the list of Job objects
#  @param timeout the JobGroup object timeout specification in seconds
class JobGroup(object):

    ## @brief initialisation method
    def __init__(
        self,
        jobs = None,
        name = None,
        timeout = None
        ):
        self.jobs = jobs
        self.className = self.__class__.__name__
        self.completeStatus = False
        self.timeStampSubmission = None
        if name == None:
            self._name = uniqueIdentifier()
        else:
            self._name = name
        #self.timeStampSubmissionComplete = None #delete
        if timeout == None:
            self.timeout = 0
            for job in self.jobs:
                self.timeout += job.workFunctionTimeout
        self.results = []

    @property
    def name(self):
        return self._name

    ## @brief return an object self description string
    #  @ detail	This method returns an object description string consisting of
    #  a listing of the items of the object self.
    #  @return object description string
    def __str__(self):
        descriptionString = ""
        for key, value in sorted(vars(self).items()):
            descriptionString += str("{key}:{value} ".format(
                key = key,
                value = value)
            )
        return descriptionString

    ## @brief return Boolean JobGroup timeout status
    #  @details This method returns the timeout status of a JobGroup object. If
    #  the JobGroup object has not timed out, the Boolean False is returned. If
    #  the JobGroup object has timed out, the Boolean True is returned. If the
    #  JobGroup object has been completed or is not submitted, the Boolean False
    #  is returned.
    #  @return Boolean indicating the JobGroup timeout status
    def timeoutStatus(self):
        # If the JobGroup is complete or not submitted, then it is not timed
        # out.
        if self.completeStatus is True or self.timeStampSubmission is None:
            return False
        # If the JobGroup is not complete or submitted, then it may be timed
        # out.
        elif time.time() > self.timeout + self.timeStampSubmission:
            return True
        else:
            return False

    ## @brief print in a human-readable way the items of the object self
    #  @details This function prints in a human-readable way the items of the
    #  object self.
    def printout(self):
        printHR(vars(self))


## @brief initisation procedure for processes of process pool
def initialise_processes():
    # Multiprocessing uses signals to communicate with subprocesses, so the
    # following two lines prevent the transforms signal handlers from
    # interfering:
    from PyJobTransforms.trfSignal import resetTrfSignalHandlers
    resetTrfSignalHandlers()
    signal.signal(signal.SIGINT, signal.SIG_IGN)


## @brief ParallelJobProcessor: a multiple-process processor of Job objects
#  @param jobSubmission Job object or JobGroup object for submission
#  @param numberOfProcesses the number of processes in the process pool
class ParallelJobProcessor(object):

    ## @brief initialisation method that accepts submissions and starts pool
    #  @details	This method is the initialisation method of the parallel job
    #  processor. It accepts input JobGroup object submissions and prepares a
    #  pool of workers.
    def __init__(
        self,
        jobSubmission = None,
        numberOfProcesses = multiprocessing.cpu_count(),
        ):
        self.jobSubmission = jobSubmission
        self.numberOfProcesses = numberOfProcesses
        self.className = self.__class__.__name__
        self.status = "starting"
        msg.debug("{notifier}: status: {status}".format(
            notifier = self.className,
            status = self.status)
        )
        self.countOfJobs = None
        self.countOfRemainingJobs = 0
        self.pool = multiprocessing.Pool(
            self.numberOfProcesses,
            initialise_processes
        )
        msg.debug("{notifier}: pool of {numberOfProcesses} {units} created".format(
            notifier = self.className,
            numberOfProcesses = str(self.numberOfProcesses),
            units = units(quantity = self.numberOfProcesses,
            unitSingular = "process", unitPlural = "processes")
        ))
        self.status = "ready"
        msg.debug("{notifier}: status: {status}".format(
            notifier = self.className,
            status = self.status
        ))

    ## @brief return an object self-description string
    #  @details This method returns an object description string consisting of
    #  a listing of the items of the object self.
    #  @return object description string
    def __str__(self):
        descriptionString = ""
        for key, value in sorted(vars(self).items()):
            descriptionString += str("{key}:{value} ".format(
                key = key,
                value = value
            ))
        return descriptionString

    ## @brief print in a human-readable way the items of the object self
    #  @details This function prints in a human-readable way the items of the
    #  object self.
    def printout(self):
        printHR(vars(self)
        )

    ## @brief submit a Job object or a JobGroup object for processing
    #  @details This method submits a specified Job object or JobGroup object
    #  for processing. On successful submission, it returns the value 0.
    #  @param jobSubmission Job object or JobGroup object for submission
    def submit(
        self,
        jobSubmission = None
        ):
        # If the input submission is not None, then update the jobSubmission
        # data attribute to that specified for this method.
        if jobSubmission != None:
            self.jobSubmission = jobSubmission
        self.status = "submitted"
        msg.debug("{notifier}: status: {status}".format(
            notifier = self.className,
            status = self.status
        ))
        # If the input submission is a Job object, contain it in a JobGroup
        # object.
        if isinstance(self.jobSubmission, Job):
            jobGroup = JobGroup(
                jobs = [self.jobSubmission,],
            )
            self.jobSubmission = jobGroup
        # Count the number of jobs.
        self.countOfJobs = len(self.jobSubmission.jobs)
        self.countOfRemainingJobs = self.countOfJobs
        # Build a contemporary list of the names of jobs.
        self.listOfNamesOfRemainingJobs = []
        for job in self.jobSubmission.jobs:
            self.listOfNamesOfRemainingJobs.append(job.name)
        msg.debug("{notifier}: received job group submission '{name}' of {countOfJobs} {units}".format(
            notifier = self.className,
            name = self.jobSubmission.name,
            countOfJobs = self.countOfJobs,
            units = units(
                quantity = self.countOfRemainingJobs,
                unitSingular = "job",
                unitPlural = "jobs"
            )
        ))
        msg.debug(self.statusReport())
        msg.debug("{notifier}: submitting job group submission '{name}' to pool".format(
            notifier = self.className,
            name = self.jobSubmission.name
        ))
        # Cycle through all jobs in the input submission and apply each to the
        # pool.
        for job in self.jobSubmission.jobs:
            job.timeStampSubmission = time.time()
            msg.debug("{notifier}: job '{name}' submitted to pool".format(
                notifier = self.className,
                name = job.name
            ))
            # Apply the job to the pool, applying the object pool.ApplyResult
            # to the job as a data attribute.
            job.resultGetter = self.pool.apply_async(
                func = job.workFunction,
                kwds = job.workFunctionKeywordArguments
            )
        # Prepare monitoring of job group times in order to detect a job group
        # timeout by recording the time of complete submission of the job group.
        self.jobSubmission.timeStampSubmission = time.time()
        msg.debug("{notifier}: job group submission complete: {countOfJobs} {units} submitted to pool (timestamp: {timeStampSubmission})".format(
            notifier = self.className,
            countOfJobs = self.countOfJobs,
            units = units(
                quantity = self.countOfJobs,
                unitSingular = "job",
                unitPlural = "jobs"
            ),
            timeStampSubmission = self.jobSubmission.timeStampSubmission
        ))
        self.status = "processing"
        msg.debug("{notifier}: status: {status}".format(
            notifier = self.className,
            status = self.status
        ))
        return 0

    ## @brief get results of JobGroup object submission
    #  @details This method returns an ordered list of results for jobs
    #  submitted.
    #  @return order list of results for jobs
    def getResults(self):
        # While the number of jobs remaining is greater than zero, cycle over
        # all jobs in the JobGroup object submission submission, watching for a
        # timeout of the JobGroup object submission. If a result has not been
        # retrived for a job (i.e. the Job object does not have a result data
        # attribute), then check if a result is available for the job (using the
        # method multiprocessing.pool.AsyncResult.ready()). If a result is
        # available for the job, then check if the job has run successfully
        # (using the method multiprocessing.pool.AsyncResult.successful()). If
        # the job has not been successful, raise an exception, otherwise, get
        # the result of the job and save it to the result data attribute of the
        # job.
        msg.debug("{notifier}: checking for job {units}".format(
            notifier = self.className,
            units = units(
                quantity = self.countOfRemainingJobs,
                unitSingular = "result",
                unitPlural = "results")
            )
        )
        while self.countOfRemainingJobs > 0:
            # Check for timeout of the job group. If the current timestamp is
            # greater than the job group timeout (derived from the sum of the
            # set of all job timeout specifications in the job group) + the job
            # group submission timestamp, then raise an excepton, otherwise
            # cycle over all jobs.
            # Allow time for jobs to complete.
            time.sleep(0.25)
            if self.jobSubmission.timeoutStatus():
                msg.error("{notifier}: job group '{name}' timed out".format(
                    notifier = self.className,
                    name = self.jobSubmission.name
                ))
                self._abort()
                exceptionMessage = "timeout of a function in list {listOfNamesOfRemainingJobs}".format(
                    listOfNamesOfRemainingJobs = self.listOfNamesOfRemainingJobs
                )
                msg.error("{notifier}: exception message: {exceptionMessage}".format(
                    notifier = self.className,
                    exceptionMessage = exceptionMessage
                ))
                raise trfExceptions.TransformTimeoutException(
                    trfExit.nameToCode('TRF_EXEC_TIMEOUT'),
                    exceptionMessage
                )
            else:
                for job in self.jobSubmission.jobs:
                    self.listOfNamesOfRemainingJobs = []
                    if not hasattr(job, 'result'):
                        # Maintain a contemporary list of the names of remaining
                        # jobs.
                        self.listOfNamesOfRemainingJobs.append(job.name)
                        # If the result of the job is ready...
                        if job.resultGetter.ready():
                            msg.debug(
                                "{notifier}: result ready for job '{name}'".format(
                                    notifier = self.className,
                                    name = job.name
                                )
                            )
                            job.successStatus = job.resultGetter.successful()
                            msg.debug(
                                "{notifier}: job '{name}' success status: {successStatus}".format(
                                    notifier = self.className,
                                    name = job.name,
                                    successStatus = job.successStatus
                                )
                            )
                            # If the job was successful, create the result data
                            # attribute of the job and save the result to it.
                            if job.successStatus:
                                job.result = job.resultGetter.get()
                                msg.debug(
                                    "{notifier}: result of job '{name}': {result}".format(
                                        notifier = self.className,
                                        name = job.name,
                                        result = job.result
                                    )
                                )
                                self.countOfRemainingJobs -= 1
                                msg.debug(
                                    "{notifier}: {countOfRemainingJobs} {units} remaining".format(
                                        notifier = self.className,
                                        countOfRemainingJobs = self.countOfRemainingJobs,
                                        units = units(
                                            quantity = self.countOfRemainingJobs,
                                            unitSingular = "job",
                                            unitPlural = "jobs"
                                        )
                                    )
                                )
                            # If the job was not successful, raise an exception
                            # and abort processing.
                            elif not job.successStatus:
                                msg.error(
                                    "{notifier}: job '{name}' failed".format(
                                        notifier = self.className,
                                        name = job.name
                                    )
                                )
                                self._abort()
                                exceptionMessage = "failure of function '{name}' with arguments {arguments}".format(
                                    name = job.workFunction.__name__,
                                    arguments = job.workFunctionKeywordArguments
                                )
                                msg.error("{notifier}: exception message: {exceptionMessage}".format(
                                    notifier = self.className,
                                    exceptionMessage = exceptionMessage
                                ))
                                raise trfExceptions.TransformExecutionException(
                                    trfExit.nameToCode('TRF_EXEC_FAIL'),
                                    exceptionMessage
                                )
        # All results having been returned, create the 'results' list data
        # attribute of the job group and append all individual job results to
        # it.
        self.jobSubmission.timeStampComplete = time.time()
        self.jobSubmission.completeStatus = True
        msg.debug("{notifier}: all {countOfJobs} {units} complete (timestamp: {timeStampComplete})".format(
            notifier = self.className,
            countOfJobs = self.countOfJobs,
            units = units(
                quantity = self.countOfJobs,
                unitSingular = "job",
                unitPlural = "jobs"
            ),
            timeStampComplete = self.jobSubmission.timeStampComplete
        ))
        self.jobSubmission.processingTime = self.jobSubmission.timeStampComplete - self.jobSubmission.timeStampSubmission
        msg.debug("{notifier}: time taken to process all {units}: {processingTime}".format(
            notifier = self.className,
            countOfJobs = self.countOfJobs,
            units = units(
                quantity = self.countOfJobs,
                unitSingular = "job",
                unitPlural = "jobs"
            ),
            processingTime = self.jobSubmission.processingTime
        ))
        for job in self.jobSubmission.jobs:
            self.jobSubmission.results.append(job.result)
            self._terminate()
        return self.jobSubmission.results
        self._terminate()

    ## @brief return a status report string
    #  @details This method returns a status report string, detailing
    #  information on the JobGroup submission and on the job processing status.
    #  @return status report string
    def statusReport(self):
        statusReport = "\n{notifier}:\n   status report:".format(
            notifier = self.className
        )
        # information on parallel job processor
        statusReport += "\n       parallel job processor configuration:"
        statusReport += "\n          status: {notifier}".format(
            notifier = str(self.status)
        )
        statusReport += "\n          number of processes: {notifier}".format(
            notifier = str(self.numberOfProcesses)
        )
        # information on job group submission
        statusReport += "\n       job group submission: '{notifier}'".format(
            notifier = self.jobSubmission.name
        )
        statusReport += "\n          total number of jobs: {notifier}".format(
            notifier = str(self.countOfJobs)
        )
        statusReport += "\n          number of incomplete jobs: {notifier}".format(
            notifier = str(self.countOfRemainingJobs)
        )
        statusReport += "\n          names of incomplete jobs: {notifier}".format(
            notifier = self.listOfNamesOfRemainingJobs
        )
        # information on jobs (if existent)
        if self.jobSubmission.jobs:
            statusReport += "\n       jobs:"
            for job in self.jobSubmission.jobs:
                statusReport += "\n          job '{name}':".format(
                    name = job.name
                )
                statusReport += "\n              workFunction: '{name}'".format(
                    name = job.workFunction.__name__
                )
                statusReport += "\n              workFunctionKeywordArguments: '{arguments}'".format(
                    arguments = job.workFunctionKeywordArguments
                )
                statusReport += "\n              workFunctionTimeout: '{timeout}'".format(
                    timeout = job.workFunctionTimeout
                )
                if hasattr(job, 'result'):
                    statusReport += "\n              result: '{result}'".format(
                        result = job.result
                    )
        # statistics of parallel job processor run
        if hasattr(self.jobSubmission, 'processingTime'):
            statusReport += "\n       statistics:"
        if hasattr(self.jobSubmission, 'processingTime'):
            statusReport += "\n          total processing time: {processingTime} s".format(
                processingTime = self.jobSubmission.processingTime
            )
        return statusReport

    ## @brief abort parallel job processor
    #  @details This method aborts the parallel job processor. It is used
    #  typically when an exception is raised.
    def _abort(self):
        self.status = "aborting"
        msg.debug("{notifier}: status: {status}".format(
            notifier = self.className,
            status = self.status
        ))
        self._terminate()

    ## @brief terminate parallel job processor
    #  @details This method terminates the parallel job processor. It terminates
    #  the subprocesses of the parallel job processor. It is used typically
    #  when terminating the parallel job processor on successful completion of
    #  job processing and when aborting the parallel job processor.
    def _terminate(self):
        self.status = "terminating"
        msg.debug("{notifier}: status: {status}".format(
            notifier = self.className,
            status = self.status
        ))
        msg.debug("{notifier}: terminating pool of {numberOfProcesses} {units}".format(
            notifier = self.className,
            numberOfProcesses = str(self.numberOfProcesses),
            units = units(
                quantity = self.numberOfProcesses,
                unitSingular = "process",
                unitPlural = "processes"
            )
        ))
        self.pool.terminate()
        self.pool.join()
        self.status = "finished"
        msg.debug("{notifier}: status: {status}".format(
            notifier = self.className,
            status = self.status
        ))
        msg.debug(self.statusReport())

### @brief return Valgrind command
#   @detail This function returns a Valgrind command for use with Athena. The
#   command is returned as a string (by default) or a list, as requested using
#   the argument returnFormat.
#   The function will return a default Valgrind command specification, unless
#   the user suppress them through an option. To append additional options to
#   the command specification the argument extraOptionsList is used. This
#   causes the list of extra specified command options to be appended to
#   the command specification, which will contain the default options unless 
#   these are suppressed.
#   The Athena serialised configuration file is specified using the argument
#   AthenaSerialisedConfigurationFile.
#   @return command as string 
def ValgrindCommand(
    defaultOptions                    = True,
    extraOptionsList                  = None,
    AthenaSerialisedConfigurationFile = "athenaConf.pkl",
    returnFormat                      = "string"
    ):

    # Access Valgrind suppressions files by finding the paths from
    # environment variables. Append the files to the Valgrind suppressions
    # options.
    suppressionFilesAndCorrespondingPathEnvironmentVariables = {
        "etc/valgrind-root.supp": "ROOTSYS",
        "Gaudi.supp":             "DATAPATH",
        "oracleDB.supp":          "DATAPATH",
        "valgrindRTT.supp":       "DATAPATH",
        "root.supp":              "DATAPATH"
    }
    optionsList = ["valgrind"]
    # If default options are not suppressed, use them.
    if defaultOptions:
        optionsList.append("--num-callers=30")
        optionsList.append("--tool=memcheck")
        optionsList.append("--leak-check=full")
        optionsList.append("--smc-check=all")
    # If extra options are specified, append them to the existing options.
    if extraOptionsList:
        for option in extraOptionsList:
            optionsList.append(option)
    # Add suppression files and athena commands
    for suppressionFile, pathEnvironmentVariable in suppressionFilesAndCorrespondingPathEnvironmentVariables.iteritems():
        suppFile = findFile(os.environ[pathEnvironmentVariable], suppressionFile)
        if suppFile:
            optionsList.append("--suppressions=" + suppFile)
        else:
            msg.warning("Bad path to suppression file: {sfile}, {path} not defined".format(
                sfile = suppressionFile, path = pathEnvironmentVariable)
            ) 
    optionsList.append("$(which python)")
    optionsList.append("$(which athena.py)")
    optionsList.append(AthenaSerialisedConfigurationFile)
    # Return the command in the requested format, string (by default) or list.
    if returnFormat is None or returnFormat == "string":
        return(" ".join(optionsList))
    elif returnFormat == "list":
        return(optionsList)
    else:
        print(
            "error: invalid Valgrind command format request (requested " +
            "format: {format}; valid formats: string, list)".format(
            format = returnFormat
        ))
        raise(Exception)


# calculate cpuTime from os.times() times tuple
def calcCpuTime(start, stop):
    cpuTime = None
    if start and stop:
        cpuTime = reduce(lambda x1, x2: x1+x2, map(lambda x1, x2: x2-x1, start[2:4], stop[2:4]))

    return cpuTime

# calculate wallTime from os.times() times tuple
def calcWallTime(start, stop):
    wallTime = None
    if start and stop:
        wallTime = stop[4] - start[4]

    return wallTime

def bind_port(host, port):
    ret = 0
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.bind((host, port))
    except socket.error as e:
        if e.errno == 98:
            print("Port %s is already in use" %port)
        else:
            # something else raised the socket.error exception
            print(e)
        ret=1
    s.close()
    return ret
