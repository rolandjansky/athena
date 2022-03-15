#!/usr/bin/env python

__author__ = "Will Buttinger"
__doc__ = """
This script helps you check your config files are complete. 
You can also use it to output a single PRW config file containing just your datasets.

Example: checkPRW.py --outPRWFile=my.prw.root --inDsTxt=my.datasets.txt  path/to/prwConfigs/*.root

"""
import argparse
import os
import pyAMI
import re

def main():
    from argparse import RawTextHelpFormatter
    
    parser = argparse.ArgumentParser(description=__doc__,formatter_class=RawTextHelpFormatter)
    parser.add_argument('--outPRWFile',action="store",help="OPTIONAL Name of the output prw file containing valid configs",required=False)
    parser.add_argument('--outputSuspect',action="store_true",help="allow for suspect channels to be included in the output prw file",default=False)
    parser.add_argument('--inDsTxt',action="store",help="text file containing datasets to make PRW for (one per line)",required=True)
    parser.add_argument('prwFiles',nargs="+",help="PRW Config files to scan")
    
    args = parser.parse_args()

    
    try:
      import pyAMI.atlas.api as atlasAPI
      import pyAMI.client
    except ImportError:
      print "Could not import pyAMI ... please do: lsetup pyAMI"
      print "Also ensure you have a valid certificate (voms-proxy-init -voms atlas)"
      return 1

    client = pyAMI.client.Client(['atlas', 'atlas-replica'])
    atlasAPI.init()

    #read datasets into list
    datasets=[]
    for txtFile in args.inDsTxt.split(","):
      with open(txtFile) as f: datasets += f.read().splitlines()

    
    print "Determining provenances of %d datasets ..." % len(datasets)
    
    aodDatasets=dict()
    for dataset in datasets:
      #strip the scope if it's there
      if dataset.startswith("#"): continue
      dataset = dataset.rsplit(":")[-1].strip()
      if len(dataset)==0: continue

      print "Doing %s" % dataset
      prov = atlasAPI.get_dataset_prov(client, dataset )
      if 'node' not in prov:
        print "ERROR: Could not determine provenance of %s, skipping!" % dataset
        continue

      theParent=""
      singleTagName=""
      for ds in prov['node']:
        if ds[u'dataType']!=u'AOD': continue
        dsName = ds[u'logicalDatasetName']
        if 'recon.AOD' not in ds[u'logicalDatasetName']: continue
        etags = re.findall('e[0-9]+_', dsName)
        stags = re.findall('s[0-9]+_', dsName)
        if len(etags) == 2 or len(stags) == 2:
          if len(etags) == 2:
            print "INFO: Found a double e-tag container %s!" % dsName
            dsName = dsName.replace(etags[1], "")
          if len(stags) == 2:
            print "INFO: Found a double s-tag container %s!" % dsName
            dsName = dsName.replace(stags[1], "")
          singleTagName = dsName
          continue
        theParent = str(dsName)
        theParentSize = int(ds[u'events'])
        break

      if theParent == "":
        if singleTagName == "":
          print "ERROR: No single-tag name available for %s, skipping!" % dataset
          continue
        else:
          print "INFO: Trying with single-tag containers manually %s!" % singleTagName
          try:
            prov = atlasAPI.get_dataset_prov(client, singleTagName)
          except pyAMI.exception.Error:
            print "ERROR: Could not determine provenance of %s, skipping!" % dataset
            continue
          if 'node' in prov:
            for ds in prov['node']:
              if ds[u'logicalDatasetName'] == singleTagName:
                theParent = singleTagName
                theParentSize = int(ds[u'events'])
          else:
            print "ERROR: key 'node' not found for %s, skipping!" % dataset
            continue

      #extract the dsid ...
      theParent = theParent.split(".")[1]
      
      if theParent in aodDatasets: aodDatasets[theParent] += theParentSize
      else: aodDatasets[theParent] = theParentSize
    
    #aodDatasets is now a chanNum -> eventNumber pairing ...
    
    import ROOT
    
    out = ROOT.CP.TPileupReweighting("out")
    for f in args.prwFiles:
      out.AddConfigFile(f)
    out.ResetCountingMode() #trick tool into going into counting mode 
    
    #list of known period numbers
    periodNumbers = out.GetPeriodNumbers()
    
    for dsid,nevents in aodDatasets.iteritems():
      #get the sum of weights from the tool
      
      total=0;
      for p in periodNumbers:
        if p==-1: continue
        hist = out.GetInputHistogram(int(dsid),p)
        if hist: total += hist.GetEntries()
      
      if total==nevents:
        print "channel %s is ok" % dsid
      elif total<nevents:
        print "channel %s is incomplete (missing %d events from config files)" % (dsid,nevents-total)
        out.RemoveChannel(int(dsid))
      elif total>nevents:
        print "channel %s is suspect! (config files have additional %d events)" % (dsid,total-nevents)
        if not args.outputSuspect:
          out.RemoveChannel(int(dsid))
      
      
    
    if args.outPRWFile:
        out.Initialize();
        out.WriteToFile(args.outPRWFile);
    
    
    return 0


if __name__ == "__main__":
    os._exit(main())
