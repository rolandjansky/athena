# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, os, os.path
import anydbm

class DBKeyPrinter:
    def __init__(self):
        self.handleArgs()
        self.dbKeys = self.getAllDBkeys()

    def usage(self):
        print
        print 'printDBkeys: prints all DB keys that match the given criteria'
        print
        print 'Usage:'
        print '     python printDBkeys.py <pathToDB> "selectionKey1,selectionKey2,..."'
        print 'where:'
        print '   <pathToDB>: obligatory'
        print '   "selectionKey1,selectionKey2,...": optional. If missing = print all keys.'
        print
        
    def getDBpath(self):
        try:
            self.dbPath = sys.argv[1]
        except:
            self.usage()
            sys.exit(2)
        else:
            if not os.path.exists(self.dbPath):
                print 'DB %s does not exist.'
                sys.exit(2)

    def getDBSelectorKeys(self):
        try:
            keys = sys.argv[2]
        except:
            self.selectorKeys = []
        else:
            self.selectorKeys = [t.strip() for t in keys.split(',')]
            
    def handleArgs(self):
        self.getDBpath()
        self.getDBSelectorKeys()

    def getAllDBkeys(self):
        h = anydbm.open(self.dbPath,'r')
        keys = h.keys()
        h.close()
        return keys

    def select(self):
        matches = 0
        for k in self.dbKeys:
            doNotMatchSelectorKey = [s for s in self.selectorKeys if str(k).find(s)==-1]
            if len(doNotMatchSelectorKey) != len(self.selectorKeys) or len(self.selectorKeys)==0:
                matches += 1
                print k
                print
        print '----------'
        print '%d matching DB keys.' % matches

if __name__ == '__main__':
    dbkp = DBKeyPrinter()
    dbkp.select()
    
