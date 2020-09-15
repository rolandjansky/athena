#!/usr/bin/env python
import os
import re
"""
This is a tool which auto-generated documentation for all the python modules/executables which are included in the systematics-tools package.
"""
wdir=os.environ["SYSTTOOLSPATH"]
pydir=wdir+"/local/bin"
docdir=wdir+"/documentation"
os.system("mkdir -p %s" % docdir)
for pyfile in os.listdir(pydir):
  if  re.findall(".*py?",pyfile) != [pyfile]: continue
  if not (".py" in pyfile and pyfile[-1]=='y'): continue
  if ("updateDocumentation" in pyfile): continue
  print('processing ', pyfile)
  #if not pyfile=="systematicsTool.py": continue
  pymodule=pyfile.replace(".py","")
  print("pydoc %s > %s/%s.doc" % (pymodule, docdir, pymodule))
  os.system("pydoc %s > %s/%s.doc" % (pymodule, docdir, pymodule))
  #os.system("pydoc -w %s" % pymodule)
  #print("pydoc -w %s" % pymodule)
  #os.system("mv %s.html %s/." % (pymodule,docdir))
  fin = open ("%s/%s.doc" %(docdir, pymodule), 'r')
  fout = open ("%s/%s.md" %(docdir, pymodule), 'w')
  inHeader=True
  for line in fin.readlines():
    countSpaces=0
    lineOut=""
    inPrefix=True
    if "FUNCTIONS" in line: inHeader=False
    for l in line:
      if l==" " and inPrefix: countSpaces+=1
      else: 
        lineOut+=l
        inPrefix=False
    line=lineOut
    line=line.strip("\n")
    line=line.strip("\r")
    #if len(line)==0: continue
    lout= line
    if countSpaces==0 and len(line)>0:
      lout= "# %s"%line
    elif countSpaces==4 and not inHeader and len(line)>0:
      lout= "## %s"%line
    else:
      pass
      #lout= line.replace("\\\r","  ")
    #lout=lout+"  "
    if "Help on module" in lout: lout ="Help on module %s" % pymodule
    fout.write("%s\n" % lout)
