# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## *****************************************************************************
MODVERSION = '$Id: handimod.py,v 1.5 2009-03-16 19:48:32 boelaert Exp $'
## *****************************************************************************

import os
## Needed to correct ROOT behavior; see below
CWD = os.getcwd()

import sys
import time
import ROOT
## Importing gSystem may change the current directory to one of the
## command-line arguments; chdir to original directory to have
## predictable behavior
from ROOT import gSystem
os.chdir(CWD)

## Import the ROOT library for reading han results
gSystem.Load('libDataQualityUtils')
from ROOT import dqutils

## LumiBlock length (in minutes)
LBlength = 1.0

def handiWithComparisons( name, resultsFile, htmlDir, runlistLoc, compare, browserMenu, allDirsScriptDir ):
  ## compare: True if you want a "compare" button on every 1histo page, False by default
  ## javaScriptLoc = url of the javascript for the "compare" button
  ## HjavaScriptLoc = url of the javascript for the "history" button
  ## runlistLoc = url where to find runlist.xml (runlist catalog)
  ## browserMenu = True if you want a browser menu instead of the 
  ## allDirsScript = url of javascript to create browser menu
  
  if ( htmlDir.rfind("/")!=(len(htmlDir)-1) ):  # htmlDir needs "/" at the end
        htmlDir+="/"
  
  dirsstring = stringListSystemPaths( resultsFile, htmlDir )
  
  dirs = dirsstring.rsplit()
  for subHtmlDir in dirs:
    if( not os.access(subHtmlDir,os.F_OK) ):
      try:
        os.makedirs(subHtmlDir)
      except os.error:
        print 'Cannot create directory "' + subHtmlDir + '"; exiting.'
        sys.exit(-1)
  
  total = stringAllDQAssessments( resultsFile )

  LB_range = ''
  if (name.find('minutes10_')>-1):
    t=name.split('10_')
    digit = ((t[len(t)-1]).split(','))[0]
    digit = float(digit)
    low_limit = int((digit-1.0)*10.0/LBlength+1)
    hi_limit = int(digit*10.0/LBlength)
    LB_range = ', LB '+str(low_limit)+' - ' + str(hi_limit)
  elif (name.find('minutes30_')>-1):
    t=name.split('30_')
    digit = float( ((t[len(t)-1]).split(','))[0])
    low_limit = int((digit-1.0)*30.0/LBlength+1)
    hi_limit = int(digit*30.0/LBlength)
    LB_range = ', LB '+str(low_limit)+' - ' + str(hi_limit)

  nSaved = saveAllHistograms( resultsFile, htmlDir, True, (name+LB_range) )
  if nSaved == 0:
    print "There are no histograms in this file; writing a dummy index file"
    if( not os.access(htmlDir,os.F_OK) ):
      try:
        os.makedirs(htmlDir)
      except os.error:
        print 'Cannot create directory "' + htmlDir + '"; exiting.'
        sys.exit(-1)
    dummyIndex = htmlDir + "/index.html"
    d = open(dummyIndex,'w')
    d.write('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">\n')
    d.write('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">\n')
    d.write('<head>\n')
    d.write('<title>Error</title>\n')
    d.write('</head>\n')
    d.write('<body>\n')
    d.write('<h1>Error:</h1>\n')
    d.write('No histograms found for display.  Check the <tt>han</tt> configuration\n')
    d.write('to make sure it is consistent with the <tt>han</tt> input file.\n')
    d.write('</body>\n')
    d.write('</html>\n')
    d.close()
    return
  
  s=total.rsplit('\n')
  # number = number of lines in total
  number=len(s)
  if (len(s[number-1])<1): # last line is empty
    number-=1

  if (browserMenu):
    makeAllDirsXml( htmlDir, name, s, number, resultsFile)
    list, namelist = makeAllDirsBrowserFile( htmlDir, name, s, number, resultsFile,allDirsScriptDir )
  else:
    list, namelist = makeAllDirsFile( htmlDir, name, s, number, resultsFile )
    
  for x in range(0,len(list)):
    makeSubDirFile( htmlDir, name, s, number, namelist[x], list[x], runlistLoc, compare, allDirsScriptDir )
    makeColorFile( htmlDir, name, s, number, namelist[x], list[x], 'Red', runlistLoc, compare, allDirsScriptDir)
    makeColorFile( htmlDir, name, s, number, namelist[x], list[x], 'Yellow', runlistLoc, compare, allDirsScriptDir)
    makeColorFile( htmlDir, name, s, number, namelist[x], list[x], 'Green', runlistLoc, compare, allDirsScriptDir )
    makeCSSFile( htmlDir,"", namelist[x] )
  
  makeCSSFile( htmlDir,"", "." )


def makeAllDirsXml( htmlDir, name, s, number, resultsFile ):
  g=open(htmlDir+'AllDirs.xml','w')
  g.write('<?xml version="1.0" encoding="ISO-8859-1"?>\n')  
  # initial number of white spaces, will change to positive value once we go over the lines
  spaces=-1
  num_lists=0
  sub="";
  for x in range(0,number):
    sp=s[x].rsplit()
    if sp[3]=='dir':  # a directory
      namedir=sp[0]
      shortNameDir = namedir
      namediri = namedir.rfind("/")
      if( namediri != -1 ):
        shortNameDir = namedir[namediri+1:]
      if namedir=='<top_level>':
        shortNameDir='Overall Status'
      spaces_new=s[x].find(sp[0][0])
      if ( (x<number-1) and (s[x+1].rsplit())[3]=='ass' ):
        histo ="yes"
      else:
        histo = "no"
      if spaces_new > spaces: # current dir is subdir of previous dir -> new item in new list
        if (spaces !=-1):
          sub +="sub"
        g.write('<dir tag=\"'+sub+'dir\" id=\"' +shortNameDir + '\" status=\"'+sp[1]+'\" histo = \"' + histo + '\">\n')
        num_lists+=1
      elif spaces_new==spaces: # current en previous dir in same motherdir -> new item
        g.write('</dir>\n<dir tag=\"'+sub+'dir\" id=\"' +shortNameDir + '\" status=\"'+sp[1] +'\" histo = \"' + histo + '\">\n')
      else: # close list and open new one
        diff = spaces - spaces_new
        while diff > 0:
          g.write('</dir>\n')
          diff -= 2
          num_lists-=1
          sub=sub[3:len(sub)]
        g.write('</dir>\n')
        g.write('<dir tag=\"'+sub+'dir\" id=\"' +shortNameDir + '\" status=\"'+sp[1] +'\" histo = \"' + histo + '\">\n')
      if ( histo=='yes' ): # check that dir contains histos
        if namedir=='<top_level>':
          namedir = '.'
          g.write('<a href="'+namedir +'/toplevel.html">'+shortNameDir+ '</a>\n')
        else:
          g.write('<a href="'+namedir +'/index.html" >'+shortNameDir+ '</a>\n')
     # g.write('<'+sp[1]+'></'+sp[1]+'>\n')    
      spaces=spaces_new
  for x in range(0,num_lists):
    g.write('</dir>\n')
    sub=sub[3:len(sub)]
  g.close()

def makeAllDirsFile( htmlDir, name, s, number, resultsFile ):
  g=open(htmlDir+'index.html','w')
  g.write('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">\n')
  g.write('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">\n')
  g.write('<head>\n')
  g.write('<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />\n')
  g.write('<title>'+ name+  '</title>\n')
  g.write('<link rel="stylesheet" href="AutomChecks.css" type="text/css" />\n')
  g.write('<link rel="stylesheet" href="https://atlasdqm.web.cern.ch/atlasdqm/css/mktree.css"/>\n')
  g.write('<script type="text/javascript" src="https://atlasdqm.web.cern.ch/atlasdqm/js/mktree.js"></script>\n')
  g.write('</head>\n')
#  g.write('<body onload="CollapsibleLists.apply()">')
  g.write('<body>')
  g.write('<font class="DQGroup">[<a href="#" onclick="history.go(-1);return false;">Back</a>]</font>')
#  g.write('<font class="DQGroup">[<a href="../index.html">Back</a>]</font>\n')
  g.write('<h1>' + name + ': Monitoring and Automatic Checks</h1>\n')

  # initial number of white spaces, will change to positive value once we go over the lines
  spaces=-1
  # list = list with directories (the line number) that contain histos
  list=[]
  # namelist = list with corresponding direcotory names
  namelist=[]

  num_lists=0

  g.write('<table>\n')
  g.write('<tr><td>Click to expand</td></tr>\n')
  g.write('<tr valign="top">\n<td width="500"><font class="DQGroup">\n')
  for x in range(0,number):
    sp=s[x].rsplit()
    if sp[3]=='dir':  # a directory
      namedir=sp[0]
      shortNameDir = namedir
      namediri = namedir.rfind("/")
      if( namediri != -1 ):
        shortNameDir = namedir[namediri+1:]
      if namedir=='<top_level>':
        shortNameDir='Overall Status'
      spaces_new=s[x].find(sp[0][0])
      if spaces_new > spaces: # current dir is subdir of previous dir -> new item in new list
#        g.write('<ul%s>\n<li>' % (' class="collapsibleList"' if namedir=='<top_level>' else ''))
        g.write('<ul%s>\n<li>' % (' class="mktree"' if namedir=='<top_level>' else ''))
        num_lists+=1
      elif spaces_new==spaces: # current en previous dir in same motherdir -> new item
        g.write('</li>\n<li>')
      else: # close list and open new one
        g.write('</li>')
        diff = spaces - spaces_new
        while diff > 0:
          g.write('</ul></li>\n')
          diff -= 2
          num_lists-=1
        g.write('<li>')
#      if namedir!='<top_level>':
#        g.write('<img src="http://atlasdqm.web.cern.ch/atlasdqm/img/pixel.png" width="0" height="13" alt="" />')
      if ( (x<number-1) and (s[x+1].rsplit())[3]=='ass' ): # check that dir contains histos
        if namedir=='<top_level>':
          namedir = '.'
          g.write('<a href="'+namedir +'/toplevel.html">'+shortNameDir+ ':</a>')
        else:
          g.write('<a href="'+namedir +'/index.html" >'+shortNameDir+ ':</a>')
        g.write('&nbsp;&nbsp;&nbsp;<font class="'+ sp[1]+'">'+ sp[1] + '</font>\n')
        list.append(x)
        namelist.append(namedir)
      else:
        g.write(shortNameDir+ ':')
        g.write('&nbsp;&nbsp;&nbsp;<font class="'+ sp[1]+'">'+ sp[1] + '</font>\n')
      spaces=spaces_new
  if num_lists>0:
    g.write('</li>')
  for x in range(0,num_lists-1):
    g.write('</ul></li>\n')
  if num_lists>0:
     g.write('</ul>\n')
  #g.write('</font></td>\n<td><font class="Info">From file:</font><br/><font class="Note">' + resultsFile + '</font></td>\n</tr>\n</table>')
  g.write('</font></td>\n</tr>\n</table>')
  g.write('</body>\n</html>\n')
  g.close()
  return list, namelist


def makeAllDirsBrowserFile( htmlDir, name, s, number, resultsFile,AllDirsScriptDir ):
  g=open(htmlDir+'index.html','w')
  g.write('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">\n')
  g.write('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">\n')
  g.write('<head>\n')
  g.write('<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />\n')
  g.write('<title>'+ name+  '</title>\n')
  g.write('<link rel="stylesheet" href="AutomChecks.css" type="text/css" />\n')
  g.write('<link rel="stylesheet" href="'+AllDirsScriptDir+'/mktree/mktree.css" type="text/css" />')
  g.write('<script type="text/javascript" src="'+AllDirsScriptDir+'/AllDirs.js"><!-- dont contract--></script>\n')
  g.write('<script type="text/javascript" src="'+AllDirsScriptDir+'/mktree/mktree.js"><!-- dont contract--></script>\n')
  g.write('</head>\n')
  g.write('<body onLoad=\"loadXMLDoc(\'AllDirs.xml\');\">')
  g.write('<font class="DQGroup">[<a href="#" onclick="history.go(-1);return false;">Back</a>]</font>')
  g.write('<h1>' + name + ': Monitoring and Automatic Checks</h1>\n')
  #g.write('<p><font class="Info">From file:</font><br/><font class="Note">' + resultsFile + '</font></p>\n')
  g.write('<div id=\"Select_subdir\">\n</div>\n')
  #g.write('<table>\n<tr>\n')
  #g.write('<td valign="top"> &nbsp&nbsp &nbsp &nbsp&nbsp<input type="button" onclick="displaySubdirs();"  value="... or display directories at current level" />\n')
  #g.write('</td>\n</tr>\n</table>\n
  #g.write('<br />\n')
  g.write('<div id=\"Display_dir\">\n</div>\n')
  g.write('</body>\n</html>\n')
  g.close()
  
  # list = list with directories (the line number) that contain histos
  list=[]
  # namelist = list with corresponding direcotory names
  namelist=[]

  num_lists=0

  for x in range(0,number):
    sp=s[x].rsplit()
    if sp[3]=='dir':  # a directory
      namedir=sp[0]
      if ( (x<number-1) and (s[x+1].rsplit())[3]=='ass' ): # check that dir contains histos
        if namedir=='<top_level>':
          namedir = '.'
        list.append(x)
        namelist.append(namedir)
  g.close()
  return list, namelist

def makeSubDirFile( htmlDir, name, s, number, subname, assessIndex, runlistLoc,compare, AllDirsScriptDir):
  
  if( subname == '.' ):
    h=open(htmlDir+'/'+subname+'/toplevel.html','w')
    subnameTitle = 'Top Level'
  else:
    h=open(htmlDir+'/'+subname+'/index.html','w')
    subnameTitle = subname
  h.write('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">\n')
  h.write('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">\n')
  h.write('<head>\n')
  h.write('<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />\n')
  h.write('<title>'+ name + ' ' + subnameTitle + '</title>\n')
  h.write('<link rel="stylesheet" href="AutomChecks.css" type="text/css" />\n')
  h.write('<script type="text/javascript" src="'+AllDirsScriptDir+'/AllDirs.js"><!-- dont contract--></script>\n')
  h.write('</head>\n')
  h.write('<body>\n')
  h.write('<font class="DQGroup">[<a href="#" onclick="history.go(-1);return false;">Back</a>]</font>')
#  h.write('<font class="DQGroup">[<a href="')
#  for x in range(subname.count("/")):
#    h.write('../')
#  h.write('../index.html">Back</a>]</font>\n')
  h.write('<center>')
  h.write('<table>\n<tr valign="top">\n<td width="250"></td>\n')
  h.write('<td width="300" align="center"><h2>'+name+' '+subnameTitle+'</h2>\n')
  h.write('<a href="Red.html">[Only Red]</a>\n')
  h.write('<a href="Yellow.html">[Only Yellow]</a>\n')
  h.write('<a href="Green.html">[Only Green]</a></td>\n')
  h.write('<td width="250">\n<font class="Note">Click on images for details and full size.</font>\n')
  h.write('</td>\n</tr>\n</table>\n')
  h.write('<table cellpadding="4" cellspacing="20">\n')    
  y=assessIndex+1
  sp=s[y].rsplit()
  col=0
  while(sp[3]=='ass' and y<number):
    sp=s[y].rsplit()
    titleStream = s[y].rsplit(" title ")
    title = titleStream[1]
    col+=1
    if col==1:
      h.write('<tr>\n<td class="' + sp[1] + '" align="center"><a href="'+sp[0]+'.html" class="hintanchor" onmouseover="showhint(\'' +title+'\', this, event, \'400px\')"><img src="'+ sp[0] +'.png" height="200"  alt="' + name + ' ' + subname+'/'+sp[0]+'.png" /></a><br/><div style="text-overflow:ellipsis;overflow:hidden;max-width:240px">'+sp[0]+'</div></td>\n')
    elif col==3:
      h.write('<td class="' + sp[1] + '" align="center"><a href="'+sp[0]+'.html" class="hintanchor" onmouseover="showhint(\'' +title+'\', this, event, \'500px\')"><img src="'+ sp[0] +'.png" height="200"  alt="' + name + ' ' + subname+'/'+sp[0]+'.png" /></a><br/><div style="text-overflow:ellipsis;overflow:hidden;max-width:240px">'+sp[0]+'</div></td>\n</tr>\n')
      col=0
    else:
      h.write('<td class="' + sp[1] + '" align="center"><a href="'+sp[0]+'.html" class="hintanchor" onmouseover="showhint(\'' +title+'\', this, event, \'400px\')"><img src="'+ sp[0] +'.png" height="200" alt="' + name + ' ' + subname+'/'+sp[0]+'.png" /></a><br/><div style="text-overflow:ellipsis;overflow:hidden;max-width:240px">'+sp[0]+'</div></td>\n')
    temp = s[y].rsplit(" title ")
    sp = temp[0].split()
    makeOneHistFile( htmlDir, name, subname, sp, runlistLoc,compare )
    y=y+1
    if y< number:
      sp=s[y].rsplit()
  if not (col==3):
    h.write('</tr>\n')
  h.write('</table>\n</center>\n</body>\n</html>\n')
  h.close()


def makeColorFile( htmlDir, name, s, number, subname, assessIndex , color, runlistLoc,compare,AllDirsScriptDir):
  
  if( subname == '.' ):
    h=open(htmlDir+'/'+subname+'/'+color+'.html','w')
    subnameTitle = 'Top Level'
  else:
    h=open(htmlDir+'/'+subname+'/'+color+'.html','w')
    subnameTitle = subname
  h.write('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">\n')
  h.write('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">\n')
  h.write('<head>\n')
  h.write('<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />\n')
  h.write('<title>'+ name + ' ' + subnameTitle + ' ' + color + '</title>\n')
  h.write('<link rel="stylesheet" href="AutomChecks.css" type="text/css" />\n')
  h.write('<script type="text/javascript" src="'+AllDirsScriptDir+'/AllDirs.js"><!-- dont contract--></script>\n')
  h.write('</head>\n')
  h.write('<body>\n')
  h.write('<font class="DQGroup">[<a href="#" onclick="history.go(-1);return false;">Back</a>]</font>')
  h.write('<center>')
  h.write('<table>\n<tr valign="top">\n<td width="250"></td>\n')
  h.write('<td width="300"><h2>'+name+' '+subnameTitle+': ' + color + ' only</h2>\n')
  h.write('<a href="Red.html">[Only Red]</a>\n')
  h.write('<a href="Yellow.html">[Only Yellow]</a>\n')
  h.write('<a href="Green.html">[Only Green]</a></td>\n')
  h.write('<td width="250">\n<font class="Note">Click on images for details and full size.</font>\n')
  h.write('</td>\n</tr>\n</table>\n')
  h.write('<table cellpadding="4" cellspacing="20">\n')    
  y=assessIndex+1
  sp=s[y].rsplit()
  col=0
  count=0
  while(sp[3]=='ass' and y<number):
    titleStream = s[y].rsplit(" title ")
    title = titleStream[1]
    sp=s[y].rsplit()
    if sp[1]==color:
      count = count +1
      col+=1
      if col==1:
        h.write('<tr>\n<td class="' + sp[1] + '"><a href="'+sp[0]+'.html" class="hintanchor" onmouseover="showhint(\''+title+'\', this, event, \'400px\')"><img src="'+ sp[0] +'.png" height="200"  alt="' + name + ' ' + subname+'/'+sp[0]+'.png" /></a></td>\n')
      elif col==3:
        h.write('<td class="' + sp[1] + '"><a href="'+sp[0]+'.html" class="hintanchor" onmouseover="showhint(\''+title+'\', this, event, \'500px\')"><img src="'+ sp[0] +'.png" height="200"  alt="' + name + ' ' + subname+'/'+sp[0]+'.png" /></a></td>\n</tr>\n')
        col=0
      else:
        h.write('<td class="' + sp[1] + '"><a href="'+sp[0]+'.html" class="hintanchor" onmouseover="showhint(\'' +title+'\', this, event, \'400px\')"><img src="'+ sp[0] +'.png" height="200" alt="' + name + ' ' + subname+'/'+sp[0]+'.png" /></a></td>\n')
      temp = s[y].rsplit(" title ")
      sp = temp[0]  
      makeOneHistFile( htmlDir, name, subname, sp, runlistLoc,compare)
    y=y+1
    if y< number:
      sp=s[y].rsplit()
  if not (col==3):
    h.write('</tr>\n')
  h.write('</table>\n')
  if count==0:
    h.write('<h3> No '+color+' histograms </h3>\n')
  h.write('</center>\n</body>\n</html>\n')
  h.close()


def writeLimitDiagram( k, limitName, lowColor, hiColor, loVal, hiVal ):
  k.write('<tr><td>&nbsp;</td></tr>\n')
  k.write('<tr><td colspan="2">\n')
  k.write('<table>\n')
  k.write('<caption>'+limitName+'</caption>\n')
  k.write('<tr>\n')
  k.write('<td width="340" align="center" colspan="4">\n')
  k.write('<font color="'+lowColor+'">XXXXXXX</font><b>I</b>\n')
  k.write('<font color="#ffd700">XXXXXXX</font><b>I</b>\n')
  k.write('<font color="'+hiColor+'">XXXXXXX</font></td>\n')
  k.write('</tr><tr>\n')
  k.write('<td width="92"></td>\n')
  k.write('<td width="78" align="center">'+ loVal +'</td>\n')
  k.write('<td width="78" align="center">'+ hiVal +'</td>\n')
  k.write('<td width="92"></td>\n')
  k.write('</tr>\n</table>\n')


def makeOneHistFile( htmlDir, name, subname, sp, runlistLoc, compare ):
  import re
  runmatch = re.compile('^Run ([0-9]+), ([0-9]+)/(.+)$')
  subrunmatch = re.compile('^Run ([0-9]+), (.+)_(.*), ([0-9]+)/(.+)$')
  rm = runmatch.match(name); srm = subrunmatch.match(name)
  if rm != None:
    run, proc_ver, stream = rm.groups()
    period_type = 'run'; period = '1'
  elif srm != None:
    run, period_type, period, proc_ver, stream = srm.groups()
  else:
    run, period_type, period, proc_ver, stream = [None]*5
  
  k=open(htmlDir+'/'+subname+'/'+sp[0]+'.html','w')
  k.write('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">\n')
  k.write('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">\n')
  k.write('<head>\n')
  k.write('<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />\n')
  k.write('<title>'+name+ ' ' + subname+ ' ' + sp[0]+'</title>\n')
  k.write('<link rel="stylesheet" href="AutomChecks.css" type="text/css" />\n')
#   k.write('<script type=\"text/javascript\" src=\"'+javaScriptLoc +'\"><!-- dont contract-->\n</script>\n')
  k.write('</head>\n')
  k.write('<body>\n')
  k.write('<center>\n')
  k.write('<table>\n<tr valign="top">\n')
  k.write('<td width="250"><font class="DQGroup">[<a href="#" onclick="history.go(-1);return false;">Back</a>]</font></td>\n')
  k.write('<td width="500"><h2>'+name + ' ' + subname+'/'+sp[0]+'</h2></td>\n')
  k.write('<td width="250"></td>\n</tr>\n')
  for x in range(4,len(sp)-1):
    if ('inputname' in sp[x]):
      inputname = sp[x+1]
      k.write('<tr><td width="250"></td><td width="500">Location in ROOT file: ' + inputname + '</td>\n</tr>\n')
  k.write('</table>\n')
  k.write('<table cellpadding="10">\n<tr>\n')
  k.write('<td>\n<table width="400">\n')
  k.write('<tr><td colspan="2"><font class="Heading">Assessment Details:</font></td></tr>\n')
  k.write('<tr><td>&nbsp;</td></tr>\n')
  k.write('<tr><td align="right"><font class="DatumName">Name:</font></td>')
  k.write('<td>' +sp[0] +'</td></tr>\n' )
  k.write('<tr><td align="right"><font class="DatumName">Status:</font></td>') 
  k.write('<td><font class="'+ sp[1] +'">'+ sp[1] + '</font></td></tr>\n')
  k.write('<tr><td align="right"><font class="DatumName">Algorithm:</font></td>')
  k.write('<td>'+sp[2]+'</td></tr>\n')
  algorithm = sp[2]
  extra=len(sp)-4
  cc=4
  cG=-1
  cR=-1
  currentHeading = ''
  namecache = []
  while(extra>0):
    if 'Green' in sp[cc]:
      currentHeading = 'limits'
      spi=sp[cc].find("Green")
      limitName=sp[cc][:spi]
      Green=float(sp[cc+1])
      cG=cc+1
      cc+=2
      extra-=2
      if (cG>0 and cR>0):
        if(Green>Red):
          writeLimitDiagram( k, limitName, "Red", "Green", sp[cR], sp[cG] )
        else:
          writeLimitDiagram( k, limitName, "Green", "Red", sp[cG], sp[cR] )
        cG=-1
        cR=-1
    elif 'Red' in sp[cc]:
      currentHeading = 'limits'
      spi = sp[cc].find("Red")
      limitName=sp[cc][:spi]
      Red=float(sp[cc+1])
      cR=cc+1
      cc+=2
      extra-=2
      if (cG>0 and cR>0):
        if(Green>Red):
          writeLimitDiagram( k, limitName, "Red", "Green", sp[cR], sp[cG] )
        else:
          writeLimitDiagram( k, limitName, "Green", "Red", sp[cG], sp[cR] )
        cG=-1
        cR=-1
    elif sp[cc]=="Results":
         currentHeading = 'results'
         k.write('<tr><td>&nbsp;</td></tr>\n')
         k.write('<tr><td colspan="2"><font class="Heading">Results:</font></td></tr>\n')
         k.write('<tr><td>&nbsp;</td></tr>\n')
         cc+=2
         extra-=2
    elif sp[cc]=="Config":
         currentHeading = 'config'
         k.write('<tr><td>&nbsp;</td></tr>\n')
         k.write('<tr><td colspan="2"><font class="Heading">Configuration Parameters:</font></td></tr>\n')
         k.write('<tr><td>&nbsp;</td></tr>\n')
         cc+=2
         extra-=2
    elif sp[cc]=="entries:":
        currentHeading = 'nentries'
        k.write('<tr><td align="right"><font class="DatumName">Num. of Entries:</font> </td>')
        k.write('<td>'+sp[cc+1]+'</td></tr>\n')
        cc+=2
        extra-=2
    elif sp[cc]=="Underflow:":
        k.write('<tr><td align="right"><font class="DatumName">Underflow:</font> </td>')
        k.write('<td>'+sp[cc+1]+'</td></tr>\n')
        cc+=2
        extra-=2
    elif sp[cc]=="Overflow:":
        k.write('<tr><td align="right"><font class="DatumName">Overflow:</font> </td>')
        k.write('<td>'+sp[cc+1]+'</td></tr>\n')
        cc+=2
        extra-=2
    elif cc<len(sp)-1 and 'inputname' not in sp[cc]:
      if currentHeading == 'results':
        namecache.append(sp[cc])
        if ':' not in sp[cc]:
          cc+=1
          extra-=1
          continue
        else:
          name = ' '.join(namecache)
          namecache = []
        import urllib
        resultname = name.rsplit(':', 1)[0]
        resultval = sp[cc+1]
        if algorithm == 'RepeatAlgorithm' and resultname.endswith('|Status'):
          resultval = {'1': 'Red', '2': 'Yellow', '3': 'Green'}[resultval]
        if compare and run != None:
          if period_type == 'run':
            queryurl = 'http://atlasdqm.cern.ch:8080/dqmfquery/query?histogram=%s&result=%s&error=&stream=%s&period_type=%s&source=tier0&proc_ver=%s&low_run=%s&high_run=&low_y=&high_y=&outputtype=png' % (urllib.quote_plus(subname+'/'+sp[0]), urllib.quote_plus(resultname), stream.strip(), period_type, proc_ver, int(run)-1000)
            k.write('<tr><td align="right"><b><a href="%s">%s</a>:</b></td>' % (queryurl, resultname))
            k.write('<td>'+resultval+'</td></tr>\n')
          else:
            queryurl1 = 'http://atlasdqm.cern.ch:8080/dqmfquery/query?histogram=%s&result=%s&error=&stream=%s&period_type=%s&source=tier0&proc_ver=%s&low_run=%s&high_run=&low_y=&high_y=&outputtype=png' % (urllib.quote_plus(subname+'/'+sp[0]), urllib.quote_plus(resultname), stream.strip(), period_type, proc_ver, int(run)-1000)
            queryurl2 = 'http://atlasdqm.cern.ch:8080/dqmfquery/query?histogram=%s&result=%s&error=&stream=%s&period_type=%s&source=tier0&proc_ver=%s&low_run=%s&high_run=%s&low_y=&high_y=&outputtype=png' % (urllib.quote_plus(subname+'/'+sp[0]), urllib.quote_plus(resultname), stream.strip(), period_type, proc_ver, run, run)
            k.write('<tr><td align="right"><b><a href="%s">%s</a>:</b></td>' % (queryurl1, resultname))
            k.write('<td>'+resultval+' (<a href="%s">History for this run</a>)</td></tr>\n' % queryurl2)
        else:
          k.write('<tr><td align="right"><b> %s: </b></td>' % (resultname,))
          k.write('<td>'+resultval+'</td></tr>\n')
      else:
        k.write('<tr><td align="right"><b>'+sp[cc]+' </b></td>')
        k.write('<td>'+sp[cc+1]+'</td></tr>\n')
      cc+=2
      extra-=2
    else:
      cc+=2
      extra-=2
  k.write('</table>\n</td>\n')
  if subname == '.':
    k.write('<td><a href="toplevel.html"><img src="'+ sp[0] +'.png" alt="' + name + ' ' + subname+'/'+sp[0]+'.png" /></a></td>\n')
  else:
    k.write('<td><a href="index.html"><img src="'+ sp[0] +'.png" alt="' + name + ' ' + subname+'/'+sp[0]+'.png" /></a></td>\n')
  k.write('</tr></table>\n')
  k.write('</center>\n')
  now = time.localtime()
  #lastUpdate = "Last Update: "+str(now[0])+"-"+str(now[1])+"-"+str(now[2])+" "+str(now[3])+":"+str(now[4])+" UTC"
  lastUpdate = "Last Update: "+time.strftime('%Y-%m-%d %H:%M %Z', now)
  k.write('</br><font class=\"Note\">'+lastUpdate+'</font><br />\n')
##   if(compare):
##     t = htmlDir.split("/");
##     pref = "run";
##     stream = "no stream";
##     run = "no run";
##     for x in range(0,len(t)):
##       if (t[x].find("run_")!=-1):
##         run = t[x]
##         if(x>1):
##           stream = t[x-1]
##       if(t[x].find("minutes10_")!=-1):
##         pref =t[x]
##       if(t[x].find("minutes30_")!=-1):
##         pref =t[x]  
##     k.write('<div id=\"compareButton\">\n<br/>\n<hr/>\n')
##     hisname=pref+"/"+subname+ '/' + sp[0]  
##     k.write('<input type=\"button\" onclick=\"compare(\''+ runlistLoc +'\',\''+hisname +'\');\" value=\"compare\" />')
##     k.write('<br/>\n<hr/>\n</div>\n')
##     k.write('<table>\n<tr>\n<td>\n<div id=\"streamSelect\"><!-- insert menu for Streams here-->\n</div>\n</td>')
##     k.write('<td><div id=\"runSelect\"><!-- insert menu for Runs here-->\n</div>\n</td>\n</tr>\n</table>\n')
##     k.write('<div id=\"secondHisto\"><!-- insert menu for Runs here-->\n</div>\n')

## ## history button (Monica D'Onofrio, 08/21/2008)
##   histname='/'+subname+ '/' + sp[0]
##   # need to do a bit of tricks to point to the webpage
##   # will be improved in the future (another input variable to handi.py?)
##   if (htmlDir.find('./www/')>-1):
##     g = htmlDir.split('./www/')
##     htmlWeb='http://atlasdqm.cern.ch/tier0/FDR2c/'+g[1]
##   else:
##     htmlWeb='http://atlasdqm.web.cern.ch/atlasdqm/test/4/physics_Express/run' 
    
##   try:  
##     history_dir = htmlWeb.split('/run')[0]+"/history_plot"
##   except:
##     history_dir = "http://atlasdqm.web.cern.ch/atlasdqm/test/4/physics_Express/history_plot"

##   sep = histname.split('/')
##   riu = '-'.join(sep)
##   if (riu.find('-->')>-1):
##     hh = riu.split('-->')
##     gg = '-'.join(hh)
##     riu = gg           
      
##   hisname = history_dir+'/h'+str(riu)+'.gif'
##   k.write('<script type=\"text/javascript\" src=\"'+HjavaScriptLoc +'\"><!-- dont contract-->\n</script>\n')
##   k.write('<input type=\"button\" onclick=\"returnHistory(\''+hisname +'\');\" value=\"History\" />')
##   k.write('<br/>\n<hr/>\n</div>\n')
        
  k.write('</body>\n</html>\n')
  k.close()


def makeRootFile( htmlDir, name, subname ):
  k=open(htmlDir+'index.html','w')
  k.write('<html>\n<frameset rows="200,*">\n') 
  k.write('<frame src="'+name+'AllDirs.html">\n')
  if subname != "":
    if subname != '.':
      k.write('<frame src="'+subname+'/index.html" name="showframe"> \n')
    else:
      k.write('<frame src="'+subname+'/toplevel.html" name="showframe"> \n')
  k.write('</frameset>\n</html> \n')
  k.close()


def makeCSSFile( htmlDir, name, subname ):
  css=open(htmlDir+'/'+subname+'/'+name+'AutomChecks.css','w')
  css.write('BODY\n{\n  background: #E6E6FA;\n  color: #000000;\n  font-family: helvetica,sans-serif;\n}\n')
  css.write('H1\n{\n  font-family: helvetica,sans-serif;\n  font-size: x-large;\n  text-align: left;\n}\n')
  css.write('H2\n{\n  font-family: helvetica,sans-serif;\n  font-size: large;\n  text-align: center;\n}\n')
  css.write('H3\n{\n  font-family: helvetica,sans-serif;\n  font-size: medium;\n  text-align: left;\n}\n')
  css.write('A IMG\n{\n  border: none;\n}\n')
  css.write('FONT.Info\n{\n  color: black;\n  font-style: italic;\n}\n')
  css.write('FONT.Heading\n{\n  color: black;\n  font-weight: bold;\n  font-size: large;\n}\n')
  css.write('FONT.DatumName\n{\n  color: black;\n  font-weight: bold;\n}\n')
  css.write('FONT.Note\n{\n  color: black;\n  font-size: small;\n}\n')
  css.write('FONT.DQGroup\n{\n  font-size: small;\n}\n')
  css.write('FONT.Red\n{\n  color: red;\n  font-weight: bold;\n}\n')
  css.write('FONT.Yellow\n{\n  color: #ffd700;\n  font-weight: bold;\n}\n')
  css.write('FONT.Green\n{\n  color: green;\n  font-weight: bold;\n}\n')
  css.write('FONT.Disabled\n{\n  color: black;\n  font-weight: bold;\n}\n')
  css.write('FONT.Undefined\n{\n  color: gray;\n}\n')
  css.write('FONT.NoCheck\n{\n  color: black;\n  font-weight: bold;\n}\n')
  css.write('TD.Red\n{\n  background-color: red;\n}\n')
  css.write('TD.Yellow\n{\n  background-color: #ffd700;\n}\n')
  css.write('TD.Green\n{\n  background-color: green;\n}\n')
  css.write('TD.Disabled\n{\n background-color: black;\n}\n')
  css.write('TD.Undef\n{\n  background-color: gray;\n}\n')
  css.write('TD.NoCheck\n{\n  background-color: #E6E6FA;\n}\n')
  css.write('.hintanchor\n{\n font-weight: bold; \n color: navy; \n }\n')
  css.write('#hintbox{\n position:absolute; \n top: 0; \n background-color: lightyellow; \n  width: 150px; \n padding: 3px; \n border:1px solid black; \n')
  css.write('font:normal 15px Verdana;\n line-height:18px; \n z-index:100; \n border-right: 3px solid black; \n border-bottom: 3px solid black; \n visibility: hidden;\n }')
  css.close()


def stringListSystemPaths( resultsFile, location ):
  of = dqutils.HanOutputFile( resultsFile )
  dirsstring = of.stringListSystemPaths( location )
  of.setFile('')
  return dirsstring


def stringAllDQAssessments( resultsFile ):
  of = dqutils.HanOutputFile( resultsFile )
  total = of.stringAllDQAssessments()
  of.setFile('')
  return total


def saveAllHistograms( resultsFile, location, drawRefs, run_min_LB ):
  of = dqutils.HanOutputFile( resultsFile )
  nSaved = of.saveAllHistograms( location, drawRefs, run_min_LB )
  of.setFile('')
  return nSaved


