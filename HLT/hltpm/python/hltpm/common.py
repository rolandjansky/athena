#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# vim: set fileencoding=utf-8 :
# Created by Andre Anjos <andre.dos.anjos@cern.ch>
# Thu 04 Oct 2007 04:22:17 PM CEST 

"""Helper functions to change segments and partitions.
"""

import logging

# The centralized TDAQ software repository
import pm.project
hltRepository = pm.project.Project('daq/sw/HLT_SW_Repository.data.xml')
hltEnvironment = pm.project.Project('daq/segments/HLT-Environment.data.xml')

# Access to hltpm's centrally generated python DAL
from hltpm import dal, HLTdal

def user_provided_config(l2, ef):
  """Evaluates and warns about user provided TriggerConfiguration objects
  """
  l2_oks = False
  if l2 and isinstance(l2, str) and l2.find('.data.xml') != -1:
    l2_oks = True
    
  ef_oks = False
  if ef and isinstance(ef, str) and ef.find('.data.xml') != -1:
    ef_oks = True
    if l2_oks and l2 != ef:
      raise RuntimeError, 'L2 and EF configuration options specificy different databases (L2: %s, while EF: %s). Please pick one of them!' % (l2, ef)

  if l2_oks and not ef_oks and ef:
    logging.warning('Ignoring specific EF HLT setting in favor of global TriggerConfiguration provided by L2 (%s)' % l2)

  if ef_oks and not l2_oks and l2:
    logging.warning('Ignoring specific L2 HLT setting in favor of global TriggerConfiguration provided by EF (%s)' % ef)

  if l2_oks: return l2
  elif ef_oks: return ef
  return None

def load_trigger_config(db):
  c = pm.project.Project(db)
  obj = c.getObject('TriggerConfiguration')
  if not obj:
    raise RuntimeError, 'The provided database (%s) does not contain TriggerConfiguration objects.' % db
  if len(obj) != 1:
    logging.warning('The provided database (%s) contains more than one TriggerConfiguration object. Using the first (%s)' % (db, obj[0].id))
  return obj[0]

def is_l2ef_hltsegment(s):
  from pm.dal import DFdal
  l = []
  if s.TemplateApplications: l += s.TemplateApplications
  if s.Resources: l += s.Resources
  if s.Applications: l += s.Applications
  for a in l:
    if isinstance(a, DFdal.L2PUApplicationBase) or \
        isinstance(a, DFdal.PTApplicationBase):
          return True
  return False

def add_db_proxy(f, templates):
  """Places DBProxies in HLT subfarms (per-node and per-subfarm).

  This method will populate a given HLT subfarm with DB proxies. One
  (template) will be created for every node and one will be placed in every
  HLT subfarm (also templated). 

  Keyword arguments:

  f -- The farm you want to have explored and configured

  templates -- This must be a pm.project.Project where the default templated
  applications I should reside. It is normally loaded from a common (release)
  include file.

  Returns the modified segment.
  """
  hlt_dbgen = templates.getObject('TemplateApplication', 'HLT_DB_Gen')
  top_proxy_tpl = templates.getObject('InfrastructureTemplateApplication',
      'TopCoralProxy')
  rack_proxy_tpl = templates.getObject('InfrastructureTemplateApplication',
      'RackCoralProxy')
  node_proxy_tpl = templates.getObject('TemplateApplication', 'NodeCoralProxy')

  if isinstance(f, dal.Partition):
    # we are interested in all segments that contain HLT segments inside
    tmp = [k for k in f.Segments if k.get('HLTSegment', lookBaseClasses=True)]
    for k in tmp: add_db_proxy(k, templates)
    return f

  if f.Infrastructure: f.Infrastructure.append(top_proxy_tpl)
  else: f.Infrastructure = [top_proxy_tpl]

  # now do it for every HLTSegment available
  all_segments = [k for k in f.get('HLTSegment', lookBaseClasses=True) \
    if is_l2ef_hltsegment(k)]

  for s in all_segments:

    if s.Infrastructure: 
      s.Infrastructure.append(rack_proxy_tpl)
    else: 
      s.Infrastructure = [rack_proxy_tpl]
   
    if len(s.TemplateApplications) == 0: 
      logging.warn('HLT Segment %s does not contain Templated HLT' % s.id + \
          ' Applications. Not placing a proxy per node in this case.')
      continue
    
    # we always append the HLT DB generation program, independently of the
    # proxies being set in this node or not, as HLT applications need it.
    s.TemplateApplications.append(hlt_dbgen)
    s.TemplateApplications.append(node_proxy_tpl)

  return f

def prune_db_proxy(f, templates):
  """Prunes DBProxies in HLT subfarms (per-node and per-subfarm).

  This method will prune a given HLT subfarm w.r.t their Coral proxies. Rules:

  a) Top-level and rack-level cannot live on the same node
  b) Top-level (or rack-level) and node-level can live on the same node

  Keyword arguments:

  f -- The farm you want to have explored and configured

  Returns the modified segment.
  """
  top_proxy_tpl = templates.getObject('InfrastructureTemplateApplication',
      'TopCoralProxy')
  rack_proxy_tpl = templates.getObject('InfrastructureTemplateApplication',
      'RackCoralProxy')
  node_proxy_tpl = templates.getObject('TemplateApplication', 'NodeCoralProxy')

  # Find all segments that are pertinent
  top_segments = [k for k in f.get('Segment', lookBaseClasses=True) \
      if top_proxy_tpl in k.Infrastructure]

  # special case for top_segments
  if isinstance(f, dal.Segment) and top_proxy_tpl in f.Infrastructure:
    top_segments.append(f)

  rack_segments = [k for k in f.get('Segment', lookBaseClasses=True) \
      if rack_proxy_tpl in k.Infrastructure]

  node_segments = [k for k in f.get('HLTSegment', lookBaseClasses=True) \
      if node_proxy_tpl in k.TemplateApplications]

  # First we prune all top level proxies
  proxies = []
  for k in top_segments:
    if k.DefaultHost in proxies: #already exists, remove!
      k.Infrastructure = [i for i in k.Infrastructure if i != top_proxy_tpl]
    else:
      proxies.append(k.DefaultHost)

  # Next, we prune all rack level proxies
  for k in rack_segments:
    if k.DefaultHost in proxies: #already exists, remove!
      k.Infrastructure = [i for i in k.Infrastructure if i != rack_proxy_tpl]
    else:
      proxies.append(k.DefaultHost)

  # Finally, we prune repeated NodeProxies - don't care about racks and top

  proxies = []
  hd = {}
  for k in node_segments:
    for h in k.TemplateHosts:
      hd[h.id] = 1
      if h not in proxies: proxies.append(h)
      else: #remove, no other remedy!
        if len(proxies) != 1: #this is not a single node, warn the user!
          logging.warn('Removing NodeCoralProxy template from %s! This is not a single-node system, so I hope you know what you are doing.' % k.fullName())
        k.TemplateApplications = [i for i in k.TemplateApplications \
            if i != node_proxy_tpl]

  if f.className() == 'Partition':
    dh = f.DefaultHost.id
    hl = hd.keys()
    if len(hl) == 1 and hl[0] == dh: # then we assume it's a localhost partition
      for k in node_segments:
        for h in k.TemplateHosts:
          if h in proxies:
            k.TemplateApplications = [i for i in k.TemplateApplications if i != node_proxy_tpl]

  # There we go, it is all done.
  return f

def switch_application(obj, extra_vars=[], long_timeout=600):
  """Configures HLT processors to run HLT-enabled applications.
  
  This method will look for all applications with base type L2PUApplicationBase
  or PTApplicationBase at the given DAL object and will switch their binaries
  so they can run HLT-enabled applications.

  Keyword arguments:

  obj -- This is the Python DAL object you want modified. It will be scanned
  recursively for possible HLT nodes. It is not an error to input DAL objects
  which have no potential HLT processors.

  extra_vars -- This is a list of extra dal.Variable's or VariableSet's you
  want attached to the HLT applications.

  long_timeout -- This is the timeout value, in seconds (the default is 600 or
  10 minutes) you want the controller for this applications to consider. It is
  normally wise that this value is greater than a minute. This should cope with
  longish transitions like "CONFIGURE" or "STOP".

  Returns 'obj', the dal object you gave as first parameter.
  """

  # attach HLT-Parameters to the relevant segments
  hltpar = hltEnvironment.getObject('VariableSet', 'HLT-Parameters')
  for seg in obj.get('HLTSegment', lookBaseClasses=True):
    if seg.Parameters: seg.Parameters.append(hltpar)
    else: seg.Parameters = [hltpar]
  
  for l2pu in obj.get('L2PUApplicationBase', lookBaseClasses=True):
    l2pu.Program = hltRepository.getObject("Script", "l2pu_main_wrapper")
    l2pu.ActionTimeout = long_timeout
    l2pu.IfDies = 'Ignore'
    l2pu.IfFailed = 'Ignore'
    if l2pu.ProcessEnvironment: l2pu.ProcessEnvironment.extend(extra_vars)
    else: l2pu.ProcessEnvironment = extra_vars

  for pt in obj.get('PTApplicationBase', lookBaseClasses=True):
    pt.Program = hltRepository.getObject("Script", "pt_main_wrapper")
    pt.ActionTimeout = long_timeout
    pt.IfDies = 'Ignore'
    pt.IfFailed = 'Ignore'
    if pt.ProcessEnvironment: pt.ProcessEnvironment.extend(extra_vars)
    else: pt.ProcessEnvironment = extra_vars

  return obj

def update_dal(obj, d):
  """Updates attributes/relations from the object based on dictionary entries.
  
  This method will try to match the keys of the input object 'd' and fill the
  attributes and relationships of the dal object 'obj'.

  Keyword parameters:

  obj -- The DAL object to be filled with the contents of the dictionary 'd'
  
  d -- The (python) dictionary that contains the properties you want to copy to
  the DAL object.

  This method will not complain a certain key exists in 'd' and does not exist
  in 'obj'. It is your problem to make sure everything is at it is supposed to
  be in the schema.
  """
  for k, v in d.items(): 
    if hasattr(obj, k): setattr(obj, k, v)

def create_joboptions_implementation(obj, level=None):
  """Creates a default representation for running with JobOptions.

  This method returns the default settings for running and HLT node against
  jobOptions, pretty much like in Offline.

  Keyword arguments:

  obj -- This is a dictionary, that should contain, at least, the standard
  fields you have in the HLT OKS schema for the object
  HLTImplementationJobOptions. The 'libraries' field is optional, but if you
  don't specify it, you should specify 'level' to either 'L2' and 'EF', what
  will make me choose the right libraries for you.

  level -- The HLT level in which this object will be used. This will influence
  the choice of libraries (if you don't set the 'libraries' parameter). If you
  don't specify the level, it is assumed you are specifying the 'libraries'
  parameter. We will check for this. Valid values are "L2" or "EF".

  Returns an object of type HLdal.HLTImplementationJobOptions
  """

  # first we fill in our defaults and make standard choices
  libs = [ "TrigOks%sPSC" % level, "Trig%sServices" % level, "TrigServices" ]
  name = obj.get('name', 'HLT-%s-JobOptions-1' % level)
  retval = HLTdal.HLTImplementationJobOptions(name, libraries=libs, 
      logLevel=["ERROR"])

  # then we update with the user settings before returning
  obligatory = 'isOnline=True'
  if hasattr(obj, 'has_key'):
    if obj.has_key('preCommand'):
      if obligatory not in obj['preCommand']:
        obj['preCommand'].insert(0, obligatory)
    else: 
      obj['preCommand'] = [obligatory]
  
  update_dal(retval, obj)

  return retval

def create_db_implementation(obj, level=None):
  """Creates a default representation for running HLT from a database.

  This method returns the default settings for running and HLT node against
  a database.

  Keyword arguments:

  obj -- This is a dictionary, that should contain, at least, the standard
  fields you have in the HLT OKS schema for the object HLTImplementationDB. The
  'libraries' field is optional, but if you don't specify it, you should
  specify 'level' to either 'L2' and 'EF', what will make me choose the right
  libraries for you.

  level -- The HLT level in which this object will be used. This will influence
  the choice of libraries (if you don't set the 'libraries' parameter). If you
  don't specify the level, it is assumed you are specifying the 'libraries'
  parameter. We will check for this. Valid values are "L2" or "EF".

  Returns an object of type HLTdal.HLTImplementationDB
  """

  # first we fill in our defaults and make standard choices
  libs = [ "TrigOks%sPSC" % level, "Trig%sServices" % level, "TrigServices",
     "TrigConfigSvc"]
  name = obj.get('name', 'HLT-%s-DB-1' % level)
  retval = HLTdal.HLTImplementationDB(name, libraries=libs)
  retval.instance = level

  # then we update with the user settings before returning
  update_dal(retval, obj)

  return retval

def gen_implementation(obj, level=None):
  """Generates an HLTImplementation derived from the object passed.

  Generates an HLTImplementation object based on your input. A few variants are
  supported for the input object. Different choices may lead to different
  variations of the returned object.

  Keyword parameters:

  obj -- The input object. This object may come in two different types:

    1. A dictionary, that defines the fields of the HLTImplementationJobOptions
    object.
    2. A dictionary, that defines the fields of the HLTImplementationsDB
    object.
    3. A string that specifies the top-level jobOption of your setup.
    4. An object of subtype HLTImplementation (abstract) that, in this case, is
    returned without further processing

  In cases 1 and 2 the keys of the dictionary *must* match the keys found in
  the correspondant HLTImplementation you are trying to generate.  The
  algorithm within this method will look for obligatory fields in the object
  ans will generate the object accordingly. The 'libraries' field can be
  omitted, since this is calculated from the generated type automatically. The
  optional field 'name' will define the object name. If it is not set, a
  standard name is created for you.

  The current choice of implementation is done like this: If there is a
  'jobOptionsPath' key in the dictionary, we choose the
  HLTImplementationJobOptions, otherwise, the HLTImplementationDB. 

  level -- The HLT level in which this object will be used. This will influence
  the choice of libraries (if you don't set the 'libraries' parameter). If you
  don't specify the level, it is assumed you are specifying the 'libraries'
  parameter. We will check for this. Valid values are "L2" or "EF".
  """
  if isinstance(obj, str):
    obj = {'jobOptionsPath': obj}
  
  if not obj.has_key('libraries') and not level:
    raise ValueError, \
        'You should specify "level", if not specifying "libraries"'
    
  if level: 
    level = level.upper()
    if level not in ['L2', 'EF']:
      raise ValueError, 'Parameter "level" can only be set to "L2" or "EF"'
  
  if isinstance(obj, dict):
    if obj.has_key('jobOptionsPath'): 
      return create_joboptions_implementation(obj, level)
    else:
      return create_db_implementation(obj, level)

  # in the case everything else fails, we just return the input
  return obj

def make_hlt_segment(d, extra_area=[], proxy=False, templates=None):
  """Changes the segment so it executes HLT applications in HLT nodes.

  This method will modify your segment object in the input dictionary 'd' to
  execute HLT applications in L2 or EF.

  Keyword parameters:

  d -- This is a dictionary containing a segment and a list of includes, as
  returned by pm.segment.*.gendb() methods. The segment object and the list
  of includes will be modified so the generated database contains HLT-enabled
  applications.

  extra_area -- These are extra HLT installation paths that should be taken
  with priority to the release area. They are normally called "patch" areas.

  proxy -- if set to True, we should add the Coral Proxy infrastructure to the 
  partition

  templates -- This must be a pm.project.Project where the default templated
  applications I should reside. It is normally loaded from a common (release)
  include file.

  Returns the object 'd', modified.
  """
  extra_vars = []
  if extra_area:
    extra_vars.append(dal.Variable('HLT-PATCH-AREA-1', 
      Name="HLT_EXTRA_SW_PATH", Value=':'.join(extra_area),
      Description='HLT patch area generated by HLT/PM extension'))
    
  for k in d['segment']: 
    if proxy: 
      add_db_proxy(k, templates)
      prune_db_proxy(k, templates)
    switch_application(k, extra_vars=extra_vars)

  d['includes'] += [ 'daq/sw/HLT_SW_Repository.data.xml',
                     'daq/segments/HLT-Environment.data.xml' ]
  
  return d

def make_hlt_partition(d, l2=None, ef=None, extra_area=[], proxy=False,
    templates=None):
  """Changes the partition so it executes HLT applications in HLT nodes.

  This method will modify your partition object in the input dictionary 'd' to
  execute HLT applications in L2 and EF, depending on your inputs.

  Keyword parameters:

  d -- This is a dictionary containing a partition and a list of includes, as
  returned by pm.partition.*.gendb() methods. The partition object and the list
  of includes will be modified so the generated database contains HLT-enabled
  applications.

  l2 -- If this variable is set, then the L2 partition-level implementation
  will be changed so L2PU nodes run HLT code. The values accepted for this
  parameter are described in the function gen_implementation() in this module.

  ef -- If this variable is set, then the EF partition-level implementation
  will be changed so PT nodes run HLT code. The values accepted for this
  parameter are described in the function gen_implementation() in this module.

  extra_area -- These are extra HLT installation paths that should be taken
  with priority to the release area. They are normally called "patch" areas.

  proxy -- if set to True, we should add the Coral Proxy infrastructure to the 
  partition

  templates -- This must be a pm.project.Project where the default templated
  applications I should reside. It is normally loaded from a common (release)
  include file.

  Returns the object 'd', modified.
  """
  extra_vars = []
  if extra_area:
    extra_vars.append(dal.Variable('HLT-PATCH-AREA-1', 
      Name="HLT_EXTRA_SW_PATH", Value=':'.join(extra_area),
      Description='HLT patch area generated by HLT/PM extension'))
    
  if proxy: 
    add_db_proxy(d['partition'], templates)
    prune_db_proxy(d['partition'], templates)
    
  switch_application(d['partition'], extra_vars=extra_vars)
 
  user_config = user_provided_config(l2, ef)
  if user_config: 
    d['partition'].TriggerConfiguration = load_trigger_config(user_config)
  else: 
    if l2: d['partition'].TriggerConfiguration.l2 = gen_implementation(l2, 'L2')
    if ef: d['partition'].TriggerConfiguration.ef = gen_implementation(ef, 'EF')
  
  d['includes'] += [ 'daq/schema/hlt.schema.xml',
                     'daq/sw/HLT_SW_Repository.data.xml',
                     'daq/segments/HLT-Environment.data.xml' ]
  
  return d

