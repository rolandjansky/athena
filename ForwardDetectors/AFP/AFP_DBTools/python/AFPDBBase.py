#!/bin/env python
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from PyCool import cool

class AFPDBRecordBase(object):
    def serialiseValues (self):
        output = "["
        for quantity in dir(self):        # loop over all elements in class
            if not callable(getattr(self, quantity)) and not quantity.startswith("__"): # select only user variables 
                # print ("Moj " + quantity + " wynosi: " +  str(getattr(self, quantity)))
                output += str(getattr(self, quantity)) + ", "

        output = output[:-2] # remove the last comma and space ", "
        output += "]"        # close bracket

        return output

    def serialiseTypes (self):
        output = '"folder_payloadspec": "'
        for quantity in dir(self):        # loop over all elements in class
            if not callable(getattr(self, quantity)) and not quantity.startswith('__'): # select only user variables 
                output += quantity + ': ' + self.translateType(type(getattr(self, quantity))) + ', ' # append variable name and type

        output = output[:-2] # remove the last comma and space ", "
        output += '"'        # close parenthisis

        return output

    def translateType (self, typeName):
        if (typeName == int):
            return "Int32"
        elif (typeName == float):
            return "Float"
        elif (typeName == str):
            return "String"
        else:
            raise ValueError ("Unknown type of field, typeName is %s. Need to update method translateType in AFPDBBase.py" % typeName)


class AFPDBTableBase (object):
    def __init__ (self):
        self.records = []
        self.iovStartRun = 0
        self.iovStartLumiBlock = 0
        self.iovEndRun = 0
        self.iovEndLumiBlock = 0

        self.tag = "AFPTest-00-00-00"
        self.folderName = "/FWD/AFP/TEST"
        self.spec = cool.RecordSpecification()
#        self.spec.extend("data", cool.StorageType.Blob64k)
        self.spec.extend("data", cool.StorageType.String16M)
#        self.desc = '<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader><typeName>CondAttrListCollection</typeName>'
        self.desc = '<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="71" clid="40774348" /></addrHeader><typeName>AthenaAttributeList</typeName>'
        self.data = cool.Record(self.spec)
        self.folderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, self.spec)



    def serialiseHeader (self):
        output = '"node_description": '
#        output += '"<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader<typeName>CondAttrListCollection</typeName>"'
        output += '"<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\\"71\\" clid=\\"40774348\\" /></addrHeader><typeName>AthenaAttributeList</typeName> "'
        
        return output

    def serialiseRecords (self):
        output = '"data_array": ['
        if (len(self.records) > 0):
            channelID = 1

            for entry in self.records:
                output += '{'
                output += '"' + str(channelID) + '" : '
                output += entry.serialiseValues()
                output += '}, '
                channelID += 1

            # remove the last comma and space ", "
            output = output[:-2] 
            # close bracket
            output += ']'

        return output
            
    def serialiseTable (self):
        if (len(self.records) < 1):
            raise ValueError ("Empty records list. Please, fill records before serialising table.")

        output = '{'
        output += self.serialiseHeader() + ', '
        output += self.records[0].serialiseTypes() + ', '
        output += self.serialiseRecords()
        output += '}'
        
        return output

    def saveToDB (self):
        self.data['data'] = self.serialiseTable()
        iovStart=(self.iovStartRun<<32) + self.iovStartLumiBlock
        print ("before self.iovEndRun=" + str(self.iovEndRun))
        iovEnd=(self.iovEndRun<<32) + self.iovEndLumiBlock
        print ("after self.iovEndRun=" + str(self.iovEndRun))
        
        self.folder.storeObject(iovStart, iovEnd, self.data, 0, self.tag)

    def createOrGetFolder (self, db):
        print (self.folderSpec)
        if(db.existsFolder(self.folderName)):
            self.folder=db.getFolder(self.folderName)
        else:
            self.folder=db.createFolder(self.folderName, self.folderSpec, self.desc, True)


