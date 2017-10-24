# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfFileUtils
# @brief Transform utilities to deal with files.
# @details Mainly used by argFile class.
# @author atlas-comp-transforms-dev@cern.ch
# @version $Id: trfFileUtils.py 780653 2016-10-27 08:25:09Z mavogel $
# @todo make functions timelimited

import logging
msg = logging.getLogger(__name__)

#  @note Use the PyCmt forking decorator to ensure that ROOT is run completely within 
#  a child process and will not 'pollute' the parent python process with unthread-safe
#  bits of code (otherwise strange hangs are observed on subsequent uses of ROOT)
import PyUtils.Decorators as _decos

from PyUtils.RootUtils import import_root
from PyJobTransforms.trfDecorators import timelimited

# Use a stripped down key list, as we retrieve only 'fast' metadata  
athFileInterestingKeys = ['file_size', 'file_guid', 'file_type', 'nentries']

## @brief New lightweight interface to getting a single file's metadata
def AthenaLiteFileInfo(filename, filetype, retrieveKeys = athFileInterestingKeys):
    msg.debug('Calling AthenaLiteFileInfo for {0} (type {1})'.format(filename, filetype))
    from subprocess import CalledProcessError 

    if filetype == 'POOL':
        # retrieve GUID and nentries without runMiniAthena subprocess
        from PyUtils.AthFileLite import AthInpFile as AthFileLite
    elif filetype == 'BS':
        from PyUtils.AthFileLite import AthBSFile as AthFileLite
    elif filetype == 'TAG':
        from PyUtils.AthFileLite import AthTagFile as AthFileLite
    else:
        msg.error('Unknown filetype for {0} - no lightweight metadata interface for type {1}'.format(filename, filetype))
        return None
    
    metaDict = {}
    try:
        meta = AthFileLite(filename).fileinfo
        msg.debug('AthFileLite came back for {0}'.format(filename))
        metaDict[filename] = {}
        for key in retrieveKeys:
            msg.debug('Looking for key {0}'.format(key))
            try:
                if key is 'G4Version':
                    msg.debug('Searching for G4Version in metadata')
                    try: 
                        metaDict[filename][key] = meta['metadata']['/Simulation/Parameters']['G4Version']
                        msg.debug('Setting G4Version to {0}'.format(meta['metadata']['/Simulation/Parameters']['G4Version']))
                    except (KeyError, TypeError) as e:
                        msg.debug('Could not find G4Version information in metadata for file {0}'.format(filename))
                else:
                    metaDict[filename][key] = meta[key]
            except KeyError:
                msg.warning('Missing key in athFile info: {0}'.format(key))
    except (CalledProcessError, ValueError, AssertionError, ReferenceError) as e:
        msg.error('Problem in getting AthFile metadata for {0}'.format(filename))
        return None
    msg.debug('Returning {0}'.format(metaDict))
    return metaDict    

## @brief Determines number of events in a HIST file.
#  @details Basically taken from PyJobTransformsCore.trfutil.MonitorHistFile
#  @param fileName Path to the HIST file.  
#  @return 
#  - Number of events.
#  - @c None if the determination failed.
#  @note Use the PyCmt forking decorator to ensure that ROOT is run completely within 
#  a child process and will not 'pollute' the parent python process with unthread-safe
#  bits of code (otherwise strange hangs are observed on subsequent uses of ROOT)
@_decos.forking
def HISTEntries(fileName):

    root = import_root()

    fname = root.TFile.Open(fileName, 'READ')
    
    if not (isinstance(fname, root.TFile) and fname.IsOpen()):
        return None

    rundir = None
    keys = fname.GetListOfKeys()
    
    for key in keys:
        
        name=key.GetName()
        
        if name.startswith('run_') and name is not 'run_multiple':
            
            if rundir is not None:
                msg.warning('Found two run_ directories in HIST file %s: %s and %s' % ( fileName, rundir, name) )
                return None
            else:
                rundir = name
                
        del name
       
    if rundir is None:
        msg.warning( 'Unable to find run directory in HIST file %s' % fileName )
        fname.Close()
        return None
    
    msg.info( 'Using run directory %s for event counting of HIST file %s. ' % ( rundir, fileName ) )
    
    hpath = '%s/GLOBAL/DQTDataFlow/events_lb' % rundir
    possibleLBs = []
    if 'tmp.HIST_' in fileName:
        msg.info( 'Special case for temporary HIST file {0}. '.format( fileName ) )
        h = fname.Get('{0}'.format(rundir))
        for directories in h.GetListOfKeys() :
            if 'lb' in directories.GetName():
                msg.info( 'Using {0} in tmp HIST file {1}. '.format(directories.GetName(),  fileName ) )
                hpath = rundir+'/'+str(directories.GetName())+'/GLOBAL/DQTDataFlow/events_lb' 
                possibleLBs.append(hpath)
    else:
        msg.info( 'Classical case for HIST file {0}. '.format( fileName ) )
        possibleLBs.append(hpath)
    nev = 0
    if len(possibleLBs) == 0:
        msg.warning( 'Unable to find events_lb histogram in HIST file %s' % fileName )
        fname.Close()
        return None
    for hpath in possibleLBs:
        h = fname.Get(hpath)
        
        if not isinstance( h, root.TH1 ):
            msg.warning( 'Unable to retrieve %s in HIST file %s.' % ( hpath, fileName ) )
            fname.Close()
            return None
        
        nBinsX = h.GetNbinsX()
        nevLoc = 0
        
        for i in xrange(1, nBinsX):
            
            if h[i] < 0:
                msg.warning( 'Negative number of events for step %s in HIST file %s.' %( h.GetXaxis().GetBinLabel(i), fileName ) )
                fname.Close()
                return None
            
            elif h[i] == 0:
                continue
            
            if nevLoc == 0:
                nevLoc = h[i]
                
            else:
                if nevLoc != h[i]:
                    msg.warning( 'Mismatch in events per step in HIST file %s; most recent step seen is %s.' % ( fileName, h.GetXaxis().GetBinLabel(i) ) )
                    fname.Close()
                    return None
        nev += nevLoc        
    fname.Close()
    return nev



## @brief Determines number of entries in NTUP file with given tree names.
#  @details Basically taken from PyJobTransformsCore.trfutil.ntup_entries.
#  @param fileName Path to the NTUP file.
#  @param treeNames Tree name or list of tree names.
#  In the latter case it is checked if all trees contain the same number of events  
#  @return 
#  - Number of entries.
#  - @c None if the determination failed.
#  @note Use the PyCmt forking decorator to ensure that ROOT is run completely within 
#  a child process and will not 'pollute' the parent python process with unthread-safe
#  bits of code (otherwise strange hangs are observed on subsequent uses of ROOT)
@_decos.forking
def NTUPEntries(fileName, treeNames):
    
    if not isinstance( treeNames, list ):
        treeNames=[treeNames]
        
    root = import_root()
    
    fname = root.TFile.Open(fileName, 'READ')
    
    if not (isinstance(fname, root.TFile) and fname.IsOpen()):
        return None
    
    prevNum=None
    prevTree=None
               
    for treeName in treeNames:
            
        tree = fname.Get(treeName)
            
        if not isinstance(tree, root.TTree):
            return None
        
        num = tree.GetEntriesFast()

        if not num>=0:
            msg.warning('GetEntriesFast returned non positive value for tree %s in NTUP file %s.' % ( treeName, fileName ))
            return None
                
        if prevNum is not None and prevNum != num:
            msg.warning( "Found diffferent number of entries in tree %s and tree %s of file %s." % ( treeName, prevTree, fileName  ))
            return None
        
        numberOfEntries=num
        prevTree=treeName
        del num
        del tree

    fname.Close()

    return numberOfEntries


## @brief Determines number of entries in PRW file
#  @param fileName Path to the PRW file.
#  @param integral Returns sum of weights if true
#  @return 
#  - Number of entries.
#  - Sum of weights if integral is true.
#  - @c None if the determination failed.
#  @note Use the PyCmt forking decorator to ensure that ROOT is run completely within 
#  a child process and will not 'pollute' the parent python process with unthread-safe
#  bits of code (otherwise strange hangs are observed on subsequent uses of ROOT)
@_decos.forking
def PRWEntries(fileName, integral=False):

    root = import_root()

    fname = root.TFile.Open(fileName, 'READ')

    if not (isinstance(fname, root.TFile) and fname.IsOpen()):
        return None

    rundir = None

    for key in fname.GetListOfKeys():
        if key.GetName()=='PileupReweighting':
            rundir = fname.Get('PileupReweighting')
            break
        # Not PRW...

    if rundir is None: return None

    total = 0
    for key in rundir.GetListOfKeys():
        if 'pileup' in key.GetName():
            msg.debug('Working on file '+fileName+' histo '+key.GetName())
            if integral:
                total += rundir.Get(key.GetName()).Integral()
            else:
                total += rundir.Get(key.GetName()).GetEntries()
        # Was not one of our histograms
    return total


## @brief Get the size of a file via ROOT's TFile
#  @details Use TFile.Open to retrieve a ROOT filehandle, which will
#  deal with all non-posix filesystems. Return the GetSize() value.
#  The option filetype=raw is added to ensure this works for non-ROOT files too (e.g. BS)
#  @note Use the PyCmt forking decorator to ensure that ROOT is run completely within 
#  a child process and will not 'pollute' the parent python process with unthread-safe
#  bits of code (otherwise strange hangs are observed on subsequent uses of ROOT)
#  @param filename Filename to get size of
#  @return fileSize or None if there was a problem
@_decos.forking
def ROOTGetSize(filename):
    root = import_root()
    
    try:
        msg.debug('Calling TFile.Open for {0}'.format(filename))
        extraparam = '?filetype=raw'
        if filename.startswith("https"):
            try:
                pos = filename.find("?")
                if pos>=0:
                    extraparam = '&filetype=raw'
                else:
                    extraparam = '?filetype=raw'
            except:
                extraparam = '?filetype=raw'
        fname = root.TFile.Open(filename + extraparam, 'READ')
        fsize = fname.GetSize()
        msg.debug('Got size {0} from TFile.GetSize'.format(fsize))
    except ReferenceError:
        msg.error('Failed to get size of {0}'.format(filename))
        return None
    
    fname.Close()
    del root
    return fsize
    

## @brief Return the LAN access type for a file URL
#  @param filename Name of file to examine
#  @return
#  - String with LAN protocol
def urlType(filename):
    if filename.startswith('dcap:'):
        return 'dcap'
    if filename.startswith('root:'):
        return 'root'
    if filename.startswith('rfio:'):
        return 'rfio'
    if filename.startswith('file:'):
        return 'posix'
    if filename.startswith('https:'):
        return 'root'
    return 'posix'

