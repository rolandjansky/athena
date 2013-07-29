#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###############################################################################################
##
## Transformation for Tier-0 TAG uploading
##
##  - input parameter: file containing a pickled dictionary consisting of the key/value pairs
##
##     1) 'inputTagFile': python list ['datasetname#filename1', 'datasetname#filename2', ...] 
##        (TAG input dataset+file name)
##
##     2) 'dbCollection': TAG DB output dbCollection 
##        (default is project_physics_version if datasetname given in 1) follows nomenclature, 
##        otherwise take file name w/o extension)
##
##     3) 'dbConnect': TAG DB connect string 
##        (e.g. 'oracle://int8r_tag_writer/ATLAS_TAGS_LOADING_TEST')
##
##  - uploads TAGs to dedicated database
##  - no output data files
##  - creates gpickle report file
##
## (C) Jack Cranshaw, Luc Goossens, Armin Nairz (January 2011)
##
###############################################################################################

import sys, string, commands, os.path, os, pickle, time, pprint

#from AtlasCollectionTransforms.collRename import *

def runUpload(picklefile) :

  print " "
  print " "
  print "##################################################################"
  print "######      Tier-0 TAG Upload Wrapper Transformation       #######"
  print "##################################################################"
  print " "

  os.environ['POOL_RCOLL_LOCKTIMEOUT'] = '1800'

  cmd  = "tagUpload_trf.py --argdict=%s " % picklefile 
  print cmd
  
  t1 = time.time()
  (retcode, o) = commands.getstatusoutput(cmd)
  print o 
  dt = int(time.time() - t1)

  picklefile = 'jobReport.gpickle'
  if not os.path.isfile(picklefile) :
    reportmap = { 'prodsys': { 'trfCode': 991000, 
                               'trfAcronym': 'TRF_NOPICKLE',
                               'nevents': 0,    
                               'more': {'num1': 0, 'num2': dt, 'txt1': 'no trf gpickle file'} } } 
    # pickle the report map
    f = open('jobReport.gpickle', 'w')
    pickle.dump(reportmap, f)
    f.close()

  else :
    f = open(picklefile, 'r')
    reportmap = pickle.load(f)
    f.close()
  
  print "\nContents of jobReport.gpickle file "
  pprint.pprint(reportmap) 
                                            
  print " "
  print "## ... job finished with retcode : %s, acronym: %s" % (reportmap['prodsys']['trfCode'], reportmap['prodsys']['trfAcronym'])
  print "## ... elapsed time:    ", dt, "sec"
  print "## ... events processed:", reportmap['prodsys']['nevents']
  print "##"
  print "##"
  print "##################################################################"
  print "######   End of Tier-0 TAG Upload Wrapper Transformation   #######"
  print "##################################################################"



########################################
## main()
########################################

if __name__ == "__main__":

  if (len(sys.argv) != 2) and (not sys.argv[1].startswith('--argdict=')) :
    print " Input format wrong --- use "
    print "   --argdict=<pickled-dictionary containing input info> "
    print " with key/value pairs: "
    print "   1) 'inputTagFile': python list ['datasetname#filename', ...] " 
    print "      (TAG input dataset+file names) "
    print "   2) 'dbCollection': TAG DB output dbCollection " 
    print "   3) 'dbConnect': TAG DB connect string " 
    sys.exit(-1)

  else :
    picklefile = sys.argv[1][len('--argdict='):]
    print "Using pickled file ", picklefile, " for input parameters"
    f = open(picklefile, 'r')
    map = pickle.load(f)
    f.close()

    print "\nFull Tier-0 run options "
    pprint.pprint(map)
  
    # assemble new pickle file with modified input parameters
    # Jack's tagUpload_trf doesn't accept datasetname#filename input
    newmap = {}    

    for key in map.keys() :
      if key == 'inputTagFile' : 
        flist = [] 
        for f in map['inputTagFile'] :
          fn = f.split('#')[1]
          #(path,fn2) = os.path.split(fn)
          #fn2 += ".tmp"
          #cmd = "cp -f " + fn + " " + fn2
          #print cmd
          #(s,o) = commands.getstatusoutput(cmd)
          #renameRunNumberMC(fn2)
          flist.append(fn)
        newmap['inputTagFile'] = flist
      else :
        newmap[key] = map[key]
        
    # assemble collection name from dataset name
    tagds = (map['inputTagFile'][0]).split('#')[0]
    collName = ''
    proc_id = ''
    try :
      # form collName from project and version
      collName = tagds.split('.')[0] + '_' + tagds.split('.')[5]
      # store process id
      proc_id = tagds.split('.')[1]
    except :
      collName = tagds

    # Check that inputs belong together
    for combo in map['inputTagFile']:
      tagds = combo.split('#')[0]
      try:
        if (tagds.split('.')[0] + '_' + tagds.split('.')[5]) != collName or tagds.split('.')[1] != proc_id:
          print tag+" does not belong with "+collName
          exit(1)
      except: pass
   
    # overwrite dbCollection if necessary
    if not map.has_key('dbCollection') :
      newmap['dbCollection'] = collName
    elif map['dbCollection'] == 'default' :
      newmap['dbCollection'] = collName
    else :
      pass

    # set mcarlo hack flag
    try: 
      newmap['mc12_hack'] = True
      # add process id
      if proc_id != "":
        try:
          newmap['addattr'] = ['RunNumber', 'uint',proc_id] 
        except:
          pass
      else:
        print "process id not found"
    except:
      pass

  
    print "\nDerived JobTransform run options: "
    pprint.pprint(newmap)

    newpicklefile = 'modified_input.pickle'
    f = open(newpicklefile, 'w')
    pickle.dump(newmap, f)
    f.close()
    
    # call wrapper with modified pickle file 
    runUpload(newpicklefile)
