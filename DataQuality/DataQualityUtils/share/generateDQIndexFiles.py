#!/bin/env python

## *****************************************************************************
VERSION = '$Id: generateDQIndexFiles.py,v 1.4 2009-03-31 15:49:37 ponyisi Exp $'
## *****************************************************************************

import os
import sys


def generateDQIndexFiles( installPath, project, indexFile, runListFile, htmlAddress ):
  
  print ""
  print "!! generateDQIndexFiles !!"
  print "CVS version:"
  print VERSION
  
  origdir = os.getcwd()
  os.chdir( installPath )
  
  runDict, tmpDirList = buildRunDictionary()
  
  #printRunDictionary( runDict )
  deleteTmpDirectories( runDict, tmpDirList )
  writeIndexFile( runDict, indexFile, project )
  writeRunListXML( runDict, runListFile, htmlAddress )
  
  os.chdir( origdir )
  print ""


def buildRunDictionary():
  
  ## runDict is a dictionary of (run --> procDict)
  ## procDict is a dictionary of (proc --> streamList)
  
  runDict = { }
  allTmpDirList = [ ]
  tmpDirList = [ ]
  
  for root, dirs, files in os.walk("."):
    removeDir = []
    if "history_plot" in dirs:
      dirs.remove("history_plot")
    for name in dirs:
      if( name.find('run_') > -1 ):
        removeDir.append(name)
    if len(removeDir) > 0:
      stream = root.replace("./","")
      proc = 0
      proci = stream.find("/")
      if proci != -1:
        try:
          proc = int(stream[:proci])
        except ValueError:
          proc = 0
      if proc != 0:
        for rundir in dirs:
          run = rundir.replace("run_","")
          procDict = { }
          streamList = [ ]
          if run in runDict:
            procDict = runDict[run]
          if proc in procDict.keys():
            streamList = procDict[proc]
          procText = str(proc) + "/"
          shortStream = stream.replace(procText,"")
          if shortStream.find("tmp_") == 0:
            allTmpDirList.append( (stream,run,proc) )
          streamList.append(stream)
          procDict[proc] = streamList
          runDict[run] = procDict
    for name in removeDir:
      dirs.remove(name)
  
  for tmpStream,run,proc in allTmpDirList:
    stream = tmpStream.replace("tmp_","")
    procDict = runDict[run]
    streamList = procDict[proc]
    if stream in streamList:
      tmpDir = tmpStream + "/run_" + run
      tmpDirList.append(tmpDir)
      streamList.remove(tmpStream)
  
  return runDict, tmpDirList


def deleteTmpDirectories( runDict, tmpDirList ):
  for tmpDir in tmpDirList:    
    cmd = "rm -rf %s" % tmpDir
    print cmd
    os.system( cmd )


def printRunDictionary( runDict ):
  reversedRunKeys = [(int(x), x) for x in runDict.keys()]
  reversedRunKeys.sort()
  reversedRunKeys.reverse()
  reversedRunKeys = [x[1] for x in reversedRunKeys]; del x
  for run in reversedRunKeys:
    margin = run + ": "
    procDict = runDict[run]
    sortedProcDictKeys = procDict.keys()
    sortedProcDictKeys.sort()
    for proc in sortedProcDictKeys:
      streamList = procDict[proc]
      sortedStreamList = [ ]
      for stream in streamList:
        if stream.find("Express") != -1:
          sortedStreamList.append(stream)
          streamList.remove(stream)
          break
      tmpSortedList = streamList
      tmpSortedList.sort()
      for stream in tmpSortedList:
        sortedStreamList.append(stream)
      print margin, sortedStreamList
      margin = "       "


def writeIndexFile( runDict, indexFile, project ):
  import xmlrpclib, time
  s = xmlrpclib.ServerProxy('http://atlasdqm.cern.ch:8080')
  print "Writing HTML index file..."
  f = open(indexFile,'w')
  f.write('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">\n')
  f.write('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">\n')
  f.write('<head>\n')
  f.write('<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />\n')
  f.write('<title>ATLAS Data-Quality Monitoring: '+project+'</title>\n')
  f.write('<link rel="stylesheet" href="atlasdqm.css" type="text/css" />\n')
  f.write('<script type="text/javascript" src="AllDirs.js"><!-- dont contract--></script>')
  f.write('</head>\n')
  f.write('<body>\n\n')
  f.write('<h1>'+project+' Monitoring</h1>\n\n')
  f.write('<p><font class="Note">\n')
  f.write('***Indicates reconstruction is in progress; histograms represent accumulated statistics and are temporary.\n')
  f.write('</font></p>\n\n')
  f.write('<table class="Catalogue">\n')
  f.write('<tr><td><b>Run Number</b></td><td width="20">&nbsp;</td>\n')
  f.write('    <td><b>T0 Iteration</b></td><td width="20">&nbsp;</td>\n')
  f.write('    <td><b>Streams</b></td></tr>\n')
  f.write('\n')
  
  bgcolor = '#bbffff'
  writeBGColor = False
  reversedRunDictKeys = [(int(x), x) for x in runDict.keys()]
  reversedRunDictKeys.sort()
  reversedRunDictKeys.reverse()
  reversedRunDictKeys = [x[1] for x in reversedRunDictKeys]; del x
  for run in reversedRunDictKeys:
    mouseovertext = ''
    try:
      runinfo = s.get_run_information({'low_run': run, 'high_run': run}).get(run, None)
      if runinfo != None:
        mouseovertext = 'Run %s<br/>Run Start: %s<br/>' % (run, time.strftime('%Y-%m-%d %H:%M %Z', time.localtime(runinfo[4])))
        if runinfo[5] != 0:
          mouseovertext += ('Run End: %s<br/>' % time.strftime('%Y-%m-%d %H:%M %Z', time.localtime(runinfo[5])))
        if runinfo[3] != -1:
          mouseovertext += ('Event Filter Events: %s<br/>' % runinfo[3])
        mouseovertext += ('Project Tag: %s<br/>' % runinfo[1])
        if runinfo[2] != '':
          mouseovertext += ('Partition: %s<br/>' % runinfo[2])
    except Exception, e:
        print e
    procDict = runDict[run]
    nproc = len(procDict.keys())
    margin =  ' <td rowspan="' + str(nproc) + '" valign="top" align="right">'
    margin += '<a href="http://atlas-service-db-runlist.web.cern.ch/atlas-service-db-runlist/cgi-bin/runDetails.py?run=' + str(run) + """" onMouseover="showhint('""" + mouseovertext + """', this, event, '400px')">"""
    margin += str(run) + '</a></td>\n'
    margin += ' <td rowspan="' + str(nproc) + '"></td>\n'
    sortedProcKeys = procDict.keys()
    sortedProcKeys.sort()
    for proc in sortedProcKeys:
      if(writeBGColor):
        f.write('<tr bgcolor="' + bgcolor + '">\n')
      else:
        f.write('<tr>\n')
      f.write(margin)
      f.write(' <td align="right">' + str(proc) + '</td>\n')
      f.write(' <td></td>\n')
      f.write(' <td>\n')
      
      endtext = ""
      
      streamList = procDict[proc]
      sortedStreamList = [ ]
      for stream in streamList:
        if stream.find("Express") != -1:
          sortedStreamList.append(stream)
          streamList.remove(stream)
          break
      tmpSortedList = streamList
      streamList.sort()
      for stream in streamList:
        sortedStreamList.append(stream)
      
      for stream in sortedStreamList:
        link = stream + '/run_' + run
        proci = stream.rfind("/")
        shortStream = stream[proci+1:]
        if shortStream.find("tmp_") == 0:
          shortStream = shortStream.replace("tmp_","")
          shortStream += "***"
        f.write(endtext)
        f.write('  [<a href="' + link + '">' + shortStream + '</a>]')
        endtext = '&nbsp;&nbsp;\n'
      
      f.write('\n </td>\n')
      f.write('</tr>\n')
      margin = ""
    writeBGColor = not writeBGColor
  
  f.write('</table>\n')
  f.write('</body>\n')
  f.write('</html>\n')
  f.close()


def writeRunListXML( runDict, runListFile, htmlAddress ):
  print "Writing run list XML file..."
  
  f = open(runListFile,'w')
  f.write('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
  f.write('<DqFileCatalogue>\n')
  
  streamDict = {}
  
  for root, dirs, files in os.walk("."):
    removeDir = []
    for name in dirs:
      if( name.find('run_') > -1 ):
        removeDir.append(name)
    if len(removeDir) > 0:
      stream = root.replace("./","")
      runList = []
      proc = 0
      proci = stream.find("/")
      if proci != -1:
        try:
          proc = int(stream[:proci])
        except ValueError:
          proc = 0
      if proc != 0:
        #f.write('<Stream id=\'' + stream + '\'>\n')
        for rundir in dirs:
          run = rundir.replace("run_","")
          runList.append(run)
          #address = htmlAddress + "/" + stream + "/run_" + run
          #f.write('<Run runNumber=\'' + run + '\'>\n')
          #f.write('<html>' + address + '</html>\n')
          #f.write('</Run>\n')
        #f.write('</Stream>\n')
        streamDict[stream] = runList
          
    for name in removeDir:
      dirs.remove(name)
  
  streamList = streamDict.keys()
  sortedStreamList = []
  for stream in streamList:
    if stream.find("Express") != -1:
      sortedStreamList.append(stream)
      streamList.remove(stream)
      break
  tmpSortedList = streamList
  tmpSortedList.sort()
  for stream in tmpSortedList:
    sortedStreamList.append(stream)
  
  for stream in sortedStreamList:
    f.write('<Stream id=\'' + stream + '\'>\n')
    runList = streamDict[stream]
    reversedRunList = runList
    reversedRunList.sort()
    reversedRunList.reverse()
    for run in reversedRunList:
      address = htmlAddress + "/" + stream + "/run_" + run
      f.write('<Run runNumber=\'' + run + '\'>\n')
      f.write('<html>' + address + '</html>\n')
      f.write('</Run>\n')
    f.write('</Stream>\n')
  
  f.write('</DqFileCatalogue>\n')


def usage():
  cmdi = sys.argv[0].rfind("/")
  cmd = sys.argv[0][cmdi+1:]
  print ""
  print "Usage: ", cmd, "<installPath> <project> <indexFile> <runListFile> <htmlAddress>"
  print ""


if __name__ == "__main__":
  
  if len(sys.argv) != 6:
    usage()
    sys.exit(0)
  
  installPath = sys.argv[1]
  project     = sys.argv[2]
  indexFile   = sys.argv[3]
  runListFile = sys.argv[4]
  htmlAddress = sys.argv[5]
  
  generateDQIndexFiles( installPath, project, indexFile, runListFile, htmlAddress )

