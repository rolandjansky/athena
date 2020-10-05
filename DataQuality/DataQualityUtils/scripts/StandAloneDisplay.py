#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

import os
## Needed to correct ROOT behavior; see below
CWD = os.getcwd()

import sys

import ROOT  # noqa: F401
## Importing gSystem may change the current directory to one of the
## command-line arguments; chdir to original directory to have
## predictable behavior
from ROOT import gSystem
os.chdir(CWD)

## Import the ROOT library for reading han results
gSystem.Load('libDataQualityUtils')
from ROOT import dqutils


def handi( name, resultsFile, htmlDir ):

  if ( htmlDir.rfind("/")!=(len(htmlDir)-1) ):  # htmlDir needs "/" at the end
        htmlDir+="/"
  
  of = dqutils.HanOutputFile(resultsFile)
  
  dirsstring = of.stringListSystemPaths(htmlDir)
  dirs = dirsstring.rsplit()
  for subHtmlDir in dirs:
    if( not os.access(subHtmlDir,os.F_OK) ):
      try:
        os.makedirs(subHtmlDir)
      except os.error:
        print('Cannot create directory "' + subHtmlDir + '"; exiting.')
        sys.exit(-1)
  
  total=of.stringAllHistograms()
  of.saveAllHistograms(htmlDir,False)
  
  s=total.rsplit('\n')
  # number = number of lines in total
  number=len(s)
  if (len(s[number-1])<1): # last line is empty
    number-=1

  list, namelist = makeAllDirsFile( htmlDir, name, s, number, resultsFile )

  for x in range(0,len(list)):
    makeSubDirFile( htmlDir, name, s, number, namelist[x], list[x] )
    makeCSSFile( htmlDir,"", namelist[x] )
  
  makeCSSFile( htmlDir,"", "." )


def usage():
  cmdi = sys.argv[0].rfind("/")
  cmd = sys.argv[0][cmdi+1:]
  print("Usage: ", cmd, "<imput_file> <html_output_directory>")

def makeAllDirsFile( htmlDir, name, s, number, resultsFile ):
  g=open(htmlDir+'index.html','w')
  g.write('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">\n')
  g.write('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">\n')
  g.write('<head>\n')
  g.write('<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />\n')
  g.write('<title>'+ name+  '</title>\n')
  g.write('<link rel="stylesheet" href="AutomChecks.css" type="text/css" />\n')
  g.write('</head>\n')
  g.write('<body>')
  g.write('<font class="DQGroup">[<a href="../index.html">Back</a>]</font>\n')
  g.write('<h1>' + name + ': Monitoring</h1>\n')

  # initial number of white spaces, will change to positive value once we go over the lines
  spaces=-1
  # list = list with directories (the line number) that contain histos
  list=[]
  # namelist = list with corresponding direcotory names
  namelist=[]

  num_lists=0

  g.write('<table>\n<tr valign="top">\n<td width="500">\n')
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
        g.write('<ul>\n<li>')        
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
      if namedir!='<top_level>':
        g.write('<img src="pixel.png" width="0" height="13" alt="" />')
      if ( (x<number-1) and (s[x+1].rsplit())[3]=='ass' ): # check that dir contains histos
        if namedir=='<top_level>':
          namedir = '.'
          g.write('<font class="DQGroup"><a href="'+namedir +'/toplevel.html">'+shortNameDir+ '</a></font>')
        else:
          g.write('<font class="DQGroup"><a href="'+namedir +'/index.html" >'+shortNameDir+ '</a></font>')
        list.append(x)
        namelist.append(namedir)
      else:
        g.write('<font class="DQGroup">'+shortNameDir+ '</font>')
      spaces=spaces_new
  if num_lists>0:
    g.write('</li>')
  for x in range(0,num_lists-1):
    g.write('</ul></li>\n')
  if num_lists>0:
     g.write('</ul>\n')
  g.write('</td>\n<td><font class="Info">From file:</font><br/><font class="Note">' + resultsFile + '</font></td>\n</tr>\n</table>')
  g.write('</body>\n</html>')
  g.close()
  return list, namelist


def makeSubDirFile( htmlDir, name, s, number, subname, assessIndex ):
  
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
  h.write('</head>\n')
  h.write('<body>\n')
  h.write('<font class="DQGroup">[<a href="')
  for x in range(subname.count("/")):
    h.write('../')
  h.write('../index.html">Back</a>]</font>\n')
  h.write('<center>')
  h.write('<table>\n<tr valign="top">\n<td width="250"></td>\n')
  h.write('<td width="300"><h2>'+name+' '+subnameTitle+'</h2></td>\n')
  h.write('<td width="250">\n<font class="Note">Click on images for details and full size.</font>\n')
  h.write('</td>\n</tr>\n</table>\n')
  h.write('<table cellpadding="4" cellspacing="20">\n')    
  y=assessIndex+1
  sp=s[y].rsplit()
  col=0
  while(sp[3]=='ass' and y<number):
    sp=s[y].rsplit()
    col+=1
    if col==1:
      h.write('<tr>\n<td class="' + sp[1] + '"><a href="'+sp[0]+'.html"><img src="'+ sp[0] +'.png" height="200"  alt="' + name + ' ' + subname+'/'+sp[0]+'.png" /></a></td>\n')
    elif col==3:
      h.write('<td class="' + sp[1] + '"><a href="'+sp[0]+'.html"><img src="'+ sp[0] +'.png" height="200"  alt="' + name + ' ' + subname+'/'+sp[0]+'.png" /></a></td>\n</tr>\n')
      col=0
    else:
      h.write('<td class="' + sp[1] + '"><a href="'+sp[0]+'.html"><img src="'+ sp[0] +'.png" height="200" alt="' + name + ' ' + subname+'/'+sp[0]+'.png" /></a></td>\n')
    makeOneHistFile( htmlDir, name, subname, sp )
    y=y+1
    if y< number-1:
      sp=s[y].rsplit()
  if not (col==3):
    h.write('</tr>\n')
  h.write('</table>\n</center>\n</body>\n</html>\n')
  h.close()


def makeOneHistFile( htmlDir, name, subname, sp ):
  k=open(htmlDir+'/'+subname+'/'+sp[0]+'.html','w')
  k.write('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">\n')
  k.write('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">\n')
  k.write('<head>\n')
  k.write('<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />\n')
  k.write('<title>'+name+ ' ' + subname+ ' ' + sp[0]+'</title>\n')
  k.write('<link rel="stylesheet" href="AutomChecks.css" type="text/css" />\n')
  k.write('</head>\n')
  k.write('<body>\n')
  k.write('<center>\n')
  k.write('<table>\n<tr valign="top">\n')
  if subname=='.':
    k.write('<td width="250"><font class="DQGroup">[<a href="toplevel.html">Back</a></font>]</td>\n')
  else:
    k.write('<td width="250"><font class="DQGroup">[<a href="index.html">Back</a></font>]</td>\n')
  k.write('<td width="500"><h2>'+name + ' ' + subname+'/'+sp[0]+'</h2></td>\n')
  k.write('<td width="250"></td>\n</tr></table>\n')
  k.write('<table cellpadding="10">\n<tr>\n')
  k.write('<td>\n<table width="400">\n')
  k.write('<tr><td colspan="2"><font class="Heading">Details:</font></td></tr>\n')
  k.write('<tr><td>&nbsp;</td></tr>\n')
  k.write('<tr><td align="right"><font class="DatumName">Name:</font></td>')
  k.write('<td>' +sp[0] +'</td></tr>\n' )
  k.write('</table>\n</td>\n')
  if subname == '.':
    k.write('<td><a href="toplevel.html"><img src="'+ sp[0] +'.png" alt="' + name + ' ' + subname+'/'+sp[0]+'.png" /></a></td>\n')
  else:
    k.write('<td><a href="index.html"><img src="'+ sp[0] +'.png" alt="' + name + ' ' + subname+'/'+sp[0]+'.png" /></a></td>\n')
  k.write('</tr></table>\n')
  k.write('</center>\n</body>\n</html>\n')
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
  css.write('FONT.Info\n{\n  color: black;\n  font-style: italic;\n}\n')
  css.write('FONT.Heading\n{\n  color: black;\n  font-weight: bold;\n  font-size: large;\n}\n')
  css.write('FONT.DatumName\n{\n  color: black;\n  font-weight: bold;\n}\n')
  css.write('FONT.Note\n{\n  color: black;\n  font-size: small;\n}\n')
  css.write('FONT.DQGroup\n{\n  font-size: small;\n}\n')
  css.write('FONT.Red\n{\n  color: red;\n  font-weight: bold;\n}\n')
  css.write('FONT.Yellow\n{\n  color: #ffd700;\n  font-weight: bold;\n}\n')
  css.write('FONT.Green\n{\n  color: green;\n  font-weight: bold;\n}\n')
  css.write('FONT.Undef\n{\n  color: gray;\n  font-weight: bold;\n}\n')
  css.write('FONT.NoCheck\n{\n  color: black;\n  font-weight: bold;\n}\n')
  css.write('TD.Red\n{\n  background-color: red;\n}\n')
  css.write('TD.Yellow\n{\n  background-color: #ffd700;\n}\n')
  css.write('TD.Green\n{\n  background-color: green;\n}\n')
  css.write('TD.Undef\n{\n  background-color: gray;\n}\n')
  css.write('TD.NoCheck\n{\n  background-color: #E6E6FA;\n}\n')
  css.close()


########################################
## main()
########################################

if __name__ == "__main__":

  if len(sys.argv) != 3:
    usage()
    sys.exit(0)
  
  # Input
  resultsFile = sys.argv[1] # input .root file
  html_dir=sys.argv[2] # destination directory for html files
  name=resultsFile
  handi( name, resultsFile, html_dir )
