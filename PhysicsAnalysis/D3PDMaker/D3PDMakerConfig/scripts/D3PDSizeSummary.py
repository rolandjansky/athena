#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import ROOT
import os
import sys
import re
from optparse import OptionParser

#ROOT.gROOT.Macro( os.path.expanduser( '~/RootUtils/rootlogon.C' ) )

usage = "usage: %prog [options] input_file"
parser = OptionParser(usage=usage)
parser.add_option ("-s", "--subgroups" , action="store_true" , dest="subgroups" , default=False, help="Include additional subgroups for primary branch groups")
parser.add_option ("-T", "--TeX"       , action="store_true" , dest="TeX"       , default=False, help="Print branch sizes in TeX format")
parser.add_option ("-p", "--plots"     , action="store_true" , dest="plots"     , default=False, help="Create pie plots with fractional fize size for branch groups")
parser.add_option ("-v", "--debug"     , action="store_true" , dest="debug"     , default=False, help="Print verbose output (for debugging)")
parser.add_option ("-f", "--file"      , action="store"      , dest="filename"  , default="pie", help="Filename for output pie chart", type="string")
parser.add_option ("-e", "--ext"       , action="store"      , dest="extension" , default="png", help="Filetype for output pie chart", type="string")
(options, args) = parser.parse_args()


subgroups = {}
subgroups['el'] = [['Basic', 'n E Et pt m eta phi px py pz charge author ' +
                             'isEM loose medium mediumIso tight tightIso'],
                   ['PID',   'electronweight electronbgweight ' +
                             'isolationlikelihood.* softeweight ' +
                             'softebgweight neuralnet Hmatrix Hmatrix5 ' +
                             'adaboost softeneuralnet '],
                   ['Conv',  '.*conv.* .*Conv.*'],
                   ['Truth', 'type origin truth_.*'],
                   ['Calo',  'Ethad Ethad1 f1 f1core Emins1 fside Emax2 ' +
                             'ws3 wstot emaxs1 deltaEs E233 E237 E277 ' +
                             'weta2 f3 f3core rphiallcalo reta rphi ' +
                             'deltaEmax2 pos7'],
                   ['Iso',   'Etcone.* ptcone.* Etring.* isIso'],
                   ['Track', 'deltaeta1 deltaeta2 deltaphi2 deltaphiRescaled '+
                             'track.* n.*Hits n.*Holes n.*Outliers ' +
                             'vert.* hastrack TRT.*Ratio etacorrmag'],
                   ['Pointing', 'zvertex errz etap depth'],
                   ['Brem',  'brem.* refitted.* hasbrem'],
                   ['Cluster', 'Es[0-3] etas[0-3] phis[0-3] cl_.*'],
                   ['Forward', 'firstEdens cellmaxfrac longitudinal ' +
                               'secondlambda lateral secondR centerlambda'],
                   ['Rawcl',   'rawcl_.*'],
                   ['Jettruth','jet_truth_.*'],
                   ['Jet',     'jet_.*'],
                   ['Trigger', 'L1_.* L2_.* EF.*'],
                   ['*Other',  '.*'],
                   ]


class Branchgroup:
    text_format = '%(name)-10s %(nbr)5s %(nobj)7s %(nobjper)7s %(totsiz)8s %(totsizper)8s %(filesiz)8s %(filesizper)9s %(comp)4s'
    sg_text_format = text_format.replace ('(name)-', '(name)')

    tex_format = '%(name)-10s&%(nbr)5s&%(nobj)7s&%(nobjper)7s&%(totsiz)8s&%(totsizper)8s&%(filesiz)8s&%(filesizper)9s&%(comp)4s \\\\'
    sg_tex_format = tex_format

    if options.TeX:
        format = tex_format
        sg_format = sg_tex_format
    else:
        format = text_format
        sg_format = sg_text_format

    def __init__ (self, name):
        self.name = name
        self.nbr = 0
        self.totbytes = 0
        self.filebytes = 0
        self.totobj = None
        self.subgroups = {}
        return


    def get_subgroup (self, b):
        sglist = subgroups.get (self.name)
        if not sglist: return None
        bname = b.GetName()
        for (sgname, pats) in sglist:
            for p in pats.split():
                pp = self.name + '_' + p + '$'
                if re.match (pp, bname):
                    if sgname[0] == '*':
                        print '***', bname
                        sgname = sgname[1:]
                    sg = self.subgroups.get (sgname)
                    if not sg:
                        sg = Branchgroup (sgname)
                        self.subgroups[sgname] = sg
                    return sg
        return None


    def add (self, b):
        self.nbr += 1
        self.totbytes += b.GetTotalSize()
        self.filebytes += b.GetZipBytes()
        bname = b.GetName()
        if bname == self.name + '_n':
            self.totobj = 0
            for i in range(b.GetEntries()):
                b.GetEntry(i)
                self.totobj += int(b.GetLeaf(bname).GetValue())

        if options.subgroups:
            sg = self.get_subgroup (b)
            if sg: sg.add (b)
        
        
        
        return


    def get_dict (self, nev):
        if self.totobj == None:
            self.totobj = nev

        d = {}
        d['name'] = self.name
        d['nbr']  = `self.nbr`
        d['nobj'] = `int(self.totobj)`
        d['nobjper'] = "%.1f" % (float(self.totobj) / nev)
        d['totsiz'] = "%dk" % int(float(self.totbytes) / 1024 + 0.5)
        d['totsizper'] = "%.2fk" % (float(self.totbytes) / 1024 / nev )
        d['filesiz'] = "%dk" % int(float(self.filebytes) / 1024 + 0.5)
        d['filesizper'] = "%.2fk" % (float(self.filebytes) / 1024 / nev)
        d['comp'] = "%.2f" % (float(self.totbytes) / self.filebytes)
        return d


    @staticmethod
    def print_header ():
        if options.TeX:
            print '\\begin{tabular}{|l|r|r|r|r|r|r|r|r|}'
            print '\\hline'
        d = {}
        d['name'] = 'Name'
        d['nbr']  = 'Nbr'
        d['nobj'] = 'Nobj'
        d['nobjper'] = 'Nobj/ev'
        d['totsiz'] = 'Totsz'
        d['totsizper'] = 'Totsz/ev'
        d['filesiz'] = 'Filesz'
        d['filesizper'] = 'Filesz/ev'
        d['comp'] = 'Comp'
        print Branchgroup.format % d
        if options.TeX:
            print '\\hline'
        return
        
    def print_stats (self, nev):
        print Branchgroup.format % self.get_dict (nev)

        sl = self.subgroups.values()
        sl.sort (lambda a, b: int(b.filebytes - a.filebytes))
        for sg in sl:
            print Branchgroup.sg_format % sg.get_dict (nev)
        return

    @staticmethod
    def print_trailer ():
        if options.TeX:
            print '\\hline'
            print '\\end{tabular}'
        return


class Filestats:
    def __init__ (self, fname, treename = None):
        self.fname = fname
        self.treename = treename
        self.branchgroups = {}
        self.get_stats()
        return


    def get_treename (self):
        if self.treename: return
        for kk in self.rfile.GetListOfKeys():
            if (kk.GetClassName() == 'TTree' and
                kk.GetName() != 'CollectionTree'):
                self.treename = kk.GetName()
        return


    def get_branchgroup (self, b):
        bname = b.GetName()
        ll = bname.split ('_')
        if bname in ['RunNumber',
                     'EventNumber',
                     'timestamp',
                     'timestamp_ns',
                     'lbn',
                     'bcid',
                     'detmask0',
                     'detmask1',
                     'collcand']:
            gname = 'EventInfo'
        elif len(ll) > 1:
            gname = ll[0]
        elif (bname.endswith ('Error') or bname.endswith ('Flags')):
            gname = 'DetFlags'
        else:
            gname = bname
        group = self.branchgroups.get (gname)
        if not group:
            group = Branchgroup (gname)
            self.branchgroups[gname] = group
        return group


    def get_stats (self):
        self.file_size = os.stat(self.fname).st_size
        self.rfile = ROOT.TFile.Open (self.fname)
        self.get_treename()
        if self.treename == None:
            raise Exception ("Can't find tree name in file.")
        self.tree = self.rfile.Get (self.treename)
        if not self.tree:
            raise Exception ("Can't find tree " + self.treename + " in file.")
        self.nev = self.tree.GetEntries()
        self.scan_branches()
        return


    def scan_branches (self):
        for b in self.tree.GetListOfBranches():
            bg = self.get_branchgroup (b)
            bg.add (b)
        return


    def print_stats_text (self):
        print "Number of events:", self.nev
        print "File size: %dk" % int(float(self.file_size)/1024 + 0.5)
        print "Overall size/event: %.1fk" % (float(self.file_size) / self.nev / 1024)

        bl = self.branchgroups.values()
        bl.sort (lambda a, b: int(b.filebytes - a.filebytes))
        Branchgroup.print_header()
        for bg in bl:
            bg.print_stats (self.nev)
        Branchgroup.print_trailer()
        return


    def draw_pie (self):
        
        if options.debug:
            print "Number of branch groups:", len(self.branchgroups)
            print "Values of branch groups:", list(self.branchgroups.keys()) 
        
        blist = self.branchgroups.values()
        blist.sort (lambda a, b: int(b.filebytes - a.filebytes))            
        pie = ROOT.TPie("BranchPie", "", len(self.branchgroups))
        for i, bgroup in enumerate( blist ):
            bginfo = bgroup.get_dict(self.nev) 
            
            if options.debug:
                print "Branch group #%d, named %s with size %f = %f%%" % (i ,  bginfo['name'], (float(bgroup.filebytes) / 1024 / self.nev ), (100. * float(bgroup.filebytes) / float(self.file_size)) )
            
            pie.SetEntryLabel(i, bginfo['name'])
            pie.SetEntryVal  (i, (100. * float(bgroup.filebytes) / float(self.file_size)) )
            pie.SetEntryFillColor(i, i+2)
          
        
        c1 = ROOT.TCanvas( 'c1' , 'c1' )
        
        pie.SetCircle(.5,.45,.2)    
        pie.SetLabelFormat("#splitline{%txt}{(%perc)}")
        pie.SetLabelFormat("%txt")
        pie.SetRadius(0.2)
        pie.SetTextSize(0.03)
        pie.SetCircle(0.4808696, 0.7940109, 0.2)
        pie.SetValueFormat("%4.2f")
        pie.SetLabelFormat("%txt")
        pie.SetPercentFormat("%3.1f")
        pie.SetLabelsOffset(0.005)
        pie.SetAngularOffset(265.2655)
        leg = ROOT.TLegend(0.05580866, 0.06554878, 0.785877, 0.4512195)
        for i, bgroup in enumerate( blist ):
          if ( (float(bgroup.filebytes) / float(self.file_size)) > 0.05):
              pie.SetEntryRadiusOffset(i,.07)
              bginfo = bgroup.get_dict(self.nev) 
              leg.AddEntry( pie.GetSlice(i), "%s : %1.1lf%%" % (bginfo['name'] , 100. * float(bgroup.filebytes) / float(self.file_size)) , "f" )
          else:
              pie.SetEntryLabel(i,"")

        leg.SetFillColor(0);
        leg.SetFillStyle(0);
        
        pie.Draw("3d")
        leg.Draw();
        
        c1.SaveAs(options.filename + "." + options.extension)
        
        
  
       

ff = Filestats (args[0])
ff.print_stats_text()
if options.plots:
    ff.draw_pie()
