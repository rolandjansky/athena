#!/usr/bin/env python
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Module to collect JSON specific trigger configuration helpers
#
import pickle
import json

def create_joboptions_json(pkl_file, json_file):
   """Create the configuration JSON file from the properties in `pkl_file`
   and save it into `json_file`.
   """

   with open(pkl_file, "rb") as f:
      jocat = pickle.load(f)   # basic job properties
      jocfg = pickle.load(f)   # some specialized services
      jocat.update(jocfg)       # merge the two dictionaries

      hlt_json = {'filetype' : 'joboptions'}
      hlt_json['properties'] = jocat


   with open(json_file, "w") as f:
      json.dump(hlt_json, f, indent=4, sort_keys=True, ensure_ascii=True)


   """ also create a configuration JSON file that can be uploaded
   to the triggerdb for running at P1
   """
   assert json_file[-5:] == ".json"
   db_file = json_file.replace(".json", ".db.json")

   modifyConfigForP1(json_file, db_file)


def modifyConfigForP1(json_file, db_file):
   """ modifies a number of job properties to run from the TriggerDB and writes out modified JSON
   """

   with open(json_file, 'r') as f:
      jocat = json.load(f)
      properties = jocat['properties']

      def mod(props, alg, prop, fnc):
         if alg in props and prop in props[alg]:
            origVal = props[alg][prop]
         else:
            origVal = ""
         props[alg][prop] = fnc(origVal)

      # L1 and HLT Config Svc must read from db
      mod( properties, "LVL1ConfigSvc", "InputType", lambda x : "db" )
      mod( properties, "HLTConfigSvc", "InputType", lambda x : "db" )
      mod( properties, "HLTPrescaleCondAlg", "Source", lambda x : "COOL" ) # prescales will be read from COOL online
      mod( properties, "HLTPrescaleCondAlg", "TriggerDB", lambda x : "JOSVC" ) # configuration will be taken from the JOSvc at P1
      # remove filenames to avoid duplicates
      mod( properties, "LVL1ConfigSvc", "JsonFileName", lambda x : "None" )
      mod( properties, "TrigConf__BunchGroupCondAlg", "Filename", lambda x : "None" )
      mod( properties, "HLTConfigSvc", "JsonFileName", lambda x : "None" )
      mod( properties, "HLTConfigSvc", "MonitoringJsonFileName", lambda x : "None" )
      mod( properties, "HLTPrescaleCondAlg", "Filename", lambda x : "None" )

   with open(db_file,'w') as f:
      json.dump(jocat, f, indent=4, sort_keys=True, ensure_ascii=True)


if __name__ == "__main__":
   # this is to test the modification of the JO
   import sys
   if len(sys.argv)<2:
      print("Please run\n%s <HLTJobOptions.json>" % sys.argv[0].split("/")[-1])
      sys.exit(1)
   with open(sys.argv[1]) as fh:
      hlt_json = json.load( fh )
      properties = hlt_json["properties"]

   modifyConfigForP1(sys.argv[1], sys.argv[1].replace("json", "db.json"))

   outfn = sys.argv[1].replace(".json",".test.json")
   with open(outfn,'w') as f:
      hlt_json['properties'] = properties
      json.dump(hlt_json, f, indent=2, sort_keys=True, ensure_ascii=True)
      print("Wrote %s" % outfn)
