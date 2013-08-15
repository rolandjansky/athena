# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import time
import os
import sys
import subprocess
import re
from getpass import getpass

password = None

def ask_for_password():
    global password
    if password == None: password = getpass()


def do_scp(sourcefile, remoteFile):
    global password    
    ask_for_password()
    
    cmd = "./uscp %s %s %s" % (sourcefile, remoteFile, password) 
    subprocess.call(cmd, shell=True)    




user = os.environ["USER"]

def server_local():
    sourceFolder = "/tmp/%s/to_p1/" % user
    destinationFolder = "atlasgw:/atlas-home/0/ggalster/tmp/to_p1/"
    
    filesShipped = set([])
    
    while True:
        
        dirContent = set(os.listdir(sourceFolder))
        
        
        for fileName in filesShipped - dirContent:
            filesShipped.discard(fileName)
        
        
        """ here """ 
        
        fileName = None
        
        for newFile in dirContent:
            if newFile in filesShipped: continue
                
            match = re.match("TriggerRates_ATLAS_([0-9]+).root", newFile)
            if match:
                fileName = newFile
                break 
        
        sleeptime = 1
        if fileName:
            doneFileName = "%s%s" % (sourceFolder, "done")

            transferFile = False

            if os.path.exists(doneFileName): os.remove(doneFileName) 
            
            do_scp(destinationFolder+"done",sourceFolder)

            if not os.path.exists(doneFileName): transferFile = True
    
            if transferFile:
                fullFileName = "%s%s" % (sourceFolder, fileName)
    
                do_scp(fullFileName, destinationFolder)
                
                subprocess.call("touch %s" % doneFileName, shell=True)
                
                do_scp(doneFileName, destinationFolder)
                
                filesShipped.add(fileName)
            sleeptime += 8
            
        time.sleep(sleeptime)
    
    
def server_remote():
    sourceFolder = "/atlas-home/0/ggalster/tmp/to_p1/"
    destinationFolder = "/tmp/ggalster/"

    while 1:
        if os.path.exists(sourceFolder+'done'):
            subprocess.call('rsync -avz --remove-sent-files --include="*.root" --exclude="*" %s %s' % (sourceFolder, destinationFolder), shell=True)
            os.remove(sourceFolder+'done')
        
        time.sleep(1)

def main():  
    if len(sys.argv) == 2 or len(sys.argv) == 3:
        
        if "local" in sys.argv:
            server_local()
            return
        elif "remote" in sys.argv:
            server_remote()
            return
    print "specify either local or remote" 
    
if __name__ == "__main__":    
    main()
