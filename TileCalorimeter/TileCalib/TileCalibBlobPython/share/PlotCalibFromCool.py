#!/bin/env python

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# PlotCalibFromCool.py
# Using Part of ReadCalibfromCool.py to plot constants
# Tigran Mkrtchyan 2017-06-15



import getopt,sys,os,string,math,re
os.environ['TERM'] = 'linux'

def usage():
    print "How to use: ",sys.argv[0]," [OPTION] ... "
    print "Plots the TileCal constants from various schemas / folders / tags as a function of run number/ lumiblock"
    print ""
    print "-h, --help      shows this help"
    print "-T, --tree      save all in a tree"
    print "-o, --opt=      specify plotting option: line, noline or graph, bin, dist or 2d, default is dist"
    print "-P, --print     print all the values on the console"
    print "-n, --norm      normalize everything to the first point"
    print "-f, --folder=   specify status folder to use f.i. /TILE/OFL02/CALIB/CIS/LIN "
    print "-t, --tag=      specify tag to use, f.i. UPD1 or UPD4 or full suffix like RUN2-HLT-UPD1-00"
    print "-b, --begin=    specify the starting run number"
    print "-e, --end=      sepcify the last run number"
    print "-r, --run=      specify fixed run number, will plot constants as a function of lumi"
    print "-l, --lumi=     specify lumi block number, default is 0"
    print "-p, --part=     specify which partition to plot, only for --plotopt=2d"
    print "-I, --modmin=   specify minimal module to use for 2D plot, default is 1"
    print "-A, --modmax=   specify maximal module to use for 2D plot, default is 64"
    print "-N, --chmin=    specify minimal channel to use for 2D plot, default is 0"
    print "-X, --chmax=    specify maximal channel to use for 2D plot, default is 47"
    print "-Y, --gmin=     specify minimal gain to use for print option, default is 0"
    print "-Z, --gmax=     specify maximal gain to use for print option, default is 1"
    print "-m, --module=   specify module to use for 1D plots, default is LBA01"
    print "-c, --chan=     specify channel to use for 1D plots, default is 0"
    print "-g, --gain=, -a, --adc=  specify adc(gain) to use, default is 0 (low gain)"
    print "-v, --val=,  -i, --ind=  specify index of value to use, default is 0 "
    print "-C, --cut=      specify additional cut on displayed values"
    print "-s, --schema=   specify schema to use, like 'COOLONL_TILE/CONDBR2' or 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2' or tileSqlite.db"


letters = "hr:l:s:t:f:a:g:b:e:o:Pp:Tv:i:m:c:I:A:N:X:Y:Z:C:n"
words = ["help","run=","lumi=","schema=","tag=","folder=","adc=","gain=","print","module=","opt=","chan=","begin=","end=","tree","val=","ind=","part=","modmin=","modmax=","chmin=","chmax=","gmin=","gmax=","cut=","norm"]
try:
    options,args = getopt.getopt(sys.argv[1:],letters,words)
except getopt.GetoptError, err:
    print
    print str(err)
    print
    usage()
    sys.exit(2)



# defaults
schema = 'COOLOFL_TILE/CONDBR2'
folderPath =  "/TILE/OFL02/CALIB/CIS/LIN"
tag = "UPD4"
line = False
noline = False
vsbin = False
plotopt= "dist"
save_tree = False
print_msg = False
opt2d = False
partname = "LBA"
modulename = "LBA01"
modmin=0
modmax=9999
chanmin=0
chanmax=9999
gainmin=0
gainmax=0
mod_n = 0
chan_n = 0
gain_n = 0
val_n = 0
begin = 0
end = 0
runNum = 0
lumiNum = 0
one_run = False
multi = False
norm = False
cut = None

for o, a in options:
    if o in ("-f","--folder"):
        folderPath = a
    elif o in ("-t","--tag"):
        tag = a
    elif o in ("-s","--schema"):
        schema = a
    elif o in ("-b","--begin"):
        begin = int(a)
    elif o in ("-e","--end"):
        end = int(a)
    elif o in ("-r","--run"):
        runNum = int(a)
        one_run = True
    elif o in ("-l","--lumi"):
        lumiNum = int(a)
    elif o in ("-m","--module"):
        modulename = a
    elif o in ("-c","--chan"):
        chan_n = int(a)
        chanmin = chan_n
        chanmax = chan_n
    elif o in ("-a","--adc","-g","--gain"):
        gain_n = int(a)
        gainmin = gain_n
        gainmax = gain_n
    elif o in ("-v","--val","-i","--ind"):
        val_n = int(a)
    elif o in ("-p","--part"):
        partname = a
        modulename = partname+"00"
    elif o in ("-I","--modmin"):
        modmin = int(a)
    elif o in ("-A","--modmax"):
        modmax = int(a)
    elif o in ("-N","--chmin"):
        chanmin = int(a)
        chan_n = chanmin
    elif o in ("-X","--chmax"):
        chanmax = int(a)
    elif o in ("-Y","--gmin"):
        gainmin = int(a)
        gain_n = gainmin
    elif o in ("-Z","--gmax"):
        gainmax = int(a)
    elif o in ("-o","--opt"):
        plotopt = a
    elif o in ("-C","--cut"):
        cut = a
    elif o in ("-T","--tree"):
        save_tree = True
    elif o in ("-P","--print"):
        print_msg = True
    elif o in ("-n","--norm"):
        norm = True
    elif o in ("-h","--help"):
        usage()
        sys.exit(2)
    else:
        assert False, "unhandeled option"


if opt2d: plotopt = "2d"
if plotopt == "print": print_msg = True
elif plotopt == "line": line = True
elif plotopt == "bin": vsbin = True
elif plotopt == "noline" or plotopt == "graph": noline = True
elif plotopt == "2d": opt2d = True; one_run = True

chan_n=max(0,chan_n)
gain_n=max(0,gain_n)
val_n=max(0,val_n)
modmin=max(1,modmin)
modmax=min(64,modmax)
chanmin=max(0,chanmin)
chanmax=min(47,chanmax)
gainmin=max(0,gainmin)
gainmax=min(5,gainmax) # up to 6 gains for integrators
nval=1

if opt2d:
    partname=partname[:3]
    modulename=partname+"00"
else:
    if len(modulename) < 5 or len(modulename) > 5:
        print "Wrong module name:",modulename
        sys.exit(2)
    partname=modulename[:3]
    modnum=modulename[3:]
    if modnum.isdigit():
        mod_n=int(modnum)
        if mod_n<0 or mod_n>64:
            print "Wrong module name:",modulename
            sys.exit(2)
        elif mod_n>0:
            modmin=mod_n
            modmax=mod_n
    else:
        print "Wrong module name:",modulename
        sys.exit(2)

part_dict = {'AUX':0,'LBA':1,'LBC':2,'EBA':3,'EBC':4,'ALL':5}
if partname in part_dict:
    ros=part_dict[partname]
    if ros==5: rosmin=0; rosmax=5; ros=-1
    else: rosmin=ros; rosmax=ros+1
else:
    if opt2d: print "Wrong partition name:",partname
    else: print "Wrong module name:",modulename
    sys.exit(2)

many=False
if modmin!=modmax:
    many=True
    modulename="%s%2.2d-%s%2.2d" % (partname,modmin,partname,modmax)
else:
    mod_n=modmin
    modulename="%s%2.2d" % (partname,mod_n)

if chanmin!=chanmax:
    many=True
    channame="%2.2d-%2.2d" % (chanmin,chanmax)
else:
    chan_n=chanmin
    channame="%2.2d" % (chan_n)

if gainmin!=gainmax:
    many=True
    gain_n = -1

if one_run and plotopt == "dist":
    print "Switching to noline mode"
    noline = True
    plotopt = "noline"

if many:
    many=(line or noline)

if not many and gain_n==-1:
    gain_n=gainmin

if gain_n == -1:
    gainname = "AllG"
elif gain_n == 0:
    gainname = "LG"
elif gain_n == 1:
    gainname = "HG"
else:
    gainname = "G"+str(gain_n)
if val_n>0: gainname += " val[%d]" % (val_n)


import ROOT
from ROOT import TCanvas, TH1D, TH2D, TGraph, TTree, TLegend
from ROOT import gROOT
from ROOT import kTRUE
from array import array
import numpy as np


from TileCalibBlobPython import TileCalibTools
from TileCalibBlobPython import TileCellTools
from TileCalibBlobObjs.Classes import *
from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("PlotCalibFromCool")
import logging
logLevel=logging.DEBUG
log.setLevel(logLevel)
log1 = getLogger("PlotCalibFromCool")
log1.setLevel(logLevel)

folderPath=folderPath.split(",")
folderTag=[]
for fp in folderPath:
    ft = TileCalibTools.getFolderTag(schema , fp, tag)
    log.info("Initializing folder %s with tag %s" % (fp, ft))
    folderTag += [ft]
multi=(len(folderPath)>1)
if multi and many:
    print "Can not calculate product of "+" ".join(folderPath)+" in multi-channel mode"
    sys.exit(2)


#=== Get list of IOVs for given drawer or for comments record
if ros!=-1:
    modmax=min(modmax,TileCalibUtils.getMaxDrawer(ros))
if ros!=-1 and modmin==modmax:
    COOL_part = ros
    COOL_chan = modmin-1
elif ros==0 and modmin==1:
    COOL_part = ros
    COOL_chan = modmin-1
else:
    COOL_part = -1
    COOL_chan = 1000
idb = TileCalibTools.openDbConn(schema,'READONLY')
iovList = []
blobReader = []
for (fp,ft) in zip(folderPath,folderTag):
  try:
    br = TileCalibTools.TileBlobReader(idb,fp, ft)
    blobReader += [br]
    dbobjs = br.getDBobjsWithinRange(COOL_part, COOL_chan)
    if (dbobjs == None): raise Exception("No DB objects retrieved when building IOV list!")
    while dbobjs.goToNext():
      obj = dbobjs.currentRef()
      objsince = obj.since()
      sinceRun = objsince >> 32
      sinceLum = objsince & 0xFFFFFFFF
      since    = (sinceRun, sinceLum)
      objuntil = obj.until()
      untilRun = objuntil >> 32
      untilLum = objuntil & 0xFFFFFFFF
      until    = (untilRun, untilLum)

      if multi:
          iov = (since, since)
      else:
          iov = (since, until)
      iovList.append(iov)
  except:
    print "Warning: can not read IOVs from input DB file"
    sys.exit(2)

if multi:
    il=[]
    iov1=(-1,-1)
    for iov in sorted(iovList):
        iov2=iov[0]
        if iov1!=iov2:
            il.append(iov)
            iov1=iov2
    iovList=[]
    for i in xrange(len(il)-1):
        iovList.append(((il[i][0][0],il[i][0][1]),(il[i+1][1][0],il[i+1][1][1])))
    i=len(il)-1
    iovList.append(((il[i][0][0],il[i][0][1]),(0xFFFFFFFF,0xFFFFFFFF)))

###################################################################
print "\nPlotting in %s mode" % plotopt
print '-'*20

be=iovList[0][0][0]
en=iovList[-1][0][0]

import cx_Oracle
connection=cx_Oracle.connect(dsn="ATLR",user="atlas_run_number_r",password="-Run.Num@rEaDeR-x-12")
con=connection.cursor()
sql="SELECT MAX(RUNNUMBER) FROM ATLAS_RUN_NUMBER.RUNNUMBER"
con.execute(sql)
data=con.fetchall()
lastrun=data[0][0]
connection.close()

if runNum <= 0:
    runNum = en
if one_run:
    begin = runNum
    end = runNum
    if line or noline:
        lumiNum = 0
else:
    if begin <= 0: begin = be
    if end <= 0: end = lastrun

veryEnd = end if end<lastrun else lastrun

if begin != be or end != en:
    ib=0
    ie=len(iovList)
    for i,iovs in enumerate(iovList):
        run = iovs[0][0]
        lumi = iovs[0][1]
        if (run<begin and run>be) or (run==begin and lumi<=lumiNum):
            be=run
            ib=i
        if run>end and run<en:
            en=run
            ie=i+1
    if be != begin:
        print "Changing begin run from",begin,"to",be,"(start of IOV)"
        begin=be
    if en != end:
        if en>end: print "Changing end run from",end,"to",en,"(start of next IOV)"
        else: print "Changing end run from",end,"to",en,"(start of last IOV)"
        end=en
    iovList=iovList[ib:ie]

if one_run:
    if opt2d and not (line or noline):
        iovList=iovList[:1]
        print "Plotting values for (Run,Lumi) = (%i,%i) " % (runNum,lumiNum)
        print "from IOV",iovList[0]
    elif len(iovList)<=2 and iovList[-1][0][0]!=runNum:
        print "Too few points to plot value as a function of lumi block"
        print "Just one IOV around run", runNum
        print iovList[0]
        print "Plotting value as a function of run number"
        one_run=False
    else:
        print "Plotting Run: %i" % runNum
        print "IOV around this run"
        print iovList
        if not line and runNum!=iovList[-1][0][0]:
            iovList=iovList[:-1]

if not one_run:
    print "Plotting Runs: %i to %i" % (begin, end)
    print len(iovList),"different IOVs in total"
    if iovList[0][0][0]!=begin or iovList[-1][0][0]!=end:
        print "Congratulations! You found the bug in this script"
        print "Please, contact authors"
        sys.exit(2)

####################################################################

#### Creating tree for constants

ROOT.gROOT.SetBatch(kTRUE)

labels = ROOT.vector('string')()
vals = ROOT.vector('double')()

titsuff=""
if one_run or opt2d:
    if lumiNum<=0:
        titsuff=" Run " + str(runNum)
    else:
        titsuff=" Run " + str(runNum) + " LB " + str(lumiNum)
titsuff+=" Tag " + " ".join(folderTag)
rlt=titsuff.replace(" ","_").replace("_Tag","")
gtitle = modulename +" " + " Channel" +("s " if (chanmin!=chanmax) else " ") + channame +" " + gainname + titsuff

if opt2d: fname="%s_g%d_v%d%s_%s" % ( partname,gain_n,val_n,rlt,plotopt)
else: fname="%s_ch%s_g%d_v%d%s_%s" % ( modulename,channame,gain_n,val_n,rlt,plotopt)

if save_tree: tree_file = ROOT.TFile(fname+".root","RECREATE")

tree = TTree("tree","ttree")
run_n = np.zeros(1, dtype = float)
lumi_n = np.zeros(1, dtype = float)
channel_n = np.zeros(1, dtype = float)
module_n = np.zeros(1, dtype = int)
ros_n = np.zeros(1, dtype = int)
value = np.zeros(1, dtype = float)
scale = np.zeros(1, dtype = float)

tree.Branch("runnumber",run_n,'runnumber/D')
tree.Branch("luminum",lumi_n,'luminum/D')
tree.Branch("channel",channel_n,'channel/D')
tree.Branch("module",module_n,'module/I')
tree.Branch("ros",ros_n,'ros/I')
tree.Branch("labels", labels)
tree.Branch("vals", vals)
if multi: tree.Branch("value", value,'value/D')
if norm: tree.Branch("scale", scale,'scale/D')

####################################################################
#Looping over iovs and filling the tree
first=True
value[0]=1.
scale[0]=1.
vals.clear()
labels.clear()
one_iov = (len(iovList)<2)

for iovs in iovList:

    if one_iov:
        run = runNum
        lumi = lumiNum
        rl = "(%i,%i) - (%i,%i)" % (iovs[0][0],iovs[0][1],iovs[1][0],iovs[1][1])
    else:
        run = iovs[0][0]
        lumi = iovs[0][1]
        rl = "(%i,%i)" % (run,lumi)
        if one_run and run!=runNum:
            if run<runNum: lumi=-1
            if run>runNum: lumi=2000
            run=runNum

    run_n[0] = run
    lumi_n[0] = lumi
    if vals.size()>0:
        if first:
            scale[0]=value[0]
            first=False
        tree.Fill()
        labels.clear()
        vals.clear()
        value[0]=1.

    for ros in xrange(rosmin,rosmax):
        ros_n[0] = ros
        for mod in xrange(modmin-1, min(modmax,TileCalibUtils.getMaxDrawer(ros))):
            module_n[0] = mod
            modName = TileCalibUtils.getDrawerString(ros,mod)
            for (fp,ft,br) in zip(folderPath,folderTag,blobReader):
                try:
                  flt = br.getDrawer(ros, mod,(run,lumi), False, False)   # modified
                  if flt is None or isinstance(flt, (int)):
                      if one_iov or ros!=0: print "%s is missing in DB" % modName
                  else:
                      nchan = flt.getNChans()
                      ngain = flt.getNGains()
                      nval  = flt.getObjSizeUint32()
                      for chn in xrange(chanmin,min(chanmax+1,nchan)):
                          for adc in xrange(gainmin,min(gainmax+1,ngain)):
                              msg = "%s  %s %2i %1i  " % (rl, modName, chn, adc )
                              for val in xrange(nval):
                                  msg += "  %f" % flt.getData(chn, adc, val)
                                  vals.push_back(flt.getData(chn, adc, val))
                                  if val==0:
                                      value[0] *= flt.getData(chn, adc, val)
                              if print_msg:
                                  if multi:
                                      print fp,msg
                                  else:
                                      print msg
                          channel_n[0] = chn
                          if one_run: labels.push_back(str(lumi))
                          else: labels.push_back(str(run))
                          if not multi or opt2d:
                              if first:
                                  if vals.size()>val_n:
                                      scale[0]=vals[val_n]
                                  first=False
                              tree.Fill()
                              if not line or many:
                                  labels.clear()
                                  vals.clear()
                                  value[0] = 1.

                except Exception, e:
                  print e

            if multi and not opt2d and vals.size()>0:
                if first:
                    scale[0]=value[0]
                    first=False
                tree.Fill()
                if not line:
                    labels.clear()
                    vals.clear()
                    value[0] = 1.


if not opt2d and vals.size()>0:
    run_n[0] = veryEnd
    lumi_n[0] = 0
    if first:
        scale[0]=value[0]
        first=False
    tree.Fill()
    labels.clear()
    vals.clear()
    value[0]=1.

idb.closeDatabase()

if plotopt == "print":
    sys.exit(0)


####################################################################################

ROOT.gROOT.SetStyle("Plain")
ROOT.gROOT.ForceStyle()
ROOT.gStyle.SetOptStat(0)


if one_run:
    cut_cond = "runnumber == " + str(runNum)
else:
    cut_cond = "runnumber >= " + str(begin) + " && " + "runnumber <= " + str(veryEnd)

##### Which value to choose
if multi:
    data = "value"
elif many:
    data = "vals[%d]"
else:
    data = "vals[" + str(val_n) + "]"

if cut:
    if not "val" in cut: cut = data + cut
    cut_cond = "(" + cut_cond + ") && (" + cut + ")"

if norm:
    data += "/scale"

if (line or noline) and not one_run:
    data += ":runnumber"

elif vsbin:
    data +=":labels"

elif one_run:
    data += ":luminum"


canv = TCanvas("PlotCalib","plotCalib",0,0,1600,800)

if opt2d:
    hhh = TH2D("hhh","hhh",modmax-modmin+1,modmin-0.5,modmax+0.5,chanmax-chanmin+1,chanmin-0.5,chanmax+0.5)

if not many and not opt2d:
    print "Plotting",data
    print "With cut",cut_cond
    if vsbin: tree.Draw(data,cut_cond,"goff",15)
    else: tree.Draw(data,cut_cond,"goff")

    if tree.GetSelectedRows() <= 0:
        print "Not enough points to plot"
        sys.exit(2)

    if vsbin:
        if tree.GetSelectedRows() >= 15:
            print "Maximum number of bins is 15"

if line or noline:
    if many:
        first=True
        color=1
        grall=[]
        vmin=1.e+38
        vmax=-1.e+38
        cut0 = cut_cond
        data1 = data
        leg = TLegend(0.7,0.7,0.9,0.9)
        for ros in xrange(rosmin,rosmax):
            for mod in xrange(modmin-1, modmax):
                for chn in xrange(chanmin,chanmax+1):
                    for adc in xrange(gainmin,gainmax+1):
                        if "%d" in data:
                            data1 = data.replace("%d",str((adc-gainmin)*nval+val_n))
                        if "%d" in cut_cond:
                            cut0 = cut_cond.replace("%d",str((adc-gainmin)*nval+val_n))
                        cut1 = "(ros == %d && module == %d && channel == %d) && %s" % (ros,mod,chn,cut0)
                        tree.Project("htemp",data1,cut1)
                        if tree.GetSelectedRows() > 0:
                            color = color + 1
                            gr = TGraph(tree.GetSelectedRows(),tree.GetV2(),tree.GetV1())
                            yy=gr.GetY()
                            for ii in xrange(len(yy)):
                                vv=yy[ii]
                                if vv<vmin: vmin=vv
                                if vv>vmax: vmax=vv
                            grall += [gr]
                            leg.AddEntry(gr,"%s%2.2d ch %2.2d" % ( part_dict.keys()[part_dict.values().index(ros)],mod+1,chn),"lep")
                            gr.SetMarkerStyle(20)
                            gr.SetMarkerSize(1.3)
                            gr.SetMarkerColor(color)
                            gr.SetLineColor(color)
                            if first:
                                print "First plot:"
                                print "Plotting",data1
                                print "With cut",cut1
                                print "Note that in TTree module number starts from 0"
                                gr.SetTitle(gtitle)
                                gr.GetXaxis().SetNoExponent(kTRUE)
                                if one_run: gr.GetXaxis().SetTitle("Lumi")
                                else: gr.GetXaxis().SetTitle("Runs")
                                if noline: gr.Draw("ap")
                                else: gr.Draw("apl")
                                first=False
                            else:
                                if noline: gr.Draw("p")
                                else: gr.Draw("pl")
        lg=len(grall)
        if lg==0:
            print "Plotting",data
            print "With cut",cut_cond
            print "Not enough points to plot"
            sys.exit(2)
        if lg>400: nc=20
        elif lg>144: nc=int(math.sqrt(lg))
        else: nc=int((lg-1)/12+1)
        leg.SetNColumns(nc)
        extra = (2+0.4*int(lg/nc+1))
        dv=(vmax-vmin)/10.
        grall[0].SetMinimum(vmin-dv)
        grall[0].SetMaximum(vmax+dv*extra)
        if nc>8: nc=8
        leg.SetX1(0.9-0.1*nc)
        leg.SetY1(0.9-0.8*(extra-1)/(11+extra))
        leg.Draw()
    else:
        gr = TGraph(tree.GetSelectedRows(),tree.GetV2(),tree.GetV1())
        gr.SetTitle(gtitle)
        gr.SetMarkerStyle(20)
        gr.SetMarkerSize(1.3)
        gr.SetMarkerColor(2)
        gr.SetLineColor(2)
        gr.GetXaxis().SetNoExponent(kTRUE)
        if one_run: gr.GetXaxis().SetTitle("Lumi")
        else: gr.GetXaxis().SetTitle("Runs")
        if noline: gr.Draw("ap")
        else: gr.Draw("apl")

elif opt2d:
    nentries = tree.GetEntries()
    hhh.SetName(modulename)
    hhh.SetTitle(gtitle)
    hhh.GetYaxis().SetTitle("Channels")
    hhh.GetXaxis().SetTitle("Modules")

    if multi:
        Matrix = [[1 for y in range(48)] for x in range(64)]
        for i in xrange(nentries):
            tree.GetEntry(i)
            if vals.size()>0:
                x=int(module_n[0])
                y=int(channel_n[0])
                Matrix[x][y] *= vals[0]
        sc = Matrix[modmax-1][chanmax] if norm and Matrix[modmax-1][chanmax]>0 else 1
        for x in xrange(modmin,modmax+1):
            for y in xrange(chanmin,chanmax+1):
                hhh.Fill(x, y, Matrix[x-1][y]/sc)
    else:
        for i in xrange(nentries):
            tree.GetEntry(i)
            #print ("%f %f %f") % (module_n[0], channel_n[0],vals[0])
            if norm:
                hhh.Fill(module_n[0]+1, channel_n[0],vals[val_n]/scale[0])
            else:
                hhh.Fill(module_n[0]+1, channel_n[0],vals[val_n])

    hhh.Draw("colz")

else:
    h = ROOT.gDirectory.Get("htemp")
    h.SetName(modulename)
    h.SetTitle(gtitle)
    h.SetLineColor(2)
    h.SetLineWidth(2)
    h.GetXaxis().SetTitle("Value")
    if vsbin or one_run:
        h.SetMarkerStyle(21)
        h.SetMarkerSize(1.4)
        h.SetMarkerColor(2)
        h.GetXaxis().SetTitle("Run")
        if one_run: h.GetXaxis().SetTitle("Lumi");  h.SetTitle(gtitle)
        h.GetYaxis().SetTitle("")
        h.GetYaxis().SetTitleOffset(1.35)
    try:
        h.Draw()
    except: print "Empty "

canv.SetGridx();
canv.SetGridy();

canv.Update()

##########################################################


canv.Print(fname+".eps")
canv.Print(fname+".png")

if save_tree:
    tree_file.Write()
    tree_file.Close()


#=== display plot
os.system("display "+fname+".png")


#
#if __name__ == '__main__':
#   rep = ''
#   while not rep in [ 'q', 'Q' ]:
#      rep = raw_input( 'enter "q" to quit: ' )
#      if 1 < len(rep):
#         rep = rep[0]
