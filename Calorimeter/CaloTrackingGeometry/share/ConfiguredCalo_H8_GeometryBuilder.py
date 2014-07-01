from AthenaCommon.GlobalFlags import GlobalFlags
###################################################################################
# ConfiguredCaloGeometryBuilder
###################################################################################
# Python Setup Class for CaloGeometryBuilder
#
# Author: Andreas.Salzburger@cern.ch, modified by claire.bourdarios@cern.ch
#
# Date: 26/07/2005
#
###################################################################################
#  TODO: write documentation
###################################################################################

class ConfiguredCaloGeometryBuilder :
   def __init__(self, instname         = None,  
                      magfieldtool     = None,
                      larvolumebuilder = None, 
                      tilevolumebuilder = None, 
                      args = None):
                      
        #load the Dlls
        if 'CaloTrackingGeometry' not in theApp.Dlls:
           theApp.Dlls = [ 'CaloTrackingGeometry' ]
        
        # set the service name
        self.__svcname__      = 'CaloTrackingGeometryBuilder'
        #if GlobalFlags.DetGeo.is_ctb() :
        #  self.__svcname__       = 'Calo::CaloCTB_TrackingGeometryBuilder'
        
        # set (a default) instance name for the GeometryBuilder
        if instname is None:
          instname = 'CaloTrackingGeometryBuilder'
        self.__instname__     = instname

        # create the link to the ToolSvc name
        self.__toolname__ = 'ToolSvc.'+self.__instname__
        self.__thisGeoBuilder__ = Service( self.__toolname__ )
        
        # set the magnetic field tool with appropriate method
        self.setMagneticFieldTool(magfieldtool)
        # set the LAr volume builder with the appropriate method
        self.setLArVolumeBuilder(larvolumebuilder)
        # set the Tile volume builder with the appropriate method
        self.setTileVolumeBuilder(tilevolumebuilder)        
   
   # Set the MsgStream level      
   def msgStreamLevel(self, level):
        self.__thisGeoBuilder__.OutputLevel = level
        
   def setMagneticFieldTool(self, magfieldtool=None) :
        # set (a default) magnetic field tool to be used by the GeometryBuilder
        if magfieldtool is None:
           include ( 'TrkMagFieldTools/ConfiguredMagneticFieldTool.py' )
           magfieldtool = ConfiguredMagneticFieldTool()
        # declare the magnetic field tool
        self.__thisGeoBuilder__.MagneticFieldTool           = magfieldtool.name()
        self.__thisGeoBuilder__.MagneticFieldToolInstance   = magfieldtool.instance()       


   # Define set method for the LAr Volume Builder     
   def setLArVolumeBuilder(self, larvolumebuilder=None) :
        # set (a default) LAr volume builder
        if larvolumebuilder is None:
           include ( 'CaloTrackingGeometry/ConfiguredLArVolumeBuilder.py' )
           larvolumebuilder = ConfiguredLArVolumeBuilder()
        # declare the LAr volume builder 
        self.__thisGeoBuilder__.LArVolumeBuilder          = larvolumebuilder.name()
        self.__thisGeoBuilder__.LArVolumeBuilderInstance  = larvolumebuilder.instance()           
   
   # Define set method for the Tile Volume Builder     
   def setTileVolumeBuilder(self, tilevolumebuilder=None) :
        # set (a default) Tile volume builder
        if tilevolumebuilder is None:
           include ( 'CaloTrackingGeometry/ConfiguredTileVolumeBuilder.py' )
           tilevolumebuilder = ConfiguredTileVolumeBuilder()
        # declare the Tile volume builder 
        self.__thisGeoBuilder__.TileVolumeBuilder          = tilevolumebuilder.name()
        self.__thisGeoBuilder__.TileVolumeBuilderInstance  = tilevolumebuilder.instance()           
           
   # Return method for service name
   def name(self):
       return self.__svcname__
   
   # Return method for instance name
   def instance(self):
       return self.__instname__        
               

# calorimeter specifics :
include ( 'TBCaloGeometry/TBCaloGeometry_H8_joboptions.py' )

# mimic H8 :
include ( 'LArDetDescr/LArDetDescr_H8_joboptions.py' )
ToolSvc.TBCaloCoordinate.DBRead=0
theApp.Dlls   += [ 'LArDetDescr'  ]
