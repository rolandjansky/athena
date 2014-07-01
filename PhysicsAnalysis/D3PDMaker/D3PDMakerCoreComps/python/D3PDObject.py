# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: D3PDObject.py 528970 2012-12-05 11:23:27Z ssnyder $
#
# @file D3PDMakerCoreComps/python/D3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief D3PD object creator class.
#


from Block import Block
from reset_props import reset_props
import D3PDMakerCoreComps
from AthenaCommon.Logging import logging
from StackedDict import StackedDict


def _make_name (name, prefix, name_prefix, parent_prefix, default_prefix):
    """Helper to form the name for object filler tools."""
    if name == None:
        pref = prefix + name_prefix
        if len (pref) == 0: pref = default_prefix
        if len (pref) == 0:
            pref = 'Unnamed_'
        name = parent_prefix + pref + 'Filler'
    return name


def _testLOD (lod, reqlev, args, hookargs):
    """Helper to perform LOD test.

    lod: The level-of-detail to test.  Either an integer or a callable.
    reqlev: Requested level of detail for this object.  An integer.
    args: Arguments to be passed to the block.
    hookargs: Arguments passed to the D3PD object, to be passed
              on to LOD functions.
"""
    # Decide whether to include this block.
    # If the block's LOD is callable, call it.
    # Otherwise, just compare.
    if callable (lod):
        # Allow for an optional third hookargs argument.
        try:
            doblock = lod (reqlev, args, hookargs)
        except TypeError as exc:
            if (len(exc.args) > 0 and
                exc.args[0].find ('takes exactly 2 arguments') >= 0):
                doblock = lod (reqlev, args)
            else:
                raise
    else:
        doblock = (lod <= reqlev)

    return doblock


class DeferArg (object):
    """Defer evaluating a block argument until the D3PDObject is instantiated.

    Sometimes you want to pass an argument to a block function that isn't
    known at the time the D3PDObject is defined, for example the
    StoreGate key to use.  You could do this with a hook function
    or a LOD function, but that's a bit cumbersome.  Instead, you can
    use DeferArg.  DeferArg stores a string.  If a block argument
    has a value that's an instance of DeferArg, then this string
    is evaluated at the time the D3PDObject is instantiated.
    The arguments passed to the D3PDObject will be available
    in the context in which the string is evaluated.
    A global dictionary may be provided as an optional argument;
    any additional keyword arguments will be merged into this
    dictionary.

    Example:

      FooD3PDObject.defineBlock (1, 'SomeBlock', SomeTool,
                                 Name = DeferArg ('"name_" + sgkey'))

    If FooD3PDObject is then instantiated with a StoreGate key of `Bar',
    then the Name argument of this block will be set to `name_Bar'.
"""
    def __init__ (self, str, gdict = globals(), **kw):
        self.str = str
        self.gdict = gdict
        self.kw = kw
        return


    def __call__ (self, hookargs = {}, **kw):
        gdict = self.gdict.copy()
        gdict.update (self.kw)
        hookargs = hookargs.copy()
        hookargs.update (kw)
        return eval (self.str, gdict, hookargs)
    


class D3PDObject:
    """D3PD object creator class.

    For each type of object written to the D3PD, create
    an instance of this class.  The arguments to the constructor
    are a function to create the Configurable for the object filler
    tool and a default value for the variable name prefix
    for this block.

    After creating the instance, one calls defineBlock to define
    the blocks of variables that can be written for this object.

    Finally, one calls the object in order to create the
    configurable.

    So, a typical usage might look like this.

      def makeElectronD3PDObject (name, prefix,
                                  getter = None,
                                  sgkey = 'ElectronAODCollection')
          if not getter:
              getter = D3PDMakerCoreComps.SGDataVectorGetterTool \
                       (name + '_Getter',
                        TypeName = 'ElectronContainer',
                        SGKey = sgkey)

          return D3PDMakerCoreComps.VectorFillerTool (name,
                                                      Prefix = prefix,
                                                      Getter = getter)

      ElectronD3PDObject = D3PDObject (makeElectronD3PDObject, 'el_')

      ElectronD3PDObject.defineBlock (0, 'Kinematics',
                                      EventCommonD3PDMaker.FourMomFillerTool)

      alg = ...
      alg += ElectronD3PDObject (level)
"""

    _hookArgsStore = {}

    
    def __init__ (self, maker, default_prefix = '', default_object_name = '',
                  default_name = None, allow_args = [], **kw):
        """Constructor.

        Arguments:
          maker: A function to create the IObjFillerTool Configurable
                 for this object.  It is called with two fixed arguments,
                 the tool name and the variable name prefix.  Any other
                 arguments provided to the call will be passed through.
          default_prefix: Default variable name prefix for objects
                          of this type.
          default_object_name: Default D3PDObject name, something like
                               'EventInfoD3PDObject'
          default_name: If specified, the default tool name to use
                        for these objects.
          allow_args: Additional arguments that may be passed in when
                      instantiating the D3PDObject, but which are not
                      to be passed to the maker function.  This can be
                      used to allow for additional arguments to hook
                      or level-of-detail functions.

          Any additional arguments are passed into any hook functions.

"""
        self._maker = maker
        self._default_object_name = default_object_name
        self._blocks = []
        self._hooks = []
        self._all_blocknames = {}
        self._default_prefix = default_prefix
        self._default_name   = default_name
        self._hookArgs = kw
        self._parent = None
        self._allow_args = allow_args
        return


    def defineBlock (self, lod, name, func, prefix = None, **kw):
        """Define a new block of variables for this object type.

    Arguments:
      lod: Level of detail of this block.  See below.
      name: Name of this block.
      func:   Function to create the block Configurable.
      prefix: Variable name prefix for this block.
      kw: Arguments to pass to the creation function.

    The creation function will be called like this:

       b.func (name, BlockName = blockname, **b.kw)

    where name is the name for the tool.

    However, if b.func is a class deriving from D3PDObject, then the creation
    function will instead be called like this:

       b.func (level, name, parent_prefix, **b.kw)

    LOD is used to control whether this block should be included.

    In the simple case, this is an integer, and the block is included
    if the block's level of detail is less than or equal to the
    requested level of detail (unless the block was explicitly
    included or excluded).

    In the general case, LOD may be a function.  The first two arguments
    passed to the function will be the requested level of detail
    and the block filler
    arguments.  The requested level of detail will be an integer;
    it will be 999 if the block was explicitly included, and -999
    if the block was explicitly excluded.  The block filler arguments
    is a dictionary of keyword arguments.  The LOD function should
    return a boolean value saying whether or not the block should
    be included.  It may also alter the dictionary of arguments
    (this overrides all other argument settings).
    The LOD function may optionally take a third argument; if present,
    this is a dictionary of the arguments passed to the D3PDObject
    (the same arguments that are passed to hook functions).
"""
        if prefix != None:
            kw = kw.copy()
            kw['Prefix'] = prefix

        # Record this block name.
        # Also recursively record any blocks that it contains.
        # This will check that the names are unique.
        self._add_blockname (name)
        if isinstance (func, D3PDObject):
            for n in func.allBlocknames():
                self._add_blockname (n)
            func._parent = self
            func._allow_args = self._allow_args

        self._blocks.append (Block (name, lod, func, kw))
        return


    def removeBlock (self, name, allowMissing = False):
        """Remove the block NAME from this object."""
        bb = None
        for b in self._blocks:
            if isinstance (b.func, D3PDObject):
                if b.func.removeBlock (name, True):
                    return True
            if b.name == name:
                bb = b
                break

        if not bb:
            if allowMissing:
                return False
            raise ValueError ('Unknown block name: ', name)

        self._blocks.remove (bb)
        self._remove_blockname (name)
        if isinstance (bb.func, D3PDObject):
            for n in bb.func.allBlocknames():
                self._remove_blockname (n)

        return True


    def defineHook (self, hook):
        """Define a new hook function for this D3PD object.

        HOOK is a function that will be called for each instance
        of this D3PD object after all block filler tools have been
        added.

        The arguments to the hook function are the object filler tool,
        followed by the arguments that were passed to __call__.
"""
        self._hooks.append (hook)


    def _add_blockname (self, n, warnonly = False):
        """Add a new block name N to the list of all names.
        Raises an exception if a block name is duplicated.
"""
        if self._all_blocknames.has_key (n):
            if warnonly:
                log = logging.getLogger ('D3PDObject')
                log.warn ('Duplicate block name: %s (%s %s %s)' %
                          (n,
                           self._default_prefix,
                           self._default_name,
                           self._default_object_name))
            else:
                raise ValueError ('Duplicate block name: %s (%s %s %s)' %
                                  (n,
                                   self._default_prefix,
                                   self._default_name,
                                   self._default_object_name))
        self._all_blocknames[n] = 1
        if self._parent:
            self._parent._add_blockname (n, True)
        return


    def _remove_blockname (self, n):
        """Remove block name N."""
        del self._all_blocknames[n]
        if self._parent:
            self._parent._remove_blockname (n)
        return


    def allBlocknames (self):
        """Return a dictionary the keys of which are the names of all blocks
        defined for this object.  This list will include any blocks defined
        recursively.
"""
        return self._all_blocknames


    def __call__ (self,
                  level,
                  name = None,
                  prefix = None,
                  name_prefix = '',
                  parent_prefix = '',
                  object_name = None,
                  include = [],
                  exclude = [],
                  blockargs = {},
                  parent_hookargs = None,
                  *args, **kw):
        """Create a Configurable to fill an object of this type.

        Arguments:
          level: Requested level of detail for this object.  An integer.
          name: Tool name.  If omitted, one will be constructed.
          prefix: Variable name prefix for this object.
          name_prefix: If name wasn't specified, this is a prefix
                       to add to the tool name.
          parent_prefix: Variable name prefix for any parent tool.
          object_name: Name of the D3PDObject, like 'EventInfoD3PDObject'
          include: List of block names to include, regardless
                   of level of detail.
          exclude: List of block names to exclude, regardless
                   of level of detail.
          blockargs: Extra arguments to pass to block filler tools.
                     The keys are block names.
                     Values are dictionaries of keyword arguments.
                     These override anything specified in defineBlock.
          parent_hookargs: [Not to be used by user code.]
                           Dictionary of hook args from the parent,
                           to be merged into our hook arg dictionary.
          args, kw: Additional arguments to pass to the maker function.
                    However, any entries in KW that have the form
                    BLOCK_parm, where BLOCK is a known block name,
                    will instead be interpreted as arguments to pass
                    to a block filler tool.  These override entries
                    in BLOCKARGS.
"""

        # Prefix defaults to that given to the constructor.
        if prefix == None:
            prefix = self._default_prefix

        # Object name defaults to that given in the constructor
        if object_name == None:
            object_name = self._default_object_name

        # The tool name.
        if name == None:
            name = self._default_name
        name = _make_name (name, prefix, name_prefix, parent_prefix,
                           self._default_prefix)

        # Copy blockargs and kw.
        blockargs = dict ([(k, v.copy()) for (k, v) in blockargs.items()])
        kw = kw.copy()

        # Move any block filler args from kw to blockargs.
        for k in kw.keys():
            # It is possible for block names themselves to contain
            # underscores.  So in the case we have more than one underscore
            # in the name, try all prefixes we can make, stopping at
            # the first (shortest) match.
            kk = k.split('_')
            for i in range(1, len(kk)):
                this_key = '_'.join (kk[:i])
                if self._all_blocknames.has_key (this_key):
                    this_var = '_'.join (kk[i:])
                    blockargs.setdefault(this_key,{})[this_var] = kw[k]
                    del kw[k]
                    break

        # Don't pass extra args listed in allow_args to the maker function.
        kw2 = kw.copy()
        for k in self._allow_args:
            if kw2.has_key(k): del kw2[k]

        # Call the maker function.  We actually do this twice.
        # This is going to be a private tool, so the name need not
        # be unique.  But if the name isn't unique, then we'll
        # get any property settings which were specified before.
        # So, we call the maker once to get the (singleton) instance,
        # reset its properties, then call the maker function again.
        try:
            c = self._maker (name=name, prefix=prefix, object_name=object_name,
                             *args, **kw2)
            reset_props (c)
            c = self._maker (name=name, prefix=prefix, object_name=object_name,
                             *args, **kw2)
        except TypeError as exc:
            if (len(exc.args) > 0 and
                exc.args[0].find ("unexpected keyword argument 'object_name'") >= 0):
                log = logging.getLogger ('D3PDObject')
                log.warn ("Maker function missing `object_name' formal arg: %s" % self._maker)
                c = self._maker (name=name, prefix=prefix, *args, **kw2)
                reset_props (c)
                c = self._maker (name=name, prefix=prefix, *args, **kw2)
            else:
                raise

        # Construct arguments to pass to hooks.
        # Use a StackedDict: that allows any additions that a LOD function
        # makes to hookargs to be visible in the rest of the D3PDObject.
        if parent_hookargs == None:
            parent_hookargs = {}
        local_hookargs = {}
        hookargs = StackedDict (local_hookargs, kw,
                                self._hookArgs, parent_hookargs)
        local_hookargs['level'] = level
        local_hookargs['name'] = name
        local_hookargs['prefix'] = prefix
        local_hookargs['name_prefix'] = name_prefix
        local_hookargs['parent_prefix'] = parent_prefix
        local_hookargs['include'] = include
        local_hookargs['exclude'] = exclude
        local_hookargs['blockargs'] = blockargs

        # Create block filler tools.
        self._makeBlockFillers (c, level, parent_prefix + prefix,
                                include, exclude, blockargs, hookargs)

        # We used to run the hooks from here.
        # But in that case, we don't have access to the containing
        # algorithm --- too early for that.
        # So we break out the running of the hooks to a separate method
        # that gets called after the tool gets added to the algorithm.
        # A catch, though, is that we have to remember the arguments
        # that we pass to the hook functions.  We can't attach it
        # to the configurable, since it doesn't have a dictionary.
        # Instead, we need to keep a dictionary here of the tools
        # and the corresponding hook args.
        D3PDObject._hookArgsStore[c.getName()] = (self._hooks, args, hookargs)
        return c


    @staticmethod
    def runHooks (c):
        """Run creation hooks for configurable C.

Should be run by the D3PD maker algorithm; not for general use.
"""
        info = D3PDObject._hookArgsStore.get (c.getName())
        if not info: return
        (hooks, args, hookargs) = info
        for h in hooks:
            h (c,
               *args,
               **hookargs
               )
        return


    def blockFillers (self,
                      level,
                      parent_name,
                      parent_prefix = '',
                      include = [],
                      exclude = [],
                      blockargs = {},
                      hookargs = {}):
        """Return a list of block filler tool configurables.
        
        Arguments:
          level: Requested level of detail for this object.  An integer.
          parent_name: Name of the parent component.
          parent_prefix: Variable name prefix for any parent tool.
          include: List of block names to include, regardless
                   of level of detail.
          exclude: List of block names to exclude, regardless
                   of level of detail.
          blockargs: Extra arguments to pass to block filler tools.
                     The keys are block names.
                     Values are dictionaries of keyword arguments.
                     These override anything specified in defineBlock.
          hookargs: Arguments passed to the D3PD object, to be passed
                    on to LOD functions.
          """
        out = []
        for b in self._blocks:
            # Find the requested level.
            if b.name in include:
                # Always request inclusion.
                reqlev = 999
            elif b.name in exclude:
                # Always request exclusion.
                reqlev = -999
            else:
                # Request level passed in.
                reqlev = level

            # Block filler arguments.
            args = b.kw.copy()
            args.update (blockargs.get (b.name, {}))

            # Need to copy lists in arguments; otherwise, += operations
            # can modify the saved value.
            # Also evaluate deferred arguments here: need to do it before
            # calling the LOD function, as that should have the final
            # say about any changes in the argument list.
            for k, v in args.items():
                if type(v) == type([]):
                    args[k] = v[:]
                if isinstance(v, DeferArg):
                    args[k] = v(hookargs)

            if _testLOD (b.lod, reqlev, args, hookargs):
                fillername = parent_name + '_' + b.name
                if isinstance (b.func, D3PDObject): # ugly!
                    for k in self._allow_args:
                        if hookargs.has_key (k):
                            args[k] = hookargs[k]
                    bf = b.func (level, fillername,
                                 parent_prefix = parent_prefix,
                                 include = include,
                                 exclude = exclude,
                                 blockargs = blockargs,
                                 parent_hookargs = hookargs,
                                 **args)
                else:
                    bf = b.func (fillername, **args)

                out += [bf]
        return out


    def _makeBlockFillers (self,
                           c,
                           level,
                           parent_prefix,
                           include,
                           exclude,
                           blockargs,
                           hookargs):
        """Create block filler tools for object C, for the specified
        level of detail.

        Arguments:
          c: Object for which to create the block fillers.
          level: Requested level of detail for this object.  An integer.
          parent_prefix: Variable name prefix for any parent tool.
          include: List of block names to include, regardless
                   of level of detail.
          exclude: List of block names to exclude, regardless
                   of level of detail.
          blockargs: Extra arguments to pass to block filler tools.
                     The keys are block names.
                     Values are dictionaries of keyword arguments.
                     These override anything specified in defineBlock.
          hookargs: Arguments passed to the D3PD object, to be passed
                    on to LOD functions.
"""
        for bf in self.blockFillers (level, c.getName(), parent_prefix,
                                     include, exclude, blockargs,
                                     hookargs):
            c.BlockFillers += [bf]
            c += [bf]
        return



    def copy (self,
              default_prefix = None,
              default_object_name = None,
              default_name = None,
              **kw_in):
        """Return a copy of this D3PD object, possibly changing parameters."""
        if default_prefix==None:
            default_prefix = self._default_prefix
        if default_object_name==None:
            default_object_name = self._default_object_name
        if default_name==None:
            default_name = self._default_name
        kw = self._hookArgs.copy()
        kw.update (kw_in)

        c = D3PDObject (self._maker,
                        default_prefix,
                        default_object_name,
                        default_name,
                        **kw)

        #def defineBlock (self, lod, name, func, prefix = None, **kw):
        for b in self._blocks:
            func = b.func
            if isinstance (func, D3PDObject):
                func = func.copy()
            c.defineBlock (b.lod, b.name, func, **b.kw)

        for h in self._hooks:
            c.defineHook (h)

        return c


def make_Void_D3PDObject (default_prefix = '',
                          default_object_name = '',
                          default_name = None,
                          allow_args = []):
    """Helper to make a D3PDObject for the case where no explicit
input object is being used.    

        Arguments:
          default_prefix: The default prefix to put in front of variables.
          default_object_name: The name of the D3PDObject which is created,
                               like 'EventInfoD3PDObject'
          default_name: If specified, the default tool name to use
                        for these objects.
          allow_args: Additional arguments that may be passed in when
                      instantiating the D3PDObject, but which are not
                      to be passed to the maker function.  This can be
                      used to allow for additional arguments to hook
                      or level-of-detail functions.

        Typical usage would be something like this:

           METD3PDObject = \
              make_Void_D3PDObject ('trig_', 'METD3PDObject')
        
"""
    def make_obj (name, prefix, object_name):
        from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
        return D3PDMakerCoreComps.VoidObjFillerTool (name,
                                                     Prefix = prefix,
                                                     ObjectName = object_name,
                                                     SaveMetadata = \
                                                     D3PDMakerFlags.SaveObjectMetadata())
    return D3PDObject (make_obj, default_prefix, default_object_name,
                       default_name = default_name,
                       allow_args = allow_args)



def make_SG_D3PDObject (default_typeName,
                        default_sgkey,
                        default_prefix,
                        default_object_name = None,
                        default_allowMissing = False,
                        default_getterClass = \
                          D3PDMakerCoreComps.SGObjGetterTool,
                        allow_args = []):
    """Helper to make a D3PDObject for the common case where the default
input source is a single object from StoreGate.

        Arguments:
          default_typeName: The default name of the type being
                            fetched from StoreGate.
          default_sgkey: The default value for the StoreGate key.
          default_prefix: The default prefix to put in front of variables.
          default_object_name: The name of the D3PDObject which is created,
                               like 'EventInfoD3PDObject'
          default_allowMissing: The default value for the AllowMissing
            property (defaults to False).
          default_getterClass: Default value to use for the getter class,
            when we create the getter.  Defaults to SGObjGetterTool.
          allow_args: Additional arguments that may be passed in when
                      instantiating the D3PDObject, but which are not
                      to be passed to the maker function.  This can be
                      used to allow for additional arguments to hook
                      or level-of-detail functions.

        Typical usage would be something like this:

           METD3PDObject = \
              make_SG_D3PDObject ('MissingET',
                                  D3PDMakerFlags.MissingETSGKey(),
                                  'met_', 'METD3PDObject')
        
"""
    def make_obj (name, prefix, object_name,
                  getter = None,
                  sgkey = None,
                  allowMissing = default_allowMissing,
                  typeName = default_typeName,
                  getterClass = default_getterClass):
        if sgkey == None: sgkey = default_sgkey
        if not getter:
            getter = getterClass (name + '_Getter',
                                  TypeName = typeName,
                                  SGKey = sgkey)
        from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
        return D3PDMakerCoreComps.ObjFillerTool (name,
                                                 Prefix = prefix,
                                                 Getter = getter,
                                                 ObjectName = object_name,
                                                 AllowMissing=allowMissing,
                                                 SaveMetadata = \
                                                 D3PDMakerFlags.SaveObjectMetadata())

    if default_object_name == None:
        default_object_name = default_typeName
        default_object_name = default_object_name.split('::')[-1]

    return D3PDObject (make_obj, default_prefix, default_object_name,
                       sgkey = default_sgkey,
                       typeName = default_typeName,
                       allow_args = allow_args)



def make_SGDataVector_D3PDObject (default_typeName,
                                  default_sgkey,
                                  default_prefix,
                                  default_object_name = None,
                                  default_allowMissing = False,
                                  default_getterFilter = None,
                                  default_label = None,
                                  default_getterClass = \
                                    D3PDMakerCoreComps.SGDataVectorGetterTool,
                                  allow_args = [],
                                  **other_defaults):
    """Helper to make a D3PDObject for the common case where the default
input source is a DataVector container from StoreGate.

        Arguments:
          default_typeName: The default name of the type being
                            fetched from StoreGate.
          default_sgkey: The default value for the StoreGate key.
          default_prefix: The default prefix to put in front of variables.
          default_object_name: The name of the D3PDObject which is created,
                               like 'EventInfoD3PDObject'
          default_allowMissing: The default value for the AllowMissing
            property (defaults to False).
          default_label: The default value to use for the collection label
            (default to the prefix).
          default_getterFilter: A collection getter filter to wrap
            around the primary getter (defaults to no filter).
            If this is a string, then it is interpreted as the
            StoreGate key of a SelectedParticles object to use
            to do the filtering.
          default_getterClass: Default value to use for the getter class,
            when we create the getter.  Defaults to SGDataVectorGetterTool,
            which is appropriate for iterating over a DataVector.
          allow_args: Additional arguments that may be passed in when
                      instantiating the D3PDObject, but which are not
                      to be passed to the maker function.  This can be
                      used to allow for additional arguments to hook
                      or level-of-detail functions.

        Typical usage would be something like this:

           JetD3PDObject = \
              make_SGDataVector_D3PDObject ('JetCollection',
                                            D3PDMakerFlags.JetSGKey(),
                                            'jet_', 'JetD3PDObject')

        In addition, each object has automatically defined a
        ContainerFlagFillerTool block named `SelectionFlags'.
        This means that you can pass in an argument like

          SelectionFlags_FlagNames [ 'selected@MyContainerType/mine:Objects in mine' ]

        This will create a flag variable `selected' which will be true if
        the object contained in the SG container MyContainerType/mine.
        The string after the colon is used for documentation.
"""
    def make_obj (name, prefix, object_name,
                  getter = None,
                  sgkey = None,
                  label = default_label,
                  allowMissing = default_allowMissing,
                  getterFilter = default_getterFilter,
                  typeName = default_typeName,
                  getterClass = default_getterClass,
                  **kw):
        if sgkey == None: sgkey = default_sgkey
        if label == None: label = prefix
        if not getter:
            getter = getterClass (name + '_Getter',
                                  TypeName = typeName,
                                  SGKey = sgkey,
                                  Label = label)
        if getterFilter:
            if type(getterFilter) == type(''):
                selgetter = D3PDMakerCoreComps.SGObjGetterTool \
                            (name + '_SelectionGetter',
                             TypeName = 'SelectedParticles',
                             SGKey = getterFilter)
                getter = D3PDMakerCoreComps.SelectedParticlesFilterTool \
                         (name + '_GetterFilter',
                          Getter = getter,
                          SelectionGetter = selgetter)
            else:
                getter = getterFilter (name + '_GetterFilter',
                                       Getter = getter)
        defs = other_defaults.copy()
        defs.update (kw)
        from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
        return D3PDMakerCoreComps.VectorFillerTool (name,
                                                    Prefix = prefix,
                                                    Getter = getter,
                                                    ObjectName = object_name,
                                                    AllowMissing=allowMissing,
                                                    SaveMetadata = \
                                                    D3PDMakerFlags.SaveObjectMetadata(),
                                                    **defs)

    if default_object_name == None:
        default_object_name = default_typeName
        if default_object_name.endswith ('Collection'):
            default_object_name = default_object_name[:-10]
        if default_object_name.endswith ('Container'):
            default_object_name = default_object_name[:-9]
        default_object_name = default_object_name.split('::')[-1]

    ret = D3PDObject (make_obj, default_prefix, default_object_name,
                      sgkey = default_sgkey,
                      typeName = default_typeName,
                      allow_args = allow_args)

    # Automatically add SelectionFlags to each object.
    # It won't actually create any variables unless the Flags property is set.
    ret.defineBlock (0, 'SelectionFlags',
                     D3PDMakerCoreComps.ContainerFlagFillerTool)

    return ret

