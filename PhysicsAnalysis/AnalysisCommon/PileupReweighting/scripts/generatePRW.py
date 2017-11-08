#!/usr/bin/env python

__author__ = "Will Buttinger"
__doc__ = """
Submit a task to the grid to generate PRW config files for a list of datasets.
This command ensures that the PRW are generated from the unskimmed primary AOD datasets.

Example: generatePRW.py --outDS=user.will.myPRW  --inDsTxt=datasets.txt

"""


def main():
    import argparse
    from argparse import RawTextHelpFormatter
    
    parser = argparse.ArgumentParser(description=__doc__,formatter_class=RawTextHelpFormatter)
    parser.add_argument('--outDS',action="store",help="Name of the output dataset",required=True)
    parser.add_argument('--inDsTxt',action="store",help="text file containing datasets to make PRW for (one per line)",required=True)
    
    args = parser.parse_args()

    import pyAMI.atlas.api as atlasAPI
    import pyAMI.client

    client = pyAMI.client.Client('atlas')
    atlasAPI.init()

    #read datasets into list
    with open(args.inDsTxt) as f: datasets = f.read().splitlines()

    
    print "Determining provenances of %d datasets ..." % len(datasets)
    
    aodDatasets=[]
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
      aodDatasets += [theParent]
    
    
    
    print "...submitting job to grid..."

    mycommand = """pathena --inDS="%s" --outDS="%s" PileupReweighting/generatePRW_jobOptions.py --forceStaged --mergeOutput --nGBPerJob=MAX --useContElementBoundary --addNthFieldOfInDSToLFN=2,4""" % (",".join(aodDatasets),args.outDS)
    
    print "Command: %s" % mycommand
    
    from subprocess import call
    call(mycommand,shell=True)


    print "... finished. Please monitor your job on the grid, and when it is finished, download the files!"
    print "Thank you for generating config files, you get a gold star!"

    return 0


if __name__ == "__main__":
    import sys
    try:
      sys.exit(main())
    except:
      print "There was an error ... please ensure you have done the following before running the script:"
      print "lsetup pyAMI panda"
      print "voms-proxy-init -voms atlas"
