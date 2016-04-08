#!/usr/bin/env python
"""Create HLT patch repository."""

import os
import sys
import optparse

import pm.project
import pm.common
from pm.dal import dal

from HLTUtils import hltOksUtils

parser = optparse.OptionParser(description=__doc__,
                               usage='%prog [Options]')

parser.add_option('-n', '--name', action='store', type='string',
                  help='Name of the patch repository [default: $AtlasProject_$AtlasVersion]')

parser.add_option('-p', '--path', action='store', type='string',
                  help='Path to patch area [default: $AtlasArea]')
   
(opt, args) = parser.parse_args()

names = [ 'XMLPATH', 'DATAPATH', 'CALIBPATH', 'JOBOPTSEARCHPATH', 'PYTHONPATH' ]
rep = pm.project.Project('daq/sw/HLT_SW_Repository.data.xml')
vars = [ rep.getObject('SW_PackageVariable', name) for name in names ]

AtlasArea = os.getenv('AtlasArea') if opt.path==None else opt.path
AtlasVersion = os.getenv('AtlasVersion')
AtlasProject = os.getenv('AtlasProject')

if opt.name==None:
    pkg_id   = '%s_%s' % (AtlasProject, AtlasVersion)
    pkg_name = '%s %s' % (AtlasProject, AtlasVersion)
else:
    pkg_id = pkg_name = opt.name

env_vars = [ dal.Variable(pkg_id+'_Project',
                          Name="AtlasProject",
                          Value=AtlasProject),
             dal.Variable(pkg_id+'_Version',
                          Name="AtlasVersion",
                          Value=AtlasVersion)
             ]

env = dal.VariableSet(pkg_id,
                      Contains=env_vars
                      )

pkg = dal.SW_Repository(pkg_id,
                        Name=pkg_name,
                        InstallationPath=os.path.join(AtlasArea,'InstallArea'),
                        InstallationPathVariableName="AtlasP1HLT_InstPath",
                        ProcessEnvironment=[env],
                        AddProcessEnvironment=vars,
                        Tags=hltOksUtils.defaultTags()
                        )
    
proj = pm.project.Project('%s.data.xml' % pkg_id,
                          ['daq/schema/core.schema.xml',
                           'daq/sw/HLT_SW_Repository.data.xml'
                           ])
   
proj.addObjects([pkg])


