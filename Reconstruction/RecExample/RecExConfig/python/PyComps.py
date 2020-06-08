# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import AthenaPython.PyAthena as PyAthena

class AutoConfigConsistencyCheckSvc(PyAthena.Svc):
   

   # this is executed after all jobo have been loaded (setDefault does not seem to be) 
   #   def setDefaults(cls, handle):
   def setup(self):


       if hasattr(self, '_autocfg_check_already_executed'):
          return

       # hack to only execute this once...
       setattr(self, '_autocfg_check_already_executed', True)
    
       from AthenaCommon.AppMgr import ServiceMgr
       from AthenaCommon.JobProperties import jobproperties as jp
       from RecExConfig.RecFlags import jobproperties as jp
       if jp.Rec.AutoConfiguration()==[]:
           self.msg.info("no AutoConfiguration requested. Leave.")
           return
       else:
           self.msg.info("Now checking AutoConfiguration see https://twiki.cern.ch/twiki/bin/view/Atlas/RecExCommonAutoConfiguration. Mainly checking that list of input file are identical on C++ and python side.")
           
       if jp.AthenaCommonFlags.isOnline():
          self.msg.info("AthenaCommonFlags.isOnLine is True : autoconfiguration checking would be tricky. Leave.")
          return
       
       fi=jp.AthenaCommonFlags.FilesInput()

       from AthenaCommon.Resilience import treatException
       # 
       try:

          # check which EventSelector,get collection accordingly
          esName=ServiceMgr.EventSelector.getFullName().partition("/")[0]

          if esName=="EventSelectorAthenaPool":
             ic=ServiceMgr.EventSelector.InputCollections
          elif esName=="EventSelectorByteStream":
             ic=ServiceMgr.EventSelector.Input
          else:
             self.msg.warning("Unknown EventSelector instance.Cannot check AutoCOnfig.")
             return


          # print fi
          # print ic
          lfi=len(fi)
          lic=len(ic)
          if lfi==0:
              self.msg.warning("AutoConfiguration requested but FilesInput is empty ! This could lead to wrong configuration. Proceed at your own risk")
              return

          if lic==0:
              self.msg.warning("AutoConfiguration requested but ServiceMgr.EventSelector.InputCollections empty ! Does not make sense. Proceed at your own risk.")
              return

          samelist=(fi==ic)
          samefirstfile=( fi[0]==ic[0] )  

          if samelist:
              self.msg.info("AutoConfiguration requested, athena input file list and AthenaCommonFlags.FilesInput  are identical. All Ok!")
              return
          else:
              self.msg.warning("AutoConfiguration requested,  athena input file list  and AthenaCommonFlags.FilesInput() are different, %s vs %s " % (ic,fi))       

          if samefirstfile :
              self.msg.warning ("At least the first files are the same, but still this is dangerous.! Proceed at your own risk.")           
          else:    
              self.msg.warning("Even the first files are different ( %s vs %s ) This could lead to wrong configuration. Proceed at your own risk" % (ic[0],fi[0]))
       except Exception:

          treatException("Could not check autoconfiguration. Could be very wrong! Please report.")


       return


   def initialize(self):
       return PyAthena.StatusCode.Success
   def finalize(self):
       return PyAthena.StatusCode.Success


