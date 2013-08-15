# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------------------------------------
import time
from model.basics.DataSelection import DataSelection
from model.basics.TRPFile import TRPFile
#----------------------------------------------------------------------------------------------------

#----------------------------------------------------------------------------------------------------
def trp_to_cool(*runNumbers):
    runNumbers = list(runNumbers)
    if not len(runNumbers): return
    
    print "[+] Checking file availability"
    for runNumber in runNumbers:
        try:
            TRPFile(runNumber).get_path()
        except:
            print "[+] Cannot load run ", runNumbers
            runNumbers.remove(runNumber)
    
    for runNumber in runNumbers:
        print "[+] Run: ",runNumber 
        
        print "[+]\t-", "Getting chains dictionary"
        
        
        print "[+]\t-", "Getting streams dictionary"
        
        
    
    print "Stop", time.time()
#----------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    print "Start", time.time()
    #runNumbers = [189288, 189280, 189242, 189207, 189205, 189184]
    runNumbers = [187552]
    trp_to_cool(*runNumbers)
#----------------------------------------------------------------------------------------------------
