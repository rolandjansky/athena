#!/usr/bin/env python

import os
import sys
import optparse

import pm.project
import pm.common

from HLTUtils import hltOksUtils
from pm.dal import dal

prefix = 'HLT'
modifier = ''

parser = optparse.OptionParser(description="Create HLT S/W respository")

parser.add_option("-p", "--prefix", action="store",
                  help="Prefix used in make_hlt_env (default: 'HLT')")

parser.add_option("-m", "--modifier", action="store",
                  help="Modifier for objects in S/W repository (default: '')")

(opt, args) = parser.parse_args()

if opt.prefix!=None:
    prefix = opt.prefix
    
if opt.modifier!=None and len(opt.modifier)>0:
    modifier = '_' + opt.modifier

rep = pm.project.Project('daq/sw/repository.data.xml')
onlineRep  = rep.getObject('SW_Repository', 'Online')
hlt_env = pm.project.Project('daq/segments/%s-Environment.data.xml' % prefix)

apps = [
    dal.Script('l2pu_main_wrapper%s' % modifier,
               BinaryName='l2pu_main_wrapper',
               Description='L2PU wrapper',
               ProcessEnvironment=[hlt_env.getObject('VariableSet', '%s-Environment' % prefix)],           
               Authors=['Andre.dos.Anjos@cern.ch', 'Werner.Wiedenmann@cern.ch']
               ),
    
    dal.Script('pt_main_wrapper%s' % modifier,
               BinaryName='pt_main_wrapper',
               Description='PT wrapper',
               ProcessEnvironment=[hlt_env.getObject('VariableSet', '%s-Environment' % prefix)],
               Authors=['Serge.Shuskov@cern.ch', 'Andrea.Negri@cern.ch']
               ),
    
    dal.Script('athena_wrapper%s' % modifier,
               BinaryName='athena_wrapper',
               Description='athena.py wrapper',
               ProcessEnvironment=[hlt_env.getObject('VariableSet', '%s-Environment' % prefix)],
               Authors=['Reiner.Hauser@cern.ch']
               )    
    ]

vars = [
    dal.SW_PackageVariable('XMLPATH%s' % modifier,
                           Name='XMLPATH',
                           Suffix="XML"),
    dal.SW_PackageVariable('DATAPATH%s' % modifier,
                           Name='DATAPATH',
                           Suffix="share"),
    dal.SW_PackageVariable('JOBOPTSEARCHPATH%s' % modifier,
                           Name='JOBOPTSEARCHPATH',
                           Suffix="jobOptions"),
    dal.SW_PackageVariable('PYTHONPATH%s' % modifier,
                           Name='PYTHONPATH',
                           Suffix="python")
    ]


try:
    rel = hlt_env.getObject('Variable','%s-AtlasArea' % prefix).Value.split('/')[-1]
except:
    print ('Could not determine release number from AtlasArea')
  
hltRep = dal.SW_Repository('%s-Repository' % prefix,
                           Name='HLT-Repository',
                           InstallationPath="${ATLAS_BASE}/AtlasHLT/%s/InstallArea" % rel,
                           SW_Objects=apps,
                           Tags=hltOksUtils.defaultTags(),
                           ISInfoDescriptionFiles=['share/data/daq/schema/is_trigconf_hlt.schema.xml']
                           )

for app in apps:
    app.BelongsTo = hltRep

proj = pm.project.Project('%s_SW_Repository.data.xml' % prefix,
                          ['daq/schema/core.schema.xml',
                           'daq/sw/repository.data.xml',
                           'daq/segments/%s-Environment.data.xml' % prefix
                           ])
    
proj.addObjects([hltRep])
proj.addObjects(vars)
    
