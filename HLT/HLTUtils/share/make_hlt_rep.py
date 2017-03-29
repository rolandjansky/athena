#!/usr/bin/env python

import optparse
import os

import pm.project
import pm.common

from HLTUtils import hltOksUtils
from pm.dal import dal

prefix = 'HLT'
modifier = ''

parser = optparse.OptionParser(description="Create HLT S/W respository")

parser.add_option("-p", "--prefix", action="store",
                  help="Prefix used for all objects (default: 'HLT')")

parser.add_option("-m", "--modifier", action="store",
                  help="Modifier for objects in S/W repository (default: '')")

(opt, args) = parser.parse_args()

if opt.prefix is not None:
    prefix = opt.prefix
    
if opt.modifier is not None and len(opt.modifier)>0:
    modifier = '_' + opt.modifier

rep = pm.project.Project('daq/sw/repository.data.xml')
onlineRep  = rep.getObject('SW_Repository', 'Online')
common_env = pm.project.Project('daq/segments/common-environment.data.xml')

atlas_project = os.environ.get('AtlasProject')

if 'AtlasBuildStamp' in os.environ:
    # nightly
    atlas_version='%s,r%s' % (os.environ.get('AtlasBuildBranch'),
                              os.environ.get('AtlasBuildStamp'))
else:    
    # numbered releases
    atlas_version = os.environ.get('AtlasVersion')

swvars = [
    dal.SW_PackageVariable('XMLPATH%s' % modifier,
                           Name='XMLPATH',
                           Suffix="XML"),
    dal.SW_PackageVariable('DATAPATH%s' % modifier,
                           Name='DATAPATH',
                           Suffix="share"),
    dal.SW_PackageVariable('CALIBPATH%s' % modifier,
                           Name='CALIBPATH',
                           Suffix="share"),                           
    dal.SW_PackageVariable('JOBOPTSEARCHPATH%s' % modifier,
                           Name='JOBOPTSEARCHPATH',
                           Suffix="jobOptions"),
    dal.SW_PackageVariable('PYTHONPATH%s' % modifier,
                           Name='PYTHONPATH',
                           Suffix="python")
]

puvars = [
    dal.Variable('%s_ASETUP_ARGS' % prefix,
                 Name='ASETUP_ARGS',
                 Value=('%s,%s' % (atlas_project, '${%s_VERSION}' % prefix)),
                 Description='asetup arguments'),

    dal.Variable('%s_PRELOAD' % prefix,
                 Name='ASETUP_PRELOAD',
                 Value='libtcmalloc_minimal.so:libimf.so:libintlc.so.5',
                 Description='Preload libraries'),

    dal.Variable('%s_VERBOSE' % prefix,
                 Name='ASETUP_VERBOSE',
                 Value='0',
                 Description='Verbosity for asetup_wrapper'),

    dal.Variable('%s_EXTRA_SW_PATH' % prefix,
                 Name='ASETUP_EXTRA_SW_PATH',
                 Value='',
                 Description='Patch area (e.g. /my/path/InstallArea)'),
    
    common_env.getObject('Variable','AtlasSetup')
]

pars = dal.VariableSet('%s-Parameters' % prefix,
                       Description='Parameters to expand for executables',
                       Contains = [ dal.Variable('%s_VERSION' % prefix,
                                                 Name='%s_VERSION' % prefix,
                                                 Value=atlas_version,
                                                 Description='%s release version' % prefix) ])

apps = [
    dal.Binary('HLTMPPU_asetup',
               BinaryName='asetup_wrapper',
               Description='HLTMPPU_main with asetup environment',
               DefaultParameters='HLTMPPU_main',
               ProcessEnvironment=puvars)  
]


hltRep = dal.SW_Repository('%s-Repository' % prefix,
                           Name=('%s-Repository' % prefix),
                           InstallationPath="%s/InstallArea" % os.environ.get('AtlasArea'),
                           SW_Objects=apps,
                           Tags=hltOksUtils.defaultTags(),
                           ISInfoDescriptionFiles=['share/data/daq/schema/is_trigconf_hlt.schema.xml'],
                           AddProcessEnvironment = swvars
                           )

for app in apps:
    app.BelongsTo = hltRep

proj = pm.project.Project('%s_SW_Repository.data.xml' % prefix,
                          ['daq/schema/core.schema.xml',
                           'daq/sw/repository.data.xml'])
    
proj.addObjects([hltRep])
proj.addObjects([pars])

