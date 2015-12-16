# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################
# GeometryAlpineLayout flags
#
# Defines layout flags for Alpine/LoI/... in ISF_Fatras
# XML files are read according to the flag values
#
######################################################

## to overwrite flags first do:
# from ISF_FatrasDetDescrAlpine.CustomInDetTrackingGeometryAlpineProperties import GeometryAlpineLayoutFlags

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties

class IdDictFilename(JobProperty):
    """ IdDict.xml """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'UNDEFINED'

class PixelLayout(JobProperty):
    """ Alpine,Conical,LoI """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'UNDEFINED'

class PixelEndcapLayout(JobProperty):
    """ ECRings, Alpine,Conical,LoI """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'UNDEFINED'

class SCTLayout(JobProperty):
    """ LoI,ATLAS """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'UNDEFINED'

class doPix(JobProperty):
    """ do Pixels detector: True,False """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doSCT(JobProperty):
    """ do SCT detector: True,False """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    
class splitBarrelLayers(JobProperty):
    """ Separate builder for Inner Layers: True,False """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class isRingLayout(JobProperty):
    """ check for ring layout when building tracking volumes: True,False """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class GeometryAlpineLayoutFlags_JobProperties(JobPropertyContainer):
    """ The geometry layout flag/job property container """
	
    def __init__(self, context=""):
        JobPropertyContainer.__init__(self,context)
        return
	
    def setDefaultValues(self):
	       
        self.IdDictFilename.set_Value( "InDetIdDictFiles/IdDictInnerDetector_SLHC-24-24-dev4.xml" )
        self.PixelLayout.set_Value( "LoI" )
        self.PixelEndcapLayout.set_Value( "LoI" )
        self.SCTLayout.set_Value( "LoI" )
        self.doPix.set_Value( True )
        self.doSCT.set_Value( True )
        self.splitBarrelLayers.set_Value( False )
        self.isRingLayout.set_Value( False )
	
    def dump(self):
	
        print "IdDict filename = ", self.IdDictFilename()
        print "Pixel Layout    = ", self.PixelLayout()
        print "Pixel Endcap Layout = ", self.PixelEndcapLayout()
        print "SCT Layout        = ", self.SCTLayout()
        print "do Pix            = ", self.doPix()
        print "do SCT            = ", self.doSCT()
        print "splitBarrelLayers = ", self.splitBarrelLayers()
        print "isRingLayout      = ", self.isRingLayout()

jobproperties.add_Container(GeometryAlpineLayoutFlags_JobProperties)
jobproperties.GeometryAlpineLayoutFlags_JobProperties.add_JobProperty(IdDictFilename)
jobproperties.GeometryAlpineLayoutFlags_JobProperties.add_JobProperty(PixelLayout)
jobproperties.GeometryAlpineLayoutFlags_JobProperties.add_JobProperty(PixelEndcapLayout)
jobproperties.GeometryAlpineLayoutFlags_JobProperties.add_JobProperty(SCTLayout)
jobproperties.GeometryAlpineLayoutFlags_JobProperties.add_JobProperty(doPix)
jobproperties.GeometryAlpineLayoutFlags_JobProperties.add_JobProperty(doSCT)
jobproperties.GeometryAlpineLayoutFlags_JobProperties.add_JobProperty(splitBarrelLayers)
jobproperties.GeometryAlpineLayoutFlags_JobProperties.add_JobProperty(isRingLayout)

GeometryAlpineLayoutFlags = jobproperties.GeometryAlpineLayoutFlags_JobProperties
GeometryAlpineLayoutFlags.setDefaultValues()
