from ROOT import TFile, TH1F, TH2F, TF1, TCanvas, TGraph, TProfile,TChain, TGraphErrors, TPostScript, TLorentzVector, TLine
from ROOT import gDirectory, gROOT, gStyle, gPad, TMultiGraph, TLegend, TF1, TPad, TNtuple
import ROOT
import math,sys,time,string,glob,pickle
from array import array


class TRTBase:
    def __init__(self):
        self.entries=0.
        self.t0=0.
        self.sumtimeres=0.
        self.sumw=0
        self.oldt0=0.
        self.deltat0=0.
        self.sumoldt0=0.
        self.sumchild=0
        self.t0_from_rt=0.
        self.rtmode=0

        self.sumrawres=0
        self.sumrawresq=0

        self.bec=-3
        self.sector=-1
        self.layer=-1
        self.strawlayer=-1
        self.straw=-1

        self.updatecalib=False
        self.updatecalibRT=False

        self.performRT=False
        self.performT0=False

    def avtimeresidual(self):
        return self.sumtimeres/self.sumw
        
    def avrawtimeresidualerror(self):
        rms = math.sqrt( ( self.sumrawresq - self.sumrawres*self.sumrawres/self.sumw)/self.sumw ) 
        return rms /math.sqrt(self.entries)


def compare(a,b):
    if a.bec<b.bec:
        return -1
    elif a.bec>b.bec:
        return 1
    if a.layer<b.layer:
        return -1
    elif a.layer>b.layer:
        return 1
    if a.sector<b.sector:
        return -1
    elif a.sector>b.sector:
        return 1
    if a.strawlayer<b.strawlayer:
        return -1
    elif a.strawlayer>b.strawlayer:
        return 1
    if a.straw<b.straw:
        return -1
    elif a.straw>b.straw:
        return 1
    return 0
        

class Straw(TRTBase):
    def __init__(self):
        TRTBase.__init__(self)
        self.chip=-1


class Chip(TRTBase):
    def __init__(self):
        TRTBase.__init__(self)
        self.straws={}
        self.chip=-1

    def calculate(self):
        self.sumtimeres=0.
        self.sumw=0.
        #self.oldt0=0.
        for k,v in self.straws.iteritems():
            self.sumtimeres+=v.sumtimeres
            self.sumw+=v.sumw
            self.sumoldt0+=v.oldt0
            self.sumrawres+=v.sumrawres
            self.sumrawresq+=v.sumrawresq
        if len(self.straws)>0:
            self.sumchild=len(self.straws)
            self.oldt0=self.sumoldt0/self.sumchild

    def getStrawsSorted(self):
        nlist=self.straws.values()
        nlist.sort(compare)
        return nlist

    def getNStraws(self):
        return len(self.straws)
        

class Board(TRTBase):
    def __init__(self):
        TRTBase.__init__(self)
        self.chips={}
        self.board=-1

    def calculate(self):
        self.sumtimeres=0.
        self.sumw=0.
        self.oldt0=0.
        for k,v in self.chips.iteritems():
            self.sumtimeres+=v.sumtimeres
            self.sumw+=v.sumw
            self.sumoldt0+=v.oldt0
            self.sumrawres+=v.sumrawres
            self.sumrawresq+=v.sumrawresq
        if len(self.chips)>0:
            self.sumchild=len(self.chips)
            self.oldt0=self.sumoldt0/self.sumchild

        hlist=[]
        for k,v in self.chips.iteritems():
            hlist.append(v.treshist.__class__(v.treshist))
        #hret=hlist[0].Clone("TRT_"+repr(self.bec))
        hret=hlist[0].__class__(hlist[0])
        #hret.SetName("TRT_"+repr(self.bec))
        for i in range(1,len(hlist)):
            hret.Add(hlist[i])
        self.treshist=hret


    def getChipsSorted(self):
        nlist=self.chips.values()
        nlist.sort(compare)
        return nlist

    def getNChips(self):
        return len(self.chips)

    def getNStraws(self):
        count=0
        for i in self.chips.itervalues():
            count+=i.getNStraws()
        return count

class Module(TRTBase):
    def __init__(self):
        TRTBase.__init__(self)
        self.boards={}
        self.rtpars=[]
        self.rtmode=0
        self.polyorder=3

        self.resmean=-1000.
        self.reswidth=-1000.

    def calculate(self):
        self.sumtimeres=0
        self.sumw=0
        self.oldt0=0.
        for k,v in self.boards.iteritems():
            self.sumtimeres+=v.sumtimeres
            self.sumw+=v.sumw
            self.sumoldt0+=v.sumoldt0
            self.sumchild+=v.sumchild
            self.sumrawres+=v.sumrawres
            self.sumrawresq+=v.sumrawresq
        if len(self.boards)>0:
            self.oldt0=self.sumoldt0/self.sumchild

        maxbin=1
        maxval=0
        for j in range(1,self.reshist.GetNbinsX()+1):
            val=self.reshist.GetBinContent(j)
            if val>maxval:
                maxval=val
                maxbin=j

        if self.reshist.GetEntries()>200:
            mean=self.reshist.GetBinCenter(maxbin)
            tmp=self.reshist.__class__(self.reshist)
            tmp.SetName("tmp")
            #
            tmp.Fit("gaus","Q0RI","",mean-0.4,mean+0.4)
            self.resmean=tmp.GetFunction("gaus").GetParameter(1)
            self.reswidth=tmp.GetFunction("gaus").GetParameter(2)
            #self.resmean=self.reshist.GetMean()
            #self.reswidth=self.reshist.GetRMS()
        else:
            self.resmean=0
            self.reswidth=0

        hlist=[]
        for k,v in self.boards.iteritems():
            hlist.append(v.treshist.__class__(v.treshist))
        #hret=hlist[0].Clone("TRT_"+repr(self.bec))
        hret=hlist[0].__class__(hlist[0])
        #hret.SetName("TRT_"+repr(self.bec))
        for i in range(1,len(hlist)):
            hret.Add(hlist[i])
        self.treshist=hret

    def getBoardsSorted(self):
        nlist=self.boards.values()
        nlist.sort(compare)
        return nlist

    def getNStraws(self):
        count=0
        for i in self.boards.itervalues():
            count+=i.getNStraws()
        return count





class TRT_SubDetector(TRTBase):
    def __init__(self):
        TRTBase.__init__(self)
        self.modules={}
        self.rtpars=[]

    def calculate(self):
        self.sumtimeres=0.
        self.sumw=0.
        self.oldt0=0.
        for k,v in self.modules.iteritems():
            self.sumtimeres+=v.sumtimeres
            self.sumw+=v.sumw
            self.sumoldt0+=v.sumoldt0
            self.sumchild+=v.sumchild
            self.sumrawres+=v.sumrawres
            self.sumrawresq+=v.sumrawresq
        if len(self.modules)>0:
            self.oldt0=self.sumoldt0/self.sumchild

        hlist=[]
        for k,v in self.modules.iteritems():
            hlist.append(v.hist.__class__(v.hist))
        #hret=hlist[0].Clone("TRT_"+repr(self.bec))
        hret=hlist[0].__class__(hlist[0])
        hret.SetName("TRT_"+repr(self.bec))
        for i in range(1,len(hlist)):
            hret.Add(hlist[i])
        self.hist=hret

        hlist=[]
        for k,v in self.modules.iteritems():
            hlist.append(v.reshist.__class__(v.reshist))
        hret=hlist[0].__class__(hlist[0])
        hret.SetName("TRTres_"+repr(self.bec))
        for i in range(1,len(hlist)):
            hret.Add(hlist[i])
        self.reshist=hret

        hlist=[]
        for k,v in self.modules.iteritems():
            hlist.append(v.treshist.__class__(v.treshist))
        #hret=hlist[0].Clone("TRT_"+repr(self.bec))
        hret=hlist[0].__class__(hlist[0])
        #hret.SetName("TRT_"+repr(self.bec))
        for i in range(1,len(hlist)):
            hret.Add(hlist[i])
        self.treshist=hret


    def getModulesSorted(self):
        nlist=self.modules.values()
        nlist.sort(compare)
        return nlist

    def getNStraws(self):
        count=0
        for i in self.modules.itervalues():
            count+=i.getNStraws()
        return count


class TRT_Detector(TRTBase):
    def __init__(self):
        TRTBase.__init__(self)
        self.subdet={}
        self.rtpars=[]

    def calculate(self):
        self.sumtimeres=0.
        self.sumw=0.
        self.oldt0=0.
        for k,v in self.subdet.iteritems():
            self.sumtimeres+=v.sumtimeres
            self.sumw+=v.sumw
            self.sumoldt0+=v.sumoldt0
            self.sumchild+=v.sumchild
            self.sumrawres+=v.sumrawres
            self.sumrawresq+=v.sumrawresq
        if len(self.subdet)>0:
            self.oldt0=self.sumoldt0/self.sumchild

        hlist=[]
        for k,v in self.subdet.iteritems():
            hlist.append(v.hist.__class__(v.hist))
        #hret=hlist[0].Clone("TRT")
        hret=hlist[0].__class__(hlist[0])
        hret.SetName("TRT")
        for i in range(1,len(hlist)):
            hret.Add(hlist[i])
        self.hist=hret

        hlist=[]
        for k,v in self.subdet.iteritems():
            hlist.append(v.reshist.__class__(v.reshist))
        hret=hlist[0].__class__(hlist[0])
        hret.SetName("TRTres")
        for i in range(1,len(hlist)):
            hret.Add(hlist[i])
        self.reshist=hret

            
        hlist=[]
        for k,v in self.subdet.iteritems():
            hlist.append(v.treshist.__class__(v.treshist))
        #hret=hlist[0].Clone("TRT_"+repr(self.bec))
        hret=hlist[0].__class__(hlist[0])
        #hret.SetName("TRT_"+repr(self.bec))
        for i in range(1,len(hlist)):
            hret.Add(hlist[i])
        self.treshist=hret


        #mean=self.reshist.GetBinCenter(maxbin)
        #tmp=self.reshist.__class__(self.reshist)
        #tmp.SetName("tmp")
        #tmp.Fit("gaus","Q0RI","",mean-0.4,mean+0.4)
        #self.resmean=tmp.GetFunction("gaus").GetParameter(1)
        #self.reswidth=tmp.GetFunction("gaus").GetParameter(2)


    def getSubsSorted(self):
        nlist=self.subdet.values()
        nlist.sort(compare)
        return nlist

    def getNStraws(self):
        count=0
        for i in self.subdet.itervalues():
            count+=i.getNStraws()
        return count



Modules={}
TRT=TRT_Detector()


def get_filelist():
    list=glob.glob(sys.argv[1])

#    list=glob.glob("/afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/batch/alextest/temp/*.root")
    for i in list:
    	print "Reading module histograms from file %s" % i
    return list

def get_modulelist_perfile(calibfile):
    f=TFile(calibfile)
    items=f.GetDirectory("").GetListOfKeys()
    modules=[]
    for i in items:
        name=i.GetName()
        if name.find("Module")>=0:
            tokens=name.split('_')
            #bec=int(tokens[1])
            #phisector=int(tokens[2])
            #layer=int(tokens[3])
            modules.append(name[name.find('_')+1:])

    return modules

def get_boardchiplist_permodule(f,bec,sector,layer):
    dirname="Module_%d_%d_%d" %  (bec,sector,layer)
    items=f.GetDirectory(dirname).GetListOfKeys()
    boardchips=[]
    for i in items:
        name=i.GetName()
        if name.find("ChipBoard")>=0:
            tokens=name.split('_')
            chip=int(tokens[2])
            board=int(tokens[1])
            
            boardchips.append([board,chip])
    return boardchips


def build_moduledict(filelist):
    for i in filelist:
        list=get_modulelist_perfile(i)
        f=TFile(i)

        print "Reading module histograms from file %s" % i

        for s in list:
            tokens=s.split('_')
            bec=int(tokens[0])
            sector=int(tokens[1])
            layer=int(tokens[2])

            mid="%d:%d:%d" % (bec, sector, layer)
            hname="Module_%d_%d_%d/ttodcalibh2" % (bec,sector,layer)
            h=f.Get(hname)
            #h2=h.Clone("ttodcalibh2_"+mid)
            h2=h.__class__(h)
            h2.SetName("ttodcalibh2_"+mid)
            h2.SetDirectory(0)
            
            hname="Module_%d_%d_%d/resh1" % (bec,sector,layer)
            h=f.Get(hname)
            h3=h.__class__(h)
            h3.SetName("resh1_"+mid)
            #h3=h.Clone("resh1_"+mid)
            h3.SetDirectory(0)

            #check if we have seen this subdetector already and if not create it
            if not bec in TRT.subdet:
                nsdet=TRT_SubDetector()
                nsdet.bec=bec
                TRT.subdet[bec]=nsdet

            if mid in TRT.subdet[bec].modules:
                #add this histo to the already existing histo
                omod=TRT.subdet[bec].modules[mid]
                omod.hist.Add(h2)
                omod.reshist.Add(h3)
            else:
                #create a new module
                nmod=Module()
                nmod.bec=bec
                nmod.sector=sector
                nmod.layer=layer
                nmod.hist=h2
                nmod.reshist=h3
                TRT.subdet[bec].modules[mid]=nmod

            #module exists now
            bc=get_boardchiplist_permodule(f,bec,sector,layer)

            for j in bc:
                board=j[0]
                chip=j[1]

                hname="Module_%d_%d_%d/ChipBoard_%d_%d/timeresh1" % (bec,sector,layer,board,chip)

                if chip<0:
                    board=-1
                    chip+=100

                if abs(bec)==2:
                    board=-1
                    #print "Creating Board-Chip combination : %d - %d" % (board,chip)

                h=f.Get(hname)
                h4=h.__class__(h)
                h4.SetName("tresh1_"+mid+'_'+`board`+'_'+`chip`)
                h4.SetDirectory(0)

                if board in TRT.subdet[bec].modules[mid].boards:
                    #board exists already
                    if chip in TRT.subdet[bec].modules[mid].boards[board].chips:
                        #chip exists already
                        pass
                    else:
                        nchip=Chip()
                        nchip.bec=bec
                        nchip.chip=chip
                        nchip.board=board
                        nchip.sector=sector
                        nchip.layer=layer
                        nchip.treshist=h4
                        TRT.subdet[bec].modules[mid].boards[board].chips[chip]=nchip
                else:
                    #create this board
                    nboard=Board()
                    nboard.bec=bec
                    nboard.sector=sector
                    nboard.layer=layer
                    nboard.board=board

                    nchip=Chip()
                    nchip.bec=bec
                    nchip.sector=sector
                    nchip.layer=layer
                    nchip.treshist=h4

                    TRT.subdet[bec].modules[mid].boards[board]=nboard
                    TRT.subdet[bec].modules[mid].boards[board].chips[chip]=nchip



    #for k,v in TRT.subdet.iteritems():
    #    print "Number of modules in subdet %d : %d" % (k,len(v.modules))
    #print "Found %d modules" % len(TRT.modules)
                

def get_rthist_comb():
    hlist=[]
    for k,v in TRT.modules.iteritems():
        hlist.append(v.hist)

    hret=hlist[0]
    for i in range(1,len(hlist)):
        hret.Add(hlist[i])
    return hret
    

def build_chain(filelist):
    chain=TChain("strawtuple")
    for i in filelist:
        chain.Add(i)

    return chain

def build_strawdict(chain):
    entries = chain.GetEntries()
    for jentry in xrange( entries ):

        ientry = chain.LoadTree( jentry )
        if ientry < 0:
            break

        # copy next entry into memory and verify
        nb = chain.GetEntry( jentry )
        if nb <= 0:
            continue

        #build identifier
        bec=int(chain.bec)
        mod=int(chain.mod)
        lay=int(chain.lay)
        chip=int(chain.chip)
        strawlay=int(chain.strawlay)
        straw=int(chain.straw)
        board=int(chain.board)
        oldt0=float(chain.oldt0)
        id="%d:%d:%d:%d:%d" % (bec, mod, lay, strawlay, straw)
        mid="%d:%d:%d" % (bec, mod, lay)

        try:
            TRT.subdet[bec].modules[mid].boards[board].chips[chip].oldt0=oldt0

            if id in TRT.subdet[bec].modules[mid].boards[board].chips[chip].straws:
                ostraw=TRT.subdet[bec].modules[mid].boards[board].chips[chip].straws[id]
                ostraw.entries+=chain.num
                ostraw.sumw+=chain.sumw2
                ostraw.sumtimeres+=chain.t0

                try:
                    ostraw.sumrawres+=chain.srawr
                    ostraw.sumrawresq+=chain.srawrq
                except:
                    pass
            else:
                nstraw=Straw()
                nstraw.bec=bec
                nstraw.sector=mod
                nstraw.layer=lay
                nstraw.strawlayer=strawlay
                nstraw.straw=straw
                nstraw.oldt0=chain.oldt0
                nstraw.chip=int(chain.chip)
                nstraw.sumtimeres=chain.t0
                nstraw.sumw=chain.sumw2
                nstraw.entries=chain.num
                try:
                    nstraw.sumrawres=chain.srawr
                    nstraw.sumrawresq=chain.srawrq
                except:
                    pass

                TRT.subdet[bec].modules[mid].boards[board].chips[chip].straws[id]=nstraw
                

        except:
            print "Problem: bec=%d mid=%s board=%d chip=%d" % (bec,mid,board,chip)
            print repr(TRT.subdet[bec].modules[mid].boards.keys())
            print repr(TRT.subdet[bec].modules[mid].boards[board].chips.keys())

def build_strawdict_old(chain):
    entries = chain.GetEntries()
    for jentry in xrange( entries ):

        ientry = chain.LoadTree( jentry )
        if ientry < 0:
            break

        # copy next entry into memory and verify
        nb = chain.GetEntry( jentry )
        if nb <= 0:
            continue

        #build identifier
        bec=int(chain.bec)
        mod=int(chain.mod)
        lay=int(chain.lay)
        chip=int(chain.chip)
        strawlay=int(chain.strawlay)
        straw=int(chain.straw)
        board=int(chain.board)
        id="%d:%d:%d:%d:%d" % (bec, mod, lay, strawlay, straw)
        mid="%d:%d:%d" % (bec, mod, lay)

        if not mid in TRT.subdet[bec].modules:
            print "Mismatch between Module hists and Strawtuple!!! : "+mid
            print "Aborting ..."
            sys.exit(-1)

        module=TRT.subdet[bec].modules[mid]

        if chip in module.chips:
            if id in module.chips[chip].straws:
                #add information to this straw
                ostraw=module.chips[chip].straws[id]
                ostraw.entries+=chain.num
                ostraw.sumw+=chain.sumw2
                ostraw.sumtimeres+=chain.t0

                try:
                    ostraw.sumrawres+=chain.srawr
                    ostraw.sumrawresq+=chain.srawrq
                except:
                    pass

            else:
                #create a new straw object and put it into the dictionary
                nstraw=Straw()
                nstraw.bec=bec
                nstraw.sector=mod
                nstraw.layer=lay
                nstraw.strawlayer=strawlay
                nstraw.straw=straw
                nstraw.oldt0=chain.oldt0
                nstraw.chip=int(chain.chip)
                nstraw.sumtimeres=chain.t0
                nstraw.sumw=chain.sumw2
                nstraw.entries=chain.num
                TRT.subdet[bec].modules[mid].chips[chip].straws[id]=nstraw

                try:
                    nstraw.sumrawres=chain.srawr
                    nstraw.sumrawresq=chain.srawrq
                except:
                    pass

        else:
            #create new chip class (and therefore also a new straw class
            nchip=Chip()
            nchip.layer=lay
            nchip.sector=mod
            nchip.bec=bec
            nstraw=Straw()
            nstraw.bec=bec
            nstraw.sector=mod
            nstraw.layer=lay
            nstraw.strawlayer=strawlay
            nstraw.straw=straw
            nstraw.chip=int(chain.chip)
            nstraw.sumtimeres=chain.t0
            nstraw.sumw=chain.sumw2
            nstraw.entries=chain.num
            nstraw.oldt0=chain.oldt0

            try:
                nstraw.sumrawres=chain.sumrawres
                nstraw.sumrawresq=chain.sumrawresq
            except:
                pass

            nchip.straws[id]=nstraw
            nchip.chip=chip
            TRT.subdet[bec].modules[mid].chips[chip]=nchip



def setRTlevel(level):
    if level=="TRT":
        TRT.performRT=True
    elif level=="SUBDET":
        TRT.performRT=True
        for sub in TRT.subdet.itervalues():
            sub.performRT=True
    elif level=="MODULE":
        TRT.performRT=True
        for sub in TRT.subdet.itervalues():
            sub.performRT=True
            for mod in sub.modules.itervalues():
                mod.performRT=True
    elif level=="BOARD":
        print "Board level RT is not supported!!!"
    elif level=="CHIP":
        print "Chip level RT is not supported!!!"
    elif level=="STRAW":
        print "STRAW level RT is not supported!!!"
    else:
        print "Unknown RT level: "++level
        

def setT0level(level):
    if level=="TRT":
        TRT.performT0=True
    elif level=="SUBDET":
        TRT.performT0=True
        for sub in TRT.subdet.itervalues():
            sub.performT0=True
    elif level=="MODULE":
        TRT.performT0=True
        for sub in TRT.subdet.itervalues():
            sub.performT0=True
            for mod in sub.modules.itervalues():
                mod.performT0=True
    elif level=="BOARD":
        TRT.performT0=True
        for sub in TRT.subdet.itervalues():
            sub.performT0=True
            for mod in sub.modules.itervalues():
                mod.performT0=True
                for board in mod.boards.itervalues():
                    board.performT0=True
    elif level=="CHIP":
        TRT.performT0=True
        for sub in TRT.subdet.itervalues():
            sub.performT0=True
            for mod in sub.modules.itervalues():
                mod.performT0=True
                for board in mod.boards.itervalues():
                    board.performT0=True
                    for chip in board.chips.itervalues():
                        chip.performT0=True
    elif level=="STRAW":
        print "Straw level t0 not yet suported"
    else:
        print "Unknown T0 level: "++level
        

def debug_straw(s):
    print "\t\tStraw %d:%d:%d:%d:%d" % (s.bec, s.sector,s.layer, s.strawlayer, s.straw)
    print "\t\tEntries: %d" % s.entries
    print "\t\tsumtimeres=%f sumw=%f" % (s.sumtimeres,s.sumw)
    print "\t\toldt0=%f" % s.oldt0


def debug_chip(c):
    print "\tChip %d" % c.chip
    print "\tContaining %d straws" % len(c.straws)
    print "\tsumtimeres=%f sumw=%f" % (c.sumtimeres,c.sumw)
    print "\toldt0=%f" % c.oldt0
    if len(c.straws)>0:
        print "\tDumping straws:"
        for k,v in c.straws.iteritems():
            debug_straw(v)

def debug_module(m):
    print "Module %d:%d:%d" % (m.bec, m.sector, m.layer)
    print "Entries in histo: %d " % m.hist.GetEntries()
    print "Containing %d chips" % len(m.boards)
    print "sumtimeres=%f sumw=%f" % (m.sumtimeres,m.sumw)
    print "oldt0=%f" % m. oldt0
    if len(m.boards)>0:
        print "Dumping chips:"
        for k,v in m.boards.iteritems():
            debug_chip(v)

def debug_TRTSub(sub):
    print "TRT Subdetector %d" % sub.bec
    for k,v in sub.modules.iteritems():
        debug_module(v)

def debug_TRT():
    for k,v in TRT.subdet.iteritems():
        debug_TRTSub(v)


def driftradius(rtpars,time):
    return rtpars[0]+time*(rtpars[1]+time*(rtpars[2]+time*rtpars[3]))

def drdt(rtpars,time):
    return rtpars[1]+time*(2*rtpars[2]+time*3*rtpars[3])

def drifttime_debug(rtpars,r):
    t=-25
    while driftradius(rtpars,t)<r and t<75:
        print "t=%f -> r=%f" % (t,driftradius(rtpars,t))
        t+=1
    if t>=75:
        print "Not able to invert r-t relation!!!"
        return 0
    oldt=t
    t-=1
    while driftradius(rtpars,t)<r and t<oldt:
        print "t=%f -> r=%f" % (t,driftradius(rtpars,t))
        t+=0.1

    oldt=t
    t-=0.1
    while driftradius(rtpars,t)<r and t<oldt:
        print "t=%f -> r=%f" % (t,driftradius(rtpars,t))
        t+=0.01
    
    return t

def drifttime(rtpars,r):
    precision = 0.0001
    stepsize=1
    t = 20 
    residual = abs(r) - driftradius(rtpars,t)
    maxtries = 500 
    ntries = 0 ;
    while abs(residual) > precision:
        t += residual / drdt(rtpars,t) 
        residual = r - driftradius(rtpars,t) 
        ntries+=1
        if ntries>maxtries:
            print "Not able to invert r-t relation!!!"
            t=drifttime_debug(rtpars,r)
            break
    return t 




# ------ Perform the actual calibration --------

def analyze_rt(hist,mode,polyorder):
    dtfitfunc=TF1("dtfitfunc","gaus(0)")
    dtfitfunc.SetLineWidth(1)
    dtfitfunc.SetLineColor(2)
    mintime=999.
    mindistance = 0.0# 0.1 
    hlist=[]
    npoints=hist.GetNbinsY()
    dtgr = TGraphErrors(npoints)

    #print "Analyzing hist %s" % hist.GetName()

    for i in range(npoints):
        hname="%s_timehist_%d" % (hist.GetName(),i)
        htmp=hist.ProjectionX(hname,i+1,i+1)
        maxbin=1
        maxval=0
        for j in range(1,htmp.GetNbinsX()+1):
            val=htmp.GetBinContent(j)
            if val>maxval:
                maxval=val
                maxbin=j
        tbinsize=3.125
        width=2.5*tbinsize
        mean=htmp.GetBinCenter(maxbin)
        dtfitfunc.SetRange(mean-width,mean+width) 
        dtfitfunc.SetParameter(0,htmp.GetMaximum()) 
        dtfitfunc.SetParameter(1,mean) 
        dtfitfunc.SetParameter(2,5)

        #print "Bin %d with mean=%f range=%f --  %f" % (i, mean, mean-width, mean+width)
        #print "\tParameters for fit: %f %f %f" % (htmp.GetMaximum(),mean,5)

        htmp.Fit(dtfitfunc,"Q0RI")
        htmp.GetListOfFunctions().Add(dtfitfunc.Clone())
        hisname="%4.2f < d < %4.2f  [mm]" % (hist.GetYaxis().GetBinLowEdge(i+1),hist.GetYaxis().GetBinUpEdge(i+1))
        htmp.SetTitle(hisname)
        hlist.append(htmp)
        d = 0.5*(hist.GetYaxis().GetBinCenter(i+1) + hist.GetYaxis().GetBinCenter(i+1))
        t = dtfitfunc.GetParameter(1)
        dtgr.SetPoint(i,t,d)
        dtgr.SetPointError(i,dtfitfunc.GetParError(1),hist.GetYaxis().GetBinWidth(1)/math.sqrt(12.)) 
        if( d>=mindistance  and t<mintime ):
            mintime = t 

    pars=[]

    #dtgr.SetMarkerColor(18)
    #hist.GetListOfFunctions().Add(dtgr.Clone())

    if mode==0:  # BasicRtRelation
        #rtfunc = TF1("rtfunc","[0] + x * ([1] + x*([2]+x*[3]))",mintime,200) 
        rtfunc = TF1("rtfunc","1 ++ x ++ x*x ++ x*x*x",mintime,200)
        rtfunc.SetLineColor(1)
        rtfunc.SetLineStyle(3)
        #for i in range(4):
        #    rtfunc.SetParameter(i,0)

        if polyorder<3:
            rtfunc.FixParameter(3,0) 
        if polyorder<2:
            rtfunc.FixParameter(2,0) 

        result=dtgr.Fit(rtfunc,"QR")
        if not result==0:
            result=dtgr.Fit(rtfunc,"R")
            if not result==0:
                print "Cannot fit! Help!!!"
        rtfunc.SetRange(-200,200)
        hist.GetListOfFunctions().Add(rtfunc) 
    
        # get the global time offset: t for r=0
        pars=[rtfunc.GetParameter(0),rtfunc.GetParameter(1),rtfunc.GetParameter(2),rtfunc.GetParameter(3)]
        t0=drifttime(pars,0.)
        #print "Global t0 offset: %f " % t0
        
        pars[0]=0.


    print "Global t0 offset: %f" % t0

    return hist,dtgr,hlist,pars,t0




def calibrate_rt(minentries,mode,polyorder,updatet0):
    if TRT.hist.GetEntries()>minentries and TRT.performRT:
        print "Calibrating whole TRT"
        hist,dtgr,hlist,pars,t0 = analyze_rt(TRT.hist,mode,polyorder)
        TRT.hist=hist
        TRT.hlist=hlist
        TRT.dtgr=dtgr
        if updatet0:
            TRT.t0_from_rt=t0
        else:
            TRT.t0_from_rt=0.
        TRT.rtpars=pars
        TRT.rtcalibhists=hlist
        TRT.updatecalibRT=True

        for l,sub in TRT.subdet.iteritems():
            if sub.hist.GetEntries()>minentries and sub.performRT:
                print "Calibrating subdetector %d" % sub.bec
                hist,dtgr,hlist,pars,t0 = analyze_rt(sub.hist,mode,polyorder)
                sub.hist=hist
                sub.hlist=hlist
                sub.dtgr=dtgr
                if updatet0:
                    sub.t0_from_rt=t0
                else:
                    sub.t0_from_rt=0.
                sub.rtpars=pars
                sub.rtcalibhists=hlist
                sub.updatecalibRT=True
                
                for k,v in sub.modules.iteritems():
                    if v.hist.GetEntries()>minentries and v.performRT:
                        print "Calibrating Module %d:%d:%d" % (v.bec,v.layer,v.sector)
                        hist,dtgr,hlist,pars,t0 = analyze_rt(v.hist,mode,polyorder)
                        v.hist=hist
                        v.hlist=hlist
                        v.dtgr=dtgr
                        if updatet0:
                            v.t0_from_rt=t0
                        else:
                            v.t0_from_rt=0.
                        v.rtpars=pars
                        v.rtcalibhists=hlist
                        v.updatecalibRT=True

def calculate_tree():
    tentries=0
    for h,sub in TRT.subdet.iteritems():
        sentries=0
        for k,m in sub.modules.iteritems():
            mentries=0

            for x,b in m.boards.iteritems():
                bentries=0

                for j,c in b.chips.iteritems():
                    centries=0
                    for l, s in c.straws.iteritems():
                        centries+=s.entries
                    c.entries=centries
                    c.calculate()
                    bentries+=centries
                b.entries=bentries
                b.calculate()
                mentries+=bentries
            m.entries=mentries
            m.calculate()
            sentries+=mentries
        sub.entries=sentries
        sub.calculate()
        tentries+=sentries

    TRT.calculate()
    TRT.entries=tentries
    #print "Total number of hits in tree: %d" % tentries

def analyze_t0(hist,name=""):
    mean=hist.GetMean()
    rms=hist.GetRMS()


    if not name=="":
        print "+++++++++++++++++++++++"
        print
        print "Analyzing %s" % name

    maxbin=1
    maxval=0
    for j in range(1,hist.GetNbinsX()+1):
        val=hist.GetBinContent(j)
        if val>maxval:
            maxval=val
            maxbin=j

    fitmean=hist.GetBinCenter(maxbin)
    print "Bin with maximal content: %f" % fitmean
    

    if abs(fitmean)>5:
        print "Fitmean  = %f is too far away, using %f instead" % (fitmean,mean)
        #too far away, fitting doesnt make sense
        return mean,rms

    fitrms=rms
    if fitrms>5:
        print "Fitrms = %f >5 --> setting it to 5" % fitrms
        fitrms=5.

    hist.Fit("gaus","Q0RI","",fitmean-fitrms,fitmean+fitrms)
    t0=hist.GetFunction("gaus").GetParameter(1)
    t0err=hist.GetFunction("gaus").GetParError(1)

    if abs(t0-mean)>rms:
        print "Problematic fit?????"
        print hist.GetName()
        print "t0=%f t0err=%f entries=%d underflow=%d overflow=%d mean=%f rms=%f" % (t0,t0err,hist.GetEntries(), hist.GetBinContent(0), hist.GetBinContent(101),mean,rms)
        print "fitmean=%f  fitrms=%f" % (fitmean,fitrms)
        print '------'
        return mean,rms

    print "Result from fit: t0=%02.2f +- %02.2f" % (t0,t0err)


    return t0,t0err

def calibratechip_t0(gt0,minentries):
     mint0=1000.
     if TRT.treshist.GetEntries()>minentries and TRT.performT0:
         t0,t0err = analyze_t0(TRT.treshist,"Whole TRT")

         if abs(TRT.t0_from_rt)>5:
             gt0=TRT.t0_from_rt
         
         TRT.updatecalib=True
         TRT.t0=t0+gt0+TRT.oldt0
         print "t0=%02.2f gt0=%02.2f oldt0=%02.2f" % (t0,gt0,TRT.oldt0)
         TRT.t0err=t0err
         if t0<mint0:
             mint0=t0

         for sub in TRT.subdet.itervalues():
             if sub.treshist.GetEntries()>minentries and sub.performT0:
                 name="Subdetector %d" % sub.bec
                 t0,t0err = analyze_t0(sub.treshist,name)
                 if abs(sub.t0_from_rt)>5:
                     gt0=sub.t0_from_rt
                 sub.updatecalib=True
                 sub.t0=t0+gt0+sub.oldt0
                 print "t0=%02.2f gt0=%02.2f oldt0=%02.2f" % (t0,gt0,sub.oldt0)
                 sub.t0err=t0err
                 if t0<mint0:
                     mint0=t0

                 for mod in sub.modules.itervalues():
                     if mod.treshist.GetEntries()>minentries and mod.performT0:
                         name="Module %d/%d/%d" % (mod.bec,mod.sector,mod.layer)
                         t0,t0err = analyze_t0(mod.treshist,name)
                         if abs(mod.t0_from_rt)>5:
                             gt0=mod.t0_from_rt
                         mod.updatecalib=True
                         mod.t0=t0+gt0+mod.oldt0
                         print "t0=%02.2f gt0=%02.2f oldt0=%02.2f" % (t0,gt0,mod.oldt0)	
                         mod.t0err=t0err
                         if t0<mint0:
                             mint0=t0

                         for board in mod.boards.itervalues():
                             if board.treshist.GetEntries()>minentries and board.performT0:
                                 name="Module %d/%d/%d - Board %d" % (mod.bec,mod.sector,mod.layer,board.board)
                                 t0,t0err = analyze_t0(board.treshist,name)
                                 board.updatecalib=True
                                 board.t0=t0+gt0+board.oldt0
                                 board.t0err=t0err
                                 if t0<mint0:
                                     mint0=t0

                                 for chip in board.chips.itervalues():
                                     if chip.treshist.GetEntries()>minentries and chip.performT0:
                                         name="Module %d/%d/%d - Board %d - Chip %d" % (mod.bec,mod.sector,mod.layer,board.board,chip.chip)
                                         t0,t0err = analyze_t0(chip.treshist,name)
                                         chip.updatecalib=True
                                         chip.t0=t0+gt0+chip.oldt0
                                         chip.t0err=t0err
                                         if t0<mint0:
                                             mint0=t0

     return mint0



def calibrate_t0(gt0,minentries):
    mint0=1000.
    
    if TRT.entries>minentries:
        TRT.updatecalib=True

        if TRT.updatecalibRT:
            gt0=TRT.t0_from_rt

        deltat0=TRT.avtimeresidual()+gt0
        
        #don't correct for an R-t bias for big fluctuations as this leads to double counting
        #if abs(deltat0)<5:
        #    deltat0=+gt0
        #apply a damping factor of 0.5
        deltat0=deltat0*0.5
        t0err=TRT.avrawtimeresidualerror()
        t0=TRT.oldt0+deltat0
        TRT.t0=t0
        TRT.deltat0=deltat0
        TRT.t0err=t0err

        if TRT.t0<mint0:
            mint0=TRT.t0

        print "oldt0=%f  deltat0=%f print t0=%f gt0=%f" % (TRT.oldt0,deltat0,t0,gt0)


        for h,sub in TRT.subdet.iteritems():
            if TRT.updatecalibRT:
                gt0=TRT.t0_from_rt

            if sub.entries>minentries:
                sub.updatecalib=True

                if sub.updatecalibRT:
                    gt0=sub.t0_from_rt

                deltat0=sub.avtimeresidual()+gt0
                #if abs(deltat0)<5:
                #    deltat0=+gt0
                #apply a damping factor of 0.5
                deltat0=deltat0*0.5
                t0err=sub.avrawtimeresidualerror()
                t0=sub.oldt0+deltat0
                sub.t0=t0
                sub.t0err=t0err
                sub.deltat0=deltat0

                if sub.t0<mint0:
                    mint0=sub.t0


                for k,v in sub.modules.iteritems():

                    if sub.updatecalibRT:
                        gt0=sub.t0_from_rt

                    if v.entries>minentries:
                        v.updatecalib=True
                
                        if v.updatecalibRT:
                            gt0=v.t0_from_rt

                        deltat0=v.avtimeresidual()+gt0
                        #if abs(deltat0)<5:
                        #    deltat0=+gt0
                        t0err=v.avrawtimeresidualerror()
                        #apply a damping factor of 0.5
                        deltat0=deltat0*0.5
                        t0=v.oldt0+deltat0
                        v.t0=t0
                        v.t0err=t0err
                        v.deltat0=deltat0

                        if v.t0<mint0:
                            mint0=v.t0

                        #print "Module %d %d %d : oldt0=%f  deltat0=%f print t0=%f gt0=%f" % (v.bec,v.layer,v.sector,v.oldt0,deltat0,t0,gt0)

                        for j,c in v.chips.iteritems():
                            if c.entries>minentries:
                                c.updatecalib=True
                                deltat0=c.avtimeresidual()+gt0
                                #if abs(deltat0)<5:
                                #    deltat0=+gt0
                                t0err=c.avrawtimeresidualerror()
                                #apply a damping factor of 0.5
                                deltat0=deltat0*0.5
                                t0=c.oldt0+deltat0
                                c.t0=t0
                                c.t0err=t0err
                                c.deltat0=deltat0

                                if c.t0<mint0:
                                    mint0=c.t0

                                #if c.t0<20:
                                #    print "Chip %02d module %02d layer %02d bec %01d -> t0=%02.4f" % (c.chip,c.sector,c.layer,c.bec, c.t0)
                                #    print "avtimeresidual=%02.4f gt0=%02.4f oldt0=%02.4f" % (c.avtimeresidual(),gt0,c.oldt0)

                                for l,s in c.straws.iteritems():
                                    if s.entries>minentries:
                                        s.updatecalib=True
                                        deltat0=s.avtimeresidual()+gt0
                                        #if abs(deltat0)<5:
                                        #    deltat0=+gt0
                                        #apply a damping factor of 0.5
                                        deltat0=deltat0*0.5
                                        t0err=s.avrawtimeresidualerror()
                                        t0=s.oldt0+deltat0
                                        s.t0=t0
                                        s.t0err=t0err
                                        s.deltat0=deltat0
                                        
                                        if s.t0<mint0:
                                            mint0=s.t0
                                        
                                        #if abs(s.bec)==1 and s.layer==0 and s.strawlayer<9:
                                        #    print "---"
                                        #    print "Straw %d:%d:%d:%d:%d" % (s.bec, s.layer, s.sector, s.strawlayer, s.straw)
                                        #    print "av=%f detla=%f old=%f -> new=%f" % (s.avtimeresidual(), deltat0, gt0, t0)
                                        #    print "Entries: %d" % s.entries
                                            
    return mint0

def delete_gt0():
    if TRT.updatecalibRT:
        TRT.t0_from_rt=0
        for sub in TRT.subdet.itervalues():
            if sub.updatecalibRT:
                sub.t0_from_rt=0
                for mod in sub.modules.itervalues():
                    mod.t0_from_rt=0
                


def correct_global_offset(filename):
    offset=0.
    try:
        lines=open(filename).readlines()
        for i in lines:
            if i.find("#GLOBALOFFSET")>=0:
                tokens=i.split()
                offset=float(tokens[1])
                break
    except:
        print "Cannot read global offset"
        return

    print "Correcting oldt0 by %f" % offset

    #update oldt0
    TRT.oldt0-=offset
    for sub in TRT.subdet.itervalues():
        sub.oldt0-=offset
        for mod in sub.modules.itervalues():
            mod.oldt0-=offset
            for chip in mod.chips.itervalues():
                chip.oldt0-=offset
                for straw in chip.straws.itervalues():
                    straw.oldt0-=offset
                                
 
# ------ Write out the calibration constants as textfile --------

 
def write_rt(outfile):
    if TRT.updatecalibRT:
        outfile.write("# RtRelation\n")
        s="-3 -1 -1 -1 -1 : 0 %e %e %e %e\n" % (TRT.rtpars[0], TRT.rtpars[1], TRT.rtpars[2], TRT.rtpars[3])
        outfile.write(s)

        for sub in TRT.getSubsSorted():
            if sub.updatecalibRT:
                #unfortunately it is not possible to have it directly per subdetector, setting individual modules instead
                #s="%d -1 -1 -1 -1 :  0 %f %f %f %f\n" % (sub.bec, sub.rtpars[0], sub.rtpars[1], sub.rtpars[2], sub.rtpars[3]) 
                #outfile.write(s)

                for v in sub.getModulesSorted():
                    if v.updatecalibRT:
                        s="%d %d %d -1 -1 : 0 %e %e %e %e\n" % (v.bec, v.layer, v.sector, v.rtpars[0], v.rtpars[1], v.rtpars[2], v.rtpars[3]) 
                        outfile.write(s)
                    else:
                        s="%d %d %d -1 -1 : 0 %e %e %e %e\n" % (v.bec, v.layer, v.sector, sub.rtpars[0], sub.rtpars[1], sub.rtpars[2], sub.rtpars[3]) 
                        outfile.write(s)

def write_t0(outfile,corrt0):
    if TRT.updatecalib:
        outfile.write("# StrawT0\n")
        s="-3 -1 -1 -1 -1 : %f %f\n" % (TRT.t0+corrt0, TRT.t0err)
        outfile.write(s)

        for sub in TRT.getSubsSorted():
            if sub.updatecalib:
                #unfortunately it is not possible to have a t0 per sub-detector
                #s="%d -1 -1 -1 -1 : %f %f\n" % (sub.bec, sub.t0, sub.t0err)
                #outfile.write(s)

                for v in sub.getModulesSorted():
                    if v.updatecalib:
                        s="%d %d %d -1 -1 : %f %f\n" % (v.bec, v.layer, v.sector, v.t0+corrt0, v.t0err)
                        outfile.write(s)

                        for b in v.getBoardsSorted():
                            if b.updatecalib:
                                for c in b.getChipsSorted():
                                    if c.updatecalib:
                                        for st in c.getStrawsSorted():
                                            if st.updatecalib:
                                                s="%d %d %d %d %d : %f %f\n" % (st.bec, st.layer, st.sector, st.strawlayer, st.straw, st.t0+corrt0, st.t0err)
                                            else:
                                                s="%d %d %d %d %d : %f %f\n" % (st.bec, st.layer, st.sector, st.strawlayer, st.straw, c.t0+corrt0, c.t0err)
                                    else:
                                        for st in c.getStrawsSorted():
                                            s="%d %d %d %d %d : %f %f\n" % (st.bec, st.layer, st.sector, st.strawlayer, st.straw, b.t0+corrt0, b.t0err)
                                            outfile.write(s)
                
                    else:
                        s="%d %d %d -1 -1 : %f %f\n" % (v.bec, v.layer, v.sector, sub.t0+corrt0, sub.t0err)
                        outfile.write(s)
    s="#GLOBALOFFSET %2.4f" % corrt0
    outfile.write(s)




def write_calibfile(outname,corrt0):
    outfile=open(outname,"w")
    outfile.write("# Fileformat=1\n")
    write_rt(outfile)
    write_t0(outfile,corrt0)
    outfile.close()

    #also write out the files separately
    outfile=open(outname+"_rt","w")
    outfile.write("# Fileformat=1\n")
    write_rt(outfile)
    outfile.close()

    outfile=open(outname+"_t0","w")
    write_t0(outfile,corrt0)
    outfile.close()




# ------ General root setup --------

def rootlogon():
    gROOT.SetStyle("Plain")
    gROOT.ForceStyle()
    gStyle.GetAttDate().SetTextColor(1)    
    gStyle.GetAttDate().SetTextSize(0.02)  
    gStyle.GetAttDate().SetTextFont(42)    
    gStyle.SetTitleX(0.01)
    gStyle.SetTitleY(0.99)
    gStyle.SetMarkerColor(1)
    #gStyle.SetMarkerStyle(20)
    #gStyle.SetMarkerSize(0.7)
    gStyle.SetErrorX(0.0)
    gStyle.SetOptFit(0)
    gStyle.SetPalette(1)


# ------ Some summary histograms showing statistics and t0 distributions --------

def create_summary_hists():
    rootlogon()
    histstrawentries=TH1F("Strawhits","Entries per Straw",100,0.,100.)
    histchipentries=TH1F("Chiphits","Entries per Chip",100,0.,1000.)
    histboardentries=TH1F("Boardhits","Entries per Board",100,0.,1000.)
    histmoduleentries=TH1F("Modulehits","Entries per Module",100,0.,20000.)
    histsubdetentries=TH1F("BEChits","Entries per subdetector",100,0.,10000000)

    histstrawt0=TH1F("StrawT0","Straw t0",100,-10.,40.)
    histchipt0=TH1F("ChipT0","Chip t0",100,-10.,40.)
    histboardt0=TH1F("BoardT0","Board t0",100,-10.,40.)
    histmodulet0=TH1F("ModuleT0","Module t0",100,-10.,40.)
    histTRTt0=TH1F("TRTBECT0","TRT BEC t0",100,-10.,40.)

    for sub in TRT.subdet.itervalues():
        count_s=0
        for module in sub.modules.itervalues():
            count_m=0
            for board in module.boards.itervalues():
                count_b=0
                for chip in board.chips.itervalues():
                    count_c=0
                    for straw in chip.straws.itervalues():
                        histstrawentries.Fill(straw.entries)
                        count_c+=straw.entries
                        if straw.updatecalib:
                            histstrawt0.Fill(straw.t0)
                    count_b+=count_c
                    histchipentries.Fill(count_c)
                    if chip.updatecalib:
                        histchipt0.Fill(chip.t0)

                count_m+=count_b
                histboardentries.Fill(count_c)
                if board.updatecalib:
                    histboardt0.Fill(board.t0)
            count_s+=count_m
            histmoduleentries.Fill(count_m)
            if module.updatecalib:
                histmodulet0.Fill(module.t0)
        histsubdetentries.Fill(count_s)
        if sub.updatecalib:
            histTRTt0.Fill(sub.t0)
            

    gStyle.SetOptStat(111111)
    
    c=TCanvas()
    c.Divide(2,2)
    c.cd(1)
    histstrawentries.Draw()
    c.cd(2)
    histchipentries.Draw()
    c.cd(3)
    histmoduleentries.Draw()
    c.cd(4)
    histsubdetentries.Draw()
    c.Update()
    c.SaveAs("hitsummary.pdf")

    c.Clear()
    c.Divide(2,2)
    c.cd(1)
    histstrawt0.Draw()
    c.cd(2)
    histchipt0.Draw()
    c.cd(3)
    histmodulet0.Draw()
    c.cd(4)
    histTRTt0.Draw()
    c.Update()
    c.SaveAs("t0summary.pdf")



# ------ Display the r-t relation(s) --------

def display_rt():
    rootlogon()
    if TRT.updatecalibRT:

        gStyle.SetOptStat(0)

        c=TCanvas()
        p1=TPad("pad_rt","RT-Pad",0.002,0.002,0.498,0.998)
        p2=TPad("pad_h1","Fit_1",0.502,0.667,0.748,0.999)
        p3=TPad("pad_h2","Fit_2",0.752,0.667,0.998,0.999)
        p4=TPad("pad_h3","Fit_3",0.502,0.334,0.748,0.666)
        p5=TPad("pad_h4","Fit_4",0.752,0.334,0.998,0.666)
        p6=TPad("pad_h5","Fit_5",0.502,0.001,0.748,0.333)
        p7=TPad("pad_h6","Fit_6",0.752,0.001,0.998,0.333)

        TRT.hist.SetTitle("Whole TRT")

        p1.Draw()
        p1.cd()
        TRT.hist.Draw("COLZ")
        c.cd()
        p2.Draw()
        p2.cd()
        TRT.hlist[0].Draw()
        
        c.cd()
        p3.Draw()
        p3.cd()
        TRT.hlist[4].Draw()

        c.cd()
        p4.Draw()
        p4.cd()
        TRT.hlist[8].Draw()

        c.cd()
        p5.Draw()
        p5.cd()
        TRT.hlist[12].Draw()

        c.cd()
        p6.Draw()
        p6.cd()
        TRT.hlist[16].Draw()

        c.cd()
        p7.Draw()
        p7.cd()
        TRT.hlist[19].Draw()


        c.Update()
        name="TRT_rtcalib.pdf"
        c.SaveAs(name)
        
        for h,sub in TRT.subdet.iteritems():
            if sub.updatecalibRT:
                c.Clear()
                #c=TCanvas()
                p1=TPad("pad_rt","RT-Pad",0.002,0.002,0.498,0.998)
                p2=TPad("pad_h1","Fit_1",0.502,0.667,0.748,0.999)
                p3=TPad("pad_h2","Fit_2",0.752,0.667,0.998,0.999)
                p4=TPad("pad_h3","Fit_3",0.502,0.334,0.748,0.666)
                p5=TPad("pad_h4","Fit_4",0.752,0.334,0.998,0.666)
                p6=TPad("pad_h5","Fit_5",0.502,0.001,0.748,0.333)
                p7=TPad("pad_h6","Fit_6",0.752,0.001,0.998,0.333)
                
                hn="TRT bec= %d" % (sub.bec)
                sub.hist.SetTitle(hn)
                
                p1.Draw()
                p1.cd()
                sub.hist.Draw("COLZ")
                c.cd()
                p2.Draw()
                p2.cd()
                sub.hlist[0].Draw()
                
                c.cd()
                p3.Draw()
                p3.cd()
                sub.hlist[4].Draw()
                
                c.cd()
                p4.Draw()
                p4.cd()
                sub.hlist[8].Draw()
                
                c.cd()
                p5.Draw()
                p5.cd()
                sub.hlist[12].Draw()

                c.cd()
                p6.Draw()
                p6.cd()
                sub.hlist[16].Draw()
                
                c.cd()
                p7.Draw()
                p7.cd()
                sub.hlist[19].Draw()
                
                c.Update()
                name="TRT_rtcalib_bec_%d.pdf" % (sub.bec)
                c.SaveAs(name)

                for k,mod in sub.modules.iteritems():
                    if mod.updatecalibRT:
                        c.Clear()

                        p1=TPad("pad_rt","RT-Pad",0.002,0.002,0.498,0.998)
                        p2=TPad("pad_h1","Fit_1",0.502,0.667,0.748,0.999)
                        p3=TPad("pad_h2","Fit_2",0.752,0.667,0.998,0.999)
                        p4=TPad("pad_h3","Fit_3",0.502,0.334,0.748,0.666)
                        p5=TPad("pad_h4","Fit_4",0.752,0.334,0.998,0.666)
                        p6=TPad("pad_h5","Fit_5",0.502,0.001,0.748,0.333)
                        p7=TPad("pad_h6","Fit_6",0.752,0.001,0.998,0.333)
                        
                        hn="TRT module %d:%d:%d" % (mod.bec, mod.layer, mod.sector)
                        mod.hist.SetTitle(hn)
                        
                        p1.Draw()
                        p1.cd()
                        mod.hist.Draw("COLZ")
                        c.cd()
                        p2.Draw()
                        p2.cd()
                        mod.hlist[0].Draw()
                        
                        c.cd()
                        p3.Draw()
                        p3.cd()
                        mod.hlist[4].Draw()
                        
                        c.cd()
                        p4.Draw()
                        p4.cd()
                        mod.hlist[8].Draw()
                        
                        c.cd()
                        p5.Draw()
                        p5.cd()
                        mod.hlist[12].Draw()
                        
                        c.cd()
                        p6.Draw()
                        p6.cd()
                        mod.hlist[16].Draw()
                        
                        c.cd()
                        p7.Draw()
                        p7.cd()
                        mod.hlist[19].Draw()

                        c.Update()
                        name="TRT_rtcalib_module_%d_%d_%d.pdf" % (mod.bec,mod.layer,mod.sector)
                        c.SaveAs(name)


# ------ Show the detector illumination --------

def display_occ():
    h=TH2F("occupancy","Detector illumination",36,-17.,17.,32,0.,32.)

    hdict={}
    
    hdict[-2]=TH2F("occ_-2","Negative Endcap illumination",14,0.,14.,32,0.,32.)
    hdict[2]=TH2F("occ_+2","Positive Endcap illumination",14,0.,14.,32,0.,32.)
    hdict[-1]=TH2F("occ_-1","Negative Barrel illumination",3,0.,3.,32,0.,32.)
    hdict[1]=TH2F("occ_+1","Positive Barrel illumination",3,0.,3.,32,0.,32.)

    for sub in TRT.subdet.itervalues():
        for mod in sub.modules.itervalues():
            biny=mod.sector+1
            binx=0
            if mod.bec==-2:
                binx=14-mod.layer
            elif mod.bec==-1:
                binx=18-mod.layer
            elif mod.bec==1:
                binx=mod.layer+19
            else:
                binx=mod.layer+23

            h.SetBinContent(binx,biny,mod.entries)
            hdict[sub.bec].SetBinContent(mod.layer+1,mod.sector+1,mod.entries)

    gStyle.SetPadRightMargin(0.15)

    c=TCanvas()
    h.Draw("COLZ")
    c.Update()
    c.SaveAs("occupancy_trt.pdf")

    max=hdict[-2].GetMaximum()
    if max>hdict[2].GetMaximum():
        hdict[2].SetMaximum(max)
    else:
        hdict[-2].SetMaximum(hdict[2].GetMaximum())

    max=hdict[-1].GetMaximum()
    if max>hdict[1].GetMaximum():
        hdict[1].SetMaximum(max)
    else:
        hdict[-1].SetMaximum(hdict[1].GetMaximum())


    min=hdict[-2].GetMinimum()
    if min<hdict[2].GetMinimum():
        hdict[2].SetMinimum(min)
    else:
        hdict[-2].SetMinimum(hdict[2].GetMinimum())

    min=hdict[-1].GetMinimum()
    if min<hdict[1].GetMinimum():
        hdict[1].SetMinimum(min)
    else:
        hdict[-1].SetMinimum(hdict[1].GetMinimum())


    c.Clear()
    c.Divide(2,2)
    c.cd(1)
    hdict[-2].Draw("COLZ")
    c.cd(2)
    hdict[2].Draw("COLZ")
    c.cd(3)
    hdict[-1].Draw("COLZ")
    c.cd(4)
    hdict[1].Draw("COLZ")
    c.Update()
    c.SaveAs("occupancy_sub.pdf")


def display_t0():
    h=TH2F("t0","t0 distribution",36,-17.,17.,32,0.,32.)

    hdict={}
    
    hdict[-2]=TH2F("t0_-2","Negative Endcap t0 distribution",14,0.,14.,32,0.,32.)
    hdict[2]=TH2F("t0_+2","Positive Endcap t0 distribution",14,0.,14.,32,0.,32.)
    hdict[-1]=TH2F("t0_-1","Negative Barrel t0 distribution",3,0.,3.,32,0.,32.)
    hdict[1]=TH2F("t0_+1","Positive Barrel t0 distribution",3,0.,3.,32,0.,32.)

    for sub in TRT.subdet.itervalues():
        for mod in sub.modules.itervalues():
            biny=mod.sector+1
            binx=0
            if mod.bec==-2:
                binx=14-mod.layer
            elif mod.bec==-1:
                binx=18-mod.layer
            elif mod.bec==1:
                binx=mod.layer+19
            else:
                binx=mod.layer+23

            if mod.updatecalib:
                h.SetBinContent(binx,biny,mod.t0)
                hdict[sub.bec].SetBinContent(mod.layer+1,mod.sector+1,mod.t0)
            elif sub.updatecalib:
                h.SetBinContent(binx,biny,sub.t0)
                hdict[sub.bec].SetBinContent(mod.layer+1,mod.sector+1,sub.t0)

    gStyle.SetPadRightMargin(0.15)

    h.SetMinimum(h.GetMinimum())

    c=TCanvas()
    h.Draw("COLZ")
    c.Update()
    c.SaveAs("t0_trt.pdf")

    max=hdict[-2].GetMaximum()
    if max>hdict[2].GetMaximum():
        hdict[2].SetMaximum(max)
    else:
        hdict[-2].SetMaximum(hdict[2].GetMaximum())

    max=hdict[-1].GetMaximum()
    if max>hdict[1].GetMaximum():
        hdict[1].SetMaximum(max)
    else:
        hdict[-1].SetMaximum(hdict[1].GetMaximum())


    min=hdict[-2].GetMinimum()
    if min<hdict[2].GetMinimum():
        hdict[2].SetMinimum(min)
    else:
        hdict[-2].SetMinimum(hdict[2].GetMinimum())

    min=hdict[-1].GetMinimum()
    if min<hdict[1].GetMinimum():
        hdict[1].SetMinimum(min)
    else:
        hdict[-1].SetMinimum(hdict[1].GetMinimum())


    c.Clear()
    c.Divide(2,2)
    c.cd(1)
    hdict[-2].Draw("COLZ")
    c.cd(2)
    hdict[2].Draw("COLZ")
    c.cd(3)
    hdict[-1].Draw("COLZ")
    c.cd(4)
    hdict[1].Draw("COLZ")
    c.Update()
    c.SaveAs("t0_sub.pdf")


    pdict={}
    fitf=TF1("fitf","pol0")
    for i in hdict.keys():
        htmp=TH1F("htmp_"+hdict[i].GetName(),"temp",hdict[i].GetNbinsY(),hdict[i].GetYaxis().GetBinLowEdge(1),hdict[i].GetYaxis().GetBinUpEdge(hdict[i].GetNbinsY()))

        hnew=TH1F("prof_"+hdict[i].GetName(),"t0 vs. layer in subdet "+repr(i),hdict[i].GetNbinsX(),hdict[i].GetXaxis().GetBinLowEdge(1),hdict[i].GetXaxis().GetBinUpEdge(hdict[i].GetNbinsX()))

        for x in range(1,hdict[i].GetNbinsX()+1):
            htmp.Clear()
            for j in range(hdict[i].GetNbinsY()+1):
                htmp.SetBinContent(j,hdict[i].GetBinContent(x,j))
            htmp.Fit(fitf,"Q")
            mean=fitf.GetParameter(0)
            error=fitf.GetParError(0)
            #print "BEC=%d bin=%d :  mean=%f +- %f" % (i,x,mean,error)
            hnew.SetBinContent(x,mean)
            hnew.SetBinError(x,error)
        
        pdict[i]=hnew

    #show the t0 vs layer as profile plot
    c.Clear()
    c.Divide(2,2)
    c.cd(1)
    pdict[-2].Draw()
    c.cd(2)
    pdict[2].Draw()
    c.cd(3)
    pdict[-1].Draw()
    c.cd(4)
    pdict[1].Draw()
    c.Update()
    c.SaveAs("t0_sub_prof.pdf")



def display_deltat0():
    h=TH2F("deltat0","t0 distribution",36,-17.,17.,32,0.,32.)

    hdict={}
    
    hdict[-2]=TH2F("deltat0_-2","Negative Endcap #Delta t0 distribution",14,0.,14.,32,0.,32.)
    hdict[2]=TH2F("deltat0_+2","Positive Endcap #Delta t0 distribution",14,0.,14.,32,0.,32.)
    hdict[-1]=TH2F("deltat0_-1","Negative Barrel #Delta t0 distribution",3,0.,3.,32,0.,32.)
    hdict[1]=TH2F("deltat0_+1","Positive Barrel #Delta t0 distribution",3,0.,3.,32,0.,32.)

    for sub in TRT.subdet.itervalues():
        for mod in sub.modules.itervalues():
            biny=mod.sector+1
            binx=0
            if mod.bec==-2:
                binx=14-mod.layer
            elif mod.bec==-1:
                binx=18-mod.layer
            elif mod.bec==1:
                binx=mod.layer+19
            else:
                binx=mod.layer+23

            if mod.updatecalib:
                h.SetBinContent(binx,biny,mod.avtimeresidual())
                hdict[sub.bec].SetBinContent(mod.layer+1,mod.sector+1,mod.avtimeresidual())
            elif sub.updatecalib:
                h.SetBinContent(binx,biny,sub.avtimeresidual())
                hdict[sub.bec].SetBinContent(mod.layer+1,mod.sector+1,sub.avtimeresidual())

    gStyle.SetPadRightMargin(0.15)

    h.SetMinimum(h.GetMinimum())

    c=TCanvas()
    h.Draw("COLZ")
    c.Update()
    c.SaveAs("deltat0_trt.pdf")

    max=hdict[-2].GetMaximum()
    if max>hdict[2].GetMaximum():
        hdict[2].SetMaximum(max)
    else:
        hdict[-2].SetMaximum(hdict[2].GetMaximum())

    max=hdict[-1].GetMaximum()
    if max>hdict[1].GetMaximum():
        hdict[1].SetMaximum(max)
    else:
        hdict[-1].SetMaximum(hdict[1].GetMaximum())


    min=hdict[-2].GetMinimum()
    if min<hdict[2].GetMinimum():
        hdict[2].SetMinimum(min)
    else:
        hdict[-2].SetMinimum(hdict[2].GetMinimum())

    min=hdict[-1].GetMinimum()
    if min<hdict[1].GetMinimum():
        hdict[1].SetMinimum(min)
    else:
        hdict[-1].SetMinimum(hdict[1].GetMinimum())


    c.Clear()
    c.Divide(2,2)
    c.cd(1)
    hdict[-2].Draw("COLZ")
    c.cd(2)
    hdict[2].Draw("COLZ")
    c.cd(3)
    hdict[-1].Draw("COLZ")
    c.cd(4)
    hdict[1].Draw("COLZ")
    c.Update()
    c.SaveAs("deltat0_sub.pdf")




def display_residuals():
    h=TH2F("res","Residual mean distribution",36,-17.,17.,32,0.,32.)
    h2=TH2F("resw","Residual width distribution",36,-17.,17.,32,0.,32.)

    hdict={}
    hdict2={}

    hdict[-2]=TH2F("res_-2","Negative Endcap <res> distribution",14,0.,14.,32,0.,32.)
    hdict[2]=TH2F("res_+2","Positive Endcap <res> distribution",14,0.,14.,32,0.,32.)
    hdict[-1]=TH2F("res_-1","Negative Barrel <res> distribution",3,0.,3.,32,0.,32.)
    hdict[1]=TH2F("res_+1","Positive Barrel <res> distribution",3,0.,3.,32,0.,32.)

    hdict2[-2]=TH2F("resw_-2","Negative Endcap sigma(res) distribution",14,0.,14.,32,0.,32.)
    hdict2[2]=TH2F("resw_+2","Positive Endcap sigma(res) distribution",14,0.,14.,32,0.,32.)
    hdict2[-1]=TH2F("resw_-1","Negative Barrel sigma(res) distribution",3,0.,3.,32,0.,32.)
    hdict2[1]=TH2F("resw_+1","Positive Barrel sigma(res) distribution",3,0.,3.,32,0.,32.)

    for sub in TRT.subdet.itervalues():
        for mod in sub.modules.itervalues():
            biny=mod.sector+1
            binx=0
            if mod.bec==-2:
                binx=14-mod.layer
            elif mod.bec==-1:
                binx=18-mod.layer
            elif mod.bec==1:
                binx=mod.layer+19
            else:
                binx=mod.layer+23

            #print "%d:%d:%d" % (mod.bec,mod.sector,mod.layer)
            if mod.updatecalib:
                h.SetBinContent(binx,biny,mod.resmean)
                h2.SetBinContent(binx,biny,mod.reswidth)
                hdict[sub.bec].SetBinContent(mod.layer+1,mod.sector+1,mod.resmean)
                hdict2[sub.bec].SetBinContent(mod.layer+1,mod.sector+1,mod.reswidth)

    gStyle.SetPadRightMargin(0.15)

    h.SetMinimum(h.GetMinimum())

    c=TCanvas()
    h.Draw("COLZ")
    c.Update()
    c.SaveAs("res_trt.pdf")

    c.Clear()
    h2.Draw("COLZ")
    c.Update()
    c.SaveAs("reswidth_trt.pdf")


    max=hdict[-2].GetMaximum()
    if max>hdict[2].GetMaximum():
        hdict[2].SetMaximum(max)
    else:
        hdict[-2].SetMaximum(hdict[2].GetMaximum())

    max=hdict[-1].GetMaximum()
    if max>hdict[1].GetMaximum():
        hdict[1].SetMaximum(max)
    else:
        hdict[-1].SetMaximum(hdict[1].GetMaximum())


    min=hdict[-2].GetMinimum()
    if min<hdict[2].GetMinimum():
        hdict[2].SetMinimum(min)
    else:
        hdict[-2].SetMinimum(hdict[2].GetMinimum())

    min=hdict[-1].GetMinimum()
    if min<hdict[1].GetMinimum():
        hdict[1].SetMinimum(min)
    else:
        hdict[-1].SetMinimum(hdict[1].GetMinimum())


    max=hdict2[-2].GetMaximum()
    if max>hdict2[2].GetMaximum():
        hdict2[2].SetMaximum(max)
    else:
        hdict2[-2].SetMaximum(hdict2[2].GetMaximum())

    max=hdict2[-1].GetMaximum()
    if max>hdict2[1].GetMaximum():
        hdict2[1].SetMaximum(max)
    else:
        hdict2[-1].SetMaximum(hdict2[1].GetMaximum())


    min=hdict2[-2].GetMinimum()
    if min<hdict2[2].GetMinimum():
        hdict2[2].SetMinimum(min)
    else:
        hdict2[-2].SetMinimum(hdict2[2].GetMinimum())

    min=hdict2[-1].GetMinimum()
    if min<hdict2[1].GetMinimum():
        hdict2[1].SetMinimum(min)
    else:
        hdict2[-1].SetMinimum(hdict2[1].GetMinimum())



    #overwrite
    hdict[-2].SetMinimum(0.1)
    hdict[-1].SetMinimum(0.1)
    hdict[1].SetMinimum(0.1)
    hdict[2].SetMinimum(0.1)

    c.Clear()
    c.Divide(2,2)
    c.cd(1)
    hdict[-2].Draw("COLZ")
    c.cd(2)
    hdict[2].Draw("COLZ")
    c.cd(3)
    hdict[-1].Draw("COLZ")
    c.cd(4)
    hdict[1].Draw("COLZ")
    c.Update()
    c.SaveAs("resmean_sub.pdf")


    c.Clear()
    c.Divide(2,2)
    c.cd(1)
    hdict2[-2].Draw("COLZ")
    c.cd(2)
    hdict2[2].Draw("COLZ")
    c.cd(3)
    hdict2[-1].Draw("COLZ")
    c.cd(4)
    hdict2[1].Draw("COLZ")
    c.Update()
    c.SaveAs("reswidth_sub.pdf")

    c.Clear()
    gStyle.SetOptFit(1)
    c.Divide(1,1)
    c.cd(1)
    TRT.reshist.Draw()
    TRT.reshist.Fit("gaus","RI","",-0.3,0.3)
    c.Update()
    c.SaveAs("residual_overall_03.pdf")

    c.Clear()
    gStyle.SetOptFit(1)
    c.Divide(1,1)
    c.cd(1)
    TRT.reshist.Draw()
    TRT.reshist.Fit("gaus","RI","",-0.4,0.4)
    c.Update()
    c.SaveAs("residual_overall_04.pdf")

    c.Clear()
    gStyle.SetOptFit(1)
    c.Divide(1,1)
    c.cd(1)
    TRT.reshist.Draw()
    TRT.reshist.Fit("gaus","RI","",-0.5,0.5)
    c.Update()
    c.SaveAs("residual_overall_05.pdf")



def missing_straws():
    missing=[]
    found={}
    for sub in TRT.subdet.itervalues():
        for mod in sub.modules.itervalues():
            for board in mod.boards.itervalues():
	         for chip in board.chips.itervalues():
        	      for straw in chip.straws.itervalues():
                   	 name="%d:%d:%d:%d:%d" % (straw.bec, straw.layer, straw.sector, straw.strawlayer, straw.straw)
                   	 found[name]=True

    for sub in TRT.subdet.itervalues():
        for mod in sub.modules.itervalues():
            for board in mod.boards.itervalues():
	        for chip in mod.chips.itervalues():
        	    for straw in chip.straws.itervalues():
                	name2="%d:%d:%d:%d:%d" % (-straw.bec, straw.layer, straw.sector, straw.strawlayer, straw.straw)
                   	if not name2 in found:
                       	      missing.append(name2)

    print "%d straws are missing!!" % len(missing)
    for i in missing:
        print i


def t0_vs_strawlayer_barrel():
    slt0={}
    sle={}
    for mod in TRT.subdet[-1].modules.itervalues():
        if mod.layer==0:
            for chip in mod.chips.itervalues():
                for straw in chip.straws.itervalues():
                    if straw.strawlayer in slt0:
                        if straw.updatecalib:
                            slt0[straw.strawlayer]+=straw.t0
                        else:
                            slt0[straw.strawlayer]+=chip.t0
                        sle[straw.strawlayer]+=1
                    else:
                        if straw.updatecalib:
                            slt0[straw.strawlayer]=straw.t0
                        else:
                            slt0[straw.strawlayer]=chip.t0   
                        sle[straw.strawlayer]=1

    for k,v in slt0.iteritems():
        print "========="
        print "sum t0: %f   entries=%d" % (v,sle[k])
        av=v/sle[k]
        print "Strawlayer %d has average t0 of %f" % (k,av)

# ------ Write merged root-tuple --------                        

def write_rootfile():
    out=TFile("merged_trtcalib.root","RECREATE")
    hlist=[]
    #first we write the hists for all the modules
    for sub in TRT.subdet.itervalues():
        for m in sub.modules.itervalues():
            path="Module_%d_%d_%d" % (m.bec,m.sector,m.layer)
            #hist=m.hist.Clone("ttodcalibh2")
            hist=m.hist.__class__(m.hist)
            hist.SetName("ttodcalibh2")
            #reshist=m.reshist.Clone("resh1")
            reshist=m.reshist.__class__(m.reshist)
            reshist.SetName("resh1")
            out.mkdir(path).cd()
            hist.SetDirectory(gDirectory)
            #hlist.append(hist)
            hist.Write()
            reshist.Write()
            out.cd()

    #then we write the strawtuple
    nt=TNtuple("strawtuple","straw calibration tuple",
               "bec:mod:lay:strawlay:straw:chip:board:num:t0:srawr:srawrq:oldt0:gt0:sumw2")
    for sub in TRT.subdet.itervalues():
        for mod in sub.modules.itervalues():
            for chip in mod.chips.itervalues():
                for straw in chip.straws.itervalues():
                    nt.Fill(straw.bec,straw.sector,straw.layer,straw.strawlayer,straw.straw,chip.chip,-1.,straw.entries,straw.sumtimeres,straw.sumrawres,straw.sumrawresq,straw.oldt0,0.,straw.sumw)
    nt.Write()
    out.Close()


# ------ Main routine --------

ROOT.gErrorIgnoreLevel = 5000

#ROOT.SetMemoryPolicy( ROOT.kMemoryStrict )

start=time.time()
start2=start
tdict={}

list=get_filelist()
tdict['filelist']=time.time()-start2
start2=time.time()

build_moduledict(list)
tdict['moduledict']=time.time()-start2
start2=time.time()

chain=build_chain(list)
tdict['chain']=time.time()-start2
start2=time.time()

straws=build_strawdict(chain)
tdict['strawdict']=time.time()-start2
start2=time.time()

#write_rootfile()
#tdict['writerootfile']=time.time()-start2
#start2=time.time()

calculate_tree()
tdict['calctree']=time.time()-start2
start2=time.time()


##setRTlevel("TRT")
##setT0level("CHIP")

setRTlevel("MODULE")
setT0level("CHIP")


#correct for global offset
correct_global_offset("old_calibconstants.txt")

if sys.argv[2].find("RT")>=0:
    calibrate_rt(10000,0,2,False) # was True
else:
    calibrate_rt(10000,0,2,False)
tdict['calib_rt']=time.time()-start2
start2=time.time()

#reset global t0 from RT in case we are in the first iteration
    #if sys.argv[2].find("first")>=0:
    #    delete_gt0()

#delete_gt0()

mint0=calibratechip_t0(0.,200)
tdict['calib_t0']=time.time()-start2
start2=time.time()

if mint0<0:
    mint0=abs(mint0)+0.5
    print
    print '**********'
    print "Correct t0 by %f" % mint0
    print '**********'
    print
else:
    if mint0>0.5:
        mint0=-mint0+0.5
    else:
        mint0=0.

#t0_vs_strawlayer_barrel()

mint0=0.
#debug_TRT()

write_calibfile("calibout.txt",mint0)
tdict['write_calib']=time.time()-start2
start2=time.time()

if not sys.argv[2].find("RT")>=0:
    rootlogon()
    gStyle.SetOptStat(0)

#sys.exit(0)
create_summary_hists()
display_rt()
display_occ()
display_t0()
display_deltat0()
display_residuals()
#missing_straws()
tdict['create_plots']=time.time()-start2


print "Total number of straws seen: %d" % (TRT.getNStraws())
for i in TRT.subdet.itervalues():
    print "Subdet %d : %d" % (i.bec, i.getNStraws())


endt=time.time()-start

print "Process needed %d seconds" % endt
print "Detailed listing:"
for k,v in tdict.iteritems():
    print "\t%s : %d" % (k,v)
