# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import os
import json
import six
import xml.etree.ElementTree as ET
from collections import OrderedDict as odict

def getFileType(filename):
    filetype = "unknown"
    with open(filename, 'r') as fp:
        config = json.load(fp)
        filetype = config['filetype']
    return filetype

from enum import Enum
class ConfigType(Enum):
    NONE = ("Config", "None")
    L1MENU = ("L1Menu", "l1menu")
    HLTMENU = ("HLTMenu", "hltmenu")
    L1PS = ("L1PrescalesSet", "l1prescale")
    HLTPS = ("HLTPrescalesSet", "hltprescale")
    BGS = ("L1BunchGroupsSet", "bunchgroupset")
    HLTJO = ("HLTJobOptions", "joboptions")
    def __init__(self, basename, filetype):
        self.basename = basename
        self.filetype = filetype
    def __eq__(self, other):
        if isinstance(other,six.string_types):
            return self.filetype == other
        else:
            return self.filetype == other.filetype
    def __ne__(self, other):
        return not self.__eq__(other)

class ConfigLoader(object):
    """ 
    ConfigLoader derived classes hold the information of the configuration source
    and define the method to load the configuration
    """ 
    def __init__(self,configType):
        self.configType = configType
    def confirmConfigType(self,config):
        """
        checks that the in-file specification of the configuration type matches the expected type
        """
        if config['filetype'] != self.configType:
            raise RuntimeError("Can not load file with filetype '%s' when expecting '%s'" % (config['filetype'], self.configType.filetype))
        

class ConfigFileLoader(ConfigLoader):
    def __init__(self, configType, filename ):
        super(ConfigFileLoader,self).__init__(configType) 
        self.filename = filename
    def load(self):
        with open(self.filename, 'r') as fp:
            config = json.load(fp, object_pairs_hook = odict)
            self.confirmConfigType(config)
        return config
    def setQuery(self, query):
        pass
    def getWriteFilename(self):
        outfn = os.path.basename(self.filename)
        if outfn.endswith(".json"):
            outfn = outfn.rsplit('.',1)[0]
        return outfn + ".out.json"

class ConfigDBLoader(ConfigLoader):
    def __init__(self, configType, dbalias, dbkey):
        super(ConfigDBLoader,self).__init__(configType)
        self.dbalias = dbalias
        self.dbkey = dbkey
        self.query = None
        self.schema = None
    def setQuery(self, query):
        """
        query template can be a single query or a list of queries (to deal with different schemata)
        (internally stored as list of queries, which are tried in order)
        """
        self.query = [ query ] if isinstance(query,six.string_types) else query
    @staticmethod
    def getResolvedFileName(filename, pathenv=""):
        """ looks for file, first absolute, then by resolving envvar pathenv"""
        if os.access(filename,os.R_OK):
            return filename
        pathlist = os.getenv(pathenv,'').split(os.pathsep)
        for path in pathlist:
            f = os.path.join( path, filename )
            if os.access( f, os.R_OK ):
                return f
        raise RuntimeError("Can't read file %s, neither locally nor in %s" % (filename, pathenv) )
    @staticmethod
    def getConnectionParameters(dbalias):
        dblookupFile = ConfigDBLoader.getResolvedFileName("dblookup.xml", "CORAL_DBLOOKUP_PATH")
        dbp = ET.parse(dblookupFile)
        listOfServices = None
        for logSvc in dbp.iter("logicalservice"):
            if logSvc.attrib["name"] != dbalias:
                continue
            listOfServices = [ serv.attrib["name"] for serv in logSvc.iter("service") ]
            if len(listOfServices) == 0:
                raise RuntimeError("DB %s has no services listed in %s" % (dbalias, dblookupFile))
            break
        if listOfServices is None:
            raise RuntimeError("DB %s not available in %s" % (dbalias, dblookupFile))
        # now get the account and pw for oracle connections
        credentials = odict().fromkeys(listOfServices)
        authFile = ConfigDBLoader.getResolvedFileName("authentication.xml", "CORAL_AUTH_PATH")
        for svc in filter(lambda s : s.startswith("oracle:"), listOfServices):
            ap = ET.parse(authFile)
            count = 0
            for con in filter( lambda c: c.attrib["name"]==svc, ap.iter("connection")):
                credentials[svc] = dict([(par.attrib["name"],par.attrib["value"]) for par in con])
                count += 1
            if count==0:
                raise RuntimeError("No credentials found for connection %s from service %s for db %s" % (con,svc,dbalias))
            if count>1:
                raise RuntimeError("More than 1 connection found in %s for service %s" % (authFile, svc))
        return credentials
    @staticmethod
    def getConnection(credentials):
        for connSvc, userpw in credentials.items():
            try:
                if connSvc.startswith("oracle:"):
                    from cx_Oracle import connect
                    [tns,schema] = connSvc.split("/")[-2:]
                    connection = connect(userpw["user"], userpw["password"], tns, threaded=False)
                elif connSvc.startswith("frontier:"):
                    raise NotImplementedError("Python-loading of trigger configuration from Frontier has not yet been implemented")
                return connection, schema
            except Exception as e:
                raise RuntimeError(e)
    def load(self):
        from cx_Oracle import DatabaseError
        credentials = ConfigDBLoader.getConnectionParameters(self.dbalias)
        connection, self.schema = ConfigDBLoader.getConnection(credentials)
        cursor = connection.cursor()
        qdict = { "schema" : self.schema, "dbkey" : self.dbkey }
        failures = []
        config = None
        for q in self.query:
            try:
                cursor.execute( q.format(**qdict) )
            except DatabaseError as e:
                failures += [ (q.format(**qdict), str(e)) ]
            else:
                configblob = cursor.fetchall()[0][0]
                config = json.loads(configblob.read().decode("utf-8"), object_pairs_hook = odict)
                break
        if not config:
            for q,f in failures:
                print("Failed query: %s\nFailure: %s" % (q,f))
            raise RuntimeError("Query failed")
        self.confirmConfigType(config)
        return config
    # proposed filename when writing config to file
    def getWriteFilename(self):
        return "{basename}_{schema}_{dbkey}.json".format(basename = self.configType.basename, schema = self.schema, dbkey = self.dbkey)



class TriggerConfigAccess(object):
    """ 
    base class to hold the configuration (OrderedDict) 
    and provides basic functions to access and print
    """
    def __init__(self, configType, mainkey, filename = None, dbalias = None, dbkey = None):
        self._getLoader(configType = configType, filename = filename, dbalias = dbalias, dbkey = dbkey)
        self._mainkey = mainkey
        self._config = None

    def _getLoader(self, configType, filename = None, dbalias = None, dbkey = None ):
        if filename:
            self.loader = ConfigFileLoader( configType, filename )
        elif dbalias and dbkey:
            self.loader = ConfigDBLoader( configType, dbalias, dbkey )
        else:
            raise RuntimeError("Neither input file nor db alias and key provided")

    def load(self):
        self._config = self.loader.load()

    def __str__(self):
        return str(self._config)

    def __iter__(self):
        return iter(self[self._mainkey])

    def __getitem__(self, item):
        return self._config[item]

    def __len__(self):
        return len(self[self._mainkey])

    def config(self):
        """ returns the configuration """
        return self._config

    def prettyPrint(self):
        if self._config:
            print(json.dumps(self._config, indent = 4, separators=(',', ': ')))
    
    def name(self):
        return self["name"]

    def filetype(self):
        return self["filetype"]

    def printSummary(self):
        """ print summary info, should be overwritten by derived classes """
        print("Configuration name: %s" % self.name())
        print("Configuration size: %s" % len(self))

    def writeFile(self, filename = None):
        if filename is None:
            filename = self.loader.getWriteFilename()
        with open(filename, 'w') as fh:
            json.dump(self.config(), fh, indent = 4, separators=(',', ': '))
            print("Wrote file %s" % filename)
