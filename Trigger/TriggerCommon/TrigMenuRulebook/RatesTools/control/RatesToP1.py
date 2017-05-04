# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Sep 17, 2011

@author: ggalster
'''

import os
import subprocess

from model.basics.TRPFile import TRPFile


def download_trp(*runNumbers):
    if runNumbers == None: return
    
    failed = []
    for runNumber in runNumbers:
        try:
            TRPFile(runNumber).make_local()
        except:
            failed.append(runNumber)
    
    return failed


def ship_local_trp(*runNumbers, **kwargs):
    if runNumbers == None: return
    
    user = os.environ["USER"]
    
    destination_path = "atlasgw:/atlas-home/0/%s/tmp/" % user
    if kwargs.has_key("destination_path"): destination_path = kwargs["destination_path"]  
    
    
    
    include_sring = ""
    for runNumber in runNumbers:
        trp = TRPFile(runNumber)
        
        if not trp.is_local(): continue
        
        include_sring += "--include='%s' " % trp.get_short_name()
    
    if not len(include_sring): return
    
    
    cmd = "rsync -avz %s--exclude='*' /tmp/%s/*.root %s@%s" % (include_sring, user, user, destination_path)
    
    
    
    print "[+] About to execute command:"
    print cmd
    retcode = subprocess.call(cmd, shell=True)
    
    
def trp_to_p1(*runNumbers, **kwargs):
    if runNumbers == None: return
    
    failed = download_trp(*runNumbers, **kwargs)
    
    if len(failed):
        print "[-] I failed to download the following runs:"
        print failed
             
    transfer = set(runNumbers) - set(failed)
    
    ship_local_trp(*transfer)
    

    
    

if __name__ == "__main__":
    runNumbers = [186873, 186877, 186878, 186923, 186933, 186934, 186965, 187014, 187196, 187219, 187453, 187457, 187501, 187543, 187552, 187763, 187811, 187812, 187815]
    #failed [186873, 186965, 187014]
    trp_to_p1(*runNumbers)
    pass