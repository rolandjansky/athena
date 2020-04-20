#!/usr/bin/env python
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Module to collect JSON specific trigger configuration helpers
#
import json

def create_joboptions_json(properties, filename="HLTJobOptions.json"):
   """Create the configuration JSON file from the `properties` dictionary
   and save it into `filename`.
   """
   hlt_json = {'filetype' : 'joboptions'}
   with open(filename,'w') as f:
      hlt_json['properties'] = properties
      json.dump(hlt_json, f, sort_keys=True, indent=4)


   """ also create a configuration JSON file that can be uploaded
   to the triggerdb for running at P1
   """
   base, ending = (filename.rsplit('.',1) + ["json"])[:2]
   filenameUpload = base + ".db." + ending

   modifyConfigForP1(properties)

   with open(filenameUpload,'w') as f:
      hlt_json['properties'] = properties
      json.dump(hlt_json, f, sort_keys=True, indent=4)


def modifyConfigForP1(properties):
   """ modifies (in place) a number of job properties to run from the TriggerDB
   modification in place is OK, as the original properties are temporary
   """

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


if __name__ == "__main__":
   # this is to test the modification of the JO
   import sys
   if len(sys.argv)<2:
      print("Please run\n%s <HLTJobOptions.json>" % sys.argv[0].split("/")[-1])
      sys.exit(1)
   with open(sys.argv[1]) as fh:
      hlt_json = json.load( fh )
      properties = hlt_json["properties"]

   modifyConfigForP1(properties)

   outfn = sys.argv[1].replace(".json",".test.json")
   with open(outfn,'w') as f:
      hlt_json['properties'] = properties
      json.dump(hlt_json, f, sort_keys=True, indent=4)
      print("Wrote %s" % outfn)
