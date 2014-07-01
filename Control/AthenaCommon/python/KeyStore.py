# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: AthenaCommon/python/KeyStore.py
# @author: Sebastien Binet (binet@cern.ch)

### data
__version__ = '$Revision: 1.11 $'
__author__  = 'Sebastien Binet <binet@cern.ch>'

__all__ = [ "CfgItemList", "CfgKeyStore",
            "loadKeyStoreFromPoolFile",
            "keystore_diff" ]

import weakref
import os
import copy
import types
import re

_allowedTriggerKeys = re.compile( r"(?P<KlassName>.*?)#HLTAutoKey.*\*$" )

### Configurable code we can recycle
from Configurable import Configurable

### logging
from AthenaCommon.Logging import logging
msg = logging.getLogger( 'CfgItemList' )

class CfgItemList( object ):
    """ Python class to hold the so-called output ItemList of a given JobOption.
    This class allows to fine tune what will be written out in a POOL file, as
    it has a switch to prevent people from wildcarding containers (ie: no
    outStream.ItemList += [ 'SomeContainer#*' ] allowed).
    This check can however be disabled on a per CfgItemList instance basis:
    @code
    >>> mycfg = CfgItemList( allowWildcard = True )
    @endcode
    
    A typical use would look like:
    @code
    esdList = CfgItemList( 'esd' )
    # fetch an already existing list (using the singleton-ness of Configurables)
    esdList += CfgItemList( 'InDetEsd' )
    # create a list on the fly and add it to the ESD list
    esdList += CfgItemList(
                 'CaloEsd',
                 items = [ 'CaloCellContainer#CaloCells',
                           'CaloClusterContainer#Clusters' ]
                 )
    # special case of trigger
    esdList += CfgItemList(
                 'TrigEsd',
                 items = [ 'JetCollection#HLT_AutoKey#*' ],
                 allowWildCard = True
                 )
    # fill the stream's output list
    from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    StreamESD = AthenaPoolOutputStream('StreamESD')
    StreamESD.ItemList = esdList()
    @endcode
    """
    instances = weakref.WeakValueDictionary() # all item lists (by name)
    __slots__ = { '_name'          : 'CfgItemList',
                  '__weakref__'    : None, # required for dealing w/ weak refs
                  '_children'      : None,
                  '_items'         : None, # list of the form ['CppCont#Key',]
                  '_allowWildCard' : False }

    def __new__(cls, *p, **kw):

        if not kw.has_key('name'):
            if len(p) == 0: kw['name'] = cls.__slots__['_name']
            else:           kw['name'] = p[0]

        kw['_name'] = kw['name']
        
        # already created...
        if kw['name'] in cls.instances.keys():
            return cls.instances[kw['name']]

        # create instance
        obj = object.__new__(cls)
        object.__init__(obj)

        del kw['name']
        
        for k in cls.__slots__.keys():
            if k.startswith('__'): continue
            if k in kw.keys(): v = kw[k]
            else: v = cls.__slots__[k]
            setattr(obj, k, v)
            
        # update repository of instances
        cls.instances[kw['_name']] = obj
        
        return obj

    def __init__( self, *p, **kwargs ):

        # call base class init
        super( CfgItemList, self ).__init__()

        if not self._children: self._children = [] 
        if not self._items:    self._items    = []

        items = []
        if kwargs.has_key('items'):
            items = kwargs['items']

        if kwargs.has_key('allowWildCard'):
            self._allowWildCard = kwargs['allowWildCard']
            
        msg.verbose( "create [%s] items = %r allowWildCard = %s",
                     self.name(), items, self._allowWildCard )

        if len(items) > 0: self.add( items )
        return

    ## pickle support
    def __getstate__( self ):
        d = { '_name'          : self._name,
              '_children'      : self._children,
              '_items'         : self._items,
              '_allowWildCard' : self._allowWildCard,
              }
        return d

    def __setstate__( self, dct ):
        for k,v in dct.items():
            setattr(self, k, v)
        return
    
    def __getnewargs__( self ):
        return (self.name(),)
    ##
    
    def __deepcopy__( self, memo ):
        newconf = super( CfgItemList, self).__deepcopy__(memo)
        newconf._children = copy.deepcopy(self._children)
        newconf._items    = copy.deepcopy(self._items)
        newconf._allowWildCard = copy.deepcopy(self._allowWildCard)
        return newconf
    
    # to allow a few basic sanity checks, as well as nice syntax
    def __len__( self ):
        return len( self._children )

    def __iter__( self ):
        return iter( self._children )

    # hierarchy building, and ownership rules of children
    def __iadd__( self, itemLists ):
        if not type(itemLists) in (list,tuple):
            itemLists = ( itemLists, )

        name = self._name

        for cfg in itemLists:
            # prevent type mismatches
            if not isinstance( cfg, CfgItemList ):
                raise TypeError( "'%r' is not a CfgItemList" % cfg )

            # filters dupes; usually "ok" (backdoor should catch them)
            for c in self._children:
                if c._name == cfg._name:
                    msg.error( '%s already exists !! (in %s)',
                               cfg._name, self._name )
                    msg.error( 'attempt to add a duplicate ... dupe ignored' )
                    break
            else:
                self._children.append( cfg )
            try:
                setattr(self, cfg._name, cfg)
            except AttributeError:
                pass
            
        return self

    def __getattr__( self, attr ):

        if attr in CfgItemList.__slots__:
            return super(CfgItemList, self).__getattribute__(attr)
        
        for c in self.children():
            if c._name == attr:
                return c

        return super(CfgItemList, self).__getattribute__(attr)
    
##         raise AttributeError( "CfgItemList' object has no attribute '%s'" %
##                               ( attr ) )

    def __setattr__( self, k, v ):
        if k in CfgItemList.__slots__:
            return super(CfgItemList, self).__setattr__(k, v)
        for c in self.children():
            if c.name() == k:
                return setattr(c, k, v)
        try:
            return super(CfgItemList, self).__setattr__(k, v)
        except AttributeError:
            pass
        
    def __delattr__( self, attr ):
        for c in self._children:
            if c._name == attr:
                self._children.remove( c )

    def __nonzero__(self):
        return True

    def removeItem( self, item ):
        if not type(item) in ( types.ListType, types.TupleType ):
            items = ( item, )
        else:
            items = item
        for item in items:
            for i in self._items:
                if i == item:
                    self._items.remove( i )
        return
    
    def removeAllItems( self, item ):
        self.removeItem( item )
        for c in self._children:
            c.removeAllItems( item )
        return
    
    def remove( self, items ):
        if not type(items) in ( types.ListType, types.TupleType ):
            items = [ items ]

        self._children = [ e for e in self._children if not e in items ]

    def removeAll( self ):
        self.remove( self._children )

    def clear( self ):
        self.removeAll()
        self._items = []
        
    def setup( self ):
        msg.debug( 'setup of Output ItemList: %s', self._name )
        if msg.isEnabledFor( logging.VERBOSE ):
            msg.verbose( 'layout of sequence: %s\n%s', self._name, str(self) )

        for child in self.children():
            child.setup()

    def getProperties( self ):
        props = {}
        for item in self._items:
            cppType = item.split("#")[0]
            sgKey   = item.replace( cppType+"#", '' )
            if props.has_key( cppType ):
                if sgKey not in props[cppType]:
                    props[cppType].append( sgKey )
            else:
                props[cppType] = [ sgKey ]
        return props

    def _addItem( self, i ):
        if i.count( "*" ) <= 0:
            if i not in self._items:
                self._items.append( i )
        elif (self._allowWildCard or _allowedTriggerKeys.match(i)):
            if i not in self._items:
                self._items.append( i )
        else:
            msg.warning( "Attempt to add an invalid element: '%s'", i )
            msg.warning( "     ==> Ignoring invalid element: '%s'", i )
        return
                    
    def add( self, arg ):
        if isinstance(arg, (list, tuple) ):
            for a in set(arg):
                self._addItem( a )
        elif isinstance(arg, str):
            self._addItem( arg )
        elif isinstance(arg, dict):
            new = arg.copy()
            for k in new.keys():
                if not isinstance(new[k], (list,tuple)):
                    new[k] = ( new[k], )
                self.add( [ '%s#%s' % (k,e) for e in new[k] ] )
        else:
            msg.error( "Could not add '%r' of type '%s'", arg, type(arg) )
        return

    def has_item( self, item ):
        from fnmatch import fnmatchcase as match
        for i in self._items:
            if match(i, item):
                return True

        ## still there ? Try children then...
        for c in self._children:
            if c.has_item( item ):
                return True
        return False
    
    def allowWildCard( self, allow ):
        self._allowWildCard = allow
        
    def __call__( self, key = None ):
        if key is not None:
            items = set( [ i for i in self._items if i.startswith(key+"#") ] )
        else:
            items = set( self._items )
        for c in self._children:
            try:
                for i in c.list( key ):
                    items.add( i )
            except AttributeError,err:
                msg.warning( err )
                pass
        items = [ i for i in items ]
        items.sort()
        return items

    def list( self, key = None ):
        return self( key )

    def dict( self ):
        props = self.getProperties()
        for c in self._children:
            props.update( c.dict() )
        for k in props.keys():
            props[k] = sorted(props[k])
        return props
    
    def __repr__( self ):
        return '<%s/%s at %s>' % (self.__class__.__name__,self._name,
                                hex(id(self)))

    def __str__( self, indent = 0,
                 headerLastIndentUnit=Configurable.indentUnit ):
        global msg  # to print some info depending on output level
        indentStr = indent * Configurable.indentUnit
        # print header
        title = "%s/%s" % ( self.__class__.__name__, self._name )

        # print line to easily see start-of-configurable
        if indent > 0:
            headerIndent = (indent-1)*Configurable.indentUnit \
                           + headerLastIndentUnit
        else:
            headerIndent = ''

        rep = Configurable._printHeader( headerIndent, title )
        rep += os.linesep

        # print own properties
        props = self.getProperties()
        if not props:
            #rep += indentStr + '|-<no output-item-list>' + os.linesep
            rep += indentStr + '| ' + os.linesep
        else:
            # get property name with
            nameWidth = 0
            for p in props.keys():
                nameWidth=max(nameWidth,len(p))
            for p, v in props.items():
                # start with indent and property name
                prefix = indentStr + '|-%-*s' % (nameWidth,p)
                # add memory address for debugging (not for defaults)
                if msg.isEnabledFor( logging.DEBUG ):
                    address = ' @%11s' % hex(id(v))
                    prefix += address
                # add value and default
                strVal = repr(v)
                # add the value
                line = prefix + ' = ' + strVal
                # add the line to the total string
                rep += line + os.linesep
        # print configurables + their properties, or loop over sequence
        for cfg in self.allChildren():
            rep += cfg.__str__( indent + 1, '|=' ) + os.linesep

        # print line to easily see end-of-configurable. Note: No linesep!
        rep += Configurable._printFooter( indentStr, title )
        return rep

    def name( self ):
        return self._name
    
    def children( self ):
        return self._children
    
    def allChildren( self ):
        children = []
        for c in self._children:
            children.append( c )
            children += c.allChildren()
           
        return children
   
    pass # CfgItemList

### ---------------------------

class CfgKeyStore( object ):
    """Python class to manage different stores of keys, their content and
    origin ('StreamESD', 'StreamAOD', ...)
    """
    instances = weakref.WeakValueDictionary() # all key store instances
    __slots__ = {
        '_name'  : 'KeyStore',
        '_items' : None,
        '__weakref__' : None, # required for dealing with weak references
        'Labels' : [ "inputBackNav","inputFile","transient",
                     "streamRDO","streamESD","streamAOD","streamTAG",
                     "metaData",
                     "detector"
                     ]
        }

    def __new__(cls, *p, **kw):

        if not kw.has_key('name'):
            if len(p) == 0: kw['name'] = cls.__slots__['_name']
            else:           kw['name'] = p[0]

        # already created...
        if kw['name'] in cls.instances.keys():
            return cls.instances[kw['name']]

        # create instance
        obj = object.__new__(cls)
        object.__init__(obj)

        kw['_name'] = kw['name']
        del kw['name']
        
        for k in cls.__slots__.keys():
            if k in ('__weakref__', 'Labels'): continue
            if k in kw.keys(): v = kw[k]
            else: v = cls.__slots__[k]
            setattr(obj, k, v)
            
        # update repository of instances
        cls.instances[kw['_name']] = obj
        
        return obj

    def __init__(self, *p, **kwargs):

        # call base class init
        super( CfgKeyStore, self ).__init__() # *p, **kwargs )
        self._items = CfgItemList( self._name+"ItemList" )

        if len(self._items) == 0:
            self._items += CfgItemList( self._name+"_transient" )
            for label in [ l for l in self.keys() if l != 'transient' ]:
                item = CfgItemList( '%s_%s' % (self.name(), label) )
                if label.count('stream') > 0: self['transient'] += item
                else:                         self._items       += item
        #self += self._items
        
        return 

    ## pickle support
    def __getstate__( self ):
        d = { '_name'  : self._name,
              '_items' : self._items,
              'Labels' : CfgKeyStore.__slots__['Labels'],
              }
        return d

    def __setstate__( self, dct ):
        for k,v in dct.items():
            setattr(self, k, v)
        return
    
    def __getnewargs__( self ):
        return (self.name,)
    ##

    def __getitem__( self, k ):
        if not k in CfgKeyStore.__slots__['Labels']:
            raise KeyError, "key [%s] is not an allowed one: %s" % \
                  ( k, CfgKeyStore.__slots__['Labels'] )
        root = self._items
        if k.count('stream') > 0:
            root = getattr( root, self._name+'_transient' )
        try:
            return getattr( root, self._name+"_"+k )
        except AttributeError,err:
            raise KeyError, str(err)
    
    def __setitem__( self, k, v ):
        if not k in CfgKeyStore.__slots__['Labels']:
            raise KeyError, "key [%s] is not an allowed one: %s" % \
                  ( k, CfgKeyStore.__slots__['Labels'] )
        root = self._items
        if k.count('stream') > 0:
            root = getattr( root, self._name+'_transient' )
        return setattr( root, self._name+"_"+k, v )

    def __getattr__( self, k ):
        try:
            if k in CfgKeyStore.__slots__['Labels']:
                return self.__getitem__(k)
            if k in CfgKeyStore.__slots__:
                return super(CfgKeyStore, self).__getattribute__(k)
            return super(CfgKeyStore, self).__getattribute__(k)
        except KeyError,err:
            raise AttributeError,str(err)

    def __setattr__( self, k, v ):
        if k in CfgKeyStore.__slots__['Labels']:
            return self.__setitem__(k,v)
        return super(CfgKeyStore, self).__setattr__(k, v)

    def name( self ):
        return self._name

    def keys(self):
        return CfgKeyStore.__slots__['Labels']

    def values(self):
        # could use a generator instead...
##         for l in self.keys():
##             yield self[l]
        return [ self[l] for l in self.keys() ]

    def items(self):
        return zip( self.keys(), self.values() )
    
    def clear(self, label = None):
        if label != None:
            self[label].clear()
        else:
            for c in self._items.children():
                c.clear()
        return

    def __repr__( self ):
        return '<%s/%s at %s>' % (self.__class__.__name__,self._name,
                                hex(id(self)))

    def __str__( self, indent = 0,
                 headerLastIndentUnit=Configurable.indentUnit ):
        
        global msg  # to print some info depending on output level
        indentStr = indent * Configurable.indentUnit
        # print header
        title = "%s/%s" % ( self.__class__.__name__, self._name )

        # print line to easily see start-of-configurable
        if indent > 0:
            headerIndent = (indent-1)*Configurable.indentUnit \
                           + headerLastIndentUnit
        else:
            headerIndent = ''

        rep = Configurable._printHeader( headerIndent, title )
        rep += os.linesep

        rep += self._items.__str__( indent + 1, '|=' ) + os.linesep
        # print line to easily see end-of-configurable. Note: No linesep!
        rep += Configurable._printFooter( indentStr, title )
        return rep

    def write( self, fileName, label ):
        item = self[label]
        try:
            from pprint import pprint
            from datetime import datetime
            from cStringIO import StringIO
            buf = StringIO()
            pprint( item.dict(), stream = buf )
            
            out = open( os.path.expanduser(os.path.expandvars(fileName)), 'w' )
            out.writelines( os.linesep.join( [
                "## autogenerated at [%s]" % str(datetime.now()),
                "",
                "## method filling an item-list with some data",
                "def _fill( item ):",
                "    item.add(","%s    )" % buf.getvalue(),
                "    return",
                "",
                "## EOF ##",
                ] ) )
            out.close()
        except Exception, e:
            print "Py:Error :",e
            raise e
        return

    def read( self, fileName, label ):
        item = self[label]
        import imp, inspect
        from AthenaCommon.Include import FindFile,optionsPath
        osp = os.path
        name = osp.expanduser( osp.expandvars( fileName ) )
        name = FindFile( name, optionsPath, os.R_OK )
        if not name:
            name = FindFile( osp.basename(fileName), optionsPath, os.R_OK )
        uri = open( name, 'r' )
        mod = imp.load_source( uri.name[:-3], uri.name, uri )
        uri.close()

        fill = inspect.getmembers(
            mod, lambda o: inspect.isfunction(o) and (o.__name__ == '_fill')
            )[0][1]
        fill( item )
        return
    
    pass # class CfgKeyStore

def loadKeyStoreFromPoolFile(keyStore, pool_file,
                             label='transient',
                             items_type='eventdata'):
    """
    Helper function to create a CfgKeyStore of name `keyStore` from the
    content of the POOL file `pool_file` (a python string).
    The content of the POOL file will be stored into the label `label`
    """
    if not isinstance(keyStore, str):
        raise ValueError("argument 0 (`keyStore`) should be string")
    if not isinstance(pool_file, str):
        raise ValueError("argument 1 (`pool_file`) should be string")
    if not isinstance(label, str):
        raise ValueError("argument 2 (`label`) should be string")
    
    from PyUtils.PoolFile import extract_items
    item_list = ['%s#%s'%(name,key)
                 for name,key in extract_items(pool_file,
                                               verbose=False,
                                               items_type=items_type)
                 ]
    ks = CfgKeyStore(keyStore)
    ks_label = getattr(ks, label)
    
    ks_label.add (item_list)

    return ks
    
def keystore_diff (ref, chk, labels=None, ofile=None):
    """
    Helper function to print the 'diff' of two KeyStores `ref` and `chk` into
    the file-like object `ofile` (default value: sys.stdout)
     `ref` and `chk` can either be :
       - @c KeyStore.CfgKeyStore instances or,
       - strings being the names of the _already created_
         @c KeyStore.CfgKeyStore instances
     @param `labels` the list of labels (default: CfgKeyStore.Labels) to compare
            between the two CfgKeyStores
    """
    if isinstance (ref, str):
        if not CfgKeyStore.instances.has_key (ref):
            raise ValueError \
                  ('invalid `ref` argument (non existing instance name [%s])'%\
                   ref)
        ref = CfgKeyStore.instances [ref]

    if isinstance (chk, str):
        if not CfgKeyStore.instances.has_key (chk):
            raise ValueError \
                  ('invalid `chk` argument (non existing instance name [%s])'%\
                   chk)
        chk = CfgKeyStore.instances [chk]

    if labels is None:
        labels = CfgKeyStore.__slots__['Labels']
    elif isinstance (labels, str):
        labels = [l.strip() for l in labels.split() if l.strip() != '']
        
    if ofile is None:
        import sys
        ofile = sys.stdout
    
    diff = []
    if ref is chk or ref == chk:
        return ""
    
    for label in labels:
        ref_content = sorted (ref[label].list())
        chk_content = sorted (chk[label].list())
        if len(ref_content) != len(chk_content):
            diff.append ("- len(ref[%s]) == %i" % (label,len(ref_content)))
            diff.append ("+ len(chk[%s]) == %i" % (label,len(chk_content)))
        for r in ref_content:
            if not r in chk_content:
                diff.append ("- ref[%s] : %s" % (label, r))
        for c in chk_content:
            if not c in ref_content:
                diff.append ("+ chk[%s] : %s" % (label, c))
    if len(diff) == 0:
        return ""

    diff.append ("")
    map (ofile.writelines, (d for d in os.linesep.join(diff)))
    ofile.flush()
    diff = "".join(diff)
    return diff
