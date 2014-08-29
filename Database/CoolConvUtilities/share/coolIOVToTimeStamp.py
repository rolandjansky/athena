#!/usr/bin/env python
# coolIOVtoTimeStamp <iovtime>
# convert a 63 bit COOL IOV value to a time stamp (in GMT and local)

import sys,time

if len(sys.argv)<2:
    print "Usage: ",sys.argv[0]," <63 bit IOV value or value in seconds>"
    sys.exit(-1)

# interpret either as a number in seconds or nano-seconds depending on length
sinput=sys.argv[1]
if len(sinput)>10:
    tsec=int(float(sinput)/1.E9)
else:
    tsec=int(sinput)
utctime=time.asctime(time.gmtime(tsec))
loctime=time.asctime(time.localtime(tsec))
print "Time of %i sec = %s UTC or %s local" % (tsec,utctime,loctime)
