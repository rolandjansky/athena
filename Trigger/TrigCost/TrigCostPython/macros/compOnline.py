#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import subprocess
import string
import sys

def check_output(args):
    print "Running: ",string.join(args)
    proc = subprocess.Popen(args, shell=False, stdout=subprocess.PIPE)
    return proc.communicate()

basecmd = ["exampleCost.py","--trp",
           "--run","x","--lb-beg","y","--lb-end","z",
           "--writexml","trp.xml"]

lbbeg=sys.argv[2]
lbend=sys.argv[2]
if len(sys.argv)==7:
    lbend=str(string.atoi(lbend)+string.atoi(sys.argv[6]))

basecmd[3]=sys.argv[1] # run
basecmd[5]=lbbeg
basecmd[7]=lbend

(stdin,stderr)=check_output(basecmd)
print stdin
print "--------------------------------------------------"
print stderr
trp1 = "trp_"+sys.argv[1]+"_"+lbbeg+"_"+lbend+".xml"

lbbeg=sys.argv[4]
lbend=sys.argv[4]
if len(sys.argv)==7:
    lbend=str(string.atoi(lbend)+string.atoi(sys.argv[6]))

basecmd[3]=sys.argv[3] # run
basecmd[5]=lbbeg
basecmd[7]=lbend

(stdin,stderr)=check_output(basecmd)
trp2 = "trp_"+sys.argv[3]+"_"+lbbeg+"_"+lbend+".xml"

compcmd = ["compRates.py",trp1,trp2,"-o",
           sys.argv[5]+"_r"+sys.argv[1]+"_l"+sys.argv[2]+"_r"+sys.argv[3]+"_l"+sys.argv[4],
           "--labels","r"+sys.argv[1]+"_l"+sys.argv[2]+",r"+sys.argv[3]+"_l"+sys.argv[4],
           "--ratio-columns=2","--show-err"]
(stdin,stderr)=check_output(compcmd)
