# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import time
#----------------------------------------------------------------------------------------------------



def serve_forever():
    
    syncLast = time.time()
    syncDeltaT = 3
    while True:
        if time.time() > syncLast + syncDeltaT:
            print "[+] Syncing.."
            
            
            
            syncLast = time.time()
             
             
        time.sleep(1) 
        
        


if __name__ == "__main__":
    serve_forever()
#----------------------------------------------------------------------------------------------------