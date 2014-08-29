# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# AtlCoolSchema.py defines the COOL schema in use in ATLAS production databases
# Richard Hawkings 09/9/07
# stripped down version for sw release - no write cn or AtlCoolCopy information

class coolSchema:
    "Definition of a COOL schema"
    def __init__(self,schema,instance,extra=0):
        """initialise from a schema ('ATLAS_COOLONL_INDET')
        and instance ('OFLP200')"""
        x1=schema.index('_')
        x2=schema.index('_',x1+1)
        self._schema=schema
        self._logic=schema[x1+1:]
        self._detector=schema[x2+1:]
        self._instance=instance
        self._extra=extra

    def schema(self):
        "Return full database schema name"
        return self._schema

    def logical(self):
        "Return logical (readonly) connection name"
        return self._logic+'/'+self._instance

    def instance(self):
        "Return database instance name"
        return self._instance

    def detector(self):
        "Return detector name"
        return self._detector

    def offline(self):
        "Returns true if the connection is to an offline DB"
        return (self._schema.find('ONL')<0)
            

def instanceList(instanceSel="",schemaSel=""):
    "Define all instances in use, optionally only selected instances/schemas"
    ilist=[]
    # list of all the active databases
    if (instanceSel=='OFLP200' or instanceSel==''):
        addSchema(ilist,'ATLAS_COOLOFL_INDET','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_INDET','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_PIXEL','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_PIXEL','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_SCT','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_SCT','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_TRT','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_TRT','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_LAR','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_LAR','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_TILE','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_TILE','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_CALO','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_CALO','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_MDT','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_MDT','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_MUONALIGN','OFLP200',schemaSel,2)
        addSchema(ilist,'ATLAS_COOLONL_MUONALIGN','OFLP200',schemaSel,2)
        addSchema(ilist,'ATLAS_COOLOFL_CSC','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_CSC','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_TRIGGER','OFLP200',schemaSel,1)
        addSchema(ilist,'ATLAS_COOLOFL_RPC','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_TGC','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_GLOBAL','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_GLOBAL','OFLP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_DCS','OFLP200',schemaSel)
    if (instanceSel=='COMP200' or instanceSel==''):
        addSchema(ilist,'ATLAS_COOLOFL_INDET','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_INDET','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_PIXEL','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_PIXEL','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_SCT','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_SCT','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_TRT','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_TRT','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_LAR','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_LAR','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_TILE','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_TILE','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_CALO','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_CALO','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_MDT','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_MDT','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_MUONALIGN','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_MUONALIGN','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_TDAQ','COMP200',schemaSel,1)
        addSchema(ilist,'ATLAS_COOLONL_TRIGGER','COMP200',schemaSel,3)
        addSchema(ilist,'ATLAS_COOLOFL_TRIGGER','COMP200',schemaSel,3)
        addSchema(ilist,'ATLAS_COOLOFL_CSC','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_CSC','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_RPC','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_RPC','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_TGC','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_GLOBAL','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_GLOBAL','COMP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_DCS','COMP200',schemaSel,3)
        addSchema(ilist,'ATLAS_COOLONL_FWD','COMP200',schemaSel)
    if (instanceSel=='CONDBR2' or instanceSel==''):
        addSchema(ilist,'ATLAS_COOLOFL_INDET','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_INDET','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_PIXEL','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_PIXEL','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_SCT','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_SCT','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_TRT','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_TRT','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_LAR','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_LAR','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_TILE','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_TILE','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_CALO','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_CALO','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_MDT','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_MDT','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_MUONALIGN','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_MUONALIGN','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_TDAQ','CONDBR2',schemaSel,1)
        addSchema(ilist,'ATLAS_COOLONL_TRIGGER','CONDBR2',schemaSel,3)
        addSchema(ilist,'ATLAS_COOLOFL_TRIGGER','CONDBR2',schemaSel,3)
        addSchema(ilist,'ATLAS_COOLOFL_CSC','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_CSC','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_RPC','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_RPC','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_TGC','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_GLOBAL','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_GLOBAL','CONDBR2',schemaSel)
        addSchema(ilist,'ATLAS_COOLOFL_DCS','CONDBR2',schemaSel,3)
        addSchema(ilist,'ATLAS_COOLONL_FWD','CONDBR2',schemaSel)
    if (instanceSel=='CMCP200' or instanceSel==''):
        addSchema(ilist,'ATLAS_COOLONL_INDET','CMCP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_PIXEL','CMCP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_SCT','CMCP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_TRT','CMCP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_LAR','CMCP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_TILE','CMCP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_CALO','CMCP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_CSC','CMCP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_GLOBAL','CMCP200',schemaSel)
    if (instanceSel=='TMCP200' or instanceSel==''):
        addSchema(ilist,'ATLAS_COOLONL_LAR','TMCP200',schemaSel)
        addSchema(ilist,'ATLAS_COOLONL_CALO','TMCP200',schemaSel)
    return ilist
        
def addSchema(ilist,schema,instance,schemaSel,extra=0):
    if (schemaSel=='' or schema.find(schemaSel)>=0):
        ilist+=[coolSchema(schema,instance,extra)]
