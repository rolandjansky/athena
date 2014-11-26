# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# InDet GeoModel initialization
#
from AthenaCommon.GlobalFlags import globalflags

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
import coral


# -------------------------------------------------------------------------------------
#  AtlasDBInterface : Interface with the geometry DB
# -------------------------------------------------------------------------------------

class AtlasDBInterface:

    def __init__(self,dbTag,verbose=False):

        self.dbGeoTag=dbTag
        self.dbGeoTagId=""
        self.dbSession=0
        self.dbSchema=0
        self.bVerbose=verbose

        self.TagAndNodeVersionDict={}

    def __del__(self):

        if self.bVerbose: print "-> close DB connection"
        del self.dbSession


    def ConnectToDB(self):

        svc = coral.ConnectionService()
        svcconfig = svc.configuration()
        svcconfig.disablePoolAutomaticCleanUp()
        svcconfig.setConnectionTimeOut(0)
        svcconfig.setConnectionRetrialPeriod(300)
        svcconfig.setConnectionRetrialTimeOut(3600)
        self.dbSession = svc.connect( 'ATLASDD', accessMode = coral.access_ReadOnly )
        transaction = self.dbSession.transaction()

        self.dbSchema = self.dbSession.nominalSchema()
        transaction.start(True)

    def ConnectAndBrowseGeoDB(self):

        self.ConnectToDB()
        self.ReadGeometryTagId()
        self.ReadTagAndNodeVersionsFromDB()


    def ReadGeometryTagId(self):
        """ Read the geometry Id corresponding to the geometry tag name in order to collect the child node Id's """
        
        query = self.dbSchema.tableHandle("HVS_TAG2NODE").newQuery()

        query.addToOutputList('TAG_NAME')
        query.addToOutputList('TAG_ID')

        bindstag2node = coral.AttributeList() 
        bindstag2node.extend('tagN','string') 
        bindstag2node[0].setData(self.dbGeoTag)
        condString='TAG_NAME=:tagN'
        
        query.setCondition(condString,bindstag2node)

        for currentRow in iter(query.execute()):
            for i in range(0,currentRow.size()):
                if currentRow[i].specification().name()=="TAG_ID":
                    if self.bVerbose: print "*** GeoTagId ******** ",str(currentRow)
                    self.dbGeoTagId=currentRow[i].data()
                    continue

        del query
        return


    def AnalyseDBRequestOutput_Query(self,request,keyName=""):
        """ Analyze request output -> return a list of dictionaries and a list of parameter types """
        
        dataList=[]
        typeList=[]

        # Loop over row
        iCmpt=0
        for row in iter(request.execute()):
            dictLoc={}
            # loop over parameters read fron the DB
            for j in range(0,row.size()):
                name=row[j].specification().name()
                paramType=row[j].specification().typeName()
                value=row[j].data()
                dictLoc[name]=value
                # collect parameter name/types
                if iCmpt==0: typeList.append((name,paramType))
            iCmpt+=1
            dataList.append(dictLoc)

        # return datalist as a list of dictionaries ( 1 dictionary per line)
        if keyName=="":
            return dataList,typeList

        # reorganize dataList as a dictionary with keys based on the keyName input parameter
        dataDict={}
        for d in dataList:
            k=d[keyName]
            v=[d[paramName] for paramName,tmp in typeList if paramName!=keyName]
            dataDict[k]=v

        return dataDict,typeList


    def ReadTagAndNodeVersionsFromDB(self):
        """ Read tag and node versions defined for the geometry tag """

        # Proceed by looping over the chilg Id's as long as childs are defined 
        
        bStopLoop=False
        tagDictGbl={}
        tagIdList=[int(self.dbGeoTagId)]    # start with the geometry tag Id

        # Loop while child tags are found
        while not bStopLoop :

            query0 = self.dbSchema.newQuery();

            query0.addToOutputList('C.NODE_NAME',"nodename")
            query0.addToOutputList('A.TAG_NAME',"tagname")
            query0.addToOutputList('A.TAG_ID',"tagid")

            query0.addToTableList( "HVS_TAG2NODE","A" )
            query0.addToTableList( "HVS_LTAG2LTAG","B")
            query0.addToTableList( "HVS_NODE","C" )

            bindstag2node = coral.AttributeList()
            condString="C.NODE_ID=A.NODE_ID AND A.TAG_ID=B.CHILD_TAG and B.PARENT_TAG IN (%s)" % str(tagIdList)[1:-1]
            if self.bVerbose: print "----------------------------\n"+condString
            query0.setCondition(condString,bindstag2node)

            # Analyze the output and collect the new tag and node versions
            dictRes,paramType=self.AnalyseDBRequestOutput_Query(query0,"nodename")
            tagDictGbl.update(dictRes)
            del query0
            
            if len(dictRes.keys())==0:
                # No child found
                bStopLoop=True
            else:
                # create child Id's list for the next loop
                tagIdList=[int(dictRes[k][1]) for k in dictRes.keys()]


        self.TagAndNodeVersionDict.update(tagDictGbl)

    
    def GetCurrentLeafContent(self, leafName):
        """ Read a DB table content and return 3 lists :
                     - dbId : the data_id list corresponding to each entry in the table
                     - dbContent : the table values as a dictionary with keys corresponding to the dbId list
                     - paramName : the name of the table parameters """
        
        upLeafName=leafName.upper()

        # check if table is defined in the current geometry
        if not self.TagAndNodeVersionDict.has_key(leafName):
            dbId=[]
            dbContent={}
            paramName=[]
            return dbId,dbContent,paramName


        # ------ leafName_DATA_ID entries        
        leafTagId=self.TagAndNodeVersionDict[leafName][1]

        query = self.dbSchema.tableHandle(upLeafName+"_DATA2TAG").newQuery()
        query.addToOutputList(upLeafName+'_DATA_ID')
        
        bindstag2node = coral.AttributeList() 
        condString=upLeafName+'_TAG_ID='+str(leafTagId)
        query.setCondition(condString,bindstag2node)

        dictRes,typeList=self.AnalyseDBRequestOutput_Query(query)
        dataIdList=[int(s[upLeafName+'_DATA_ID']) for s in dictRes]
        del query
        
        # ------- values corresponding to leafName_DATA_ entries

        query = self.dbSchema.tableHandle(upLeafName+"_DATA").newQuery()

        condString=upLeafName+'_DATA_ID IN ('+str(dataIdList)[1:-1]+") ORDER BY "+upLeafName+"_DATA_ID" 
        query.setCondition(condString,bindstag2node)

        dictRes,typeList=self.AnalyseDBRequestOutput_Query(query)
        del query

        # ------- build output
        #   paramNames : name of the DB parameters (following the table columns order)
        #   dbId       : data_id vector
        #   dbContent  : table content (key = dbId, value=list of value fooliwin the paramName ranking scheme)

        paramName=[s[0] for s in typeList]

        paramKey=typeList[0][0]
        dbId=[]
        dbContent={}
        for s in dictRes:
            key=int(s[paramKey])
            dbId.append(key)
            v=[s[k[0]] for k in typeList]
            dbContent[key]=v

        return dbId,dbContent,paramName



# -------------------------------------------------------------------------------------
#  InDet geometry flags initialization
# -------------------------------------------------------------------------------------

class InDetGMFlags:

    def __init__(self, geoTag="none"):

        self.__dict__={}

        # set geometry tag name
        self.__dict__["geomTag"]= globalflags.DetDescrVersion()
        if geoTag!="none": self.__dict__["geomTag"] = geoTag
        
        self.dbGeomCursor = 0
        self.connectToDB()
    
        self.InitializeGeometryParameters()

    def connectToDB(self):

        bVerbose=False
        self.dbGeomCursor = AtlasDBInterface(self.__dict__["geomTag"],bVerbose)
        self.dbGeomCursor.ConnectAndBrowseGeoDB()

        return


    def InitializeGeometryParameters(self):

        # ----------------------------------------------------------------------------    
        # Read versionname, layout and dbm from PixelSwicthes table

        dbId,dbSwitches,dbParam = self.dbGeomCursor.GetCurrentLeafContent("PixelSwitches")

        _versionName="UNDEFINED"
        _layout="UNDEFINED"
        _dbm = False

        if len(dbId)>0:
            key=dbId[0] 
            if "VERSIONNAME" in dbParam: _versionName = dbSwitches[key][dbParam.index("VERSIONNAME")]
            if "LAYOUT" in dbParam : _layout = dbSwitches[key][dbParam.index("LAYOUT")]
            if "BUILDDBM" in dbParam : _dbm = (dbSwitches[key][dbParam.index("BUILDDBM")] != 0)


        self.__dict__["VersionName"] = _versionName
        self.__dict__["Layout"] = _layout
        self.__dict__["DBM"] = _dbm


        # ----------------------------------------------------------------------------    
        # IBL layout

        dbId,dbLayers,dbParam = self.dbGeomCursor.GetCurrentLeafContent("PixelLayer")
        IBLStaveIndex = -1
        IBLgeoLayout = -1
        _IBLlayout = "noIBL"
        if len(dbId)>0:
            key=dbId[0] 
            if "STAVEINDEX" in dbParam and dbLayers[key][dbParam.index("STAVEINDEX")] not in ["NULL",None]:
                IBLStaveIndex = int(dbLayers[key][dbParam.index("STAVEINDEX")])

            if IBLStaveIndex>-1:
                dbId,dbStaves,dbParam = self.dbGeomCursor.GetCurrentLeafContent("PixelStave")

                if len(dbId)>0 and IBLStaveIndex<=len(dbStaves.keys()):
                    key=dbId[IBLStaveIndex] 
                    if "LAYOUT" in dbParam and dbStaves[key][dbParam.index("LAYOUT")] not in ["NULL",None]:
                        IBLgeoLayout = int(dbStaves[key][dbParam.index("LAYOUT")])
                        if IBLgeoLayout in [3,4] : _IBLlayout = "planar"
                        elif IBLgeoLayout in [5] : _IBLlayout = "3D"

        self.__dict__["IBLlayout"]=_IBLlayout

        # ----------------------------------------------------------------------------    
        # Read versionname, layout and dbm from AtlasCommon table

        dbId,dbCommon,dbParam = self.dbGeomCursor.GetCurrentLeafContent("AtlasCommon")

        _run = "UNDEFINED"
        _geotype = "UNDEFINED"
        if len(dbId)>0:
            key=dbId[0] 
            if "CONFIG" in dbParam : _run = dbCommon[key][dbParam.index("CONFIG")]
            if "GEOTYPE" in dbParam : _geotype = dbCommon[key][dbParam.index("GEOTYPE")]
        
        self.__dict__["Run"]=_run
        self.__dict__["GeoType"]=_geotype

        # ----------------------------------------------------------------------------    
        # IBL and SLHC parameters

        self.__dict__["IBL"] = False
        self.__dict__["SLHC"] = False
        if _layout in ['IBL'] : self.__dict__["IBL"] = True
        if self.__dict__["IBL"] == False: self.__dict__["IBLlayout"]="noIBL"
        if _layout not in ['SLHC'] and _run.lower()=="run2": self.__dict__["IBL"] = True
        if _layout in ['SLHC'] : self.__dict__["SLHC"] = True


    def getValue(self,name):

        return self.__dict__[name]

    def dump(self):

        print "Geometry tag InDetGMFlags : ",self.__dict__["geomTag"]," ------------------------------------"
        print "VersionName = ",self.__dict__["VersionName"]
        print "Layout      = ",self.__dict__["Layout"]
        print "DBM         = ",self.__dict__["DBM"]

        print "SLHC flag : ",self.__dict__["SLHC"]
        print "IBL flag   : ",self.__dict__["IBL"]
        print "IBL layout : ", self.__dict__["IBLlayout"]
        print "RUN flag      : ",self.__dict__["Run"]
        print "GeoType flag  : ",self.__dict__["GeoType"]



# -------------------------------------------------------------------------------------
#  Geometry jobproperties defined for the InDet geometry
# -------------------------------------------------------------------------------------

class GeoVersionName(JobProperty):
     """ version name DC3,IBL,SLHC,... """
     statusOn     = True
     allowedTypes = ['str']
     StoredValue  = 'UNDEFINED'

class GeoLayout(JobProperty):
     """ geometry layout Final, IBL. SLHC,... """
     statusOn     = True
     allowedTypes = ['str']
     StoredValue  = 'UNDEFINED'

class isIBL(JobProperty):
     """ IBL layer defined or not True/False"""
     statusOn     = True
     allowedTypes = ['bool']
     StoredValue  = False

class IBLLayout(JobProperty):
     """ IBL layer layout : noIBL, planar (all modules are planar),  3D (hybrid geometry) """
     statusOn     = True
     allowedTypes = ['str']
     allowedValues=["planar","3D","noIBL","UNDEFINED"]
     StoredValue  = "UNDEFINED"

class isSLHC(JobProperty):
     """ SLHC geometry"""
     statusOn     = True
     allowedTypes = ['bool']
     StoredValue  = False

class isDBM(JobProperty):
     """ DBM monitoring system defined or not"""
     statusOn     = True
     allowedTypes = ['bool']
     StoredValue  = False

class Run(JobProperty):
     """ RUN1/RUN2 parameter  """
     statusOn     = True
     allowedTypes = ['str']
     StoredValue  = "UNDEFINED"

class GeoType(JobProperty):
     """ Geometry type ( ITKLoI, ITkLoI-VF, etc...) """
     statusOn     = True
     allowedTypes = ['str']
     StoredValue  = "UNDEFINED"


# add to jobproperties
class GeometryFlags_JobProperties(JobPropertyContainer):
    """ The geometry flag/job property container """

    def __init__(self, context=""):
        JobPropertyContainer.__init__(self,context)
        return

    def setupValuesFromDB(self,geoTagName="none"):
        
        InDetGeoFlags = InDetGMFlags(geoTagName)

        self.isSLHC.set_Value_and_Lock( InDetGeoFlags.getValue("SLHC") )
        self.isIBL.set_Value_and_Lock(InDetGeoFlags.getValue("IBL"))
        self.isDBM.set_Value_and_Lock(InDetGeoFlags.getValue("DBM"))
        self.IBLLayout.set_Value_and_Lock(InDetGeoFlags.getValue("IBLlayout"))
        self.GeoVersionName.set_Value_and_Lock(InDetGeoFlags.getValue("VersionName"))
        self.GeoLayout.set_Value_and_Lock(InDetGeoFlags.getValue("Layout"))
        self.Run.set_Value_and_Lock(InDetGeoFlags.getValue("Run"))
        self.GeoType.set_Value_and_Lock(InDetGeoFlags.getValue("GeoType"))


    def reset(self,geoTagName="none"):
        
        self.isSLHC.unlock()
        self.isIBL.unlock()
        self.isDBM.unlock()
        self.IBLLayout.unlock()
        self.GeoVersionName.unlock()
        self.GeoLayout.unlock()
        self.Run.unlock()
        self.GeoType.unlock()


    def dump(self):

        print "VersionName = ", self.GeoVersionName()
        print "Layout      = ", self.GeoLayout()
        print "DBM         = ", self.isDBM()

        print "SLHC flag : ", self.isSLHC()
        print "IBL flag   : ", self.isIBL()
        print "IBL layout : ", self.IBLLayout()
        print "RUN flag      :", self.Run()
        print "GeoType flag  : ", self.GeoType()


jobproperties.add_Container(GeometryFlags_JobProperties)
jobproperties.GeometryFlags_JobProperties.add_JobProperty(GeoVersionName)
jobproperties.GeometryFlags_JobProperties.add_JobProperty(GeoLayout)
jobproperties.GeometryFlags_JobProperties.add_JobProperty(isIBL)
jobproperties.GeometryFlags_JobProperties.add_JobProperty(IBLLayout)
jobproperties.GeometryFlags_JobProperties.add_JobProperty(isSLHC)
jobproperties.GeometryFlags_JobProperties.add_JobProperty(isDBM)
jobproperties.GeometryFlags_JobProperties.add_JobProperty(Run)
jobproperties.GeometryFlags_JobProperties.add_JobProperty(GeoType)

GeometryFlags = jobproperties.GeometryFlags_JobProperties
GeometryFlags.setupValuesFromDB()
        



