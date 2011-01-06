#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# vim: set fileencoding=utf-8 :
# Created by Andre Anjos <andre.dos.anjos@cern.ch>
# Thu 04 Oct 2007 05:04:39 PM CEST 

"""Adds and/or modified options from partition/segment generators to add HLT.
"""
from pm.option.specific import option as pmopt
option = {}

option['l2-hlt'] = dict(pmopt['l2-hlt'])
option['l2-hlt']['default'] = {'jobOptionsPath': 'TrigExMTHelloWorld/MTHelloWorldOptions.py'}
option['l2-hlt']['description'] = 'Defines the L2 HLT Implementation to use. It has to be either a python dictionary (like the default argument), a dictionary that configures an HLTImplementation, or a string, that determines the path to the jobOptions to use. This parameter can also be set to dal.HLTImplementation object that will be used blindly to configure the L2 system at your partition.' 

option['ef-hlt'] = dict(pmopt['ef-hlt'])
option['ef-hlt']['default'] = option['l2-hlt']['default']
option['ef-hlt']['description'] = 'Defines the EF HLT Implementation to use. It has to be either a python dictionary (like the default argument), a dictionary that configures an HLTImplementation, or a string, that determines the path to the jobOptions to use. This parameter can also be set to dal.HLTImplementation object that will be used blindly to configure the EF system at your partition.' 

option['hlt-extra-path'] = {'short': 'K', 'arg': True, 'default': [],
  'group': option['l2-hlt']['group'],
  'description': 'Defines a set of extra paths that should contain valid installation areas where you have patches for the HLT/Offline software. The order you set is preserved. These areas are attached both to L2 and EF HLT nodes.'}
  
option['proxy'] = {'short': 'Y', 'arg': False, 'default': None,
  'group': option['l2-hlt']['group'],
  'description': 'If you set this option, we will populate the generated segment or partition with coral proxies where they fit. If you do not set this option, the default action is not to place any proxies.'}
