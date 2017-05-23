#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 
from L1TopoMenu import *

if __name__=="__main__":
    
    menu = L1TopoMenu("TRIGGERDB", 2346)
    print menu
    print "############### algorithms #################"
    for algo in menu.algorithms:
        print algo
    print "############### output lines ###############"
    for line in menu.outputLines:
        print line

