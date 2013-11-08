#!/usr/bin/env python

import getopt, sys, os, time
import user
import shelve


inFileName = 'trigger_db.pickle'

print ">>> opening py-shelve [%s]..." % inFileName
hlt_config_db = shelve.open(inFileName)['config']
print ">>> found [%i] configurables" % len(hlt_config_db)

def is_nested(c):
  return c.count('.') > 0
#def cnv_prop():

import GaudiKernel.GaudiHandles as GaudiHandles
from AthenaCommon import CfgMgr
from AthenaCommon.Configurable import (ConfigurableAlgorithm,
                                       ConfigurableService,
                                       ConfigurableAlgTool,
                                       ConfigurableAuditor)

# load configurable db and create the ToolSvc
toolSvc = CfgMgr.ToolSvc()

from AthenaCommon.ConfigurableDb import getConfigurable,cfgDb
from AthenaCommon.AppMgr import ToolSvc,ServiceMgr,theApp
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

algs     = []
services = []
tools    = []
auditors = []
nprivate_algs = 0
# EventSelector
#include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
# Geometry
#include( "DetDescrDictionary/DetDescrDictionaryDict_joboptions.py" )
#include ("AtlasGeoModel/SetGeometryVersion.py")

# Get the Sorted list of DB Components
names = hlt_config_db.keys()
# Exclude the ApplicationMgr
del names[names.index('ApplicationMgr')]
# Sort the Components (alg1 > alg1.some) in order to initialize the Parent algorithms first
names.sort()

for name in names:
    c   = hlt_config_db[name]
    cfg_name = name
    cfg_type = c['type']
    if name.count('.') > 0:
        cfg_type = cfg_type.split('.')[-1]
        if 'ToolSvc.' in name :
          cfg_name = name.replace('ToolSvc.','')
        #cfg_name = cfg_name.split('.')[-1]
    #print 'Try to get Configurable:', cfg_type, ':\t %s/%s' % (c['type'],name)
    cfg_class = getConfigurable(cfg_type, assumeCxxClass=True)
    if not cfg_class :
      print 'WARNING: Configurable %s/%s (%s/%s) not found' % (cfg_type, cfg_name, c['type'],name)
      continue

    ## create an instance with the correct name
    stmt = 'cfg = CfgMgr.%(cfg_class)s( "%(cfg_name)s", %(properties)s)'

    properties = []
    for n,v in c['properties'].items():
        try:
            dflt_value = cfg_class.__slots__[n]
        except KeyError,err:
            print "::ERROR:: configurable [%s] has no such property [%s] !!"%(
                cfg_class.__name__, n
                )
            continue
        p = '%s = %s'
        if isinstance(dflt_value,str):
            p = '%s = "%s"'
        elif isinstance(dflt_value,GaudiHandles.GaudiHandle):
            p = '%s = "%s"'
        properties += [ p % (n,v) ]

    stmt = stmt % {
        'cfg_class' : cfg_class.__name__,
        'cfg_name'  : cfg_name,
        'properties': ', '.join(properties)
        }
        
    
    try:
      exec(stmt)
      
      #print 'Configurable:', cfg_type, ':\t %s/%s' % (c['type'],name), 'Succesfully initialized'
      #print '\t', cfg
      
      if isinstance(cfg, ConfigurableAlgorithm): algs.append    ({'cfg':cfg, 'topalg':c['topalg']})
      elif isinstance(cfg, ConfigurableService): services.append(cfg)
      elif isinstance(cfg, ConfigurableAlgTool): tools.append   (cfg)
      elif isinstance(cfg, ConfigurableAuditor): auditors.append(cfg)
      else:
          print ":::ERROR::: not nested and not an algorithm:",
          print c,"(%s)"%type(cfg)
          pass
    except NameError:
        if cfg_name.count('.') > 0 :
          alg_name = cfg_name.split('.')[-1]
          top_parent_name = cfg_name.split('.')[0]
          parent_name = cfg_name[:cfg_name.rfind(alg_name)-1]
          # cfg_name = alg_name
          top_parent_cfg = None
          top_parent_cfg_str = ''
          
          # Search the Algorithms for the parent
          if not top_parent_cfg:
            count = 0
            for a in algs :
              if a['cfg'].getName() == top_parent_name :
                top_parent_cfg_str = 'algs[%d][\'cfg\']' % (count)
                top_parent_cfg = algs[count]['cfg']
              count += 1
          
          # Search the Services for the parent
          if not top_parent_cfg:
            count = 0
            for s in services :
              if s.getName() == top_parent_name :
                top_parent_cfg_str = 'services[%d]' % (count)
                top_parent_cfg = services[count]
              count += 1
          
          # Search the Tools for the parent
          if not top_parent_cfg:
            count = 0
            for t in tools :
              if t.getName() == top_parent_name :
                top_parent_cfg_str = 'tools[%d]' % (count)
                top_parent_cfg = tools[count]
              count += 1
              
          # Search the Auditors for the parent
          if not top_parent_cfg:
            count = 0
            for a in auditors :
              if a.getName() == top_parent_name :
                top_parent_cfg_str = 'auditors[%d]' % (count)
                top_parent_cfg = auditors[count]
              count += 1
          
          if top_parent_cfg:
            parent_cfg = top_parent_cfg
            if parent_name.count('.') > 1 :
              for parent in parent_name[len(top_parent_name):].split('.') :
                child_cfg = None
                for child in parent_cfg.getAllChildren() :
                  if child.getName() == parent :
                    child_cfg = child
                    break
                if child_cfg : 
                  parent_cfg = child_cfg
                else :
                  print 'ERROR: ', 'Configurable:', cfg_type, ':\t %s/%s' % (c['type'],name), 'Parent algorithm: %s was not found' % (parent_name)
                
            # Test the existence of the Child Configurable if exists set the properties only 
            cfg = None
            for child in parent_cfg.getAllChildren() :
              if child.getName() == alg_name:
                cfg = child
                break;
            
            #print top_parent_cfg_str, parent_name
            #stmt = top_parent_cfg_str + parent_name[len(top_parent_name):]
            #print stmt
            if not cfg :
              stmt = 'parent_cfg += CfgMgr.%(cfg_class)s( "%(cfg_name)s", %(properties)s)'
              stmt = stmt % {
              'cfg_class' : cfg_class.__name__,
              'cfg_name'  : alg_name,
              'properties': ', '.join(properties)
              }
  
              try :
                exec(stmt)
                nprivate_algs += 1
                #print 'Private algorithm %s/%s (%s/%s) successfully initialized with name %s' % (private_alg['cfg_class'], private_alg['name'], c['type'], c['name'], cfg_name)
                #print parent_cfg
              except NameError:
                print 'ERROR: Configurable:', cfg_type, ':\t %s/%s' % (c['type'],name), 'Can not be initialized'
              #except AttributeError:
            else:
              for k,v in c['properties'].items():
                try :
                  v = eval(v)
                except :
                  v = v
                stmt = 'cfg.%s = %s' % (k,v)
                if isinstance(v,str) :
                  stmt = 'cfg.%s = \'%s\'' % (k,v)
                exec (stmt)
              nprivate_algs += 1
          else :
            print 'ERROR: Configurable:', cfg_type, ':\t %s/%s' % (c['type'],name), 'Parent algorithm: %s was not found' % (parent_name)
        else:
          print 'ERROR: Configurable:', cfg_type, ':\t %s/%s' % (c['type'],name), 'is not a private or nested algorithm'
    pass
      
        
## testing
## Sort the algs list such that parent algorithms are before the child algorithms
# means: alg1 < alg1.child => alphabetical sorting of the alg names would do
def alg_compare(a1, a2) :
  if a1['topalg'] == a1['cfg'].getFullJobOptName() :
    return -1
  
  if a2['topalg'] == a2['cfg'].getFullJobOptName() :
    return 1
  
  return cmp(a1['cfg'].getName(), a2['cfg'].getName())

algs.sort(alg_compare)
unbound_algs = []
hlt_app = hlt_config_db['ApplicationMgr']
for a in algs:
  alg_full_name = a['cfg'].getFullJobOptName()
  if alg_full_name in hlt_app['properties']['TopAlg']:
    print 'Top Algorithm Found: ', alg_full_name
    topSequence += a['cfg']
  elif not a['topalg'] : # In case there is no TopAlg 
    unbound_algs.append(a['cfg'])
  else:
    cfg = a['cfg']
    stmt = 'topSequence.%s += cfg' % (a['topalg'].split('/')[-1])
    exec(stmt)

print "-"*40,'Unbound Algorithms: %d' %(len(unbound_algs)),"-"*40
for a in unbound_algs :
  print "\t%s" % (a.getFullJobOptName())

for s in services: ServiceMgr += s
for t in tools:    ToolSvc    += t
for a in auditors: ServiceMgr.AuditorSvc += a

# then, take care of the application manager
print "-"*80

for n,v in hlt_app['properties'].items():
    stmt = 'theApp.%s = %s'
    dflt_value = theApp.__slots__[n]
    if v == dflt_value:
        continue
    if isinstance(dflt_value,str): stmt = 'theApp.%s = "%s"'
    stmt = stmt % (n,v)
    if n=='CreateSvc' :
      v = eval(v)
      for vv in v:
        theApp.CreateSvc += [vv]
        print ":::","theApp.CreateSvc += [\'%s\']" % (vv)
    else :
      try:
          exec(stmt)
          print ":::",stmt
      except Exception,err:
          print "::ERROR:: theApp setup:",err
          print "          ",n,v
          print "          ",stmt

        
print ">>> configurables in db:",len(hlt_config_db)
print ">>> algs:    ",len(algs)
print ">>> services:",len(services)
print ">>> tools:   ",len(tools)
print ">>> auditors:",len(auditors)
print ">>> private algs:", nprivate_algs
print "::: failed to restore [%i] configurables" %(len(hlt_config_db)
                                                   - len(algs)
                                                   - len(services)
                                                   - len(tools)
                                                   - len(auditors)
                                                   - nprivate_algs)


theApp.setup()