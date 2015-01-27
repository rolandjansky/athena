#!/usr/bin/env python
# @purpose a simple python script to beautify and make genreflex cleverer
# @date June 2010

import sys
import os
import subprocess
import shlex

def main():
    #print ":::",os.getcwd()
    genreflex_cmd = None
    env = dict(os.environ)
    for idx in range(2,len(sys.argv)):
        if os.path.exists(sys.argv[idx]) or sys.argv[idx] == 'python':
            genreflex_cmd = sys.argv[idx:]
            for e in sys.argv[2:idx]:
                k = e.split("=")[0]
                v = e[len(k)+1:]
                env[k] = str(v)
            break
    
    # now remove possible "-isystem dir" from the list of arguements
    #  gcc will not issue warnings that come from system headers, so in future we
    #   need to be able to suppress warnings from external packages as well
    #  The way to do it is to add these directories to headers from externals to
    #   the list of system headers by " -isystem dir "
    #  Directories listed as "-isystem dir" AND "-I dir" are still classified as
    #   system headers
    idx=2
    while ( idx < len(genreflex_cmd) ):
        print "RS---",genreflex_cmd[idx],genreflex_cmd[idx].startswith( '-isystem' );
        if genreflex_cmd[idx].startswith( '-isystem' ):
            if len(genreflex_cmd)>idx+1:
                del genreflex_cmd[idx+1]
            del genreflex_cmd[idx]
        idx += 1
        
    libdict_name = sys.argv[1]
    munged_reflex_opts = []
    if (libdict_name != "DBDataModelTPCnv" and
        libdict_name != "PersistentDataModelTPCnv"):
        import re
        # match dict names xyzTPCnv, xyzTPCnv2
        if (re.match(r"\w*?TPCnv(\d*?|)", libdict_name) or
            libdict_name.startswith('ARA_')):
            munged_reflex_opts.append("--dataonly")
            munged_reflex_opts.append("--no_membertypedefs")
    #print ">>>", libdict_name, "[%s]" % (munged_reflex_opts,)

    cmd = genreflex_cmd + munged_reflex_opts
    #print "---",cmd
    import subprocess
    p = subprocess.Popen(cmd, env=env)
    rc = p.wait()
    #print ">>>", rc
    return p.returncode

if __name__ == "__main__":
    sys.exit(main())
    
