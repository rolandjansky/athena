#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#----------------------------------------------------------------------------------------------------------
#
#           This file contains names of the collections used across the Inner detector Trigger SW
#     


class IDTrigTRTKeys:
   """ Keys for the Transition Radiation Tracker """
   def __init__ (self):
      #Created by TRTRawDataProvider 
      self.RDOs         = "TRT_RDOs_TRIG" #Raw data objects

      #Created by RIO_MAKER
      self.DriftCircles = "TRT_TrigDriftCircles" 


class IDTrigPixelKeys:
   """ Keys for the Pixel """
   def __init__ (self):

      self.Clusters = "PixelTrigClusters" #FIXME: "TrigPixelClusters" 

      #Created by silicon Tracker space point finder alg
      self.SpacePoints = "PixelTrigSpacePoints"#FIXME: "TrigPixelSpacePoints" 

      #Created by InDetPRD_AssociationTool(GangedPixels)
      #self.PRDtoTrackMap = "TrigPixelClusterAmbiguitiesMap"
      self.PRDtoTrackMap = "TrigPRDtoTrackMap"
      self.PixelClusterAmbiguitiesMap = 'TrigPixelClusterAmbiguitiesMap'


class IDTrigSCTKeys:
   """ Keys for the Pixel """
   def __init__ (self):

      self.Clusters = "SCT_TrigClusters"  #FIXME unify this with the naming of others? (Remove the underscore, Trig as prefix)

      #Created by silicon Tracker space point finder alg
      self.SpacePoints = "SCT_TrigSpacePoints" # FIXME: should be unified "TrigSCTSpacePoints" 



TrigTRTKeys   = IDTrigTRTKeys( )
TrigPixelKeys = IDTrigPixelKeys( )
TrigSCTKeys   = IDTrigSCTKeys( )
