#!/usr/bin/python

import os, sys, pprint

Tier0LogFile=sys.argv[1]
HandshakeLogFile=sys.argv[2]

CronjobFinishedRunList = [ line.split(" - ") for line in open(HandshakeLogFile) ]
Tier0FinishedRunList = [ line.split(" - ") for line in open(Tier0LogFile) ]

def is_finished( tier0Elem, CronjobFinishedRunList=CronjobFinishedRunList ) :
    flag=True
    for cron in CronjobFinishedRunList:
        
        cron_run = ( cron[3].split()[-3] ).split(".")[1]
        if cron[3].find("(dryrun)") >= 0:
            continue
        if tier0Elem[1] == cron_run:
            flag=False
            break
    
    if len(tier0Elem)!=3: flag = False
    return flag

UnfinishedRunList = filter( is_finished, Tier0FinishedRunList )

for run in UnfinishedRunList:
    print run[1]

