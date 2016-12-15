# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

triggerMenuXML_dicts = [
    {'EBstep': -1,
     'signatures': '',
     'stream': ['Main'],
     'chainParts': [
         {'trigType': 'j',
          # 'extra': 'test4',
          'extra': '',
          'etaRange': '0eta490',
          'gscThreshold': '',
          # 'threshold': '40',
          'threshold': '8',
          # 'chainPartName': '2j40_0eta490_invm250_test4',
          'chainPartName': '2j8_0eta490_invm250_test4',
          'recoAlg': 'a4',
          'bTag': '',
          'scan': 'FS',
          'calib': 'em',
          'bMatching': [],
          'L1item': '',
          'bTracking': '',
          'dataType': 'tc',
          'jetCalib': 'subjes',
          'topo': ['invm250'],
          'TLA': '',
          'cleaning': 'noCleaning',
          'bConfig': [],
          'multiplicity': '2',
          'signature': 'Jet',
          'addInfo': [],
          'dataScouting': ''}
     ],
     
     'topo': [],
     'chainCounter': 842,
     'groups': ['RATE:MultiJet', 'BW:Jet'],
     'signature': 'Jet',
     'topoThreshold': None,
     'topoStartFrom': False,
     'L1item': 'L1_XE35_MJJ-200',
     'chainName': '2j8_0eta490_invm250_test4'},

    {'EBstep': -1, 'signatures': '', 'stream': ['Main'], 'chainParts': [{'trigType': 'j', 'extra': '', 'etaRange': '0eta490', 'gscThreshold': '', 'threshold': '40', 'chainPartName': '2j40_0eta490_invm250', 'recoAlg': 'a4', 'bTag': '', 'scan': 'FS', 'calib': 'em', 'bMatching': [], 'L1item': '', 'bTracking': '', 'dataType': 'tc', 'jetCalib': 'subjes', 'topo': ['invm250'], 'TLA': '', 'cleaning': 'noCleaning', 'bConfig': [], 'multiplicity': '2', 'signature': 'Jet', 'addInfo': [], 'dataScouting': ''}], 'topo': [], 'chainCounter': 841, 'groups': ['RATE:MultiJet', 'BW:Jet'], 'signature': 'Jet', 'topoThreshold': None, 'topoStartFrom': False, 'L1item': 'L1_XE35_MJJ-200', 'chainName': '2j40_0eta490_invm250'},
    
#    {'EBstep': -1, 'signatures': '', 'stream': ['Main'], 'chainParts': [{'bTag': '', 'scan': 'FS', 'etaRange': '0eta320', 'threshold': '0', 'chainPartName': 'j0_1i2c500m700TLA', 'recoAlg': 'a4', 'trigType': 'j', 'extra': 'test4', 'calib': 'em', 'bMatching': [], 'L1item': '', 'bTracking': '', 'dataType': 'tc', 'jetCalib': 'subjes', 'topo': [], 'TLA': '1i2c500m700TLA', 'cleaning': 'noCleaning', 'bConfig': [], 'multiplicity': '1', 'signature': 'Jet', 'addInfo': [], 'dataScouting': ''}], 'topo': [], 'groups': ['RATE:MultiJet', 'BW:Jet'], 'topoThreshold': None, 'topoStartFrom': False, 'chainCounter': 658, 'signature': 'Jet', 'L1item': 'L1_J100', 'chainName': 'j0_1i2c500m700TLA_test4'},

#    {'EBstep': -1, 'signatures': '', 'stream': ['Main'], 'chainParts': [{'bTag': '', 'scan': 'FS', 'etaRange': '0eta320', 'threshold': '85', 'chainPartName': 'j85', 'recoAlg': 'a4', 'trigType': 'j', 'extra': 'test4', 'calib': 'em', 'bMatching': [], 'L1item': '', 'bTracking': '', 'dataType': 'tc', 'jetCalib': 'subjes', 'topo': [], 'TLA': '', 'cleaning': 'noCleaning', 'bConfig': [], 'multiplicity': '1', 'signature': 'Jet', 'addInfo': [], 'dataScouting': ''}], 'topo': [], 'groups': ['RATE:SingleJet', 'BW:Jet'], 'topoThreshold': None, 'topoStartFrom': False, 'chainCounter': 475, 'signature': 'Jet', 'L1item': 'L1_J20', 'chainName': 'j85_test4'},

#    {'EBstep': -1, 'signatures': '', 'stream': ['Main'], 'chainParts': [{'bTag': '', 'scan': 'FS', 'etaRange': '0eta490', 'threshold': '40', 'chainPartName': '2j40_0eta490_invm250', 'recoAlg': 'a4', 'trigType': 'j', 'extra': 'test4', 'calib': 'em', 'bMatching': [], 'L1item': '', 'bTracking': '', 'dataType': 'tc', 'jetCalib': 'subjes', 'topo': ['invm250'], 'TLA': '', 'cleaning': 'noCleaning', 'bConfig': [], 'multiplicity': '2', 'signature': 'Jet', 'addInfo': [], 'dataScouting': ''}], 'mergingOrder': ['2j40_0eta490_invm250', 'xe80_L1XE70'], 'topo': [], 'mergingOffset': -1, 'groups': ['RATE:JetMET', 'BW:Jets'], 'topoThreshold': None, 'topoStartFrom': False, 'mergingStrategy': 'serial', 'chainCounter': 2354, 'signature': 'Jet', 'L1item': 'L1_XE70', 'chainName': '2j40_0eta490_invm250_xe80_L1XE70_test4'}
]
