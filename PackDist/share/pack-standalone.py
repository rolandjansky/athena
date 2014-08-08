#!/usr/bin/env python
"""File: pack-standalone.py

Usage: pack-standalone.py [OPTION]... PATH...
Create a standalone pacman file from the pacman file built by the PackDist tools
for project (binary) package(s) found in command line argument(s) PATH...
Only dependencies on the same project packages and external software packages,
if found in the same directory as the processed package, are preserved.

  -d, --debug                  print lots of debugging information
  -h, --help                   display this help and exit
      --version                output version information and exit

Report bugs to <Grigori.Rybkine@cern.ch>.
"""

__version__ = '0.3.1'
__date__ = 'Sun Aug 01 2010'
__author__ = 'Grigory Rybkin'

import sys, os
import re
import commands
import logging
import shutil
import getopt

try:
    __path__ = [os.path.join(os.path.dirname(sys.modules[__name__].__file__), 'python')]
except AttributeError, e:
    # with Python 2.2.3, sys.modules['__main__']
    # AttributeError: 'module' object has no attribute '__file__': even for scripts
    if __name__ == '__main__':
        __path__ = [os.path.join(os.path.dirname(sys.argv[0]), 'python')]
    else:
        raise
from PackDistCommon import *

class Parser(object):
    def __init__(self, package):
        self.package = package
        self.parsers = []
        p_cmt = re.compile(r'package\(\'CMTv\d+(?:r\d+)?(?:p\d+)?.*\'\)\s*')
        self.parsers.append((p_cmt, self.comment))
        p_settings = re.compile(r'package\(\'Atlas(?:Settings|Login|Setup)\'\)\s*')
        self.parsers.append((p_settings, self.comment))
        p_self = re.compile(r'package\(\'%s.+\'\)\s*' % \
                            re.sub('[-.]', '_', '%s_%s' % \
                                   (self.package.project, self.package.release)))
        self.parsers.append((p_self, self.keep))
        p_other = re.compile(r'package\(\'(?P<package>.+)\'\)\s*')
        self.parsers.append((p_other, self.other))
        p_shell = re.compile(r'shell\s*\(.*\)\s*')
        self.parsers.append((p_shell, self.shell))
        p_any = re.compile(r'(?s).*')
        self.parsers.append((p_any, self.keep))
        self.pacman = []
        self.pre = True
        self.post = False
    def comment(self, line, match):
        self.pacman.append('#%s' % line)
        logger.debug('comment: %r\n%s' % (line, self.pacman))
    def keep(self, line, match):
        self.pacman.append(line)
        logger.debug('keep: %r\n%s' % (line, self.pacman))
    def other(self, line, match):
        logger.debug('other: %r %s' % (line, match.group('package')))
        try:
            pkg = Package(os.path.join(os.path.dirname(self.package.filename),
                                       match.group('package') + self.package.name[1]
                                       )
                          )
            if pkg.type == 'extern':
                self.keep(line, match)
                logger.warning('%s: Kept external dependency %r',
                               self.package.name[0], match.group('package'))
            else:
                self.comment(line, match)
        except IOError, e:
            logger.debug('other: %s', e, exc_info=True)
            self.comment(line, match)
    def skip(self, line, match):
        logger.debug('skip: %r\n%s' % (line, self.pacman))
    def shell(self, line, match):
        logger.debug('shell: %r\n%s' % (line, self.pacman))
        if self.pre:
            self.pre = False
            self.post = True
            l = self.package.pre + os.linesep
            self.pacman.append(l)
            logger.debug('append: %r\n%s' % (l, self.pacman))
        elif self.post:
            try:
                self.post = False
                l = self.package.post + os.linesep
                self.pacman.append(l)
                logger.debug('append: %r\n%s' % (l, self.pacman))
            except AttributeError, e:
                logger.debug('shell: %s', e, exc_info=True)
                self.comment(line, match)
        else:
            logger.warning('unexpected: %r' % line)
            self.comment(line, match)
            
    def parse(self):
        for line in self.package.pacman.splitlines(True):
            logger.debug('parsing: %r', line)
            for (p, a) in self.parsers:
                m = p.match(line)
                if m:
                    a(line, m)
                    break

        logger.debug('parsed: %s' % self.pacman)
        return ''.join(self.pacman)

class Package(object):
    def __init__(self, filename):
        self.name = os.path.splitext(os.path.basename(filename))
        assert self.name[1] == '.pacman', '%s: Instantiated with file name' % `filename`
        self.filename = filename

        pacmanfile = open(self.filename)
        self.pacman = pacmanfile.read()
        pacmanfile.close()
        
        m = re.search(r'description(.*External.*)', self.pacman)
        if m: self.type = 'extern'
        else: self.type = 'proj'

        if self.name[0].startswith('CMTv'): self.type = 'CMT'

        if self.type in ('extern', 'CMT'): return

        name = self.name[0].split('_')
        self.project = name[0]
        if name[0] in ('AtlasSettings', 'AtlasLogin', 'AtlasSetup'):
            self.platform = 'noarch'
            self.release = re.search(r'version\(\'(?P<version>.+)\'\)',
                                     self.pacman).group('version')
        elif name[-1] in ('src', 'noarch'):
            self.platform = name[-1]
            self.release = '.'.join(name[1:-1])
        else:
            self.platform = '-'.join(name[-4:])
            self.release = '.'.join(name[1:-4])
            self.pre = 'shell(\'[ "$CMTROOT" ] && [ "$CMTBIN" ] && [ "$CMTCONFIG" ] || { echo "Project environment not set" >&2; exit 1; }\')'

            m = re.search(r'shell\s*\(".*?(?P<config>cd\s+\$PACMAN_INSTALLATION/(?P<project>[\w.-]+)/(?P<release>[\w.-]+)/(?P<cntnrcmt>([\w.-]+/)+cmt).*)"\)\s*', self.pacman)
            if m:
                self.project = m.group('project')
                self.release = m.group('release')
                self.cntnrcmt = m.group('cntnrcmt')
                self.post = 'shell("unset CMTPATH; %s")' % m.group('config')
                logger.debug('cntnrcmt: %s', m.group('cntnrcmt'))
                logger.debug('postinstall script:\n%s', self.post)
            else:
                logger.warning('%s %s %s: Cannot determine postinstall script',
                               self.project, self.release, self.platform)

        logger.debug('%s %s %s: Initialised',
                     self.project, self.release, self.platform)

    def standalone(self):
        if self.type in ('extern', 'CMT'):
            logger.debug('type %s: %s: Nothing to change',
                         `self.type`, self.name[0])
            return 0
        if self.platform in ('src', 'noarch'):
            logger.debug('platform %s: %s: Nothing to change',
                         `self.platform`, self.name[0])
            return 0

        if len(self.release.split('.')) < 4:
            logger.warning('%s %s %s %s: Does not look a patch project',
                           self.name[0], self.project, self.release, self.platform)
            
        try:
            pacman_new = Parser(self).parse()
#            if pacman_new == ''.join(pacman):
            if pacman_new == self.pacman:
                logger.info('standalone: %s' % self.filename)
                return 0

            filename_real = os.path.realpath(self.filename)
            os.rename(filename_real, filename_real + '.save')
#            shutil.copy2(filename_real, filename_real + '.save')
#            logger.info('copied %s -> %s',
            logger.info('renamed %s -> %s',
                        `filename_real`, `filename_real + '.save'`)
            
            pacmanfile = open(filename_real, 'w')
            pacmanfile.write(pacman_new)
            pacmanfile.close()
            logger.info('wrote: %s' % pacmanfile.name)
            return 0

        except (IOError, os.error), e:
#            logger.error(e)
            logger.error(e, exc_info=True)
#            pacmanfile.close()
            return 1

def pkg_files(paths):
    ps = []
    errs = []
    for p in paths:
        if os.path.isfile(p) or os.path.isdir(p):
            ps.append(p)
        else:            
            errs.append(p)
    for p in errs:
        print >> sys.stderr, 'No such file or directory:', p
    if not ps:
        raise InputError(' '.join(paths), 'No valid arguments')

    cmd = 'find ' + ' '.join(ps) + ' -noleaf -name "*.pacman"'
#    cmd = 'find ' + ' '.join(ps) + ' -noleaf \\( -name "*opt*.pacman" -o -name "*dbg*.pacman" \\)'
    sc, filenames = commands.getstatusoutput(cmd)
    if sc != 0:
        # The exit status for the command can be interpreted
        # according to the rules for the C function wait().
        raise CommandError(cmd, filenames, sc)
    return filenames.splitlines()

def help():
    print sys.modules[__name__].__doc__

def main(argv=[__name__]):
    self = os.path.basename(argv[0])
    try:
        opts, args = getopt.getopt(argv[1:],
                                   "hd",
                                   ["help", "version", "debug"])
    except getopt.error, e:
        print >>sys.stderr, '%s: %s' % (self, str(e))
        print >>sys.stderr, "Try '%s --help' for more information." % self
        return 1

    global logger
    logging.basicConfig()
    logger = logging.getLogger(self)
    logger.setLevel(logging.INFO)

#     ch = logging.StreamHandler()
#     ch.setFormatter(logging.Formatter("%(name)s: %(levelname)s: %(message)s"))
#     ch.setLevel(logging.INFO)

    for o, v in opts:
        if o in ("-h", "--help"):
            print sys.modules[__name__].__doc__
            return 0
        elif o in ("--version",):
            print '%s %s (%s)' % (self, __version__, __date__)
            print '%sWritten by %s.' % (os.linesep, __author__)
            return 0
        elif o in ("-d", "--debug"):
            logger.setLevel(logging.DEBUG)
#             ch.setLevel(logging.DEBUG)

#     logger.addHandler(ch)

    if not args:
        print >>sys.stderr, '%s: missing path argument' % self
        print >>sys.stderr, "Try '%s --help' for more information." % self
        return 1

    try:
        filenames = pkg_files(args)
    except InputError, e:
        print >> sys.stderr, e
        help()
        return 1

    sc = 0
    for filename in filenames:
        sc += Package(filename).standalone()

    if sc != 0: return 1
    else: return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))
