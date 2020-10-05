#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# -*- coding: utf-8 -*-

from __future__ import print_function

import os, shutil
import time
from time import sleep
from DataQualityUtils.dqu_subprocess import apply as _local_apply

## Needed to correct ROOT behavior; see below
CWD = os.getcwd() # noqa

import sys
import ROOT
## Importing gSystem may change the current directory to one of the
## command-line arguments; chdir to original directory to have
## predictable behavior
from ROOT import gSystem

from DataQualityUtils.handimod import handiWithComparisons
from DataQualityUtils.handimod import makeCSSFile
from DataQualityUtils.SingleAppInstance import SingleAppInstance
from DataQualityUtils.hancoolmod import hancool
from DataQualityUtils.DQHistogramMergeMod import DQHistogramMerge
import DataQualityUtils.HanMetadata

gSystem.Load("libdqm_core"); # noqa 
gSystem.Load("libDataQualityInterfaces"); # noqa
from ROOT import dqutils

os.chdir(CWD)

## libDataQualityUtils is loaded by handi
VERSION = '$Id: DQWebDisplay.py 529090 2012-12-05 22:32:03Z ponyisi $'
FROMEMAIL = 'atlasdqm@cern.ch'

# return code
LOCK_ERROR = 5


## Get usable signal handlers
os.environ['TDAQ_ERS_NO_SIGNAL_HANDLERS'] = '1'


class LockAcquisitionException(Exception):
    def __init__(self, msg):
        super(LockAcquisitionException,self).__init__(msg)

def DQWebDisplay( inputFilePath, runAccumulating, c ):
    
    print("")
    print("SVN version:")
    print(VERSION)

    # we use the multiprocessing module to isolate code that possibly
    # has memory leaks, so they don't build up over time

    if isinstance(c.server, str):
        if c.server == '':
            c.server = []
        else:
            c.server = [c.server]
    
    if c.server != [] or c.webHandoffDir != "" or c.lockFile == '':
        print("")
        print("Lockfile is disabled")
        print("")
        instance = None
    else:
        while True:
            try:
                instance = SingleAppInstance( c.htmlDir + "/" + c.lockFile, True )
                islocked = instance.acquire()
            except SingleAppInstance.FileLockAcquisitionError as e:
                print('=========================================================================')
                print('Cannot acquire lock on', c.htmlDir)
                print('This usually means either')
                print('  -> the directory does not exist,')
                print('  -> you do not have the required permissions, or')
                print('  -> the directory is out of space.')
                print('If you ran into this error while trying to test the web display, please')
                print('read the instructions at:')
                print('https://twiki.cern.ch/twiki/bin/view/Atlas/DQOperationalRecipes#Obtain_SVN_and_disk_write_permis')
                print()
                print('Explicit error:', e)
                print('=========================================================================')
                sys.exit(1)
            if islocked:
                break
            print("Another instance is running; waiting 60 seconds...")
            sleep(60)

    ## Settings for compare button, hintbox (and browser menu in case you want it)
    #javaScript = "compare.js"   # javascript for compare button
    #HjavaScript = "returnHistory.js"   # javascript for history button 
    #allDirsScript = "AllDirs.js" # javascript for browser menu and hintbox
    
    #javaScriptLoc =  c.htmlWeb + "/" + javaScript
    #HjavaScriptLoc =  c.htmlWeb + "/" + HjavaScript
    allDirsScriptLoc = c.htmlWeb #+ "/" + allDirsScript
    runlistLoc = c.htmlWeb + "/" +  c.runlist ## datafile catalog (run and streams),
    
    compare = True   # Create the compare button
    #history = True   # Create the history button
    browserMenu = True  # Use the menu interface
    
    if ( c.hanResultsDir.rfind("/")!=(len(c.hanResultsDir)-1) ):
        c.hanResultsDir+="/"
    
    if ( c.htmlDir.rfind("/")!=(len(c.htmlDir)-1) ):
        c.htmlDir+="/"
    
    inputFileName = inputFilePath
    i = inputFilePath.rfind("/")
    if( i != -1 ):
        inputFileName = inputFilePath[i+1:]
    
    runid       = 99999999
    shortStream = "NoStream"
    amitag      = None
    procpass    = int(sys.argv[3])

    if c.config == "RTT":
        shortStream = time.strftime("%A")
    else:
        fields = inputFileName.split(".")
        if len(fields) >= 3:
            runid = int(fields[1])
            shortStream = fields[2]
        if len(fields) >= 6:
            amitag = fields[5]
    
    if runAccumulating:
        print("")
        print("Searching for existing histogram cache to merge with input histograms...")
        try:
            inputFilePath = _local_apply(mergeAndCache, ( inputFilePath, runid, shortStream, c.histogramCache ) )
        except LockAcquisitionException:
            sys.exit(LOCK_ERROR)
        shortStream = "tmp_" + shortStream
    elif c.histogramCache != "":
        cacheFile, version = findCacheFile( inputFilePath, runid, shortStream, c.histogramCache )
        if cacheFile != "":
            print("")
            print("Removing cached histograms used during accumulation mode")
        try:
            while cacheFile != '':
                fullCachePath = c.histogramCache + "/" + cacheFile
                os.unlink( fullCachePath )
                cacheFile, version = findCacheFile( inputFilePath, runid, shortStream, c.histogramCache )
        except OSError:
            pass

    stream = shortStream
    if len(sys.argv) >= 5:
        stream = repr(procpass) + "/" + shortStream
    
    outputHanResultsDir = c.hanResultsDir + stream + '/run_' + repr(runid)
    outputHtmlDir = c.htmlDir + stream + "/"
    
    if c.server != [] or c.webHandoffDir != "" or c.eosResultsDir:
        print("Writing all output to \'./\'; will copy at the end to", end='')
        if c.webHandoffDir != "":
            print("a handoff directory:", c.webHandoffDir)
        else:
            print("the web server(s):", ', '.join(c.server))
        print("")
        outputHanResultsDir = "./han_results/" + stream + '/run_' + repr(runid)
        outputHtmlDir = "./www/" + stream + "/"
    
    makeOutputDirectory( outputHanResultsDir )
    makeOutputDirectory( outputHtmlDir )
    
    total=_local_apply(getHanResults, ( inputFilePath, outputHanResultsDir, inputFilePath, c.hcfg, c.hcfg_min10, c.hcfg_min30 ))
    fileList=total.rsplit('\n')
    if amitag is not None:
        addAmiTags(fileList, amitag)
    
    number=len(fileList) 
    if (len(fileList[number-1])<1): # last line is empty
        number-=1

    # find run number to create index.html in run-dir
    runfile=fileList[0].rsplit()
    if (len(runfile) == 0):
        print("ERROR: TFile has no \"run_*\" directories.  The input monitoring file is not organized for production use,")
        print("probably because AthenaMonitoring is not configured properly.  Exiting.\n")
        sys.exit(1)
    
    xferFileList = []
    for allLines in fileList:
        splitLine = allLines.rsplit()
        if len(splitLine) >= 1:
            xferFileList.append( splitLine[0] + "\n" )
    
    ## Archive han results - 090422 : PUEO
    if c.server != []:
        failures = 0
        for server in c.server:
            print("Transfering han files to server: ", server)
            success = transferFilesToServer( xferFileList[:], "./han_results/", c.hanResultsDir, server )
            if success:
                print("Done.")
                print("")
            else:
                failures += 1
                print("FAILED!", end='')
                if c.emailWarnings:
                    email('The transfer of han files\n\n' +
                          ''.join(xferFileList) +
                          '\nto server ' + server +
                          ' failed.  This may indicate that this server is down.\n'
                          'Please investigate as soon as possible!',
                          'WARNING! File transfer from Tier-0 failed',
                          FROMEMAIL,
                          'hn-atlas-data-quality-operations@cern.ch'
                    )
                    print("Email sent...")
                else:
                    print("")
                print("")
        if failures == len(c.server):
            print("SERIOUS PROBLEM: file transfers failed to ALL defined servers")
            print("These are:", ', '.join(c.server))
            print("Will die so as to alert Tier-0 shifter")
            raise IOError('tarfile ssh transfer failed')
    if c.eosResultsDir != "":
        print("Transfering han files to EOS")
        success_EOS = transferFilesToEOS( xferFileList[:], "./han_results/", c.eosResultsDir )
        if success_EOS:
            print("Done.")
            print("")
        else:
            print("FAILED!",)
            if c.emailWarnings:
                email('The transfer of han files\n\n' +
                      ''.join(xferFileList) +
                      '\nto EOS failed.\n'
                      'Please investigate as soon as possible!',
                      'WARNING! File transfer from Tier-0 failed',
                      FROMEMAIL,
                      'hn-atlas-data-quality-operations@cern.ch'
                      )
                print("Email sent...")
            else:
                print("")
                        

    runNumber = runfile[1]
    rN=runNumber.lstrip("run_")
    if c.doHandi:
        rundir=outputHtmlDir+runNumber+"/"
        makeOutputDirectory( rundir )

        ## local time
        now = time.localtime()
        lastUpdate = "Last Update: "+time.strftime('%Y-%m-%d %H:%M %Z', now)

        ## # make an index file with links to run and minutes files
        g=open(outputHtmlDir+runNumber +'/index.html','w')
        g.write('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">\n')
        g.write('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">\n')
        g.write('<head>\n')
        g.write('<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />\n')
        g.write('<title>run ' +rN+ ' ' + stream + '</title>\n')
        g.write('<link rel="stylesheet" href="AutomChecks.css" type="text/css" />\n')
        g.write('</head>\n')
        g.write('<body>\n')
        g.write('<font class="DQGroup">[<a href="#" onclick="history.go(-1);return false;">Back</a>]</font>\n')
        g.write('<h1>Run '+rN+', ' + stream + '</h1>\n\n')

        makeCSSFile( rundir, "", "" )

        if number >0:
            min10List = []
            min30List = []
            for x in range(0,number):
                sp=fileList[x].rsplit()
                if (len(sp)==2): # the run file
                    print("Running handi on " + sp[0] + ":")
                    print('handi("Run '+rN+ ', '+ stream + '",' + sp[0] + ','+rundir+'/run)')
                    _local_apply(handiWithComparisons, ( ('Run '+rN+', ' + stream), sp[0], (rundir+"/run"), runlistLoc, compare, browserMenu, allDirsScriptLoc) )
                    print("")
                else:# a minutes file
                    if sp[1].find("lowStat")!=-1: # low statistics interval
                        print("Running handi on " + sp[0] + ":")
                        print('handi("Run '+rN+ ', '+sp[1]+', ' + stream + '",' + sp[0] + ','+rundir+sp[3]+')')
                        _local_apply(handiWithComparisons, ( ('Run '+rN+ ', '+sp[1]+', ' + stream), sp[0], (rundir+sp[3]), runlistLoc, compare, browserMenu, allDirsScriptLoc) )
                        print("")
                        mN = sp[1].lstrip("lowStat_")
                        min10List.append( (sp[3], mN) )
                    elif sp[1].find("medStat")!=-1: # medium statistics interval
                        print("Running handi on " + sp[0] + ":")
                        print('handi("Run '+rN+ ', '+sp[1]+', ' + stream + '",' + sp[0] + ','+rundir+sp[3]+')')
                        _local_apply(handiWithComparisons, (('Run '+rN+ ', '+sp[1]+', ' + stream), sp[0], (rundir+sp[3]), runlistLoc, compare, browserMenu, allDirsScriptLoc))
                        print("")
                        mN = sp[1].lstrip("medStat_")
                        min30List.append( (sp[3], mN) )

            #min10List.sort(key=lambda x: int(x[1]))
            #min30List.sort(key=lambda x: int(x[1]))

            g.write('<table>\n')

            if len(min10List) == 0 and len(min30List) == 0:
                g.write('<tr>\n')
                g.write(' <td width=120>\n')
                g.write('  <a href="run/index.html">Entire Run</a>\n')
                g.write(' </td>\n')
                g.write('</tr>\n')
            elif len(min10List) == 0 or len(min30List) == 0:
                minList = []
                blockName = ""
                if len(min10List) == 0:
                    minList = min30List
                    blockName = "medium stat interval"
                else:
                    minList = min10List
                    blockName = "low stat interval"
                g.write('<tr>\n')
                g.write(' <td rowspan='+ str(len(minList)) +' valign="top" width=120>\n')
                g.write('  <a href="run/index.html">Entire Run</a>\n')
                g.write(' </td>\n')
                margin = ""
                minList.sort()
                for page, nMin in minList:
                    g.write(margin)
                    g.write(' <td width=200>\n')
                    if nMin.find('merged_')!=-1:
                        g.write('  <a href="' + page + '/index.html">' + nMin + '</a>\n')
                    else:
                        g.write('  <a href="' + page + '/index.html">' + blockName + ' ' + nMin + '</a>\n')
                    g.write(' </td>\n')
                    margin = '</tr>\n<tr>\n'
                g.write('</tr>\n')
            else:
                g.write('<tr>\n')
                g.write(' <td rowspan='+ str(len(min10List)) +' valign="top" width=120>\n')
                g.write('  <a href="run/index.html">Entire Run</a>\n')
                g.write(' </td>\n')
                margin = ""
                count = 0
                min10List.sort()
                for page10, nMin10 in min10List:
                    g.write(margin)
                    nMin30 = 0
                    if count % 3 == 0:
                        iMin30 = (count / 3)
                        page30, nMin30 = min30List[iMin30]
                        g.write(' <td rowspan=3 valign="top" width=200>\n')
                        g.write('  <a href="' + page30 + '/index.html">medium stat interval ' + nMin30 + '</a>\n')
                        g.write(' </td>\n')
                    g.write(' <td width=200>\n')
                    g.write('  <a href="' + page10 + '/index.html">low stat interval ' + nMin10 + '</a>\n')
                    g.write(' </td>\n')
                    count = count + 1
                    margin = '</tr>\n<tr>\n'
                g.write('</tr>\n')

            g.write('</table>\n')


        g.write('<br/>\n<font class=\"Note\">'+lastUpdate+'</font><br />\n')
        g.write('</body>\n</html>')
        g.close()

        if c.webHandoffDir != "":
            print('Transfering web files to handoff directory')
            transferDirectoryToHandoffDir( rundir, './www/', c.webHandoffDir, c )
        elif c.server != []:
            failures = 0
            for server in c.server:
                print("Transfering web files to server: ", server)
                success = transferDirectoryToServer( rundir, "./www/", c.htmlDir, server )
                if success:
                    print("Done.")
                    print("")
                else:
                    failures += 1
                    if c.emailWarnings:
                        email('The transfer of web files\n'
                              + c.htmlDir +
                              '\nto server' + server +
                              'failed.  This may indicate that this server is down.\n'
                              'Please investigate as soon as possible!',
                              'WARNING! File transfer from Tier-0 failed',
                              FROMEMAIL,
                              'hn-atlas-data-quality-operations@cern.ch'
                              )
                    print("FAILED!")
                    print("")
            if failures == len(c.server):
                print("SERIOUS PROBLEM: web files transfer failed to ALL defined servers")
                print("These are:", ', '.join(c.server))
                print("Will die so as to alert Tier-0 shifter")
                raise IOError('Directory ssh transfer failed')
                

        #markCompleteTransfer( rundir, './www/', c.htmlDir, c )

        if c.webHandoffDir == '':
            print("Generating index files...")
            for server in c.server:
                generateDQIndexFiles( server, c.htmlDir, c.config, c.indexFile, c.runlist, c.htmlWeb )
    
    if not runAccumulating:
        print("Getting COOL authentications")
#        if c.auth:
#            os.environ['CORAL_AUTH_PATH'] = c.auth    
#        else:
#            home = os.environ.get('HOME')
#            os.environ['CORAL_AUTH_PATH'] = home+"/private"        
               
        # write monitor comparison results to cool
        # MB : 29.05.2008
        if 'COOLUPLOADS' in os.environ:
            uploadAllowed = (os.environ['COOLUPLOADS'] == '1')
        else:
            uploadAllowed = True

        if not uploadAllowed:
            print('hancool run and upload to DB switched off by request')
            
        if (uploadAllowed and len(c.dbConnection)>0) and (len(c.dqmfOfl)>0):
            print("")
            print("Now calling hancool ...")
            #stream = os.path.basename(outputHanResultsDir)
            if '_' in stream: stream = stream.split('_',1)[-1]
            if amitag is not None and 'x' in amitag and stream.startswith('express'): # this is ESn processing
                dbTagName = c.dbTagNameESn
                isESn = True
            else:
                dbTagName = c.dbTagName
                isESn = False
            if '%' in dbTagName:
                # this is a little fragile
                uploadTag = dbTagName % { 'stream': stream,
                                          'amitag': amitag,
                                          'procpass': procpass }
                doUpload = True
            else:
                uploadTag = dbTagName
                doUpload = ('-%s-' % stream in uploadTag)

            if (doUpload):
                print('isESn?', isESn)
                _local_apply(hancool, (int(rN),outputHanResultsDir,c.dbConnection,c.dqmfOfl,uploadTag,c.shiftOfl,c.dbTagNameShiftOfl,isESn))

    if runAccumulating:
        os.unlink(inputFilePath)
    
    if instance is not None:
        instance.release()
    return fileList


def makeOutputDirectory( dirname ):
    if( not os.access(dirname,os.F_OK) ):
        try:
            os.umask(0o002)
            os.makedirs(dirname)
        except os.error:
            print('Cannot create directory "' + dirname + '"; exiting.')
            sys.exit(-1)


def getHanResults(inputFilePath, *args):
    of = dqutils.MonitoringFile(inputFilePath)
    rv = of.getHanResults(*args)
    return rv

def mergeAndCache( inputFilePath, run, stream, cache ):
    if cache == "":
        print("Cannot use cache: No cache directory defined")
        print("")
        return inputFilePath

    MAX_LOCK_TRIES = 3

    for i in range(MAX_LOCK_TRIES):
        try:
            lockfilename = os.path.join(cache, 'lock_%s_%s' % (run, stream))
            instance = SingleAppInstance( lockfilename, True )
            islocked = instance.acquire()
        except SingleAppInstance.FileLockAcquisitionError as e:
            print('Unable to start to acquire cache directory lock! Bailing')
            raise e
        if islocked:
            break
        else:
            print('Unable to acquire cache directory lock; waiting 60 seconds; try', (i+1))
            sleep(60)

    if not islocked:
        raise LockAcquisitionException(' '.join(['Tried', repr(MAX_LOCK_TRIES), 'times to acquire cache lock; exiting now']))
    
    cacheFile, version = findCacheFile( inputFilePath, run, stream, cache )
    fullCachePath = ""
    if cacheFile != "":
        print("Found cached histograms: ")
        print("   dir: ", cache)
        print("  file: ", cacheFile)
        fullCachePath = cache + "/" + cacheFile
    else:
        print("Did not find cached histograms: ")
        print("   dir: ", cache)
        
    inputFileName = inputFilePath
    i = inputFilePath.rfind("/")
    if( i != -1 ):
        inputFileName = inputFilePath[i+1:]
    updatedFile = inputFileName + ".CACHE_" + str(version+1)
    print("Creating updated cache: ")
    print("  file: ", updatedFile)
    fullUpdatedFile = cache + "/" + updatedFile
    print("")

    if fullCachePath != '':
        print('Copying previous cache to local directory ...', end='')
        shutil.copy(fullCachePath, cacheFile)
        print('done\n')
  
    mergeListName = "cache_merge.list"
    f = open( mergeListName, 'w' )
    txtstr = ""
    txtstr += "%s\n" % cacheFile
    txtstr += "%s\n" % inputFilePath
    f.write(txtstr)
    f.close()

    print("Running DQHistogramMerge...")
    DQHistogramMerge( mergeListName, updatedFile, False )
    print("Finished DQHistogramMerge.")
    print("")

    if fullCachePath != '':
        print('Deleting local copy of old cache ...', end='')
        os.unlink(cacheFile)
        print('done\n')

    print('Copying new cache to cache directory ...', end='')
    shutil.copy(updatedFile, fullUpdatedFile)
    print('done\n')

    # make copy so we can process further without interference
    # from other jobs
    #localFullUpdatedFile = fullUpdatedFile + '-local'
    #print('Copying to new file', localFullUpdatedFile, '...', end='')
    #shutil.copy(updatedFile, localFullUpdatedFile)
    #print('done')

    os.unlink( mergeListName )
    if fullCachePath != "":
        os.unlink( fullCachePath )
        
    instance.release()

    print('Now running histogram postprocessing ...')
    from DataQualityUtils import DQPostProcessMod
    DQPostProcessMod.DQPostProcess( updatedFile, isIncremental=True )
  
    return updatedFile

def verifyHasRunKey(inputFile, run):
    f = ROOT.TFile(inputFile, 'READ')
    if f.IsOpen():
        if f.Get('run_%s' % run):
            f.Close()
            return True
    f.Close()
    print('WARNING: cache file %s appears to be corrupted' % inputFile)
    return False

def findCacheFile( inputFilePath, run, stream, cache ):
    fileDict = { }
    for root, dirs, files in os.walk(cache):
        removeDir = []
        for name in dirs:
            removeDir.append(name)
        for fileName in files:
            if fileName.find(str(run)) == -1:
                continue
            if fileName.find(stream) == -1:
                continue
            vi = fileName.rfind(".CACHE_")
            if vi != -1:
                try:
                    v = int(fileName[vi+7:])
                    fileDict[v] = fileName
                except Exception:
                    pass
        for name in removeDir:
            dirs.remove(name)
    for v in reversed(sorted([key for key in fileDict.keys() if verifyHasRunKey(cache+'/'+fileDict[key], run)])):
        return fileDict[v], v
  
    return "", 0


def transferDirectoryToHandoffDir( dirName, localDir, targetDir, config ):
    import time, shutil, glob
    import six.moves.configparser as configparser
    targetfname = repr(int(time.time())) + '-' + repr(os.getpid()) \
                  + '-' + os.uname()[1]  + '.tgz'
    targetfile = os.path.join(targetDir, targetfname)

    print('Creating tarball', targetfname, '...')
    parser = configparser.ConfigParser()
    parser.set('DEFAULT', 'target', config.htmlDir)
    parser.add_section('execute')
    parser.set('execute', 'command', config.htmlDir + '/generateDQIndexFiles.py')
    parser.set('execute', 'options', '"%s" "%s" "%s" "%s" "%s"' % (config.htmlDir, config.config, config.indexFile, config.runlist, config.htmlWeb))
    dnames = glob.glob('%s/*/*/*' % localDir)
    if len(dnames) != 1:
        print('Unexpected glob result')
    else:
        parser.add_section('webdisplay')
        parser.set('webdisplay', 'id', dnames[0][len(localDir):])
    manifest = open(localDir + '/MANIFEST', 'w')
    parser.write(manifest); manifest.close()

    #loctf = tarfile.open(targetfname, 'w:gz')
    ##loctf.debug=3
    #loctf.add(localDir + '/MANIFEST', 'MANIFEST')
    #loctf.add(dirName, dirName.replace(localDir, ''))
    #loctf.close()
    print('About to execute', end='')
    cmd = 'cd %s ; tar czf %s MANIFEST %s ; mv %s ..' % (localDir, targetfname, dirName.replace(localDir, ''), targetfname)
    print(cmd)
    rv = os.system(cmd)
    if rv != 0:
        raise IOError('Unable to create tarfile')
    
    print('done')

    print('Copying tarball to', targetDir, '...', end='')
    shutil.copy(targetfname, targetfile)
    print('done')
    print('Removing local tarball copy ...', end='')
    os.unlink(targetfname)
    print('done\n')

def transferDirectoryToServer( dirName, localDir, targetDir, server ):
    username = "atlasdqm"
    if 'USER' in os.environ.keys():
        username = os.environ['USER']
    
    dirName = dirName.replace(localDir,"")
    
    cmd = ""
    cmd += "tar -czf - -C " + localDir + " " + dirName
    cmd += " | "
    cmd += "ssh -l " + username + " " + server + " -x \"umask 002; cd " + targetDir + " ; tar -xzf - \""
    
    print(cmd)
    rv = os.system( cmd )
    #if rv != 0:
    #    raise IOError('Directory ssh transfer failed')
    return rv == 0

def markCompleteTransfer( dirName, localDir, targetDir, config ):
    if config.webHandoffDir != '':
        return
    if config.server == '':
        os.system('touch ' + os.path.join(dirName, 'complete'))
    else:
        dirName = dirName.replace(localDir, '')
        username = "atlasdqm"
        if 'USER' in os.environ.keys():
            username = os.environ['USER']
        cmd = "ssh -l " + username + " " + config.server + " -x \"umask 002; cd " + os.path.join(targetDir, dirName) + " ; touch complete\""

        print(cmd)
        rv = os.system( cmd )
        if rv != 0:
            raise IOError('Directory complete transfer marking failed')
        

def transferFilesToServer( fileList, localDir, targetDir, server ):
    username = "atlasdqm"
    if 'USER' in os.environ.keys():
        username = os.environ['USER']
    tarFileListName = "./tarFileList.tmp"
    t = open( tarFileListName, 'w' )
    for xferFile in fileList:
        os.system( "chmod 664 " + xferFile )
        xferFile = xferFile.replace(localDir,"")
        t.write(xferFile)

    t.close()
    
    cmd = ""
    cmd += ("tar -czf - -C %s -T %s" % (localDir, tarFileListName))
    cmd += " | "
    cmd += "ssh -l " + username + " " + server + " -x \"umask 002; cd " + targetDir + " ; tar -xzf - \""
    
    print(cmd)
    rv = os.system( cmd )
    os.unlink(tarFileListName)
    #if rv != 0:
    #    raise IOError('tarfile ssh transfer failed')
    return rv == 0

def transferFilesToEOS(fileList, localDir, eosResultsDir):
    os.system('export XRD_REQUESTTIMEOUT=10')
    run_eos = 0
    for xferFile in fileList:
        xferFile = xferFile.rstrip()
        os.system( "chmod 664 " + xferFile )
        file_name = xferFile.replace(localDir,"")
      
        eos = "xrdcp --force --path --silent " + xferFile + " "+ os.path.join(eosResultsDir, file_name)

        print(eos)
        run_eos += os.system( eos )
    
    return run_eos == 0

def retrieveFileFromServer( filePath, localDir, remoteDir, server ):
    username = "atlasdqm"
    if 'USER' in os.environ.keys():
        username = os.environ['USER']
    
    filePath = filePath.replace(localDir,"")
    
    cmd = ""
    cmd += "cd " + localDir
    cmd += " ; "
    cmd += "ssh -l " + username + " " + server + " -x \"cd " + remoteDir + " ; cat " + filePath + "\""
    cmd += " > " + filePath
    
    print(cmd)
    rv = os.system( cmd )
    if rv != 0:
        raise IOError('file retrieve failed')

def generateDQIndexFiles( server, installPath, project, indexFile, runListFile, htmlAddress ):
    username = "atlasdqm"
    if 'USER' in os.environ.keys():
        username = os.environ['USER']
    
    cmd = ""
    
    if server != "":
        cmd += "ssh -l " + username + " " + server + " -x \"umask 002; "
        cmd += installPath + "/generateDQIndexFiles.py " + installPath + " " + project + " " + indexFile + " "
        cmd += runListFile + " " + htmlAddress + "\""
    else:
        cmd += installPath + "/generateDQIndexFiles.py " + installPath + " " + project + " " + indexFile + " "
        cmd += runListFile + " " + htmlAddress
    
    print(cmd)
    return os.system( cmd ) == 0

def importConfiguration(modname):
    from DataQualityConfigurations import getmodule
    print('getting configuration', modname)
    return getmodule(modname)

def email(msg, subject, whofrom, addressees):
    import smtplib
    from email.mime.text import MIMEText
    if isinstance(addressees, str):
        addressees = [addressees]
    email = MIMEText(msg)
    email['Subject'] = subject
    email['From'] = whofrom
    email['To'] = ','.join(addressees)
    serv = smtplib.SMTP()
    serv.connect()
    serv.sendmail(whofrom, addressees, email.as_string())
    serv.quit()

def addAmiTags(fileList, amitag):
    fileList = [x.split()[0] for x in fileList if x != '']
    print('Appending AMI information to metadata; tag =', amitag)
    for f in fileList:
        rf = ROOT.TFile.Open(f, 'UPDATE')
        DataQualityUtils.HanMetadata.addMetadata(rf, 'AMI', {'AMI Tag': amitag})
        rf.Close()

def usage():
    cmdi = sys.argv[0].rfind("/")
    cmd = sys.argv[0][cmdi+1:]
    print("")
    print("Usage: ", cmd, "<data_file> <config> <processing_version> [run_accumulating [conditions_string]]")
    print("")
    print("This is a production utility; use TEST config for development and testing.")
    print("")
    print("Processing version is an integer, starting from 1 (not 0)")
    print("")

if __name__ == "__main__":
    if len(sys.argv) < 4 or len(sys.argv) > 7:
        usage()
        sys.exit(0)
  
    inputFile  = sys.argv[1] # data file
    runAccumulating = False
    if len(sys.argv) in (6,7):
        if sys.argv[4] == "True" or sys.argv[4] == "1":
            runAccumulating = True
  
    if len(sys.argv) == 7:
        ROOT.gSystem.Load('libDataQualityInterfaces')
        ROOT.dqi.ConditionsSingleton.getInstance().setCondition(sys.argv[5])

    configModule = ""
  
    if   sys.argv[2] == "TEST":
        configModule = "TestDisplay"
    elif sys.argv[2] == "RTT":
        configModule = "RTTDisplay"
    elif sys.argv[2] == "TCT":
        configModule = "TCTDisplay"
    elif sys.argv[2] == "FDR1":
        configModule = "fdr08_run1"
    elif sys.argv[2] == "FDR2" or sys.argv[2] == "FDR2a" or sys.argv[2] == "FDR2b" or sys.argv[2] == "FDR2c":
        configModule = "fdr08_run2"
    elif sys.argv[2] == "Cosmics08":
        configModule = "data08_cos"
    elif sys.argv[2] == "SingleBeam08":
        configModule = "data08_1beam"
    else:
        configModule = sys.argv[2]
  
    try:
        cmod = importConfiguration(configModule)
    except Exception:
        print("Could not import configuration module \'" + configModule + "\'")
        sys.exit(1)

    try:
        config = cmod.dqconfig
    except Exception:
        print("Configuration object 'dqconfig' not defined in module \'" + configModule + "\'")
    sys.exit(1)

  
    DQWebDisplay( inputFile, runAccumulating, config )

