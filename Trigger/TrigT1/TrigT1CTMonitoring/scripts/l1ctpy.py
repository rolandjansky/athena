#!/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Level 1 Central Trigger crew,
# Johan.Lundberg@cern.ch, CERN, 2009

import os

#os.environ['CORAL_DBLOOKUP_PATH']='/atlas/db/.adm/coral/'
#os.environ['CORAL_AUTH_PATH']='/atlas/db/.adm/coral/'

ORBIT_FREQUENCY = 40.0790e6 / 3564.

#+================================================================
#+
#+ Generic utility functions
#+
#+================================================================

_COOLDBOWNER='' 

_COOLDB="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=DBNAME;user=ATLAS_COOL_READER;password=COOLRED4PRO"

def get_dbid_MONP(owner=False):
   if owner:
       print "NO OWNER ACCOUNT IMPLEMENTED"
       return _COOLDBOWNER.replace("DBNAME","MONP200")
   else:
       return _COOLDB.replace("DBNAME","MONP200")

def get_dbid_COMP(owner=False):
   if owner:
       print "NO OWNER ACCOUNT IMPLEMENTED"
       return _COOLDBOWNER.replace("DBNAME","COMP200")
   else:
       return _COOLDB.replace("DBNAME","COMP200")
   

def get_dbid_TDAQ_COMP(owner=False):
   _COOLDB="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TDAQ;dbname=DBNAME;user=ATLAS_COOL_READER;password=COOLRED4PRO"
   if owner:
       print "NO OWNER ACCOUNT IMPLEMENTED"
       return _COOLDBOWNER.replace("DBNAME","COMP200")
   else:
       return _COOLDB.replace("DBNAME","COMP200")
   

#+-----------------------------------------------
#+ convert text to range list 
def text_to_ranges(instr):
    # from the string, make a list of positive numbers
    # comma separated numbers go in directly
    # ranges are written with - and adds to the list [start,stop] points
    # open ended itevals are allowed, example
    #   1,2,3,10-12        ->  [ 1,2,3,[10,12] ]
    #   1,2,3,10-          ->  [ 1,2,3,[10,None] ]
    #   1,2,3,-10          ->  [ 1,2,3,[None,10] ]
    output=[]
    for j in instr.split(","):
        try:
            o=j.split("-")
            if len(o) == 1:
                output.append(int(o[0]))
            else:
                a=0
                b=0
                if not o[0]:
                    a=None
                else:
                    a=int(o[0])
                if not o[1]:
                    b=None
                else:
                    b=int(o[1])
                output.append([a,b])
        # things we do not get we put there unparsed
        except ValueError:
            output.append(j)
    return output        

#+-----------------------------------------------
#+ expand list of ranges sort + unique
def expandlist(inp,min,max):    
    out=[]
    # make a vector of empty vectors 
    for b in inp:
        if str(b) == b:
            out.append(b)
        else:
            try:
                x0=b[0]
                x1=b[1]
                if x0 == None:
                    x0=min
                if x1 == None:
                    x1=max
            except TypeError:
                x0=b
                x1=b        
            if x1>max or x0<min:
                raise ValueError("Range",b," not in permitted range ",min,max)
            map(out.append,range(x0,x1+1))
    out=list(set(out))
    out.sort()
    for x in out:
        if str(x) == x:
            continue
        if x<min or x>max :
            raise ValueError("Range value ",x," not in permitted range ",min,max)
    return out


#+ -------------------------------------------------------------
#+ Returns specified cool folder or folderset
def getCoolFolder(dbname,foldername):
    try:
        from PyCool import cool
        dbSvc=cool.DatabaseSvcFactory.databaseService()
        db=dbSvc.openDatabase(dbname)
        try:
            fldr=db.getFolderSet(foldername)
        except RuntimeError:
            fldr=db.getFolder(foldername)
        return fldr
    except Exception,e:
        print('Problem opening database or folder',e)
        raise e

#+ -------------------------------------------------------------
#+ Lists contents of a cool folder or folderset
def ListFolder(infolder):
    print("list for folder:",infolder)
    try:
        myspec = infolder.payloadSpecification();
        for myit in myspec:
            print("Record name:",myit.name()," type:",myit.storageType().name())
    except AttributeError,e:
        for x in infolder.listFolders() :
            print '   subfolder: ',x



#+-------------------------------------------------------------
#+ Show what run and LB and optionally what blob times are to be loaded
def showlbandtime(LBs,OPT):
    print "Run:                         ",OPT["--run"]
    print "Lumiblock limits requested:  ",OPT["--lb"],
    if 'L' in str(OPT['--lb']):
        print " ( L<X> means last <X> lumiblocks )"
    else:
        print ""
    
        print "Selected ",len(LBs),"lumiblocks."
        if OPT["--verbose"]:
            for a,b in zip(LBs,TimeRanges):
                print "    Lumiblock :",a,"  TimeRange:",b
                

#+-------------------------------------------------------------
#+ Convert a 4 byte string to an integer (small endian)
def IntegerDecode(ivbytes):        
    # for small endian:
    d,c,b,a=map(ord,list(ivbytes))
    return a*2**24 + b*2**16 + c*2**8 + d


#+-------------------------------------------------------------
#+ make blob out of a vector of numbers
#+ For the packing option, read help(struct)
def blob_pack(numbers,packopts='<f'):
    import struct
    return ''.join(map(lambda j : struct.pack(packopts,j) , numbers))

#+-------------------------------------------------------------
#+ vector of numbers from blob 
#+ For the unpacking option, read help(struct)
def blob_unpack(blob,packopts='<f'):
    import struct
    packlength=len(struct.pack(packopts,3.14))
    result=[]
    for i in range(0,len(blob),packlength):
        i2=i+packlength        
        result.append(struct.unpack(packopts,blob[i:i2])[0])
    return result

#+ -----------------------------------------------------------
#+ applying time zone and daylight time diff
#+

def local2utcDT(intime):
   import time   
   return (time.mktime(time.gmtime(int(intime))[0:-1]+(-1,))-int(intime))



#+================================================================
#+
#+ Generic Cool Reading helper functions
#+ Developed for ctpmon
#+
#+================================================================

#+ -------------------------------------------------------------
#+ Get a list of lumiblocks and a _list_ of (start,stop) times 
#+ for the specified rangelist
#+    rangelist is a list of lumi block ranges. A range
#+    is either a number, a [start,stop] pair or a string starting with L,
#+    for example L32, expands to the last 32 lumiblocks of the run
#+
#+ return list of lbs, and a list of start,stoptimes
#+
def getLbTimesMulti(lbfolder,run,rangelist):
    LBs=[]
    Times=[]
    for r in rangelist:
        # letter L
        if str(r).find("L") >= 0:
            xxx=r.replace("L","")
            selection=int(xxx)
            T1=1
            T2=None
            a,b=getLbTimes(lbfolder,run,T1,T2)
            a=a[-selection:]
            b=b[-selection:]
            map(LBs.append,a)
            map(Times.append,b)
            continue
        T1=r
        T2=r
        try:
            T1=r[0]
            T2=r[1]
        except TypeError:
            pass
        if T1 == None:
            T1=1     # first LB is 1
        a,b=getLbTimes(lbfolder,run,T1,T2)
        map(LBs.append,a)
        map(Times.append,b)
    return LBs,Times



#+ -------------------------------------------------------------
#+ Get a list of lumiblocks one pair of start,stop times
#+                                   lb values are inclusive
def getLbTimes(lbfolder,run,lbstart=1,lbstop=None):
    from PyCool import cool
    if lbstart < 1 :        
        raise ValueError("error: lbstart must be >=1 , not:",lbstart)        
    import operator
    ROV1=(run << 32)+lbstart
    if lbstop == None:
        ROV2=((run+1) << 32)
    else:
        if lbstop < 1 :        
            raise ValueError("error: lbstop must be >=1 or None, not:",lbstop)        
        ROV2=(run << 32)+lbstop
        
    # find out the found Lumiblock numbers
    objs=list(lbfolder.browseObjects(ROV1,ROV2,cool.ChannelSelection.all()))
    LBs=[]
    starttimes=[]
    stoptimes=[]    
    for obj in objs:
        payload=obj.payload()
        starttimes.append(payload["StartTime"])
        stoptimes.append(payload["EndTime"])
        LBs.append(lbstart)
        lbstart+=1
    
    return LBs,zip(starttimes,stoptimes)




#+ -------------------------------------------------------------
#+ for the input blob times, find how much of each lumiblock is contained
#+ returns a list of (luminumber,fraction) tuples
def compareranges(lbs,lbtimes,input_times):
    lllist=[]
    for ll,tt in zip(lbs,lbtimes):
        duration=tt[1]-tt[0]
        startol=tt[0]
        stopol=tt[1]
        if startol<input_times[0]:
            startol=input_times[0]
        if stopol>input_times[1]:
            stopol=input_times[1]
        fraction=(stopol-startol)/(1.0*duration)
        if fraction<0:
            fraction=0
        lllist.append((ll,fraction))
    return lllist

#+ -------------------------------------------------------------
#+ convert times [start,stop] into an approximate lumiblock scale
def time2lb(lbs,lbtimes,tin):
    maxlb=max(lbs)
    lbsEnd=map(lambda x:x+1 if not x==maxlb else x+0.999,lbs)
    lumiaxis=[]
    timeaxis=[]
    for a,b,c in zip(lbs,lbsEnd,lbtimes):
        lumiaxis.append(a)
        lumiaxis.append(b)
        timeaxis.append(c[0])
        timeaxis.append(c[1])    
    def tinterp(tmpval):
        outp=lumiaxis[0]
        if tmpval>=timeaxis[-1]:
            return lumiaxis[-1]
        if tmpval<=timeaxis[0]:
            return lumiaxis[0]
        for l,t,i in zip(lumiaxis,timeaxis,range(len(timeaxis))):
            if i+1 == len(timeaxis):
                # the end
                continue
            if tmpval>=timeaxis[i] and tmpval<=timeaxis[i+1]  :
                if timeaxis[i+1] == timeaxis[i]:
                    fract=0
                else:
                    fract=(tmpval-t)/(1.0*timeaxis[i+1]-timeaxis[i])
                    if lumiaxis[i+1] == lumiaxis[i]:
                        outp=lumiaxis[i]
                    else:
                        outp=l+fract*(lumiaxis[i+1]-lumiaxis[i])/(1.0*lumiaxis[i+1]-lumiaxis[i])
                break
        return outp        
    return tinterp(tin[0]),tinterp(tin[1])








#+-----------------------------------------------------
#+ get the configuration of counters
#
def getcounterConfig(dbstring,ROV1,ROV2,pattern='.*',CounterType='.*'):
    from PyCool import cool
    import re
    folder=getCoolFolder(dbstring,'/TRIGGER/LVL1/CTPInMonitoringMapping')
    objs=folder.browseObjects(ROV1,ROV2,cool.ChannelSelection.all())
    counters=[]
    for obj in objs:
        payload=obj.payload()
        name=payload["ThresholdName"]
        if re.match(CounterType,payload["CounterType"],re.IGNORECASE) and re.match(pattern,name,re.IGNORECASE):
            pl={}
            CounterNr=obj.channelId()
            if payload["CounterType"] == 'CTPMON' :
                CounterNr-=4*3*64
            pl['CounterNr']=CounterNr
            for it in folder.payloadSpecification():                
                pl[it.name()]=(payload[it.name()])
            counters.append(pl)
    if counters:
        def sortkey(sig):
            return [sig['CounterType'],sig["CtpinSlot"],sig["CtpinConnector"],sig["ThresholdBitStart"],sig['ThresholdActive'],sig]
        counters.sort(key=sortkey)

    def namesorthelper(inpname):
        name=inpname.rstrip('0123456789')
        rest=inpname.lstrip(name)
        number="____"+rest
        try:
            number=(int(rest))
        except:
            pass
        return name, number
        
    counters.sort(key=lambda k: [k['CounterType'],k['CtpinSlot'],k['CtpinConnector'],namesorthelper(k['ThresholdName'])])

    return counters


#def findCounternumbers(NameList,CounterType=CTPIN,):
    
def set_rate_palette(dolog=False):

    import sys
    sys.argv[1:]=[]
    import ROOT
    import numpy

    def rootarray(inp):
        return numpy.array(inp, dtype=numpy.double)

    NRGBs = 6;
    NCont = 255;
    if(dolog):    
       stops = rootarray([0., 0.39 ,  0.53 ,  0.7275 ,  0.89 ,  1.0])
    else:
       stops = rootarray([0, 0.02, 0.06, 0.15, 0.30 ,1.0])
       stops = rootarray([0, 0.15, 0.30, 0.50, 0.70 ,1.0])
    red   = rootarray([1, 0.50, 0.87, 1.00, 0.51 ,0])
    green = rootarray([1,  0.75, 1.00, 0.20, 0.00 ,0])
    blue  = rootarray([1,  0.90, 0.12, 0.00, 0.00 ,0])
    ROOT.gStyle.SetNumberContours(NCont);
    ROOT.TColor.CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    ROOT.gStyle.SetLabelFont(63);
    ROOT.gROOT.ForceStyle()

def get_current_run_IS(is_partName='ATLAS'):
   number=-2
   try:
      import ispy

      is_partition = ispy.IPCPartition(is_partName)
      oo=ispy.ISObject(is_partition, 'RunParams.SOR_RunParams','RunParams')
      oo.checkout()
      number=oo.run_number;
   except:
      raise RuntimeError("Error: To get the current RUN number from IS, you must setup the proper ONLINE environment (at Point 1)")
   return number


def get_current_runs(historical_days=5,historical_maxruns=1):
   final=[]
   try:
      from PyCool import cool
      import time
      SOR_fld=getCoolFolder(get_dbid_COMP(),'/TRIGGER/LUMI/LBTIME')
      ROV1=1e9*(time.time()-historical_days*24*3600)
      ROV1=cool.ValidityKey(ROV1)
      ROV2=cool.ValidityKeyMax
      #
      objs=list(SOR_fld.browseObjects(ROV1,ROV2,cool.ChannelSelection.all()))

      runs=map(lambda x:x.get().payload()["Run"] ,objs)
      runs=sorted(list(set(runs)))
      runs.reverse()
      i=0

      for x in runs:
         i+=1
         final.append(int(x))
         if i >= historical_maxruns:
            break
   except BaseException,e:      
      print "Exception trying to find Run numbers:",e
   return final

def get_current_run():
   run=get_current_runs(1,1)
   if run :
      run=run[0]
   else:
      run=-1  
   return run

def backport_counternames(counterdb):
   if counterdb:
      for i in range(len(counterdb)):
         c=counterdb[i]
         if not 'CounterName' in c or not c['CounterName'].strip():
            c['CounterName']=c['ThresholdName']+"_nr"+str(c['CounterNr'])
            counterdb[i]=c

#+ Operates on counters listen in OPT["--counters"]
#+ and adds OPT["--countersnames"]
#+ based on counters database (as returned by l1ctpy.getcounterConfig)
def filter_and_add_counternames(counterdb,OPT):
    if counterdb:       
        ## first make and add CounterName for old entris without it
        ## filter on counter names
        def counterselector(c):
            import re
            if c['CounterNr'] in OPT["--counters"]:
                return True
            for patt in OPT["--counters"]:
                try:
                    int(patt)
                except:
#                    if patt=='-':
#                        return True
                    if ('CounterName' in c and (re.match(str('.*'+patt+'.*'),c['CounterName'],re.IGNORECASE)) or
                        re.match(str('.*'+patt+'.*'),c['ThresholdName'],re.IGNORECASE) ):
                       return True
            return False
        counterdb=filter(counterselector,counterdb)

        if not counterdb:
            print "no counters matched. bye"
            import sys
            sys.exit(2)

        OPT["--counters"]=[]
        OPT["--countersnames"]=[]
        for x in counterdb:
            OPT["--counters"].append(x['CounterNr'])
            if 'CounterName' in x:
               OPT["--countersnames"].append(x['CounterName'])
            else:
               OPT["--countersnames"].append(x['ThresholdName'])
    else:
        print "No counters names found, using numbers"
        counterdb=OPT["--counters"]
        OPT["--counters"]=[]
        OPT["--countersnames"]=[]
        for x in counterdb:
            OPT["--counters"].append(int(x))
            OPT["--countersnames"].append(str('cntr:')+str(x))


# write root object (for example canvas) to open root file
def rootWrite(myTobject,newname):
   oldname=myTobject.GetName()
   myTobject.SetName("canvas_"+newname)
   myTobject.Write()
   myTobject.SetName(oldname)

# Draw LB numbers on top of the histogram
# timeaxis must be the x axis of h, and lbaxis have the same length 
def drawaxis_timeLB(h,timeaxis,lbaxis,log=False,fontfactor=1):
   import sys
   sys.argv[1:]=[]
   import ROOT

   h.GetXaxis().SetTitle("LB");
   h.GetXaxis().SetTitle("UTC Time");
   h.GetXaxis().SetTimeDisplay(1);
   h.GetXaxis().SetTimeFormat("%H:%M"); 
   h.GetXaxis().SetLabelSize(20*fontfactor);

   ROOT.gPad.Update()
   ohigh=ROOT.gPad.GetUymax()
   olow=ROOT.gPad.GetUymin()

   log=ROOT.gPad.GetLogy()
   if not 'TH2' in str(type(h)):
      ohigh=h.GetMaximum()
      olow=h.GetMinimum()
   if(log):
      import math
      y = (math.pow(10,math.log10(ohigh) + 0.021*(math.log10(ohigh)-math.log10(olow))))
   else:
      y = (ohigh + 0.022*(ohigh-olow))

   dy=y-ohigh

   h.GetXaxis()
   t=ROOT.TText();
   t.SetTextSize(0.03*fontfactor);

   rangemax=ROOT.gPad.GetUxmax()
   rangemin=ROOT.gPad.GetUxmin()
   #print rangemin,rangemax
   ilow=len(timeaxis)+1
   ihigh=0
   for i,tt in enumerate(timeaxis):
      if tt>=rangemin and tt<=rangemax :
         ilow=min(ilow,i)
         ihigh=max(ihigh,i)
#   print ilow,ihigh
   timeaxis=timeaxis[ilow:ihigh+1]
   lbaxis=lbaxis[ilow:ihigh+1]
   
   x=min(timeaxis)-0.02 * (max(timeaxis)-min(timeaxis))
   t.SetTextAlign(31);
   t.DrawText(x,y,str("LB:"));

   t.SetTextAngle(90);
   t.SetTextAlign(12);

   # convert to UTC time - NB! this adjusts the time offset to effectively convert local time to UTC (not pretty but leaving as is... /CO)
   h.GetXaxis().SetTimeOffset(ROOT.gStyle.GetTimeOffset()+local2utcDT(timeaxis[0]))
   h.GetXaxis().SetTimeFormat("%H:%M")

   #Number of uniq lbs
   nlb=len(sorted(list(set(map(lambda x:int(x),lbaxis)))))
   ntics=35
   divs=1
   if nlb>50:
       divs=2
   if nlb>100:
       divs=5
   if nlb>300:
       divs=10

   lastlb=0
   lasttime=0
   olddv=0
   started=0

   # find out where to place the last lb label (always drawn)
#   lastindex=0
#   for ll in range(h.GetNbinsX()-1):
#       if int(lbaxis[ll]) != int(lbaxis[ll+1]) :
#           lastindex=ll+1
#   finallb=int(lbaxis[lastindex])
#   finaltime=timeaxis[lastindex]

   finallb=int(lbaxis[-1])
   finaltime=timeaxis[-1]

#   print "final: ",finallb

   # draw lb lables as often as they fit in
   # but limited to the 'divs' modulo (eg print on multiples of 10) 
   t.DrawText(finaltime,y,str(finallb));
        
   for ll in range(len(lbaxis)):
       nextlb=int(lbaxis[ll]) 
       dv=divs*(nextlb//divs)
       nexttime=(timeaxis[ll]) 
       i=nextlb-lastlb
       dt=min(nexttime-lasttime,1.5*(finaltime-nexttime))

       x = timeaxis[ll]#h.GetXaxis().GetBinCenter(ll+1);

       if not started and nextlb>0:                
           t.DrawText(x,y,str(int(lbaxis[ll]))); ## first LB
           started=1
           lastlb=nextlb
           lasttime=nexttime
           continue
       if dt<(timeaxis[-1]-timeaxis[0])/(ntics): continue
       if not i :continue
       if(olddv != dv):
           t.DrawText(x,y,str(dv));
           lastlb=nextlb
           lasttime=nexttime
           odldv=dv




def timeAverage(inTimeRanges,rates,numbins):
    import numpy
    import operator
    factor=len(inTimeRanges)//numbins
    if(factor<2):
        return inTimeRanges,rates
    
    newT=[]
    newR=[]
    tmpR=list(rates[0])
    tmpT=list(inTimeRanges[0])
    totTime=0
    def clearRate(R):
        for k in len(R):
            R[k]=R[k]*0
    for ilong,t in enumerate(inTimeRanges):
        i=int(ilong)
        thisTime=(t[1]-t[0])
        totTime=totTime+thisTime

        tmpR=numpy.array(rates[i])*thisTime + numpy.array(tmpR)

        #print 'iii ' ,i        
        if (i%factor==0) or (i+1) == len(inTimeRanges):
            #print 'iiiiiiix ' ,i
            # divide by total time integrated normalize away the time
            tmpR=map(lambda aa:aa/totTime,tmpR)
            # set end time to the end of this record
            tmpT[1]=t[1]
            newT.append(list(tmpT))
            newR.append(list(tmpR))
            #newT=[inTimeRanges[i][0],inTimeRanges[i][0]]
            # let the next one just continue from where we are
            tmpT[0]=tmpT[1]
            totTime=0
    return newT,newR


# Draws an axis maching that of the input histogram, + space at the edges.
# (done my a 2d histogram, matching the dimensions of the input 1d or 2d histogram
# but with additional space outside the axis edges )
# example (inhist,0.1,0.1,0.1,0.1) adds 10% of padding on each side
def drawspaceaxis(inhist,xlow=0,xhigh=0,ylow=0,yhigh=0):               
   import sys
   sys.argv[1:]=[]
   import ROOT
   import numpy
   newxmin=inhist.GetXaxis().GetBinLowEdge(1)
   newxmax=inhist.GetXaxis().GetBinUpEdge(inhist.GetNbinsX())
   newymin=0
   newymax=0

   numybins=inhist.GetNbinsY()
   numxbins=inhist.GetNbinsX()
   if 'TH1' in str(type(inhist)):
      newymin=inhist.GetMinimum()
      newymax=inhist.GetMaximum()
   else:
      newymin=inhist.GetYaxis().GetBinLowEdge(1)
      newymax=inhist.GetYaxis().GetBinUpEdge(inhist.GetNbinsY())

   
   newxmin,newxmax = newxmin-(newxmax-newxmin)*xlow, newxmax+(newxmax-newxmin)*xlow
   newymin,newymax = newymin-(newymax-newymin)*ylow, newymax+(newymax-newymin)*yhigh              

#   tmp2d = ROOT.TH1F(hex(int(1e10*(numpy.random.uniform())))[2:],"",2,newxmin,newxmax)
   tmp2d = ROOT.TH2F(hex(int(1e10*(numpy.random.uniform())))[2:],"",numybins,newxmin,newxmax,numybins,newymin,newymax)
   
   tmp2d.SetTitle(inhist.GetTitle()) 
   tmp2d.SetMaximum(inhist.GetMaximum())
   tmp2d.SetMinimum(inhist.GetMinimum())
   tmp2d.DrawCopy("A")
   inhist.DrawCopy("AXIS same")
   return tmp2d

#+ -------------------------------------------------------------
#+ Histogram, plot and save BUSY,CTPIN or CTPMON cool data as returned by getXXXrates
#+ inblockLBs should of the format returned by time2lb()
#+ OPT should contains som options about what should be done, read code...
def plot(OPT,inTimeRanges,rates,lbmatches,inblockLBs,opts=[[0,0]],picformat='.png'):

    def setzscale(histo,OPT,scaledown=1.0):
#       maxval=histo.GetMaximum()
       maxval=(histo.GetBinContent(histo.GetMaximumBin()))
       if '--maxzscale' in OPT:
          try:
             maxvallim=((1/scaledown)*float(OPT['--maxzscale']))
             if maxval>maxvallim:
                maxval=maxvallim
          except ValueError:
             pass
#       minval=(histo.GetBinContent(histo.GetMinimumBin()))
       minval=histo.GetMinimum()
       maxval=maxval+0.01*(maxval-minval)
#       minimum=histo.GetMinimum()
       if maxval<=(minval) :
          maxval=((minval)+1e-6)*1.01 
       histo.SetMaximum(maxval)
       if maxval<=(minval*10.01) :
          histo.SetMinimum(maxval/10.1)

#          minimum=histo.GetMinimum()
#          if minimum > histo.GetMaximum() :
#             histo.SetMaximum(minimum*1.5+10) 

    def setzscaleautomin(histo,OPT=None,scaledown=1.0):
       # loop all bins, find smallest non-zero bin
       # and set zscale to that
       smallest=histo.GetBinContent(histo.GetMaximumBin())
       if(smallest<1e-3):
          histo.SetMinimum(1e-3)
       else:
          nbins=histo.GetNbinsZ()*histo.GetNbinsY()*histo.GetNbinsX()
          for b in range(nbins):
             thisv=histo.GetBinContent(b)
             if thisv >1e-4:
                smallest=min(thisv,smallest)
          maxval=histo.GetBinContent(histo.GetMaximumBin())          
          minval=0.98*smallest
          #-0.02*(maxval-smallest)
          if(minval<=1e-4):
             minval=1e-4
          histo.SetMinimum(minval)
          

#    print "WARNIN WARNING, COUNTS IN TERMS OF COUNTS"
#    rateunit="COUNTS"
    rateunit="Rate /Hz"
    print "filling histograms for run: " + str(OPT["--run"])
    basename=str(OPT["--filename"])+str("_")
    for o,a in opts:
        if o in ('-h','--help'):
            usage()
            sys.exit(0)
        if o == '--verbose':                 
            OPT["--verbose"] = True
            OPT["--textsum"] = True
            OPT["--verbose"] = True
    import l1ctpy
    import numpy

    # root batch mode isdefault
    if not '--nobatch' in OPT:
       OPT['--nobatch']=False
    if not OPT['--nobatch']:                   
       import sys
       sys.argv.append( '-b' )

    import sys
    sys.argv[1:]=[]
    import ROOT

    ###################################
    def makeaxis(inRanges):
        out_ax=set()
        map(lambda x:out_ax.add(1.0*x[0]), inRanges)
        map(lambda x:out_ax.add(1.0*x[1]), inRanges)
        out_ax=sorted(list(out_ax))
        return out_ax

    def drawExactSame(hh):
       hh.Draw("same hist E0")
#       hh.Draw("same P0")
    def drawExact(hh):
       hh.Draw("hist E0")
#       hh.Draw("same P0")

    

    timeaxis=makeaxis(inTimeRanges)
    # ns->seconds

    ## print "NOTE, LB AxIS MODE"
    timeaxis=numpy.multiply(timeaxis,1e-9)
    timeDiffUnixToROOT = (365*25+6)*24*3600 # number of seconds from Jan 1, 1970, 00:00:00 to Jan 1, 1995, 00:00:00 (6 leap years)
    timeaxis=numpy.subtract(timeaxis,timeDiffUnixToROOT)

    #print "LB AXIS"
    #timeaxis=makeaxis(inblockLBs)
    #print "LB AXIS"

    lbaxis=makeaxis(inblockLBs)


    
#    print "LB AXIS"
#    timeflow_ax=lbaxis
    timeflow_ax=timeaxis

    timeflow_ax.sort()

    #    ROOT.gStyle.SetPalette(16)
    ROOT.gStyle.SetOptStat(0);
    ROOT.gStyle.SetOptTitle(1);
    ROOT.gStyle.SetTitleBorderSize(0)
    ROOT.gStyle.SetTitleFillColor(0)

    fname=basename + '.root'
    histFile = ROOT.TFile(fname,"RECREATE")

    l1ctpy.set_rate_palette()

    # make an array, and add one more entry at the end (end+1)
    def rootarray(inp):
        inp=list(inp)
        inp.append(inp[-1]+1)
        return numpy.array(inp, dtype=numpy.double)

    timeflow_ax_r=rootarray(timeflow_ax)
    counteraxis_r=rootarray(range(len(OPT["--counters"])))

    ### rate vs lb
    lbtotals=ROOT.TH1D(str("lb_total"),str(""),len(timeflow_ax_r)-1,timeflow_ax_r)
    lbtotals.GetYaxis().SetTitle(OPT["--RateTitle"]);
    lbtotals.SetLineColor(2)
    lbtotals.SetFillColor(11)
    lbtotals.SetFillStyle(1000)
    setzscaleautomin(lbtotals,OPT)
    setzscale(lbtotals,OPT)
#    lbtotals.SetMinimum(0)


    ### rate for lumiblock vs counter
    lbcounters=ROOT.TH2D(str("lb2d"),str(""),len(timeflow_ax_r)-1,timeflow_ax_r,len(counteraxis_r)-1,counteraxis_r)

    lbcounters.GetYaxis().SetTitle("");

    lbcounters1d=[]
    for c,nme in zip(OPT["--counters"],OPT["--countersnames"]):
        h=ROOT.TH1D(nme,nme,len(timeflow_ax_r)-1,timeflow_ax_r)
        h.GetYaxis().SetTitle(OPT["--RateTitle"]);
        h.GetXaxis().SetTitle("Time");
        lbcounters1d.append(h)
            #a lists of 1d histograms, one for each counter

    if not OPT["--bcids"]:
        OPT["--bcids"]=None

    if OPT["--bcids"]:
        print "selected number of bcids: ", len(OPT["--bcids"])
        bcidaxis_r=rootarray(range(OPT["--bcids"][0],1+OPT["--bcids"][-1]))
        #print bcidaxis_r
        bcidcounters1d=[]
        bcidcounters2d=[]

        ## For each counter, a plot Rate vs Bunch
        for c,nme in zip(OPT["--counters"],OPT["--countersnames"]):
            h=ROOT.TH1D(nme+str('bcid'),nme,len(bcidaxis_r)-1,bcidaxis_r)
            h.GetYaxis().SetTitle(rateunit);
            h.GetXaxis().SetTitle("Bunch Id");
            h.GetYaxis().SetTitleOffset(1.7)
            bcidcounters1d.append(h)
            h.GetXaxis().SetLabelSize(20);

        #For each counter, a plot BCID(Y) vs LB(X)
        for c,nme in zip(OPT["--counters"],OPT["--countersnames"]):
            h=ROOT.TH2D(nme+str('bcid_Time'),nme,len(timeflow_ax_r)-1,timeflow_ax_r,len(bcidaxis_r)-1,bcidaxis_r)
            h.GetXaxis().SetTitle("");
            h.GetYaxis().SetTitle("Bunch Id");
            h.GetYaxis().SetTitleOffset(1.7)
            bcidcounters2d.append(h)

#            h.GetXaxis().SetLabelSize(20);

        ### rate for bcid vs counter
        bccounters=ROOT.TH2D(str("bc2d"),OPT["--RateTitle"],len(bcidaxis_r)-1,bcidaxis_r,len(counteraxis_r)-1,counteraxis_r)
        bccounters.GetXaxis().SetTitle("Bunch ID");
        bccounters.GetYaxis().SetTitle(); # counter nr
        bccounters.GetXaxis().SetLabelSize(20);

        ## rate vs bcid
        bcidtotals=ROOT.TH1D(str("bc_total"),str(""),len(bcidaxis_r)-1,bcidaxis_r)
        bcidtotals.GetXaxis().SetTitle("Bunch ID");
        bcidtotals.GetYaxis().SetTitle(OPT["--RateTitle"]);
        bcidtotals.GetYaxis().SetTitleOffset(1.4)
        bcidtotals.SetLineColor(2)
        bcidtotals.SetFillColor(11)
        bcidtotals.SetFillStyle(1000)
        setzscaleautomin(bcidtotals,OPT)
        setzscale(bcidtotals,OPT)

#        bcidtotals.SetMinimum(0)
        bcidtotals.GetXaxis().SetLabelSize(20);

    # works both with the ctpin, ctpmon and busy data
    def fillresults(rates,timewise_ax,timewise_ax_widths,counters,textsum=False,filltotals=True,bcids=None):
         ## Note: Totals is the max of all used
         if len(rates) != len(timewise_ax):
            raise RuntimeError('Rates and (time) axis not of same size')
       
         totaltimeax=sum(timewise_ax_widths)*1.0
         for tt,rr,busy_i in zip(timewise_ax,rates,range(len(rates))):
             ## bunch (center value) for use with filling
             fillb=tt
             per_time_scalefactor=timewise_ax_widths[busy_i]/totaltimeax
             sums=map(lambda _:0,rr)
             for i,sums in enumerate(rr):   # loop over counters
                 try:
                     try:
                         value=float(sums)
                     except TypeError,e:                     
                         value=numpy.sum(sums)
                 except ValueError:
                     value=ord(sums)

# NOTE: plots which do NOT have a time scale must be filled with COUNTS, not rate
# then divide with the total time 
                     
                 lbcounters.Fill(fillb,i+0.1,value)
                 lbcounters.SetBinError(lbcounters.FindBin(fillb,0,0),1e-12);
                 lbcounters1d[i].Fill(fillb,value)
                 lbcounters1d[i].SetBinError(lbcounters1d[i].FindBin(fillb,0,0),1e-12);
                 if bcids:
                     try:
                         for rateloop,thisbc in zip(sums,bcids):  # loop over bcid
                             bccounters.Fill(thisbc+0.1,i+0.1,(1.0*rateloop)*per_time_scalefactor)                     ## Do it here ,timewise_ax_widths
                             bccounters.SetBinError(bccounters.FindBin(thisbc+0.1,0,0),1e-12);
                             bcidcounters1d[i].Fill(thisbc+0.1,(1.0*rateloop)*per_time_scalefactor)                    ## Do it here ,timewise_ax_widths
                             bcidcounters1d[i].SetBinError(bcidcounters1d[i].FindBin(thisbc+0.1,0,0),1e-12);
                             #                    bccounters.Fill(thisbc+0.1,counters[i]+0.1,(1.0*rateloop))
                             bcidtotals.Fill(thisbc+0.1,(1.0*rateloop)*per_time_scalefactor)                           ## Do it here ,timewise_ax_widths
                             bcidtotals.SetBinError(bcidtotals.FindBin(thisbc+0.1,0,0),1e-12);

                             bcidcounters2d[i].Fill(fillb,thisbc+0.1,(1.0*rateloop))


                     except BaseException,e:
                         print "Some type of error with this container ",e

                     


    blobcenters=[]

#1e-9*
    
#    timeax_centers=map(lambda t:1e-9*0.5*(t[0]+t[1]), inTimeRanges)
#    print "WARNING LB MODE"
#    timeax_centers=map(lambda t:0.5*(t[0]+t[1]), inTimeRanges)
    timeax_centers=map(lambda t:1e-9*0.5*(t[0]+t[1]), inTimeRanges)
    timeax_widths=map(lambda t:1e-9*(t[1]-t[0]), inTimeRanges)
#    timeax_widths=map(lambda t:1, inTimeRanges)
    fillresults(rates,timeax_centers,timeax_widths,OPT["--counters"],OPT["--textsum"],True,OPT["--bcids"])
#%    fillresults(rates,timeflow_ax,OPT["--counters"],OPT["--textsum"],True,OPT["--bcids"])

    histFile.Write()
    print "Saved histograms to:",fname
    
    if not OPT["--nodrawHist"] :

        general_minimum=.01
        if "--general_minimum" in OPT:
           general_minimum=OPT["--general_minimum"]

        print "drawing plots for run: " + str(OPT["--run"]) + str(' ')+str(int(1+max(lbaxis)-min(lbaxis))) + str(' lumiblocks')

        if OPT["--bcids"]:
            for hh,h_i in zip(bcidcounters1d,range(len(bcidcounters1d))):
                hh.SetLineWidth(2)
                hh.SetMarkerStyle(0)
                hh.SetLineStyle(1+(h_i)%3)
                hh.SetLineColor(1+(h_i)%7)
                hh.SetLineWidth(2+((h_i)%3))
                hh.SetMarkerColor(1+((h_i)%7))
                hh.SetMarkerSize(.3*(2+(h_i+2)%6))
                hh.SetMarkerStyle(1+(h_i+3)%5)
##                hh.SetMinimum(1e-5)
                setzscaleautomin(hh,OPT)
                if "--general_minimum" in OPT:
                   hh.SetMinimum(OPT["--general_minimum"])
                setzscale(hh,OPT)
#                setzscaleautomin(hh)
#                hh.SetMaximum(hh.GetMaximum()+1e-4)

        for hh,h_i in zip(lbcounters1d,range(len(lbcounters1d))):
            hh.SetLineWidth(2)
            hh.SetMarkerStyle(0)
            hh.SetLineStyle(1+(h_i)%3)
            hh.SetLineColor(1+(h_i)%7)
            hh.SetLineWidth(2+((h_i)%3))
            hh.SetMarkerColor(1+((h_i)%7))
            hh.SetMarkerSize(.2*(2+(h_i+2)%6))
            hh.SetMarkerStyle(1+(h_i+3)%5)    
            #hh.SetMinimum(general_minimum)
            #hh.SetMaximum(hh.GetMaximum()+1e-4)
            setzscaleautomin(hh,OPT)
            if "--general_minimum" in OPT:
               hh.SetMinimum(OPT["--general_minimum"])
            setzscale(hh,OPT)


        title=str(OPT["--run"])

        def setmargins(canvas):
            canvas.SetRightMargin(.12)
            canvas.SetLeftMargin(.20)
            canvas.SetTopMargin(.22)
            canvas.SetBottomMargin(.15)

        def setmargins2d(canvas):
            canvas.SetRightMargin(.12)
            canvas.SetLeftMargin(.20)
            canvas.SetTopMargin(.17)
            canvas.SetBottomMargin(.15)

        def makecanvas(title):
            canvas=ROOT.TCanvas(str(numpy.random.uniform()),title,50,50,1000,700) 
            canvas.SetFillStyle(0)
            canvas.SetFillColor(0)
            canvas.SetBorderSize(0);
            setmargins(canvas)
            return canvas

        def makecanvas2d(canvas,numlab=0):
            scalefactor=1
            numfits=45
            if numlab>numfits:
               scalefactor=1+(numlab-numfits)/(numfits*1.0)
            canvas=ROOT.TCanvas(str(numpy.random.uniform()),title,50,50,1000,700);
            canvas.SetFillStyle(0)
            canvas.SetFillColor(0)
            canvas.SetBorderSize(0);
            setmargins2d(canvas)
            canvas.SetTopMargin(.17/scalefactor**.5)
            canvas.SetRightMargin(.12*(scalefactor**.5))
            canvas.SetLeftMargin(.20/(scalefactor**.5))
#            canvas.SetBottomMargin(.15/scalefactor)
            if scalefactor>1.1:
               canvas.SetCanvasSize(int(1000),int(700*scalefactor))
            
            return canvas

        ylablesize=1.6 / (10+len(OPT["--countersnames"]))
        if ylablesize>0.05: ylablesize=0.05

        #############################################################

        for dolog in [False,True]:
            canvas1d=makecanvas(title) 
            canvas1d.cd()
            setmargins(canvas1d)

            plotmax=0
            plotmin=150
            for hh in lbcounters1d:
                if hh.GetMaximum() > plotmax: plotmax = hh.GetMaximum()
                if hh.GetMinimum() < plotmin: plotmin = hh.GetMinimum()
            plotmax=plotmax*1.03+1e-4

#            lbtotals.SetMaximum(plotmax)
#            setzscaleautomin(lbtotals,OPT)
            lbtotals.SetMaximum(plotmax+1e-8)
            lbtotals.SetMinimum(plotmin*0.99+1e-9)

            drawExact(lbtotals)

            plotleg=ROOT.TLegend(0,.83,1,1)
            plotleg.SetNColumns(1+len(OPT["--counters"])//4)
            plotleg.SetFillStyle(0)
            plotleg.SetBorderSize(0);
            plotleg.Draw()

            lbtotals.SetLineColor(0)

            for hh,h_i in zip(lbcounters1d,range(len(lbcounters1d))):
                drawExactSame(hh)
                #.Draw("same hist P0")
                #hh.Draw("same hist E0")
                plotleg.AddEntry(hh,str(OPT["--countersnames"][h_i]),"lp")


#            lbtotals.SetMinimum(0)
            plotleg.Draw()

#            lbtotals.SetMaximum(plotmax)
#            lbtotals.SetMinimum(general_minimum*(-1+2*dolog))
#            plotleg.Draw()

            canvas1d.SetLogy(dolog)
            drawaxis_timeLB(lbtotals,timeaxis,lbaxis,dolog)
            if not 'LB1D' in OPT['--nodraw']:
                if dolog and not 'Log' in OPT['--nodraw']:
                   canvas1d.Print(basename+str("lb_log"+str(picformat)))
                   rootWrite(canvas1d,basename+str("lb_log"))
                if not dolog and not 'Lin' in OPT['--nodraw']:
                   canvas1d.Print(basename+str("lb"+str(picformat)))
                   rootWrite(canvas1d,basename+str("lb"))

        canvas2dlog=makecanvas2d(title,len(OPT["--countersnames"]))
        canvas2dlog.cd()        
        canvas2dlog.SetLogz(False)
        setzscaleautomin(lbcounters,OPT)
        if "--general_minimum" in OPT:
           lbcounters.SetMinimum(OPT["--general_minimum"])
        setzscale(lbcounters,OPT)

#        lbcounters.SetMinimum(0)
#        lbcounters.SetMaximum(plotmax)



# this is a way to hardcode ctpmon max scale
#        if 'CTPMON' in basename:           

        if 'CTPMON' in basename:           
           setzscale(lbcounters,OPT)
        else:
           if 'BUSY' in basename or 'busy' in basename:
              lbcounters.SetMaximum(100);
              lbcounters.SetMinimum(0.);

        lbcounters.SetFillColor(1);


        for i,name in enumerate(OPT["--countersnames"]):
            lbcounters.GetYaxis().SetBinLabel(i+1,name)            
            lbcounters.GetYaxis().SetTitleOffset(1.4)
            lbcounters.GetYaxis().SetLabelSize(ylablesize)
            if OPT["--bcids"]:
                bccounters.GetYaxis().SetBinLabel(i+1,name)
                bccounters.GetYaxis().SetTitleOffset(1.4)
                bccounters.GetYaxis().SetLabelSize(ylablesize)

        lbcounters.SetTitle(OPT["--RateTitle"])
        lbcounters.Draw("colz")

        drawaxis_timeLB(lbcounters,timeaxis,lbaxis)

        if not 'Lin' in OPT['--nodraw']:
           canvas2dlog.Print(basename+str("lb_2D"+str(picformat)))
           rootWrite(canvas2dlog,basename+str("lb_2D"))

        ###########################

        canvas2d=canvas2dlog# ;#makecanvas(title) 
#        canvas2d.cd()

#        setzscaleautomin(lbcounters,OPT)
#        if "--general_minimum" in OPT:
#           lbcounters.SetMinimum(OPT["--general_minimum"])
#        setzscale(lbcounters,OPT)                           


#        lbcounters.SetMinimum(general_minimum)
#        lbcounters.SetMaximum(plotmax)
        if 'BUSY' in basename or 'busy' in basename:
           lbcounters.SetMaximum(100);
           lbcounters.SetMinimum(0.1);
        else:
           setzscaleautomin(lbcounters,OPT)
        if 'CTPMON' in basename or 'CTPIN' in basename:
           setzscale(lbcounters,OPT)
                   
        set_rate_palette(1)
        canvas2d.SetLogz(True)
        lbcounters.Draw("colz")
#        canvas2d.Print(basename+str("lb_2D_log.eps"))
        drawaxis_timeLB(lbcounters,timeaxis,lbaxis)
        if not 'Log' in OPT['--nodraw']:
           canvas2d.Print(basename+str("lb_2D_log"+str(picformat)))
           rootWrite(canvas2d,basename+str("lb_2D_log"))

        if OPT["--bcids"]:
            canvas2dlog=makecanvas2d(title,len(OPT["--countersnames"]))

            canvas2dlog.cd()
            set_rate_palette(0)
            canvas2dlog.SetLogz(False)

            setzscaleautomin(bccounters,OPT)
#            bccounters.SetMinimum(1e-4)
            setzscale(bccounters,OPT)

               
            bccounters.GetYaxis().SetTitle(''); # Counter names

            drawspaceaxis(bccounters,0.02,0.02,0.02,0.02)
#            bccounters.Draw("AXIS same")
            bccounters.Draw("colz same")
            if not 'Lin' in OPT['--nodraw']:
               canvas2dlog.Print(basename+str("bcid_2D"+str(picformat)))
               rootWrite(canvas2dlog,basename+str("bcid_2D"))
            set_rate_palette(1)
            canvas2dlog.SetLogz(True)
            if not 'Log' in OPT['--nodraw']:
               canvas2dlog.Print(basename+str("bcid_2D_log"+str(picformat)))
               rootWrite(canvas2dlog,basename+str("bcid_2D_log"))

            if not 'allBCID' in OPT['--nodraw']:
              for hh,h_i in zip(bcidcounters2d,range(len(bcidcounters1d))):
                 canvas2d=canvas2dlog
                 setzscaleautomin(hh,OPT)
                             #hh.SetMinimum(1e-9)
                 #hh.SetMinimum(general_minimum+1e-6)
                 set_rate_palette(1)
                 setzscale(hh,OPT)

                 canvas2d.SetLogz(True)
                 hh.Draw("colz")
                 drawaxis_timeLB(hh,timeaxis,lbaxis,True)
                 tmpax2=drawspaceaxis(hh,0.02,0.02,0.02,0.02)
                 hh.Draw("colz same")
                 drawaxis_timeLB(hh,timeaxis,lbaxis,True)
                 rootWrite(canvas2d,basename+str("lb_2D_bcid_")+OPT["--countersnames"][h_i]+"_log")
                 canvas2d.Print(basename+str("lb_2D_bcid_")+OPT["--countersnames"][h_i]+"_log"+str(picformat))


                 #################

#                 canvas2d.SetLogz(True)
#                 drawaxis_timeLB(hh,timeaxis,lbaxis,True)
#                 canvas2d.Update()
                 canvas2d.SetLogz(False)
                 tmpax=drawspaceaxis(hh,0.02,0.02,0.02,0.02)
                 hh.Draw("colz same")
                 drawaxis_timeLB(hh,timeaxis,lbaxis)
                 rootWrite(canvas2d,basename+str("lb_2D_bcid_")+OPT["--countersnames"][h_i]+"_lin")
                 canvas2d.Print(basename+str("lb_2D_bcid_")+OPT["--countersnames"][h_i]+"_lin"+str(picformat))

                 ## log

                 
            ###################################################

            if True:

               canvas1d=makecanvas(title) 
               canvas1d.cd()
               setmargins(canvas1d)

               setzscaleautomin(bcidtotals,OPT)
               setzscale(bcidtotals,OPT)

#               drawExact(bcidtotals)
               drawspaceaxis(bcidtotals,0.02,0.02,0.02,0.02)
               drawExactSame(bcidtotals)

#               drawExact(bcidtotals)

   #            bcidtotals.Draw()

               plotleg=ROOT.TLegend(0,.83,1,1)
               plotleg.AddEntry(bcidtotals,"total","F")
               plotleg.SetNColumns(1+len(OPT["--counters"])//4)
               plotleg.SetFillStyle(0)
               plotleg.SetBorderSize(0);
               plotleg.Draw()

               plotmax=bcidtotals.GetMaximum()+1e-4
               plotmin=bcidtotals.GetMinimum()
               canvas1d.SetLogy(False)
               plotleg.Draw()

#               if "--general_minimum" in OPT:
#                  bcidtotals.SetMinimum(OPT["--general_minimum"])

               if not 'Lin' in OPT['--nodraw']:
                  canvas1d.Print(basename+str("bcid_sum"+str(picformat)))
                  rootWrite(canvas1d,basename+str("bcid_sum"))

               canvas1d.SetLogy(True)

               setzscaleautomin(bcidtotals)
#               bcidtotals.SetMinimum(1e-5)
               if "--general_minimum" in OPT:
                  bcidtotals.SetMinimum(OPT["--general_minimum"])

               bcidtotals.SetMaximum(bcidtotals.GetBinContent(bcidtotals.GetMaximumBin())+1e-4)
               plotleg.Draw()
               if not 'Log' in OPT['--nodraw']:
                  canvas1d.Print(basename+str("bcid_sum_log"+str(picformat)))
                  rootWrite(canvas1d,basename+str("bcid_sum_log"))
       #        canvas1d.Print(basename+str("bcid_sum_log.eps"))

               bcidtotals.SetLineColor(0)

#               setzscaleautomin(bcidtotals)
#               bcidtotals.SetMinimum(0)
               plotleg.Draw()

               canvas1di=makecanvas(title) 
               canvas1di.cd()
               setmargins(canvas1di)

               if not 'allBCID' in OPT['--nodraw']:
                  for hh,h_i in zip(bcidcounters1d,range(len(bcidcounters1d))):
                      canvas1d.cd()
                      plotleg.AddEntry(hh,str(OPT["--countersnames"][h_i]),"lp")
                      hh.Draw("hist same")

                      canvas1di.cd()

                      lc=hh.GetLineColor()
                      lw=hh.GetLineWidth()
                      ls=hh.GetLineStyle()
                      lfs=hh.GetFillStyle()
                      lfc=hh.GetFillColor()
                      
                      hh.SetLineWidth(3)
                      hh.SetLineColor(1)
                      hh.SetLineStyle(1)
                      hh.SetFillColor(2)
                      hh.SetFillStyle(1000)
                      
                      ms=hh.GetMarkerStyle()
                      hh.SetMarkerStyle(0)
                      drawspaceaxis(hh,0.02,0.02,0.02,0.02)
                      drawExactSame(hh)

#                      hh.Draw("hist")
                      canvas1di.SetLogy(0)
                      if not 'Lin' in OPT['--nodraw']:
                         canvas1di.Print(basename+str("bcid_"+OPT["--countersnames"][h_i]+"_"+str(picformat)))
                         #rootWrite(canvas1di,basename+str("bcid_"+OPT["--countersnames"][h_i]))
                      #hh.SetMinimum(hh.GetMinimum()+1e-6)
                      #if (hh.GetMaximum()<12*hh.GetMinimum()):
                      #   hh.SetMaximum(12*hh.GetMinimum())
                      
                      canvas1di.SetLogy(1)
                      if not 'Log' in OPT['--nodraw']:
                         canvas1di.Print(basename+str("bcid_"+OPT["--countersnames"][h_i]+"_log"+str(picformat)))
                         #rootWrite(canvas1di,basename+str("bcid_"+OPT["--countersnames"][h_i]+'_log'))

                      hh.SetLineWidth(lw)
                      hh.SetLineColor(lc)
                      hh.SetLineStyle(ls)
                      hh.SetFillStyle(lfs)
                      hh.SetFillColor(lfc)
                      hh.SetMarkerStyle(ms)

               canvas1d.SetLogy(False)
               bcidtotals.SetMaximum(bcidtotals.GetMaximum()*1.05+1e-4)
               plotleg.Draw()
               if not 'Lin' in OPT['--nodraw']:
                  canvas1d.Print(basename+str("bcid"+str(picformat)))
                  #rootWrite(canvas1d,basename+str("bcid_"))

               if not 'Log' in OPT['--nodraw']:
                  canvas1d.SetLogy(True)
#                  bcidtotals.SetMinimum(1e-5)
                  if "--general_minimum" in OPT:
                     bcidtotals.SetMinimum(OPT["--general_minimum"])
#                     bcidtotals.SetMaximum(bcidtotals.GetMaximum()+1e-4)
                     plotleg.Draw()
                  canvas1d.Print(basename+str("bcid_log"+str(picformat)))
                  #rootWrite(canvas1d,basename+str("bcid_log"))
     

    histFile.Close()

