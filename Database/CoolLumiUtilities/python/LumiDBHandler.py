# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# LumiDBHandler
#
# Eric Torrence - October 2010
#
# Contents:
# LumiDBHandler - utility object to handle opening and closing COOL DB connections within
#                 a large python script.  The benefit over just using AtlCoolLib directly
#                 is that each DB connection is cached, so multiple connections to the same
#                 DB will not be made.
#
#                 The parent script should call closeAllDB in its __del__ function to close
#                 the DB connections, even if the script crashes.
#
# General usage example
# dbH = LumiDBHandler()
# myFolder = dbH.getFolder('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LBLESTONL')
#
# One can then browse the folder as usual using browseObjects
#
# The CoolDataReader uses this class internally to make for more easy access
#

from PyCool import cool
from CoolConvUtilities.AtlCoolLib import indirectOpen

class LumiDBHandler:

    # Define dbDict here at class scope
    # Then access with self.__class__.dbDict and it will be the same for all instances of the class
    # This is a pythonish way to create static classes

    # Dict to store DB connection indexed by text DB connection name
    dbDict = dict()

    
    def __init__(self):

        # Debug output (can be changed for each instance, slick...)
        self.verbose = False
        
    # Return a folder reference for the dbstring, folder specified
    # DB will be opened if necessary
    # Example: getFolder('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LBLESTONL')
    def getFolder(self, dbstring, folder, force=False):

        if self.verbose:
            print 'LumiDBHandler.getFolder(', dbstring, ',', folder, ') called'

        if not self.openDB(dbstring, force=force):
            print "LumiDBHandler.getFolder - can't connect to DB!"
            return None

        return self.__class__.dbDict[dbstring].getFolder(folder)
    
    # Open a COOL DB connection based on a name such as "COOLONL_INDET/OFLP200"
    # Returns True if successful (or DB already open) 
    def openDB(self, dbstring, oracle=False, debug=False, force=False):

        if self.verbose:
            print 'LumiDBHandler.openDB(', dbstring, ') called'

        # Check if already open
        if dbstring in self.__class__.dbDict:

            # No force, just return 
            if not force:
                if self.verbose:
                    print 'LumiDBHandler.openDB - Connection already exists'
                return True # Yes it is

            # Force specified, close so we can re-open
            if self.verbose:
                print 'LumiDBHandler.openDB - Connection already exists, closing first due to force=True'
            self.closeDB(dbstring)

        # Try to open DB connection
        if self.verbose:
            print 'LumiDBHandler.openDB - Connecting to', dbstring
            
        try:
            db = indirectOpen(dbstring, readOnly=True, oracle=oracle, debug=debug)
        except Exception, e:
            print e
            return False

        # OK, opened.  Save this to our dict for later use
        self.__class__.dbDict[dbstring] = db
        
        return True

    # Close specific DB
    def closeDB(self, dbstring):

        if self.verbose:
            print 'LumiDBHandler.closeDB - Closing connection to', dbstring

        if not dbstring in self.__class__.dbDict:
            print "LumiDBHandler.closeDB - DB doesn't exist:", dbstring
        else:
            try:
                self.__class__.dbDict[dbstring].closeDatabase()
            except Exception, e:
                print e
            self.__class__.dbDict.pop(dbstring)

    # Called by default in the destructor, but not guaranteed if there are problems
    def closeAllDB(self):
        self.closeAll()
        
    def closeAll(self):
        
        if self.verbose:
            print 'LumiDBHandler.closeAllDB called'

        # Can't use iterkeys here as we are deleting the elements
        for dbstring in self.__class__.dbDict.keys():
            self.closeDB(dbstring)
            
