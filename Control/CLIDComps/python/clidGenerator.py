# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file clidGenerator.py
# @author CETull@lbl.gov
# @brief Athena CLID Generator Class
#
import string, re, os, csv

## Athena CLID Generator Class
class clidGenerator (object):
    "Athena CLID Generator"
    __clidGenerator_type = "Basic"
    __clidGenerator_version = "$Revision: 1.4 $"
    # CLID Mask: Determines bits used for CLID
    __mask = 0x0FFFFFFF
    # CLID Repositories - Dictionaries of CLID<=>Name
    __clidRep = {} # Lookup by CLID
    __clidPkg = {} # Lookup by CLID
    __clidTid = {} # Lookup by CLID
    __nameRep = {} # Lookup by ClassName
    __tidRep  = {} # Lookup by typeid-name
    # CLID DataBase (Default = clid.db)
#   __cliddb = os.getenv('CLIDDB')
    def __init__(self, db, debug=False):
        self.setCLIDDB(db, debug)
        self.readdb()

    def cleardb(self):
        clidGenerator.__clidRep = {} # Lookup by CLID
        clidGenerator.__nameRep = {} # Lookup by ClassName
        
    def readdb(self):
        "Read CLID DataBase file"
        try:
            for cliddb in self.__cliddbs:
                if os.path.isfile(cliddb):
                    for row in csv.reader (open(cliddb, 'r'),
                                           delimiter=';'):
                        row = [i.strip() for i in row]
                        if len(row) >= 3:
                            clid = int(row[0])
                            class_name = row[1]
                            pkg_name   = row[2]
                            if len(row) > 3: tid_name = row[3]
                            else:            tid_name = class_name
                            
                            self.__clidRep[clid] = class_name
                            self.__clidPkg[clid] = pkg_name
                            self.__clidTid[clid] = tid_name
                            self.__nameRep[class_name] = clid
                            self.__tidRep [tid_name]   = clid
                            
                else:
                    print "No CLID DataBase file <%s> " % cliddb        
        except Exception, err:
            print "Error reading from CLID DataBase files <%s>:\n%s " %  (
                self.__cliddbs,
                err)
            
    def setCLIDDB(self, db, debug):
        "Initializes a CLID Generator object with a CLID Database"
        if db:
            self.__cliddbs = search_files(db, os.getenv('DATAPATH'))
            if debug: print "Using specified CLID DataBase files %s " % self.__cliddbs
        elif os.getenv('CLIDDB'):
            # CLID DataBase (Default = clid.db)
            self.__cliddbs.append(os.getenv('CLIDDB'))
            if debug: print "Using DataBase file from CLIDDB env variable %s " % self.__cliddbs
        else:
            self.__cliddbs = search_files('clid.db', os.getenv('DATAPATH'))
            if debug: print "Using DataBase file from DATAPATH %s " % self.__cliddbs

    def writedb(self,db):
        "Read CLID DataBase file"
        output = open(db,'w')
        for k in self.__clidRep.keys():
            output.write("%d "%k+self.__clidRep[k]+"\n")
        output.close()
    def genClidFromName(self,className):
        """Generate CLID from ClassName: A recursive hash with a bit
        mask and validity range. Will check collisions against and
        update CLID Repository."""
        n = self.demangleClassName(className)
        c = self.getClidFromName(className)
        if c:
            return c
        c = hash(className) & self.__mask
        if c < 10001 or c > self.__mask:
            c = self.genClidFromName(className+'_')
        if self.isCollection(className):
            c += 0x40000000
        if self.__clidRep.has_key(c):
            if n != self.__clidRep[c]:
                c = self.genClidFromName(className+'_')
        else:
            self.__clidRep[c] = n
            self.__clidTid[c] = n # make typeid name the same than class-name
            self.__nameRep[n] = c
            self.__tidRep [n] = c # idem
        return c
    def getClidFromName(self,className):
        "Get the CLID in the repository of class name <className>"
        if self.__nameRep.has_key(className):
            return self.__nameRep[className]
        else:
            return None
    def getClidFromTid(self,tidName):
        "Get the CLID in the repository of typeid name <tidName>"
        if tidName in self.__tidRep:
            return self.__tidRep[tidName]
        else:
            return None
    def getNameFromClid(self,clid):
        "Get the class name in the repository with CLID <clid>"
        if self.__clidRep.has_key(clid):
            return self.__clidRep[clid]
        else:
            return None
    def getTidFromClid(self,clid):
        "Get the typeid name in the repository with CLID <clid>"
        if clid in self.__clidTid:
            return self.__clidTid[clid]
        else:
            return None
    def getPackageFromClid(self,clid):
        "Get the name of the package defining <clid>"
        if self.__clidPkg.has_key(clid):
            return self.__clidPkg[clid]
        else:
            return None
    def demangleClassName(self,s):
        return s
#        pat = re.compile('\s*(.?)__*\s*')
#        n = pat.findall(s)
#        if n:
#            return n[0]
#        else:
#             return s

    def isCollection(self,className):
        collMatch = re.search(r'.*?Collection_*|.*?Container_*',className)
        return collMatch

    def findPattern(self,s):
        """Find the regular expression pattern s in dictionary."""
#        pat = re.compile('^'+s+'$')
        pat = re.compile(s)
        results = {}
        for k in self.__clidRep.keys():
            if pat.match(str(k)) or pat.match(self.__clidRep[k]):
                results[k] = self.__clidRep[k]
        return results

def search_file(filename, search_path, pathsep=os.pathsep):
    """Given a search path, find file with requested name """
    for path in string.split(search_path, pathsep):
        candidate = os.path.join(path, filename)
        if os.path.exists(candidate): return os.path.abspath(candidate)
    return None

def search_files(filename, search_path, pathsep=os.pathsep):
    """Given a search path, find file with requested name """
    clidFiles = []
    for path in string.split(search_path, pathsep):
        candidate = os.path.join(path, filename)
        if os.path.exists(candidate): clidFiles.append(os.path.abspath(candidate))
    return clidFiles
