#!/usr/bin/env python

import json
import sys
import os, os.path

testStatus = int(os.environ["testStatus"])
if testStatus != 0:
    print(f"Post evaluation: ERROR: previous step went wrong (exit code {testStatus})")
    sys.exit(testStatus)

if os.path.exists("NoDBConnection.txt"):
    # in case we don't have a db connection we don't run this test
    sys.exit(0)


foundUnequal=False

for ft in ["L1Menu", "L1Prescales", "Bunchgroups", "HLTMenu", "HLTPrescales"]:
    with open("%s1.json" % ft, 'r') as fh1, open("%s2.json" % ft, 'r') as fh2:
        j1 = json.load(fh1)
        j2 = json.load(fh2)

        if j1 == j2:
            print(f"Post evaluation: Files {ft}1.json and {ft}2.json are identical")
        else:
            print(f"Post evaluation: ERROR: Files {ft}1.json and {ft}2.json are not identical !!")
            foundUnequal = True

if foundUnequal:
    sys.exit(1)
else:
    sys.exit(0)
