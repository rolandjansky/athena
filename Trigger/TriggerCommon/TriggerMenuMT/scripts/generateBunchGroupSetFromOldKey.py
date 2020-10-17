#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import sys
import json

from TriggerMenuMT.L1.Base.BunchGroupSet import BunchGroupSet
from TriggerMenuMT.L1.Base.L1MenuFlags import L1MenuFlags
from AthenaCommon.Logging import logging
log = logging.getLogger("generateBunchGroupSetFromOldKey")

L1MenuFlags.CTPVersion = 4
L1MenuFlags.BunchGroupPartitioning = [1, 15, 15]

bgsname = "All_FilledEmpty_expCalreq"

bgnames = [
    "BCRVETO",
    "Filled",
    "Calreq",
    "Empty",
    "UnpairedIsolated",
    "UnpairedNonisolated",
    "EmptyAfterFilled",
    "Unpaired",
    "NotUsed",
    "NotUsed",
    "NotUsed",
    "NotUsed",
    "NotUsed",
    "NotUsed",
    "NotUsed",
    "NotUsed"
]

def transform2(oldbgs):
    newbgs = BunchGroupSet(bgsname)
    for idx,bg in enumerate(oldbgs):
        newbg = BunchGroupSet.BunchGroup(name = bgnames[idx], internalNumber = idx)
        for b in bg:
            newbg.addBunch(b)
        newbg.normalize()
        newbgs.addBunchGroup(newbg)
    return newbgs

def main():

    if len(sys.argv)<2:
        print("Please run\n%s <old-style.json>\n" % sys.argv[0].split('/')[-1]) # noqa: ATL901
        print("The old-style json file can be downloaded from the web using https://atlas-trigconf.cern.ch/bunchgroups?key=<key>&type=json") # noqa: ATL901
        return 1
        
    inputFN = sys.argv[1]

    with open(inputFN) as fp:
        oldBGS = json.load(fp)[0]['code']
        newBGS = transform2(oldBGS)

    outputFN = inputFN.replace(".json",".newstyle.json")
    newBGS.writeJSON(outputFN)

    

if __name__ == "__main__":
    sys.exit(main())



