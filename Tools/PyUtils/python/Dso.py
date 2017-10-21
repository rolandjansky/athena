# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @author: Sebastien Binet
## @file : PyUtils/python/Dso.py
## @purpose: a set of classes to model so-called 'rootmap' files

__version__ = "$Revision$"
__author__  = "Sebastien Binet"

__all__ = [
    'DsoDb',
    'gen_typeregistry_dso',
    'load_typeregistry_dso',

    ]

import os
import re

def _libName(lib):
    import platform
    if platform.system() == "Linux":
        if lib[:3]  != "lib": lib = "lib"+lib
        if lib[-3:] != ".so": lib = lib+".so"
    return lib

### data ----------------------------------------------------------------------
_aliases = {
    'ElementLinkInt_p1'           : 'ElementLink_p1<unsigned int>',
    'basic_string<char>'          : 'string',
    'std::basic_string<char>'     : 'string',
    'vector<basic_string<char> >' : 'vector<string>',
    ## FIXME: in waiting for a proper registration of typedefs in genreflex...
    'INavigable4MomentumCollection' : 'DataVector<INavigable4Momentum>',
    'IParticleContainer'            : 'DataVector<IParticle>',
    }
_typedefs = {
    ## FIXME: in waiting for a proper registration of typedefs in genreflex...
    'INavigable4MomentumCollection' : 'DataVector<INavigable4Momentum>',
    'IParticleContainer'            : 'DataVector<IParticle>',
    }

_cpp_builtins = (
    'char',      'unsigned char',      'signed char',
    'signed', 
    'short int',                       'short signed', 'short signed int',
    'short',     'unsigned short',     'signed short',
    'int',       'unsigned int',

    'long int',
    'long signed int',
    'signed long int',

    'long',
    'long signed',  'signed long',
    'unsigned long',
    'unsigned long int',
    'long unsigned int',
    
    'long long',
    'long long int',
    'unsigned long long',
    'longlong',
    
    # no clue from where this one comes from, who's requesting it nor who
    # got the alien naming scheme idea...
    'ulonglong',
    
    'float',
    'double',
    'long double',
    'bool',
    )

_is_stl_sequence = re.compile (r'std::(?P<ContType>.*?)'\
                               r'<(?P<TemplateArg>.*?)'\
                               r',\s*?std::allocator<\2> >')
_is_stl_mapping = re.compile (r'std::map<'\
                              r'(?P<TemplateArg1>.*?),\s*?'\
                              r'(?P<TemplateArg2>.*?)'\
                              r',\s*?std::allocator<\2> >')
    

### functions -----------------------------------------------------------------

### helpers
def _get_native_libname(libname):
    """ return the OS-native name from an OS-indenpendent one """
    import sys
    plat = sys.platform
    if plat.count('linux')>0:
        lib_prefix,lib_suffix = 'lib', '.so'
    elif plat == 'win32':
        lib_prefix,lib_suffix = '', '.dll'
    elif plat == 'darwin':
        lib_prefix,lib_suffix = 'lib','.dylib'
    else:
        raise RuntimeError ("sorry platform [%s] is not (yet?) supported"%plat)
    _sys_libname = libname
    if not _sys_libname.startswith (lib_prefix):
        _sys_libname = ''.join([lib_prefix,_sys_libname])
    if not _sys_libname.endswith (lib_suffix):
        _sys_libname = ''.join([_sys_libname, lib_suffix])
    return _sys_libname

def load_library (libname):
    """
    Helper method to load a library by its natural name, not the OS-native name.
    But if the OS-native name is given, it is safely handled too.
    usage:
     >>> load_library ('AthenaServices')
     >>> load_library ('AthenaServicesDict')
    """
    _sys_libname = _get_native_libname(libname)
    import ctypes
    return ctypes.cdll.LoadLibrary (_sys_libname)

def find_library(libname):
    """
    Helper function to find the (full)path to a library given its natural name.
     @return None on failure
     
    usage:
     >>> find_library('AthenaServices')
     '/afs/cern.ch/.../AtlasCore/[release]/InstallArea/.../libAthenaServices.so
    """
    import os, sys
    import ctypes.util as cu
    ## # ctypes.util.find_library does not return the path
    ## # to the library, just the basename of the so-name...
    ## lib = cu._findLib_ldconfig(libname) or cu._findLib_gcc(libname)
    ## if lib:
    ##     return os.path.abspath(lib)
    _sys_libname = _get_native_libname(libname)
    # FIXME: REALLY not portable...
    if os.name != 'posix':
        raise RuntimeError('sorry OS [%s] is not supported' % os.name)
    
    if 'LD_LIBRARY_PATH' in os.environ:
        for d in os.environ['LD_LIBRARY_PATH'].split(os.pathsep):
            lib = os.path.join(d, _sys_libname)
            if os.path.exists(lib):
                return lib
    return

_dflt_typereg_fname = 'typereg_dso_db.csv'
def gen_typeregistry_dso(oname=_dflt_typereg_fname):
    '''inspect all the accessible reflex types and get their rootmap-naming.
    also associate the clid if available.
    '''
    import CLIDComps.clidGenerator as _c
    cliddb = _c.clidGenerator(db=None)
    del _c

    import PyUtils.path as _p
    oname = _p.path(oname)
    del _p
        
    import PyUtils.Logging as _L
    msg = _L.logging.getLogger('typereg-dso')
    msg.setLevel(_L.logging.INFO)
    #msg.setLevel(_L.logging.VERBOSE)   #MN
    del _L
    
    msg.info("installing registry in [%s]...", oname)

    # FIXME: should use the Cxx one...
    #reg = DsoDb()
    reg = PyDsoDb()
    
    cls_names = reg.db.keys()
    import cppyy
    _load_lib = cppyy.loadDict
    rflx = cppyy.gbl.RootType

    def _load_dict(libname,retry=10):
        msg.debug("::: loading [%s]...", libname)
        try:
            return _load_lib(libname)
        except (Exception,SystemError,), err:
            msg.warning("**error** %s", err)
        return

    # we need to pre-load these guys as HepPDT is missing a linkopts
    # against HepPID. see bug #46551
    hep_pid = _load_lib('libHepPID.so')
    hep_pdt = _load_lib('libHepPDT.so')

    from PyUtils.Decorators import forking
    
    import os
    dict_libs = reduce(set.union, [set(v) for v in reg.db.values()])
    dict_libs = [os.path.basename(l) for l in dict_libs]

    _veto_libs = [
        'libG4EventGraphicsDict.so', # freaking statics !
        ]
    dict_libs = [l for l in dict_libs if l not in _veto_libs]
    
    msg.debug("::: loading dict-libraries...")
    @forking
    def inspect_dict_lib(lib):
        _load_dict(lib)
        try:
            rflx_names = update_db(lib)
            return rflx_names
        except Exception, err:
            msg.warning(err)
        return {}

    msg.debug(":"*80)
    def update_db(libname):
        rflx_names={}
        for i in xrange(rflx.TypeSize()):
            rflx_type = rflx.TypeAt(i)
            rflx_name = rflx_type.Name(7)
            root_name = _to_rootmap_name(rflx_name)
##             # could also retro-fit typedefs, and allow their auto-loading...
##             if rflx_type.IsTypedef():
##                 import ROOT
##                 print "[%s] ::: processing [%s -> %s]..." % (
##                     ROOT.TClass.GetClass(rflx_name).GetSharedLibs(),
##                     rflx_type.Name(6),
##                     rflx_name)
            if not(root_name in reg.db):
##                 print "::ERR::",root_name
                continue
            ##rflx_names[rflx_name] = root_name
            rflx_names[root_name] = rflx_name
        return rflx_names

    rflx_names = {}
    # for lib in dict_libs:
    #   rflx_names.update(inspect_dict_lib(lib))
    msg.warning("::: DSO functionality disabled in ROOT6!")
                     
    msg.debug("::: rflx types: %d %d",len(rflx_names),len(reg.db.keys()))
    msg.info("::: saving informations in [%s]...", oname)
    
    import csv
    db= csv.writer(open(oname,'w'), delimiter=';')
    keys = sorted(rflx_names.keys())
    for k in keys:
        v = rflx_names[k]
        clid = (cliddb.getClidFromName(k) or
                cliddb.getClidFromName(v) or
                cliddb.getClidFromTid(k)  or
                cliddb.getClidFromTid(v))
        if k != v:
            db.writerow([k,v,clid or ''])
        elif clid:
            db.writerow([k,v,clid])

    return rflx_names

def load_typeregistry_dso(iname=None):
    import os
    import PyUtils.path as _p
    if iname is None:
        iname = _p.path(_dflt_typereg_fname)
        if not iname.exists():
            import os
            projects = os.environ.get('CMTPATH','').split(os.pathsep)[:2]
            for project_root in projects:
                n = _p.path(project_root)/"InstallArea"/"share"/iname
                if n.exists():
                    iname = n
                    break
    else:
        iname = _p.path(iname)

    if not iname.exists():
        raise OSError('no such file [%s]'%iname)
    
    import PyUtils.Logging as _L
    msg = _L.logging.getLogger("typereg-dso")
    msg.setLevel(_L.logging.INFO)
    del _L
    msg.info("::: loading typeregistry from [%s]...", iname)
    
    rflx_names = {}
    f = iname.open(mode='r')
    import csv
    db = csv.reader(f, delimiter=';')
    for row in db:
        row = [i.strip() for i in row]
        root_name = row[0]
        rflx_name = row[1]
        rflx_names[root_name] = rflx_name

    del _p, csv
    return rflx_names


import re
def _is_rootcint_dict (libname):
    """helper function to reject rootcint libraries entries from rootmap
    files (which appeared w/ ROOT v21/22)
    It seems all of them (and on all platforms) are named like:
     vector<char>: vector.dll
    """
    if libname == ".dll": # pathological case...
        return False
    pat = re.compile(r'\w*?.dll')
    return not (libname.startswith("lib")) and \
           not (pat.match (libname) is None)

class CxxDsoDb(object):
    """
    The repository of 'rootmap' files (location, content,...)
    """
    def __init__(self):
        import cppyy
        # import root
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()
        self._cxx = ROOT.Ath.DsoDb.instance()

    def _to_py(self, cxx):
        dd = {}
        kk = self._cxx.py_keys_from(cxx)
        vv = self._cxx.py_vals_from(cxx)
        for i in range(kk.size()):
            dd[kk[i]] = list(vv[i])
        return dd

    @property
    def db(self):
        return self._to_py(self._cxx.db())
    
    @property
    def pf(self):
        return self._to_py(self._cxx.pf())
    
    def has_type(self, typename):
        return self._cxx.has_type(typename)

    def load_type(self, typename):
        return self._cxx.load_type(typename)
    
    def capabilities(self, libname):
        return list(self._cxx.capabilities(libname))
    
    def duplicates(self, libname, pedantic=False):
        return self._to_py(self._cxx.duplicates(libname, pedantic))

    def dict_duplicates(self, pedantic=False):
        return self._to_py(self._cxx.dict_duplicates(pedantic))

    dictDuplicates = dict_duplicates
    
    def pf_duplicates(self, pedantic=False):
        return self._to_py(self._cxx.pf_duplicates(pedantic))

    pfDuplicates = pf_duplicates
    
    def libs(self, detailed=False):
        return list(self._cxx.libs(detailed))

    def content(self, pedantic):
        return self._to_py(self._cxx.content(pedantic))

    @property
    def dso_files(self):
        return list(self._cxx.dso_files())

    @property
    def dsoFiles(self):
        return self.dso_files

def _to_rootmap_name(typename):
    """
    helper method to massage a typename into something understandable
    by the rootmap files
    """
    global _aliases
    typename = typename.replace(', ',',')
    # first the easy case: builtins
    if typename in _cpp_builtins:
        return typename
    # known missing aliases ?
    if typename in _aliases.keys():
        t = _aliases[typename]
        return _to_rootmap_name(t)
    # handle default template arguments of STL sequences
    if _is_stl_sequence.match(typename):
        # rootmap files do not contain the default template arguments
        # for STL containers... consistency, again.
        _m = _is_stl_sequence.match(typename)
        _cont_type = _m.group('ContType')
        _m_type = _m.group('TemplateArg')
        # handle the dreaded 'std::Bla<Foo<d> >
        _m_type = _to_rootmap_name(_m_type.strip())
        if _m_type.endswith('>'):
            _m_type += ' '
        typename = 'std::%s<%s>' % (_m.group('ContType'),
                                    _m_type)
    # need to massage a bit the typename to match ROOT naming convention
    typename = typename.replace('std::basic_string<char> ',
                                'string ')
    typename = typename.replace('std::basic_string<char>',
                                'string')
    typename = typename.replace('std::', '')
    typename = typename.replace('> >', '>->')
    typename = typename.replace(' >', '>')
    typename = typename.replace('>->', '> >')
    return typename

def _to_rflx_name (typename):
    """helper method to massage a typename into something understandable
    by reflex (which doesn't understand the same thing than rootmaps).
    """
    global _aliases,_typedefs
    typename = typename.replace(', ',',')
    # first the easy case: builtins
    if typename in _cpp_builtins:
        return typename
    # known missing typedefs ?
    if typename in _typedefs.keys():
        t = _typedefs[typename]
        return _to_rflx_name(t)
    # handle default template arguments of STL sequences
    if _is_stl_sequence.match(typename):
        # rootmap files do not contain the default template arguments
        # for STL containers... consistency, again.
        _m = _is_stl_sequence.match (typename)
        _m_type = _m.group('TemplateArg')
        # handle the dreaded 'std::Bla<Foo<d> >
        _m_type = _to_rflx_name (_m_type.strip())
        if _m_type.endswith('>'):
            _m_type += ' '
        typename = 'std::%s<%s>' % (_m.group('ContType'), _m_type)
    typename = typename.replace('std::string>',
                                'std::basic_string<char> >')
    typename = typename.replace('std::string',
                                'std::basic_string<char>')
    return typename

class PyDsoDb( object ):
    """
    The repository of 'rootmap' files (location, content,...)
    """
    RootMap = "rootmap"
    DsoMap  = "dsomap"
    PluginNamespace = "__pf__"
    
    def __init__(self, name = "DsoDb"):
        object.__init__(self)
        self.name = name
        self.db = { } # repository of components
        self.pf = { } # repository of known components to the plugin svc

        import PyUtils.Logging as _L
        self.msg = _L.logging.getLogger('DsoDb')

        self.dsoPath = os.environ['LD_LIBRARY_PATH']
        self.__buildRepository()
        return

    def __buildRepository(self):
        msg = self.msg
        self.dsoFiles = set()
        dsoPath = [p for p in self.dsoPath.split( os.pathsep )
                   if not p.startswith(os.environ['ROOTSYS'])]
        for path in dsoPath:
            if not os.path.exists(path): continue
            dir_content = None
            try:
                dir_content = os.listdir(path)
            except Exception:
                # try again...
                try:
                    dir_content = os.listdir(path)
                except Exception,err:
                    msg.warning("caught:\n%s", err)
            if dir_content is None:
                msg.warning("could not run os.listdir on [%s]" % path)
                dir_content = []
            dsoFiles = [ f for f in dir_content
                         if f.endswith(self.RootMap) ]
            for dsoFile in dsoFiles:
                dsoFile = os.path.join( path, dsoFile )
                if os.path.exists(dsoFile):
                    line_nbr = -1
                    self.dsoFiles.add(dsoFile)
                    for line in open(dsoFile, 'r'):
                        line_nbr += 1
                        line = line.strip()
                        if len(line) <= 0 or line[0] == "#":
                            continue
                        line = line.split()
                        # Note that as of LCG-55, rootmaps have the following
                        # format: 'symbol': libDict.so [listOfLinkedLibs.so...]
                        # we are only interested in libDict.so...
                        try:
                            dsoKey, libName = line[0], line[1]
                        except Exception,err:
                            msg.warning(
                                'could not parse %s:%i', dsoFile, line_nbr
                                )
                            msg.warning(
                                '(some) reflex-dicts may fail to be auto-loaded'
                                )
                            msg.warning(err)
                            continue
                        dsoKey = dsoKey\
                                .replace("Library.", "")\
                                .replace( ":", ""  )\
                                .replace( "@", ":" )\
                                .replace( "-", " " )
                        if dsoKey.startswith( self.PluginNamespace ):
                            db = self.pf
                        else:
                            db = self.db
                        if not db.has_key(dsoKey): db[dsoKey] = list()
                        import re
                        if _is_rootcint_dict (libName):
                            #print "## discarding [%s]..." % libName
                            continue
                        libName = os.path.join(path, _libName(libName))
                        db[dsoKey].append(libName)
                        pass # loop over dso-lines
                pass # loop over dsoFiles
            pass # iter over dsoPath
        return

    def __str__(self):
        s = os.linesep.join( [
            "+--- %s ---" % self.name,
            "|nbr of lib components: %i" % len(self.db.keys()),
            "|nbr of pf  components: %i" % len(self.pf.keys()),
            "|nbr of dso files:      %i" % len(self.dsoFiles),
            "|nbr of known libs:     %i" % len(self.libs()),
            "+-------------------------"
            ] )
        
        return s

    def __dups(self, db, pedantic):
        dups = {}
        for k in db.keys():
            if len(db[k]) == 1: continue
            if pedantic:        libs = db[k]
            else:
                baseLibs = set()
                libs = []
                for lib in db[k]:
                    if os.path.basename(lib) not in baseLibs:
                        libs.append(lib)
                        baseLibs.add(os.path.basename(lib))
                        pass
                    pass
            if len(libs) > 1:
                dups[k] = [ lib for lib in libs ]
        return dups

    def duplicates(self, libName, pedantic = False):
        caps = self.capabilities(libName)
        dups = {}
        for dupDb in [ self.dictDuplicates(pedantic),
                       self.pfDuplicates(pedantic) ]:
            for k in dupDb:
                if k in caps:
                    if not dups.has_key(k): dups[k] = []
                    dups[k] += [ lib for lib in dupDb[k]
                                 if not libName in os.path.basename(lib) ]
        dups.keys().sort()
        for k in dups.keys():
            dups[k].sort()
        return dups
    
    def dictDuplicates(self, pedantic = False):
        return self.__dups(self.db, pedantic)

    def pfDuplicates(self, pedantic = False):
        return self.__dups(self.pf, pedantic)

    def capabilities(self, libName):
        libName = _libName(libName)
        caps = set()
        for db in [self.db, self.pf]:
            for k in db.keys():
                if libName in [ os.path.basename(lib) for lib in db[k] ]:
                    caps.add( k )
        caps = [ cap for cap in caps ]
        caps.sort()
        if len(caps) == 0:
            print "::: ERROR: No such library [%s] in dsoDb !!" % libName
            raise ValueError, ""
        return caps

    def libs(self, detailedDump = False):
        if detailedDump: fct = lambda x : x
        else:            fct = os.path.basename
        libs = set()
        for db in [self.pf, self.db]:
            for k in db.keys():
                for lib in db[k]:
                    libs.add(fct(lib))
        libs = [ lib for lib in libs ]
        libs.sort()
        return libs
    
    def content(self, pedantic):
        d = {}
        for db in [self.pf, self.db]:
            for k in db.keys():
                if pedantic: libs = db[k]
                else:
                    baseLibs = set()
                    libs = []
                    for lib in db[k]:
                        if os.path.basename(lib) not in baseLibs:
                            libs.append(lib)
                            baseLibs.add(os.path.basename(lib))
                            pass
                        pass
                d[k] = [ lib for lib in libs ]
        return d

    def _to_rootmap_name(self, typename):
        """
        helper method to massage a typename into something understandable
        by the rootmap files
        """
        return _to_rootmap_name(typename)

    def _to_rflx_name (self, typename):
        """helper method to massage a typename into something understandable
        by reflex (which doesn't understand the same thing than rootmaps).
        """
        return _to_rflx_name(typename)

DsoDb = CxxDsoDb
#DsoDb = PyDsoDb
