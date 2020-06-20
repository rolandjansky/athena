# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# $Id: common.py,v 1.39 2009-05-20 20:48:52 ssnyder Exp $
#
# File: CaloClusterCorrection/python/common.py
# Created: Nov 2006, sss
# Purpose: Common utility code for configuring cluster corrections.
#

from __future__ import print_function

from AthenaCommon.Logging import logging
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.GlobalFlags   import globalflags
from AthenaCommon.Configurable  import Configurable
from IOVDbSvc.CondDB import conddb
from fnmatch import fnmatchcase
import sys
import string

from CaloClusterCorrection.poolfiles import poolfiles
from CaloClusterCorrection.CaloClusterCorrectionConf import \
     CaloRunClusterCorrections, CaloDummyCorrection

from CaloClusterCorrection.constants import *

# Used for reporting configuration errors.
class CaloCorrectionConfigError (Exception):
    pass

# Keep track of pool files we're already seen.
_poolfiles_seen = {}

# Keep track of pool folders we've used.
_folders_used = {}

# Keep track of all tool names, to prevent duplicates.
# The value is a (folder, tag, sgkey) tuple.
_alltools = {}



class MetatoolPropDummy:
    def __init__ (self, obj):
        self.obj = obj
    def __getitem__ (self, i):
        return self.obj.__dict__.get (i, '')
        return ''
    def has_key (self, k):
        if k == 'isDummy':
            return False
        return True
    def __contains__ (self, k):
        return self.has_key (k)
    
class MetatoolHelper:
    def __init__ (self, confclass, name):
        self.__dict__["clsname"] = confclass.__name__
        self.__dict__["name"] = name
        self.__dict__["prefix"] = ""
        self.__dict__["DBHandleKey"] = None
        self.__dict__["props"] = {}
        self.__dict__["_propdum"] = MetatoolPropDummy (self)
        return


    def getName (self):
        return self.name

    def __setattr__ (self, k, v):
        if k in self.__dict__:
            self.__dict__[k] = v
        else:
            if isinstance (v, Configurable):
                v = v.getFullName()
            self.props[k] = str(v)
        return


    def properties (self):
        return self._propdum


    def corrspec (self):
        if self.DBHandleKey == None:
            location = '+' + self.clsname
        elif self.DBHandleKey.find ('/') >= 0:
            location = '@' + self.DBHandleKey
        else:
            location = self.DBHandleKey
        spec = [self.name,
                location,
                self.prefix]
        for (k, v) in self.props.items():
            spec += [k, v]
        spec.append ("")
        return spec


###########################################################################
# Code to handle creating the list of correction tools.
#
# The common code is in the class CaloClusterCorrSetup.
# You are expected to derive from it for your cluster type, and then
# create an instance.
# The following class variables must be defined in the derived class:
#  name:                  A short descriptive name of the type of cluster
#                         (to be written to the log file)
#
#  version_override_flag: A job property.
#                         If set, it overrides the selected correction
#                         version.
#
#  correction_generation_flag:
#                         A job property.
#                         If set, this gives the correction generation
#                         string that's embedded in tags written to the
#                         database.  This string should be changed for
#                         each write to the database.
#
#  correction_generation_default:
#                         Default value for the generation string,
#                         if the job property isn't set.
#
#  versions:              The table of all known correction versions.
#                         Should be a dictionary; the keys are correction
#                         version names, and the values are correction lists.
#                         A correction list is a list
#                         of individual corrections:
#
#                           [[CORR], [CORR], ...]
#
#                         where each individual correction here is also
#                         a list:
#
#                           [CORRFUNC, [VERSION,] [ORDER,] ARGS...]
#
#                         CORRFUNC is a function to call to create
#                         the correction; it should have the signature:
#
#                           def CORRFUNC (cells_name, suffix, version,
#                                         key, source, **kw):
#
#                         VERSION is a string giving the correction version.
#                         It may be omitted, and defaults to an empty string.
#                         It may also be `@', in which case the global
#                         version string is used instead.
#
#                         ORDER is an integer.  When the metatool is used,
#                         tools will be executed in increasing order
#                         of the ORDER parameter.  Note that a given
#                         tool/version must have the same ORDER each
#                         time it's used.
#
#                         The remainder of the list should be a set
#                         of (NAME,VALUE) tuples giving correction parameters
#                         to override.  This may be (and usually should be)
#                         empty.
#
#                         A key of `@' is used if the global correction
#                         version is tag-based (starts with `@').
#
# geom_versions:          The mapping between geometry versions and
#                         corrections versions.  A list of lists,
#                         each of which has the form
#
#                            [ GEOM-PATTERN, CORRECTION-VERSION ]
#
#                         Here, GEOM-PATTERN is a string that's matched
#                         against the geometry version string.  It may
#                         contain glob-style wildcards (as implemented
#                         by fnmatch).  CORRECTION-VERSION is a string
#                         giving the correction version to use.
#                         The patterns are tried in order; first
#                         to match wins.
#
# newest_version:         The name of the newest version.  This will be
#                         used if there's no match for the geometry
#                         string.
#


# Helper: Split up a version specifier SPEC.
# Returns (FUNC, VERSION, ORDER, EXTRA_ARGS)
def split_version_spec (cspec):
    func = cspec[0]
    version = ''
    order = 0
    ii = 1
    if ii < len(cspec) and type(cspec[ii]) == type(''):
        version = cspec[ii]
        ii += 1
    if ii < len(cspec) and type(cspec[ii]) == type(1):
        order = cspec[ii]
        ii += 1
    extra_args = cspec[ii:]
    return (func, version, order, extra_args)


class CaloClusterCorrSetup:
    # Must be overridden in derived classes.  See above for their meaning.
    # name = XXX
    # version_override_flag = XXX
    # correction_generation_flag = XXX
    # correction_generation_default = XXX
    # versions = XXX
    # geom_versions = XXX
    # newest_version = XXX

    #
    # Create and return a list of correction tools.
    # CORRCLASS is the correction class string, as defined above.
    # KEY is a string that specifies the correction type.
    # SUFFIX is a string to add to the end of each tool name.
    # VERSION specifies which version of corrections to use.
    # VERSION may also be of the form `@TAG' to force a specific COOL tag TAG,
    # or it may be `@GLOBAL' to use to globally-configured COOL tag.
    # In such cases, if there's no direct match of the version string
    # in the version table, we look for one with a version of `@'.
    # If no match for that is found, the last entry in the table
    # will be used.
    # If VERSION starts with `@', USE_METATOOL must be enabled.
    # CORRLIST can be used to explicitly specify which corrections to run.
    # CELLS_NAME is the SG key to use to find the calorimeter cells,
    # for those corrections that require it.
    # SOURCE specifies the source(s) from which tools are configured.
    # None means to use the default.
    # If USE_METATOOL is true, then instead of making the list of tools here,
    # return a single CaloRunClusterCorrections tool.
    #
    def make_corrections (self,
                          corrclass,
                          key = None,
                          suffix = '',
                          version = None,
                          corrlist = None,
                          cells_name = None,
                          source = None,
                          use_metatool = False,
                          **kw):

        # Make a logger.
        log = logging.getLogger ('CaloClusterCorrection')

        tryhier = False

        # First, find the appropriate overall version string to use,
        # if it wasn't specified explicitly.
        if version == None:
            # First see if the flag was set to override the version.
            # at the top-level.
            if self.version_override_flag.statusOn:
                version = self.version_override_flag()

        if version == None:
            # No explicitly specified version.
            # Try to guess it from the data source + geometry string.
            # Otherwise, use the latest version.
            geom = jobproperties.Global.DetDescrVersion()
            datasource = globalflags.DataSource()
            (version, tryhier) = self.geom_match (datasource, geom)

        #if version[0] == '@' and not use_metatool:
        #    raise CaloCorrectionConfigError ('COOL tag requested but not metatool.')
            
        # Get the correction generation.
        generation = self.get_generation()

        # Use the default source if one wasn't specified.
        if source == None:
            from CaloClusterCorrection.CaloClusterCorrectionFlags \
                 import caloClusterCorrectionFlags
            source = caloClusterCorrectionFlags.DefaultSource()

        if type(source) != type([]):
            source = [source]
        if CALOCORR_COOL not in source:
            tryhier = False

        if tryhier and version[0] != '@':
            version = "@%s-%s%s" % (corrclass, generation, version)

        # Only use COOL if we're actually trying to resolve a COOL tag.
        # Otherwise we can run into problems: it looks like `ununsed'
        # COOL tags are removed in DB replicas.
        if version[0] != '@' and len(source) > 0:
            if CALOCORR_COOL in source:
                source.remove (CALOCORR_COOL)

        (vcorrlist, version) = self.lookup_version (version)

        # Default to the standard list if no explicit correction list.
        if corrlist == None:
            corrlist = vcorrlist
            preserve_order = False
        else:
            # If list was given explicitly, don't reorder it.
            preserve_order = True

        log.info ("%s corrections for %s (%s) using version %s" %
                  (self.name, key, suffix, version))

        # Now, walk through the list of corrections.
        out = []
        for cspec in corrlist:
            (func, this_version, this_order, extra_args) = \
                   split_version_spec (cspec)

            # If the version or extra args isn't specified here,
            # look in the internal corrlist to see if we find it there.
            if this_version == '' or extra_args == []:
                for vcspec in vcorrlist:
                    if vcspec[0] == func:
                        (vfunc, vversion, vorder, vextra_args) = \
                                split_version_spec (vcspec)
                        if this_version == '':
                            this_version = vversion
                        if extra_args == []:
                            extra_args = vextra_args
                        if this_order == 0:
                            this_order = vorder
                        break

            if this_version == '@':
                this_version = version

            # Start building the other variables to use in the call.
            this_args = dict (extra_args)
            this_key = key
            this_suffix = suffix
            this_cells_name = cells_name
            this_source = source

            corrname = func.__name__

            # Look for any overriders in our keyword list.
            for (k, v) in kw.items():
                if k == corrname + '_suffix':
                    this_suffix = v
                elif k == corrname + '_key':
                    this_key = v
                elif k == corrname + '_cells_name':
                    this_cells_name = v
                elif k == corrname + '_source':
                    this_source = v
                elif k == corrname + '_version':
                    this_version = v
                elif k == corrname + '_order':
                    this_order = v
                elif k.startswith (corrname + '_'):
                    this_args[k[len(corrname)+1:]] = v

            # Make the tool.
            tool = func (this_cells_name,
                         this_suffix,
                         this_version,
                         this_key,
                         this_source,
                         generation = generation,
                         use_metatool = use_metatool,
                         order = this_order,
                         **this_args)
            out.append (tool)

        if use_metatool:
            out = [self.make_metatool (suffix + key, out, preserve_order)]

        return out


    # Look up a specific correction version; return the correction
    # list and the version.  Broken out to allow overriding, for
    # any special version-name handling needed.
    def lookup_version (self, version):
        vcorrlist = self.versions.get (version)
        if vcorrlist == None and version.startswith ('@'):
            vcorrlist = self.versions.get ('@')
        if vcorrlist == None:
            raise CaloCorrectionConfigError\
                  ("Can't find global correction version %s." % version)
        return (vcorrlist, version)


    # Match a data source + geometry string against geom_versions.
    def _geom_match1 (self, geom):
        for (pat, v) in self.geom_versions:
            if fnmatchcase (geom, pat):
                return v
        return None
    def geom_match (self, datasource, geom):
        if not geom: geom = ''
        vv = self._geom_match1 (datasource + '-' + geom)
        if not vv:
            vv = self._geom_match1 (geom)
        if not vv:
            vv = self.newest_version
        tryhier = True
        if vv.startswith ('NOHIER#'):
            tryhier = False
            vv = vv[7:]
        return (vv, tryhier)


    # Get the correction generation, with a `-' appended if appropriate.
    @classmethod
    def get_generation (cls):
        generation = cls.correction_generation_default
        if cls.correction_generation_flag.statusOn:
            generation = cls.correction_generation_flag()
        if generation != '':
            generation = generation + '-'
        return generation


    #####################################################################
    # Code for writing to pool.
    # Make a set of corrections for writing to pool.
    def make_ForPool (self, keys, corrclass):
        # Map from SG key to a list of tool instances.
        sgkeys = {}

        # Map from tool names to instances.
        tools = {}

        # Map from (func, version) to (order, globversion).
        # For consistency checking.
        ordermap = {}

        # Go through each version, and make corrections.
        for (v,l) in self.versions.items():
            if v.startswith ('@'): continue
            last_order = 0
            last_funcname = '(none)'
            for c in l:
                # Consistency check: tool lists should be sorted in order.
                (func, version, order, extra_args) = split_version_spec (c)
                if order <= last_order:
                    raise CaloCorrectionConfigError\
                          ("Tools for global version %s are not in order!"
                           " %d(%s) < %d(%s)" % (v, order, func.__name__,
                                                 last_order, last_funcname))
                last_order = order
                last_funcname = func.__name__

                # Consistency check: a given tool func/version should
                # always have the same order setting.
                okey = (func, version)
                if okey not in ordermap:
                    ordermap[okey] = (order, v)
                elif ordermap[okey][0] != order:
                    raise CaloCorrectionConfigError\
                          ("Tool %s has order %d in %s but order %d in %s!" %
                           (func.__name__, order, v,
                            ordermap[okey][0], ordermap[okey][1]))
                
                # Find the list of valid keys for this tool.
                valid_keys = func (None,
                                   version = version,
                                   key = '@VALID_KEYS')
                if valid_keys == None:
                    valid_keys = keys
                for key in valid_keys:
                    self.maybe_make_correction_for_pool (sgkeys, tools, key, c,
                                                         corrclass)

        return sgkeys


    def maybe_make_correction_for_pool (self, sgkeys, tools, key, cspec,
                                        corrclass):
        (func, version, order, extra_args) = split_version_spec (cspec)
        name = func.__name__ + '_' + version + '_' + key
        if name in tools:
            return
        tool = func (None, '', version, key, CALOCORR_TOPOOL,
                     generation = self.get_generation(),
                     order = order)
        if tool != None and folder(tool).find ('/' + corrclass + '/') >= 0:
            tools[name] = tool
            sgkeys.setdefault (sgkey(tool), []).append (tool)
        return


    # Add a dummy correction object to each folder.
    def add_dummies (self, sgkeys, valid_keys, generation):
        toolnames = {}
        for sgkey in sgkeys.keys():
            ll = sgkey.split ('-')
            if len(ll) == 1:
                name = ll[0]
            else:
                (name, ver) = ll
            toolnames[name] = 1
        for name in toolnames.keys():
            (corrclass, basename) = name.split ('.')
            sgkey = name + '-dummy'
            tagname = "%s.%s%s-dummy" % (corrclass, generation, basename)
            folder = '/CALO/%s/%s' % (corrclass, basename)
            sgkeys[sgkey] = []
            for key in valid_keys:
                toolname = basename + key + '-dummy'
                tool = CaloDummyCorrection (toolname)
                tool.prefix = key + '.'
                sgkeys[sgkey].append (tool)
                _alltools[toolname] = (folder, tagname, sgkey)
        return


    # Configure algorithms for writing to pool.
    def config_for_pool (self, keys, corrclass):
        from CaloRec.CaloRecMakers import make_CaloClusterCorrDBWriter
        from AthenaCommon.AlgSequence import AlgSequence
        topalg = AlgSequence ("TopAlg")

        corr_output_list = []
        tag_list = []

        generation = self.get_generation()

        sgkeys = self.make_ForPool (keys, corrclass)
        self.add_dummies (sgkeys, keys, generation)

        for (sgkey, tools) in sgkeys.items():
            if len (tools) == 0: continue
            name = 'write_' + sgkey
            name = name.replace ('.', '_')

            alg = make_CaloClusterCorrDBWriter (name, sgkey, tools)
            topalg += alg

            corr_output_list.append ('CaloRec::ToolConstants#' +
                                     sgkey + '#' +
                                     folder (tools[0]))
            tag_list.append (tag (tools[0]))

        print ("Final OutputList:")
        print (corr_output_list)
        print (tag_list)

        return (corr_output_list, tag_list)


    # Given a list of MetatoolHelper objects, construct an appropriate
    # Configurable for a CaloRunClusterCorrections tool to run
    # the desired corrections.
    def make_metatool (self, suffix, tools, preserve_order):
        name = 'CaloRunClusterCorrections' + suffix
        if name in _alltools:
            nsuff = 2
            while (name + str (nsuff)) in _alltools:
                nsuff += 1
            name = name + str (nsuff)
        _alltools[name] = (None, None, None)
        metatool = CaloRunClusterCorrections (name)
        specs = sum ([t.corrspec() for t in tools], [])
        metatool.CorrSpecs = specs
        metatool.PreserveOrder = preserve_order
        log = logging.getLogger ('CaloClusterCorrection')
        log.debug (" making metatool %s" % metatool.getName())
        log.debug ("  specs: %s", specs)

        # # If we're running this to make the trigger DB, turn on
        # # NoClearProps.  But try to avoid importing trigger packages
        # # if they haven't already been read.
        # import sys
        # if sys.modules.has_key ('TriggerJobOpts.TriggerFlags'):
        #     TriggerFlags = sys.modules['TriggerJobOpts.TriggerFlags']
        #     if hasattr (TriggerFlags, 'TriggerFlags'):
        #         TriggerFlags = TriggerFlags.TriggerFlags
        #         if hasattr (TriggerFlags, 'Online'):
        #             Online = TriggerFlags.Online
        #             if Online.doDBConfig():
        #                 metatool.NoClearProps = True

        # Ok, clearing the properties was an interesting idea, but in the
        # usual configuration it won't save very much, and it causes problems
        # with trying to clone algorithms for Hive.  Just disable it for now.
        metatool.NoClearProps = True
            
        return metatool


###########################################################################

#
# Create a correction tool.
# VERSIONS is a table listing all the available versions of the corrections.
# It is a list of 4-element lists.  The last row should correspond to the
# most recent version of the correction.
# The first column is the version name.
# The second column is the Configurable class for the class which implements
# the correction.  This may also be a dictionary, to be indexed by
# a calorimeter sampling.
# The third column is a list containing the possible sources for this tool.
# This list may contain any of the following:
#
#   - A string giving the name of the parameter class used to initialize
#     the correction tool via job options.  It should be of the form
#     `MODULE.CLASS'.  If MODULE has no dots, we look for it in
#     CaloClusterCorrection.
#
#   - A string defined in poolfiles, giving the name of a pool file
#     that contains this correction.
#
#   - The string CALOCORR_COOL, indicating that this correction is available
#     in COOL.
#
#   - The string CALOCORR_NOPOOL, indicating that this tool is not
#     to be written to pool/cool (always gets initialized via job options).
#
# In the case of reading from job options, we look up the specified name.
# If this is a dictionary, we look up the requested sampling in the dictionary.
# We should then have a class; the names in this class are the parameter
# settings to make.  If any of them are dictionaries, then we look up the key
# in the dictionary.
#
# In the case of reading from cool, we construct folder and tag names
# with the following formats:
#
#    /CALO/<class>/<basename>
#    <class>.<basename>-<version>
#
# and a prefix in the format:
#
#    <key><sampling>.
#
# If we've already read a different version of this correction from
# the folder, then the read from cool fails (but we can fall back
# on a direct pool read or initialization from job options).
# Otherwise, we arrange to read this object; the StoreGate key
# will be the same as the folder name.
#
# In the case of reading from pool, the StoreGate key will be the same
# as the tag name we formed above.
#
# The fourth column is a list of cluster type keys that are valid for
# this correction.  When reading from pool, if the key we're given doesn't
# match one on this list, we replace it with the longest match (first
# one wins on ties) from this list.  Set this to None or omit it
# to skip this.
#
# NAME is the base name for this tool, excluding the suffix.  If set to None,
# a name will be constructed from the name of the correction, the version,
# the key, and the sampling (if applicable).
#
# BASENAME is the base name for the correction: a string like `gap',
# `etaoff', etc.
#
# If SUFFIX is not None, it will be added onto the end of the tool name.
#
# VERSION is a string specifying which of several versions of the correction
# to choose.  If set to None, the latest version of the correction is chosen.
# VERSION may also be of the form `@TAG' to force a specific COOL tag TAG,
# or it may be `@GLOBAL' to use to globally-configured COOL tag.
# In such cases, if there's no direct match of the version string
# in the version table, we look for one with a version of `@'.
# If no match for that is found, the last entry in the table
# will be used.
# If VERSION starts with `@', USE_METATOOL must be enabled.
#
# KEY is a string to specify the type of cluster to which the correction
# applies.  The convention is to use `ele55' for 5x5 electron clusters,
# `gam35' for 3x5 photon clusters, and so on.
# If KEY is the special string `@VALID_KEYS', then we do not create
# a tool.  Instead, we return the list of keys that are declared
# to be valid for this tool (or None, if none were).
#
# Some corrections have different tools for different calorimeter samplings.
# If this is the case for this correction, SAMPLING should be set to the
# applicable sampling.  Otherwise, set it to None.
#
# SOURCE tells from where we should read the calibration constants.
#  CALOCORR_JO means to read the constants from job options.
#    Look in the correction's source list to find the parameter class.
#  CALOCORR_POOL means to read the constants from a pool file.
#    Look in the correction's source list to find the pool file.
#  CALOCORR_COOL means to read from cool.
#  A string of the form `MODULE.NAME' means to use this name
#    as the parameter class.
#  A string that's present in the poolfiles dictionary means to read
#    from this pool file.
#
#  SOURCE may also be a list of these, to try in order.
#
#  SOURCE may also be CALOCORR_TOPOOL, meaning to construct a tool
#  from job options appropriate for writing to a pool file.
#
# CONFCLASS gives the Configurable class to use to create the tool.
# If set to None, this is taken from the version table.
#
# CORRCLASS is the correction class string, as defined above.
#
# GENERATION is the generation string to embed in COOL tags.
#
# If USE_METATOOL is true, then instead of making a Configurable,
# return a MetatoolHelper object.
#
# ORDER specifies the relative order in which this tool should be executed.
#
# Additional keyword arguments may be passed to override any tool
# parameters/constants.
#
def makecorr (versions,
              name,
              basename,
              suffix,
              version,
              key,
              sampling,
              source,
              confclass,
              corrclass,
              generation = '',
              use_metatool = False,
              order = 0,
              **kw):

    # If no version specified, use the last one in the table.
    if version == None:
        version = versions[-1][0]

    # Try to find the requested version.
    for v in versions:
        if v[0] == version:
            break
    else:
        if version.startswith ('@'):
            # A COOL tag is requested.
            # Look for a table entry of `@'; otherwise, use the last entry.
            for v in versions:
                if v[0] == '@':
                    break
            else:
                v = versions[-1]
        else:
            raise CaloCorrectionConfigError \
                  ("Can't find version `%s' for correction named `%s'." %
                   (version, basename))

    # The valid keys.
    valid_keys = None
    if len (v) >= 4:
        valid_keys = v[3]

    # Handle @VALID_KEYS request.
    if key == '@VALID_KEYS':
        return valid_keys

    # Use the default source if one wasn't specified.
    if source == None:
        from CaloClusterCorrection.CaloClusterCorrectionFlags \
             import caloClusterCorrectionFlags
        source = caloClusterCorrectionFlags.DefaultSource()

    # Test to see if this correction specifies nopool.
    nopool = CALOCORR_NOPOOL in v[2]

    # Don't try to write a nopool tool to pool.
    if nopool and source == CALOCORR_TOPOOL:
        return None

    # Find the SG key and cool tag.
    # If the tag is specified in the version, then we must be using
    # the metatool, and the SG key doesn't get specified.
    # Otherwise, the tag name is the same as the SG key.
    if version.startswith ('@'):
        #assert use_metatool
        sgkey = None
        if version == '@GLOBAL':
            fulltag = version
        else:
            fulltag = version[1:]

        if not nopool:
            # Must read from cool in this case.
            source = CALOCORR_COOL
    else:
        # Construct the SG key name.
        tmp = basename
        if version != '':
            tmp = tmp + "-" + version
        sgkey = "%s.%s" % (corrclass, tmp)
        fulltag = "%s.%s%s" % (corrclass, generation, tmp)

    # The cool folder name.
    if globalflags.DataSource() == 'data':    
       folder = "/CALO/%s/%s" % (corrclass, basename)
    else:   
       folder = "/CALO/Ofl/%s/%s" % (corrclass, basename)

    # Prefix to locate constants in the ToolConstants block.
    prefix = key + sampnames[sampling] + "."
  
    # Construct a default tool name if one isn't specified.
    if name == None:
        name = basename
        name = name + sampnames[sampling]
        if version != '':
            vv = version
            if vv[0] == '@':
                vv = vv[1:]
            vv = vv.split('-')[-1]
            name = name + "_" + vv
        if key != '':
            name = name + "_" + key

    # Add a suffix if given.
    if suffix != None:
        name = name + suffix

    # If we're not writing to pool, we need to make sure the name's unique.
    if source != CALOCORR_TOPOOL and name in _alltools:
        nsuff = 2
        while (name + str (nsuff)) in _alltools:
            nsuff += 1
        name = name + str (nsuff)
    _alltools[name] = (folder, fulltag, sgkey)

    # If no class was explicitly specified, take it from the table.
    if confclass == None:
        confclass = v[1]

    # It may be sampling-dependent.
    if type (confclass) == type ({}) and sampling != None:
        confclass = confclass[sampling]

    # Create the tool!
    if use_metatool:
        corr = MetatoolHelper (confclass, name)
    else:
        _maybe_patchclass (confclass)
        corr = confclass (name)

    # Set the prefix for all pool-capable tools.
    if not nopool:
        corr.prefix = prefix

    # Try to find a source from which to configure it.
    if type(source) != type([]):
        source = [source]
    avail = v[2]
    wherefrom = None
    for s in source:
        if s == CALOCORR_JO or s == CALOCORR_TOPOOL:
            sel = [x for x in avail if _is_jo_source (x)]
            if len (sel) > 0 and _config_from_jo(corr, sel[0], key,
                                                 sampling , valid_keys,
                                                 order):
                wherefrom = sel[0]
                break

        # elif s == CALOCORR_POOL:
        #     sel = [x for x in avail if _is_pool_source (x)]
        #     if len (sel) > 0 and _config_from_pool (corr, sel[0], sgkey):
        #         if not use_metatool:
        #             _mung_prefix (corr, key, valid_keys)
        #         wherefrom = sel[0]
        #         break

        elif s == CALOCORR_COOL:
            sel = [x for x in avail if _is_cool_source (x)]
            if len (sel) > 0 and config_from_cool (corr, folder, fulltag):
                if not use_metatool:
                    _mung_prefix (corr, key, valid_keys)
                wherefrom = 'cool'
                break

        elif _is_jo_source (s):
            if _config_from_jo (corr, s, key, sampling, valid_keys, order):
                wherefrom = s
                break

        elif _is_pool_source (s):
            if _config_from_pool (corr, s, sgkey):
                if not use_metatool:
                    _mung_prefix (corr, key, valid_keys)
                wherefrom = s
                break

    if wherefrom == None:
        raise CaloCorrectionConfigError \
              ("Can't find any source to configure tool `%s'.  Sources: %s" %
               (name, source))
  
    log = logging.getLogger ('CaloClusterCorrection')
    log.debug (" correction %s (%s, from %s)" % (name, confclass.__name__,
                                                wherefrom))

    # If any other keyword arguments were passed, make those assignments.
    # This will override anything otherwise read from JO/pool.
    for (k, val) in kw.items():
        if val != None:
            _setprop (corr, k, val)

    # Done!
    return corr


# Helper for setting a correction tool property/constant.
def _setprop (obj, k, val):
    if type(val) == type([]) and type(obj.properties()[k]) != type([]):
        val = str(val)
    setattr (obj, k, val)
    return


# Test to see if S looks like a job options source.
def _is_jo_source (s):
    if s == CALOCORR_COOL: return 0
    if s == CALOCORR_POOL: return 0
    if s in poolfiles: return 0
    if s.find ('.') > 0: return 1
    return 0


# Test to see if S looks like a pool source.
def _is_pool_source (s):
    return s in poolfiles


# Test to see if S looks like a cool source.
def _is_cool_source (s):
    return s == CALOCORR_COOL


# Configure a correction tool from job options.
def _config_from_jo (corr, jo, key, sampling, valid_keys, order):
    # Split off the last dotted field as paramclass.
    xjo = jo.split ('.')
    paramclass = xjo[-1]
    modname = '.'.join (xjo[:-1])

    # If not otherwise specified, try to find the module
    # in CaloClusterCorrection.
    if modname.find ('.') < 0:
        modname = 'CaloClusterCorrection.' + modname

    # Import the module and look up the class.
    mod = __import__ (modname, globals(), locals(), [paramclass])
    parms = getattr (mod, paramclass)

    # It may be sampling-dependent.
    if type (parms) == type ({}) and sampling != None:
        parms = parms[sampling]

    if order != 0 and 'order' in corr.properties():
        _setprop (corr, 'order', order)
    if 'isDummy' in corr.properties():
        _setprop (corr, 'isDummy', 0)

    log = logging.getLogger ('CaloClusterCorrection')

    # Go through the names in the class and assign them to the tool.
    for (k, val) in parms.__dict__.items():
        # Skip internal names.
        if k[0] != '_':
            try:
                # First try looking up the key.
                # Note: don't test on val's type here --- it may
                # be an object instance that implements __getitem__.
                # We have to try the lookup and catch the error.
                val = val[key]
            except KeyError:
                # We can do a lookup, but didn't find the key.
                # Use the default key instead.
                default_keys = parms.__dict__.get ('_default_keys')
                if default_keys:
                    defkey = _longest_match (key, default_keys)
                elif valid_keys:
                    defkey = _longest_match (key, valid_keys)
                else:
                    defkey = CALOCORR_DEFAULT_KEY
                val = val[defkey]

                log.debug (" correction %s from JO using %s instead of %s" %
                           (corr.getName(), defkey, key))

            except TypeError:
                # Can't look up a key in val --- just use val as-is.
                pass

            _setprop (corr, k, val)
    return True


# Configure a correction tool from POOL.
def _config_from_pool (corr, poolfile, sgkey):
    if not poolfile or not poolfiles[poolfile]:
        return False
    
    # Tell the tool to look in pool for this key.
    corr.DBHandleKey = sgkey

    # If this is the first time we've seen this file,
    # add it to CondProxyProvider.
    # Pick up the provider by looking in the top-level global scope.
    global ServiceMgr
    if poolfile not in  _poolfiles_seen:
        _poolfiles_seen[poolfile] = 1
        try:
            ServiceMgr.CondProxyProvider.InputCollections += \
                                                          [poolfiles[poolfile]]
        except AttributeError:
            # CondProxyProvider hasn't been made yet?
            from EventSelectorAthenaPool.EventSelectorAthenaPoolConf \
                 import CondProxyProvider
            ServiceMgr += CondProxyProvider()
            ServiceMgr.ProxyProviderSvc.ProviderNames += ["CondProxyProvider"]
            ServiceMgr.CondProxyProvider.InputCollections += \
                                                          [poolfiles[poolfile]]
    return True


# Configure a correction tool from COOL.
def config_from_cool (corr, folder, tag):
    # Folder name has form /CALO/CORRCLASS/NAME
    # Find the subdetector name string to use.
    fsplit = folder.split ('/')
    if fsplit[1] == 'Ofl':
       corrclass = folder.split ('/')[3]
    else:   
       corrclass = folder.split ('/')[2]
    from CaloClusterCorrection.CaloClusterCorrectionFlags \
         import caloClusterCorrectionFlags
    sndict = caloClusterCorrectionFlags.DBSubdetName()
    subdetname = sndict.get (corrclass)
    if not subdetname:
        subdetname = sndict.get (None)

    corr.DBHandleKey = folder

    # We can't use more than one tag from a folder.
    oldtag = _folders_used.get (folder)
    if oldtag != None and oldtag != tag:
        return False

    _folders_used[folder] = tag
    if oldtag == None:
        if tag != '@GLOBAL':
            folder = folder + ' <tag>%s</tag>' % tag
        sdsuffix = '_OFL' if 'Ofl' in folder else ''
        conddb.addFolder (subdetname + sdsuffix,
                          folder,
                          className = 'CaloRec::ToolConstants')
        log = logging.getLogger ('CaloClusterCorrection')
        log.debug ("Adding cool folder `%s' for subdetector name %s" %
                   (folder, subdetname))
    return True


# When we're reading from pool/cool, the prefix _must_ be one that
# exists in the file.  Here, we see if the key is on the list of known
# keys.  If not, then we substitute.
def _matchlen (a, b):
    i = 0
    while i < len(a) and i < len(b) and a[i] == b[i]:
        i = i+1
    return i
def _longest_match (key, valid_keys):
    if type(valid_keys) != type([]):
        valid_keys = [valid_keys]
    if valid_keys == None or key in valid_keys:
        return key
    new_key = valid_keys[0]
    for k in valid_keys[1:]:
        if _matchlen (key, k) > _matchlen (key, new_key):
            new_key = k
    return new_key
def _mung_prefix (corr, key, valid_keys):
    if valid_keys == None or key in valid_keys: return

    # Find the best match.
    new_key = _longest_match (key, valid_keys)

    new_prefix = new_key + corr.prefix[len(key):]
    corr.prefix = new_prefix
    return

# Return the folder for a tool.
def folder (tool):
    return _alltools[tool.getName()][0]


# Return the tag for a tool.
def tag (tool):
    return _alltools[tool.getName()][1]


# Return the SG key for a tool.
def sgkey (tool):
    return _alltools[tool.getName()][2]



##############################################################################
# Hack around a problem with the interaction between Configurable
# and reading tool constants from pool.
#
# When ToolWithConstantsMixin reads constants from pool, we still want
# to allow overriding the constants from job options.  So, if TWCM sees
# a setProperty call for a given property, it assumes that this is being
# overridden from JO, and will use what's set in the setProperty call
# instead of what's read from pool.
#
# This worked fine for non-Configurable setup.  However, when one uses
# Configurable to set up ones class, it ends up setting _all_ the properties,
# even those that are still defaulted.  So, what we end up seeing
# is that TWCM::setProperty gets called for _all_ properties with
# the default --- i.e., null --- values, so the values read from pool
# never get used.
#
# The code here patches the Configurable classes for the corrections
# so that setup() does not transport to C++ those properties that
# were defaulted.
#

from AthenaCommon.Configurable import Configurable
def _calocorr_setup (self):
    save_properties = {}
    try:
        for (k, v) in list(self._properties.items()):
            if self in v.history and len (v.history[self]) >= 1:
                pass
            else:
                save_properties[k] = v
                del self._properties[k]
        Configurable.setup (self)
    finally:
        for (k, v) in save_properties.items():
            self._properties[k] = v
    return

def _maybe_patchclass (cls):
    if '_calocorr_patched' in cls.__dict__:
        return
    cls.setup = _calocorr_setup
    cls._calocorr_patched = 1
    return
