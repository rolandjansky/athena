# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""
Validation of Coherent Prescale Groups
- Python check that all Coherent Prescale group assignments are reasonable
- Allows early failure in the case of invalid groups, such as:
  - Only one chain in CPS group
  - Multiple L1 inputs for same CPS group
  - CPS group not matching L1 name

Author: Teng Jian Khoo
"""

__doc__="Validation of coherent prescales set on the list of chains"

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

def checkCPSGroups(chainDicts):

  cps_to_chain = {}
  for hlt in chainDicts:
    # Extract all CPS items
    for group in hlt["groups"]:
      if group.startswith('RATE:CPS'):        
        # Add if not existing
        if group not in cps_to_chain:
          cps_to_chain[group] = []
        cps_to_chain[group].append(hlt)
  
  CPS_OK = True
  for CPS_group, HLT_list in cps_to_chain.items():
    CPS_item = CPS_group.split('CPS_',1)[1]

    # Checks for each CPS item
    if len(HLT_list)<2:
      log.error("CPS group %s contains too few chains %s", CPS_group, [hlt['chainName'] for hlt in HLT_list])
      CPS_OK = False

    # Checks for every chain
    for hlt in HLT_list:
      # Can't apply CPS to multi-seed L1
      if ',' in hlt['L1item']:
        log.error("CPS group %s contains HLT chain %s with multi-seed L1 item", CPS_group, hlt['chainName'])
        CPS_OK = False

      # Verify L1 item matches CPS group
      if CPS_item != hlt['L1item'][3:]:
        log.error("CPS group %s does not match L1 item %s", CPS_group, hlt['L1item'])
        CPS_OK = False
      # Passing this check also implies that there is exactly 1 L1 for the CPS group

  if not CPS_OK:
    raise Exception(f"Invalid CPS group assignments found")
