#!/usr/bin/env python
# coolTimeStampUTCToIOV <UTC time string>
# convert a UTC string time to a number of seconds / ns since 1970

import sys,time,calendar

if len(sys.argv)<2:
    print "Usage: ",sys.argv[0]," UTC time in form 2007-05-25:14:01:00"
    sys.exit(-1)

sinput=sys.argv[1]
try:
    ts=time.strptime(sinput+'/UTC','%Y-%m-%d:%H:%M:%S/%Z')
    tsec=int(calendar.timegm(ts))
    print "UTC time %s = %i sec or %i ns" % (sinput,tsec,tsec*1000000000L)
except ValueError: 
    print "ERROR in time specification, use e.g. 2007-05-25:14:01:00"
