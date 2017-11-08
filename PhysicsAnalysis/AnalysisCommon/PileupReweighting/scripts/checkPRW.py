#!/usr/bin/env python

__author__ = "Will Buttinger"
__doc__ = """
This script helps you check your config files are complete. 
You can also use it to output a single PRW config file containing just your datasets.

Example: checkPRW.py --outPRWFile=my.prw.root --inDsTxt=my.datasets.txt  path/to/prwConfigs/*.root

"""


def main():
    import argparse
    from argparse import RawTextHelpFormatter
    
    parser = argparse.ArgumentParser(description=__doc__,formatter_class=RawTextHelpFormatter)
    parser.add_argument('--outPRWFile',action="store",help="OPTIONAL Name of the output prw file containing valid configs",required=False)
    parser.add_argument('--inDsTxt',action="store",help="text file containing datasets to make PRW for (one per line)",required=True)
    parser.add_argument('prwFiles',nargs="+",help="PRW Config files to scan")
    
    args = parser.parse_args()

    import pyAMI.atlas.api as atlasAPI
    import pyAMI.client

    client = pyAMI.client.Client('atlas')
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
      for ds in prov['node']:
        if ds[u'dataType']!=u'AOD': continue
        theParent = str(ds[u'logicalDatasetName'])
        theParentSize = int(ds[u'events'])
        break
      if theParent=="":
          print "ERROR: Could not determine provenance of %s, skipping!" % dataset
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
        out.RemoveChannel(int(dsid))
      
      
    
    if args.outPRWFile:
        out.Initialize();
        out.WriteToFile(args.outPRWFile);
    
    
    return 0


if __name__ == "__main__":
    import sys
    try:
      sys.exit(main())
    except:
      print "There was an error ... please ensure you have done the following before running the script:"
      print "lsetup pyAMI"
      print "voms-proxy-init -voms atlas"
