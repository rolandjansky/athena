#
# 

from PyCmt.pkgbuild.cmmi import *

pkg_name= "gdb"
pkg_ver = "7.4.1"
pkg_src = "%(pkg_root)s/src/%(pkg_name)s-%(pkg_ver)s.tar.bz2"
# defaults from cmmi are fine for those:
#pkg_install_dir = "%(CMTINSTALLAREA)s/%(tag)s"

cmmi_configure = configure
def configure(self):
    env = self.env
    msg = self.msg
    pkg_install_dir = self['pkg_install_dir']
    sh = self.sh
    
    ## env['CPPFLAGS'] = env["CPPFLAGS"]\
    ##                   .replace('"','')\
    ##                   .replace('-D_GNU_SOURCE', '')
    ## env['CFLAGS'] = env['CFLAGS'].replace('-D_GNU_SOURCE', '')\
    ##                 .replace('-ansi', '-std=c99')\
    ##                 .replace('-W ', ' ')
    
    ## print "---> [%s]" % env['CFLAGS']
    for k in ('CFLAGS','CXXFLAGS'):
        env[k] = env[k].replace('-Wall', '')
        env[k] = env[k].replace('-pedantic', '')
        env[k] = env[k].replace('-Werror', '')
        #env[k] = env[k].replace('-ansi', '-std=c99')
        env[k] = env[k].replace('-std=c11', '')
        env[k] = env[k].replace('-ansi', '')
        env[k] = env[k].replace('-Wstrict-prototypes ',' ')
        env[k] = env[k].replace('-W ', ' ')
        #print "---> [%s]: %s" % (k, env[k])
        pass

    # FIXME: this should be properly propagated from the cmt-uses...
    _pylib = os.path.join(self.cmt(macro_value='Python_home'), 'lib')
    env['LDFLAGS'] += ' -L%s' % (_pylib,)
    
    ## print "---> [%s]" % env['CFLAGS']

    is32bits = '-m32' in env['CFLAGS']
    
    for k in ('CPPFLAGS',
              'CFLAGS',
              'CXXFLAGS',
              'FCFLAGS',
              'LDFLAGS',
              'LIBS',
              ):
        #env[k] = ''
        #del env[k]
        pass
    
    target='x86_64-unknown-linux-gnu'
    import sys
    if sys.platform == 'darwin':
        target='x86_64-apple-darwin10'
        
    if is32bits:
        #env['CFLAGS'] = '-fPIC -m32 -std=c99'
        #env['LDFLAGS'] = '-Wl,-Bdynamic -ldl -m32'
        env['CC'] = env['CC'] + ' -m32'
        env['CXX'] = env['CXX'] + ' -m32'
        target='i686-unknown-linux-gnu'
        if sys.platform == 'darwin':
            target='i686-apple-darwin10'

    ## for k in ('CPPFLAGS',
    ##           'CFLAGS',
    ##           'CXXFLAGS',
    ##           'FCFLAGS',
    ##           'LDFLAGS',
    ##           'LIBS',
    ##           ):
    ##     print ":::[%s]: [%s]" % (k, env.get(k, 'N/A'))
        
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % env)
    
    # apply atlas patches
    # ...
    from glob import glob
    if 0:
        patches = sorted(glob("%(pkg_root)s/src/change*.diff"%env))
        for patch in patches:
            cmd = [
                "patch",
                "-Np0",
                "-i", patch,
                ]
            print "---",patch
            #print "==>",sh.getcwd()
            #print ":::",' '.join(cmd)
            self.run(cmd)
        

    msg.debug('configure...')
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % env)

    cmd = [
        "./configure",
        "--prefix=%(pkg_install_dir)s" % env,
        #"--disable-werror",
        #"--disable-nls",
        #"--with-system-readline",
        "--build=%s" % target,
        "--enable-werror=no",
        "--enable-build-warnings=no",
        ]
    self.run(cmd)
    #return cmmi_configure(self)

def build(self):
    sh  = self.sh
    msg = self.msg
    env = self.env

    msg.debug('make...')
    sh.chdir("%(pkg_build_dir)s/%(pkg_name)s-%(pkg_ver)s" % env)

    cmd = "make -j%i" % cpu_count()
    #cmd = "make"
    self.run(cmd)
    
