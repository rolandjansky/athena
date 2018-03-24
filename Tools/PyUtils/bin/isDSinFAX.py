#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import subprocess, threading, os, sys 
import urllib2
import json

import argparse
 
parser = argparse.ArgumentParser(description='Checks if dataset is accessible through FAX.')
parser.add_argument('dataset', type=str, help='Dataset name')
parser.add_argument('-af','--accessfile', action='store_const', const='1', help='try to open the first root file of the dataset using root. ')
parser.add_argument('-aa','--accessall', action='store_const', const='1', help='try to open all the root files of the dataset using root. ')

args = vars(parser.parse_args())

try:
    import dq2.clientapi.cli.cliutil
    from dq2.common.cli.DQDashboardTool import DQDashboardTool
    from dq2.clientapi.cli.cliutil import getDQ2
    from dq2.filecatalog.lfc.lfcconventions import to_native_lfn
except ImportError:
    print "Environment not set [error importing DQ2 dependencies]!"
    sys.exit(1)


class Command(object):
    
    def __init__(self, cmd):
        self.cmd = cmd
        self.process = None
    
    def run(self, timeout):
        def target():
#            print 'command started: ', self.cmd
            self.process = subprocess.Popen(self.cmd, shell=True)
            self.process.communicate()
        
        thread = threading.Thread(target=target)
        thread.start()
        
        thread.join(timeout)
        if thread.is_alive():
            print 'Terminating process'
            self.process.terminate()
            thread.join()
        return self.process.returncode


#print 'Geting data from AGIS ...'

class site:    
    name=''
    host=''
    port=1094
     
    def __init__(self, na, ho):
        self.name=na
        ho=ho.replace("root://","")
        self.host=ho.split(":")[0]
        if ho.count(":"):
            self.port=ho.split(":")[1]
    
    def prnt(self, what):
        if (what>=0 and self.redirector!=what): return
        print  'name:', self.name, '\thost:', self.host, '\tport:', self.port 

sites=[];

try:
    req = urllib2.Request("http://atlas-agis-api-0.cern.ch/request/service/query/get_se_services/?json&state=ACTIVE&flavour=XROOTD", None)
    opener = urllib2.build_opener()
    f = opener.open(req)
    res=json.load(f)
    for s in res:
#        print s["name"], s["rc_site"], s["endpoint"]
        ns = site( s["rc_site"], s["endpoint"] )
        sites.append(ns)
#    print res
#    print ' got FAX SEs from AGIS.'

except:
    print "Unexpected error:", sys.exc_info()[0]    

#for s in sites: s.prnt(-1)

allddms=set()

try:
    req = urllib2.Request("http://atlas-agis-api-0.cern.ch/request/ddmendpoint/query/list/?json&state=ACTIVE", None)
    opener = urllib2.build_opener()
    f = opener.open(req)
    res=json.load(f)
    for s in res:
        for c in sites:
            if s["rc_site"]==c.name:
 #               print s["rc_site"], s["name"]
                allddms.add(s["name"])
                break
#    print ' got related ddmendpoints from agis.'

except:
    print "Unexpected error:", sys.exc_info()[0]    


DS=args['dataset']

com=Command('dq2-ls -r '+ DS + ' > fax.tmp' )
com.run(300)
dsets={}
cds=''
f = open('fax.tmp', 'r') 
for line in f:
    if line.startswith('Multiple'): break
    line=line.strip()
    if line.count(':')==0:continue
    line=line.split(":")
  #  print line

    if line[0]=='INCOMPLETE':
        if len(line[1])==0: continue
        rep=line[1].split(',')
        for r in rep:
            if r in allddms:
                dsets[cds][0]+=1
        continue
  
    if line[0]=='COMPLETE':
        if len(line[1])==0: continue
        rep=line[1].split(',')
        for r in rep:
            if r in allddms:
                dsets[cds][1]+=1 
        continue  
    
    cds=line[0]
    dsets[cds]=[0,0]

for d  in dsets.keys():
    print d,'\tcomplete replicas:',dsets[d][1],'\tincomplete:',dsets[d][0]
