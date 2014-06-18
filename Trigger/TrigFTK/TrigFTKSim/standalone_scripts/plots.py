#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os,sys

_IGNORE_ROADS = False
_IGNORE_TRACKS = False
_ATTACH_HITS = True
_LOOP_OVER_TRACKS = False
_FILL_HISTOS = False
_PERCENTILES = False

print "Loading ROOT environment..."
try:
    import ROOT
except:
    print "Couldn't import ROOT module"
    print "Try using the version of python that comes with Athena"
    print ">>   XX.X.X/sw/lcg/external/Python/"
    sys.exit(0)

def m(data):
    """ Computes the average of the list"""
    try:
        res=sum(data)*1.0/len(data)
    except:
        res=-1
    return res
def mi(data):
    """ Computes the int(average of the list)"""
    return int(m(data))

def pctl(a_list):
    """ Returns a 3-tuple of the 50th, 90th, and 99th percentile in the list """
    try:
        a_list.sort()
        listlen = len(a_list)
        res = (a_list[int(0.5*listlen)],a_list[int(0.9*listlen)],a_list[int(0.99*listlen)])
    except:
        res = (-1,-1,-1)
    return res

class RootPlotter:
    """ Base class for processing ftksim output """
    def __init__(self,libftk,banks,froads,ftracks,nplanes):
        """
        libftk is full path to libftk_classes.so
        banks is a list of banks
        froads is the path to merged road files. E.g., merged/raw_11L_reg%d.root
        ftracks is the path to final tracks (if present). E.g., tracks/raw_11L_reg%d.root
        """
        self.banks=banks
        self.froads=froads
        self.ftracks=ftracks
        self.nplanes=nplanes
        self.libftk=libftk
        self.plotdir = './'
        self.doDump=False
        ROOT.gSystem.Load(self.libftk)
    def SetPlotDir(self,dir):
        dir = os.path.expanduser(dir)
        dir = os.path.abspath(dir)
        self.plotdir = dir
    def SetDump(self,v):
        if v != None:
            self.doDump=True
    def GetPlotDir(self):
        return self.plotdir
    def Load(self, use_merged_tracks):
        """ Sets access to TTree's and does some sanity checks """
        self.roads=[]
        self.tracks=[]
        nfiles=[]
        nentries=[]
        for i in self.banks:
            print 'Working on bank',i
            try:
                roads=ROOT.FTKRoadStream()
                if self.ftracks is not None:
                    tracks=ROOT.FTKTrackStream()
            except:
                print "Couldn't import libftk module, or wrong libftk version"
                print "Check location and compilation settings:",self.libftk
                return -1
            # Roads tree
            if not _IGNORE_ROADS:
                print "Loading merged roads:",self.froads%i
                tree1=ROOT.TChain("ftkdata","Merged data")
                ROOT.SetOwnership(tree1, False )
                nfiles1=tree1.Add(self.froads%i)
                nentries1=tree1.GetEntries()
                if nfiles1==0 or nentries1==0:
                    print "Didn't find any events in road TChain in bank",i
                    print "Check the path to root file:",self.froads%i
                    return -1
                tree1.SetBranchAddress("FTKBank%d."%i,ROOT.AddressOf(roads))
                self.roads.append((roads,tree1,nfiles1,nentries1,i))
                nfiles.append(nfiles1)
                nentries.append(nentries1)
                print 'Added',nfiles1,'road files with',nentries1,'entries in bank #',i
            # Tracks tree
            if self.ftracks is not None and not _IGNORE_TRACKS:
                print "Loading tracks:",self.ftracks%i
                tree2=ROOT.TChain("ftkdata","Tracks")
                ROOT.SetOwnership(tree2, False )
                nfiles2=tree2.Add(self.ftracks%i)
                nentries2=tree2.GetEntries()
                if nfiles2==0 or nentries2==0:
                    print "Didn't find any events in track TChain in bank",i
                    print "Check the path to root file:",self.ftracks%i
                    return -1
                if use_merged_tracks=='1':
                    tree2.SetBranchAddress("FTKBankMerged",ROOT.AddressOf(tracks))
                else:
                    tree2.SetBranchAddress("FTKBank%d."%i,ROOT.AddressOf(tracks))
                self.tracks.append((tracks,tree2,nfiles2,nentries2,i))
                nfiles.append(nfiles2)
                nentries.append(nentries2)
                print 'Added',nfiles2,'track files with',nentries2,'entries in bank #',i
            if _IGNORE_ROADS:
                self.roads = [None for i in self.tracks]
            if _IGNORE_TRACKS:
                self.tracks = [None for i in self.roads]
        if len(set(nfiles)) != 1:
            print 'Error: different number of files in different regions:',nfiles
            return -1
        if len(set(nentries)) != 1:
            print 'Error: different number of events in different regions:',nentries
            return -1
        self.nentries=nentries[0]
    def GetEntry(self,n):
        """ Retrieves the same event from all underlying root files """
        if not _IGNORE_ROADS:
            for road in self.roads:
                road[1].GetEntry(n)
        if not _IGNORE_TRACKS:                
            for track in self.tracks:
                track[1].GetEntry(n)
    def Loop(self,nentries=int(1e10)):
        """ Main program loop """
        self.BookHistos()
        self.BookCounters()
        n=min(self.nentries,nentries)
        print '[',
        for iev in range(n):
            if iev%(n/30+1)==0 and not self.doDump:
                print '#',
                sys.stdout.flush()
            # Get entry from all open root files
            self.GetEntry(iev)
            # counters
            nroads=0
            nroads_good=0
            ntracks=0
            # loop over banks
            for roads,tracks in zip(self.roads,self.tracks):
                if self.doDump:
                    if not _IGNORE_ROADS:  roads[0].Print(True)
                    if not _IGNORE_TRACKS: tracks[0].Print()
                curroads=0
                curroads4L=0
                curtracks=0
                if not _IGNORE_ROADS:
                    curroads+=roads[0].getNRoads()
                    curroads4L+=roads[0].getN4LRoads()
                    i=roads[4]
                    # naoki counters
                    self.m_nao_nhits_tot[i].append(roads[0].naoGetNhitsTot())
                    self.m_nao_nclus_tot[i].append(roads[0].naoGetNclusTot())
                    self.m_nao_nroads_am[i].append(roads[0].naoGetNroadsAM())
                    self.m_nao_nroads_mod[i].append(roads[0].naoGetNroadsMOD())
                    self.m_nao_nroads_rw[i].append(roads[0].naoGetNroadsRW())
                    for pl in range(self.nplanes):
                        self.m_nao_nclus[i][pl].append(roads[0].naoGetNclus(pl))
                        self.m_nao_nss[i][pl].append(roads[0].naoGetNss(pl))
                if not _IGNORE_TRACKS:
                    curtracks+=tracks[0].getNTracks()
                    i=tracks[4]
                nroads+=curroads
                ntracks+=curtracks
                # loop over roads
                if not _IGNORE_ROADS:
                    mean_ncomb=[]
                    mean_sscnt=[list() for pl in range(self.nplanes)]
                    for ir in range(curroads):
                        road=roads[0].getRoad(ir)
                        if not road.getRWRejected():
                            nroads_good += 1
                        # example accessing detailed info about each road (e.g., # of hits in each SS)
                        if _ATTACH_HITS:
                            roads[0].attachHits(ir)
                            ncomb=1
                            for pl in range(self.nplanes):
                                nhits=road.getNHits(pl)
                                # ignore missing layers in the calculation
                                if nhits>0:
                                    mean_sscnt[pl].append(nhits)
                                    ncomb*=nhits
                            mean_ncomb.append(ncomb)
                            roads[0].detachHits(ir)
                    self.m_nao_perroad_ncomb[i].append(m(mean_ncomb))
                    for pl in range(self.nplanes):
                        self.m_nao_sscnt[i][pl].append(m(mean_sscnt[pl]))
                self.cnt_nroads[i].append(curroads)
                self.cnt_ntracks[i].append(curtracks)
                self.cnt_nroads4L[i].append(curroads4L)
                if not _IGNORE_TRACKS:
                    self.cnt_nfits[i].append(tracks[0].getNFits()-tracks[0].getNFitsMajority()) #11/11 fits
                    self.cnt_nfits_maj[i].append(tracks[0].getNFitsMajority())                  #10/11 roads
                    self.cnt_nfits_rec[i].append(tracks[0].getNFitsRecovery())                  #10/11 recoveries
                    # old definitions (prior to Guido's changes to FTKTrackStream in Feb 2010)
                    #self.cnt_nfits[i].append(tracks[0].getNFits()-tracks[0].getNFitsMajority())
                    #self.cnt_nfits_maj[i].append(tracks[0].getNFitsMajority()-tracks[0].getNFitsRecovery())
                    #self.cnt_nfits_rec[i].append(tracks[0].getNFitsRecovery())
                    if _FILL_HISTOS:
                        self.histo_nfits[i].Fill(tracks[0].getNFits())
                        self.histo_nfits_maj[i].Fill(tracks[0].getNFitsMajority())
                    # loop over tracks
                    if _LOOP_OVER_TRACKS:
                        for t in range(curtracks):
                            track = tracks[0].getTrack(t)
                            self.histo_ftkphi.Fill(track.getPhi())
                            self.histo_ftkchi2.Fill(track.getChi2())
            if _FILL_HISTOS:
                self.histo_nroads.Fill(nroads)
                self.histo_nroads_good.Fill(nroads_good)
                self.histo_ntracks.Fill(ntracks)
        print ']'
    def BookCounters(self):
        self.cnt_nfits = {}
        self.cnt_nfits_maj = {}
        self.cnt_nfits_rec = {}
        self.cnt_nroads = {}
        self.cnt_nroads4L = {}
        self.cnt_ntracks = {}
        for b in self.banks:
            self.cnt_nfits[b]=[]
            self.cnt_nfits_maj[b]=[]
            self.cnt_nfits_rec[b]=[]
            self.cnt_nroads[b]=[]
            self.cnt_nroads4L[b]=[]
            self.cnt_ntracks[b]=[]
        # naoki counters
        self.m_nao_nhits_tot = {}
        self.m_nao_nclus_tot = {}
        self.m_nao_nclus = {}
        self.m_nao_nss = {}
        self.m_nao_nroads_am = {}
        self.m_nao_nroads_mod = {}
        self.m_nao_nroads_rw = {}
        self.m_nao_perroad_ncomb = {}
        self.m_nao_sscnt = {}
        for b in self.banks:
            self.m_nao_nhits_tot[b] = []
            self.m_nao_nclus_tot[b] = []
            # per-plane counters
            self.m_nao_nclus[b] = [list() for zz in range(self.nplanes)]
            self.m_nao_nss[b] = [list() for zz in range(self.nplanes)]
            self.m_nao_nroads_am[b] = []
            self.m_nao_nroads_mod[b] = []
            self.m_nao_nroads_rw[b] = []
            self.m_nao_perroad_ncomb[b] = []
            self.m_nao_sscnt[b] = [list() for zz in range(self.nplanes)]
    def PrintCounters(self,fout="/dev/stdout"):
        try:
            f=open(fout,"w")
            print "Run statistics saved under",fout
        except:
            f=open("/dev/stdout","a")
        print >>f,"Average # of roads and fits per event:"
        for b in self.banks:
            try:
                print >>f,"Bank",b
                if not _IGNORE_ROADS:
                    print >>f,"DATAFLOW INFORMATION:"
                    print >>f,"  # of hits (entire detector):        ",
                    if _PERCENTILES:
                        print >>f,"%d %d %d"%pctl(self.m_nao_nhits_tot[b])
                    else:
                        print >>f,"%d"%mi(self.m_nao_nhits_tot[b])
                    print >>f,"  # of clusters (entire detector):    ",
                    if _PERCENTILES:
                        print >>f,"%d %d %d"%pctl(self.m_nao_nclus_tot[b])
                    else:
                        print >>f,"%d"%mi(self.m_nao_nclus_tot[b])
                    print >>f,"  # of clusters (only those routed to region %d), per plane:"%(b,)
                    print >>f,'   ',
                    for pl in range(self.nplanes):
                        print >>f,mi(self.m_nao_nclus[b][pl]),
                    print >>f,''
                    print >>f,"  # of superstrips (within region %d), per plane:"%(b,)
                    print >>f,'   ',
                    for pl in range(self.nplanes):
                        print >>f,mi(self.m_nao_nss[b][pl]),
                    print >>f,''
                    if _ATTACH_HITS:
                        print >>f,"  # of fit combinations per road:     ",
                        if _PERCENTILES:
                            print >>f,"%.2f %.2f %.2f"%pctl(self.m_nao_perroad_ncomb[b])
                        else:
                            print >>f,"%.2f"%m(self.m_nao_perroad_ncomb[b])
                        print >>f,"  # of hits per superstrip in each plane:"
                        print >>f,'   ',
                        for pl in range(self.nplanes):
                            print >>f,"%.2f"%m(self.m_nao_sscnt[b][pl]),
                        print >>f,''
                    print >>f,"ROAD INFORMATION:"
                    print >>f,"  # of roads out of AM:         ",
                    if _PERCENTILES:
                        print >>f,"%d %d %d"%pctl(self.m_nao_nroads_am[b])
                    else:
                        print >>f,"%d"%mi(self.m_nao_nroads_am[b])
                    if(mi(self.m_nao_nroads_mod[b])>0):
                        print >>f,"  # of true 4L roads out of AM: ",
                        if _PERCENTILES:
                            print >>f,"%d %d %d"%pctl(self.m_nao_nroads_mod[b])
                        else:
                            print >>f,"%d"%mi(self.m_nao_nroads_mod[b])
                    print >>f,"  # of roads out of RW:         ",
                    if _PERCENTILES:
                        print >>f,"%d %d %d"%pctl(self.m_nao_nroads_rw[b])
                    else:
                        print >>f,"%d"%mi(self.m_nao_nroads_rw[b])
                if not _IGNORE_TRACKS:
                    print >>f,"TRACK INFORMATION:"
                    print >>f,"  # of tracks:                                         ",
                    if _PERCENTILES:
                        print >>f,"%d %d %d"%pctl(self.cnt_ntracks[b])
                    else:
                        print >>f,"%d"%mi(self.cnt_ntracks[b])
                    if not _PERCENTILES:
                        _fits = mi(self.cnt_nfits[b])
                        _fits_maj = mi(self.cnt_nfits_maj[b])
                        _fits_rec = mi(self.cnt_nfits_rec[b])
                        print >>f,"  # of fits (total):                                   %d"%(_fits+_fits_maj+_fits_rec)
                    print >>f,"  # of 11/11 fits:                                     ",
                    if _PERCENTILES:
                        print >>f,"%d %d %d"%pctl(self.cnt_nfits[b])
                    else:
                        print >>f,"%d"%_fits
                    print >>f,"  # of 10/11 fits coming from 10/11 roads:             ",
                    if _PERCENTILES:
                        print >>f,"%d %d %d"%pctl(self.cnt_nfits_maj[b])
                    else:
                        print >>f,"%d"%_fits_maj
                    print >>f,"  # of 10/11 fits coming from 11/11 roads (majority):  ",
                    if _PERCENTILES:
                        print >>f,"%d %d %d"%pctl(self.cnt_nfits_rec[b])
                    else:
                        print >>f,"%d"%_fits_rec
            except:
                print >>f,"Bank %d: division by zero"%int(b)
        f.close()
    def BookHistos(self):
        """ Histogram booking """
        self.histo_nroads=ROOT.TH1I("histo_nroads","Number of roads",40,0,40)
        self.histo_nroads_good=ROOT.TH1I("histo_nroads_good","Number of roads",40,0,40)
        self.histo_ntracks=ROOT.TH1I("histo_ntracks","Number of tracks",40,0,40)
        self.histo_ftkphi=ROOT.TH1F("histo_ftkphi","Phi distribution of ftk tracks",40,-3.15,3.15)
        self.histo_ftkchi2=ROOT.TH1F("histo_ftkchi2","Chi2 distribution of ftk tracks",40,0,20)
        # number of fits in each bank
        self.histo_nfits = {}
        self.histo_nfits_maj = {}
        for i in self.banks:
            self.histo_nfits[i] = ROOT.TH1F("histo_nfits%d"%i,"# of fits in bank %d"%i,10,0,100e6)
            self.histo_nfits_maj[i] = ROOT.TH1F("histo_nfits_maj%d"%i,"# of majority fits in bank %d"%i,10,0,100e6)
    def PlotHistos(self):
        """ Histogram plotting """
        # roads
        ROOT.gStyle.SetOptStat(111111);
        ROOT.gStyle.SetOptFit(1111);
        self.cnroads=ROOT.TCanvas("cnroads","N roads",1)
        self.histo_nroads.SetLineColor(ROOT.kRed)
        self.histo_nroads.SetLineWidth(2)
        self.histo_nroads.Draw()
        self.histo_nroads_good.SetLineColor(ROOT.kBlack)
        self.histo_nroads_good.SetLineWidth(2)
        self.histo_nroads_good.Draw("same")
        self.histo_ntracks.SetLineColor(ROOT.kBlue)
        self.histo_ntracks.SetLineWidth(2)
        self.histo_ntracks.Draw("same")
        #self.cnroads.SetLogy(ROOT.kTRUE)
        leg = ROOT.TLegend(.4,.6,.8,.8)
        leg.AddEntry(self.histo_nroads,"# roads (InSec RW)","l")
        leg.AddEntry(self.histo_nroads_good,"# good roads (all RWs)","l")
        leg.AddEntry(self.histo_ntracks,"# tracks (NEW)","l")
        leg.Draw()
        self.cnroads.SaveAs(os.path.join(self.plotdir,"RF_Nroadstracks.png"))
        # track phi
        self.cftkphi=ROOT.TCanvas("cftkphi","FTK tracks phi distribution",1)
        self.histo_ftkphi.SetLineColor(ROOT.kRed)
        self.histo_ftkphi.SetLineWidth(2)
        self.histo_ftkphi.Draw()
        self.cftkphi.SetLogy(ROOT.kTRUE)
        self.cftkphi.SaveAs(os.path.join(self.plotdir,"RF_tracks_phi.png"))
        # track chi2
        self.cftkchi2=ROOT.TCanvas("cftkchi2","FTK tracks chi2 distribution",1)
        self.histo_ftkchi2.SetLineColor(ROOT.kRed)
        self.histo_ftkchi2.SetLineWidth(2)
        self.histo_ftkchi2.Draw()
        self.cftkchi2.SaveAs(os.path.join(self.plotdir,"RF_tracks_chi2.png"))
        # nfits
        for i in self.banks:
            self.cftknfits=ROOT.TCanvas("cftknfits%d"%i,"FTK nfits in bank %d"%i,1)
            self.histo_nfits[i].Draw()
            self.cftknfits.SaveAs(os.path.join(self.plotdir,"nfits_%d.png"%i))
            self.cftknfitsmaj=ROOT.TCanvas("cftknfitsmaj%d"%i,"FTK majority nfits in bank %d"%i,1)
            self.histo_nfits_maj[i].Draw()
            self.cftknfitsmaj.SaveAs(os.path.join(self.plotdir,"majority_nfits_%d.png"%i))
