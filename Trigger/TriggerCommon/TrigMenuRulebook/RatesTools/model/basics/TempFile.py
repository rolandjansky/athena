# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 4, 2011

@author: galster
'''

import os
import random
import string

from subprocess import call

from Outputter import Outputter

class TempFile(Outputter):
    def __init__(self):
        self.__path = ""
        self.Create()
        
    __path = str()
    
    def Create(self):
        if self.__path != "":
            self.Delete()
        
        if not os.path.exists("/tmp/" + os.environ["USER"]):
            os.mkdir("/tmp/" + os.environ["USER"])
        
        filename = "/tmp/" + os.environ["USER"] + "/TempFile_" + "".join([random.choice(string.letters) for i in xrange(15)])
        try:
            call("touch " + filename, shell=True)
            self.__path = filename
        except:
            self.PrintError("There was an error creating the temporary file " + filename)
            return None
        
        return self.__path
        
    
    def Delete(self):
        filename = self.__path
        #print "???",filename
        if len(filename) > 0:
            if not os.path.exists(filename):
                self.__path = ""
                return
        
        #print "???",os.path.exists(filename) 
        try:
            call("rm " + filename, shell=True)
        except:
            self.PrintError("There was an error deleting the temporary file " + filename)
        
        self.__path = ""

        
    
    def GetPath(self):
        return self.__path
    