# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from logging import getLogger; log = getLogger("DQDefects.folders")

from DQUtils import Databases

from DQDefects import (DEFECTS_FOLDER, DEFECT_FOLDER_DESC, DEFECT_LOGIC_FOLDER, 
                       DEFECT_LOGIC_FOLDER_DESC, PARENT_FOLDERSET)

from PyCool import cool

from typing import Tuple

ST = cool.StorageType
FV = cool.FolderVersioning

class DefectsDBFoldersMixin(object):
    """
    This mixin contains code for managing the defect folders
    """
    
    def __init__(self) -> None:
        self._defects_folder = None
        self._defect_logic_folder = None
        self._parent_folderset = None
        self._connections = []
        super(DefectsDBFoldersMixin, self).__init__()
        
    def _clear_connections(self) -> None:
        log.debug("DefectsDB connections cleared")
        for connection in self._connections:
            connection.closeDatabase()
        self._connections = []
    
    @property
    def parent_folderset(self) -> cool.IFolderSet:
        """
        Lazy-loads parent folderset (useful for heirarchical tag manipulation)
        """
        if self._parent_folderset is None:
            db = Databases.get_instance(self.connection_string, 
                                        read_only=self._read_only)
            self._parent_folderset = db.getFolderSet(PARENT_FOLDERSET)
        return self._parent_folderset
    
    def _load_folder(self, folder, create_function) -> Tuple[cool.IFolder, cool.Record]:
        """
        Internal function used to load a COOL folder
        """
        db_folder_string = "%s::%s" % (self.connection_string, folder)
    
        if not self._create:
            create_function = None
    
        db, folder = Databases.get_folder(db_folder_string, 
                                          read_only=self._read_only,
                                          create_function=create_function,
                                          also_db=True)
        
        self._connections.append(db)
        
        payload = cool.Record(folder.payloadSpecification())
        
        return folder, payload
        
    @property
    def defects_folder(self) -> cool.IFolder:
        """
        Returns the folder containing the defects, loading it if necessary
        """
        if self._defects_folder is None: self._load_defects_folder()
        return self._defects_folder
    
    def _load_defects_folder(self) -> None:
        """
        Internal function for populating the self._defects_folder variable
        """
        log.debug("Loading defects folder")
        res = self._load_folder(DEFECTS_FOLDER, self._create_defects_folder)
        self._defects_folder, self._defect_payload = res
    
    def _create_defects_folder(self, db: cool.IDatabase) -> cool.IFolder:
        """
        Creates the COOL database/folder for defects, if they don't exist.
        Internal - use create=True in the constructor to create the COOL folder.
        """
        log.info("Creating defects folder")
        
        record_spec = cool.RecordSpecification()
        record_spec.extend("present",     ST.Bool)
        record_spec.extend("recoverable", ST.Bool)
        record_spec.extend("user",        ST.String255)
        record_spec.extend("comment",     ST.String255)
                
        folder_spec = cool.FolderSpecification(FV.MULTI_VERSION, record_spec)
        return db.createFolder(DEFECTS_FOLDER, folder_spec, 
                                DEFECT_FOLDER_DESC, True)
                                
    @property
    def defect_logic_folder(self) -> cool.IFolder:
        """
        Returns the folder containing the virtual defect logic, loading it if necessary
        """
        if self._defect_logic_folder is None: self._load_defect_logic_folder()
        return self._defect_logic_folder
    
    def _load_defect_logic_folder(self) -> None:
        """
        Internal function for populating the self._defect_logic_folder variable
        """        
        res = self._load_folder(DEFECT_LOGIC_FOLDER, 
                                self._create_defect_logic_folder)
        self._defect_logic_folder, self._defect_logic_payload = res
        
    def _create_defect_logic_folder(self, db: cool.IDatabase) -> cool.IFolder:
        """
        Creates the COOL database/folder for virtual defect logic, if they don't exist.
        Internal - use create=True in the constructor to create the COOL folder.
        """
        log.info("Creating defect logic folder")
        
        record_spec = cool.RecordSpecification()
        record_spec.extend("clauses", ST.String4k)
        
        folder_spec = cool.FolderSpecification(FV.MULTI_VERSION, record_spec)
        return db.createFolder(DEFECT_LOGIC_FOLDER, folder_spec, 
                               DEFECT_LOGIC_FOLDER_DESC, True)
