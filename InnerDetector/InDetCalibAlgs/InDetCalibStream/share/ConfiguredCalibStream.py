# Configured Calibration Stream  
#
# To be configured:
#   - Instance name

class ConfiguredCalibStream :
   def __init__(self, instname = None, outputDirectory = "/tmp/belmiro",  trackParticlesName = "TrackParticleCandidate"):
       # load the Dll for the processor
       if 'InDetCalibStream' not in theApp.Dlls:
         theApp.Dlls += [ "InDetCalibStream" ]

       # set the service name
       self.__svcname__ = 'InDet::CalibStream'

# ---------- set up (a default) instance name
       if instname is None :
          instname = 'CalibStream'
       self.__instname__ = instname
       self.__toolname__ = self.__instname__
       theApp.TopAlg += [ self.__svcname__+"/"+self.__instname__ ]
       self.__thisEx__ = Algorithm( self.__instname__ )
       # set input track collection
       self.__thisEx__.OutputDirectory=outputDirectory
       self.__thisEx__.AppName="Athena"
       self.__thisEx__.FileTag="csc13"
       self.__thisEx__.TrackParticlesName=trackParticlesName
       self.__thisEx__.MakeByteStream=True
       self.__thisEx__.UseTrackCollection=True
       
       self.printInfo();

   # Set the MsgStream level
   def msgStreamLevel(self, level):
       self.__thisEx__.OutputLevel = level

   # Return method for the service name
   def name(self):
       return self.__svcname__

   # Return method for the instance name
   def instance(self):
       return self.__instname__

   # Output method for screen dump
   def printInfo(self):
       print '***** ConfiguredCalibStream ***********************************************'
       print '*---------------------------------------------------------------------------------'
       print '* - Algorithm Name:     ', self.__toolname__
       print '* - Output Directory        : ', self.__thisEx__.OutputDirectory
       print '* - AppName                 : ', self.__thisEx__.AppName
       print '* - FileTag                 : ', self.__thisEx__.FileTag
       print '* - TrackParticleOutputColl : ', self.__thisEx__.TrackParticlesName
       print '**********************************************************************************'

 
ConfiguredCalibStream()
