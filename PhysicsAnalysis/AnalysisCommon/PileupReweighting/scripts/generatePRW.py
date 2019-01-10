#!/usr/bin/env python

__author__ = "Will Buttinger"
__doc__ = """
Submit a task to the grid to generate PRW config files for a list of datasets.
This command ensures that the PRW are generated from the unskimmed primary AOD datasets.

Example: generatePRW.py --outDS=user.will.myPRW  --inDsTxt=datasets.txt

"""

import os
import argparse
import ROOT

def main():
    from argparse import RawTextHelpFormatter
    
    parser = argparse.ArgumentParser(description=__doc__,formatter_class=RawTextHelpFormatter)
    
    parser.add_argument('--inDsTxt',action="store",help="text file containing datasets to make PRW for (one per line) [REQUIRED]",required=True)
    parser.add_argument('--outDS',action="store",default="",help="Name of the output dataset",required=False)
    parser.add_argument('--forceStaged',action="store_true",help="If set, grid jobs will be submitted with forceStaged option")
    parser.add_argument('--skipNTUP_PILEUP',action="store_true",help="If set, will not check for existing NTUP_PILEUP datasets")
    parser.add_argument('prwFiles',nargs="*",help="Exosting PRW Config files to check")
    
    args = parser.parse_args()

    try:
      import pyAMI.atlas.api as atlasAPI
      import pyAMI.client
    except ImportError:
      print "Could not import pyAMI ... please do: lsetup pyAMI"
      print "Also ensure you have a valid certificate (voms-proxy-init -voms atlas)"
      return 1

    client = pyAMI.client.Client('atlas')
    atlasAPI.init()

    #read datasets into list
    with open(args.inDsTxt) as f: datasets = f.read().splitlines()


    
    import ROOT
    
    out = ROOT.CP.TPileupReweighting("out")
    for f in args.prwFiles:
      out.AddConfigFile(f)
    out.ResetCountingMode() #trick tool into going into counting mode 
    #list of known period numbers
    periodNumbers = out.GetPeriodNumbers()
    
    print "Determining provenances of %d datasets ..." % len(datasets)
    
    aodDatasets=[]
    ntupDatasets=[]
    for dataset in datasets:
      dataset = dataset.strip("/")
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
      
      #check input prw files, if we specified
      isIncomplete=False
      if len(args.prwFiles):
        total=0;
        dsid = theParent.split(".")[1]
        for p in periodNumbers:
          if p==-1: continue
          hist = out.GetInputHistogram(int(dsid),p)
          if hist: total += hist.GetEntries()
        
        if total==theParentSize:
            print "INFO: %s is complete in your existing PRW files. Good!" % dataset
            continue
        if total>theParentSize:
            print "WARNING: %s is suspect in your existing PRW files, has %d events when expected %d ... please check you didn't overmerge" % (dataset,total,theParentSize)
            continue
        else:
            if total!=0: 
              print "WARNING: %s is incomplete (%d events when expected %d) ... will try to find centrally produced NTUP_PILEUP or prepare to generate" % (dataset,total,theParentSize)
              isIncomplete=True
        
      #before adding the dataset, see if we can find an NTUP_PILEUP for it
      if not args.skipNTUP_PILEUP:
        ntupDatasetName = theParent.replace("AOD","NTUP_PILEUP")
        ntupDatasetName = ntupDatasetName.replace("aod","%")
        ntupDatasetName = ntupDatasetName.replace("merge","%")
        #remove everything after first rtag of ami tag .. replace with wildcard
        first_rtag_pos = ntupDatasetName.index("_r",ntupDatasetName.index("NTUP_PILEUP"))
        try:
            next_underscore_pos = ntupDatasetName.index("_",first_rtag_pos+1)
        except ValueError:
            next_underscore_pos = len(ntupDatasetName)
        ntupDatasetName = ntupDatasetName[:next_underscore_pos]+"%"
        res = atlasAPI.list_datasets(client, ntupDatasetName,fields='ldn,prodsys_status')
        foundNTUP=False
        for r in res:
          if r[u'prodsys_status']!="ALL EVENTS AVAILABLE" and (isIncomplete or r[u'prodsys_status']!="EVENTS PARTIALLY AVAILABLE"): continue
          print "Found existing NTUP_PILEUP ... please download: %s" % r[u"ldn"]
          ntupDatasets += [r[u'ldn']]
          foundNTUP=True
          break
        if foundNTUP==True: continue
        
      aodDatasets += [theParent]
    
    
    if len(aodDatasets)>0:
      if args.outDS=="":
          print "NTUP_PILEUP need generating for the following datasets, please specify the --outDS option to give a name to the output dataset"
          print ",".join(aodDatasets)
          return 1
      print "...submitting job to grid..."
  
      extraOpts=""
      if args.forceStaged: extraOpts += "--forceStaged "
  
      mycommand = """pathena --inDS="%s" --outDS="%s" PileupReweighting/generatePRW_jobOptions.py %s--mergeOutput --nGBPerJob=MAX --useContElementBoundary --addNthFieldOfInDSToLFN=2,6""" % (",".join(aodDatasets),args.outDS,extraOpts)
      
      print "Command: %s" % mycommand
      
      from subprocess import call
      if call(mycommand,shell=True)!=0:
         print "Problem executing command. Did you remember to do: lsetup panda"
         return 1
  
  
      print "... finished. Please monitor your job on the grid, and when it is finished, download the files!"
    
    if len(ntupDatasets):
      frucio_fn = 'rucio_downloads_%s.sh' % args.inDsTxt
      print "Please download existing config files from these datasets (see also output file %s):" % frucio_fn
      with open(frucio_fn, 'w') as frucio:
        for ds in ntupDatasets:
          command = "rucio download %s" % ds
          print command
          frucio.write(command + '\n')
      print ""
    
    if len(ntupDatasets) or len(aodDatasets):
      print "After downloading, you are advised to check they are complete: checkPRW.py --inDsTxt=%s <downloaded files> " % args.inDsTxt
      print "Thank you for generating config files, you get a gold star!"
    else:
      print "Looks like you are ready for pileup reweighting!"

    return 0


if __name__ == "__main__":

    os._exit(main())

