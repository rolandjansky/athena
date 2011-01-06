#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# vim: set fileencoding=utf-8 :
# Created by Andre Anjos <andre.dos.anjos@cern.ch>
# Wed 10 Oct 2007 11:09:53 AM CEST 

# to bypass variable lenght check in OKS (do it before OKS is loaded)
import os
os.environ['OKS_SKIP_MAX_LENGTH_CHECK'] = 'true'

import socket
defaulthost = socket.getfqdn().split('.', 1)[0]

option = {}

option['hw-file'] = {'short': 'w', 'arg': True, 'default': '',
  'description': 'The hardware OKS database you want to include'}
option['node'] = {'short': 'n', 'arg': True, 'default': defaulthost,
  'description': 'Name of the node you want to place the monitoring PT at'}
option['extra-path'] = {'short': 'e', 'arg': True, 'default': [],
  'description': 'Extra installation paths you want added to the environment the monitoring node will execute at'}
option['joboptions'] = {'short': 'j', 'arg': True, 'default': '',
  'description': 'Path to the top-level jobOptions file to use for the monitoring task'}
option['prefix'] = {'short': 'p', 'arg': True, 'default': 'PTMon',
  'description': 'If set, this will prefix all OKS identifiers by the value given. This is useful to avoid clashes, if you plan to add this to an existing partition developed by others.'}
option['sfi'] = {'short': 's', 'arg': True, 'default': ['SFI-1'],
  'description': 'The names of the SFIs you want to connect to. If you give a decimal number instead of a list, the emon interface will randomly connect to the number of SFIs you define.'}
option['template-file'] = {'short': 'J', 'arg': True,
  'default': 'daq/sw/hlt-templates.data.xml',
  'description': 'Which standard template file to include in your partition. This file contains the RC and RDB templates to be used at your segment or partition.'}

short_intro = 'Generates a single OKS database with a PT monitoring segment'

def main():

  from EventApps import myopt
  import logging
  
  if len(sys.argv) < 2:
    logging.error('There should be at least one extra argument: the output file')
    parser = myopt.Parser(short_intro, extra_args=True)
    for (k,v) in option.items():
      parser.add_option(k, v['short'], v['description'], v['arg'], v['default'])
    print parser.usage()
    return 1

  # process the global options
  parser = myopt.Parser(short_intro, extra_args=True)
  for (k,v) in option.items():
    parser.add_option(k, v['short'], v['description'], v['arg'], v['default'])
  (kwargs, extra) = parser.parse(sys.argv[1:],
      prefix='"%s" options:' % sys.argv[0])

  # check for the extra argument
  if len(extra) != 1:
    logging.error('There should be one and only one extra argument: the segment file. I cannot accept:' + extra)
    parser.usage('"%s" options:' % sys.argv[0])
    return 2

  if len(kwargs['joboptions']) == 0:
    logging.warn('You have not set your jobOptions path. I\'ll continue, but you may have to set your generated database later to get a valid setup.')

  # if the user has included a hwfile, we take the description from there
  import pm.farm
  computer = pm.farm.local_computer()
  pm.farm.remap_hwtag_tdaq([computer])
  if len(kwargs['hw-file']) != 0:
    # we load with both short and long names
    hosts = pm.farm.load(kwargs['hw-file'])
    hosts.update(pm.farm.load(kwargs['hw-file'], short_names=True))
    if hosts.has_key(kwargs['node']):
      computer = hosts[kwargs['node']]
    else:
      logging.warn('You have specificed a hardware database, but I cannot find a description for %s there. I\'ll create a private representation for that computer myself. Optionally, you may review your entries.' % kwargs['node'])
      computer = pm.farm.network_computer(kwargs['node'])

  # now we generate all required stuff
  import pm.common
  import pm.ef
  import hltpm.common
  from pm.dal import dal, DFdal

  # we generate a bare PT that will be patched up later
  impl = hltpm.common.gen_implementation(kwargs['joboptions'], "EF")
  dcapp_config = pm.common.create_default_dcapp_config(kwargs['prefix'] + '-DCApplicationConfig')
  pt = pm.ef.create_pt(kwargs['prefix'] +'-PT', instances=-1, 
      dcapp_config=dcapp_config, 
      hlt_implementation=impl,
      program=hltpm.common.hltRepository.getObject('Script',
        'pt_main_wrapper'))
  if len(kwargs['extra-path']) != 0:
    patch_area = dal.Variable(kwargs['prefix'] + '-EXTRA-PATH')
    patch_area.Name = 'HLT_EXTRA_SW_PATH'
    patch_area.Value = ':'.join(kwargs['extra-path'])
    pt.ProcessEnvironment = [patch_area]
  pt.ActionTimeout = 600
  pt.IfDies = "Error"
  sampPar = DFdal.SamplingParameter(kwargs['prefix'] + '-SampPar')
  sampPar.BufferSize = 100
  sampPar.DispersalDistribution = False
  sampPar.SamplerTypes = 'SFI'
  if isinstance(kwargs['sfi'], list):
    sampPar.SamplerNames = kwargs['sfi']
  else:
    sampPar.NumberOfSamplers = int(kwargs['sfi'])
  ptioMon = DFdal.ptioMon(kwargs['prefix'] + '-PTIO')
  ptioMon.library = 'ptioMon'
  ptioMon.async = False
  ptioMon.timeout_ms = 5000
  ptioMon.samplingParameters = sampPar
  pt.PtioImplementation = ptioMon

  # loads our base template repository
  templates = pm.project.Project(kwargs['template-file']) 
  
  segment = dal.Segment(kwargs['prefix'] + '-Segment')
  hltenv = pm.project.Project('daq/segments/HLT-Environment.data.xml')
  segment.Parameters = [hltenv.getObject('VariableSet', 'HLT-Parameters')]
  segment.IsControlledBy = \
      templates.getObject("RunControlTemplateApplication", "DefRC") 
  segment.DefaultHost = computer
  segment.Resources = [pt]

  # now we make this segment HLT compliant
  includes = ['daq/schema/df.schema.xml',
              'daq/sw/repository.data.xml',
              'daq/schema/hlt.schema.xml',
              'daq/sw/HLT_SW_Repository.data.xml',
              'daq/segments/HLT-Environment.data.xml']

  if kwargs['hw-file']: includes.append(kwargs['hw-file'])
  if kwargs['template-file']: includes.append(kwargs['template-file'])

  # and we save it in the output database you chose
  import pm.project
  save_db = pm.project.Project(extra[0], includes)
  save_db.addObjects([segment])

  return 0

if __name__ == '__main__':
  import sys
  sys.exit(main())

