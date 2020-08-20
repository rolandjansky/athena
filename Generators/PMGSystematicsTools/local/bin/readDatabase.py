#!/usr/bin/env python
#Author: Abhishek Nag (TU Dresden)
#email: abhishek.nag@cern.ch
import os, sys, yaml
import argparse


SystToolsPath="/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PMGTools/SystematicsDatabase/"
if os.path.exists(SystToolsPath+"DSID_Database.yaml"):
  pass
  #print("[INFO] found syst database path at %s"%SystToolsPath )
elif 'SYSTTOOLSPATH' in os.environ.keys():
  print("[WARNING] Could not find syst tools path on cvfms, defaulting to one in %s/data/"%os.environ['SYSTTOOLSPATH'])
  SystToolsPath='/data/'+os.environ['SYSTTOOLSPATH']
else:
  print("[ERROR] Environment variable SYSTTOOLSPATH  is not set. It should be set to the systematics-tools directory. Use setupSystematicsTool.sh")
  exit(1)

def getWeights(dsid):
  info={}
  keys=[]
  with open("%s/DSID_Database.yaml"%SystToolsPath) as f:
      database=yaml.load(f)
      if any(str(dsid)==k for k in database.keys()):
          keys=database.get(str(dsid))
          #print(keys)
          if keys == ['nominal']:
              info.update({"nominal":"nominal"})
          else:
              with open('%s/Weight_Database.yaml'%SystToolsPath) as d:
                  weight_data = yaml.load(d)
                  for key in keys:
                      if key in weight_data.keys():
                          info.update({key:weight_data.get(key)})
                      else:
                          print(key+' key for '+str(dsid)+' not in Weight Databse')
      else:
          print((str(dsid) + ' not in Database'))
  return info, keys

def getOrderedWeights(dsid):
  res={}
  for fn in os.listdir("%s/Weight_files"%SystToolsPath):
    print(fn, str(dsid), (str(dsid) in fn))

    if str(dsid) in fn:
      with open("%s/Weight_files/%s"%(SystToolsPath,fn)) as f:
        counter=0
        for line in f.readlines():
          line=line.strip()
          res[line]=counter
          counter+=1
      break
  if len(res)==0:
    print((str(dsid) + ' not in Database'))
  return res
                  
def main(argv):
  parser = argparse.ArgumentParser( description = 'Variation string needed' )
  parser.add_argument('-d', '--dsid',  default = 410425, type=int,  help = 'six digit DSID', )
  args = parser.parse_args()
  dictionary, keys=getWeights(args.dsid)
  print(keys)
  print(dictionary)

if __name__ == "__main__":
  main( sys.argv[1:] )

