# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
#dbString = "sqlite://;schema=COOLTEST.db;dbname=COMP200" 
#dbString = "sqlite://;schema=COOLTEST.db;dbname=COMP200"
dbString = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=MONP200;"

dbEFFolderName = "/TRIGGER/HLT/EFRates"
dbL2FolderName = "/TRIGGER/HLT/L2Rates"
dbTRFolderName = "/TRIGGER/HLT/TotalRates"

dbDesc = "<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>"
#----------------------------------------------------------------------------------------------------
def database_create():
    
    from PyCool import cool
    try:
        cool.DatabaseSvcFactory.databaseService().dropDatabase(dbString)
    except Exception as e:
        print str(e)
    db = cool.DatabaseSvcFactory.databaseService().createDatabase(dbString)
    

    def make_containing_folder(folderName):
        subfolders = folderName.split("/")
        subfolders.pop(0)
        subfolders.pop(len(subfolders)-1)
        newFolder = "/" + "/".join(subfolders)
        if len(subfolders) > 1:
            make_containing_folder(newFolder)
        
        if not db.existsFolderSet(newFolder):
            db.createFolderSet(newFolder)
    
    make_containing_folder(dbL2FolderName)
    make_containing_folder(dbEFFolderName)
    make_containing_folder(dbTRFolderName)
    
    
    folderSpecs = cool.RecordSpecification()
    
    folderSpecs.extend("Input", cool.StorageType.Float)
    folderSpecs.extend("Prescale", cool.StorageType.Float)
    folderSpecs.extend("Raw", cool.StorageType.Float)
    folderSpecs.extend("Output", cool.StorageType.Float)
    
    for folderName in [dbEFFolderName, dbL2FolderName]:    
        db.createFolder(folderName, folderSpecs, dbDesc, cool.FolderVersioning.MULTI_VERSION)

    
    folderSpecs = cool.RecordSpecification()
    
    folderSpecs.extend("Input", cool.StorageType.Float)
    folderSpecs.extend("Output", cool.StorageType.Float)
        
    db.createFolder(dbTRFolderName, folderSpecs, dbDesc, cool.FolderVersioning.MULTI_VERSION)

#----------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    database_create()
#----------------------------------------------------------------------------------------------------