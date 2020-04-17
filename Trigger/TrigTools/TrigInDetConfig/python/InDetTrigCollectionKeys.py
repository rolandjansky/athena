#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

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
      self.PixelClusterAmbiguitiesMap = 'TrigPixelClusterAmbiguitiesMap'






TrigTRTKeys   = IDTrigTRTKeys( )
TrigPixelKeys = IDTrigPixelKeys( )



