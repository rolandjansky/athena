# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Usage:      python plotRootSimple.py [OPTIONS]
Author:     Tae Min Hong tmhong@hep.upenn.edu
Purpose:    Plots (rates or xsection) from the output of makeRoot.py.
Note:       Kept deliberately simple. See plotRoot.py.
Examples:

python plotRootSimple.py --help
python plotRootSimple.py
python plotRootSimple.py --yaxis-list=xbp__L1_XE30:1.0,xbp__L1_XE40:5.0,xbp__L1_XE50:20.0 --xaxis-list=evt__bunchlumi --cutmin-list=evt__lumi:1000.0 --verbose
python plotRootSimple.py --yaxis-list=rbp__L1_XE30:1.0,rbp__L1_XE40:5.0,rbp__L1_XE50:20.0 --xaxis-list=evt__lumi      --cutmin-list=evt__lumi:1000.0 --verbose

Nomenclature for y-axis:

tbp = counts before prescale
tap = counts after prescale
tav = counts after veto
rbp = rates before prescale
rap = rates after prescale
rav = rates after veto
xbp = cross-section before prescale
xap = cross-section after prescale
xav = cross-section after veto

Nomenclature for x-axis:

evt__lumi
evt__bunchlumi
evt__time
...
"""

import ROOT
import array
import time
import optparse

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(False)
ROOT.TGaxis.SetMaxDigits(4)

#
# Default inputs
#

dbdir = '/afs/cern.ch/user/x/xmon/work/public/root'
outdir = '.'
yaxis_list = {'rbp__L1_XE30': 1.0,      # name, scale factor
              'rbp__L1_XE40': 5.0,      # name, scale factor
              'rbp__L1_XE50': 20.0,     # name, scale factor
             }
xaxis_list = {'evt__time': 1.0,         # name, scale factor
              'evt__bunchlumi': 1.0,    # name, scale factor
             }
cutmin_list = {'evt__lumi': 1000.0,     # name, cut value
              }
cutmax_list = {
              }


#
# User inputs
#

p = optparse.OptionParser()
p.add_option( '--xaxis-list',   type='string',default=None,         dest='xaxis_list',  help='Comma-sep var:scale_factor -- evt__time:1,evt__lumi:1')
p.add_option( '--yaxis-list',   type='string',default=None,         dest='yaxis_list',  help='Comma-sep var:scale_factor -- evt__time:1,evt__lumi:1')
p.add_option( '--cutmin-list',  type='string',default=None,         dest='cutmin_list', help='Comma-sep var:scale_factor -- evt__time:1,evt__lumi:1')
p.add_option( '--cutmax-list',  type='string',default=None,         dest='cutmax_list', help='Comma-sep var:scale_factor -- evt__time:1,evt__lumi:1')
p.add_option( '--run',          type='string',default='208811',     dest='run',         help='Comma-sep run numbers -- default: %default')
p.add_option( '--db',           type='string',default='cool',       dest='db',          help='Xmon ntuple type -- TRP, COOL -- default: %default')
p.add_option( '--dbdir',        type='string',default=dbdir,        dest='dbdir',       help='Xmon ntuple directory -- default: %default')
p.add_option( '--outdir',       type='string',default='.',          dest='outdir',      help='Output directory -- ., tae -- default: %default')
p.add_option( '--interactive',  action='store_true',default=False,  dest='interactive', help='Interactive mode')
p.add_option( '--verbose',      action='store_true',default=False,  dest='verbose',     help='Verbose mode')
(options, args) = p.parse_args()

#
# Color, label settings
#

# Color order in the plots -- no yellow!
tcolor = [ROOT.kBlack,
          ROOT.kRed,
          ROOT.kBlue,
          ROOT.kMagenta,
          ROOT.kOrange,
          ROOT.kCyan,
         ]

# Axis labels
labels = {'time'        : 'time',
          'lumi'        : 'instantaneous luminosity per crossing (1e30)',
          'bunchlumi'   : 'instantaneous luminosity per crossing per bunch (1e30)',
          'rbp'         : 'trigger rate before prescale',
          'rap'         : 'trigger rate after prescale',
          'rav'         : 'trigger rate after veto',
          'xbp'         : 'trigger cross-section before prescale',
          'xap'         : 'trigger cross-section after prescale',
          'xav'         : 'trigger cross-section after veto',
         }

#----------------------------------------------------------------------
def get_canvas():
    canvas = ROOT.TCanvas('canvas', 'canvas', 900, 600)
    canvas.SetFillColor(ROOT.kWhite)
    canvas.SetTickx()
    canvas.SetTicky()
    canvas.SetLeftMargin(0.08)
    canvas.SetBottomMargin(0.10)
    canvas.SetTopMargin(0.05)
    canvas.SetRightMargin(0.35)
    return canvas

#----------------------------------------------------------------------
def get_plot(name, title, nbinsx, xmin, xmax, nbinsy, ymin, ymax):
    plot = ROOT.TH2F(name, title, nbinsx, xmin, xmax, nbinsy, ymin, ymax)
    plot.SetMarkerSize(0.5)
    plot.SetMarkerStyle(20)
    plot.GetXaxis().SetTitleOffset(1.2)
    plot.GetYaxis().SetTitleOffset(1.0)
    plot.GetYaxis().SetNdivisions(504)
    plot.GetXaxis().SetNdivisions(504)
    return plot

#----------------------------------------------------------------------
def parse_input(inputlist):
    keys = inputlist.split(',') 
    newlist = dict()
    for key in keys:
        tokens = key.split(':')
        var = tokens[0]
        scale_factor = 1
        if len(tokens) > 1: scale_factor = float(tokens[1])
        newlist[var] = scale_factor

    return newlist

#----------------------------------------------------------------------
# Main

#
# Parse inputs
#

if options.outdir != 'tae': outdir      = options.outdir
if options.xaxis_list:      xaxis_list  = parse_input(options.xaxis_list)
if options.yaxis_list:      yaxis_list  = parse_input(options.yaxis_list)
if options.cutmin_list:     cutmin_list = parse_input(options.cutmin_list)
if options.cutmax_list:     cutmax_list = parse_input(options.cutmax_list)
run_list = options.run.split(',')

#
# Initialize
#

# Combine input files in TChain
tchain = ROOT.TChain('ntp1')
for run in run_list:
    tfile_name = '%s/%s_%s.root' % (options.dbdir, run, options.db)
    status = tchain.Add( tfile_name )
    if options.verbose: print 'Adding %s status is %s' % (tfile_name, str(status))

# Turn off all branches by default
tchain.SetBranchStatus('*', 0)
tchain.ResetBranchAddresses()

# Define local vars
hvar = dict()       # dictionary of var values
hmin = dict()       # dictionary of var minimums
hmax = dict()       # dictionary of var maximums
var_list = dict(    # merged dictionary of all used vars
    xaxis_list.items()+
    yaxis_list.items()+
    cutmin_list.items()+
    cutmax_list.items())

# Activate TBranches
for v in sorted(var_list.iterkeys()):
    if options.verbose: print 'Activating branch', v 
    hvar[v] = array.array('f', [0])
    hmin[v] = 1e15
    hmax[v] = -1
    tchain.SetBranchStatus(v, 1)
    tchain.SetBranchAddress(v, hvar[v])

#
# Scan for min, max range
#

for ientry in xrange(int(tchain.GetEntries())):
    if options.verbose: print '\nScanning entry', ientry,
    tchain.GetEntry(ientry)

    # Quality cuts -- from command-line input
    skip = False
    for c in sorted(cutmin_list.iterkeys()): skip = skip or hvar[c][0]<cutmin_list[c]
    for c in sorted(cutmax_list.iterkeys()): skip = skip or hvar[c][0]>cutmax_list[c]
    if skip:
        continue

    # Find min, max range for each var
    for v in sorted(var_list.iterkeys()):
        scale_factor = var_list[v] 
        val = scale_factor * hvar[v][0]
        if hmin[v] > val: hmin[v] = val
        if hmax[v] < val: hmax[v] = val
        if options.verbose: print v, hvar[v][0],

# Add 10% to min, max range for each var
for v in sorted(var_list.iterkeys()):
    range = hmax[v] - hmin[v]
    hmin[v] = hmin[v] - 0.1*range
    hmax[v] = hmax[v] + 0.1*range
    if hmin[v] < 0:
        hmin[v] = 0

# Find min, max for all yaxis vars
min = 1e15
max = -1
for y in sorted(yaxis_list.iterkeys()):
    if hmin[y] < min: min = hmin[y]
    if hmax[y] > max: max = hmax[y]
    if options.verbose: print y, hvar[y][0],

# Minimum is positive-definite
if min < 0: min = 0

# Set min, max for all yaxis vars
for y in sorted(yaxis_list.iterkeys()):
    hmin[y] = min
    hmax[y] = max

# Verbose
if options.verbose:
    for v in sorted(var_list.iterkeys()):
        print 'Min/max for', v, hmin[v], hmax[v]

#
# Define & fill histograms -- we use TH2F
#

# Constructor
plot = dict()
for x in sorted(xaxis_list.iterkeys()):
    plot[x] = dict()
    for y in sorted(yaxis_list.iterkeys()):
        h = x + '_vs_' + y
        plot[x][y] = get_plot(h, '',
                              100, hmin[x], hmax[x],
                              100, hmin[y], hmax[y])

# Fill
for ientry in xrange(int(tchain.GetEntries())):
    tchain.GetEntry(ientry)
    for x in sorted(xaxis_list.iterkeys()):
        scale_factor = var_list[x]
        xval = scale_factor * hvar[x][0]
        for y in sorted(yaxis_list.iterkeys()):
            scale_factor = var_list[y]
            yval = scale_factor * hvar[y][0]
            plot[x][y].Fill(xval, yval, 1)

#
# Prepare & draw histograms (& legends)
#

# Canvas
tcanvas = get_canvas()

# Legends
ntrig = len(yaxis_list.keys()) 
legend = None
if ntrig > 1:
    ylow = 0.95 - 0.04 - 0.04*ntrig
    legend = ROOT.TLegend(0.66, ylow, 1.0, 0.95)
    legend.SetHeader('Run' + ' '.join(run_list))
    legend.SetFillColor(0)
    legend.SetBorderSize(0)
else:
    legend = ROOT.TText()

# Loop over x-axis types
for xidx, x in enumerate(sorted(xaxis_list.iterkeys())):
    h = x + '_vs'
    xtitle = labels[x.split('__')[1]]

    # Loop over trigger histograms
    for yidx, y in enumerate(sorted(yaxis_list.iterkeys())):
        h += '_' + y
        ytitle = labels[y.split('__')[0]]
        thisplot = plot[x][y]
        
        # X-axis for time
        if xtitle == 'time':
            starttime = thisplot.GetXaxis().GetBinCenter(1)
            tarray = time.gmtime(starttime)
            year = tarray[0]
            month = time.strftime('%b', tarray)
            day = tarray[2]
            hour = tarray[3] + 1
            thisplot.GetXaxis().SetTimeFormat('%Hh')#%F1970-01-01 00:00:00')
            thisplot.GetXaxis().SetTimeDisplay(1)
            thisplot.GetXaxis().SetTimeOffset(0, 'gmt')

        # Color
        hcolor = tcolor[yidx%len(tcolor)]
        thisplot.SetMarkerColor(hcolor)

        # Legend text
        legend_text = y.split('__')[1]
        scale_factor = var_list[y] 
        if scale_factor != 1:
            legend_text += ' (x%d)' % scale_factor

        # Axes titles
        xtitle = xtitle.capitalize()
        ytitle = ytitle.capitalize()
        if xtitle == 'Time': xtitle += ' starting around %sh %s %d, %d (UTC)' %(hour, month, day, year)
        if y[0] == 'x': ytitle += ' (mb)'
        if y[0] == 'r': ytitle += ' (Hz)'
        thisplot.SetXTitle(xtitle)
        thisplot.SetYTitle(ytitle)

        # Draw histogram
        if yidx == 0:
            thisplot.DrawCopy()
        else:
            thisplot.DrawCopy('SAME')

        # Legend add
        if xidx == 0:
            if ntrig > 1:
                thatplot = thisplot
                thatplot.SetMarkerSize(1.5)
                legend.AddEntry(thatplot, legend_text, 'P')
            else:
                #legend.DrawText(legend_text, 0.2, 0.8, 0.5, 0.9)
                legend.DrawText( 0.2, 0.8, legend_text)

    # Draw legend
    legend.Draw()

    # Save plot
    outpath = outdir + '/' + h + '.png'
    tcanvas.SaveAs(outpath)




