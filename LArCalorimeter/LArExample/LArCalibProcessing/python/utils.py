# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from PyCool import cool

_globaltag=""

class FolderTagResolver:
    def __init__(self,dbname="COOLOFL_LAR/CONDBR2"):
        dbSvc = cool.DatabaseSvcFactory.databaseService()
        self._db = dbSvc.openDatabase(dbname)
        return

    def __del__(self):
        self._db.closeDatabase()
        return

    def getFolderTag(self,foldername,globalTag=_globaltag):
        try:
          folder=self._db.getFolder(foldername)
          return folder.resolveTag(globalTag)
        except Exception:
          # new folder, should "create a tag"
          return ''.join(foldername.split('/')) + '-UPD3-00'
          
            
    def getFolderTagSuffix(self,foldername,globalTag=_globaltag):
        ft=self.getFolderTag(foldername,globalTag)
        p=ft.find("-")
        if p==-1:
            return "-Default"
        else: 
            return ft[p:]
