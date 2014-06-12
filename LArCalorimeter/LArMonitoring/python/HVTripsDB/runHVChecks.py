# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import CheckHVStatus_daily
import os
import subprocess as sp

for day in range(14,26):
    date = "2011-03-%02d:07:30:00" % day
    print date
    sp.Popen("python CheckHVStatus_daily.py %s" % date,shell=True)
    
print "done!"
