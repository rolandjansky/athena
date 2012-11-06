# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os

# module: PixeRTT_PythonValidationModule.py
		
class PixeRTT_ValidationModule:
   def __init__(self,argDict={}):
       # access my args from the dictionary passed in, and convert any basic types
       self.executable = argDict.get('executable')
       self.input = argDict.get('input')
       self.output = argDict.get('output')
       self.reference = argDict.get('reference')
       #self.nevents = argDict.get('nevents')

   def run(self):
       # do the stuff I wanted to do...
       os.execlp(self.executable,self.executable,self.input,self.output,self.reference)
