#!/usr/bin/env python

__author__ = "Will Buttinger"
__doc__ = """
Submit a task to the grid to generate PRW config files for a list of datasets.
This command ensures that the PRW are generated from the unskimmed primary AOD datasets.

Example: generatePRW.py --outDS=user.will.myPRW  --inDsTxt=datasets.txt

"""

import os
import argparse


def main():
    from argparse import RawTextHelpFormatter
    
    parser = argparse.ArgumentParser(description=__doc__,formatter_class=RawTextHelpFormatter)
    parser.add_argument('--outDS',action="store",help="Name of the output dataset",required=True)
    parser.add_argument('--inDsTxt',action="store",help="text file containing datasets to make PRW for (one per line)",required=True)
    parser.add_argument('--forceStaged',action="store_true",help="If set, grid jobs will be submitted with forceStaged option")
    parser.add_argument('--skipNTUP_PILEUP',action="store_true",help="If set, will not check for existing NTUP_PILEUP datasets")
    
    args = parser.parse_args()

    import pyAMI.atlas.api as atlasAPI
    import pyAMI.client

    client = pyAMI.client.Client('atlas')
    atlasAPI.init()

    #read datasets into list
    with open(args.inDsTxt) as f: datasets = f.read().splitlines()

    
    print "Determining provenances of %d datasets ..." % len(datasets)
    
    aodDatasets=[]
    ntupDatasets=[]
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
        break
      if theParent=="":
          print "ERROR: Could not determine provenance of %s, skipping!" % dataset
          continue
      
      #before adding the dataset, see if we can find an NTUP_PILEUP for it
      if not args.skipNTUP_PILEUP:
        ntupDatasetName = theParent.replace("AOD","NTUP_PILEUP")
        ntupDatasetName = ntupDatasetName.replace("aod","%")
        ntupDatasetName = ntupDatasetName.replace("merge","%")
        #remove everything after first rtag of ami tag .. replace with wildcard
        first_rtag_pos = ntupDatasetName.index("_r",ntupDatasetName.index("NTUP_PILEUP"))
        next_underscore_pos = ntupDatasetName.index("_",first_rtag_pos+1)
        ntupDatasetName = ntupDatasetName[:next_underscore_pos]+"%"
        res = atlasAPI.list_datasets(client, ntupDatasetName,fields='ldn,prodsys_status')
        foundNTUP=False
        for r in res:
          if r[u'prodsys_status']!="ALL EVENTS AVAILABLE": continue
          print "Found existing NTUP_PILEUP ... please download: %s" % r[u"ldn"]
          ntupDatasets += [r[u'ldn']]
          foundNTUP=True
          break
        if foundNTUP==True: continue
        
      aodDatasets += [theParent]
    
    
    if len(aodDatasets)>0:
      print "...submitting job to grid..."
  
      extraOpts=""
      if args.forceStaged: extraOpts += "--forceStaged "
  
      mycommand = """pathena --inDS="%s" --outDS="%s" PileupReweighting/generatePRW_jobOptions.py %s--mergeOutput --nGBPerJob=MAX --useContElementBoundary --addNthFieldOfInDSToLFN=2,6""" % (",".join(aodDatasets),args.outDS,extraOpts)
      
      print "Command: %s" % mycommand
      
      from subprocess import call
      call(mycommand,shell=True)
  
  
      print "... finished. Please monitor your job on the grid, and when it is finished, download the files!"
    
    if len(ntupDatasets):
      print "Please download existing config files from these datasets:"
      for ds in ntupDatasets:
        print "rucio download %s" % ds
      print ""
    print "After downloading, you are advised to check they are complete: checkPRW.py --inDsTxt=%s <downloaded files> " % args.inDsTxt
    print "Thank you for generating config files, you get a gold star!"

    return 0


if __name__ == "__main__":

    try:
      os._exit(main())
    except:
      print "There was an error ... please ensure you have done the following before running the script:"
      print "lsetup pyAMI panda"
      print "voms-proxy-init -voms atlas"
