# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, time
from popen2 import Popen4

files = {
    ('bug', '32bit', 'SLC4') : '/afs/cern.ch/atlas/software/builds/nightlies/bugfix/AtlasProduction/latest_copied_releaseBF32BS4AllOpt',
    ('bug', '32bit', 'SLC3') : '/afs/cern.ch/atlas/software/builds/nightlies/bugfix/AtlasProduction/latest_copied_releaseBF32BS3AllOpt',
    ('val', '32bit', 'SLC4') : '/afs/cern.ch/atlas/software/builds/nightlies/val/AtlasProduction/latest_copied_releaseVAL32BS4AllDbg'
    }

today = time.ctime().split()
todaysDate = {}
todaysDate['month'] = today[1]
todaysDate['day']   = today[2]
todaysDate['hour']  = today[3].split(':')[0]
todaysDate['mins']  = today[3].split(':')[1]

for platform, flag in files.items():
    cmd = 'cd %s;ls -l %s' % (os.path.dirname(flag), os.path.basename(flag))
    po = Popen4(cmd)
    out = []
    while po.poll()==-1:
        out.extend(po.fromchild.readlines())

    if not out:
        print '%s: unable to obtain any information' % str(platform)
        continue

    out   = out[0].split()
    month = out[5]
    day   = out[6]
    hour  = out[7].split(':')[0]
    mins  = out[7].split(':')[1]

    theDate = '%s %s@%s:%s' % (day, month, hour, mins)

    isAvailable = False
    if month == todaysDate['month'] and day == todaysDate['day']:
        if int(hour) < int(todaysDate['hour']):
            isAvailable = True
        elif (int(hour) == int(todaysDate['hour']) and (int(mins) < int(todaysDate['mins']))):
            isAvailable = True

    if isAvailable: 
        status = 'READY'
    else:
        status = 'NOT READY'    
    
    print '%s.....%s (%s)' % (str(platform), status, theDate)

