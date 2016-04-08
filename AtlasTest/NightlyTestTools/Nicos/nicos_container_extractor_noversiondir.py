#!/usr/bin/env python2

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys
import re
import string
import os
dname=sys.argv[1]
bas=re.split('/',dname)
i=len(bas)
abn=range(i)
abn.reverse()
cmtstruct="with_version_directory"
if os.environ.has_key('CMTSTRUCTURINGSTYLE') :
    cmtstruct=os.environ['CMTSTRUCTURINGSTYLE']
relhome=""
if os.environ.has_key('NICOS_RELHOME') :
    relhome=os.environ['NICOS_RELHOME']+os.sep
icoun=-1
jbas=dname[:-1]
found=0
for i in abn:
                          jbas=string.join(bas[:i+1],'/')+os.sep
                          path_full=relhome+jbas+"cmt"+os.sep+"requirements"
#                          print path_full
                          found=os.access(path_full, os.F_OK)
#                          print "DDD", found, "\n"
                          if found: break
print jbas

                                      
