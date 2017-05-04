# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
from PyCool import cool
#----------------------------------------------------------------------------------------------------
from model.basics.DataSelection import DataSelection 
#----------------------------------------------------------------------------------------------------
class COOLQuery(object):
    def __init__(self):
        self.SetChannelSelction(DataSelection())
        self.SetIoVSelection(DataSelection())
        
        self.__db = {}
        self.__dbMode = {}
        self.__svc = cool.DatabaseSvcFactory.databaseService()
        pass
    
    
    def GetSvcFactory(self): return self.__svc
             
        
    def GetDatabaseHandle(self, connectionString, readOnly = True):
        if self.__db.has_key(connectionString):
            if self.__dbMode[connectionString] == readOnly:
                return self.__db[connectionString]
            else:
                self.CloseConnection(connectionString)
                self.GetDatabaseHandle(connectionString, readOnly)
        else:
            self.__db[connectionString] = self.GetSvcFactory().openDatabase(connectionString, readOnly)
            self.__dbMode[connectionString] = readOnly
            return self.GetDatabaseHandle(connectionString, readOnly)

    def GetFolderHandle(self, folderName, connectionString = None, readOnly = True):
        if connectionString == None:
            connectionString = self.GetConnectionString(folderName) 
        return self.GetDatabaseHandle(connectionString, readOnly).getFolder(folderName)
    
    def GetConnectionString(self, folderName):
        if "/LHC/DCS/" in folderName:
            return "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200;"
        
        if folderName[0] != "/":
            folderName = "/" + folderName
        schema = folderName.split("/")[1]
        return "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_" + schema + ";dbname=COMP200;"        
    
    def CloseConnection(self, connectionString = None):
        if connectionString == None:
            for conStr in self.__db.keys():
                self.CloseConnection(conStr)
        else:
            self.__db[connectionString].closeDatabase()
            self.__db.pop(connectionString)
             
    
    def GetChannelSelction(self): return self.__channelSel
    def SetChannelSelction(self, val): self.__channelSel = val
    
    def GetCOOLChannelSelection(self):
        sel = None
        for channelRange in self.GetChannelSelction().GetListOfRanges():
            if sel == None:
                sel = cool.ChannelSelection(channelRange.GetFirst(),channelRange.GetLast())
            else:
                if channelRange.IsSingle():
                    sel.addChannel(channelRange.GetFirst())
                else:
                    sel.addRange(channelRange.GetFirst(), channelRange.GetLast())
        
        if sel == None:
            sel = cool.ChannelSelection()
        
        return sel
    
    def SelectSingleChannel(self, val): self.SetChannelSelction(DataSelection(val))
    def SetIoV(self, a = None, b = None): self.SetIoVSelection(DataSelection(a,b))
    
    def AddIoV(self, a = None, b = None): self.SetIoVSelection(self.GetChannelSelction().Union(DataSelection(a,b)))
    def RemoveIoV(self, a = None, b = None): self.SetIoVSelection(self.GetChannelSelction().Complement(DataSelection(a,b)))
    
    def AddChannelRange(self, a ,b = None):
        if b == None: b = a
        self.SetChannelSelction(self.GetChannelSelction().Union(DataSelection(a,b)))
    
    def RemoveChannelRange(self, a ,b = None):
        if b == None: b = a
        self.SetChannelSelction(self.GetChannelSelction().Complement(DataSelection(a,b)))
            
    def GetIoVSelection(self): return self.__iovSel
    def SetIoVSelection(self, val): self.__iovSel = val

    
    def GetFolder(self, folderName, connectionString = None):
        readOnly = True
        if connectionString in self.__db.keys():
            readOnly = self.__dbMode[connectionString]
            
        selection = self.GetCOOLChannelSelection() 
        
        if connectionString == None:
            connectionString = self.GetConnectionString(folderName)
        
        folder = self.GetFolderHandle(folderName, connectionString, readOnly)
    
        objects = []
        for IoV in self.GetIoVSelection().GetListOfRanges():
            for obj in list(folder.browseObjects(int(IoV.GetFirst()), int(IoV.GetLast()), selection)):
                objects.append((obj,obj.payload()))
        
        return objects
#----------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    from model.COOL.COOLTime import COOLTime
    q = COOLQuery()
    q.SetIoVSelection(DataSelection(*COOLTime().Run(189184)))
    for obj, payload in q.GetFolder("/TRIGGER/HLT/Menu"):
        print payload["ChainName"]
        
    """
    dbString = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=MONP200;"
    q = COOLQuery()
    print "Pass"
    f = q.GetFolderHandle("/TRIGGER/HLT/TotalRates", dbString, True)
    print "Pass"
    f.createChannel(10, "asdasd")
    print "Pass"
    """