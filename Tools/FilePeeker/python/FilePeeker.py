# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import os,sys
from subprocess import check_output

def PeekFile(inputfile,isByteStream=False):
    if not os.access(inputfile,os.R_OK):
        print "ERROR: Failed input file",inputfile,"not accessible"
        return None

    if (isByteStream):
        retval=check_output(["BSFilePeeker","-k",inputfile])
    else:
        retval=check_output(["PoolFilePeeker","-k",inputfile])
    

    result=dict()
    for line in retval.split(os.linesep):
        linesplit=line.split(":")
        if len(linesplit)!=2: continue;
        k=linesplit[0].strip();
        v=linesplit[1].strip();
        if len(v)==0: continue
        if (k=="RunNumber" or k=="LBNumber"):
            result[k]=set(int(rn) for rn in v.split())
        elif (k=="BeamEnergy"):
            result[k]=int(v)

        elif (k=="SGkeys"):
            result[k]=set(sgk for sgk in v.split())
        
        else:
            result[k]=v

    return result

    

if __name__=="__main__":
    fname=sys.argv[1]
    bsfile=not "pool.root" in fname
        
    print PeekFile(sys.argv[1],bsfile)
     
                  
