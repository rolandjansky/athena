""" SLHC_Setup
    Python module to hold storegate keys of InDet objects.
"""

__author__ =   "A. Salzburger"
__version__=   "$Revision: 1.13 $"
__doc__    =   "SLHC_PathSetting"
__all__    = [ "SLHC_PathSetting" ]

import os
from os.path import exists, join
from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import AtlasGeoDBInterface

class SLHC_Setup_XMLReader :
    # constructor requires the SLHC_Flags

    def __init__(self,**kwargs):

        from InDetTrackingGeometryXML.XMLReaderJobProperties import XMLReaderFlags
        print dir(XMLReaderFlags)
        if XMLReaderFlags.Initialized(): return

        import os, shutil
        from PyJobTransformsCore.envutil import find_file_env

        print "*******************************************************************************************"
        print "*******************************************************************************************"
        print "*******************************************************************************************"

        for key in kwargs:
            print "another keyword arg: %s: %s" % (key, kwargs[key])
        
        #--------------------------------------------------------------
        # XML reader
        #-------------------------------------------------------------

        ###### Setup ITk XML reader ######
        from InDetTrackingGeometryXML.InDetTrackingGeometryXMLConf import InDet__XMLReaderSvc
        xmlReader = InDet__XMLReaderSvc(name='InDetXMLReaderSvc')

        dictionaryFileName = ""
        if kwargs.has_key("dictionaryFileName"): dictionaryFileName = kwargs["dictionaryFileName"]
        createXML = False
        if kwargs.has_key("createXML"): createXML = kwargs["createXML"]
        doPix = kwargs["doPix"]
        doSCT = kwargs["doSCT"]
        isGMX = kwargs["isGMX"]
        AddBCL = False
        if kwargs.has_key("addBCL"): AddBCL = kwargs["addBCL"]
        
        PIXMODULEFILE = "ITK_PixelModules.xml"
        PIXSTAVEFILE  = str(kwargs["PixelLayout"]) + "_PixelStave.xml"
        PIXBARRELFILE = str(kwargs["PixelLayout"]) + "_PixelBarrel.xml"
        PIXENDCAPFILE = str(kwargs["PixelLayout"]) + "_PixelEndcap.xml"
        if kwargs.has_key("PixelEndcapLayout"): PIXENDCAPFILE = str(kwargs["PixelEndcapLayout"]) + "_PixelEndcap.xml"
        
        SCTMODULEFILE = "ITK_SCTModules.xml"
        SCTSTAVEFILE  = str(kwargs["SCTLayout"]) + "_SCTStave.xml"
        SCTBARRELFILE = str(kwargs["SCTLayout"]) + "_SCTBarrel.xml"
        SCTENDCAPFILE = str(kwargs["SCTLayout"]) + "_SCTEndcap.xml"

        ## ###### Setup dictionary file to use  ######
        ## dictDir = os.path.dirname(dictionaryFileName)
        ## if not os.path.exists(dictDir):
        ##     os.makedirs(dictDir)

        # Check if PIXXD node is define din the DB
        readXMLfromDB_PIXXDD = False
        from AthenaCommon.GlobalFlags import globalflags
        geoTagName = globalflags.DetDescrVersion()
        dbGeomCursor = AtlasGeoDBInterface.AtlasGeoDBInterface(geoTagName,False)
        dbGeomCursor.ConnectAndBrowseGeoDB()
        dbId,dbXDD,dbParam = dbGeomCursor.GetCurrentLeafContent("PIXXDD")
        if len(dbId)>0: readXMLfromDB_PIXXDD = True

        ###### Setup XMLreader flags
        print "**** FLAGS **************************************************"
        print "Datapath: "+'DATAPATH'
        XMLReaderFlags.setValuesFromSetup( PixelBarrelLayout = find_file_env(str(PIXBARRELFILE),'DATAPATH'),
                                           PixelEndcapLayout = find_file_env(str(PIXENDCAPFILE),'DATAPATH'),
                                           SCTBarrelLayout = find_file_env(str(SCTBARRELFILE),'DATAPATH'),
                                           SCTEndcapLayout = find_file_env(str(SCTENDCAPFILE),'DATAPATH'),
                                           doPix = kwargs["doPix"],
                                           doSCT = kwargs["doSCT"],
					   isGMX = kwargs["isGMX"],
					   addBCL = AddBCL,
                                           readXMLfromDB = readXMLfromDB_PIXXDD
                                           )
        XMLReaderFlags.dump() #
        print "******************************************************"

        ###### Setup XML builders properties ######
        xmlReader.dictionaryFileName  = kwargs["dictionaryFileName"]
        xmlReader.createXMLDictionary = False
        if kwargs.has_key("createXML"): xmlReader.createXMLDictionary = kwargs["createXML"]
        xmlReader.doPix = XMLReaderFlags.doPix()
        xmlReader.doSCT = XMLReaderFlags.doSCT()
        xmlReader.isGMX = XMLReaderFlags.isGMX()
        xmlReader.readXMLfromDB = XMLReaderFlags.readXMLfromDB()

        if not XMLReaderFlags.readXMLfromDB():

            ###### Setup XML files for Material ######
            xmlReader.XML_Materials         = find_file_env("Materials.xml",'DATAPATH')

            ###### Setup XML filesfor pixels ######
            xmlReader.XML_PixelModules      = find_file_env(str(PIXMODULEFILE),'DATAPATH')
            xmlReader.XML_PixelStaves       = find_file_env(str(PIXSTAVEFILE),'DATAPATH')
            xmlReader.XML_PixelBarrelLayers = XMLReaderFlags.PixelBarrelLayout()
            if XMLReaderFlags.PixelEndcapLayout()!='UNDEFINED' :
                xmlReader.XML_PixelEndcapLayers = XMLReaderFlags.PixelEndcapLayout()
            ###### Setup XML files for SCT ######
            xmlReader.XML_SCTModules      = find_file_env(str(SCTMODULEFILE),'DATAPATH')
            xmlReader.XML_SCTStaves       = find_file_env(str(SCTSTAVEFILE),'DATAPATH')
            xmlReader.XML_SCTBarrelLayers = XMLReaderFlags.SCTBarrelLayout()
            xmlReader.XML_SCTEndcapLayers = XMLReaderFlags.SCTEndcapLayout()
            
        else:

           # Create local XML file correspondng to the XML CLOB defined in the geometry DB
            keywordIndex = dbParam.index("KEYWORD")
            clobIndex = dbParam.index("XMLCLOB")
            pathName = "/".join([os.environ["PWD"],"XML-"+geoTagName])
            print "PATHNAME : ",pathName
            if not os.path.exists(pathName): os.mkdir( pathName, 0755 )
            for key in dbXDD.keys():
                v = dbXDD[key]
                fileName = pathName+"/"+v[keywordIndex]+".xml"
                f=open(fileName,"w")
                f.write(v[clobIndex])
                f.close()

            ###### Setup XML files for Material ######
            xmlReader.XML_Materials         = pathName+"/Materials.xml"

            ###### Setup XML filesfor pixels ######
            xmlReader.XML_PixelModules      = pathName+"/"+str(PIXMODULEFILE)
            xmlReader.XML_PixelStaves       = pathName+"/"+str(PIXSTAVEFILE)
            xmlReader.XML_PixelBarrelLayers = pathName+"/"+str(PIXBARRELFILE)
            if XMLReaderFlags.PixelEndcapLayout()!='UNDEFINED' :
                xmlReader.XML_PixelEndcapLayers = pathName+"/"+str(PIXENDCAPFILE)
            ###### Setup XML files for SCT ######
            xmlReader.XML_SCTModules      = pathName+"/"+str(SCTMODULEFILE)
            xmlReader.XML_SCTStaves       = pathName+"/"+str(SCTSTAVEFILE)
            xmlReader.XML_SCTBarrelLayers = pathName+"/"+str(SCTBARRELFILE)
            xmlReader.XML_SCTEndcapLayers = pathName+"/"+str(SCTENDCAPFILE)
            
        from AthenaCommon.AppMgr import theApp
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        svcMgr += xmlReader
        theApp.CreateSvc.insert(0,"InDet::XMLReaderSvc/InDetXMLReaderSvc")
#        print svcMgr

        
        from AthenaCommon.AppMgr import theApp
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from InDetTrackingGeometryXML.InDetTrackingGeometryXMLConf import InDet__GMXReaderSvc
        gmxReader = InDet__GMXReaderSvc(name='InDetGMXReaderSvc')
        gmxReader.dictionaryFileName=xmlReader.dictionaryFileName
        gmxReader.addBCL= XMLReaderFlags.addBCL()
        svcMgr += gmxReader
        gmxReader.OutputLevel=1
        theApp.CreateSvc.insert(1,"InDet::GMXReaderSvc/InDetGMXReaderSvc")
