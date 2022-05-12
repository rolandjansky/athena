#!/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys,os,subprocess,tempfile
import subprocess
from string import *
from LArCafJobs.GetLBsToIgnore import getLBsToIgnore

#eoscmd="/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select"
eoscmd="eos "

if __name__=="__main__": 
    if len(sys.argv)<2 or not sys.argv[1].isdigit():
        print("Usage: %s <runnumber> {outputfile}" % sys.argv[0])
        sys.exit(-1)
    

    runnumber=int(sys.argv[1])
    if len(sys.argv)>2:
        outputFile=sys.argv[2]
    else:
        outputFile="LCE_CellList_%i.txt" % runnumber

    if not os.access(os.path.dirname(outputFile),os.W_OK):
        print("ERROR, no write access to output text file",outputFile)
        sys.exit(-1)
        

    #1. Try find input file on eos
    #model path name: /eos/atlas/atlascerngroupdisk/det-larg/Tier0/temp/data15_cos/calibration_LArCellsEmpty/00265083/data15_cos.00265083.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c840/data15_cos.00265083.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c840._0001.1
    dustbin=open("/dev/null")
    #dustbin=sys.stdout

    path=None
    #for projectTag in ("data15_comm","data15_cos","data15_13TeV","data15_1beam","data15_5TeV","data15_hi"): 
    for projectTag in ("data22_comm","data22_cos","data22_13TeV","data22_1beam","data22_900GeV","data22_hip8TeV","data22_hip5TeV","data22_calib"): 
        print("Probing with project tag", projectTag)
        #Could alternativlgy get project tag from TDAQ Database
        #castorpath="/castor/cern.ch/grid/atlas/caf/atlcal/perm/lar/LArCAF/%s/calibration_LArCellsEmpty/%8.8i" % (projectTag,runnumber)
        eospath="/eos/atlas/atlascerngroupdisk/det-larg/Tier0/perm/%s/calibration_LArCellsEmpty/%8.8i/" % (projectTag,runnumber)
        #cmd=[eoscmd,"ls", eospath]
        #cmd=["env -i bash -l -c '"+eoscmd+" ls "+eospath+"'"]
        cmd = eoscmd+" ls "+eospath
        print(cmd)
        #process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=dustbin)
        #eosoutput = process.communicate()
        sRes = subprocess.getstatusoutput(cmd)
        print(sRes[1])
        output = sRes[1].split('\n')
        #post-process eos output to split at linebreaks
        #output=[]
        #for o in eosoutput:
        #    if o is not None: output+=o.split()
            
        found=False
        #if (process.returncode==0):
        if (sRes[0] == 0):
            print("output: ", output)
            for d1 in output:
                if d1.find("NTUP_SAMPLESMON")!=-1:
                    eospath+="/"+d1
                    found=True
                    break
                pass
            if not found:
                print("Directory for LCE output of run %i not found on eos" % runnumber)
                sys.exit(-1)
        
            #cmd=[eoscmd,"ls", eospath]
            cmd = eoscmd+" ls "+eospath
            #cmd=["env -i bash -l -c '"+eoscmd+" ls "+eospath+"'"]
            print("Checking path ",eospath)
            #process1 = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=dustbin)
            #eosoutput = process1.communicate()
            sRes = subprocess.getstatusoutput(cmd)
            print(sRes[1])
            output = sRes[1].split('\n')
            #output=[]
            #for o in eosoutput:
            #    if o is not None: output+=o.split()

            #if (process.returncode==0):
            if (sRes[0] == 0):
                for d2 in output:
                    print(d2)
                    if d2 is not None and d2.find("NTUP_SAMPLESMON")!=-1:
                        filename=d2
                        path="root://eosatlas/"+eospath+"/"+filename
                        break
                    pass
                pass
            if path is not None: break
            pass
        pass
    
    #dustbin.close()
    if path is not None:
        print("Found project Tag",projectTag)
        print("Input file ",path)
    else:
        print("LCE output of run %i not found on eos" % runnumber)
        sys.exit(-1)


    workdir=tempfile.mkdtemp()
    LCEFile=workdir+"/"+filename
    #cmd=[eoscmd,"cp ",path,LCEFile]
    cmd=eoscmd+" cp "+path+" "+LCEFile
    #print(cmd)
    #copyprocess=subprocess.Popen(cmd)#,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    #output=copyprocess.communicate()
    sRes = subprocess.getstatusoutput(cmd) 
    #if (process.returncode!=0 or not os.access(LCEFile,os.R_OK)):
    if (sRes[0] != 0 or not os.access(LCEFile,os.R_OK)):
        print("ERROR, failed to copy LCE ntuple to local directory")
        #print(output)
        sys.exit(-1)
        pass


    #2. Get LBs to ignore:
    requireStableBeam=projectTag.endswith("TeV")
    bulk=False
    badLBs=getLBsToIgnore(runnumber,True,bulk,requireStableBeam)
    (badLBsF,badLBsFN)=tempfile.mkstemp(text=True)
    #print(badLBsF,badLBsFN)
    os.write(badLBsF,str.encode(', '.join([ str(i) for i in sorted(badLBs) ])))
    os.write(badLBsF,str.encode("\n"))
    os.close(badLBsF)
    print(badLBsFN)


    #3. symlink LArIdTree.root
    #if not os.access("LarIdTree.root",os.F_OK):
    #    os.symlink("/afs/cern.ch/user/l/larmon/public/LarIdTree.root","LarIdTree.root")

    print("Running LCE_CellList",LCEFile,outputFile,badLBsFN)
    
    sc=subprocess.call(["LCE_CellList",LCEFile,outputFile,badLBsFN])
    
    os.remove(badLBsFN)
    os.remove(LCEFile)
