# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 29, 2011

@author: Gorm Galster
'''

import os
import subprocess
import coca
import time

class TRPFile(object):
    def __init__(self, runNumber = None):
        self.set_time_out(600)
        if runNumber:
            self.set_run_number(runNumber)
        
    __runNumber = int()
    def get_run_number(self): return self.__runNumber
    def set_run_number(self, val): self.__runNumber = int(val)
        
    __timeOut = int()
    def get_time_out(self): return self.__timeOut
    def set_time_out(self, val): self.__timeOut = int(val)
    
    def get_path(self):
        if self.is_local():
            return self.get_local_path()
        if self.is_on_AFS():
            return self.get_AFS_path()
        
        self.make_local()
        return self.get_local_path()


    
    def get_short_name(self):
        return "TriggerRates_ATLAS_%i.root" % self.get_run_number()
        
    def is_local(self):
        return os.path.exists(self.get_local_path())
    
    def get_local_path(self):
        filename = "/tmp/%s/%s" % (os.environ["USER"], self.get_short_name())
        return filename
        
    def is_on_AFS(self):
        return os.path.exists(self.get_AFS_path())
        
        
    def get_AFS_path(self):
        fileName = "/afs/cern.ch/user/a/atlasdqm/dqmdisk1/histos/Cosmics08/online/tdaq-03-00-01/coca/TRP-Rates/%s" % self.get_short_name()
        return fileName


    def make_local(self):
        if self.is_local(): return
        
        lockName = "/tmp/%s/%s.lock" % (os.environ["USER"], self.get_short_name())
        def set_lock():
            subprocess.call("touch %s" % lockName, shell=True)
        def remove_lock():
            subprocess.call("rm -f %s" % lockName, shell=True)
        def lock_exists():
            return os.path.exists(lockName)
            
        if not lock_exists():            
            try:
                set_lock()
                       
                CORAL_AUTH_PATH = "/afs/cern.ch/atlas/project/tdaq/databases/.coral"
                if not os.environ.has_key("CORAL_AUTH_PATH"):
                    os.environ["CORAL_AUTH_PATH"] = CORAL_AUTH_PATH
                else:
                    if not CORAL_AUTH_PATH in str(os.environ["CORAL_AUTH_PATH"]).split(":"):
                        os.environ["CORAL_AUTH_PATH"] = ":".join([CORAL_AUTH_PATH] + str(os.environ["CORAL_AUTH_PATH"]).split(":"))
                
                
                try:
                    dbClient = coca.DBClient()
                    remoteFiles = dbClient.files("TRP-Rates", "", self.get_short_name())
                except UserWarning as warn:
                    raise warn
                
                
                
                for remoteFile in remoteFiles:
                    archivePathRemote = "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/tdaq-mon/coca/%s" % remoteFile.archive()
                    archivePathLocal = "/tmp/%s/%s" % (os.environ["USER"], remoteFile.archive())
                    
                    if not os.path.exists(archivePathLocal):
                        retcode = subprocess.call("xrdcp %s /tmp/%s/" % (archivePathRemote, os.environ["USER"]), shell=True)
                        print "xrdcp %s /tmp/%s/" % (archivePathRemote, os.environ["USER"])
                        if not retcode == 0: #0 == good, 255 == file exists
                            raise Exception("An error occured while retriving archive %s from CASTOR." % remoteFile.archive())
                        
                    print "unzip -u -o %s %s -d /tmp/%s/" % (archivePathLocal, self.get_short_name(), os.environ["USER"])
                    retcode = subprocess.call("unzip -u -o %s %s -d /tmp/%s/" % (archivePathLocal, self.get_short_name(),  os.environ["USER"]), shell=True)
                    if not retcode == 0: #0 == good
                        raise Exception("An error occured unzipping archive %s" % (archivePathLocal))
                        
                    retcode = subprocess.call("rm -f %s" % (archivePathLocal), shell=True)
                    if not retcode == 0: #0 == good
                        raise Exception("An error occured while removing %s" % (archivePathLocal))
                    
                    remove_lock()
            except:
                remove_lock()
                raise
        else:
            counter = 0
            counterMax = self.get_time_out()
            
            while lock_exists() and counter < counterMax:
                time.sleep(1)
                counter += 1
                
            if os.path.exists(self.get_local_path()):
                return
            else:
                if lock_exists():
                    remove_lock()
                self.make_local()
            
            

if __name__ == "__main__":
#    runNumber = 182456
    #runNumber = 188921
    runNumber = 191933 
    trp = TRPFile(runNumber)
    trp.make_local()
    
    print trp.get_path()
    
    #print TRPFile(runNumber).get_path()
    
