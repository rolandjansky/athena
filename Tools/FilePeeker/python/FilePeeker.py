# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import os
import sys
from subprocess import check_output


def PeekFiles(inputfiles):

    bsfiles=[]
    poolfiles=[]
    for inputfile in inputfiles:
        if not os.access(inputfile,os.R_OK):
            print ("ERROR: Failed input file",inputfile,"not accessible")
            return None
        if "pool.root" in inputfile:
            poolfiles.append(inputfile)
        else:
            bsfiles.append(inputfile)
        

    retval=""
    if len(bsfiles)>0:
        retval=check_output(["BSFilePeeker","-k"]+bsfiles).decode('utf-8')

    if len(poolfiles)>0:
        retval+=check_output(["PoolFilePeeker","-k"]+poolfiles).decode('utf-8')
    

    fullResult=dict()
    for line in retval.split(os.linesep):
        linesplit=line.split(":")
        if len(linesplit)!=2: continue
        k=linesplit[0].strip()
        v=linesplit[1].strip()
        if len(v)==0: continue
        if (k=="File"):
            result=dict()
            fullResult[v]=result
            

        elif (k=="RunNumber" or k=="LBNumber"):
            result[k]=set(int(rn) for rn in v.split())
        elif (k=="BeamEnergy"):
            result[k]=int(v)

        elif (k=="SGkeys"):
            result[k]=set(sgk for sgk in v.split())
        
        elif (k=="isMC"):
            result[k]=bool(int(v))
        else:
            result[k]=v

    return fullResult

    

if __name__=="__main__":
    print (PeekFiles(sys.argv[1:]))
     
                  
