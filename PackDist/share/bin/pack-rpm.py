#!/usr/bin/env python
"""File: pack-rpm.py

This script builds (binary) rpm package
based on meta-data provided and from a tarball built
by the PackDist package.
"""

__version__ = '0.12.0'
__date__ = 'Tue Mar 15 2016'
__author__  = 'Grigori Rybkine <Grigori.Rybkine@cern.ch>'

#
# Write spec file and build (binary) rpm package
#
# Parameters: 'name package/project version [architecture]' ['requires-file'
# ['postinstall-script' ['preinstall-script' ['version' ['source-paths' 'destination-paths'
# ['preuninstall-script']]]]]]
#

import sys, os
import re
import commands

try:
    __path__ = [os.path.join(os.path.dirname(sys.modules[__name__].__file__), os.pardir, 'python')]
except AttributeError, e:
    # with Python 2.2.3, sys.modules['__main__']
    # AttributeError: 'module' object has no attribute '__file__': even for scripts
    if __name__ == '__main__':
        __path__ = [os.path.join(os.path.dirname(sys.argv[0]), os.pardir, 'python')]
    else:
        raise
from PackDistCommon import *

def _getver(name, default='', type=None):
    fpath = os.path.join(os.environ['projcache'], 'specs', name + '.spec')
    if not os.access(fpath, os.R_OK): return default
    s = file(fpath).read()
    p = r'version[ \t]*:[ \t]*(?P<ver>[\w.]+)'
    m = re.search(p, s, re.I)
    if m: return m.group('ver')
    else: return default

def info(message='', location=''):
    if message: message = ': ' + message
    if location: location += ': '
    print >> sys.stderr, "%s: %sINFO%s" % (os.path.basename(sys.argv[0]), location, message)

def warn(message='', location=''):
    if message: message = ': ' + message
    if location: location += ': '
    print >> sys.stderr, "%s: %sWARNING%s" % (os.path.basename(sys.argv[0]), location, message)

def error(message='', location=''):
    if message: message = ': ' + message
    if location: location += ': '
    print >> sys.stderr, "%s: %sERROR%s" % (os.path.basename(sys.argv[0]), location, message)

def rpmfile(name, version, release, arch, type=None):
    cmd = 'rpm -D "NAME %s" -D "VERSION %s" -D "RELEASE %s" -D "ARCH %s" -E `rpm -E %%_rpmfilename`' % (name, version, release, arch)
    status, rpmfilename = commands.getstatusoutput(cmd)
    if status != 0:
        error(rpmfilename, rpmfile.__name__)
        raise CommandError(cmd, rpmfilename, exitstatus(status))
    if type == 'extern': rpmdir = os.getenv('externdir')
    elif type == 'proj': rpmdir = os.getenv('projdir')
    else: rpmdir = None
    if not rpmdir: rpmdir = os.getenv('rpmdir')
    if not rpmdir:
        cmd = 'rpm -E %_rpmdir'
        status, rpmdir = commands.getstatusoutput(cmd)
        if status != 0:
            error(rpmdir, rpmfile.__name__)
            raise CommandError(cmd, rpmfilename, exitstatus(status))
    return (rpmdir, rpmfilename)

def _getalien(requires):
    reqs = []
    for r in requires:
        i = r.rfind(':')
        if i != -1: reqs.append((r[0:i], r[i+1:]))
    return reqs
        
def _buildalien(aliendeps, buildarch):
    errs = list()
    builddir = _builddir()
    buildrootdir = _buildrootdir()
    tmppath = _tmppath()
    sourcedir = tmppath
    suffix = '.tar.gz'
    for d in aliendeps:
        i = d[1].find('-')
        if i == -1:
            warn('%s %s: No name or version specified' % (d[0], d[1]),
                 _buildalien.__name__)
            continue
        else:
            _name = d[1][0:i]
            _version = d[1][i + 1:]
        if not _name:
            warn('%s %s: Empty name specified' % (d[0], d[1]),
                 _buildalien.__name__)
            continue
        if not _version:
            warn('%s %s: Empty version specified' % (d[0], d[1]),
                 _buildalien.__name__)
            continue
        try:
            rpmdir, rpmfilename = rpmfile(d[1], _version.replace('-', '.'), '1', buildarch, 'extern')
        except Error, e:
            errs.append(e)
            e.write()
            continue
        rpmpath = os.path.join(rpmdir, rpmfilename)
        if os.path.isfile(rpmpath):
            info('%s: File exists' % rpmpath)
            continue
        specpath = os.path.join(os.path.dirname(sys.argv[0]), os.pardir, 'specs', _name + '.spec')
        if not os.path.isfile(specpath):
            warn('%s: No such file' % specpath,
                 _buildalien.__name__)
            continue
        _swversion = os.getenv('release')
        if not _swversion:
            _swversion = _version
        source0 = os.path.join(sourcedir, d[1] + suffix)
        if d[0].startswith('http://'):
            cmd = '{ cd %s && wget %s; } >&2' % (sourcedir, os.path.join(d[0], d[1] + suffix))
            status = os.system(cmd)
            if status != 0:
                errs.append(Error(_buildalien.__name__, cmd, exitstatus(status)))
                error(cmd, _buildalien.__name__)
                if os.path.exists(source0):
                    os.remove(source0)
                continue
        else:
            warn('%s %s: No source found' % (d[0], d[1]),
                 _buildalien.__name__)
            continue
        
        cmd = 'rpmbuild --define "_version %s" --define "_swversion %s" --define "_sourcedir %s" --define "_builddir %s" --define "_buildrootdir %s" --define "_rpmdir %s" --define "_tmppath %s" --target=%s -bb %s >&2' % (_version, _swversion, sourcedir, builddir, buildrootdir, rpmdir, tmppath, buildarch, specpath)
        status = os.system(cmd)
        if status != 0:
            errs.append(Error(_buildalien.__name__, cmd, exitstatus(status)))
            error(cmd, _buildalien.__name__)
        os.remove(source0)

    if errs: raise errs.pop()

#macro AtlasDBRelease_requires "http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/pacman4/DBRelease:DBRelease-$(AtlasDBRelease_native_version)"

def _tmppath():
    #tmppath = os.getenv('builddir')
    tmppath = os.getenv('TMPDIR') # should always be set
    if not tmppath:
        cmd = 'rpm -E %_tmppath'
        status, tmppath = commands.getstatusoutput(cmd)
        if status != 0:
            #error(tmppath, rpmfile.__name__)
            raise CommandError(cmd, tmppath, exitstatus(status))
    return tmppath

def _builddir():
    builddir = os.getenv('builddir') # should always be set
    if not builddir:
        cmd = 'rpm -E %_builddir'
        status, builddir = commands.getstatusoutput(cmd)
        if status != 0:
            #error(builddir, rpmfile.__name__)
            raise CommandError(cmd, builddir, exitstatus(status))
    return builddir

def _buildrootdir():
    buildrootdir = os.getenv('buildrootdir') # should always be set
    if not buildrootdir:
        cmd = 'rpm -E %_buildrootdir'
        status, buildrootdir = commands.getstatusoutput(cmd)
        if status != 0:
            #error(buildrootdir, rpmfile.__name__)
            raise CommandError(cmd, buildrootdir, exitstatus(status))
    return buildrootdir

def _specdir():
    specdir = os.getenv('specdir') # should always be set
    if not specdir:
        cmd = 'rpm -E %_specdir'
        status, specdir = commands.getstatusoutput(cmd)
        if status != 0:
            #error(specdir, rpmfile.__name__)
            raise CommandError(cmd, specdir, exitstatus(status))
    return specdir

##################################### main ##########################################

NPVA = sys.argv[1].split() # Name Package Version Architecture (CMTCONFIG noarch)
                           # Name Project Release Architecture (bin src doc noarch)
if len(NPVA) < 3:
    error('%s: Too few parameters' % ' '.join(NPVA))
    sys.exit(2)
if len(NPVA) == 3:
    NPVA.append(os.getenv('CMTCONFIG'))

name = NPVA[0] # 'ROOT_3_10_02_0411_slc3_gcc323'

if len(sys.argv) > 7 and sys.argv[6] and sys.argv[7]: # external software paths
    type = 'extern'
else:
    type = 'proj'

if len(sys.argv) > 5 and sys.argv[5]:
    version = sys.argv[5]
else:
    try: version = str(int(_getver(name)) + 1)
    except ValueError:
        version = '1'
        print >> sys.stderr, "%s: Version set to %s" % (name, version)
        warn('%s: Set version to %s' % (name, version))
        
summary = ' '.join(NPVA[1:3]) #name #'ROOT'

if NPVA[3] in ['src', 'doc', 'noarch']:
    buildarch = 'noarch'
else:
    buildarch = os.getenv('CMTCONFIG').split('-')[0] #'i686'

# add dependencies
extradeps = ['atlas-filesystem']
requires = extradeps
if len(sys.argv) > 2 and sys.argv[2]:
    reqfile = file(sys.argv[2])
    requires += reqfile.read().splitlines()
requires = [r.split()[0] for r in requires if r]

retval = 0
# try to build dependencies from external tarballs, if can be found
try:
    _buildalien(_getalien(requires), buildarch)
except Error, e:
    retval += 1

requires = ', '.join([r[r.rfind(':') + 1:] for r in requires])

rpmdir, rpmfilename = rpmfile(name, version, '1', buildarch, type)
rpmpath = os.path.join(rpmdir, rpmfilename)
if os.path.isfile(rpmpath):
    info('%s: File exists' % rpmpath)
    sys.exit(retval)

# add provides, if needed
provides = []
#if len(sys.argv) > 7 and sys.argv[6] and sys.argv[7]: # external software paths
if type == 'extern':
    p = '_'.join([NPVA[1], NPVA[2], '_'.join(NPVA[3].split('-'))])
    if p != NPVA[0] and p not in provides:
        provides += [p]
# IndexError: list index out of range
provides = ', '.join(provides)

# write spec file
specinfile=file(os.path.join(os.path.dirname(sys.argv[0]), os.pardir, 'specs', 'proj.spec.in'))
#IOError: [Errno 2] No such file or directory: '/data/rybkine/PAD/bin/specs/extern.spec.in'
spec = specinfile.read() % vars()
# KeyError: provides
specinfile.close()

if not requires:
    spec = re.sub(r'(?mi)^requires[ \t]*:[ \t\S]*\n', '', spec)
if not provides:
    spec = re.sub(r'(?mi)^provides[ \t]*:[ \t\S]*\n', '', spec)

# fill in %files section
if type == 'extern': sourcedir = os.path.join(os.environ['externcache'], 'kits')
elif type == 'proj': sourcedir = os.path.join(os.environ['projcache'], 'kits')
else: sourcedir = None
if not sourcedir:
    error('No source directory specified')
    sys.exit(2)

source0 = os.path.join(sourcedir, name + '.tar.gz')
if os.path.isfile(source0):
    if len(sys.argv) > 7 and sys.argv[6] and sys.argv[7]: # external software paths
        if NPVA[3].endswith('_debuginfo'):
            cmd = 'find . -noleaf \\( \\! -type d -o -empty \\) -print >|$RPM_BUILD_DIR/files.txt\n'
            cmd += '%{__sed} -i "s#^\\.#%%{prefix}#" $RPM_BUILD_DIR/files.txt\n'
            spec = spec.replace('%{_fixperms} .\n', '%{_fixperms} .\n' + cmd)
            spec = spec.replace('%files', '%files -f files.txt')
        else:
            src = sys.argv[6].split()
            dst = sys.argv[7].split()
            paths = ''
            for f in zip(src, dst):
                if not (os.path.isfile(f[0]) or os.path.isdir(f[0])):
                    error("%s: No such file or directory" % f[0])
                    retval += 1
                paths += os.path.join('%{prefix}', f[1]) + '\n'
            if paths:
                spec = spec.replace('%defattr(-,root,root,-)\n',
                                    '%defattr(-,root,root,-)\n' + paths)

    elif len(NPVA) > 3: # projects, settings
        if NPVA[3] == 'bin':
            cmd = 'find . -noleaf -name ' + os.getenv('CMTCONFIG') + ' -prune -print >|$RPM_BUILD_DIR/files.txt\n'
            cmd += '%{__sed} -i "s#^\\.#%%{prefix}#" $RPM_BUILD_DIR/files.txt\n'
            spec = spec.replace('%{_fixperms} .\n', '%{_fixperms} .\n' + cmd)
            spec = spec.replace('%files', '%files -f files.txt')
        elif NPVA[3] == 'debuginfo':
            cmd = 'find . -noleaf \\( \\! -type d -o -empty \\) -print >|$RPM_BUILD_DIR/files.txt\n'
            # cmd = 'find . -noleaf -name "*' + os.getenv('debuginfosuffix') + '" -print >|$RPM_BUILD_DIR/files.txt\n'
            cmd += '%{__sed} -i "s#^\\.#%%{prefix}#" $RPM_BUILD_DIR/files.txt\n'
            spec = spec.replace('%{_fixperms} .\n', '%{_fixperms} .\n' + cmd)
            spec = spec.replace('%files', '%files -f files.txt')
        elif NPVA[3] in ['src', 'doc']:
            cmd = 'find . -noleaf \\( \\! -type d -o -empty \\) -print >|$RPM_BUILD_DIR/files.txt\n'
            cmd += '%{__sed} -i "s#^\\.#%%{prefix}#" $RPM_BUILD_DIR/files.txt\n'
            spec = spec.replace('%{_fixperms} .\n', '%{_fixperms} .\n' + cmd)
            spec = spec.replace('%files', '%files -f files.txt')
        elif NPVA[3] == 'noarch':
            cmd = 'find . -noleaf -type d -name cmt -exec test -f \'{}\'/requirements \\; \
-exec touch -a \'{}\'/setup.sh \'{}\'/setup.csh \'{}\'/cleanup.sh \'{}\'/cleanup.csh \\; \
-not -exec test -f \'{}\'/Makefile \\; -exec %{__cp} -p $PACKRPM/specs/Makefile.cmt  \'{}\'/Makefile \\;\n'
            spec = spec.replace('%{_fixperms} .\n', '%{_fixperms} .\n' + cmd)
            if NPVA[1] == 'AtlasSetup':
                # %{prefix}/NPVA[1]
                paths = os.path.join('%{prefix}', NPVA[1]) + '\n'
            else:
                # %{prefix}/NPVA[1]/NPVA[2]
                paths = os.path.join('%{prefix}', NPVA[1], NPVA[2]) + '\n'
            spec = spec.replace('%defattr(-,root,root,-)\n',
                                '%defattr(-,root,root,-)\n' + paths)
        else:
            error("%s: Unknown platform: %s %s %s" % NPVA[3], NPVA[0], NPVA[1], NPVA[2])
            sys.exit(1)
    else:
        error("%s: Cannot build file list: %s" % source0, sys.argv[1])
        sys.exit(1)
else:
    warn("%s: No such file" % source0)
    spec = re.sub(r'(?mi)^Source0[ \t]*:[ \t\S]*\n', '', spec)

    spec = re.sub(r'(?m)^%{__gzip}.*$', 'touch %{name}.ghost', spec)
    paths = '%ghost %{prefix}/%{name}.ghost\n'
    spec = spec.replace('%defattr(-,root,root,-)\n',
                        '%defattr(-,root,root,-)\n' + paths)
    #spec = re.sub(r'(?s)\n%install.*\n%clean', '\n%install\n\n%clean', spec)

    #spec = re.sub(r'(?s)\n%files.*\Z', '\n', spec)

logpath = '$RPM_INSTALL_PREFIX/var/log/scripts.log'
# add preinstall-script
if len(sys.argv) > 4 and sys.argv[4]:
    prefile = file(sys.argv[4])
    pre = prefile.read().replace('@INSTALL_PREFIX@', '$RPM_INSTALL_PREFIX')
    prefile.close()
    if pre:
        pre = '{ %s; } >>%s 2>&1' % (pre.rstrip(), logpath)
        #spec += '\n%pre\n' + pre + '\n'
        spec += '\n%pre\nshift $#\n' + pre + '\n' # better add to the script
#    echo "shell('$(sed s/@INSTALL_PREFIX@/\$PACMAN_INSTALLATION/g $4)')" >>${fpath}

# add postinstall-script
if len(sys.argv) > 3 and sys.argv[3]:
    postfile = file(sys.argv[3])
    post = postfile.read().replace('@INSTALL_PREFIX@', '$RPM_INSTALL_PREFIX')
    postfile.close()
    if post:
        post = '{ %s; } >>%s 2>&1' % (post.rstrip(), logpath)
        #spec += '\n%post\n' + post + '\n'
        spec += '\n%post\nshift $#\n' + post + '\n' # better add to the script
#    echo "shell(\"$(sed s/@INSTALL_PREFIX@/\$PACMAN_INSTALLATION/g $3)\")" >>${fpath}

# add preuninstall-script
if len(sys.argv) > 8 and sys.argv[8]:
    preunfile = file(sys.argv[8])
    preun = preunfile.read().replace('@INSTALL_PREFIX@', '$RPM_INSTALL_PREFIX')
    preunfile.close()
    if preun:
        preun = '{ %s; } >>%s 2>&1' % (preun.rstrip(), logpath)
        spec += '\n%preun\n' + preun + '\n'

builddir = _builddir()
buildrootdir = _buildrootdir()
tmppath = _tmppath()
specdir = _specdir()

specpath = os.path.join(specdir, name + '.spec')
specfile = file(specpath, 'w')
specfile.write(spec)
specfile.close()
#info "Writing ${fpath}, version ${ver}" $FUNCNAME
info("Wrote: %s, version %s" % (specpath, version))

cmd = 'rpmbuild --define "_sourcedir %s" --define "_builddir %s" --define "_buildrootdir %s" --define "_rpmdir %s" --define "_tmppath %s" --target=%s -bb %s >&2' \
      % (sourcedir, builddir, buildrootdir, rpmdir, tmppath, buildarch, specpath)
status = os.system(cmd)
if status != 0:
    if exitstatus(status):
        retval = exitstatus(status)
    else:
        retval += 1
    error(cmd)

sys.exit(retval)
