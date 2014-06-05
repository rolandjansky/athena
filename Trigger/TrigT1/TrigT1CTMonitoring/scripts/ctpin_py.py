#!/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Level 1 Central Trigger crew,
# Johan.Lundberg@cern.ch, CERN, 2009

if __name__ == "__main__":
    print "Warning:"
    print "   The ctpin_py module is meant to be executed."
    print "      try ctpin_cool_reader.py "
    print "       or ctpin_cool_plot.py "

# this makes it easy to move functions and constants
# from here to l1ctpy
from l1ctpy import *

# note, we use also busy_py.py 
# for most things and rededine only a few methods
from busy_py import *

#+================================================================
#+
#+ CTPIN specific functions for reading and parsing blob data
#+
#+================================================================

#+ -------------------------------------------------------------
#+ Factory function, returns full information for the specified ranges.
#+ produced function Returns a list 
#+   index goes over the selected conters
def ReadInBlob_detailed_Factory(counters=range(768),verbose=False):
    import struct
    if verbose:
        print "Contructing blob calculator for:"
        print "  counters   : ",counters
    if(len(counters)<1):
        raise ValueError("can't handle empty counter range")
    def Blobcalc(blob,downscaling=1.0):
        " Reads a  blob at given counter. Returns selected counter values. "
        # output vector of empty vectors
        sums=[]        
        for counter in counters:
            indx=(counter)*4            
            blob.seek(indx)
            sums.append(struct.unpack('<f',blob.read(4)))
        return sums
    return Blobcalc


#+ -------------------------------------------------------------
#+ returns requested rates 
#+ times should be a list of (start,stop) time tuples, eg from getLbTimes
#+ Output is a list of (start,stop) tuples for the blob ROVs
#+ and the correspondig output from the blob calculator
def getINrates(folder,times,blobcalc):
    from PyCool import cool
    blobs=[]
    StopTimes=[]
    StartTimes=[]        
    oldpersentage=""
    for t,t_i in zip(times,range(len(times))):
        dir(cool)
        dir(folder)
        obj=folder.browseObjects(t[0],t[1],cool.ChannelSelection.all())
        persentage='reading CTPIN blobs, %3.0f %%' % (100*(t_i+1)/len(times))
        if not persentage == oldpersentage:
            print persentage
            oldpersentage=persentage
        for o in obj:
            payload=o.payload()
            starttime=o.since()
            if starttime in StartTimes :
                pass
            else:
                StartTimes.append(starttime)
                StopTimes.append(o.until())
                of=payload["Overflows"]
                pe=payload["ParityErrors"]
                # parity is expert stuff only - not all clients
                # compute their parity
                if (False): 
                    # one bit per cable. Eg. cable 1 and 2 gives 0x6 
                    if of : print "****Error, overflows:   7:0x%x,8:0x%x,9:0x%x"% (pe&15,(pe>>4)&15,(pe>>8)&15)
                    if pe : print "****Error, parityErrors: 7:0x%x,8:0x%x,9:0x%x"% (pe&15,(pe>>4)&15,(pe>>8)&15)
                thissum=blobcalc(payload["Rates"],1.0)
                blobs.append(thissum)             
    return zip(StartTimes,StopTimes),blobs

