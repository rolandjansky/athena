
# -------------------------------------------------------------------------------------
#  XMLReader jobproperties 
# -------------------------------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties

class IdDictFileName(JobProperty):
    """ IdDict.xml """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'UNDEFINED'
class PixelBarrelLayout(JobProperty):
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'UNDEFINED'
class PixelEndcapLayout(JobProperty):
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'UNDEFINED'
class SCTBarrelLayout(JobProperty):
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'UNDEFINED'
class SCTEndcapLayout(JobProperty):
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'UNDEFINED'
class doPix(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
class doSCT(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True   
class isGMX(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False   
class addBCL(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False 
class splitBarrelLayers(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
class isRingLayout(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
class InnerLayerIndices(JobProperty):
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
class InnerDiskIndices(JobProperty):
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
class Initialized(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
class readXMLfromDB(JobProperty):
    statusOn     = True;
    allowedTypes = ['bool']
    StoredValue  = False

# add to jobproperties
class XMLReaderFlags_JobProperties(JobPropertyContainer):
    """ XMLReader properties """
    def __init__(self, context=""):
        JobPropertyContainer.__init__(self,context)
        return

    def setValuesFromSetup(self,**kwargs):

        print "XMLReaderFlags setup : "
        for key in kwargs:
            print "   %s: %s" % (key, kwargs[key])    

        self.PixelBarrelLayout = kwargs["PixelBarrelLayout"]
        if kwargs["PixelEndcapLayout"]!=None: 
            self.PixelEndcapLayout = kwargs["PixelEndcapLayout"]
        self.SCTBarrelLayout = kwargs["SCTBarrelLayout"]
        self.SCTEndcapLayout = kwargs["SCTEndcapLayout"]
        self.doPix  = kwargs["doPix"]
        self.doSCT  = kwargs["doSCT"]
        self.isGMX  = kwargs["isGMX"]
        self.addBCL = kwargs["addBCL"]

        self.splitBarrelLayers = False
        self.isRingLayout = False
        self.InnerLayerIndices = []
        self.InnerDiskIndices = []

        if "ECRing" in self.PixelEndcapLayout(): #
            self.isRingLayout  = True            #
            
        if "Alpine4" in self.PixelBarrelLayout():#
            self.isRingLayout  = True            #
            self.splitBarrelLayers = True        #  
            if "Alpine4" in self.PixelEndcapLayout():#
                self.InnerLayerIndices = [0]#

        if "ExtBrl" in self.PixelBarrelLayout():#
            self.splitBarrelLayers = True       #  
            self.InnerLayerIndices = [0,1]      #

        if "InclBrl" in self.PixelBarrelLayout():#
            self.splitBarrelLayers = True        #
            self.InnerLayerIndices = [0,1]       #
            from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags#
            if (SLHC_Flags.LayoutOption == "InclinedQuads"):           #
              self.splitBarrelLayers = False                           #
            if (SLHC_Flags.LayoutOption == "InclinedAlternative"):     #
              self.InnerDiskIndices = [0,29]                           #

        self.Initialized = True                                        #
        self.readXMLfromDB = False                                     #
        if kwargs.has_key("readXMLfromDB"): self.readXMLfromDB = kwargs["readXMLfromDB"]

    def dump(self):
        print "Pixel = ", self.PixelBarrelLayout(), " ", self.PixelEndcapLayout()
        print "SCT   = ", self.SCTBarrelLayout(), " ", self.SCTEndcapLayout()
        print "ID    =>  doPixel ", self.doPix,"  doSCT ",self.doSCT, "  isGMX ",self.isGMX, "  addBCL ",self.addBCL
        print "splitBarrel  : ", self.splitBarrelLayers()
        print "isRingLayout : ", self.isRingLayout()
        print "InnerLayer   : ", self.InnerLayerIndices()
        print "InnerDisk    : ", self.InnerDiskIndices()
        print "DB-CLOB      : ", self.readXMLfromDB()
        
jobproperties.add_Container(XMLReaderFlags_JobProperties)
jobproperties.XMLReaderFlags_JobProperties.add_JobProperty(IdDictFileName)
jobproperties.XMLReaderFlags_JobProperties.add_JobProperty(PixelBarrelLayout)
jobproperties.XMLReaderFlags_JobProperties.add_JobProperty(PixelEndcapLayout)
jobproperties.XMLReaderFlags_JobProperties.add_JobProperty(SCTBarrelLayout)
jobproperties.XMLReaderFlags_JobProperties.add_JobProperty(SCTEndcapLayout)
jobproperties.XMLReaderFlags_JobProperties.add_JobProperty(doPix)
jobproperties.XMLReaderFlags_JobProperties.add_JobProperty(doSCT)
jobproperties.XMLReaderFlags_JobProperties.add_JobProperty(isGMX)
jobproperties.XMLReaderFlags_JobProperties.add_JobProperty(addBCL)
jobproperties.XMLReaderFlags_JobProperties.add_JobProperty(splitBarrelLayers)
jobproperties.XMLReaderFlags_JobProperties.add_JobProperty(isRingLayout)
jobproperties.XMLReaderFlags_JobProperties.add_JobProperty(InnerLayerIndices)
jobproperties.XMLReaderFlags_JobProperties.add_JobProperty(InnerDiskIndices)
jobproperties.XMLReaderFlags_JobProperties.add_JobProperty(Initialized)
jobproperties.XMLReaderFlags_JobProperties.add_JobProperty(readXMLfromDB)

XMLReaderFlags = jobproperties.XMLReaderFlags_JobProperties
