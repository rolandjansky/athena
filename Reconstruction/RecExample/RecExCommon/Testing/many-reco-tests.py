#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__version__ = "$Revision: 270687 $"
__author__ = "Sebastien Binet"
__doc__ = "run many reco tests to prevent regressions"

### imports -------------------------------------------------------------------
import os
#import subprocess
import textwrap

import ruffus
from PyUtils.bwdcompat import subprocess

### functions -----------------------------------------------------------------

def run_athena(cmds, ctxt):
    msg = ctxt['msg']
    msg.info(":"*80)
    msg.info('running test: [%s]', ctxt['title'])
    log_file = open("def_%(step_in)sto%(step_out)s_%(suffix)s.log" % ctxt,
                    'w+b')
    msg.info('logging into [%s]', log_file.name)
    
    call = subprocess.call
    cmd = ["time", "athena.py", "-s", "-c"]
    oneliner = []
    for c in cmds:
        oneliner.append(c % ctxt)
    cmd.append('\"'+"; ".join(oneliner)+'\"')
    msg.info("starting... [%s]", subprocess.getoutput('date'))

    #print "==> %r" % cmd
    rc = call(cmd, stdout=log_file, stderr=log_file)
    log_file.flush()
    log_file.seek(0)
    stats = {'user_time': None,
             'sys_time':  None,
             'real_time': None,
             'cpu_util':  None}
    
    import re
    pattern = re.compile(r'(?P<user_time>.*?)user '
                          '(?P<sys_time>.*?)system '
                          '(?P<real_time>.*?)elapsed '
                          '(?P<cpu_util>.*?)CPU .*?')
    for line in log_file:
        m = re.match(pattern, line)
        if m:
            stats.update(m.groupdict())
            break
    for k in ('user_time', 'sys_time', 'real_time',):
        msg.info('%10s: %ss', k, stats[k])
    k = 'cpu_util'
    msg.info('%10s: %s', k, stats[k])
            
    log_file.close()
    msg.info(":"*80)
    return rc, stats
            

def main():
    suffix = os.environ.get('SUFF', 'test1')
    nevts  = int(os.environ.get('NEVT', '5'))

    import PyUtils.Logging as L
    msg = L.logging.getLogger('reco-tests')
    msg.setLevel(L.logging.INFO)
    
    msg.info('suffix used: [%s]', suffix)
    msg.info('number of events reconstructed: [%s]', nevts)

    subprocess.check_call("setupLocalDBReplica_CERN.sh",
                          stdout=open('/dev/null','w'))

    run_athena(
        cmds=textwrap.dedent(
            """
            from RecExConfig.RecFlags import rec
            rec.OutputSuffix='_%(suffix)s'
            doDumpProperties = True
            RootNtupleOutput = 'ntuple_def_%(step_in)sto%(step_out)s_%(suffix)s.root'
            EvtMax = %(nevts)s
            doNameAuditor = True
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
            acf.FilesInput = ['LFN:top_GEO-02-01-00_RDO_extract.pool']
            doWriteESD = True
            doAOD = False
            doWriteAOD = False
            doWriteTAG = False
            PoolESDOutput = 'def_%(step_in)sto%(step_out)s_%(suffix)s.ESD.pool.root'
            doDumpPoolInputContent = True
            doDumpTES = True
            """).splitlines(),
        ctxt={
            'title' : 'default file write esd',
            'msg': msg,
            'step_in': 'rdo',
            'step_out': 'esd',
            'suffix': suffix,
            'nevts':  nevts,
            }
        )
    
    return 0

if __name__ == "__main__":
    import sys
    sys.exit(main())
    
