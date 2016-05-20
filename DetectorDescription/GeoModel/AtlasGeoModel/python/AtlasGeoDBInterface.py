# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import coral
import os

# -------------------------------------------------------------------------------------
#  AtlasGeoDBInterface : Interface with the geometry DB
# -------------------------------------------------------------------------------------

class AtlasGeoDBInterface:

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

        # Check that the FRONTIER_SERVER is set properly, if not reduce the retrial period and time out values
        if not ('FRONTIER_SERVER' in os.environ and os.environ['FRONTIER_SERVER']):
            svcconfig.setConnectionRetrialPeriod(1)
            svcconfig.setConnectionRetrialTimeOut(1)
        else:
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

        # Check if the geometry tag was found in the DB
        if self.dbGeoTagId=="":
            print "The geometry tag ",self.dbGeoTag," could not be found in the database."
            print "Its name might be misspelled and/or the script might access a local DB that is not up to date."
            print ""
            import sys
            sys.exit();

        # Get node ids for the geometry tag
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


