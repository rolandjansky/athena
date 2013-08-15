# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 12, 2011

@author: ggalster
'''
from PyCool import cool
import re


from model.COOL.COOLTime import COOLTime

class COOLquery():        
    def __init__(self):
        print "* * * * * * * * * * * * * * * * * * * * * * * *"
        print "*       !!! DEPRECATED OBJECT !!!             *"
        print "* * * * * * * * * * * * * * * * * * * * * * * *"
        print "*                                             *"
        print "*  This object has been deprecated in favour  *"
        print "*        of model.COOL.COOLQuery              *"
        print "*                                             *"        
        print "* * * * * * * * * * * * * * * * * * * * * * * *"
        print "* http://www.youtube.com/watch?v=2Z4m4lnjxkY  *"
        print "* * * * * * * * * * * * * * * * * * * * * * * *"                
        self.__dbSvc = cool.DatabaseSvcFactory.databaseService()

        self.__db = {}
        self.__startIOV = COOLTime()
        self.__stopIOV = COOLTime()

        self.SetChannelSelection()
        
                

    def GetIOVStart(self): return self.__startIOV
    def SetIOVStart(self, val): self.__startIOV = val
        
    def GetIOVStop(self): return self.__stopIOV
    def SetIOVStop(self, val): self.__stopIOV = val
                    
    def __del__(self):
        self.CloseConnection()
    
    def CloseConnection(self, name = None):
        if name == None:
            for conname in self.__db.keys():
                self.CloseConnection(conname)
        else:
            if name in self.__db.keys():
                self.__db[name].closeDatabase()
                self.__db.pop(name)
    
    def GetSvcFactory(self): return self.__dbSvc
            
    def OpenConnection(self, name):
        name = name.upper()
        if not name in self.__db.keys():
            self.__db[name] = self.__dbSvc.openDatabase(self.GetConncetionString(name), False)
    
    
    __selection = None
    def GetChannelSelection(self): return self.__selection
    def SetChannelSelection(self, a = None, b = None):
        if a == None:
            self.__selection = cool.ChannelSelection.all()
        else:
            if not b == None:
                self.__selection = cool.ChannelSelection(a, b)
            else:
                self.__selection = cool.ChannelSelection(a)
    
    
    def GetConncetionString(self, schema):
        return \
            "oracle://ATLAS_COOLPROD;" + \
            "schema=ATLAS_COOLONL_" + str(schema) + ";" + \
            "dbname=COMP200;"
            
    def GetFolder(self, folderName):
        if folderName[0] != "/":
            folderName = "/" + folderName
            
        match = re.match("^/([^/]*)", folderName)
        
        dbName = match.group(1)
        self.OpenConnection(dbName)
        
        folder = self.__db[dbName].getFolder(folderName)
    
        objects = folder.browseObjects(self.GetIOVStart().GetCOOLTime(), self.GetIOVStop().GetCOOLTime(), self.GetChannelSelection())
        
        result = []
        for obj in objects:
            
            result.append(obj)
            
        return result
    
        
#    query = prettyCOOL()
#    query.SetRunRange(run, run)
#    for obj in query.GetFolder("/TDAQ/RunCtrl/DataTakingMode"):
#        pl = obj.payload()
#        if pl["ReadyForPhysics"] == 1:
#            print "RUN: ",obj.since() & 0xFFFF, obj.until() & 0xFFFF
#            query.SetLumiBlockRange(obj.since() & 0xFFFF, obj.until() & 0xFFFF)
#    
#    
#    for obj in query.GetFolder("/TRIGGER/LVL1/Lvl1ConfigKey"):
#        print obj.since() & 0xFFFF, (obj.until() & 0xFFFF)-1



if __name__ == "__main__":
    print COOLquery()