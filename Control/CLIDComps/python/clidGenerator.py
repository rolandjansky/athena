# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

## @file clidGenerator.py
# @author CETull@lbl.gov
# @brief Athena CLID Generator Class
#
import string, re, os, csv

# CLID assignment was originally based on the builtin hash() function.
# However, the results of that are not standardized, and in fact changed
# between py2 and py3.  (In py3, hash(s) can return differing values
# from run to run.)  This is an implementation of the py2 string hashing
# algorithm, so we can give the same results for py2 vs. py3.
def py2_hash (s):
    if len(s) == 0: return 0
    x = ord(s[0]) << 7
    for c in s:
        x = (1000003*x) ^ ord(c)
    x ^= len(s)
    x &= 0xffffffff
    if x == 0xffffffff:
        x = 0xfffffffe
    return x

## Athena CLID Generator Class
class clidGenerator (object):
    """Athena CLID Generator"""
    # CLID Mask: Determines bits used for CLID
    __mask = 0x0FFFFFFF
    # CLID Repositories - Dictionaries of CLID<=>Name
    __clidRep = {} # Lookup by CLID
    __clidTid = {} # Lookup by CLID
    __nameRep = {} # Lookup by ClassName
    __tidRep  = {} # Lookup by typeid-name

    def __init__(self, db, debug=False):
        self.setCLIDDB(db, debug)
        self.readdb()

    def cleardb(self):
        clidGenerator.__clidRep = {} # Lookup by CLID
        clidGenerator.__nameRep = {} # Lookup by ClassName
        
    def readdb(self):
        """Read CLID DataBase file"""
        try:
            for cliddb in self.__cliddbs:
                if os.path.isfile(cliddb):
                    with open(cliddb, 'r') as f_cliddb:
                        for row in csv.reader (f_cliddb, delimiter=';'):
                            row = [i.strip() for i in row]
                            if len(row) >= 2:
                                clid = int(row[0])
                                class_name = row[1]
                                tid_name = row[2] if len(row)>2 else class_name

                                self.__clidRep[clid] = class_name
                                self.__clidTid[clid] = tid_name
                                self.__nameRep[class_name] = clid
                                self.__tidRep [tid_name]   = clid
                            
                else:
                    print ("No CLID DataBase file <%s> " % cliddb)
        except Exception as err:
            print ("Error reading from CLID DataBase files <%s>:\n%s " %  (
                self.__cliddbs,
                err))
            
    def setCLIDDB(self, db, debug):
        """Initializes a CLID Generator object with a CLID Database"""
        if db:
            self.__cliddbs = search_files(db, os.getenv('DATAPATH'))
            if debug: print ("Using specified CLID DataBase files %s " % self.__cliddbs)
        elif os.getenv('CLIDDB'):
            # CLID DataBase (Default = clid.db)
            self.__cliddbs.append(os.getenv('CLIDDB'))
            if debug: print ("Using DataBase file from CLIDDB env variable %s " % self.__cliddbs)
        else:
            self.__cliddbs = search_files('clid.db', os.getenv('DATAPATH'))
            if debug: print ("Using DataBase file from DATAPATH %s " % self.__cliddbs)

    def writedb(self,db):
        """Read CLID DataBase file"""
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
        c = py2_hash(className) & self.__mask
        if c < 10001 or c > self.__mask:
            c = self.genClidFromName(className+'_')
        if self.isCollection(className):
            c += 0x40000000
        if c in self.__clidRep:
            if n != self.__clidRep[c]:
                c = self.genClidFromName(className+'_')
        else:
            self.__clidRep[c] = n
            self.__clidTid[c] = n # make typeid name the same than class-name
            self.__nameRep[n] = c
            self.__tidRep [n] = c # idem
        return c

    def getClidFromName(self,className):
        """Get the CLID in the repository of class name <className>"""
        return self.__nameRep.get(className, None)

    def getClidFromTid(self,tidName):
        """Get the CLID in the repository of typeid name <tidName>"""
        return self.__tidRep.get(tidName, None)

    def getNameFromClid(self,clid):
        """Get the class name in the repository with CLID <clid>"""
        return self.__clidRep.get(clid, None)

    def getTidFromClid(self,clid):
        """Get the typeid name in the repository with CLID <clid>"""
        return self.__clidTid.get(clid, None)

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
    for path in search_path.split(pathsep):
        candidate = os.path.join(path, filename)
        if os.path.exists(candidate): clidFiles.append(os.path.abspath(candidate))
    return clidFiles
