# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Copyright (c) 2002-2003, Lawrence Berkeley National Laboratory, CETull@lbl.gov
# Athena CLID Generator Class
#
import string, re, os

class clidGenerator:
    "Athena CLID Generator"
    __clidGenerator_type = "Basic"
    __clidGenerator_version = "$Revision: 1.1.1.1 $"
    # CLID Mask: Determines bits used for CLID
    __mask = 0x0FFFFFFF
    # CLID Repositories - Dictionaries of CLID<=>Name
    __clidRep = {} # Lookup by CLID
    __nameRep = {} # Lookup by ClassName
    # CLID DataBase (Default = clid.db)
#   __cliddb = os.getenv('CLIDDB')
    def __init__(self,db):
        "Initializes a CLID Generator object with a CLID Database"
        if db:
            self.__cliddb = db
        else:
            # CLID DataBase (Default = clid.db)
            self.__cliddb = os.getenv('CLIDDB')
        self.readdb()
#   def __init__(self):
#       "Initializes a default CLID Generator object"
#       self.__cliddb = os.getenv('CLIDDB')
#       self.readdb()
    def readdb(self):
        "Read CLID DataBase file"
        try:
            if os.path.isfile(self.__cliddb):
                input = open(self.__cliddb,'r')
                s = input.readlines()
                for l in s:
                    ll = string.split(l)
                    self.__clidRep[int(ll[0])] = ll[1]
                    self.__nameRep[ll[1]] = int(ll[0])
                input.close()
        except:
            print "No CLID DataBase file"
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
            self.__nameRep[n] = c
        return c
    def getClidFromName(self,className):
        "Get the CLID in the repository of class name <className>"
        if self.__nameRep.has_key(className):
            return self.__nameRep[className]
        else:
            return None
    def getNameFromClid(self,clid):
        "Get the class name in the repository with CLID <clid>"
        if self.__clidRep.has_key(clid):
            return self.__clidRep[clid]
        else:
            return None
    def demangleClassName(self,s):
        pat = re.compile('\s*(.?)__*\s*')
        n = pat.findall(s)
        if n:
            return n[0]
        else:
            return s
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

