#####################
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# 
# Author: Sami Kama 2017
#####################
# Python module to construct ptrees for ALP execution
# and provide ALP class for using from python side.
#
import os,sys
from lxml import etree as et

def recurseDict(d,rootName):
    r=et.Element(rootName)
    if isinstance(d,dict):
        for k in iter(d):
            val=d[k]
            if isinstance(val,(str,unicode)):
                et.SubElement(r,k).text=str(val)
            else:
                l=recurseDict(val,k)
                r.append(l)
    elif isinstance(d,(str,unicode)):
        r.text=d
    else:
        for val in d:
            r.append(recurseDict(val,rootName))
    return r

def recurseDictOld(d,rootName):
    r=et.Element(rootName)
    for k in iter(d):
        val=d[k]
        if isinstance(val,(str,unicode)):
            et.SubElement(r,k).text=str(val)
        else:
            l=recurseDict(val,k)
            r.append(l)
    return r

class DataSource:
    def __init__(self,library):
        self._library=library
        self._defaultDict={}

    def getLibrary(self):
        return self._library
    def getDefaultDict(self):
        return self._defaultDict
    def getTree(self):
        return None


class DCMDataSource(DataSource):
    def __init__(self,library="dfinterfaceDcm"):
        DataSource.__init__(self,library)
        self._defaultDict={
            # "HLTDFDCMBackend":{
                "UID" : "DataSource-is-DCM",
                "library" : str(self._library)
            # }
        }

    def getTree(self):
        return recurseDict({"HLTDFDCMBackend":self._defaultDict},"DataSource")

class FileDataSource(DataSource):
    def __init__(self,library="FileDataSource",
                 fileList=[],
                 outFile=None,
                 compressionFormat="ZLIB",
                 compressionLevel=2,
                 numEvents=-1,
                 loopFiles="false",
                 preload="false"):
        self._fileList=fileList
        DataSource.__init__(self,library)
        self._defaultDataDict={
            "UID" : "FileDS",
            "library" : str(self._library),
            "loopOverFiles":loopFiles,
            "start_id":1,
            "preload":preload,
            "numEvents":numEvents,
            "fileOffset":-1,
            "compressionLevel":compressionLevel,
            "compressionFormat":compressionFormat,
            "fileList":self._fileList
            }
        if outFile is not None:
            self._defaultDataDict["outputFileName"]=outFile
        self._defaultDict={
            "HLTFileDataSource":
                self._defaultDataDict
            }

    def getTree(self):
        root=et.Element("DataSource")
        ds=et.SubElement(root,"HLTFileDataSource")
        plainlist=[x for x in self._defaultDataDict.keys() if x is not "fileList" ]
        for k in plainlist:
            et.SubElement(ds,k).text=str(self._defaultDataDict[k])
        flist=et.SubElement(ds,"fileList")
        files=self._defaultDataDict["fileList"]
        for f in files:
            et.SubElement(flist,"file").text=str(f)
        return root

class DFFileDataSource(DataSource):
    def __init__(self,library="DFFileBackend",fileList=[]):
        self._fileList=fileList
        DataSource.__init__(self,library)
        self._defaultDataDict={
            "UID" : "DataSource-is-DCM",
            "library" : str(self._library),
            "loopOverFiles":"false",
            "start_id":1,
            "preload":"false",
            "numEvents":-1,
            "fileList":self._fileList
            }
        self._defaultDict={
            "HLTDFFileBackend":
                self._defaultDataDict
            }

    def getTree(self):
        root=et.Element("DataSource")
        ds=et.SubElement(root,"HLTDFFileBackend")
        plainlist=[x for x in self._defaultDataDict.keys() if x is not "fileList" ]
        for k in plainlist:
            et.SubElement(ds,k).text=str(self._defaultDataDict[k])
        flist=et.SubElement(ds,"fileList")
        files=self._defaultDataDict["fileList"]
        for f in files:
            et.SubElement(flist,"file").text=str(f)
        return root

class InfoService:
    def __init__(self,libraryName):
        self._library=libraryName
        self._defaultDict={}
    def getLibrary(self):
        return self._library
    def getDefaultDictionary(self):
        return self._defaultDict
    def getTree(self):
        return None

class MonSvcInfoService(InfoService):
    def __init__(self,libraryName="MonSvcInfoService",
                 OHServer="${TDAQ_OH_SERVER=Histogramming}",
                 OHSlots=1,
                 OHInterval=80,
                 OHRegex=".*",
                 ISServer="${TDAQ_IS_SERVER=DF}",
                 ISSlots=1,
                 ISInterval=5,
                 ISRegex=".*"
                 
    ):
        InfoService.__init__(self,libraryName)
        self._defaultDict={
            "UID":"hltMonSvc",
            "library":"MonSvcInfoService",
            "ConfigurationRules":[
                {
                    "UID":"HltpuConfigurationRuleBundle",
                    "Rules" : [
                        {
                            "UID":"HltpuOHRule",
                            "IncludeFilter":str(OHRegex),
                            "ExcludeFilter":"",
                            "Name":"Dumm",
                            "Parameters":{
                                "OHPublishingParameters":{
                                    "UID":"HltpuOHPublishingParameters",
                                    "PublishInterval":str(OHInterval),
                                    "OHServer":str(OHServer),
                                    "NumberOfSlots":str(OHSlots),
                                    "ROOTProvider":"${TDAQ_APPLICATION_NAME}"
                                }
                            }
                        },
                        {
                            "UID":"HltpuISRule",
                            "IncludeFilter":str(ISRegex),
                            "ExcludeFilter":"",
                            "Name":"DummDumm",
                            "Parameters":{
                                "ISPublishingParameters":{
                                    "UID":"HltpuISPublishingParameters",
                                    "PublishInterval":str(ISInterval),
                                    "NumberOfSlots":str(ISSlots),
                                    "ISServer":str(ISServer)
                                }
                            }
                        }
                    ]
                }
            ]
        }
    def getTree(self):
        root=et.Element("HLTMonInfoImpl")
        plainlist=[x for x in iter(self._defaultDict) if x is not "ConfigurationRules" ]
        for k in plainlist:
            et.SubElement(root,k).text=str(self._defaultDict[k])
        crl=et.SubElement(root,"ConfigurationRules")
        cr=self._defaultDict["ConfigurationRules"]
        for f in cr:
            crb=et.Element("ConfigurationRuleBundle")
            et.SubElement(crb,"UID").text=str(f["UID"])
            rules=et.SubElement(crb,"Rules")
            for r in f["Rules"]:
                #crn=et.SubElement(rules,"ConfigurationRule")
                crn=recurseDict(r,"ConfigurationRule")
                rules.append(crn)
            #et.SubElement(flist,"file").text=str(f)
            crl.append(crb)
        return root
        
class TriggerConfig:
    def __init__(self):
        self._defaultDict={}
    def getDefaultLibrary(self):
        return None
    def getDBConfig(self,SMK=0,coral=False,srv="LOCAL_HOST",port=3320,
                    user="ATLAS_CONF_TRIGGER_RUN2_R",pwd="TrigConfigRead2015",alias="TRIGGERDB"):
        TC={"TriggerDBConnection":
                {
                "UID":"TriggerDB_RUN2_CoralServer_Example",
                "Server":"%s"%(srv),
                "Port":"%s"%(port),
                "Name":"'&oracle://ATLAS_CONFIG/ATLAS_CONF_TRIGGER_RUN2'",
                "Alias":str(alias),
                "User":"%s"%(user),
                "Password":"%s"%(pwd),
                "Type":"Coral",
                "SuperMasterKey":"%s"%(SMK)
                }
            }

        if coral:
            defaultConns= [
                {
                    "UID": "ATLAS_COOLONL_INDET_CORALSRV",
                    "Server": "%s"%(srv),
                    "Port" :"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLONL_INDET'",
                    "Alias": "COOLONL_INDET",
                    "User": "''",
                    "Password": "''",
                    "Type": "Coral"
                    },
                {
                    "UID":"ATLAS_COOLONL_MDT_CORALSRV",
                    "Server": "%s"%(srv),
                    "Port": "%s"%(port),
                    "Name": "'&oracle://ATLAS_COOLPROD/ATLAS_COOLONL_MDT'",
                    "Alias":"COOLONL_MDT",
                    "User": "''",
                    "Password" :"''",
                    "Type": "Coral"
                    },
                {
                    "UID":"ATLAS_COOLONL_SCT_CORALSRV",
                    "Server": "%s"%(srv),
                    "Port": "%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLONL_SCT'",
                    "Alias":"COOLONL_SCT",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLOFL_TRT_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLOFL_TRT'",
                    "Alias":"COOLOFL_TRT",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLONL_RPC_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLONL_RPC'",
                    "Alias":"COOLONL_RPC",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLONL_TDAQ_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLONL_TDAQ'",
                    "Alias":"COOLONL_TDAQ",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLONL_MUONALIGN_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLONL_MUONALIGN'",
                    "Alias":"COOLONL_MUONALIGN",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLONL_LAR_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLONL_LAR'",
                    "Alias":"COOLONL_LAR",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLASDD_CORALSRV_THROUGHATLASDD",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_DD/ATLASDD'",
                    "Alias":"ATLASDD",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLONL_PIXEL_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLONL_PIXEL'",
                    "Alias":"COOLONL_PIXEL",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLOFL_MDT_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLOFL_MDT'",
                    "Alias":"COOLOFL_MDT",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLONL_CALO_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLONL_CALO'",
                    "Alias":"COOLONL_CALO",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLONL_CSC_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLONL_CSC'",
                    "Alias":"COOLONL_CSC",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLONL_TRT_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLONL_TRT'",
                    "Alias":"COOLONL_TRT",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOL_GLOBAL_ORACLE",
                    "Server":"ATLAS_COOLPROD",
                    "Port":"''",
                    "Name":"ATLAS_COOLONL_GLOBAL",
                    "Alias":"COOLONL_GLOBAL",
                    "User":"ATLAS_COOL_READER_U",
                    "Password":"LMXTPRO4RED",
                    "Type":"Oracle"
                    },
                {
                    "UID":"ATLAS_COOLONL_GLOBAL_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLONL_GLOBAL'",
                    "Alias":"COOLONL_GLOBAL",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLOFL_PIXEL_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLOFL_PIXEL'",
                    "Alias":"COOLOFL_PIXEL",
                    "User":"ATLAS_COOL_PIXEL",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLOFL_TILE_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLOFL_TILE'",
                    "Alias":"COOLOFL_TILE",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLOFL_INDET_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLOFL_INDET'",
                    "Alias":"COOLOFL_INDET",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLONL_TRIGGER_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLONL_TRIGGER'",
                    "Alias":"COOLONL_TRIGGER",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLOFL_CSC_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLOFL_CSC'",
                    "Alias":"COOLOFL_CSC",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLOFL_SCT_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLOFL_SCT'",
                    "Alias":"COOLOFL_SCT",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLOFL_LAR_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLOFL_LAR'",
                    "Alias":"COOLOFL_LAR",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLASDD_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://atlas_dd/atlasdd'",
                    "Alias":"ATLASDD",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLONL_TGC_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLONL_TGC'",
                    "Alias":"COOLONL_TGC",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLOFL_DCS_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLOFL_DCS'",
                    "Alias":"COOLOFL_DCS",
                    "User":"ATLAS_COOL_DCS",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLONL_MUON_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLONL_MUON'",
                    "Alias":"COOLONL_MUON",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLOFL_GLOBAL_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLOFL_GLOBAL'",
                    "Alias":"COOLOFL_GLOBAL",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    },
                {
                    "UID":"ATLAS_COOLONL_TILE_CORALSRV",
                    "Server":"%s"%(srv),
                    "Port":"%s"%(port),
                    "Name":"'&oracle://ATLAS_COOLPROD/ATLAS_COOLONL_TILE'",
                    "Alias":"COOLONL_TILE",
                    "User":"''",
                    "Password":"''",
                    "Type":"Coral"
                    }
                ]
            
            root=et.Element("DBConnections")
            for dbc in defaultConns:
                c=et.SubElement(root,"DBConnection")
                for k in dbc.keys():
                    et.SubElement(c,str(k)).text=str(dbc[k])
            return (root,recurseDict(TC,"TriggerDBConnection"))
        return (None,recurseDict(TC,"TriggerDBConnection"))
            
    def getTree(self):
        return  recurseDict(self._defaultDict,"TriggerConfiguration")
        #return None

class TriggerConfigJO(TriggerConfig):
    def __init__(self,jopath,SMK=0,prescaleKey=0,bunchKey=0,L1MenuFrom="DB"):
        TriggerConfig.__init__(self)
        self._defaultDict={
            "TriggerConfiguration":
                {
                "UID": "JobOptionsTriggerConfig-1",
                "L1TriggerConfiguration":
                    {
                    "L1TriggerConfiguration":
                        {
                        "UID":"L1TrigConf",
                        "Lvl1PrescaleKey":"%s"%(prescaleKey),
                        "Lvl1BunchGroupKey": "%s"%(bunchKey),
                        "ConfigureLvl1MenuFrom":"%s"%(L1MenuFrom)
                        }
                    },
                "TriggerDBConnection":{
                    "TriggerDBConnection":{
                        "Type":"Coral",
                        "Server":"TRIGGERDB",
                        "SuperMasterKey":"%s"%SMK,
                        "User":"",
                        "Password":"",
                        "Name":"dummy",
                        "Alias":"TRIGGERDB"
                        }
                    },
                "hlt":
                    {
                    "HLTImplementationJobOptions":
                        {
                        "UID": "HLTImplementationJobOptions-1",
                        "libraries":{
                            "library":["TrigServices",
                                       "TrigPSC",
                                   ]
                        },
                        "jobOptionsPath":"%s"%(jopath),
                        "evtSel":"NONE",
                        "pythonSetupFile": "TrigPSC/TrigPSCPythonSetup.py",
                        "showInclude": "false",
                        "logLevels":{
                            "logLevel":[
                                "INFO",
                                "ERROR"
                            ]
                        },
                        "tracePattern": "",
                        "jobOptionsType": "NONE",
                        "HLTCommonParameters":
                            {
                            "HLTCommonParameters":
                                {
                                "messageSvcType": "TrigMessageSvc",
                                "jobOptionsSvcType" :"JobOptionsSvc",
                                "dllName" :'',
                                "factoryName": ''
                                }
                            }
                        }
                    }
                }
            }

class TriggerConfigDBPython(TriggerConfig):
    def __init__(self,SMK=0,L1PSK=0,L1BG=0,HPSK=0,Coral=False,preCmds=[],postCmds=[],DBAlias="TRIGGERDB"):
        TriggerConfig.__init__(self)
        self.__SMK=SMK
        self.__useCoral=Coral
        self.__precmds=None
        self.__postcmds=None
        self.__DBAlias=DBAlias
        if preCmds is not None:
            if isinstance(preCmds,list):
                self.__precmds=preCmds
            else:
                self.__precmds=[preCmds]
        if postCmds is not None:
            if isinstance(postCmds,list):
                self.__postcmds=postCmds
            else:
                self.__postcmds=[postCmds]

        self._defaultDict={ "TriggerConfiguration":
                               {
                "UID":"DBTriggerConfig-1",
                "L1TriggerConfiguration":
                    {
                    "L1TriggerConfiguration":
                        {
                        "UID":"L1TrigConf",
                        "Lvl1PrescaleKey":"%s"%(L1PSK),
                        "Lvl1BunchGroupKey":"%s"%(L1BG),
                        "ConfigureLvl1MenuFrom":"DB",
                        }
                    },
                "hlt":
                    {
                    "HLTImplementationDBPython":
                        {
                        "UID":"HLTImplementationDBPython-1",
                        "libraries":
                            { 
                                "library":
                                [
                                    "TrigServices",
                                    "TrigPSC",
                                    "TrigConfigSvc"
                                ],
                            },
                        "hltPrescaleKey":"%s"%(HPSK),
                        "HLTCommonParameters":
                            {
                            "HLTCommonParameters":
                                {
                                "messageSvcType":"TrigMessageSvc",
                                "jobOptionsSvcType":"TrigConf::HLTJobOptionsSvc",
                                "dllName":"",
                                "factoryName":""
                                }
                            }
                        }
                    }
                }
                        }
    def getTree(self):
        TC=recurseDict(self._defaultDict,"TriggerConfiguration")
        hlt=TC.find("TriggerConfiguration").find("hlt").find("HLTImplementationDBPython")
        if self.__precmds is not None and len(self.__precmds):
            pcr=et.Element("preCommands")
            for pc in self.__precmds:
                if len(pc):
                    et.SubElement(pcr,"preCommand").text=str(pc)
            hlt.append(pcr)
        if self.__postcmds is not None and  len(self.__postcmds):
            pcr=et.Element("postCommands")
            for pc in self.__postcmds:
                if len(pc):
                    et.SubElement(pcr,"postCommand").text=str(pc)
            hlt.append(pcr)
        dbc=TriggerConfig.getDBConfig(self,self.__SMK,coral=self.__useCoral,alias=self.__DBAlias)
        if dbc[0] is not None:
            TC.find("TriggerConfiguration").append(dbc[0])
        if dbc[1] is not None:
            TC.find("TriggerConfiguration").append(dbc[1])
        return TC

class TriggerConfigDB(TriggerConfig):
    def __init__(self,SMK=0,L1PSK=0,L1BG=0,HPSK=0,Coral=False,DBAlias="TRIGGERDB"):
        TriggerConfig.__init__(self)
        self.__SMK=SMK
        self.__useCoral=Coral
        self.__DBAlias=DBAlias
        self._defaultDict={ "TriggerConfiguration":
                               {
                "UID":"DBTriggerConfig-1",
                "L1TriggerConfiguration":
                    {
                    "L1TriggerConfiguration":
                        {
                        "UID":"L1TrigConf",
                        "Lvl1PrescaleKey":"%s"%(L1PSK),
                        "Lvl1BunchGroupKey":"%s"%(L1BG),
                        "ConfigureLvl1MenuFrom":"DB",
                        }
                    },
                "hlt":
                    {
                    "HLTImplementationDB":
                        {
                        "UID":"HLTImplementationDB-1",
                        "libraries":{
                            "library":
                            [
                                "TrigServices",
                                "TrigPSC",
                                "TrigConfigSvc"
                            ],
                        },
                        "hltPrescaleKey":"%s"%(HPSK),
                        "HLTCommonParameters":
                            {
                            "HLTCommonParameters":
                                {
                                "messageSvcType":"TrigMessageSvc",
                                "jobOptionsSvcType":"TrigConf::HLTJobOptionsSvc",
                                "dllName":"",
                                "factoryName":""
                                }
                            }
                        }
                    }
                }
                            }

    def getTree(self):
        TC=recurseDict(self._defaultDict,"TriggerConfiguration")
        #print self.__DBAlias
        dbc=TriggerConfig.getDBConfig(self,self.__SMK,coral=self.__useCoral,alias=self.__DBAlias)
        if dbc[0] is not None:
            TC.find("TriggerConfiguration").append(dbc[0])
        if dbc[1] is not None:
            TC.find("TriggerConfiguration").append(dbc[1])
        return TC
                           
class TriggerConfigAthenaHLT:
    def __init__(self,args):
        from HLTTestApps.configuration import configuration, run_number_error
        from HLTTestApps.option import file_opt_spec, emon_opt_spec
        conf=configuration(file_opt_spec,args)
        t=et.fromstring(str(conf.get_config_ptree()))
        TConfig=t.find("Partition").find("TriggerConfiguration").find("TriggerConfiguration")
        TConfig.remove(TConfig.find("athenaHLTSpecificConfiguration"))
        return TConfig

class HLTMPPUConfig:
    def __init__(self,numForks=2,finalizeTimeout=120,
                 HardTimeout=60000,
                 softTimeoutFraction=0.8,
                 extraParams=[],
                 childLogRoot="/tmp/",
                 DataSrc=None,
                 InfoSvc=None,
                 partitionName="test",
                 HLTLibs=["TrigServices","TrigPSC","TrigConfigSvc"]):
        self._DataSource=DataSrc
        self._InfoService=InfoSvc
        self._childLogRoot=childLogRoot
        if len(childLogRoot)==0:
            self._childLogRoot="/log/%s/%s"%(os.environ["USER"],partitionName)
        if not os.path.exists(self._childLogRoot):
            try:
                os.mkdir(self._childLogRoot)
            except:
                e = sys.exc_info()[0]
                print "Warning log directory creation failed! %s"%(e)
        self._defaultDict={
            "UID":"ALPPy",
            "childLogRoot":self._childLogRoot,
            "numForks":numForks,
            "finalizeTimeout":finalizeTimeout,
            "HardTimeout":HardTimeout,
            "softTimeoutFraction":softTimeoutFraction,
            "extraParams":extraParams,
            "HLTImplementationLibraries":HLTLibs,
            "DataSource":self._DataSource,
            "InfoService":self._InfoService
            }

    def getTree(self):
        root=et.Element("ALPApplication")
        specials=["extraParams","HLTImplementationLibraries","DataSource","InfoService"]
        dd=self._defaultDict
        for k in dd.keys():
            if k not in specials:
                et.SubElement(root,str(k)).text=str(dd[k])
            else:
                if k =="extraParams":
                    if dd[k] is None or len(dd[k])==0:
                        continue
                    ep=et.SubElement(root,"extraParams")
                    for e in dd[k]:
                        et.SubElement(ep,"parameter").text=str(e)
                elif k=="HLTImplementationLibraries":
                    hl=et.SubElement(root,"HLTImplementationLibraries")
                    for l in dd[k]:
                        et.SubElement(hl,"library").text=str(l)
                elif k=="DataSource":
                    # dt=dd[k].getTree()
                    # root.append(dt)
                    et.SubElement(root,"DataSourceLibrary").text=str(dd[k].getLibrary())
                elif k=="InfoService":
                    # it=dd[k].getTree()
                    # root.append(dt)
                    et.SubElement(root,"InfoServiceLibrary").text=str(dd[k].getLibrary())
        root.append(self._DataSource.getTree())
        Inf=et.SubElement(root,"InfoService")
        Inf.append(self._InfoService.getTree())
        return root

class ConfigHelper:
    def __init__(self,configDict):
        self._configDict=configDict

    def xml2string(self,tree):
        return et.tostring(tree)

    def genRosMapping(self,ros2robMap):
        ''' Convert ros2robMap dictionary to xml tree
        Format has to be 
        ros2robMap={"ROS-FULL_SD_EVENT-00":[1120005,1120006,...],
        "ROS-FULL_SD_EVENT-01":[2120005,2120006,...],
        }
        '''
        root=et.Element("ROS2ROBS")
        for ros in iter(ros2robMap):
            robs=ros2robMap[ros]
            for rob in robs:
                et.SubElement(r,"ROBID").text=str(rob)
        return root
    def recurseDict(self,d,rootName):
        r=et.Element(rootName)
        for k in iter(d):
            if isinstance(d[k],str):
                et.SubElement(r,k).text=str(d[k])
            else:
                l=recurseDict(d[k],k)
                r.append(l)
        return r



class ALPPy:
    def __init__(self,libName="libALP.so"):
        import _ALPPy as ALP
        #ALP.LoadLibrary(libName)
        self.__libName=libName
        self._libLoaded=False
        self.LoadLibrary()
        ALP.SetSignalHandler()
    def LoadLibrary(self):
        import _ALPPy as ALP
        if not self._libLoaded:
            self._libLoaded=ALP.LoadLibrary(self.__libName)
    def Configure(self,configTree):
        if configTree is not None:
            import _ALPPy as ALP
            return ALP.ConfigurePU(configTree)
        return False
    def Unconfigure(self):
        import _ALPPy as ALP
        return ALP.UnconfigurePU("")
         
    def Connect(self):
        import _ALPPy as ALP
        return ALP.ConnectPU("")

    def PrepareForRun(self,prepTree):
        import _ALPPy as ALP
        import os
        currpid=os.getpid()
        retVal=ALP.PrepareForRunPU(prepTree)
        newpid=os.getpid()
        if currpid!=newpid:
            print "Running in children with pid %s. Exiting!"%newpid
            os.exit(0)
        if not retVal:
            print "Prepare for run returned false"
        return retVal
    def StopRun(self):
        import _ALPPy as ALP
        return ALP.StopRunPU("")
    def UserCommand(self,comm):
        import _ALPPy as ALP
        return ALP.UserCommandPU(comm)

