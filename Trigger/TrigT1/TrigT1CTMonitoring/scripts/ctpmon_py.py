#!/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Level 1 Central Trigger crew,
# Johan.Lundberg@cern.ch, CERN, 2009

if __name__ == "__main__":
    print "Warning:"
    print "   The ctpmon_py module is meant to be executed."
    print "      try ctpmon_cool_reader.py "
    print "       or ctpmon_cool_plot.py "

# this makes it easy to move functions and constants
# from here to l1ctpy
from l1ctpy import *

#+================================================================
#+
#+ CTPMON specific functions for reading and parsing blob data
#+
#+================================================================


#+ -------------------------------------------------------------
#+ Factory function, returns full information for the specified ranges.
#+ produced function Returns a list of lists (a matrix)
#+   first index goes over the selected ptcs
#+   seconds index goes over the selected bunches
def ReadMonBlob_detailed_Factory(ptcs=range(160),bunches=range(3564),verbose=False):
    if verbose:
        print "Contructing blob calculator for:"
        print "  ptcs   : ",ptcs
        print "  bunches: ",bunches
    if(len(ptcs)<1):
        raise ValueError("can't handle empty counter range")
    def Blobcalc(blob,downscaling=1.0):
        " Reads a monitoring CTPMON blob at given ptc and bunch.\
          Returns all counter values. "      
        # output vector of empty vectors
        sums=map(lambda _: [] ,ptcs)

        i=0
        for ptc in ptcs:
            for bunch in bunches:
                indx=(ptc*3564+bunch)*4
                blob.seek(indx)
                here=sums[i]
                sums[i].append((1.0/downscaling)*IntegerDecode(blob.read(4)))
            i+=1                     
        return sums
    return Blobcalc

#+ -------------------------------------------------------------
#+ returns requested rates 
#+ times should be a list of (start,stop) time tuples, eg from getLbTimes
#+ Output is a list of (start,stop) tuples for the CTPMON blob ROVs
#+ and the correspondig output from the blob calculator
def getMONrates(monfolder,times,blobcalc):
    from PyCool import cool
    blobs=[]
    StopTimes=[]
    StartTimes=[]        
    oldpersentage=""
    for t,t_i in zip(times,range(len(times))):
        dir(cool)
        dir(monfolder)
        obj=monfolder.browseObjects(t[0],t[1],cool.ChannelSelection.all())
        persentage='reading CTPMON blobs, %3.0f %%' % (100*(t_i+1)/len(times))
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
                turnnr=payload["TurnNumber"]
                errorbits=payload["ErrorBits"]
                if errorbits>0:
                    print "****Error, non zero errorbits:",errorbits
                livetime=turnnr/ORBIT_FREQUENCY
                thissum=blobcalc(payload["CounterBlob"],livetime)
#                thissum=blobcalc(payload["CounterBlob"],1)
                blobs.append(thissum)             
    return zip(StartTimes,StopTimes),blobs
#print "WARNING WARNING -- CTPMON IS IN TERMS OF COUNTS NOW"


#+================================================================
#+
#+ CTPMON specific functions for saving, loading, histogramming 
#+ and drawing blob data
#+
#+================================================================

#+ -------------------------------------------------------------
#+ Display CTPMON cool data as returned by getMONrates
#+ monblockLBs should of the format returned by time2lb()
def showresults(monTimeRanges,rates,monblockLBs,counters,bcids,textsum=False,textbc=False):
    import operator
    print "printing results:" 
    print "bunch list: ",bcids
    for tt,rr,ctpmon_i in zip(monTimeRanges,rates,range(len(rates))):
        print "CTPMON data time range: ",tt ," seconds: ", '%.1f' % ((tt[1]-tt[0])*1e-9)
        print "in terms of Lumiblocks:", '%.1f' % monblockLBs[ctpmon_i][0] ,'to', '%.1f' % monblockLBs[ctpmon_i][1]
        sums=map(lambda _:0,rr)
        for r,i in zip(rr,range(len(rr))):
            sums[i]=reduce(operator.add,r)
            if not textsum:
                if textbc:
                    print "  counter:",counters[i],", rate: ",r
                print "  counter:",counters[i],", sum:  ",sums[i]
        print "    total:", reduce(operator.add,sums)

#+ -------------------------------------------------------------
#+ Same the results of the blob reading to a python picke object file
#+ OPT should contains some fields, read below.
def savefile(OPT,monTimeRanges,rates,lbmatches,monblockLBs):
    import pickle
    try:
        fname=OPT["--filename"] + '_py.obj'
        dumpfile=open(fname,'w')
        contents={}
        contents['monTimeRanges']=monTimeRanges
        contents['rates']=rates
        contents['lbmatches']=lbmatches
        contents['monblockLBs']=monblockLBs
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
    monTimeRanges=contents['monTimeRanges']
    rates=contents['rates']
    lbmatches=contents['lbmatches']
    OPT=contents['OPT']
    monblockLBs=contents['monblockLBs']
    dumpfile.close()
    OPT["--drawhist"]=True
    OPT["--makehist"]=True
    OPT["--nodraw"]='no'
    for o,a in opts:
        if o in ('--nodraw'):
            OPT["--drawhist"]=False
        if o in ('--outbase','-o'):
            OPT["--filename"]=a
        if o in ('--text'):
            OPT["--text"]=True
        if o in ('--nodraw'):
            OPT["--nodraw"]=OPT["--nodraw"]+str(a)

    if "text" in OPT:
        OPT["--textbc"]=True
        showresults(monTimeRanges,rates,monblockLBs,OPT["--counters"],OPT["--bcids"],OPT["--textsum"],OPT["--textbc"])
    
    plot(OPT,monTimeRanges,rates,lbmatches,monblockLBs,opts)
    

