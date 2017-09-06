#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os
import time
import PyUtils.dbsqlite as dbsqlite
import re
import argparse
import PyUtils.RootUtils as ru
#import pdb
from EventIndexProducer.compressB64 import compressB64
from EventIndexProducer.EI_Lib import EIrecord, IOV


def options(argv):

    parser = argparse.ArgumentParser(description='Tag to EI processor')
    parser.add_argument('-v','--verbose', action='count', help='Verbosity level')
    parser.add_argument("-d","--debug", action='store_true', default=False,help="Debug messages")
    parser.add_argument("-m","--evtmax", type=int,default=0, help="Max events to read")
    parser.add_argument("input", help="Input TAG file")
    parser.add_argument("output", help="Output EI file")

    return parser.parse_args()


def _import_ROOT():
    import sys
    sys.argv.insert(1, '-b')
    import ROOT
    del sys.argv[1]
    return ROOT

def Tagfile(fname):

    #ROOT = ru.import_root()
    ROOT = _import_ROOT()
    
    f = ROOT.TFile.Open (fname, "read")
    assert not f.IsZombie() and f.IsOpen(), \
        "problem opening POOL file [{}]".format(fname)

    tree_name="CollectionMetadata"
    
    t = f.Get(tree_name)
    assert isinstance(t, ROOT.TTree), \
        "could not retrieve tree [{}]".format(tree_name)
    
    CollectionMetadata={}
    nentries = t.GetEntries()
    for i in xrange(nentries):
        t.GetEntry (i)
        k=getattr(t,"Key")
        v=getattr(t,"Value")
        
        # take string until \0 is found
        n=k.find('\0')
        k=k[:n]
        
        n=v.find('\0')
        v=v[:n]
        
        #print k,v
        CollectionMetadata[k]=v
    
    keys = [k.GetName() for k in f.GetListOfKeys()]
    #print "keys"
    #pprint (keys)
        
    tree_name="POOLCollectionTree"
    if tree_name not in keys and "CollectionTree" not in keys:
        err= "::: error: neither [{}] nor [CollectionTree] in file [{}]".format(
            tree_name, fname)
        raise RuntimeError(err)
    # try the backward compat. hack
    if tree_name not in keys:
        tree_name = "CollectionTree"
        
    t = f.Get(tree_name)
    assert isinstance(t, ROOT.TTree), \
        "could not retrieve tree [{}]".format(tree_name)
    
    branches = [str(b.GetName()) for b in t.GetListOfBranches()]
    #print "== branches"
    #pprint(branches)
    
    
    # get all leaves 
    leaves = t.GetListOfLeaves()
    #print "== leaves"
    #pprint (leaves)
    
    # disable everything...
    t.SetBranchStatus ("*", 0)
    #activate only baskets wich are of interest
    for ref in ( 'RunNumber', 'EventNumber', 'LumiBlockN', 'EventTime', 'EventTimeNanoSec', 'EventWeight', 
                 'BunchId', 'Token', 'StreamAOD_ref', 'StreamESD_ref', 'StreamRAW_ref','IsSimulation','IsCalibration','IsTestBeam' ):
        t.SetBranchStatus (ref, 1)
    # activate trigger baskets
    tbranches = [ k for k in branches if k.find("PassedTrigMask") != -1]
    for ref in tbranches:
        t.SetBranchStatus (ref, 1)
    
    class PyListOfLeaves(dict) :
        pass
    
    # create an istance
    pyl = PyListOfLeaves()
    
    # add leaves as attributes
    for i in range(0,leaves.GetEntries() ) :
        leaf = leaves.At(i)
        name = leaf.GetName()
        #print "== leaf"
        #pprint(name)
        #print leaf.GetTypeName()
        pyl.__setattr__(name,leaf)
        #print "leaf: ",name,leaf.GetTypeName()
        
    
    return (t,pyl,CollectionMetadata)


def main():    

    # get options
    opt = options(sys.argv)

    if opt.input is None or opt.output is None:
        pass
    
    # open tagfile
    (tg, pyl, cm) = Tagfile(opt.input)
    
    nentries = tg.GetEntries()

    # open database
    if os.path.exists(opt.output):
        os.remove(opt.output)
    eif = dbsqlite.open(opt.output,flags='w')
    
    eif['StartProcTime'] = int(time.time() * 1000)
    eif['StartProcTime_0'] = int(time.time() * 1000)
    eif['Schema'] = EIrecord().getRecSchema()
    eif['Version'] = EIrecord().getVersion()
    eif['TaskID'] = os.getenv('TaskID', 0)
    eif['JobID'] = os.getenv('JobID', 0)

    #processing options
    eif['ProvenanceRef'] = True
    eif['TriggerInfo'] = True

    
    ## process TAG file
    
    iov=IOV()

    # get HLTPSK,L1PSK,SMK from metadata
    # [188921,1] - [188921,215]
    iov_interval = re.compile(r'[[](?P<r0>[0-9]+?),(?P<e0>[0-9]+?)[]] - '\
                              r'[[](?P<r1>[0-9]+?),(?P<e1>[0-9]+?)[]]').match
    #pdb.set_trace()
    Project = "-"
    physicsShort = "-"
    TAG = "-"
    for k, v in cm.iteritems():
        n=k.find('|')
        if n != -1:
            k2=k[n+1:]
            pfn=k[:n+1][4:]
        else:
            k2=k
            pfn=""
        #print k,v
        if k2.startswith("Key@HLTPSK;") or k2.startswith("Key@L1PSK;") or k2.startswith("Key@SMK;"):
            parts=k2.split(";")
            k1=parts[0][4:].rstrip(";")
            iov1=parts[1][5:].rstrip("}")
            match = iov_interval(iov1)
            if not match:
                err="BAD IOV"
                raise RuntimeError(err)
            d = match.groupdict()
            iov.add(k1,int(v),(int(d['r0']),int(d['e0']),
                          int(d['r1']),int(d['e1'])))

        # asume that all events in TAG file belongs to same Project,datasetNumber,physicsShort,prodStep,dataType,TAG
        if k2 == 'POOLCollectionID' :
            try:
                Project,datasetNumber,physicsShort,prodStep,dataType,TAG = pfn.split('.')[:6]
                iund=physicsShort.find('_')
                physicsShort = physicsShort[iund+1:]
                #print pfn.split('.')[:6]
            except:
                pass


    #print iov.dump()

    eif['ProjName_0'] = Project
    eif['TrigStream_0'] = physicsShort
    eif['AMITag_0'] = TAG
    
    
    # pool token has the format:
    # [DB=dbID][CNT=cntID][CLID=classID][TECH=technology][OID=oid.first-oid.second]
    # eg.:
    #   [DB=FEAD2AD8-111F-11E2-8080-1CCD8E80BEEF][CNT=POOLContainer(DataHeader)]
    #     [CLID=D82968A1-CF91-4320-B2DD-E0F739CBC7E6][TECH=00000202][OID=00000123-00000008]
    pool_token = re.compile(r'[[]DB=(?P<db>.*?)[]]' \
                                r'[[]CNT=(?P<cnt>.*?)[]]' \
                                r'[[]CLID=(?P<clid>.*?)[]]' \
                                r'[[]TECH=(?P<tech>.*?)[]]' \
                                r'[[]OID=(?P<oid1>.*?)-(?P<oid2>.*?)[]]' \
                                ).match


    # event loop
    tnow0=int(time.time() * 1000)
    tnow=tnow0
    for evt_idx in xrange(nentries):

        if opt.evtmax != 0 and evt_idx >= opt.evtmax:
            break

        if opt.verbose > 0 and evt_idx%1000 == 0:
            tnow1=int(time.time() * 1000)
            sys.stderr.write("{:8d} {:5d} msecs\n".format(evt_idx, tnow1-tnow))
            tnow=tnow1

        tg.GetEntry (evt_idx)
        eirec = EIrecord()

        run_number=int(pyl.RunNumber.GetCnvValue())
        event_number=int(pyl.EventNumber.GetCnvValue())
        lumi_block = pyl.LumiBlockN.GetCnvValue()
        #print "run, evt", run_number,event_number

        eirec['RunNumber'] = run_number
        eirec['EventNumber'] = event_number
        eirec['LumiBlockN'] = lumi_block
        eirec["BunchId"] = pyl.BunchId.GetCnvValue()
        eirec['EventTime'] = pyl.EventTime.GetCnvValue()
        eirec['EventTimeNanoSec'] = pyl.EventTimeNanoSec.GetCnvValue()
        eirec['EventWeight'] = pyl.EventWeight.GetCnvValue()
        eirec['McChannelNumber'] = 0
        eirec['Lvl1ID'] = 0
        
        eirec['IsSimulation'] = int(pyl.IsSimulation.GetValue())
        eirec['IsTestBeam'] = int(pyl.IsTestBeam.GetValue())
        eirec['IsCalibration'] = int(pyl.IsCalibration.GetValue())


        SMK = iov.get('SMK',(run_number,event_number))
        L1PSK = iov.get('L1PSK',(run_number,lumi_block))
        HLTPSK = iov.get('HLTPSK',(run_number,lumi_block))
        #print '## SMK    ', SMK
        #print '## L1PSK  ', L1PSK
        #print '## HLTPSK ', HLTPSK
        eirec['SMK'] = SMK
        eirec['L1PSK'] = L1PSK
        eirec['HLTPSK'] = HLTPSK
        
        try:
            eirec['Sref0'] = pyl.StreamAOD_ref.GetCnvValue().rstrip('\0')
            eirec['Snam0'] = "StreamAOD"
        except:
            pass
        try:
            eirec['Sref0'] = pyl.Token.GetCnvValue().rstrip('\0')
            eirec['Snam0'] = "StreamAOD"
        except:
            pass
        try:
            eirec['Sref1'] = pyl.StreamRAW_ref.GetCnvValue().rstrip('\0')
            eirec['Snam1'] = "StreamRAW"
        except:
            pass
        try:
            eirec['Sref2'] = pyl.StreamESD_ref.GetCnvValue().rstrip('\0')
            eirec['Snam2'] = "StreamESD"
        except:
            pass


        trigL1=""
        trigL2=""
        trigEF=""
        for k in range(0,8):
            v=getattr(pyl,'L1PassedTrigMaskTBP{:d}'.format(k)).GetCnvValue()
            trigL1+="{0:032b}".format(v)[::-1]
        for k in range(0,8):
            v=getattr(pyl,'L1PassedTrigMaskTAP{:d}'.format(k)).GetCnvValue()
            trigL1+="{0:032b}".format(v)[::-1]
        for k in range(0,8):
            v=getattr(pyl,'L1PassedTrigMaskTAV{:d}'.format(k)).GetCnvValue()
            trigL1+="{0:032b}".format(v)[::-1]
        for k in range(0,32):
            v=getattr(pyl,'L2PassedTrigMask{:d}'.format(k)).GetCnvValue()
            trigL2+="{0:032b}".format(v)[::-1]
        for k in range(0,32):
            v=getattr(pyl,'EFPassedTrigMask{:d}'.format(k)).GetCnvValue()
            vs="{0:064b}".format(v)
            ef_phys=vs[:32]
            ef_inclusive=vs[32:]
            trigEF+=ef_inclusive[::-1]

        trigL1=compressB64(trigL1)
        trigL2=compressB64(trigL2)
        trigEF=compressB64(trigEF)
        eirec['L1PassedTrigMask'] = trigL1
        eirec['L2PassedTrigMask'] = trigL2
        eirec['EFPassedTrigMask'] = trigEF


        # write to db
        eif['Entry_{:d}'.format(evt_idx)] = eirec.getRec()


    eif['Nentries_0'] = evt_idx
    eif['EndProcTime_0'] = int(time.time() * 1000)
    eif['Nentries'] = evt_idx
    eif['Nfiles'] = 1
    eif['EndProcTime'] = int(time.time() * 1000)
    eif.close()
        
    tnow=int(time.time() * 1000)
    sys.stderr.write("Processed {:d} events in {:f} seconds\n".format(evt_idx, (tnow-tnow0)/1000.))
    
if __name__ == '__main__':
    main()
