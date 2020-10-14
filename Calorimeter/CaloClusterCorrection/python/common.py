# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/common.py
# Created: Nov 2006, sss
# Purpose: Common utility code for configuring cluster corrections.
#

from __future__ import print_function

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging
from fnmatch import fnmatchcase

from CaloClusterCorrection.poolfiles import poolfiles

from CaloClusterCorrection.constants import \
     CALOCORR_JO, CALOCORR_POOL, CALOCORR_COOL, \
     CALOCORR_NOPOOL, CALOCORR_TOPOOL, CALOCORR_DEFAULT_KEY, \
     sampnames

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
#  version_override_flag_name:
#                         Name of flag.
#                         If set, it overrides the selected correction
#                         version.
#
#  correction_generation_flag_name:
#                         Name of flag.
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
#                         ORDER is an integer, giving the order in which
#                         the tools are intended to run.
#                         Not currently used (they were used by the metatool),
#                         but retained in case we want to use this
#                         for checking.
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
    if ii < len(cspec) and isinstance (cspec[ii], str):
        version = cspec[ii]
        ii += 1
    if ii < len(cspec) and isinstance (cspec[ii], int):
        order = cspec[ii]
        ii += 1
    extra_args = cspec[ii:]
    return (func, version, order, extra_args)


# Given a cool tag (or GLOBAL for the currently-configured global tag),
# return the version of the correction set to use.
def _find_version_from_cool_tag (flags, coolTag, corrclass):
    if coolTag == 'GLOBAL':
        coolTag = flags.IOVDb.GlobalTag

    if flags.Input.isMC:    
        folderset = "/CALO/Ofl/" + corrclass
        connstring = 'COOLOFL_CALO/OFLP200'
    else:   
        folderset = "/CALO/" + corrclass
        connstring = 'COOLONL_CALO/' + flags.IOVDb.DatabaseInstance

    from CoolConvUtilities import AtlCoolLib
    db = AtlCoolLib.indirectOpen (connstring, readOnly=True, oracle=True)
    ff = db.getFolderSet (folderset)
    t = ff.resolveTag (coolTag)
    #  CaloOflSwClusterCorrections.00-02-12-calhits-v9
    l = t.split ('-')
    return '-'.join (l[4:])


class CaloClusterCorrSetup:
    # Must be overridden in derived classes.  See above for their meaning.
    # name = XXX
    # version_override_flag_name = XXX
    # correction_generation_flag_name = XXX
    # correction_generation_default = XXX
    # versions = XXX
    # geom_versions = XXX
    # newest_version = XXX

    #
    # Create and return a CA of correction tools.
    # FLAGS is the configuration flags.
    # CORRCLASS is the correction class string, as defined in constants.py.
    # KEY is a string that specifies the correction type.
    # SUFFIX is a string to add to the end of each tool name.
    # VERSION specifies which version of corrections to use.
    # VERSION may also be of the form `@TAG' to force a specific COOL tag TAG,
    # or it may be `@GLOBAL' to use to globally-configured COOL tag.
    # In such cases, if there's no direct match of the version string
    # in the version table, we look for one with a version of `@'.
    # If no match for that is found, the last entry in the table
    # will be used.
    # CORRLIST can be used to explicitly specify which corrections to run.
    # CELLS_NAME is the SG key to use to find the calorimeter cells,
    # for those corrections that require it.
    # SOURCE specifies the source(s) from which tools are configured.
    # None means to use the default.
    #
    def make_corrections (self,
                          flags,
                          corrclass,
                          key = None,
                          suffix = '',
                          version = None,
                          corrlist = None,
                          cells_name = None,
                          source = None,
                          **kw):

        # Make a logger.
        log = logging.getLogger ('CaloClusterCorrection')

        tryhier = False

        # First, find the appropriate overall version string to use,
        # if it wasn't specified explicitly.
        if version is None:
            # First see if the flag was set to override the version.
            # at the top-level.
            v = getattr (flags.Calo.ClusterCorrection, self.version_override_flag_name)
            if v:
                version = v

        if version is None:
            # No explicitly specified version.
            # Try to guess it from the data source + geometry string.
            # Otherwise, use the latest version.
            geom = flags.GeoModel.AtlasVersion
            datasource = 'geant4' if flags.Input.isMC else 'data'
            (version, tryhier) = self.geom_match (datasource, geom)

        # Get the correction generation.
        generation = self.get_generation(flags)

        # Use the default source if one wasn't specified.
        if source is None:
            source = flags.Calo.ClusterCorrection.defaultSource

        if not isinstance (source, list):
            source = [source]
        if CALOCORR_COOL not in source:
            tryhier = False

        if tryhier and version[0] != '@':
            cl = corrclass
            if flags.Input.isMC:
                cl = cl[0:4] + 'Ofl' + cl[4:]
            version = "@%s-%s%s" % (cl, generation, version)

        # Only use COOL if we're actually trying to resolve a COOL tag.
        # Otherwise we can run into problems: it looks like `ununsed'
        # COOL tags are removed in DB replicas.
        if version[0] != '@' and len(source) > 0:
            if CALOCORR_COOL in source:
                source.remove (CALOCORR_COOL)

        (vcorrlist, version) = self.lookup_version (flags, version, corrclass)

        # Default to the standard list if no explicit correction list.
        if corrlist is None:
            corrlist = vcorrlist

        log.info ("%s corrections for %s (%s) using version %s" %
                  (self.name, key, suffix, version))

        # Now, walk through the list of corrections.
        out = ComponentAccumulator()
        tools = []
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
            ca = func (flags,
                       this_cells_name,
                       this_suffix,
                       this_version,
                       this_key,
                       this_source,
                       generation = generation,
                       order = this_order,
                       **this_args)
            tools.append (out.popToolsAndMerge (ca))

        out.setPrivateTools (tools)
        return out


    # Look up a specific correction version; return the correction
    # list and the version.  Broken out to allow overriding, for
    # any special version-name handling needed.
    def lookup_version (self, flags, version, corrclass):
        vcorrlist = self.versions.get (version)
        if vcorrlist is None and version.startswith ('@'):
            findvers = _find_version_from_cool_tag (flags, version[1:], corrclass)
            vcorrlist = self.versions.get (findvers)
            if vcorrlist is None:
                vcorrlist = self.versions.get ('@')
        if vcorrlist is None:
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
    def get_generation (cls, flags):
        generation = cls.correction_generation_default
        gen2 = getattr (flags.Calo.ClusterCorrection, cls.correction_generation_flag_name)
        if gen2:
            generation = gen2
        if generation != '':
            generation = generation + '-'
        return generation


    #####################################################################
    # Code for writing to pool.
    # Make a set of corrections for writing to pool.

    def make_ForPool (self, flags, keys, corrclass):
        # Map from SG key to a list of tool instances.
        sgkeys = {}

        # Names of all tools made so far
        tool_names = set()

        # Map from (func, version) to (order, globversion).
        # For consistency checking.
        ordermap = {}

        result = ComponentAccumulator()
        tools = []

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
                valid_keys = func (flags,
                                   None,
                                   version = version,
                                   key = '@VALID_KEYS')
                if valid_keys is None:
                    valid_keys = keys
                for key in valid_keys:
                    ca = self.maybe_make_correction_for_pool (flags,
                                                              sgkeys,
                                                              tool_names,
                                                              key, c,
                                                              corrclass)
                    if ca:
                        tools.append (ca.popPrivateTools())
                        result.merge (ca)

        result.setPrivateTools (tools)

        return (sgkeys, result)


    def maybe_make_correction_for_pool (self, flags,
                                        sgkeys, tool_names, key, cspec,
                                        corrclass):
        (func, version, order, extra_args) = split_version_spec (cspec)
        name = func.__name__ + '_' + version + '_' + key
        if name in tool_names:
            return None
        ca = func (flags,
                   None, '', version, key, CALOCORR_TOPOOL,
                   generation = self.get_generation (flags),
                   order = order)
        if ca is None:
            return None
        tool = ca.popPrivateTools()
        if tool is not None and folder(tool).find ('/' + corrclass + '/') >= 0:
            tool_names.add (name)
            sgkeys.setdefault (sgkey(tool), []).append (tool)
        ca.setPrivateTools (tool)
        return ca


    # Add a dummy correction object to each folder.
    def add_dummies (self, sgkeys, valid_keys, generation):
        CaloDummyCorrection = CompFactory.CaloDummyCorrection # CaloClusterCorrection
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
    def config_for_pool (self, flags, keys, corrclass):
        result = ComponentAccumulator()

        corr_output_list = []
        tag_list = []

        generation = self.get_generation (flags)

        (sgkeys, ca) = self.make_ForPool (flags, keys, corrclass)
        ca.popPrivateTools()
        result.merge (ca)
        self.add_dummies (sgkeys, keys, generation)

        for (sgkey, tools) in sgkeys.items():
            if len (tools) == 0: continue
            name = 'write_' + sgkey
            name = name.replace ('.', '_')

            # In CaloRec
            alg = CompFactory.CaloClusterCorrDBWriter (name,
                                                       key = sgkey,
                                                       ClusterCorrectionTools = tools)
            result.addEventAlgo (alg)
                                                       

            corr_output_list.append ('CaloRec::ToolConstants#' +
                                     sgkey + '#' +
                                     folder (tools[0]))
            tag_list.append (tag (tools[0]))

        print ("Final OutputList:")
        print (corr_output_list)
        print (tag_list)

        return (corr_output_list, tag_list, result)


###########################################################################

#
# Create a correction tool.
# FLAGS is the corrections flag object.
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
# ORDER specifies the relative order in which this tool should be executed.
#
# Additional keyword arguments may be passed to override any tool
# parameters/constants.
#
def makecorr (flags,
              versions,
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
              order = 0,
              **kw):

    # If no version specified, use the last one in the table.
    if version is None:
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
    if source is None:
        source = flags.Calo.ClusterCorrection.defaultSource

    # Test to see if this correction specifies nopool.
    nopool = CALOCORR_NOPOOL in v[2]

    # Don't try to write a nopool tool to pool.
    if nopool and source == CALOCORR_TOPOOL:
        return None

    # Find the SG key and cool tag.
    if version.startswith ('@'):
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
    if not flags.Input.isMC:
       folder = "/CALO/%s/%s" % (corrclass, basename)
    else:   
       folder = "/CALO/Ofl/%s/%s" % (corrclass, basename)

    # Prefix to locate constants in the ToolConstants block.
    prefix = key + sampnames[sampling] + "."
  
    # Construct a default tool name if one isn't specified.
    if name is None:
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
    if suffix is not None:
        name = name + suffix

    # If we're not writing to pool, we need to make sure the name's unique.
    if source != CALOCORR_TOPOOL and name in _alltools:
        nsuff = 2
        while (name + str (nsuff)) in _alltools:
            nsuff += 1
        name = name + str (nsuff)
    _alltools[name] = (folder, fulltag, sgkey)

    # If no class was explicitly specified, take it from the table.
    if confclass is None:
        confclass = v[1]

    # It may be sampling-dependent.
    if isinstance (confclass, dict) and sampling is not None:
        confclass = confclass[sampling]

    result = ComponentAccumulator()

    # Create the tool!
    corr = confclass (name)

    # Set the prefix for all pool-capable tools.
    if not nopool:
        corr.prefix = prefix

    # Try to find a source from which to configure it.
    if not isinstance (source, list):
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

        elif s == CALOCORR_POOL:
            sel = [x for x in avail if _is_pool_source (x)]
            if len (sel) > 0:
                ca2 = _config_from_pool (flags, corr, sel[0], sgkey)
                if ca2:
                    result.merge (ca2)
                    _mung_prefix (corr, key, valid_keys)
                    wherefrom = sel[0]
                    break

        elif s == CALOCORR_COOL:
            sel = [x for x in avail if _is_cool_source (x)]
            if len (sel) > 0:
                ca2 = _config_from_cool (flags, corr, folder, fulltag)
                if ca2:
                    result.merge (ca2)
                    _mung_prefix (corr, key, valid_keys)
                    wherefrom = 'cool'
                    break

        elif _is_jo_source (s):
            if _config_from_jo (corr, s, key, sampling, valid_keys, order):
                wherefrom = s
                break

        elif _is_pool_source (s):
            ca2 = _config_from_pool (flags, corr, s, sgkey)
            if ca2:
                result.merge (ca2)
                _mung_prefix (corr, key, valid_keys)
                wherefrom = s
                break

    if wherefrom is None:
        raise CaloCorrectionConfigError \
              ("Can't find any source to configure tool `%s'.  Sources: %s" %
               (name, source))
  
    log = logging.getLogger ('CaloClusterCorrection')
    log.debug (" correction %s (%s, from %s)" % (name, confclass.__name__,
                                                wherefrom))

    # If any other keyword arguments were passed, make those assignments.
    # This will override anything otherwise read from JO/pool.
    for (k, val) in kw.items():
        if val is not None:
            setattr (corr, k, val)

    # Done!
    result.setPrivateTools (corr)
    return result


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
    if isinstance (parms, dict) and sampling is not None:
        parms = parms[sampling]

    if order != 0 and hasattr (corr, 'order'):
        setattr (corr, 'order', order)
    if hasattr (corr, 'isDummy'):
        setattr (corr, 'isDummy', 0)

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

            setattr (corr, k, val)
    return True


# Configure a correction tool from POOL.
def _config_from_pool (flags, corr, poolfile, sgkey):
    if not poolfile or not poolfiles[poolfile]:
        return False
    
    ca = ComponentAccumulator()

    # If this is the first time we've seen this file,
    # add it to CondProxyProvider.
    if poolfile not in  _poolfiles_seen:
        from EventSelectorAthenaPool.CondProxyProviderConfig import CondProxyProviderCfg
        ca.merge (CondProxyProviderCfg (flags, [poolfiles[poolfile]]))

    # Tell the tool to look in pool for this key.
    corr.DBHandleKey = sgkey

    # Set up a conditions algorithm to convert from the data in DetectorStore
    # to a conditions object.
    ToolConstantsCondAlg = CompFactory.ToolConstantsCondAlg # CaloRec

    name = 'ToolConstantsCondAlg_' + sgkey.replace ('.', '_')
    alg = ToolConstantsCondAlg (name,
                                DetStoreKey = sgkey,
                                ToolConstantsKey = sgkey)
    ca.addCondAlgo (alg)
    return ca


# Configure a correction tool from COOL.
def _config_from_cool (flags, corr, folder, tag):
    # Folder name has form /CALO/CORRCLASS/NAME
    # Find the subdetector name string to use.
    fsplit = folder.split ('/')
    if fsplit[2] == 'Ofl':
       corrclass = folder.split ('/')[4]
    else:   
       corrclass = folder.split ('/')[3]
    sndict = flags.Calo.ClusterCorrection.dbSubdetName
    subdetname = sndict.get (corrclass)
    if not subdetname:
        subdetname = sndict.get (None)

    corr.DBHandleKey = folder

    # We can't use more than one tag from a folder.
    oldtag = _folders_used.get (folder)
    if oldtag is not None and oldtag != tag:
        return False

    ca = ComponentAccumulator()

    _folders_used[folder] = tag
    if oldtag is None:
        from IOVDbSvc.IOVDbSvcConfig import addFolders
        tagstr = '' if tag =='@GLOBAL' else tag
        sdsuffix = '_OFL' if 'Ofl' in folder else ''
        ca.merge (addFolders (flags,
                              folder,
                              detDb = subdetname + sdsuffix,
                              className = 'CaloRec::ToolConstants',
                              tag = tagstr))
                               
        log = logging.getLogger ('CaloClusterCorrection')
        log.debug ("Adding cool folder `%s' for subdetector name %s" %
                   (folder, subdetname))
    return ca


# When we're reading from pool/cool, the prefix _must_ be one that
# exists in the file.  Here, we see if the key is on the list of known
# keys.  If not, then we substitute.
def _matchlen (a, b):
    i = 0
    while i < len(a) and i < len(b) and a[i] == b[i]:
        i = i+1
    return i
def _longest_match (key, valid_keys):
    if not isinstance (valid_keys, list):
        valid_keys = [valid_keys]
    if valid_keys is None or key in valid_keys:
        return key
    new_key = valid_keys[0]
    for k in valid_keys[1:]:
        if _matchlen (key, k) > _matchlen (key, new_key):
            new_key = k
    return new_key
def _mung_prefix (corr, key, valid_keys):
    if valid_keys is None or key in valid_keys: return

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



