#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Script to convert CMT projects/packages to CMake Gaudi-based configuration.
"""
import os
import sys
import glob
import shutil
import re
import time
import logging
import shelve
import json
import operator
from collections import defaultdict, namedtuple
import subprocess

def useStmt(version, doImports, isPublic):
    Use = namedtuple('Use', 'version doImports isPublic')
    return Use(version, doImports, isPublic)


def isBracketed(s, start, end):
    return s.startswith(start) and s.endswith(end)


def replaceQuote(s, prefix=None):
    start = 0 if not prefix else len(prefix)
    return s[start:].replace('"', '').replace("'", '')


def stripQuote(s, prefix=None):
    start = 0 if not prefix else len(prefix)
    return s[start:].strip('"').strip("'")


# environment
def fixSetValue(s):
    """Convert environment variable values from CMT to CMake."""
    # escape '$' if not done already
    s = re.sub(r'(?<!\\)\$', '\\\\\\\\\$', s)
    # replace parenthesis with curly braces
    s = re.sub(r'\$\(([^()]*)\)', r'${\1}', s)
    # replace variables like Package_root with PACKAGEROOT
    v = re.compile(r'\$\{(\w*)_root\}')
    m = v.search(s)
    while m:
        s = s[:m.start()] + ('${%sROOT}' % m.group(1).upper()) + s[m.end():]
        m = v.search(s)
    if ' ' in s:
        s = '"'+s+'"'
    if not s:
        s = '\\"\\"'
    return s


def makeParser(patterns=None):
    from pyparsing import Word, QuotedString, Keyword, Literal
    from pyparsing import SkipTo, StringEnd, ZeroOrMore, Optional
    from pyparsing import Combine, alphas, alphanums, printables
    dblQuotedString = QuotedString(quoteChar='"', escChar='\\', unquoteResults=False)
    sglQuotedString = QuotedString(quoteChar="'", escChar='\\', unquoteResults=False)
    value = dblQuotedString | sglQuotedString | Word(printables)

    tag_name = Word(alphas + "_", alphanums + "_-")
    tag_expression = Combine(tag_name + ZeroOrMore('&' + tag_name))
    values = value + ZeroOrMore(tag_expression + value)

    identifier = Word(alphas + "_-", alphanums + "_-")
    macro_name = Combine('$(' + identifier + ')')
    path = Word(alphanums + "_-/")
    variable = Combine(identifier + ZeroOrMore(' ') + '=' + ZeroOrMore(' ') + value)

    constituent_option = (Keyword('-no_share')
                          | Keyword('-no_static')
                          | Keyword('-prototypes')
                          | Keyword('-no_prototypes')
                          | Keyword('-check')
                          | Keyword('-target_tag')
                          | Combine('-group=' + value)
                          | Combine('-suffix=' + value)
                          | Combine('-import=' + value)
                          | variable
                          | Keyword('-OS9')
                          | Keyword('-windows'))
    source = (Word(alphanums + "-_*./$(){}\"")
              | Combine('-s=' + value)
              | Combine('-k=' + value)
              | Combine('-x=' + value))

    # statements
    comment = (Literal("#") + SkipTo(StringEnd())).suppress()

    cm_comment = Keyword('cmake_com') + SkipTo(StringEnd())
    author   = Keyword('author') + SkipTo(StringEnd())
    manager  = Keyword('manager') + SkipTo(StringEnd())

    package = Keyword('package') + Word(printables)
    version = Keyword("version") + Word(printables)
    use = Keyword("use") + (identifier + Word(printables) + Optional(path) + Optional(Keyword('-no_auto_import')) + Optional(Keyword("-no_auto_imports")) + Optional(Keyword("--no-auto-imports")) + Optional(Keyword("-no-auto-imports")) | macro_name )
    use_if = Keyword("use_if") + ZeroOrMore(variable)
    use_ifndef = Keyword("use_ifndef") + ZeroOrMore(variable)
    use_unless = Keyword("use_unless") + ZeroOrMore(variable)
    branches = Keyword( "branches" ) + ZeroOrMore( value )

    constituent = ((Keyword('library') | Keyword('application') | Keyword('document'))
                   + identifier + ZeroOrMore(constituent_option | source))
    macro = (Keyword('macro') | Keyword('macro_append') | Keyword('macro_remove')) + identifier + values
    setenv = (Keyword('set') | Keyword('path_append') | Keyword('path_prepend')) + identifier + values
    alias = Keyword('alias') + identifier + values
    private = Keyword('private')
    end_private = Keyword('end_private')
    public = Keyword('public')
    end_public = Keyword('end_public')

    apply_tag = Keyword("apply_tag") + Word(printables)

    apply_pattern = Keyword("apply_pattern") + identifier + ZeroOrMore(variable)
    if patterns:
        direct_patterns = reduce(operator.or_, map(Keyword, set(patterns)))
        # add the implied 'apply_pattern' to the list of tokens
        direct_patterns.addParseAction(lambda toks: toks.insert(0, 'apply_pattern'))
        apply_pattern = apply_pattern | (direct_patterns + ZeroOrMore(variable))

    statement = (cm_comment | author | manager | package | version | use | use_if | use_ifndef | use_unless | private | end_private | public | end_public | constituent | macro | setenv | alias | apply_pattern | apply_tag | branches)

    return Optional(statement) + Optional(comment) + StringEnd()


cache = None
def open_cache():
    global cache
    # record of known subdirs with their libraries
    # {'<subdir>': {'libraries': [...]}}
    # it contains some info about the projects too,
    # under the keys like repr(('<project>', '<version>'))
    try:
        # First we try the environment variable CMT2CMAKECACHE and the directory
        # containing this file...
        result = os.environ.get('CMT2CMAKECACHE',
                                      os.path.join(os.path.dirname(__file__),
                                                   '.cmt2cmake.cache'))
        cache = shelve.open(result)
    except:
        # otherwise use the current directory:
        result = os.path.join( os.getcwd(), '.cmt2cmake.cache' )
        #logging.info("Using cache file %s", result)
        cache = shelve.open(result)
    logging.info('Using cache file %s', result)

    return result

def close_cache(file):
    global cache
    if cache:
        cache.close()
        # An extra step is needed because shelve.open can add a postfix
        # to the file name in some cases. Like on MacOS X it adds an additional
        # ".db" to the file name.
        import glob
        cachefile = glob.glob( file + "*" )
        filename = os.path.basename( cachefile[ 0 ] )
        if os.path.realpath( cachefile[ 0 ] ) != os.path.join( os.getcwd(), filename ):
            shutil.copyfile( cachefile[ 0 ],
                             os.path.join( os.getcwd(), filename ) )
            pass
        cache = None
        pass

config = defaultdict(set)

# mappings
ignored_packages = config['ignored_packages']
data_packages = config['data_packages']


def isIgnoredPackage(package):
    return package in ignored_packages or 'External/' in package


def isExternalPackage(package):
    return package.startswith(('LCG_Interfaces/', 'find_package'))


# List of packages known to actually need Python to build
needing_python = config['needing_python']

# packages that must have the pedantic option disabled
no_pedantic = config['no_pedantic']

ignore_env = config['ignore_env']

overrides_dir = os.path.join(os.path.dirname(__file__), 'overrides')


def loadConfig(config_file):
    """Merge the content of the JSON file with
    the configuration dictionary."""
    global config
    logging.info("****loadConfig config_file:%s" % config_file)
    if os.path.exists(config_file):
        data = json.load(open(config_file))
        for k in data:
            if k not in config:
                config[k] = set()
            config[k].update(map(str, data[k]))


def extName(n):
    """Mapping between the name of the LCG_Interface name
    and the Find*.cmake name (if non-trivial).
    """
    mapping = {
        'Reflex': 'ROOT',
        'Python': 'PythonLibs',
        'neurobayes_expert': 'NeuroBayesExpert',
        'mysql': 'MySQL',
        'oracle': 'Oracle',
        'sqlite': 'SQLite',
        'lfc': 'LFC',
        'fftw': 'FFTW',
        'uuid': 'UUID',
        'fastjet': 'FastJet',
        'lapack': 'LAPACK',
        'bz2lib': 'BZip2',
    }
    return mapping.get(n, n)


def isPackage(path):
    return os.path.isfile(os.path.join(path, 'cmt/requirements'))


def isPatchProject(name):
    return name not in [
        'DetCommon',
        'AtlasCore',
        'AtlasConditions',
        'AtlasEvent',
        'AtlasReconstruction',
        'AtlasSimulation',
        'AtlasTrigger',
        'AtlasAnalysis',
        'AtlasOffline',
        'ManaCore'
    ]


# FIXME: this always just returns @name?!
def projectCase(name):
    atlasnames = ['Atlas', 'tdaq', 'dqm', 'DetCommon']
    for an in atlasnames:
        if name.startswith(an):
            return name
    return name


def callStringWithIndent(cmd, arglines):
    """
    Produce a string for a call of a command with indented arguments.

    >>> print callStringWithIndent('example_command', ['arg1', 'arg2', 'arg3'])
    example_command(arg1
                    arg2
                    arg3)
    >>> print callStringWithIndent('example_command', ['', 'arg2', 'arg3'])
    example_command(arg2
                    arg3)
    """
    indent = '\n' + ' ' * (len(cmd) + 2)
    return cmd + '( ' + indent.join(filter(None, arglines)) + ' )'


def writeToFile(filename, data, log=None):
    """Write the generated CMakeLists.txt."""
    if log and os.path.exists(filename):
        log.info('overwriting %s', filename)

    with open(filename, 'w') as f:
        f.write(data)

# Names of the packages for which a CMakeLists.txt file was generated
generatedPackages = set()


class Package(object):
    def __init__(self, path, project=None):
        self.path = os.path.realpath(path)
        if not isPackage(self.path):
            raise ValueError("%s is not a package" % self.path)

        self.name = os.path.basename(self.path)
        self.requirements = os.path.join(self.path, 'cmt/requirements')
        self.fullname = self.name
        self.project = project
        if self.project != None:
            self.fullname = os.path.relpath(self.path, self.project.path)

        # prepare attributes filled during parsing of requirements
        self.uses = {}
        self.version = None
        self.libraries = []
        self.applications = []
        self.documents = []
        self.find_packages = {}
        self.conditional_targets = defaultdict(list)
        self.library_overrides = {}
        self.macros = {}
        self.sets = {}
        self.paths = {}
        self.aliases = {}

        # These are patterns that can appear only once per package.
        # The corresponding dictionary will contain the arguments
        # passed to the pattern.
        self.singleton_patterns = set([
            'QMTest',
            'install_more_includes',
            'god_headers',
            'god_dictionary',
            'PyQtResource',
            'PyQtUIC'
        ])
        self.install_more_includes = {}
        self.QMTest = False
        self.god_headers = {}
        self.god_dictionary = {}
        self.PyQtResource = {}
        self.PyQtUIC = {}

        self.install_docs = []
        self.install_java = []
        self.install_joboptions = []
        self.install_python_modules = []
        self.install_runtime = []
        self.install_scripts = []
        self.install_xmls = []
        self.install_generic = []
        self.install_data = []
        self.add_definitions = []
        self.add_libraries = {}
        self.ignore_imports = defaultdict(list)
        self.add_cmake = []
        self.generate_componentslist = []

        # These are patterns that can be repeated in the requirements.
        # The corresponding data members will contain the list of
        # dictionaries corresponding to the various calls.
        self.multi_patterns = set([
            'reflex_dictionary',
            'component_library',
            'linker_library',
            'copy_relax_rootmap',
            'root_dictionary',
            'tpcnv_library',
            'poolcnv',
            'sercnv',
            'install_docs',
            'install_java',
            'install_joboptions',
            'install_python_modules',
            'install_runtime',
            'install_scripts',
            'install_xmls',
            'install_job_transforms',
            'install_data'
        ])
        self.fixup_library_defaults = set([
            'component_library',
            'linked_library',
            'installed_library',
            'tpcnv_library',
            'dual_use_library',
            'poolcnv',
            'sercnv',
            'detcommon_shared_library',
            'detcommon_shared_generic_library',
            'static_library',
            'shared_library',
            'component_library_no_genCLIDDB',
            'pool_plugin_library'
        ])
        self.fixup_pattern_names = {
            'installed_library': 'linker_library',
            'qt4based_library': 'linker_library',
            'default_installed_library': 'linker_library',
            'named_installed_library': 'linker_library',
            'detcommon_shared_library': 'linker_library',
            'detcommon_shared_generic_library': 'linker_library',
            'pool_plugin_library': 'linker_library',
            'named_component_library': 'component_library',
            'component_library_no_genCLIDDB': 'component_library',
            'named_dual_use_library': 'dual_use_library',
            'linked_library': 'none',
            'lcgdict': 'reflex_dictionary',
            'named_linked_library': 'none',
            'shared_library': 'none',
            'static_library': 'none',
            'declare_joboptions': 'install_joboptions',
            'declare_runtime': 'install_runtime',
            'declare_runtime_extras': 'install_runtime',
            'declare_scripts': 'install_scripts',
            'declare_java': 'install_java',
            'declare_xmls': 'install_xmls',
            'declare_python_modules': 'install_python_modules',
            'declare_docs': 'install_docs',
            'declare_job_transforms' : 'install_job_transforms',
            'declare_calib': 'install_data'
        }

        self.fixup_add_packages = {}

        # The following packages appearing in "use" statements
        # need to be remapped. Packages that are remapped to
        # "LCG_Interfaces/XXX" or "find_package   XXX" are treated
        # as externals. Note that the length of "find_package   "
        # is important and must be the same as "LCG_Interfaces/"
        self.fixup_use_names = {
            'Control/CLIDSvc':          ['Control/CLIDSvc', 'Control/SGTools'],
            'Database/AthenaPOOL/RDBAccessSvc': ['Database/AthenaPOOL/RDBAccessSvc', 'LCG_Interfaces/Boost', 'LCG_Interfaces/CORAL'],
            'DetectorDescription/GeoPrimitives': ['DetectorDescription/GeoPrimitives', 'find_package   Eigen'],
            'Event/EventPrimitives':    ['Event/EventPrimitives'],
            'External/APEGlue':         ['find_package   APE', 'find_package   yampl'],
            'External/AtlasAIDA':       ['LCG_Interfaces/AIDA'],
            'External/AtlasBaurMC':     ['find_package   BaurMC', 'find_package   Lhapdf', 'find_package   Pythia6'],
            'External/AtlasBlas':       ['find_package   BLAS'],
            'External/AtlasBoost':      ['LCG_Interfaces/Boost'],
            'External/AtlasBzip2':      ['find_package   Bzip2'],
            'External/AtlasCLHEP':      ['find_package   CLHEP'],
            'External/AtlasCoinInventor': ['find_package   Coin3D'],
            'External/AtlasCOOL':       ['LCG_Interfaces/COOL', 'LCG_Interfaces/ROOT Core'],
            'External/AtlasCORAL':      ['LCG_Interfaces/CORAL'],
            'External/AtlasCppUnit':    ['LCG_Interfaces/CppUnit'],
            'External/AtlasCython':     ['find_package   Cython'],
            'External/AtlasDCACHE':     ['LCG_Interfaces/dcache_client'],
            'External/AtlasDPM':        ['LCG_Interfaces/dpm'],
            'External/AtlasDSFMT':      ['find_package   dSFMT'],
            'External/AtlasEigen':      ['find_package   Eigen'],
            'External/AtlasExpat':      ['find_package   EXPAT'],
            'External/AtlasFastJet':    ['find_package   FastJet'],
            'External/AtlasFastJetContrib': ['find_package   FastJetContrib'],
            'External/AtlasFFTW':       ['find_package   FFTW'],
            'External/AtlasGdb':        ['find_package   gdb'],
            'External/AtlasGPerfTools': ['find_package   gperftools'],
            'External/AtlasGSL':        ['LCG_Interfaces/GSL'],
            'External/AtlasHdf5':       ['find_package   HDF5'],
            'External/AtlasHepMC':      ['find_package   HepMC'],
            'External/AtlasHepMCAnalysis': ['find_package   HepMCAnalysis'],                          'External/AtlasLapack':     ['find_package   LAPACK'],
            'External/AtlasLibUnwind':  ['find_package   libunwind'],
            'External/AtlasMagField':   ['find_package   MagField'],
            'External/AtlasmT2':        ['find_package   mT2'],
            'External/AtlasOracle':     ['LCG_Interfaces/oracle', 'LCG_Interfaces/pytools'],
            'External/AtlasPOOL':       ['Database/APR/POOLCore', 'Database/APR/CollectionBase', 'Database/APR/CollectionUtilities', 'Database/APR/StorageSvc', 'Database/APR/PersistencySvc'],
            'External/AtlasPython':     ['LCG_Interfaces/Python'],
            'External/AtlasPyROOT':     ['LCG_Interfaces/ROOT Core PyROOT'],
            'External/AtlasQt4':        ['find_package   Qt4 QtCore QtOpenGL QtGui'],
            'External/AtlasReflex':     ['LCG_Interfaces/ROOT Core'],
            'External/AtlasRELAX':      ['LCG_Interfaces/RELAX'],
            'External/AtlasROOT':       ['LCG_Interfaces/ROOT Core'],
            'External/AtlasShift':      ['LCG_Interfaces/CASTOR'],
            'External/AtlasShowerDeconstruction':['find_package   ShowerDeconstruction'],
            'External/AtlasSoQt':       ['find_package   SoQt', 'find_package   Coin3D', 'find_package   Qt4 QtCore QtOpenGL QtGui'],
            'External/AtlasSQLite':     ['LCG_Interfaces/SQLite'],
            'External/AtlasTBB':        ['find_package   TBB'],
            'External/AtlasValgrind':   ['find_package   valgrind'],
            'External/AtlasXercesC':    ['LCG_Interfaces/XercesC'],
            'External/DataCollection':  ['find_package   tdaq-common'],
            'External/Epos':            ['find_package   Crmc Epos'],
            'External/GaudiInterface':  ['GaudiKernel'],
            'External/DQM_Core':        ['find_package   dqm-common', 'find_package   tdaq-common'],
            'External/EvtGen':          ['find_package   EvtGen'],
            'External/Geant4':          ['find_package   Geant4', 'find_package   CLHEP', 'find_package   XercesC'],
            'External/Herwig':          ['find_package   Herwig'],
            'External/Herwigpp':        ['find_package   Herwigpp'],
            'External/Hijing':          ['find_package   Hijing', 'find_package   ROOT minicern'],
            'External/Hydjet':          ['find_package   Hydjet', 'find_package   Pyquen', 'find_package   Pythia6', 'find_package   ROOT minicern'],
            'External/MySQL':           ['LCG_Interfaces/MySQL'],
            'External/JavaSDK':         ['find_package   JavaSDK'],
            'External/Jimmy':           ['find_package   Jimmy'],
            'External/Lhapdf':          ['find_package   Lhapdf'],
            'External/MCTester':        ['find_package   MCTester'],
            'External/MCUtils':         ['find_package   MCUtils' ],
            'External/MadGraph':        ['find_package   MadGraph'],
            'External/HEPUtils':        ['find_package   HEPUtils'],
            'External/Photos':          ['find_package   Photos'],
            'External/Photospp':        ['find_package   Photospp'],
            'External/Pyquen':          ['find_package   Pyquen'],
            'External/Pythia':          ['find_package   Pythia6'],
            'External/Pythia6':         ['find_package   Pythia6'],
            'External/Pythia8':         ['find_package   Pythia8'],
            'External/Rivet':           ['find_package   Rivet'],
            'External/Starlight':       ['find_package   Starlight'],
            'External/Sherpa':          ['find_package   Sherpa'],
            'External/Tauola':          ['find_package   Tauola'],
            'External/Tauolapp':        ['find_package   Tauolapp'],
            'External/ThePEG':          ['find_package   ThePEG'],
            'External/YODA':            ['find_package   YODA'],
            'External/yampl':           ['find_package   yampl'],
            'External/QGSJet':          ['find_package   Crmc QgsjetII04'],
            'MuonSpectrometer/Amcdsimrec/AmdcAth': ['MuonSpectrometer/Amcdsimrec/AmdcAth', 'MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry'],
            'graphics/VP1/VP1Qt':       ['find_package   Qt4 QtCore QtOpenGL QtGui'],
            'LCG_Interfaces/COOL':      ['LCG_Interfaces/COOL', 'LCG_Interfaces/ROOT Core'],
            'LCG_Interfaces/ROOT':      ['LCG_Interfaces/ROOT Core Tree pthread RIO '],
            'HLT/HLTExternal/HLTtdaq':  ['find_package   tdaq', 'find_package   tdaq-common'],
            'HLT/HLTExternal/HLTtdaqcommon': ['find_package   tdaq-common'],
            'TDAQCPolicy/TDAQCPolicyExt': ['find_package   tdaq-common'],
            'CTPfragment':              ['find_package   tdaq-common CTPfragment'],
            'PartPropSvc':              [],
            'ForwardDetectors/ForwardTransportSvc': ['ForwardDetectors/ForwardTransportSvc', 'find_package   Geant4'],
        }

        self.fixup_install_defaults = {
            'install_docs': 'files="-s=doc"',
            'install_java': 'files="-s=java *.java *.jar"',
            'install_joboptions': 'files="-s=share"',
            'install_python_modules': 'files="-s=python"',
            'install_runtime': 'files="-s=share"',
            'install_scripts': 'files="-s=share"',
            'install_xmls': 'files="-s=data *.dtd *.xml"',
            'install_data': 'files="-s=data"'
        }

        self.unit_test = [] # Declarations of the unit tests
        self.unit_tests = [] # Names of the applications that are unit tests
        self.reflex_dictionary = []
        self.component_library = []
        self.linker_library = []
        self.tpcnv_library = []
        self.poolcnv = []
        self.sercnv = []
        self.copy_relax_rootmap = []
        self.root_dictionary = []
        self.generated_sources = {}
        self.l1common_files = {}
        self.reflex_dictionaries = {}
        self.component_libraries = []
        self.linker_libraries = []
        self.tpcnv_libraries = []
        self.poolcnv_libraries = []
        self.sercnv_libraries = []
        self.root_dictionaries = []
        self.qt4based_libraries = []
        self.include_directories = []
        self.cmake_comments = []
        self.applied_tags = []
        self.authors = []
        self.libraries_not_linked = [] # Libraries not to be linked to automatically

        self.log = logging.getLogger('Package({0})'.format(self.name))
        self.CMTParser = makeParser(self.singleton_patterns | self.multi_patterns)
        logging.debug("package %s", self.name)
        try:
            self._parseRequirements()
        except:
            self.log.error('Processing {0}'.format(self.requirements))
            raise

        # update the known subdirs
        # list of linker libraries provided by the package
        libs = list(self.linker_libraries + self.tpcnv_libraries)
        for l in self.libraries_not_linked:
            if l in libs:
                libs.remove( l )
                pass
            pass
        # true if it's a headers-only package
        incs = self.install_more_includes \
                and not (self.linker_libraries or self.tpcnv_libraries)
        cache[self.name] = {'libraries': libs, 'includes': incs}

    def _getHeaders(self):
        # the headers can be installed via "PUBLIC_HEADERS" or by hand
        headers = []
        if os.path.isdir(os.path.join(self.path, self.name)):
            headers.append(self.name)
        if self.install_more_includes:
            for ds in self.install_more_includes.values():
                for d in ds:
                    if os.path.isdir(os.path.join(self.path, d)):
                        headers.append(d)
        return headers

    def _getSubdirs(self):
        public_subdirs = []
        private_subdirs = []
        for usePackage in sorted(self.uses):
            if not isExternalPackage(usePackage) \
            and not isIgnoredPackage(usePackage) \
            and not usePackage in data_packages:
                isPublic = self.uses[usePackage].isPublic
                if isPublic:
                    public_subdirs.append(usePackage)
                else:
                    private_subdirs.append(usePackage)

        return (public_subdirs, private_subdirs)

    def getGlobalImports(self):
        # libraries
        global_imports = []
        for usePackage in self.uses:
            uu = usePackage.split()
            if isExternalPackage(usePackage):
                if self.uses[usePackage].doImports:
                    name = extName(uu[0][15:])
                    if name not in global_imports:
                        global_imports.append(name)

            if usePackage in self.fixup_add_packages:
                names = self.fixup_add_packages[usePackage]
                for name in names:
                    if name not in global_imports:
                        global_imports.append(name)
        return global_imports

    def _logLinksPackagesWarning(self, links_packages):
        sdset = set([os.path.basename(s) for s in self.subdirs])
        not_included = set(links_packages).difference(self.find_packages, sdset)
        if (len(not_included) > 1) \
        or ((len(not_included) == 1) and (not '' in not_included)):
            txt = ', '.join(sorted(not_included))
            self.log.warning('imports without use: \"%s\"', txt)

    def _findExternals_boost(self, clist):
        clist.extend(['filesystem', 'thread', 'system'])
        for c in range(len(clist)):
            if clist[c].startswith('boost_'):
                clist[c] = clist[c].replace('boost_', '')

    def _findExternals_tdaq(self, clist):
        # Libraries provided by tdaq:
        tdaq_libraries = [
                'AccessManager',
                'BackTracer',
                'BeamSpotCoolReader',
                'BeamSpotDataQuality',
                'BeamSpotIS2Cool',
                'BeamSpotISWriter',
                'BeamSpotParameters',
                'BeamSpotRCCommand',
                'CircDataOut',
                'CircDataSender',
                'CircServer',
                'CommonRequests',
                'ConfigInfoObject',
                'CoolUtils',
                'CoolUtilsCommander',
                'CoolUtilsTest',
                'CorboTriggerIn',
                'CorboTriggerInInterruptNotimeout',
                'CorboTriggerInInterruptTimeout',
                'DFDataSource',
                'DFDebug',
                'DFDescriptorTrigger',
                'DFDummyBackend',
                'DFExceptions',
                'DFFileBackend',
                'DFThreads',
                'DFTriggerIn',
                'DataDrivenTriggerIn',
                'DataProcessor',
                'DataSender',
                'DefaultBusyTimeoutAction',
                'DummyDataProcessor',
                'DummyDataSender',
                'DummyDataSource',
                'DummyServer',
                'EMonDataOut',
                'EmulatedDataOut',
                'EmulatedDescriptorModule',
                'emon',
                'ohroot'
                ]

        # Check for components amongst the link flags:
        if '%s_linkopts' % self.name in self.macros:
            libs = self.macros[ '%s_linkopts' % self.name ]
            libs = libs.replace('-l', '').strip()
            for lib in libs.split():
                if lib in tdaq_libraries:
                    clist.extend(lib)

        # Check for components amongst the shared library flags:
        if '%s_shlibflags' % self.name in self.macros:
            libs = self.macros['%s_shlibflags' % self.name]
            libs = libs.replace('-l', '')
            for lib in libs.split():
                if lib in tdaq_libraries:
                    clist.append(lib)

    def _findExternals_cool(self, clist):
        clist.extend( [ 'CoolKernel' ] )
        if 'NEEDS_COOL_FACTORY' in self.applied_tags:
            clist.extend( [ 'CoolApplication' ] )
            self.applied_tags.remove( 'NEEDS_COOL_FACTORY' )
            pass
        return

    def _findExternals_coral(self, clist):
        clist.extend(['CoralBase', 'CoralKernel', 'RelationalAccess'])

    def _findExternals_relax(self, clist):
        if self.copy_relax_rootmap:
            clist.extend([d['dict'] for d in self.copy_relax_rootmap if 'dict' in d])

    def _findExternals_root(self, clist):
        clist.extend(['Core', 'Tree', 'MathCore', 'Hist', 'RIO', 'pthread'])
        if not self.applied_tags:
            return

        tagmap = {
            'ROOTRFIOLibs': ['RFIO'],
            'ROOTMathLibs': ['MathMore', 'Minuit', 'Minuit2', 'Matrix',
                                'Physics', 'HistPainter', 'Hist', 'Rint'],
            'ROOTGenVectorLibs': ['GenVector'],
            'ROOTGraphicsLibs': ['Graf', 'Graf3d', 'Gpad', 'Html',
                                    'Postscript', 'Gui', 'GX11TTF', 'GX11'],
            'ROOTTableLibs': ['Table'],
            'ROOTRooFitLibs': ['RooFitCore', 'RooFit'],
            'ROOTTMVALibs': ['TMVA'],
            'ROOTNetLibs': ['Net'],
            'ROOTEGLibs': ['EG']
        }

        for key, values in tagmap.items():
            if key in self.applied_tags:
                clist.extend(values)
                self.applied_tags.remove(key)

    def _findExternals(self):
        #  externals (excluding specials)
        #  - Python needs to be treated in a special way
        for usePackage in sorted(self.uses):
            if not isExternalPackage(usePackage):
                continue

            nn = extName(usePackage[15:]).split()
            usePackage = nn[0]
            clist = [] if not nn else nn[1:]

            # get custom link options
            linkopts = self.macros.get(usePackage + '_linkopts', '')

            patt = r'(?:\$\(%s_linkopts_([^)]*)\))|(?:-l(\w*))' % usePackage
            for m in re.finditer(patt, linkopts):
                clist.append(m.group(1) or m.group(2))

            # FIXME: find a general way to treat the special cases
            funcname = '_findExternals_' + usePackage.lower()
            func = getattr(self, funcname, lambda x: None)
            func(clist)

            components = []
            for c in clist:
                if c not in components:
                    components.append(c)

            try:
                packages = self.find_packages[usePackage]
            except KeyError:
                self.find_packages[usePackage] = components
            else:
                for c in components:
                    if c not in packages:
                        self.find_packages[usePackage].append(c)


    def _generate_header(self):
        data = []
        data.append('#' * 80)
        data.append('# Package: {0}'.format(self.name))
        data.append('#' * 80)
        data.append('')
        return data

    def _generate_authors(self):
        data = []
        if not self.authors:
            data.append('# Author: <abandoned>')
        else:
            for a in self.authors:
                data.append('# Author: {0}'.format(a))

        data.append('#' * 80)
        data.append('')
        return data

    def _generate_subdirs(self):
        data = []
        data.append('# Declare the package name:')
        data.append('atlas_subdir( {0} )'.format(self.name))
        data.append('')

        public_subdirs, private_subdirs = self._getSubdirs()
        self.subdirs  = public_subdirs + private_subdirs
        self.inc_dirs = []
        if self.subdirs:
            # check if we are missing info for a subdir
            missing_subdirs = set([os.path.basename(s) for s in self.subdirs]) - set(cache)
            if missing_subdirs:
                msd = ' '.join(sorted(missing_subdirs))
                m = 'Missing info cache for subdirs {0}'.format(msd)
                self.log.warning(m)

            data.append('# Declare the package\'s dependencies:')
            arglines = []
            if public_subdirs:
                arglines.extend(['PUBLIC'] + public_subdirs)
            if private_subdirs:
                arglines.extend(['PRIVATE'] + private_subdirs)
            if not arglines:
                raise 'Internal logic error detected!'

            d = callStringWithIndent('atlas_depends_on_subdirs', arglines)
            data.append(d)
            data.append('')

            # consider header-only subdirs
            #  for each required subdir that comes with only headers,
            # add its location to the call to 'include_directories'
            inc_only = lambda s: cache.get(os.path.basename(s), {}).get('includes')
            self.inc_dirs = filter(inc_only, self.subdirs)

        return data

    def _generate_externals(self):
        self._findExternals()
        data = []
        # this second loops avoid double entries do
        # to converging results of extName()
        if self.find_packages:
            data.append('# External dependencies:')

        for n in sorted(self.find_packages):
            args = [n]
            components = self.find_packages[n]
            if components:
                if n == 'RELAX':
                    args.append('REQUIRED')
                args.append('COMPONENTS')
                args.extend(components)

            data.append('find_package( %s )' % ' '.join(args))

        if self.find_packages:
            data.append('')

        return data

    def _generate_no_pedantic(self):
        data = []
        if self.name in no_pedantic:
            d = ('string(REPLACE "-pedantic" "" '
                 'CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")\n')
            data.append(d)
        return data

    def _generate_include_dirs(self):
        data = []

        for name in ['Qt4', 'Coin3D', 'SoQt']:
            if name in self.find_packages:
                if '/usr/X11R6/include' not in self.include_directories:
                    self.include_directories.append('/usr/X11R6/include')
                cmpts = self.libraries + self.applications
                for tgt, sources, group, imports in cmpts:
                    if tgt not in self.add_libraries:
                        self.add_libraries[tgt] = []
                    if 'GL' not in self.add_libraries[tgt]:
                        self.add_libraries[tgt].append('GL')
                    if name != 'Coin3D':
                        if tgt not in self.add_libraries:
                            self.add_libraries[tgt] = []

        if self.include_directories:
            data.append(callStringWithIndent('include_directories',
                                             self.include_directories))
            data.append('')

        return data

    def _generate_definitions(self):
        data = []
        if self.add_definitions:
            d = callStringWithIndent("add_definitions", self.add_definitions)
            data.append(d)
            data.append('')
        return data

    def _generate_add_cmake(self):
        data = []
        if self.add_cmake:
            data.extend(self.add_cmake)
            data.append('')
        return data

    def _generate_comments(self):
        data = []
        for comments in self.cmake_comments:
            d = '# this line failed automatic conversion in cmt2cmake :\n# '
            d += comments
            d += '\n' 
            data.append(d)
        return data


    def _generate_applied_tags(self):
        data = []
        for tag in self.applied_tags:
            if tag == "notAsNeeded":
                data.append( '# Remove the --as-needed linker flags:' )
                data.append( 'atlas_disable_as_needed()\n' )
                continue
            if tag == "no_rootmap":
                continue
            d = '# tag '
            d += tag 
            d += ' was not recognized in automatic conversion in cmt2cmake\n' 
            data.append(d)
        return data

    def _generate_f90(self):
        # Special handling for FORTRAN sources
        data = []
        f90_sources = []
        components = self.libraries + self.applications
        for name, sources, group, imports in components:
            for src in sources:
                if src.endswith(('.F90', '.FF90')):
                    f90_sources.append(src)
            if 'CLHEP' in imports:
                dependCLHEP = True
        if f90_sources:
            cmd = 'set_source_files_properties'
            args = []
            args.append('PROPERTIES')
            args.append('LANGUAGE "Fortran"')
            args.append('HEADER_FILE_ONLY FALSE')
            args.append('COMPILE_FLAGS ""')
            f90data = callStringWithIndent(cmd, f90_sources + args)
            data.append(f90data)
            data.append('') # empty line
        return data

    def _generate_component_generation(self):
        # Componentlist generation:
        data = []
        if self.generate_componentslist:
            data.append('# Component list generation:')

        for comp in set(self.generate_componentslist):
            data.append('atlas_generate_componentslist( {0} )'.format(comp))

        return data

    def _generate_installs(self):
        # installation
        data = []
        installs = []

        # If the headers are not installed yet:
        if self.headers:
            if not (self.linker_libraries or self.tpcnv_libraries):
                txt = ' '.join(self.headers)
                installs.append('atlas_install_headers( %s )' % txt)

        for item in self.install_docs:
            installs.append('atlas_install_docs( %s )' % ' '.join(item))
        for item in self.install_python_modules:
            installs.append('atlas_install_python_modules( %s )' % ' '.join(item))
        for item in self.install_joboptions:
            installs.append('atlas_install_joboptions( %s )' % ' '.join(item))

        if self.install_java:
            print '    install_java:%s' % self.install_java
        for item in self.install_java:
            installs.append('athena_install_java(%s)' % (' '.join(item)))

        for item in self.install_runtime:
            installs.append('atlas_install_runtime( %s )' % ' '.join(item))

        for item in self.install_scripts:
            installs.append('atlas_install_scripts( %s )' % ' '.join(item))
        for item in self.install_xmls:
            installs.append('atlas_install_xmls( %s )' % ' '.join(item))
        for item in self.install_data:
            installs.append('atlas_install_data( %s )' % ' '.join(item))
            pass

        for item in self.install_generic:
            files, destination = item
            files = ' '.join(files)
            lines = [files,
                     'DESTINATION ' + destination,
                     'EXECUTABLE']
            output = callStringWithIndent('atlas_install_generic', lines)
            installs.append(output)

        if installs:
            data.append('# Install files from the package:')
            data.extend(installs)
            data.append('')
        return data

    def _generate_aliases(self):
        data = []
        if self.aliases:
            data.append('# Aliases:')
            for name, alias in self.aliases.iteritems():
                fixed_alias = [a.replace( "\"", "\\\"" ) for a in alias]
                data.append("atlas_add_alias( %s \"%s\" )" % (name, ' '.join(fixed_alias)))
            data.append('')
        return data

    def _get_reflex_dicts(self):
        rflx_dicts = []
        for d in self.reflex_dictionary:
            imports = [i.strip('"').replace('-import=', '')
                       for i in d.get('imports', '').strip().split()]
            rflx_dicts.append((d['dictionary'] + 'Dict',
                               d['headerfiles'] + d['selectionfile'],
                               None,
                               imports))
        return rflx_dicts

    def _get_root_dicts(self):
        root_dicts = []
        for d in self.root_dictionary:
            paths = []
            for ep in d['include_paths']:
                if ep not in paths:
                    paths.append(ep)
            root_dicts.append((d['headers_lib'],
                               ['ROOT_HEADERS ' + ' '.join(d['root_headers'])],
                               d['extra_includes'],
                               paths))
        return root_dicts

    def _getSubdirImports(self):
        sdi = []
        for s in self.subdirs:
            if self.uses[s].doImports:
                sdi.append(os.path.basename(s))
        return sdi

    def _addReflexDictArgs(self, name, args):
        if self.reflex_dictionaries[name]:
            args.append('OPTIONS ' + self.reflex_dictionaries[name])
            pass

        if "no_rootmap" in self.applied_tags:
            args.append( "NO_ROOTMAP_MERGE" )
            pass

        dmap = {
            'extralibfiles': 'EXTRA_FILES ',
            'navigables': 'NAVIGABLES ',
            'dataLinks': 'DATA_LINKS ',
            'elementLinks': 'ELEMENT_LINKS ',
            'elementLinkVectors': 'ELEMENT_LINK_VECTORS '
        } 

        for d in self.reflex_dictionary:
            if d['dictionary'] == name:
                for key, prefix in dmap.items():
                    value = d[key]
                    if value:
                        value = ' '.join(value)
                        args.append(prefix + value)
                break

    def _addPoolcnvArgs(self, name, args):
        # Add the PoolCnv specific parameters:
        # Find this component library:
        for lib in self.poolcnv + self.sercnv:
            # Check if this is the right library:
            if lib['library'] != name:
                continue

            # Add the FILES arguments:
            args.append('FILES %s' % ' '.join(lib['files']))

            # Add the TYPES_WITH_NAMESPACE arguments:
            if 'typesWithNamespace' in lib:
                args.append('TYPES_WITH_NAMESPACE %s' % lib['typesWithNamespace'])
            # Add the MULT_CHAN_TYPES arguments:
            if 'multChanTypes' in lib:
                args.append('MULT_CHAN_TYPES %s' % lib['multChanTypes'])
            # Add the CNV_PFX argument:
            if 'cnv_pfx' in lib:
                args.append('CNV_PFX %s' % lib['cnv_pfx'])

    def _ignoreImports(self, imps, name):
        imports = []
        for imp in imps:
            if imp not in self.ignore_imports[name]:
                imports.append(imp)
        return imports

    def _addMacros(self, links):
        # for e.g. -lz proper way would be to do a findlib,
        # add comment to CMakeLists.txt
        data = []
        for m in self.macros:
            if m.endswith('_linkopts'):
#                d = '# <package>_linkopts added this library: '
#                d += str(self.macros)
#                d += ' consider a FindXYZ.cmake'
#                data.append(d)

                # cmt2cmake.py cannot resolve libraries declared
                # via env or cmt variables containing a dollar ...
                if not '$' in self.macros[m]:
                    nlibs = self.macros[m].replace('-l', ' ').strip()
                    ignoreLibs = ('XMLParser',
                                  'TMVA',
                                  'lcg_CoralBase',
                                  'lcg_RelationalAccess')
                    for nlib in nlibs.split():
                        # Skip libraries that are picked up by
                        # find_package statements already:
                        if m.startswith(self.name) and nlib not in ignoreLibs:
                            links.append(nlib)
        return data

    def _splitPublicPrivate(self, items, suffix):
        private = []
        public = []
        matched = []
        for l in items:
            if not l.startswith('$'):
                for package in self.uses:
                    basep = os.path.basename(package)
                    if basep == l:
                        if self.uses[package].isPublic:
                            public.append(l)
                        else:
                            private.append(l)
                        matched.append(l)
            else:
                name = l[2:-1]  # remove brackets
                name = name.replace(suffix, '')
                name = 'ROOT Core' if name == 'ROOT' else name
                for package in self.uses:
                    p = package.replace('find_package', '').strip()
                    p = p.replace('LCG_Interfaces/', '')
                    p = p.lower()
                    if p == name.lower():
                        if self.uses[package].isPublic:
                            public.append(l)
                        else:
                            private.append(l)
                        matched.append(l)
    
        # did not find these in the self.uses, declare public
        unmatched = [item for item in items if item not in matched]
        public.extend(unmatched)
        return (public, private)

    def _generate_components(self):
        data = []
        local_links = []  # keep track of linker libraries found so far
        self.headers = self._getHeaders()

        subdir_imports = self._getSubdirImports()
        applications_names = set([a[0] for a in self.applications])
        # Unit tests are not regular applications:
        for ut in self.unit_tests:
            applications_names.remove( ut )
            pass

        rflx_dicts = self._get_reflex_dicts()
        root_dicts = self._get_root_dicts()
        components = root_dicts + self.libraries + rflx_dicts + self.applications
        if components:
            data.append('# Component(s) in the package:')
            pass

        # Note: a god_dictionary, a reflex_dictionary
        # or an application is like a module
        for name, sources, group, imps in components:
            imports = imps
            if name in self.ignore_imports:
                imports = self._ignoreImports(imps, name)

            nameDict  = name + 'Dict'
            isGODDict = False
            isRflxDict = False
            isComp = False
            isTpCnv = False
            isPoolCnv = False
            isSerCnv = False
            isApp = False
            isLinker = False
            isCint = False
            isUnitTest = False

            if name == '--GOD--':
                isGODDict = True
                name = ''  # no library name for GOD dictionaries
            elif name.endswith('Dict') and name[:-4] in self.reflex_dictionaries:
                isRflxDict = True
                name = name[:-4]
            elif name in self.linker_libraries:
                isLinker = True
                if nameDict in self.generated_sources:
                    sources += self.generated_sources[nameDict]
            elif name in self.component_libraries:
                isComp = True
            elif name in self.tpcnv_libraries:
                isTpCnv = True
            elif name in self.poolcnv_libraries:
                isPoolCnv = True
            elif name in self.sercnv_libraries:
                isSerCnv = True
            elif name in applications_names:
                isApp = True
            elif name.endswith('Cint') and name in self.root_dictionaries:
                isCint = True
                name = name[:-4]
            elif name in self.unit_tests:
                isUnitTest = True
            else:
                self.log.warning('module %s of unknown type', name)
                pass

            # prepare the bits of the command: cmd, name, sources, args
            if isComp:
                cmd = 'atlas_add_component'
            elif isTpCnv:
                cmd = 'atlas_add_tpcnv_library'
            elif isPoolCnv:
                cmd = 'atlas_add_poolcnv_library'
            elif isSerCnv:
                cmd = 'atlas_add_sercnv_library'
            elif isGODDict:
                cmd = 'god_build_dictionary'
            elif isRflxDict:
                cmd = 'atlas_add_dictionary'
            elif isApp:
                cmd = 'atlas_add_executable'
            elif isCint:
                cmd = 'atlas_add_root_dictionary'
            elif isUnitTest:
                cmd = "atlas_add_test"
            else: # i.e. isLinker (a fallback)
                isLinker = True
                cmd = 'atlas_add_library'

            if not sources and not isSerCnv:
                self.log.warning('Missing sources for target %s', name)

            args = []
            if isLinker or isTpCnv:
                if self.headers:
                    args.append('PUBLIC_HEADERS ' + ' '.join(self.headers))
                else:
                    args.append('NO_PUBLIC_HEADERS')

            if isPoolCnv or isSerCnv:
                self._addPoolcnvArgs(name, args)

            # # collection of link libraries.
            # Externals and subdirs are treated differently:
            # - externals: just use the package name
            # - subdirs: find the exported libraries in the global var cache
            # We also have to add the local linker libraries.

            # separate external and subdir explicit imports
            subdirsnames = [os.path.basename(s) for s in self.subdirs]
            subdir_local_imports = [i for i in imports if i in subdirsnames]
            ext_local_imports = [extName(i) for i in imports if i not in subdir_local_imports]

            # prepare the link list with the externals
            global_imports = self.getGlobalImports()
            links_packages = global_imports + ext_local_imports
            for n in self.find_packages:
                if n not in links_packages:
                    links_packages.append(n)
                    pass
                pass

            # Translate these to simple CMake variables:
            includes = []
            links = []
            definitions = []
            for lpkg in links_packages:
                if lpkg == 'Boost':
                    includes += ['${Boost_INCLUDE_DIRS}']
                    links += ['${Boost_LIBRARIES}']
                elif lpkg == 'PythonLibs':
                    includes += ['${PYTHON_INCLUDE_DIRS}']
                    links += ['${PYTHON_LIBRARIES}']
                elif lpkg == 'AIDA':
                    includes += ['${%s_INCLUDE_DIRS}' % lpkg.upper()]
                elif lpkg in ('PythonInterp', ''):
                    continue
                else:
                    links += ['${%s_LIBRARIES}' % lpkg.upper()]
                    includes += ['${%s_INCLUDE_DIRS}' % lpkg.upper()]
                    pass
                if lpkg == 'CLHEP':
                    definitions += [ '${CLHEP_DEFINITIONS}' ]
                    pass
                pass

            if name in self.add_libraries:
                for lib in self.add_libraries[name]:
                    if lib.startswith('-l'):
                        lib = lib[2:]
                    links += [lib] 

            if isComp:
                # Special handling of NOCLIDDB
                for lib in self.component_library:
                    if lib['library'] == name:
                        if not lib['cliddb']:
                            args.append('NOCLIDDB')
                            break

            # Collect the sources:
            srcs = []
            if isUnitTest:
                srcs += [ "SOURCES" ]
                pass
            srcs += [s for s in sources if s != '<none>']

            if isCint:
                if imports:
                    args.append('INCLUDE_PATHS ' + ' '.join(imports))
                if group:
                    args.append('EXTRA_INCLUDES ' + ' '.join(group))
                if links or self.inc_dirs:
                    args.append('EXTERNAL_PACKAGES ' +
                                ' '.join(links_packages + self.inc_dirs))
            else:
                # if links or self.inc_dirs:
                # args.append( 'INCLUDE_DIRS ' + ' '.join( includes + self.inc_dirs ) )
                if links or self.inc_dirs:
                    # external links need the include dirs
                    if cmd != 'atlas_add_library' and \
                            cmd != 'atlas_add_tpcnv_library':
                        argval = ' '.join(includes + self.inc_dirs)
                        args.append('INCLUDE_DIRS ' + argval)
                    else:
                        incs = includes + self.inc_dirs
                        public, private = self._splitPublicPrivate(incs,
                                                                   '_INCLUDE_DIRS')
                        if public:
                            args.append('INCLUDE_DIRS ' + ' '.join(public))
                        if private and len( srcs ):
                            args.append('PRIVATE_INCLUDE_DIRS ' + ' '.join(private))
                            pass
                        pass
                    pass
                if len( definitions ) and ( cmd == 'atlas_add_library' or
                                            cmd == 'atlas_add_tpcnv_library' ):
                    public, private = self._splitPublicPrivate( definitions,
                                                                '_DEFINITIONS' )
                    if public:
                        args.append( 'DEFINITIONS ' + ' '.join( public ) )
                        pass
                    if private and len( srcs ):
                        args.append( 'PRIVATE_DEFINITIONS ' +
                                     ' '.join( private ) )
                        pass
                    pass


            if links_packages:
                self._logLinksPackagesWarning(links_packages)

            # add subdirs...
            for s in subdir_imports + subdir_local_imports:
                if s in cache:
                    cachelibs = cache[s]['libraries']
                    links.extend(cachelibs)
                else:
                    self.log.warning("%s not in cache" % s)

            # ... and local libraries
            links.extend(local_links)

            data.extend(self._addMacros(links))
            
            if not isCint and links:
                # note: in some cases we get quoted library names
                strippedLinks = [l.strip('"') for l in links]
                if cmd != 'atlas_add_library':
                    args.append('LINK_LIBRARIES ' + ' '.join(strippedLinks))
                else:
                    public, private = self._splitPublicPrivate(strippedLinks,
                                                               '_LIBRARIES')
                    if public:
                        pl = ' '.join(public)
                        args.append('LINK_LIBRARIES ' + pl)
                    if private and len( srcs ):
                        pl = ' '.join(private)
                        args.append('PRIVATE_LINK_LIBRARIES ' + pl)

            if isRflxDict:
                self._addReflexDictArgs(name, args)

            if ( isLinker or isTpCnv ) and not \
                    ( name in self.libraries_not_linked ):
                local_links.append( name )
                pass

            # FIXME: very very special case :(
            if name == 'garbage' and self.name == 'FileStager':
                d = ('# only for the applications\n'
                     'find_package(Boost COMPONENTS program_options)\n')
                data.append(d)

            # Extra parameters for unit tests:
            if isUnitTest:
                for d in self.unit_test:
                    if "%s_test" % os.path.basename( stripQuote( d[ "unit_test" ] ) ) == name and \
                            "extrapatterns" in d.keys():
                        args.append( "EXTRA_PATTERNS \"%s\"" %
                                     self.resolveMacros( d[ "extrapatterns" ] ) )
                        pass
                    pass
                pass

            arglines = srcs + args
            if isRflxDict:
                arglines = [name + 'Dict'] + arglines
            elif isCint:
                arglines = [name[:-4]] + ['%sDictSource' % name[:-4]] + arglines
            else:
                arglines = [name] + arglines
            
            libdata = callStringWithIndent(cmd, arglines)
            data.append(libdata)
            data.append('') # empty line
        return data

    def _generate_pyqt(self):
        # PyQt resources and UIs
        data = []
        if self.PyQtResource or self.PyQtUIC:
            data.append("# gen_pyqt_* functions are provided by 'pygraphics'")
        if self.PyQtResource:
            qrc_files = self.PyQtResource['qrc_files'].replace('../', '')
            qrc_dest = self.PyQtResource['outputdir'].replace('../python/', '')
            qrc_target = qrc_dest.replace('/', '.') + '.Resources'
            data.append('gen_pyqt_resource(%s %s %s)' % (qrc_target, qrc_dest, qrc_files))
        if self.PyQtUIC:
            ui_files = self.PyQtUIC['ui_files'].replace('../', '')
            ui_dest = self.PyQtUIC['outputdir'].replace('../python/', '')
            ui_target = qrc_dest.replace('/', '.') + '.UI'
            data.append('gen_pyqt_uic(%s %s %s)' % (ui_target, ui_dest, ui_files))
        if self.PyQtResource or self.PyQtUIC:
            data.append('') # empty line
        return data

    def _generate_copy_relax_rootmap(self):
        data = []
        if self.copy_relax_rootmap:
            data.extend([
                '# Merge the RELAX rootmaps',
                'set(rootmapfile ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/relax.rootmap)',
                callStringWithIndent('add_custom_command',
                                     ['OUTPUT ${rootmapfile}',
                                      'COMMAND ${merge_cmd} ${RELAX_ROOTMAPS} ${rootmapfile}',
                                      'DEPENDS ${RELAX_ROOTMAPS}']),
                'add_custom_target(RelaxRootmap ALL DEPENDS ${rootmapfile})',
                '\n# Install the merged file',
                'install(FILES ${rootmapfile} DESTINATION lib)\n'
            ])
        return data

    def _generate_l1common_files(self):
        data = []
        cmd = 'athena_add_l1common_files'
        for name in self.l1common_files:
            lines = [name]
            for k, v in self.l1common_files[name].iteritems():
                line = k
                for f in v:
                    line = line + ' ' + f
                lines.append(line)
            libdata = callStringWithIndent(cmd, lines)
            print '****libdata:%s' % libdata
            data.append(libdata)
            data.append('') # empty line
        return data

    def _generate_sets(self):
        data = []
        if self.sets:
            sets = []
            for s in sorted(self.sets):
                sets.append('SET ' + s + ' ' + fixSetValue(self.sets[s]))

            data.append(callStringWithIndent('athena_env', sets))
            data.append('') # empty line
        return data

    def _generate_qmtest(self):
        data = []
        # tests
        if self.QMTest:
            data.append("\nathena_add_test(QMTest QMTEST)")
        return data

    def generate(self):
        """Generate the data to put in the CMakeLists.txt file."""
        data = []
        data.extend(self._generate_header())
        data.extend(self._generate_subdirs())
        data.extend(self._generate_externals())
        data.extend(self._generate_include_dirs())
        data.extend(self._generate_no_pedantic())
        data.extend(self._generate_add_cmake())
        data.extend(self._generate_definitions())
        data.extend(self._generate_comments())
        data.extend(self._generate_applied_tags())
        data.extend(self._generate_f90())
        data.extend(self._generate_components())
        data.extend(self._generate_l1common_files())
        data.extend(self._generate_pyqt())
        data.extend(self._generate_copy_relax_rootmap())
        data.extend(self._generate_component_generation())
        data.extend(self._generate_installs())
        data.extend(self._generate_aliases())
        data.extend(self._generate_sets())
        data.extend(self._generate_qmtest())
        return '\n'.join(data) + '\n'

    @property
    def data_packages(self):
        """Return the list of data packages used by this package
        in the form of a dictionary {name: version_pattern}.
        """
        packages = []
        for usePackage in self.uses:
            if usePackage in data_packages:
                packages.append((usePackage, self.uses[usePackage].version))
        return dict(packages)

    def _prepareSources(self, sources):
        files  = []
        for source in sources:
            srcs = (stripQuote(s) for s in source.split())
            srcs = (s for s in srcs if s)
            for src in srcs:
                if src.startswith('-s='):
                    files.append(src)
                else:
                    extends = []
                    if not isBracketed(src, '$(', ')'):
                        extends = src.split()
                    else:
                        src = src.strip('$()')
                        if src in self.macros:
                            extends = stripQuote(self.macros[src]).split()

                    files.extend(extends)
        return files

    def resolvePackageName(self, txt):
        """Replace occurences of $package with package name."""
        txt = txt.replace('${PACKAGE}', self.name)
        txt = txt.replace('${package}', self.name)
        txt = txt.replace('$(PACKAGE)', self.name)
        txt = txt.replace('$(package)', self.name)
        return txt


    def relocateSources(self, sources, prefix):
        """Relocate sources to include the 'prefix' directory by default
        unless an absolute path is specified, replace ${<file>_root} or
        $(<file>_root) by '..' and replace $(<file>) by value of macro(<file>)
        if <file> appears in a macro or macro_append statement in the
        requirements file. Sources of the form -s=<prefix> are treated as
        prefixes and are appended to the existing prefix unless they are
        absolute paths in which case they replace the current prefix.
        """
        result = []
        files = self._prepareSources(sources)
        root1 = '${' + self.name + '_root}/'
        root2 = '$(' + self.name + '_root)/'
        root3 = '$(PACKAGE_ROOT)/'
        pfix = prefix
        otherPackage = False
        for f in files:
            f = self.resolvePackageName(f)
            if f.startswith('-s='):
                f = f[3:]
                isAbsolute = f.startswith((root1, root2, root3))
                for each in (root1, root2, root3):
                    f = f.replace(each, '')

                f = f.replace( '$(', '${' )
                f = f.replace( '_root)', '_root}' )

                # Look for any other "${BLA_root}" formatted string, which
                # refer to other packages. As is the case in AthenaPool
                # packages.
                m = re.match("^\${(.*)_root}\/(.*)$", f)
                otherPackage = False
                if m:
                    isAbsolute = True
                    otherPackage = True
                    f = m.group(2)

                # If no subdirectory was given, then remove
                # a possible ../ prefix:
                if prefix == '' and f.startswith('../'):
                    f = f[3:]

                if isBracketed(f, '$(', ')') or isBracketed(f, '${', '}'):
                    f = f.strip('$(){}')
                    f = stripQuote(self.macros[f]) if f in self.macros else ''

                pfix = f if f.startswith('/') or isAbsolute else os.path.join(prefix, f)

            else:
                f = f.replace('\\', '')
                isAbsolute = f.startswith((root1, root2, root3))

                for each in (root1, root2, root3):
                    f = f.replace(each, '')

                if isBracketed(f, '$(', ')') or isBracketed(f, '${', '}'):
                    f = f.strip( '$(){}' )
                    if f not in self.macros:
                        f = ''
                    else:
                        # Get the macro's value:
                        value = stripQuote(self.macros[f])

                        # At this point call this function again,
                        # in a recursive way. As the macro's value
                        # may very well contain other macros itself.
                        recursive_files = self.relocateSources([value], prefix)

                        # The results of this are just added to the
                        # returned values at this point, since the
                        # recursive call made sure that they are well
                        # formed file names by now.
                        result.extend(recursive_files)
                        continue

                if not f:
                    continue

                if pfix and not isAbsolute:
                    f = os.path.normpath(os.path.join(pfix, f))
                else:
                    f = os.path.normpath(f)

                ff = os.path.join(self.path, f)
                gl = glob.glob(ff)
                if (gl or otherPackage) and f not in result:
                    result.append(f)

        return result

    def resolveMacros(self, value):
        """This is a little brother to the relocateSources(...) function.
        It can be used to resolve all the macro declarations in a 
        given string. It can be given either a simple string, or a list
        of strings. It should give back the same type of object in the end.
        """
        # The internal result:
        result = []

        # Create a list internally:
        valueList = value if isinstance(value, list) else [value]
        for v in valueList:
            # Loop over all of the words in this string:
            resultWords = []
            for word in v.split():
                # Strip it of its quotes:
                word = stripQuote(word)
                # If it's a macro, replace it with the macro's value:
                if not isBracketed(word, '$(', ')') \
                and not isBracketed(word, '${', '}'):
                    # If it's not a macro, pass it through:
                    resultWords.append(word)
                else:
                    # Remove the macro characters:
                    word = word.strip('$(){}')
                    # Look up its value, recursively...
                    if word in self.macros:
                        value = stripQuote(self.macros[word])
                        rec_value = self.resolveMacros(value)
                        resultWords.append(rec_value)

            # Add the words as one entry to the list:
            result.append(' '.join(resultWords))

        # Return the right type:
        retval = ''
        if isinstance(value, list):
            retval = result
        elif len(result):
            retval = result[0]
        return retval

    def process(self, overwrite=None):
        cml = os.path.join( self.path, 'CMakeLists.txt' )
        # If the file is known to SVN, then don't touch it, under any
        # circumstance:
        knownToSvn = True
        try:
            FNULL = open( os.devnull, "w" )
            subprocess.check_call( [ "svn", "info", cml ],
                                   stdout = FNULL, stderr = FNULL )
        except subprocess.CalledProcessError:
            knownToSvn = False
            pass
        if knownToSvn:
            return
        # If it is not in SVN, then use the "old" logic:
        if( ( overwrite == 'force' )
            or not os.path.exists( cml )
            or ( overwrite == 'update' and
                 os.path.getmtime( cml ) <
                 os.path.getmtime( self.requirements ) ) ):
            overfile = os.path.join(overrides_dir, self.name+'.txt')
            if not os.path.exists(overfile):
                data = self.generate()
                generatedPackages.add( self.name )
                writeToFile(cml, data, self.log)
            else:
                shutil.copy(overfile, cml)
                self.log.info("****Explicit override file for package %s exists - used instead of generated code" % self.name)
                pass
        else:
            self.log.warning("file %s already exists", cml)

    def _command_version(self, args):
        self.version = args[0]

    def _command_public(self, args):
        self.isPublic = True
    _command_end_private = _command_public

    def _command_private(self, args):
        self.isPublic = False
    _command_end_public = _command_private

    def _command_use_if(self, args):
        name, path = self._getKeyValue(args, 'pkg=', 'root=')
        path = os.path.join(path, name)
        names = self.fixup_use_names.get(path, [path])
        for name in names:
            version = '*'
            doImports = True
            isPublic = True
            self.uses[name] = useStmt(version, doImports, isPublic)
    _command_use_unless = _command_use_if

    def _command_use(self, args):
        if len(args) == 1:
            macro = args[0]
            if macro.startswith(('$(', '${')):
                macro = macro[2:-1]
                if macro in self.macros:
                    macro = self.macros[macro]

            args = macro.split()

        imp = True
        if "-no_auto_imports" in args:
            imp = False
            args.remove("-no_auto_imports")

        # only one argument means usually a conditional use
        nargs = len(args)
        if nargs > 1: 
            name = args[0] if nargs == 2 else os.path.join(args[2], args[0])
            names = self.fixup_use_names.get(name, [name])
            for name in names:
                self.uses[name] = useStmt(args[1], imp, self.isPublic)

    def _command_use_ifndef(self, args):
        for a in args:
            if a.startswith('pkg='): # package name
                path = stripQuote(a, 'pkg=')
                name = os.path.basename(path)
                names = self.fixup_use_names.get(path, [path])
                for name in names:
                    version = '*'
                    doImports = True
                    isPublic = True
                    self.uses[name] = useStmt(version, doImports, isPublic)

                break

    def _command_document(self, args):
        name = args.pop(0)
        constituent = args.pop(0)
        sources = args
        self.documents.append((name, constituent, sources))
        if name == 'rootcling':
            rdict = {}
            sources = self.relocateSources(sources, self.name)
            rdict['sources'] = sources
            rdict['headers_lib'] = constituent + 'Cint'
            rdict['root_headers'] = sources
            rdict['extra_includes'] = []
            rdict['include_paths'] = []
            rdict['group'] = []
            self.root_dictionary.append(rdict)
            self.generated_sources[constituent] = ['${%sSource}' % constituent]
        elif name == 'l1common_files':
            ldict = {}
            fpath, fname = self._getKeyValue(args, 'FILEPATH=', 'FILENAME=')
            fullpath = os.path.join(fpath, fname)
            fname = self.relocateSources([fullpath], self.name)
            ldict['SCHEMA'] = fname
            self.l1common_files[constituent] = ldict

    def _command_macro(self, args):
        # FIXME: should handle macro tags
        name = args.pop(0)
        value = stripQuote(args[0])
        self.macros[name] = value

    def _command_macro_append(self, args):
        # FIXME: should handle macro tags
        name = args.pop(0)
        value = stripQuote(args[0])
        self.macros[name] = self.macros.get(name, '') + value

    def _command_macro_prepend(self, args):
        # FIXME: should handle macro tags
        name = args.pop(0)
        value = stripQuote(args[0])
        self.macros[name] = value + self.macros.get(name, '')

    def _command_macro_remove( self, args ):
        name = args.pop( 0 )
        value = stripQuote( args[ 0 ] )
        if name in self.macros.keys():
            self.macros[ name ] = self.macros[ name ].replace( value, "" )
            pass
        # If it's a statement like "macro_remove CxxUtils_linkopts -lcalg",
        # that means that the library in question should not be added to
        # the standard link options of the package.
        if name == "%s_linkopts" % self.name:
            if value.startswith( "-l" ):
                value = value[ 2: ]
                pass
            self.libraries_not_linked += [ value ]
        return

    def _command_set(self, args):
        name = args.pop(0)
        if name not in ignore_env:
            value = stripQuote(args[0])
            self.sets[name] = value

    def _command_cmake_com(self, args):
        # ignore certain commands completely
        cmd = args[0].split()[0]

        if cmd in ('make_fragment',
                   'ignore_pattern',
                   'include_dirs',
                   'pattern',
                   'include_path'):
            return
        self.cmake_comments += args

    def _command_apply_tag(self, args):
        self.applied_tags += args

    def _command_author(self, args):
        self.authors+=args
    _command_manager = _command_author

    def _command_branches( self, args ):
        return

    def _command_alias(self, args):
        # FIXME: should handle macro tags
        name = args.pop(0)
        value = stripQuote(args[0]).split()
        self.aliases[name] = value

    def _command_application(self, args):
        name = args.pop(0)
        # digest arguments (options, variables, sources)
        imports = []
        group = None
        prefix = 'src'
        sources = []
        for a in args:
            a = stripQuote(a)
            if not a:
                continue

            if a.startswith('-'): # options
                if a.startswith('-import='):
                    imports.append(a[8:])
                elif a.startswith('-group='):
                    group = a[7:]
                elif a == '-check':  # used for test applications
                    group = 'tests'
                elif a.startswith('-s='):
                    sources.append(a)
                    pass
            elif '=' not in a:
                sources.append(a)

        isTests = [s for s in sources if 'test' in s.lower()]
        if 'test' in name.lower() or isTests:
            # usually, developers do not put tests in the right group
            group = 'tests'

        sources = self.relocateSources(sources, 'src')
        self.applications.append((name, sources, group, imports))

    def _apply_pattern_detcommon_shared_library(self, dargs):
        if 'files' not in dargs:
            dargs['files'] = '*.cxx'

        exists = False
        for name, sources, group, imports in self.libs:
            if name == dargs['library']:
                break
        else:
            sources = []
            sources.append(dargs['files'])
            sources = self.relocateSources(sources, 'src')
            self.libs.append((dargs['library'], sources, None, []))

    def _apply_pattern_cmake_add_libraries(self, args):
        target = ''
        libs = []
        for a in args:
            if a.startswith('target='):
                target = a[len('target='):]
            elif a.startswith('libraries='):
                libs_ = replaceQuote(a, 'libraries=').split()
                libs.extend(libs_)

        if target:
            if target not in self.add_libraries:
                self.add_libraries[target] = []
            for lib in libs:
                name = lib
                if name.startswith('-l'):
                    name = lib[2:]
                if name not in self.add_libraries[target]:
                    self.add_libraries[target].append(lib)

    def _apply_pattern_cmake_add_command(self, args):
        lines = []
        for a in args:
            if not a.startswith('command='):
                continue

            a = stripQuote(a, 'command=')
            if a == 'metalibrary':
                cargs = {
                    'files': ['<none>'],
                    'library': self.name,
                    'cliddb': False
                }
                self.libs.append((self.name, ['<none>'], None, []))
                self.linker_library.append(cargs)
                break

            lines.append(a)
            if not a.startswith('find_package'):
                self.add_cmake.extend(lines)
            else:
                af = a[len('find_package('): -1]
                af = af.strip().split()
                pack = af[0]
                af.remove(pack)
                if 'COMPONENTS' in af:
                    af.remove('COMPONENTS')
                if pack not in self.find_packages:
                    self.find_packages[pack] = af
                else:
                    for a in af:
                        if a not in self.find_packages[pack]:
                            self.find_packages[pack].append(a)

    def _apply_pattern_cmake_add_definitions(self, args):
        lines = []
        for a in args:
            if not a.startswith('defines='):
                continue

            aa = replaceQuote(a, 'defines=').split()
            for a in aa:
                if not isBracketed(a, '$(', ')'):
                    self.add_definitions.append(a)
                else:
                    a = a.strip('$()')
                    if a in self.macros:
                        mm = self.macros[a].split()
                        self.add_definitions.extend(mm)

    def _apply_pattern_cmake_add_generated_files(self, args):
        lib  = ''
        srcs = []
        for a in args:
            if a.startswith('library='):
                lib = replaceQuote(a, 'library=')
            elif a.startswith('files='):
                aa = replaceQuote(a, 'files=').split()
                for a in aa:
                    a = a.strip()
                    if not isBracketed(a, '$(', ')'):
                        srcs.append(a)
                    else:
                        a = a.strip('$()')
                        if a in self.macros:
                            mm = self.macros[a].split()
                            srcs.extend(mm)

        if lib and srcs:
            if not lib.endswith('Dict'):
                lib += 'Dict'
            if not lib in self.generated_sources:
                self.generated_sources[lib] = []
            for src in srcs:
                if '/' not in src:
                    src = '${CMAKE_CURRENT_BINARY_DIR}/dict/' + src

                if not src in self.generated_sources[lib]:
                    self.generated_sources[lib].append(src)

    def _apply_pattern_cmake_add_dependency(self, args):
        for a in args:
            if a.startswith(('package=', 'packages=')): # package name
                paths = a.split('=')[1]
                paths = stripQuote(paths).split()
                for path in paths:
                    version = '*'
                    doImports = True
                    isPublic = True
                    self.uses[path] = useStmt(version, doImports, isPublic)

    def _apply_pattern_cmake_conditional_target(self, args):
        target, type = self._getKeyValue(args, 'target=', 'type=')
        if target and type:
            self.conditional_targets[type].append(target)

    def _apply_pattern_cmake_ignore_import(self, args):
        lib, imp = self._getKeyValue(args, 'library=', 'import=')
        if lib:
            self.ignore_imports[lib].append(imp)

    def _apply_pattern_cmake_set_macro(self, args):
        macro, value = self._getKeyValue(args, 'macro=', 'value=')
        if macro and value:
            self.macros[macro] = value

    def _apply_pattern_use_if(self, args):
        name, path = self._getKeyValue(args, 'pkg=', 'root=')
        path += name
        self.uses[name] = useStmt(path, True, True)

    def _apply_pattern_use_ifndef(self, args):
        for a in args:
            if a.startswith('pkg='): # package name
                path = stripQuote(a, 'pkg=')
                name = os.path.basename(path)
                self.uses[name] = useStmt(path, True, True)
                break

    def _apply_pattern_have_root_headers(self, args, dargs):
        rdict = {}
        name = replaceQuote(dargs['headers_lib'])
        headers = self.relocateSources([dargs['root_headers']], self.name)
        extras = []
        constituent = name + 'Dict'
        rdict['headers_lib'] = constituent + 'Cint'
        rdict['root_headers'] = headers
        rdict['include_paths'] = []
        if 'extra_includes' in dargs:
            extras = dargs['extra_includes']
        if extras:
            extras = self.relocateSources(extras, self.name)
            pass
        rdict['extra_includes'] = extras
        rdict['group'] = []
        self.root_dictionary.append(rdict)
        self.generated_sources[constituent] = ['${%sSource}' % constituent]

    def _apply_pattern_rootdictionary(self, args):
        name = 'rootcling'
        constituent = self.name + 'Dict'
        sources = [dargs['headerfiles']]
        sources.append('../src/LinkDef.h')
        self.documents.append((name, constituent, sources))
        rdict = {}
        sources = self.relocateSources(sources, self.name)
        rdict['sources'] = sources
        rdict['headers_lib'] = constituent + 'Cint'
        rdict['root_headers'] = sources
        rdict['extra_includes'] = []
        rdict['include_paths'] = []
        rdict['group'] = []
        self.root_dictionary.append(rdict)
        self.generated_sources[constituent] = ['${%sSource}' % constituent]

    def _apply_pattern_include_path(self, args):
        paths = []
        for a in args:
            files = []
            if a.startswith('extras='):
                files = replaceQuote(a, 'extras=').split()
                paths.extend(files)
            else:
                paths.append(a)

        if paths:
            self.install_more_includes['includes'] = paths

    def _apply_pattern_multi_patterns(self, pattern, args, dargs):
        if not pattern.startswith('install_'):
            getattr(self, pattern).append(dargs)
        else:
            if pattern in self.fixup_install_defaults:
                args.insert(0, self.fixup_install_defaults[pattern])
                pass
            if pattern.startswith( 'install_job_transforms' ):
                for a in args:
                    files = []
                    if a.startswith( 'tfs=' ):
                        files = replaceQuote( a, 'tfs=' ).split()
                        sources = [ '-s=scripts' ]
                        sources.extend( files )
                        sources = self.relocateSources( sources, '' )
                        if sources:
                            self.install_runtime.append( sources )
                            pass
                    elif a.startswith( 'jo=' ):
                        files = replaceQuote( a, 'jo=' ).split()
                        sources = [ '-s=share' ]
                        sources.extend( files )
                        sources = self.relocateSources( sources, '' )
                        if sources:
                            self.install_joboptions.append( sources )
                            pass
                        pass
                    pass
            else:
                sources = []
                prefix = ''
                for a in args:
                    files = []
                    if a.startswith('files='):
                        files = replaceQuote(a, 'files=').split()
                    elif a.startswith('extras='):
                        files = replaceQuote(a, 'extras=').split()
                    elif '=' not in a:
                        files.append(a)
                        pass
                    sources.extend(files)
                    pass

                sources = self.relocateSources(sources, '')
                if sources:
                    getattr(self, pattern).append(sources)

    def _apply_pattern_reflex_dictionary(self, dargs):
        headerfiles = dargs['headerfiles']
        selectfile = dargs['selectionfile']
        dargs['headerfiles'] = self.relocateSources([headerfiles], self.name)
        dargs['selectionfile'] = self.relocateSources([selectfile], self.name)
        for opt in ('navigables',
                    'dataLinks',
                    'elementLinks',
                    'elementLinkVectors'):

            try:
                darg = stripQuote(dargs[opt])
            except KeyError:
                darg = []
            else:
                if isBracketed(darg, '$(', ')'):
                    darg = darg.strip('$()')
                    if darg in self.macros:
                        darg = stripQuote(self.macros[darg])
                darg = darg.split(' ')

            dargs[opt] = darg

        try:
            darg = dargs['extralibfiles']
        except KeyError:
            darg = []
        else:
            darg = self.relocateSources([darg], self.name)
        dargs['extralibfiles'] = darg

    def _apply_pattern_dual_use_library(self, dargs):
        cargs = {
            'files': 'src/components/*.cxx',
            'library': dargs['library'], 'cliddb': True
        }
        self.component_library.append(cargs)

        sources = []
        sources.append(cargs['files'])
        self.libs.append((cargs['library'], sources, None, []))
        pattern = 'linker_library'
        dargs['library'] += 'Lib'
        files = replaceQuote(dargs['files']).split()
        sources = self.relocateSources(files, 'src')
        self.libs.append((dargs['library'], sources, None, []))
        self.linker_library.append( dargs )
        string = ''
        for s in sources:
            if string:
                string += ' '
            string += s

        dargs['files'] = string

    def _apply_pattern_poolcnv(self, dargs):
        # Translate the file names to ones that the
        # CMake code will understand:
        files = dargs['files']
        files = self.relocateSources([stripQuote(files)], self.name)
        dargs['files'] = files
        # Add a PoolCnv postfix to the library name:
        dargs[ 'library' ] = dargs[ 'library' ] + 'PoolCnv'
        # Clean up some of the arguments:
        if 'typesWithNamespace' in dargs:
            dargs['typesWithNamespace'] = self.resolveMacros(stripQuote(dargs['typesWithNamespace']))
        if 'multChanTypes' in dargs:
            dargs['multChanTypes'] = self.resolveMacros(stripQuote(dargs['multChanTypes']))
        if 'cnv_pfx' in dargs:
            dargs['cnv_pfx'] = self.resolveMacros(stripQuote(dargs['cnv_pfx']))

        # Add it as a library:
        self.libs.append((dargs['library'], ['src/*.cxx'], None, []))

        # Make sure that the package uses AthenaPoolCnvSvc:
        version = 'AthenaPoolCnvSvc-*'
        doImports = True
        isPublic = False
        self.uses['Database/AthenaPOOL/AthenaPoolCnvSvc'] = useStmt(version,
                                                                    doImports,
                                                                    isPublic)

    def _apply_pattern_generic_declare(self, dargs):
        files = stripQuote(dargs['files']).split()
        if files:
            first = files[0]
            if first.startswith('-s=../'):
                files[0] = first.replace('-s=../', '-s=')
            elif first.startswith('../'):
                files[0] = first.replace('../', '')

        files = self.relocateSources(files, '')
        if files:
            destination = self.resolvePackageName(dargs['prefix'])
            item = (files, destination)
            self.install_generic.append(item)

    def _apply_pattern_sercnv(self, dargs):
        # Translate the file names to ones that the
        # CMake code will understand:
        files = dargs[ 'files' ]
        files = self.relocateSources( [ stripQuote( files ) ], self.name )
        dargs['files'] = files
        # Set up the library name:
        if 'libtag' in dargs:
            dargs[ 'library' ] = stripQuote( dargs[ 'library' ] ) + \
              stripQuote( dargs[ 'libtag' ] ) + 'SerCnv'
            dargs[ 'cnv_pfx' ] = stripQuote( dargs[ 'libtag' ] )
        else:
            dargs[ 'library' ] = stripQuote( dargs[ 'library' ] ) + 'SerCnv'
            pass
        # Clean up some of the arguments:
        if 'typesWithNamespace' in dargs:
            dargs['typesWithNamespace'] = \
              self.resolveMacros( stripQuote( dargs[ 'typesWithNamespace' ] ) )
            pass

        # Add it as a library:
        self.libs.append( ( dargs[ 'library' ], [], None, [] ) )

        # Make sure that the package uses TrigSerializeCnvSvc:
        version = 'TrigSerializeCnvSvc-*'
        doImports = True
        isPublic = False
        self.uses['Trigger/TrigDataAccess/TrigSerializeCnvSvc'] = \
          useStmt( version, doImports, isPublic )

    def _apply_pattern_pool_plugin_library(self, dargs):
        # Declare a simple library for this pattern:
        self.libs.append((dargs['library'], ['src/*.cpp'], None, []))
        # And that we need component list generation for it:
        self.generate_componentslist.append(dargs['library'])

    def _apply_pattern_default_installed_library(self, dargs):
        dargs['files'] = '*.c*'
        dargs['library'] = self.name
        files = replaceQuote(dargs['files']).split()
        sources = self.relocateSources(files, 'src')
        self.libs.append((dargs['library'], sources, None, []))

    def _apply_pattern_component_library(self, dargs, oldpat):
        cliddb = (oldpat != 'component_library_no_genCLIDDB')
        dargs['cliddb'] = cliddb
        # Check to see whether this has previous
        # been declared as a linker_library and override it
        for l in self.linker_library:
            ll = stripQuote(l['library'])
            if dargs['library'] == ll:
                self.linker_library.remove(l)

    def _apply_pattern_linker_library(self, dargs):
        # Check to see whether this has previous
        # been declared as a component_library and override it
        for l in self.component_library:
            ll = stripQuote(l['library'])
            if dargs['library'] == ll:
                self.component_library.remove(l)

    def _getKeyValue(self, args, searchKey, searchValue):
        key = ''
        value = ''
        for a in args:
            if a.startswith(searchKey):
                key = stripQuote(a, searchKey)
            elif a.startswith(searchValue):
                value = stripQuote(a, searchValue)
        return (key, value)

    def _apply_pattern_cmake_override(self, args):
        key, value = self._getKeyValue(args, 'library=', 'type=')
        if key and value:
            self.library_overrides[key] = value

    def _fix_detcommon_link_files_pattern(self, dargs):
        pattern = "declare_xmls"
        if 'kind' in dargs:
            if dargs['kind'] == 'python_modules':
                pattern = 'declare_python_modules'
            elif dargs['kind'] == 'scripts':
                pattern = 'declare_scripts'
            elif dargs['kind'] == 'XML' or dargs['kind'] == 'xml':
                pattern = 'declare_xmls'

        elif 'prefix' in dargs and dargs['prefix'].startswith('XML'):
            pattern = 'declare_xmls'
        return pattern

    def _command_apply_pattern(self, args):
        pattern = args.pop(0)
        dargs = dict([x.split('=', 1) for x in args])

        if pattern == 'cmake_override_library_type':
            self._apply_pattern_cmake_override( args )

        # Deal with ATLAS library patterns that use
        # the package name as the library name
        if pattern in self.fixup_library_defaults and 'library' not in dargs:
            dargs['library'] = self.name
        if 'library' in dargs:
            dargs['library'] = stripQuote(dargs['library'])

        if pattern in ('detcommon_shared_library',
                       'detcommon_shared_generic_library'):
            self._apply_pattern_detcommon_shared_library(dargs)
        elif pattern == 'detcommon_link_files':
            pattern = self._fix_detcommon_link_files_pattern(dargs)
        elif pattern == 'pool_plugin_library':
            self._apply_pattern_pool_plugin_library(dargs)
        elif pattern == 'generate_componentslist':
            self.generate_componentslist.append(dargs['library'])
        elif pattern == 'pool_utility':
            self.applications.append((dargs['name'], ['utilities/%s.cpp' % dargs['name']], None, []))
        elif pattern == "trigconf_application":
            self.applications.append( ( "TrigConf%s" % dargs[ "name" ],
                                        [ "src/test/%s.cxx" % dargs[ "name" ] ],
                                        None, [] ) )
        elif pattern == "UnitTest_run":
            ut = os.path.basename( stripQuote( dargs[ "unit_test" ] ) )
            sources = [ "test/%s_test.cxx" % ut ]
            if "extra_sources" in dargs.keys():
                sources += \
                    self.relocateSources( dargs[ "extra_sources" ].split(),
                                          "src" )
                pass
            self.applications.append( ( "%s_test" % ut,
                                        sources, None, [] ) )
            self.unit_test.append( dargs )
            self.unit_tests.append( "%s_test" % ut )
            pass

        if pattern == 'qt4based_library':
            self.qt4based_libraries.append(self.name)

        if pattern in ('generic_declare_for_link',
                       'generic_declare_for_copy'):
            self._apply_pattern_generic_declare(dargs)

        # Deal with ATLAS patterns that don't
        # directly map to GAUDI ones
        oldpat = pattern
        if pattern in self.fixup_pattern_names:
            pattern = self.fixup_pattern_names[pattern]

        if pattern in ('component_library', 
                       'linker_library',
                       'tpcnv_library',
                       'poolcnv',
                       'sercnv'):
            if 'library' in dargs and dargs['library'] in self.library_overrides:
                pattern = self.library_overrides[dargs['library']]
                if pattern in self.fixup_pattern_names:
                    pattern = self.fixup_pattern_names[pattern]

        if pattern == 'poolcnv':
            self._apply_pattern_poolcnv(dargs)
        elif pattern == 'sercnv':
            self._apply_pattern_sercnv(dargs)

        # Deal with the specific ATLAS library patterns
        if pattern == 'dual_use_library':
            self._apply_pattern_dual_use_library(dargs)

        elif oldpat in ('default_installed_library', 'qt4based_library'):
            self._apply_pattern_default_installed_library(dargs)

        elif pattern == 'none':
            self.log.warning('The %s pattern isn\'t converted yet', oldpat)

        elif pattern == 'component_library':
            self._apply_pattern_component_library(dargs, oldpat)

        elif oldpat == 'linker_library':
            self._apply_pattern_linker_library(dargs)

        elif pattern == 'reflex_dictionary':
            self._apply_pattern_reflex_dictionary(dargs)

        if pattern in self.singleton_patterns:
            setattr(self, pattern, dargs or True)
        elif pattern == 'include_path':
            self._apply_pattern_include_path(args)
        elif pattern == 'have_root_headers':
            self._apply_pattern_have_root_headers(args, dargs)
        elif pattern == 'RootDictionary':
            self._apply_pattern_rootdictionary(args)
        elif pattern in self.multi_patterns:
            self._apply_pattern_multi_patterns(pattern, args, dargs)
        elif pattern.startswith('cmake_'):
            getattr(self, '_apply_pattern_' + pattern, lambda x: None)(args)
        elif pattern == 'use_if':
            self._apply_pattern_use_if(args)
        elif pattern == 'use_ifndef':
            self._apply_pattern_use_ifndef(args)
        elif pattern == 'use_unless':
            pass

    def _command_library(self, args):
        name = args.pop(0)
        # digest arguments (options, variables, sources)
        imports = []
        group = None
        sources = []
        for a in args:
            a = a.strip('"')
            if a.startswith('-'): # options
                if a.startswith('-import='):
                    imports.append(a[8:])
                elif a.startswith('-group='):
                    group = a[7:]
                elif a.startswith('-s='):
                    sources.append(a)
            elif '=' not in a:
                sources.append(a)

        sources = self.relocateSources(sources, 'src')
        self.libs.append((name, sources, group, imports))

    def _fillReflexDictionaries(self):
        rd = []
        for each in self.reflex_dictionary:
            try:
                each['dictionary'] = each['dict']
            except KeyError:
                pass
    
            _1 = stripQuote(each['dictionary'])
            _2 = each.get('options', '')
            rd.append((_1, _2))

        self.reflex_dictionaries = dict(rd)

    def _fillROOTDictionaries(self):
        rd = []
        for each in self.root_dictionary:
            _1 = stripQuote(each['headers_lib'])
            _2 = each.get('root_headers', '')
            rd.append((_1, _2))
        self.root_dictionaries = dict(rd)

        for l in self.root_dictionary:
            cintlib = l['headers_lib']
            dictlib = cintlib[:-4] # Library name without the trailing 'Cint'
            lib = dictlib[:-4] # Library name without the trailing 'Dict'
            # following mess is because of the name mapping between CMT
            # and cmake dual_use libraries and the necessity to fix up
            # the names of the arguments to the various rootcling dictionary
            # patterns if they reference the link_library component.
            if lib not in self.linker_libraries and lib.endswith('Lib'):
                lib = lib[:-3]
                l['headers_lib'] = lib + 'DictCint'
                if dictlib in self.generated_sources:
                    srcs = self.generated_sources[dictlib]
                    del self.generated_sources[dictlib]
                    newsrcs = []
                    for src in srcs:
                        src = src.replace(dictlib, lib+'Dict')
                        newsrcs.append(src)
                    self.generated_sources[lib+'Dict'] = newsrcs

        if self.root_dictionaries:
            if 'ROOT' not in self.find_packages:
                self.find_packages['ROOT'] = ['Cint']
            else:
                if 'Cint' not in self.find_packages['ROOT']:
                    self.find_packages['ROOT'].append('Cint')

    def _fillLinkerLibraries(self):
        for name, sources, group, imports in self.libs:
            if name not in self.linker_libraries \
            and name not in self.component_libraries \
            and name not in self.tpcnv_libraries \
            and name not in self.poolcnv_libraries \
            and name not in self.sercnv_libraries:
                self.log.warning('library %s not declared as component, '
                                 'linker or tpcnv library, assume linker',
                                 name)
                self.linker_libraries.append(name)

    def _fillLibraries(self):
        for name, sources, group, imports in self.libs:
            if sources and name in self.linker_libraries:
                self.libraries.append((name, sources, group, imports))

        for name, sources, group, imports in self.libs:
            if sources and name in self.component_libraries:
                self.libraries.append((name, sources, group, imports))

        for name, sources, group, imports in self.libs:
            if sources and name in self.tpcnv_libraries:
                self.libraries.append((name, sources, group, imports))

        for name, sources, group, imports in self.libs:
            if name in self.poolcnv_libraries or name in self.sercnv_libraries:
                self.libraries.append((name, sources, group, imports))

    def _fillFixupAddPackages(self):
        # FIXME: fixup_add_packages never gets filled anywhere?!
        if self.fullname in self.fixup_add_packages:
            names = self.fixup_add_packages[self.fullname]
            for name in names:
                af = name.strip().split()
                pack = af[0]
                af.remove(pack)
                if 'COMPONENTS' in af:
                    af.remove('COMPONENTS')
                if pack not in self.find_packages:
                    self.find_packages[pack] = af
                else:
                    for a in af:
                        if a not in self.find_packages[pack]:
                            self.find_packages[pack].append(a)

    def _getRequirementsLines(self):
        with open(self.requirements) as f:
            for line in f:
                if '#' in line:
                    line = line[:line.find('#')]
                line = line.strip()
                yield line

    def _handleRequirementsLines(self, argslist):
        self.isPublic = True
        self.libs = []
        for args in argslist:
            cmd = args.pop(0)
            # dispatch
            funcName = '_command_' + cmd
            func = getattr(self, funcName, lambda x: None) 
            func(args)

    def _parseRequirementsLines(self, lines):
        statement = ''
        for line in lines:
            if not line:
                continue

            # if we have something in the line, extend the statement
            statement += line
            if statement.endswith('\\'):
                # if the statement requires another line, get the next
                statement = statement[:-1] + ' '
                continue

            # remove space before/after '=' for apply_pattern
            if statement.startswith('apply_pattern'):
                statement = statement.replace('= ','=').replace(' =', '=')

            # either we got something more in the statement or not, but
            # an empty line after a '\' means ending the statement
            while statement:
                try:
                    yield list(self.CMTParser.parseString(statement))
                except Exception as e:
                    # ignore not known statements
                    self.log.debug('Failed to parse statement: %r', statement)
                    if not statement.startswith('cmake_com'):
                        statement = 'cmake_com ' + statement
                        continue
                statement = ''

    def _parseRequirements(self):
        lines = self._getRequirementsLines()
        argslist = self._parseRequirementsLines(lines)
        self._handleRequirementsLines(argslist)

        # classification of libraries in the package
        self.component_libraries = [stripQuote(l['library']) for l in self.component_library]
        self.linker_libraries = [stripQuote(l['library']) for l in self.linker_library]
        self.tpcnv_libraries = [stripQuote(l['library']) for l in self.tpcnv_library]
        self.poolcnv_libraries = [stripQuote(l['library']) for l in self.poolcnv]
        self.sercnv_libraries = [stripQuote(l['library']) for l in self.sercnv]

        self._fillReflexDictionaries()
        self._fillROOTDictionaries()
        self._fillLinkerLibraries()
        self._fillLibraries()
        self._fillFixupAddPackages()


# FIXME: where do these two templates get used?
default_toolchain_template = '''# Special toolchain file that inherits the same heptools version as the
# used projects.
find_file(inherit_heptools_module InheritHEPTools.cmake)
# this check is needed because the toolchain seem to be called a second time
# without the proper cache
if(inherit_heptools_module)
  include(${{inherit_heptools_module}})
  inherit_heptools()
endif()
'''

heptools_toolchain_template = '''# Special wrapper to load the declared version of the heptools toolchain.
set(heptools_version {0})

# Remove the reference to this file from the cache.
unset(CMAKE_TOOLCHAIN_FILE CACHE)

# Find the actual toolchain file.
find_file(CMAKE_TOOLCHAIN_FILE
          NAMES heptools-${{heptools_version}}.cmake
          HINTS ENV CMTPROJECTPATH
          PATHS ${{CMAKE_CURRENT_LIST_DIR}}/cmake/toolchain
          PATH_SUFFIXES toolchain)

if(NOT CMAKE_TOOLCHAIN_FILE)
  message(FATAL_ERROR "Cannot find heptools-${{heptools_version}}.cmake.")
endif()

# Reset the cache variable to have proper documentation.
set(CMAKE_TOOLCHAIN_FILE ${{CMAKE_TOOLCHAIN_FILE}}
    CACHE FILEPATH "The CMake toolchain file" FORCE)

include(${{CMAKE_TOOLCHAIN_FILE}})
'''

class Project(object):
    def __init__(self, path):
        """Create a project instance from the root
        directory of the project."""
        self.path = os.path.realpath(path)
        self.requirements = os.path.join(self.path, 'cmt/project.cmt')
        self.tools = {}

        # Private variables for cached properties
        self._packages = None
        self._container = None

    @property
    def packages(self):
        """Dictionary of packages contained in the project."""
        if self._packages is None:
            self._packages = {}
            for root, dirs, _files in os.walk(self.path):
                relpath = os.path.relpath(root, self.path)
                if isPackage(root) and not isIgnoredPackage(relpath):
                    p = Package(root, self)
                    name = os.path.relpath(p.path, self.path)
                    self._packages[name] = p
                    dirs[:] = []
        return self._packages

    @property
    def container(self):
        """Name of the container package of the project.
        The name of the container is deduced using the usual LHCb convention
        (instead of the content of project.cmt).
        """
        if self._container is None:
            for suffix in ['Release', 'Sys']:
                try:
                    # gets the first package that ends with the suffix,
                    # and does not have a hat.. or raise StopIteration
                    c = (p for p in self.packages
                         if p.endswith(suffix) and "/" not in p).next()
                    self._container = self.packages[c]
                    break
                except StopIteration:
                    pass
        return self._container

    @property
    def name(self):
        return 'Unknown'

    @property
    def version(self):
        return os.path.basename(self.path)

    def uses(self):
        tools = ['LCGCMT', 'tdaq', 'tdaq-common', 'dqm-common']
        with open(self.requirements) as req:
            for line in req:
                line = line.split()
                if line and line[0] == 'use' and len(line) == 3:
                    if line[1] not in tools:
                        if 'HEAD' in l[2]:
                            yield(projectCase(line[1]), line[2].rsplit('_', 1)[-1])
                        else:
                            yield(projectCase(line[1]), line[2])
                    elif line[1] not in self.tools and line[1] != 'LCGCMT':
                        self.tools[line[1]] = line[2]
                        print '****self.tools:%s' % self.tools

        if self.name == 'ManaCore':
            yield('AtlasExternals', self.version)

    def heptools(self):
        """Return the version of heptools (LCGCMT) used by this project."""
        # we cannot guess the version of heptools
        return None

    @property
    def data_packages(self):
        """Return the list of data packages used by this project
        (i.e. by all the packages in this project) in the form of
        a dictionary {name: version_pattern}.
        """
        # for debugging we map
        def appendDict(d, kv):
            """Helper function to extend a dictionary of lists."""
            k, v = kv
            d.setdefault(k, []).append(v)
            return d

        # dictionary {"data_package": ("user_package", "data_pkg_version")}
        dp2pkg = {}
        for pkgname, pkg in self.packages.items():
            for dpname, dpversion in pkg.data_packages.items():
                appendDict(dp2pkg, (dpname, (pkgname, dpversion)))

        # check and collect the data packages
        result = {}
        for dp in sorted(dp2pkg):
            versions = set([v for _, v in dp2pkg[dp]])
            if versions:
                version = sorted(versions)[-1]
            else:
                version = '*'
            if len(versions) != 1:
                logging.warning('Different versions for data package %s, using %s from %s', dp, version, dp2pkg[dp])
            result[dp] = version

        return result

    def generate(self):
        return ''

    def generateToolchain(self):
        return None

    def process(self, overwrite=None):
        # Recurse in the packages
        for p in sorted(self.packages):
            self.packages[p].process(overwrite)



def _createParser():
    from optparse import OptionParser
    usage = '%prog [options] [path to project or package]'
    desc = 'Convert CMT-based projects/packages to CMake (Gaudi project)'
    parser = OptionParser(usage=usage, description=desc)
    parser.add_option('-f',
                      '--force',
                      action='store_const',
                      dest='overwrite',
                      const='force',
                      help='overwrite existing files')
    parser.add_option('--cache-only',
                      action='store_true',
                      help=('just update the cache without creating '
                            'the CMakeLists.txt files.'))
    parser.add_option('--dump-cache',
                      action='store_true',
                      help='just dump the cache contents.')
    parser.add_option('-u',
                      '--update',
                      action='store_const',
                      dest='overwrite',
                      const='update',
                      help=('modify the CMakeLists.txt files if they '
                            'are older than the corresponding requirements.'))
    parser.add_option('--log',
                      type='string',
                      dest='logpath',
                      help='Output logging to the given file')
    parser.add_option( '--warn-on-generation',
                       action='store_true', dest='warn_on_generation',
                       help='Print warning about the executed conversions' )
    return parser


def _parseArgs(args):
    parser = _createParser()
    return parser.parse_args(args=args)


def _initLogging(logpath=None):
    if logpath:
        logging.basicConfig(filename=logpath,
                            filemode='w',
                            level=logging.INFO)
    else:
        logging.basicConfig(level=logging.INFO)


def main(args=None):
    opts, args = _parseArgs(args)
    _initLogging(opts.logpath)
    loadConfig(os.path.join(os.path.dirname(__file__), 'cmt2cmake.cfg'))
    top_dir = os.getcwd()
    if args:
        top_dir = args[0]
        if not os.path.isdir(top_dir):
            raise ValueError('{0}: is not a directory'.format(top_dir))

    if os.path.exists('cmt2cmake.lock'):
        m = 'cmt2cmake not run - Project at %s is locked' % top_dir
        logging.critical(m)
        return

    loadConfig(os.path.join(top_dir, 'cmt2cmake.cfg'))
    file = open_cache()
    if opts.dump_cache:
        print 'Cache: %s\n%s' % (file, cache)
    else:
        # Create a project object:
        root = Project( top_dir )

        if opts.cache_only:
            # the cache is updated by instantiating the packages
            root.packages 
            # this triggers the caching of the heptools_version
            root.heptools()
            # note that we can get here only if root is a project
        else:
            root.process(opts.overwrite)
            pass
        pass

    # If the user asked for warnings about the executed conversions,
    # print those warnings now:
    if opts.warn_on_generation and len( generatedPackages ):
        logging.warning( 'CMakeLists.txt was generated for package(s): %s' % \
                             str( generatedPackages ) )
        pass

    close_cache(file)
    logging.info('Cache closed at %s' % time.strftime("%d/%m/%Y %H:%M:%S"))


if __name__ == '__main__':
    main()
    sys.exit(0)
