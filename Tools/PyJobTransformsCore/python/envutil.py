# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import os, sys, re, glob
from PyJobTransformsCore import fileutil
#from exceptions import EnvironmentError

from future import standard_library
standard_library.install_aliases()
import subprocess

__doc__ = """Environment variables utilities"""

#some names of environment variables to avoid typing mistakes
PYTHONPATH = 'PYTHONPATH'
LD_LIBRARY_PATH = 'LD_LIBRARY_PATH'
PATH = 'PATH'

# possible wildcards used in unix shell filename completion (and in module glob)
filenameWildCards = r'\?|\*|\[.*\]'
filenameWildCardsCompiled = re.compile(filenameWildCards)

# split paths according to different separators
defaultPathSeps = os.pathsep + '|,'

# regular expression with allowed endings for shared library names
_libraryNameRE = re.compile(r'\.so(\.[0-9]+)*$')


def has_wildcards(filename):
    """Return boolean indicating if the filename contains any unix shell filename wildcards"""
    if filenameWildCardsCompiled.search(filename):
        return True
    else:
        return False
    


def _get_sys_path_extras():
    extras = []
    #start with clear python (the same a the current one!)
    cmd = 'unset PYTHONPATH ; %s -c "import sys ; print sys.path"' % (sys.executable)
    status,output = subprocess.getstatusoutput(cmd)
    if status: raise EnvironmentError('Can not determine python sys.path extras')
    extras = eval( output )
    # remove first entry
    if len(extras) <= 1:
        return []
    else:
        return extras[1:]
    

def update_syspath():
    pythonPath = os.environ.get(PYTHONPATH)
    if pythonPath: 
        sys.path = sys.path[0:1] + pythonPath.split(os.pathsep) + _get_sys_path_extras()
        

def update_env(newenv):
    """Update os.environ, and sys.path from PYTHONPATH"""
    # update os.environ
    os.environ.update(newenv)
    # PYTHONPATH needs to be propagated to sys.path
    if newenv.get(PYTHONPATH): update_syspath()
    

def append_path_env( env_name, what, sep=os.pathsep ):
    """Append <what> to pathlike environment variable <env_name>.
    (A list of paths separated by <sep>).
    If environment variable does not yet exist, it will
    be created and its value will be set that <what>"""
    var = os.environ.get(env_name)
    if not var:
        newvar = what
    else:
        newvar = var + sep + what

    update_env( { env_name : newvar } )


def prepend_path_env( env_name, what, sep=os.pathsep ):
    var = os.environ.get(env_name)
    if not var:
        newvar = what
    else:
        newvar = what + sep + var

    update_env( { env_name : newvar } )


def append_path_env_if( env_name, what, sep=os.pathsep ):
    """As append_path_env, except that <what> is only appended if it
    is not already present in the path."""
    var = os.environ.get(env_name)
    newvar = None
    if not var:
        newvar = what
    elif what not in var.split(sep):
        newvar = var + sep + what

    if newvar: update_env( { env_name : newvar } )
    

def prepend_path_env_if( env_name, what, sep=os.pathsep ):
    var = os.environ.get(env_name)
    newvar = None
    if not var:
        newvar = what
    elif what not in var.split(sep):
        newvar = what + sep + var

    if newvar: update_env( { env_name : newvar } )
    

def append_path_env_force( env_name, what, sep=os.pathsep ):
    """As append_path_env, except that <what> will be removed
    first if it is already present (not at the end)"""
    var = os.environ.get(env_name)
    newvar = None
    if not var:
        newvar = what
    else:
        varList = var.split(sep)
        try:
            idx = varList.index(what)
        except ValueError:
            newvar = var + sep + what
        else:
            if idx != len(varList) - 1:
                varList.remove(what)
                varList.append(what)
                newvar = sep.join(varList)

    if newvar: update_env( { env_name : newvar } )


def prepend_path_env_force( env_name, what, sep=os.pathsep ):
    """As prepend_path_env, except that <what> will be removed
    first if it is already present (not at the beginning)"""
    var = os.environ.get(env_name)
    newvar = None
    if not var:
        newvar = what
    else:
        varList = var.split(sep)
        try:
            idx = varList.index(what)
        except ValueError:
            newvar = what + sep + var
        else:
            if idx != 0:
                varList.remove(what)
                varList.insert(0,what)
                newvar = sep.join(varList)

    if newvar: update_env( { env_name : newvar } )



def remove_path_env( env_name, whatRE, sep=os.pathsep ):
    """Remove all entries that match the regular expression <whatRE>
    from the path contained in environment variable named <env_name>."""
    var = os.environ.get(env_name)
    if not var: return
    varList = var.split(sep)
    pat = re.compile(whatRE)
    newList = []
#    print ("Starting with %s" % var)
    for item in varList:
        if not pat.search(item):
            newList.append(item)
        else:
            pass
#            print ("Removing %s" % item)
           
    newvar = sep.join(newList)
#    print ("Left with: %s" % newvar)
    update_env( { env_name : newvar } )



def insert_path_env( env_name, index, what, sep=os.pathsep ):
    var = os.environ.get(env_name)
    if not var:
        newvar = what
    else:
        valList = var.split(sep)
        n = len(valList)
        if index >= n:
            newvar = var + sep + what 
        elif index <= -n:
            newvar = what + sep + var
        else:
            valList.insert(index,what)
            newvar = sep.join(valList)

    update_env( { env_name : newvar } )

            

def replace_path_env( env_name, findRE, replaceRE, sep=os.pathsep ):
    """Replace all occurences matching the regular expression <findRE>
    with <replaceRE> in the path entries given in the environment variable <env_name>.
    <replaceRE> may contain regular expression references like \1 to refer to groups
    defined in <findRE>. (see module re and function sub() therein).
    It returns the number of replacements done."""
    var = os.environ.get(env_name)
    if not var: return
    oldList = var.split(sep)
    pat = re.compile(findRE)
    newList = []
    nReplaced = 0
    for item in oldList:
        newItem = pat.sub(replaceRE,item)
        if newItem != item: nReplaced += 1
        newList.append(newItem)

    newvar = sep.join(newList)
    update_env( { env_name : newvar } )
    return nReplaced


def find_path_env( env_name, whatRE, sep=os.pathsep ):
    """Find all entries in the path in environment variable <env_name> that match
    the regular expression given in <whatRE>"""
    found = [ ]
    var = os.environ.get(env_name)
    if var:
        varList = var.split(sep)
        pat = re.compile(whatRE)
        for item in varList:
            if pat.search(item):
                found.append(item)

    return found
    


def find_file_split( filename, dirlist = [ os.getcwd() ], access = os.R_OK, depth = 0 ):
    """Search for file <filename> with access rights <access> (see os.access()) in directory list <dirlist>.
    Search into directory tree of each directory in <dirlist> up to depth <depth>. The default directory
    list is a list containing only the current working directory.
    No wildcards are allowed in <filename>.
    <depth> = 0 : only search in directories given in list.
    <depth> > 0 : descend deeper into the directory tree up to max <depth> levels.
    <depth> < 0 : ascend upwards into the directory tree up to max -<depth> levels.
    It returns 2-tuple (dir,file) where:
       file=<filename> if no wildcards, or the actual (local) match to <filename> if wildcarded.
       dir=the directory where <file> was found (from <dirlist>, or from a subdir if depth > 0)
    If no file is found, it returns None."""
    if not dirlist: return None
    for dir in dirlist:
        dir = os.path.abspath( os.path.expandvars( os.path.expanduser(dir) ) )
        if not os.path.isdir(dir): continue
        fullfile = os.path.join( dir, filename )
        #        print ("Checking file %s..." % fullfile)
        if fileutil.access( fullfile, access ):
            return (dir,filename)

    if depth == 0:
        # not found at all
        return None
    elif depth > 0:
        # not found at this level. Go one level down in directory structure
        for dir in dirlist:
            dir = os.path.abspath( os.path.expandvars( os.path.expanduser(dir) ) )
            if not os.path.isdir(dir): continue
            subdirlist = []
            for d in fileutil.listdir(dir):
                fulldir = os.path.join(dir,d)
                if os.path.isdir(fulldir):
                    subdirlist.append( fulldir )
            if subdirlist:
                found = find_file_split( filename, subdirlist, access, depth - 1 )
                if found: return found
    elif depth < 0:
        # not found at this level. Go one level up in directory structure
        for dir in dirlist:
            dir = os.path.abspath( os.path.expandvars( os.path.expanduser(dir) ) )
            if not os.path.isdir(dir): continue
            updir = os.path.dirname(dir)
            if updir != dir:
                found = find_file_split( filename, [ updir ], access, depth + 1 )
                if found: return found
                
        
    # not found at all
    return None


def find_file( filename, dirlist = [ os.getcwd() ], access = os.R_OK, depth = 0 ):
    """Search for file <filename> with access rights <access> (see os.access()) in directory list <dirlist>,
    Search into directory tree of each directory up to depth <depth>. The default directory list is
    a list containing only the current working directory.
    <depth> = 0 : only search in directories given in list.
    <depth> > 0 : descend deeper into the directory tree up to max <depth> levels.
    <depth> < 0 : ascend upwards into the directory tree up to max -<depth> levels."""
    found = find_file_split( filename, dirlist, access, depth )
    return found and os.path.join( found[0], found[1] )


def find_file_updir( filename, dir = os.getcwd(), access = os.R_OK ):
    """Find a file in directory <dir> or its higher level dirs."""
    curdir = os.path.abspath( dir )
    fullfile = os.path.join( curdir, filename )
    if fileutil.access( fullfile, access ): return fullfile
    #go up in directory tree
    updir = os.path.dirname(curdir)
    while updir != curdir:
        curdir = updir
        fullfile = os.path.join( curdir, filename )
        if fileutil.access( fullfile, access ): return fullfile
        updir = os.path.dirname(curdir)

    return None



def find_files_split( filename, dirlist, access, depth ):
    """Search for all (regular) files that match <filename> with access rights <access> (see os.access())
    in directory list <dirlist>.
    Search is done into subdirectories each directory up to depth <depth>.
    The default value for <dirlist> is the current working directory.
    If the same file (without the directory name) is found in more than one places, only the first match is kept.
    <filename> : can contain wildcards as used on the unix command line.
    <depth> = 0 : only search in directories given in list.
    <depth> < 0 : treated as = 0
    <depth> > 0 : descend deeper into the directory tree up to max <depth> levels.
    It returns a list of 2-tuples (dir,file) where
       file=<filename> if no wildcards, or the actual (local) match to <filename> if wildcarded.
       dir=the directory where <file> was found (from <dirlist>, or from a subdir if depth > 0)
    If none is found, an empty list is returned."""
#    if dirlist is None:
#        return []
    if depth < 0: depth = 0
    # to speed up search, do a single file search if the filename does not contain wildcards
    if not has_wildcards(filename):
        singleFile = find_file_split( filename, dirlist, access, depth )
        if singleFile:
            return [ singleFile ]
        else:
            return []
    # filename has wildcards. Find all (first) files that match.
    filenameList = [] # the list of files to return
    dirnameList = [] # keep track of files already found
    for dir in dirlist:
        dir = os.path.abspath( os.path.expandvars( os.path.expanduser(dir) ) )
        # protect against non-existing entries
        if not os.path.isdir(dir): continue
        olddir = os.getcwd()
        os.chdir(dir)
##        print ("Checking files %s..." % fullfile)
        filelist = glob.glob(filename)
        for f in filelist:
##            print ("Trying %s..." % f)
            if not os.path.isfile(f) or not fileutil.access(f, access): continue
            if f not in filenameList:
##                print ("==> Adding %s to list from %s" % (f,dir))
                dirnameList.append(dir)
                filenameList.append(f)
            else:
                pass
##                print ("==> Already have %s in list" % (base))
        os.chdir(olddir)
    if depth > 0:
        # Go one level down in directory structure
        for dir in dirlist:
            dir = os.path.abspath( os.path.expandvars( os.path.expanduser(dir) ) )
            # protect against non-existing entries
            if not os.path.isdir(dir): continue
            subdirlist = []
            for d in fileutil.listdir(dir):
                fulldir = os.path.join(dir,d)
                if os.path.isdir(fulldir):
                    subdirlist.append( fulldir )
            if subdirlist:
                filelist = find_files_split( filename, subdirlist, access, depth - 1 )
                for f in filelist:
                    if not f[1] in filenameList:
                        dirnameList.append(f[0])
                        filenameList.append(f[1])
    return map( lambda arg1,arg2 : (arg1,arg2) , dirnameList, filenameList )


def find_files( filename, dirlist, access, depth ):
    """Search for all (regular) files that match <filename> with access rights <access> (see os.access()) in directory list <dirlist>.
    Search is done into subdirectories each directory up to depth <depth>.
    The default value for <dirlist> is the current working directory.
    If the same file (without the directory name) is found in more than one places, only the first match is kept.
    <filename> : can contain wildcards as used on the unix command line.
    <depth> = 0 : only search in directories given in list.
    <depth> < 0 : treated as = 0
    <depth> > 0 : descend deeper into the directory tree up to max <depth> levels.
    It returns a list of filenames with full pathnames. If none is found, an empty list is returned."""
    return list (map( lambda arg : os.path.join( arg[0], arg[1] ), find_files_split( filename, dirlist, access, depth ) ))


def find_file_env( filename, env_var_name, access = os.R_OK, sep = defaultPathSeps, depth = 0 ):
    """Search for file <filename> with access rights <access> (see os.access()) in directory list
    given as a <sep> separated list of paths in environment variable <env_var_name>.
    Search into directory tree of each directory up to depth <depth> (0=don't descend at all)."""
    env = os.environ.get( env_var_name )
    envList = [ os.getcwd() ]
    if not env:
        return envList
    envList.extend( re.split( sep, env ) )
    return find_file( filename, envList, access, depth )


def find_files_env( filename, env_var_name, access = os.R_OK, sep = defaultPathSeps, depth = 0 ):
    """Search for all files that match <filename> with access rights <access> (see os.access()) in directory list
    given as a <sep> (a regular expression) separated list of paths in environment variable <env_var_name>.
    <filename> can contain wildcards as used on the unix command line.
    Search into directory tree of each directory up to depth <depth> (0=don't descend at all)."""
    env = os.environ.get( env_var_name )
    envList = [ os.getcwd() ]
    if not env:
        return envList
    envList.extend( re.split( sep, env ) )
    return find_files( filename, envList, access, depth )


def find_library( lib ):
    """Search for library in LD_LIBRARY_PATH. Return full path of library if found, and None otherwise."""
    global _libraryNameRE
    libfull = None
    if _libraryNameRE.search(lib): # fully specified ending
        libfull = find_file_env( lib, LD_LIBRARY_PATH )
    else:
        libname = lib
        #could contain wildcards
        libsfull = find_files_env( libname, LD_LIBRARY_PATH )
        # filter results for valid shared library ending (basically to get rid of *.cmtref)
        libsfull = [ l for l in libsfull if _libraryNameRE.search(l) ]
        if not libsfull:
            # add generic ending
            libname = lib + '.so*'
            libsfull = find_files_env( libname, LD_LIBRARY_PATH )
            # filter results for valid shared library ending (basically to get rid of *.cmtref)
            libsfull = [ l for l in libsfull if _libraryNameRE.search(l) ]
        if libsfull:
            libfull = libsfull[0] # take the first
                
    if not libfull and not lib.startswith('lib'):
        # try with prefix 'lib'
        libfull = find_library( 'lib' + lib )

    return libfull


def find_libraries( lib ):
    """Search for libraries in LD_LIBRARY_PATH. Return list of full paths of libraries if found.
    <lib> can contain wildcards, in which case all files matching the wildcard will be returned.
    If the same file appears in several paths, the first one found will be taken."""
    # require extension .so (or .so with version numbers)
    global _libraryNameRE
    libsfull = []
    libname = lib
    if _libraryNameRE.search(lib): # fully specified ending
        libsfull = find_files_env( libname, LD_LIBRARY_PATH )
    else:
        libsfull = find_files_env( libname, LD_LIBRARY_PATH )
        # filter results for valid shared library ending (basically to get rid of *.cmtref)
        libsfull = [ l for l in libsfull if _libraryNameRE.search(l) ]
        if not libsfull:
            # add generic ending
            libname = lib + '.so*'
            libsfull = find_files_env( libname, LD_LIBRARY_PATH )
        # filter results for valid shared library ending (basically to get rid of *.cmtref)
        libsfull = [ l for l in libsfull if _libraryNameRE.search(l) ]
        
    # if still not found anything, try with prefix 'lib'
    if not libsfull and not lib.startswith('lib'):
        libsfull = find_libraries( 'lib' + lib )

    return libsfull


# list of possible extensions for python module filenames
_pyext = [ '.so', '.pyo', '.pyc', '.py' ]

def find_python_module_file( modname ):
    """Search for python file (full path) corresponding to python module <mod> in sys.path
    (PYTONPATH + some system paths). <mod> should not contain the filename
    extension (.py etc), and no wildcards. Returns None is module is not found."""
    # add wildcarded extension
    filename = modname.replace('.',os.sep) + '.*'
    # get all python module files
    all = [ f for f in find_files_split( filename, sys.path ) if os.path.splitext(f[1])[1] in _pyext ]
    if not all:
        return None
    else:
        # take the first. When 2 appear in the same directory, choose the one with
        # file extension that appears first in _pyext.
        found = all[0]
        if len(all) > 1:
            foundPrio = _pyext.index( os.path.splitext(found[1])[1] )
            for df in all[1:]:
                d,f = df[0],df[1]
                if d != found[0]: break # done
                prio = _pyext.index( os.path.splitext(f)[1] )
                if prio < foundPrio:
                    found = df
                    foundPrio = prio
        
        return os.path.join(found[0],found[1])


def find_python_modules( mod ):
    """Search for python module(s) in PYTHONPATH + some system paths.
    Returns a list of full paths to python module files. If non are found, returns empty list.
    <mod> can contain wildcards, in which case all files matching the wildcard will be returned.
    If the same file appears in several paths, the first one found will be taken."""
    b,e = os.path.splitext(mod)
    if e in _pyext: # us filename as-is
        # turn python module syntax into filename syntax
        filename = b.replace('.',os.sep) + e
        return find_files( filename, sys.path )
    elif not has_wildcards( mod ):
        m = find_python_module_file( mod.replace(os.sep, '.') )
        if m:
            return [ m ]
        else:
            return []
    else:
        # first expand the wildcards
        filename = mod.replace('.',os.sep) + '.*'
        modsFound = []
        for df in find_files_split( filename, sys.path ):
            f = df[1]
            b,e = os.path.splitext(f)
            if e not in _pyext: continue
            modname = b.replace(os.sep, '.')
            if modname not in modsFound:
                modsFound.append( modname )
        # now find the files for each module
        found = []
        for m in modsFound:
            f = find_python_module_file( m )
            if f: found.append( f )
        return found


def find_python_module( mod ):
    """Search for python module in sys.path (PYTONPATH + some system paths).
    Return full path of python module file if found, and None otherwise."""
    b,e = os.path.splitext(mod)
    if e in _pyext: # Filename with extension, with or without wildcards
        # turn python module syntax into filename syntax
        filename = b.replace('.',os.sep) + e
        return find_file( filename, sys.path )
    elif not has_wildcards( mod ): # module name (no extension), without wildcards
        return find_python_module_file( mod.replace(os.sep, '.') )
    else: # module name (no extension) with wildcards
        # first expand the wildcards
        filename = mod.replace('.',os.sep) + '.*'
        for df in find_files_split( filename, sys.path ):
            f = df[1]
            b,e = os.path.splitext(f)
            if e in _pyext:
                modname = b.replace(os.sep, '.')
                mf = find_python_module_file( modname )
                if mf: return mf

    return None
        

def find_executable( exe ):
    """Find executable file in PATH"""
    return find_file_env( exe, PATH, os.X_OK )


def examine_library(lib):
    """Examine why shared library <lib> can not be loaded.
    It returns the list of libraries that can not be found."""
    full_lib = find_library(lib)
    if not full_lib:
        return [lib]
    # library is found. Check on dependents
    missLibs = []
    lddOut = subprocess.getoutput( 'ldd %s' % (full_lib) )
    notFoundRE = re.compile(r"^\s*(?P<lib>[\w.-]+)\s+.*not found")
    for line in lddOut.split(os.linesep):
        match = notFoundRE.search( line )
        if match:
            misLib = match.group('lib')
            missLibs.append( misLib )

    return missLibs

    
def getenv_from_output(cmd,envRE=None):
    """Return the enviroment in a dictionary as extracted from the output generated by
    shell command <cmd>. Only environment variable names that match the regular expression envRE
    will be included. Recognised syntax: name=value at the beginning of a line (spaces allowed).
    Does *not* change os.environ"""

    status,output = subprocess.getstatusoutput( cmd )
    if status != 0:
        raise EnvironmentError( 'Error executing command: %s. exitstatus=%d  output=\"%s\"' % (cmd,status,output) )
    if not envRE: envRE=r'\S+?'
    newenv = { }
    envPat = re.compile(r'(?:^|;)\s*(%s)\s*=\s*(\S+?)\s*(?:;|$)' % (envRE))
    for line in output.split(os.linesep):
        match = envPat.search(line)
        if match:
            name = match.group(1)
            value = match.group(2)
            if value:
                # remove quotes around strings
                quotes="\"\'"
                if value[0] in quotes and value[-1] in quotes:
                    value = value[1:-1]
                newenv[name] = value

    return newenv


def source_setup(setupshell,options='',envRE=None):
    """Extract (part of) the environment of setup shell script <setupshell> using regular expressions <envRE>
    to select which environment variables to setup. envRE=None (default) means all environment vars.
    The <options> are given as arguments to the setup script. The environment is added to os.environ."""
    if not fileutil.exists(setupshell):
        raise EnvironmentError( 'setup script %s not found' % (setupshell) )

    # make sure that path starts with . (dot) if no absolute path is given
    # This is needed and desired because when sourcing a file, it is looked for
    # in PATH, whereas this is never what people intend (always want local)
    if os.path.isabs(setupshell):
        fullsetup = setupshell
    elif not (setupshell.startswith(os.curdir) or setupshell.startswith(os.pardir)):
        fullsetup = os.path.join(os.curdir,setupshell)
    source_cmd = 'source %s %s' % ( fullsetup, options )
    print (source_cmd)
    newenv = getenv_from_output( source_cmd + '; printenv', envRE )
    update_env(newenv)
    return newenv
