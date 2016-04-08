#!/usr/bin/env python2

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys
import re
import string
import os
dname=sys.argv[1]
bas=re.split('/',dname)
mm = re.compile("^.*[0-9][0-9]-[0-9][0-9]-[0-9][0-9]$",re.IGNORECASE)
i=len(bas)
icoun=-1
for i in range(len(bas)):
                          icoun=icoun+1
                          if mm.match(bas[i]):
                                       break
a=string.join(bas[:icoun],'/')+os.sep
if icoun == len(bas)-1 : a=string.join(bas[:],'/')+os.sep 
print a

                                      
