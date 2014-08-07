# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class TmpClass():
    """dynamically created output."""
    def __init__(self,key='userOutputWasUndefined'):
        keySuffix=key[len('userOutput'):]
        self.StreamName = 'StreamNTUP_'+keySuffix
        self.TreeNames = ['']
        self.trfKeyName=key
        return


class OutputsMgr():
    def __init__(self):
        self.userOutputList=[]
        return
    
    def addTrfOutput(self,key):
        from PATJobTransforms.Configuration import AddUserDPD,ConfigDic
        if ConfigDic.has_key(key):
            raise RuntimeError("key %s already exists, cannot be redefined"%key)
        tmpClass=TmpClass(key)
        AddUserDPD(tmpClass, ['e2d','a2d'])
        self.addOutput(key,'defaultFileName.root')
        return

    def addOutput(self,key,defaultValue):
        setattr(self,key,defaultValue)
        self.userOutputList.append(key)
        return

    def getFileName(self,dataTypeKey,defaultName):
        if self.userOutputList.__contains__(dataTypeKey):
            return getattr(self,dataTypeKey)

        self.addOutput(dataTypeKey,defaultName)
        return defaultName
    
outputsMgr = OutputsMgr()
magicKey='userOutput'
