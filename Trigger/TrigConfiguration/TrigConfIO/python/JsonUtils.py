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
