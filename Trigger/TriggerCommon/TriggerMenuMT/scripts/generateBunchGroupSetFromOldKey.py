#!/usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys
import json

from TriggerMenuMT.L1.Base.BunchGroupSet import BunchGroupSet
from TriggerMenuMT.L1.Base.L1MenuFlags import L1MenuFlags
from AthenaCommon.Logging import logging
log = logging.getLogger("generateBunchGroupSetFromOldKey")

L1MenuFlags.CTPVersion = 4
L1MenuFlags.BunchGroupPartitioning = [1, 15, 15]

default_bgsname = "generateBunchGroupSetFromOldKey"

default_bgnames = [
    "BCRVETO",
    "Paired",
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

def transform2(oldbgs,bgsname,bgnames):
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

    with open(inputFN) as fn:
        inputJson = json.load(fn)

        oldBGS = inputJson[0]['code']

        if 'name' in inputJson[0]:
            bgsname = inputJson[0]['name'] + " from Run2 key "
            if 'bgkey' in inputJson[0]:
                bgsname += str(inputJson[0]['bgkey'])
        else:
            bgsname = default_bgsname

        if 'bgnames' in inputJson[0]:
            bgnames = inputJson[0]['bgnames']
            # replace 0 - BCRVETO with BCRVETO
            for i in range(len(bgnames)):
                if bgnames[i].find(" - "):
                    bgnames[i] = bgnames[i].split(" - ", 1)[1]
        else:
            bgnames = default_bgnames

        newBGS = transform2(oldBGS,bgsname,bgnames)

    outputFN = inputFN.replace(".json",".newstyle.json")
    newBGS.writeJSON(outputFN)

    

if __name__ == "__main__":
    sys.exit(main())



