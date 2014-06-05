#!/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Level 1 Central Trigger crew,
# Johan.Lundberg@cern.ch, CERN, 2009

if __name__ == "__main__":
    print "Warning:"
    print "   The busy_py module is meant to be executed."
    print "      try busy_cool_reader.py "
    print "       or busy_cool_plot.py "

# this makes it easy to move functions and constants
# from here to l1ctpy
from l1ctpy import *

#+================================================================
#+
#+ BUSY specific functions for reading and parsing data
#+
#+ Note, most is als used for CTPIN data
#+
#+================================================================

#+ -------------------------------------------------------------
#+ Factory function, returns full information for the specified ranges.
#+ produced function Returns a list 
#+   index goes over the selected conters
def ReadBusyRecord_detailed_Factory(counters,verbose=False):
    import struct
    if verbose:
        print "Contructing record calculator for:"
        print "  counters   : ",counters
    if(len(counters)<1):
        raise ValueError("can't handle empty counter range")
    def Record_calc(payload,downscaling=1.0):
        " Reads a record at given counter. Returns selected counter values. "
        # output vector of empty vectors
        record=[]        
        for counter in counters:
            record.append(payload[counter])
        return record
    return Record_calc


#+ -------------------------------------------------------------
#+ returns requested rates 
#+ times should be a list of (start,stop) time tuples, eg from getLbTimes
#+ Output is a list of (start,stop) tuples for the record ROVs
#+ and the correspondig output from the record calculator
#+  when strict=True accept only records completely within
#+  the lb range 
def getBusyrates(folder,times,recordcalc,strict=True):
    from PyCool import cool
    records=[]
    StopTimes=[]
    StartTimes=[]        
    oldpersentage=""
    for t,t_i in zip(times,range(len(times))):
        dir(cool)
        dir(folder)
        obj=folder.browseObjects(t[0],t[1],cool.ChannelSelection.all())
        persentage=((100.0*(t_i+1))/len(times))//10
        if not persentage == oldpersentage:
            print 'reading counter records, %3.0f %%' % (100*(t_i+1)/len(times))
            oldpersentage=persentage
        for o in obj:
            payload=o.payload()
            starttime=o.since()
            if starttime in StartTimes :
                pass
            else:
                if strict:
                    if starttime<times[0][0] or o.until()>times[-1][1]:
                        continue
                StartTimes.append(starttime)
                StopTimes.append(o.until())                
                thissum=recordcalc(payload)
                records.append(thissum)             
    return zip(StartTimes,StopTimes),records

#+================================================================
#+
#+ BUSY specific functions for saving, loading, histogramming 
#+ and drawing record data
#+
#+================================================================

#+ -------------------------------------------------------------
#+ Display BUSY cool data as returned by getINrates
#+ inblockLBs should of the format returned by time2lb()
def showresults(inTimeRanges,rates,inblockLBs,counters,textsum=False,textbc=False):
    import operator
    print "printing results:" 
    for tt,rr,busy_i in zip(inTimeRanges,rates,range(len(rates))):
        print "counter data time range: ",tt ," seconds: ", '%.1f' % ((tt[1]-tt[0])*1e-9)
        print "in terms of Lumiblocks:", '%.1f' % inblockLBs[busy_i][0] ,'to', '%.1f' % inblockLBs[busy_i][1]
        sums=map(lambda _:0,rr)
        for r,i in zip(rr,range(len(rr))):
            sums[i]=reduce(operator.add,r)
            if not textsum:
                if textbc:
                    print "  counter:",counters[i],", rate: ",r
                print "  counter:",counters[i],", sum:  ",sums[i]
        print "    total:", reduce(operator.add,sums)

#+ -------------------------------------------------------------
#+ Same the results of the record reading to a python picke object file
#+ OPT should contains some fields, read below.
def savefile(OPT,inTimeRanges,rates,lbmatches,inblockLBs):
    import pickle
    try:
        fname=OPT["--filename"] + '_py.obj'
        dumpfile=open(fname,'w')
        contents={}
        contents['inTimeRanges']=inTimeRanges
        contents['rates']=rates
        contents['lbmatches']=lbmatches
        contents['inblockLBs']=inblockLBs
        contents['OPT']=OPT
        pickle.dump(contents,dumpfile)
        dumpfile.close()
        print "Saved results to:",fname
    except:
        print "File save failed. Filename:",fname


#+ -------------------------------------------------------------
#+ Histogram and plot the contents of a saved python pickle file
#  options:
#    print "  --outbase | -o <outname>  path base for output files"     
#    print "  --nodraw                    make histograms, do not draw"
#    print "  --verbose                   Display full data"
def plotfile(fname,opts=[[0,0]]):
    import pickle
    dumpfile=open(fname,'r')
    contents=pickle.load(dumpfile)
    inTimeRanges=contents['inTimeRanges']
    rates=contents['rates']
    lbmatches=contents['lbmatches']
    OPT=contents['OPT']
    inblockLBs=contents['inblockLBs']
    dumpfile.close()
    OPT["--nodrawhist"]=False
    OPT["--nomakehist"]=False
    for o,a in opts:
        if o in ('--nodrawHist'):
            OPT["--nodrawHist"]=True
        if o in ('--outbase','-o'):
            OPT["--filename"]=a
        if o in ('--text'):
            OPT["--text"]=True

    if "text" in OPT:
        OPT["--textbc"]=True
        showresults(inTimeRanges,rates,inblockLBs,OPT["--counters"],OPT["--textsum"])
    
    plot(OPT,inTimeRanges,rates,lbmatches,inblockLBs,opts)
    
