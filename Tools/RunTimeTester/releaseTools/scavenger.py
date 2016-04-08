# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, os.path
from popen2 import Popen4

def getPopen4Output(cmd):
    po = Popen4(cmd)
    out = []
    while po.poll()==-1:
        out.extend(po.fromchild.readlines())
    del po
    return out

# ==========================================================================

def sendEmail(emailContent):
    """A simple mailer"""
    if not emailContent.strip():
        print 'All OK, no email was sent.'
        return

    def at(): return '@'
    
    to = 'rtt' + at() + 'he' + 'p.uc' + 'l.ac' + '.uk'

    subject = 'WARNING: scavenger found problems with RTT cluster'
    command = 'mail ' + str(to) + ' -s ' + '"' + str(subject) + '"'
    os.popen(command,'w').write(emailContent)
    print '------------------'
    print 'An email has been sent to %s' % to

# ==========================================================================

def checkWPC():
    print 'Running checkWPC()'
    procs = getPopen4Output('ps -elf | grep python | grep WebPageCollector.py')
    if len(procs) == 1:
        machine = getPopen4Output('uname -n')[0]
        print 'Problems found.'
        return 'WebPageCollector process not ongoing on machine %s' % machine
        
    return ''

# ==========================================================================

def checkQuotas(reportAtGreaterThan):
    print 'Running checkQuotas()'
    def getQuotaUsage(path):
        out = getPopen4Output('fs lq %s' % path)
        volume = out[1].split()[0]
        quota  = out[1].split()[1]
        used   = out[1].split()[2]
        frac   = out[1].split()[3]
        frac   = frac[:frac.find('%')]
        return (path, volume, quota, used, frac)

    message = ''
    resBase = '/afs/cern.ch/atlas/project/RTT/Results'
    workBase = '/afs/cern.ch/atlas/project/RTT/Work'
    pathsToCheck = ['/afs/cern.ch/user/r/rtt/',
                    '/afs/cern.ch/atlas/project/RTT/prod',
                    resBase,
                    workBase,
                    '/afs/cern.ch/atlas/project/RTT/data',
                    '/afs/cern.ch/atlas/project/RTT/source']

    # now check run quotas (each on a separate AFS volume)
    for rel in ['rel_0','rel_1','rel_2','rel_3','rel_4','rel_5','rel_6',]:
        for branch in ['dev','bugfix','val', 'pcache', 'devval']:
            for config in ['i686-slc4-gcc34-opt','i686-slc4-gcc34-dbg','i686-slc3-gcc323-opt']:
                resDir = os.path.join(resBase, '%s/%s/build/%s' % (rel, branch, config))
                wrkDir = os.path.join(workBase, '%s/%s/build/%s' % (rel, branch, config))
                if os.path.exists(resDir):
                    pathsToCheck.append(resDir)
                if os.path.exists(wrkDir):
                    pathsToCheck.append(wrkDir)

    results = [getQuotaUsage(path) for path in pathsToCheck]
    for path, volName, tot, used, frac in results:
        if int(frac) >= reportAtGreaterThan:
            message += 'Volume %s (%s) is %s%% used\n' % (volName, path, frac)
            print 'Problems found.'
    return message

# ==========================================================================

if __name__ == '__main__':
    message = ''
    message += checkQuotas(90) # % usage over which raise alarm
    message += checkWPC() # check WPC process is ongoing, if not raise alarm
    sendEmail(message)

    
