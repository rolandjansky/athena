#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Tae Min Hong, Univ. of Pennsylvania (2011, 2012)
#
# Fits trigger rates or xsection from makeRoot.py output.
#
# This code is kept deliberately simple.
#
# Main functions:
#   if __name__ == '__main__'
#   def main()
#   def make_plot()
#
# Auxilliary functions:
#   def get_ytitle()
#   def set_same_xrange()
#   def set_symmetric_yaxis()
#   def expand_abbrev()
#   def save_canvas()
#   def get_fit_text()
#   def get_chi2()
#   def convert_graph_to_hist()
#   def find_data_in_set()
#   def filter_reject()
#   def fill_zdata()
#   def get_data()
#   def get_graph()
#   def draw_graph()
#   def get_fn()
#   def get_plotextrema()
#   def get_yextrema()
#   def get_xextrema()
#   def zip_data()
#   def unzip_data()
#   def filter_1dim()
#
# Archived functions:
#   def zipdata_to_tcanvas()
#   def get_canvas()
#
# Examples:
"""
python -i fitRoot.py --rootdir=root --run=167607 --db=cool --triggers=L1_4J30 --interactive
python -i fitRoot.py --rootfile=root/191933_cool.root --triggers=L1_4J30 --interactive
"""

import optparse
preamble='usage: %prog [options]'
p = optparse.OptionParser(usage=preamble, add_help_option=False, version='%prog')

## Arg't-required options
p.add_option(     '--db',          dest='db',          default='cool', help='xmon ROOT type: cool, trp  [default:%default]')
p.add_option('-f','--rootfile',    dest='rootfile',    default='root', help='xmon ROOT directory        [default:%default]')
p.add_option(     '--rootdir',     dest='rootdir',     default=None,   help='xmon ROOT file             [default:%default]')
p.add_option('-o','--outdir',      dest='outdir',      default='.',    help='Output path                [default:%default]')
p.add_option('-F','--fitfn',       dest='fitfn',       default='pol2', help='pol, expo, pow_N, zexp_N, met_N, just_N [default:%default]')
p.add_option('-r','--run',         dest='run',         default=None,   help='Run number                 [default:%default]')
p.add_option(     '--lbmin',       dest='lbmin',       default=None,   help='Minimum LB number          [default:%default]')
p.add_option(     '--lbmax',       dest='lbmax',       default=None,   help='Maximum LB number          [default:%default]')
p.add_option(     '--nmax',        dest='nmax',        default=None,   help='Max number of LB data used [default:%default]')
p.add_option(     '--level',       dest='level',       default='L1',   help='Level: all, L1, L2, EF     [default:%default]')
p.add_option(     '--imageh',      dest='imageh',      default='450',  help='Image height (px)          [default:%default]')
p.add_option(     '--thumbh',      dest='thumbh',      default='120',  help='Thumb height (px)          [default:%default]')
p.add_option(     '--par0',        dest='par0',        default='1',    help='init par0 val              [default:%default]')
p.add_option(     '--par1',        dest='par1',        default='1',    help='init par1 val              [default:%default]')
p.add_option(     '--par2',        dest='par2',        default='-1',   help='init par2 val              [default:%default]')
p.add_option(     '--par3',        dest='par3',        default='1',    help='init par3 val              [default:%default]')
p.add_option(     '--par4',        dest='par4',        default='1',    help='init par4 val              [default:%default]')
p.add_option(     '--par5',        dest='par5',        default='1',    help='init par5 val              [default:%default]')
p.add_option(     '--err0',        dest='err0',        default='1',    help='init err0 val              [default:%default]')
p.add_option(     '--err1',        dest='err1',        default='1',    help='init err1 val              [default:%default]')
p.add_option(     '--err2',        dest='err2',        default='1',    help='init err2 val              [default:%default]')
p.add_option(     '--err3',        dest='err3',        default='1',    help='init err3 val              [default:%default]')
p.add_option(     '--err4',        dest='err4',        default='1',    help='init err4 val              [default:%default]')
p.add_option(     '--err5',        dest='err5',        default='1',    help='init err5 val              [default:%default]')
p.add_option(     '--print-trig',  dest='print_trig',  default=None,   help='Print triggers matching ...[default:%default]')
p.add_option(     '--tap-min',     dest='tap_min',     default=10,     help='min tap event value        [default:%default]')
p.add_option(     '--ymin',        dest='ymin',        default=None,   help='y-axis min value           [default:%default]')
p.add_option(     '--ymax',        dest='ymax',        default=None,   help='y-axis max value           [default:%default]')
p.add_option(     '--xmin',        dest='xmin',        default=None,   help='x-axis min value           [default:%default]')
p.add_option(     '--xmax',        dest='xmax',        default=None,   help='x-axis max value           [default:%default]')
p.add_option(     '--xminfit',     dest='xminfit',     default=None,   help='x-axis min fit value       [default:%default]')
p.add_option(     '--xmaxfit',     dest='xmaxfit',     default=None,   help='x-axis max fit value       [default:%default]')
p.add_option(     '--yerr-min',    dest='yerr_min',    default=1,      help='y-axis error min value     [default:%default]')
p.add_option(     '--ymax-rotated',dest='ymax_rotated',default=None,   help='y-max for bottom plot      [default:%default]')
p.add_option(     '--nbins',       dest='nbins',       default=50,     help='rotated histo nbins        [default:%default]')
p.add_option('-x','--xthr',        dest='xthr',        default=0.0,    help='x-axis thresh in fraction  [default:%default]')
p.add_option('-y','--ythr',        dest='ythr',        default=0.05,    help='y-axis thresh in fraction  [default:%default]')
p.add_option('-z','--zthr',        dest='zthr',        default=0.1,    help='difference thresh in frac  [default:%default]')
p.add_option('-t','--triggers',    dest='triggers',    default=None,   help='trigger names: none = all  [default:%default]')
p.add_option('-T','--trig-file',   dest='trig_file',   default=None,   help='trigger file with list     [default:%default]')
p.add_option('-1','--xlegend',     dest='xlegend',     default=None,   help='x-ndc for legend           [default=%default]')
p.add_option('-2','--ylegend',     dest='ylegend',     default=None,   help='y-ndc for legend           [default=%default]')
p.add_option(     '--yunits',      dest='yunits',      default='nb',   help='y-axis units: pb, nb, mb   [default:%default]')
p.add_option(     '--l1dt-trig',   dest='l1dt_trig',   default='L1_EM18VH',help='Use this trigger for L1 deadtime for HLT [default:%default]')

## True/False options
p.add_option(     '--xml',         dest='xml',         default=False, action='store_true', help='Print XML     [default:%default]')
p.add_option(     '--latex',       dest='latex',       default=False, action='store_true', help='Print LaTex   [default:%default]')
p.add_option(     '--no-l1dt',     dest='no_l1dt',     default=False, action='store_true', help='No l1dt fix   [default: %default]')
p.add_option(     '--no-refit',    dest='no_refit',    default=False, action='store_true', help='No refit after outlier rejection [default: %default]')
p.add_option(     '--no-diff',     dest='no_diff',     default=False, action='store_true', help='No diff plot  [default:%default]')
p.add_option(     '--no-grid',     dest='no_grid',     default=False, action='store_true', help='No grid plot  [default:%default]')
p.add_option(     '--old-style',   dest='old_style',   default=False, action='store_true', help='Old 1x3 plot  [default:%default]')
p.add_option(     '--logy',        dest='logy',        default=False, action='store_true', help='Log scale     [default:%default]')
p.add_option(     '--test',        dest='test',        default=False, action='store_true', help='Test mode     [default:%default]')
p.add_option('-d','--debug',       dest='debug',       default=False, action='store_true', help='Debug mode    [default:%default]')
p.add_option('-i','--interactive', dest='interactive', default=False, action='store_true', help='Interactive   [default:%default]')
p.add_option(     '--no-timestamp',dest='no_timestamp',default=False, action='store_true', help='No timestamp  [default:%default]')
p.add_option(     '--extra-header',dest='extra_header',default=False, action='store_true', help='Extra header  [default:%default]')
p.add_option(     '--no-errbands', dest='no_errbands', default=False, action='store_true', help='No err bands  [default:%default]')
p.add_option(     '--no-save',     dest='no_save',     default=False, action='store_true', help='No err bands  [default:%default]')
p.add_option('-v','--verbose',     dest='verbose',     default=False, action='store_true', help='verbose       [default=%default]')
p.add_option('-?','--usage',       dest='help',        default=False, action='store_true', help='help & exit   [default=%default]')
p.add_option('-h','--help',        dest='help',        default=False, action='store_true', help='help & exit   [default=%default]')

# opts is from above, args are the arguments without a flag
(opts, args) = p.parse_args()

# Help
import sys
if opts.help:
    p.print_help()
    sys.exit(1)

# Hide for xml
if not opts.xml:
    print 'Hello world fitRoot.py'
    cmd = ' '.join(sys.argv)
    print cmd

#
# Import packages after parsing options
#

import ROOT
import math
import array
import os
import datetime

## Fit options
fopt = ''
fopt += 'N' # Do not plot after fit
fopt += 'S' # Save result
#fopt += 'E' # Minos errors
#fopt += 'M' # Improve fit
if opts.debug:
    fopt += 'V'
else:
    fopt += 'Q'

if not opts.interactive:
    ROOT.gROOT.SetBatch(True)

# For all pads
toffset= 1.20
nbunches = None

ROOT.TGaxis.SetMaxDigits(3)
ROOT.gStyle.SetNdivisions(1008, 'x')
ROOT.gStyle.SetNdivisions(1005, 'y')
ROOT.gStyle.SetLabelSize(0.06, 'x')
ROOT.gStyle.SetLabelSize(0.06, 'y')
ROOT.gStyle.SetLabelOffset(0.03, 'x')
ROOT.gStyle.SetLabelOffset(0.025, 'y')
ROOT.gStyle.SetTitleSize(0.06, 'x')
ROOT.gStyle.SetTitleSize(0.06, 'y')
ROOT.gStyle.SetTitleOffset(toffset, 'x')
ROOT.gStyle.SetTitleOffset(toffset, 'y')
ROOT.gStyle.SetTickLength(-0.03, 'x')
ROOT.gStyle.SetTickLength(-0.025, 'y')
ROOT.gStyle.SetFrameBorderMode(0) # this gets rid of the red line around the frame
ROOT.gStyle.SetOptStat(0)

## Keep track of warning & errors
n_warnings = 0
n_errors = 0

timestamp = None
if opts.run:
    run = opts.run
else:
    filebase = opts.rootfile.split('/')[-1]
    filetoks = filebase.split('_')
    run = '_'.join( filetoks[:-1] )

if run=='191933': timestamp = '2011-10-30'
if run=='190933': timestamp = '2011-10-13'

yunits = 1
if opts.yunits == 'pb': yunits *= 0.00001
if opts.yunits == 'nb': yunits *= 0.001
if opts.yunits == 'mb': yunits *= 1

# Prepare list of triggers
triglist = []
if opts.triggers:
    triglist = opts.triggers.split(',')
elif opts.trig_file:
    trig_file = open(opts.trig_file, 'r')
    for line in trig_file:
        if not line.strip(): continue
        triglist.append(line.rstrip())
else:
    print 'ERROR - Need to specify either --triggers or --trig-file'
    sys.exit()

#----------------------------------------------------------------------
def main(ttree):

    # List of all branches in the tree
    tbranch_list = [ k.GetName() for k in ttree.GetListOfBranches() ]

    # Make list of triggers
    ynames = []
    for bname in tbranch_list:
        btokens = bname.split('__')

        # xbp = cross-section before prescale
        if btokens[0]=='xbp':
            btokenittos = btokens[1].split('_')
            if opts.level=='all' or opts.level==btokenittos[0]:
                ynames.append(bname)

    if opts.print_trig:
        print_trig = opts.print_trig
        if opts.print_trig == 'all':
            print_trig = '_'

        trig_list = [ t for t in ynames if print_trig in t ]
        print '\n'.join(trig_list)
        print '--> Matches %s', print_trig
        sys.exit(0)

    # Define triggers
    if len(triglist):
        if opts.triggers=='test':
            ynames = ['rbp__L1_EM16VH']
        else:
            ynames = []
            for trigger in triglist:
                if trigger[:1]!='L1': rate_type = 'rav' # output rate
                else:                 rate_type = 'rbp' # input rate
                yname = rate_type+'__'+trigger
                ynames.append(yname)

    # XML/Latex headers
#	    if opts.xml: print '<xml>'

    if opts.latex:
        print '\\documentclass[margin, 11pt]{article}'
        print '\\topmargin 0in'
        print '\\oddsidemargin 0in'
        print '\\evensidemargin 0in'
        print '\\textwidth 6.5in'
        print '\\textheight 9in'
        print '\\usepackage[breaklinks]{hyperref}'
        print '\\usepackage{color}'
        print '\\usepackage{graphicx}'
        print '\\begin{document}'
        title = 'Trigger cross-section fitting for run %s' % run
        subtitle = 'One trigger per page'
        print '\\title{\\LARGE %s \\\\ {\\normalsize %s}}' % (title, subtitle)
        print '\\date{\\today}'
        print '\\author{Tae Min Hong}'
        print '\\maketitle'
        print '\\thispagestyle{empty}'
        print '\\listoffigures'

    # Make plots
    for yname in ynames:

        plot_status = make_plot(ttree, yname)
        if not plot_status:
            continue

        # Latex body
        if opts.latex:
            ytitle = get_ytitle(yname)
            eps = 'fit__' + yname + '.eps'
            print '\\clearpage\\begin{figure}[!ht]\\caption{%s}\\centering\\includegraphics[height=\\textheight]{%s}\\end{figure}' %(ytitle, eps)

    # Close
    if opts.latex: print '\\end{document}'
#	    if opts.xml: print '</xml>'


#----------------------------------------------------------------------
# Nomenclature:
#
# Dataset:
#   - "data" is reserved for the absolute x-section
#   - Zipped data sets are called:            databank['all']
#   - Filtered data sets will have a suffix:  databank['xacc']
#   - Successive filtered names are appended: databank['xyacc']
#   - However, "data - fit" will be called:   zipdiff
#
# Graphs:
#   Follow the dataset std but with graph_*
#
def make_plot(ttree, yname, xname = 'evt__mu'):

    ###############################################################################
    # Set-up canvas
    ###############################################################################


    ysize = 1000
    if opts.no_diff:
        ysize = 500

    xsize = int(700 / 0.8)

    # Canvas
    canvas = ROOT.TCanvas('c', 'c', xsize, ysize)
    canvas.SetFillColor(0)
    canvas.SetFrameLineColor(0)
    canvas.SetTopMargin(0.0)
    canvas.SetLeftMargin(0.0)
    canvas.SetBottomMargin(0.0)
#	    if zoom == None:
#	        canvas.SetLogy(True)

    # Pads
    pads = []
    if opts.no_diff:
        pads.append( ROOT.TPad('pad0','pad0', 0.0, 0.0, 0.8, 1.0) )
    else:
        pads.append( ROOT.TPad('pad0','pad0', 0.0, 0.5, 0.8, 1.0) )
        pads.append( ROOT.TPad('pad1','pad1', 0.0, 0.0, 0.8, 0.5) )
        pads.append( ROOT.TPad('pad2','pad2', 0.8, 0.5, 1.0, 1.0) )
        pads.append( ROOT.TPad('pad3','pad3', 0.8, 0.0, 1.0, 0.5) )
        if not opts.no_grid:
            pads[1].SetGridx()
            pads[1].SetGridy()
            pads[3].SetGridx()

    if opts.logy:
        pads[0].SetLogy()

    tmargin = 0.05
    lmargin = 0.15
    rmargin = 0.10
    bmargin = 0.18
    for pad in pads:
        pad.SetFrameLineColor(0)
        pad.SetLeftMargin(lmargin)
        pad.SetRightMargin(rmargin)
        pad.SetTopMargin(tmargin)
        pad.SetBottomMargin(bmargin)
        pad.SetFillColor(0)
        pad.SetFillStyle(0)
        pad.Draw()

    if len(pads)>1:
        pads[0].SetTopMargin(tmargin+bmargin)
        pads[0].SetRightMargin(0.0)
        pads[0].SetBottomMargin(0.0)
        pads[1].SetTopMargin(0.0)
        pads[1].SetBottomMargin(tmargin+bmargin)
        pads[1].SetFrameLineColor(1)
        pads[1].SetRightMargin(0.0)
        for i in [2, 3]:
            pads[i].SetLeftMargin(0.0)
            pads[i].SetRightMargin(0.10)
            pads[i].SetTopMargin(0.0)
            pads[i].SetBottomMargin(tmargin+bmargin)
            pads[i].SetFrameLineColor(0)


    ###############################################################################
    # Analysis
    ###############################################################################

    # Repositories
    databank = {}
    graphbank = {}
    fnbank = {}
    resultbank = {}

    # Settings
    ytitle = get_ytitle(yname)
    xpos = 0.15
    ypos = 0.85
    ygap = 0.07

    # Debug
    if opts.debug:
        print 'Fitting', yname

    #
    # Get data -- read ntuple, sort by x-value
    #

    oname = 'all'
    databank[oname] = get_data(ttree, xname, yname)
    graphbank[oname] = get_graph(oname, xname, yname, databank[oname])
    if not databank[oname]:
        print 'ERROR - Could not get data for "', oname, '" :', xname, yname
        return False

    #
    # Filter-out xy outliers.  Initial fit.
    #

    # Filter all -> x
    iname = 'all'
    oname = 'xaccept'
    databank[oname] = filter_1dim(databank[iname], 'x', opts.xthr, 50)
    graphbank[oname] = get_graph(oname, xname, yname, databank[oname])

    # Filter x -> xy. Declare fit function
    iname = 'xaccept'
    oname = 'xaccept,yaccept_temp'
    databank[oname] = filter_1dim(databank[iname], 'y', opts.ythr, 200)
    graphbank[oname] = get_graph(oname, xname, yname, databank[oname])

    # Filter x -> xy again
    iname = 'xaccept,yaccept_temp'
    oname = 'xaccept,yaccept'
    databank[oname] = filter_1dim(databank[iname], 'y', opts.ythr, 50)
    graphbank[oname] = get_graph(oname, xname, yname, databank[oname])

    # Do initial fit with xy-filtered data (no z-filter yet)
    xmin, xmax = get_xextrema(databank[oname])
    xminfit = float(opts.xminfit) if opts.xminfit else xmin
    xmaxfit = float(opts.xmaxfit) if opts.xmaxfit else xmax
    fnbank[oname], farg = get_fn(xminfit, xmaxfit, opts.fitfn)
    resultbank[oname] = graphbank[oname].Fit(fnbank[oname], fopt, '', xminfit, xmaxfit)
    if opts.verbose:
        print 'xminfit =', xminfit
        print 'xmaxfit =', xmaxfit

    #
    # Filter-out z-outliers.  Refit.
    #

    # Compute the difference between the fit, data -- overwrite
    iname = 'xaccept,yaccept'
    oname = 'xaccept,yaccept'
    databank[oname] = fill_zdata(graphbank[iname], fnbank[iname], databank[iname])

    # Filter z-dimension
    iname = 'xaccept,yaccept'
    oname = 'xaccept,yaccept,zaccept'
    databank[oname] = filter_1dim(databank[iname], 'z', opts.zthr)
    graphbank[oname] = get_graph(oname, '', '', databank[oname])

    # Refit
    fnbank[oname], farg = get_fn(xminfit, xmaxfit, opts.fitfn)
    resultbank[oname] = graphbank[oname].Fit(fnbank[oname], fopt, '', xminfit, xmaxfit)
    statcode = str(int(resultbank[oname])) # TODO does not work
#	    if not 'pol' in opts.fitfn:
#	        statcode = str(ROOT.gMinuit.fCstatu)

    # Compute chi2
    iname = 'xaccept,yaccept,zaccept'
    oname = 'xaccept,yaccept,zaccept,fitdiffonly'
    if opts.no_refit:
        iname = 'xaccept,yaccept'
        oname = 'xaccept,yaccept,fitdiffonly'

    databank[oname]  = fill_zdata(graphbank[iname], fnbank[iname], databank[iname], 1, True)
    graphbank[oname] = get_graph(oname, '', '', databank[oname])
    chi2, ndof = get_chi2(graphbank[oname])

    # Recompute fit-data difference
    iname = 'xaccept,yaccept,zaccept'
    oname = 'xaccept,yaccept,zaccept,fitdiff'
    if opts.no_refit:
        iname = 'xaccept,yaccept'
        oname = 'xaccept,yaccept,fitdiff'

    databank[oname]  = fill_zdata(graphbank[iname], fnbank[iname], databank[iname], 100)
    graphbank[oname] = get_graph(oname, '', '', databank[oname])

#	    #
#	    # Divide accepted/rejected data
#	    #
#	    iname = 'xaccept,yaccept,zaccept'
#	    oname = 'xyzreject'
#	    databank[oname] = filter_reject(databank['all'], databank[iname])
#	    graphbank[oname] = get_graph(oname, '', '', databank[oname])

    pad.Update()

    ###############################################################################
    # Pad bottom-right: difference histogram
    ###############################################################################

    # Profile y-axis
    ipad = 3
    pad = pads[ipad].cd()
    pad.SetLogx()

    iname = 'xaccept,yaccept,zaccept,fitdiff'
    oname = 'xaccept,yaccept,zaccept,fithist'

    # Get max from RMS -- 7 "sigma"
    hist_tmp = convert_graph_to_hist(graphbank[iname], -199, 199)
    h_rms_tmp= hist_tmp.GetRMS()
    ymax_rotated = int(h_rms_tmp * 7)
#	    ymax_rotated = 5
#	    if 'J'   in yname: ymax_rotated = 9
#	    if 'XE'  in yname: ymax_rotated = 19
#	    if 'FJ'  in yname: ymax_rotated = 29
#	    if '2FJ' in yname: ymax_rotated = 29
#	    if 'TE'  in yname: ymax_rotated = 49
    if opts.ymax_rotated: ymax_rotated = int(opts.ymax_rotated)
    ymin_rotated = -1*ymax_rotated
    if opts.debug:
        print 'RMS = ', h_rms_tmp

    # Get big histo for overflow/underflow
    bighist = convert_graph_to_hist(graphbank[iname], ymin_rotated, ymax_rotated, int(opts.nbins))
    bigaxis = bighist.GetXaxis()
    bignbins= bigaxis.GetNbins()

    # Limited x-range
    hist_xrange = convert_graph_to_hist(graphbank[iname], ymin_rotated, ymax_rotated, 1000, xminfit, xmaxfit)

    # Draw rotated histogram
    hist = convert_graph_to_hist(graphbank[iname], ymin_rotated, ymax_rotated)
    axis = hist.GetXaxis()
    nbins= axis.GetNbins()
    graphbank[oname] = hist
    xmin_rotated = 0.5
    xmax_rotated = math.pow(10, 1.1*math.log10( hist.GetMaximum() ))
    hname = hist.GetTitle() + '_rotate'

    # --> Draw a dummy histogram to set axes
    newhist = ROOT.TH2F(hname, '', 10, xmin_rotated, xmax_rotated, nbins, ymin_rotated*0.95, ymax_rotated*0.95);
    newhist.GetXaxis().SetNdivisions(203)
    newhist.SetTitle(';# of LB;')
    newhist.SetTitleSize(0, 'y')
    newhist.SetLabelSize(0, 'y')
    newhist.SetTitleOffset(0.50, 'x')
    newhist.SetLabelOffset(0.13, 'x')
    newhist.SetTitleSize(0.17, 'x')
    newhist.SetLabelSize(0.17, 'x')
    newhist.SetBit(ROOT.kCanDelete)
    newhist.SetDirectory(0)
    newhist.SetStats(0)
    newhist.GetYaxis().SetAxisColor(1)
    newhist.GetYaxis().SetTickLength(0.07)
    newhist.GetXaxis().SetTickLength(-0.030)
    newhist.Draw('X+')
    box = ROOT.TBox()
    box.SetFillColor(ROOT.kBlue)
    xoffset = math.pow(10, xmin_rotated)
    uflow = 0
    oflow = 0

    # Manually draw the histogram as "boxes"
    for i in xrange(1, bignbins+1):
        center = bigaxis.GetBinCenter(i)
        content = bighist.GetBinContent(i)
        if content <= 0:
            continue

        if center <= ymin_rotated:
            uflow += 1
            continue

        if center >= ymax_rotated:
            oflow += 1
            continue

        dy = bigaxis.GetBinWidth(i)
        x1 = 0
        x2 = content
        y1 = center - 0.3*dy
        y2 = center + 0.3*dy
        box.DrawBox(x1, y1, x2, y2)

    # Underflow
    box.SetFillColor(ROOT.kBlack)
    y1 = axis.GetBinCenter(1) - 0.3* axis.GetBinWidth(1)
    y2 = axis.GetBinCenter(1) + 0.3* axis.GetBinWidth(1)
    if uflow > 0: box.DrawBox(0, y1, uflow, y2)

    # Overflow
    box.SetFillColor(ROOT.kBlack)
    y1 = axis.GetBinCenter(nbins) - 0.3* axis.GetBinWidth(nbins)
    y2 = axis.GetBinCenter(nbins) + 0.3* axis.GetBinWidth(nbins)
    if oflow > 0: box.DrawBox(0, y1, oflow, y2)

    # Retrieve RMS, mean for later
    h_rms    = hist_xrange.GetRMS()
    h_rmserr = hist_xrange.GetRMSError()
    h_avg    = hist_xrange.GetMean()
    h_avgerr = hist_xrange.GetMeanError()

    pad.Update()

    ###############################################################################
    # Pad bottom-left: difference graph
    ###############################################################################

    ipad = 1
    pad = pads[ipad].cd()

    #
    # Prepare graph
    #

    iname = 'xaccept,yaccept,zaccept'
    oname = 'xaccept,yaccept,zaccept,fitdiff'
    graph = graphbank[oname]

    # Same x-axis as Pad 2
    hdum = ROOT.TH2F('hdum', '', 1, xmin, xmax, 1, ymin_rotated*0.95, ymax_rotated*0.95)
    hdum.SetTitle('')
    hdum.SetXTitle('Avg. interactions per pp crossing, <mu>')
    hdum.SetYTitle('(#color[4]{data} - #color[2]{fit}) / #color[4]{data} (%)     ')
    hdum.Draw()

    # Draw
    graph.GetHistogram().SetMaximum(ymax_rotated) # NB: Limits for y-axis
    graph.GetHistogram().SetMinimum(ymin_rotated) # NB: Limits for y-axis
    graph.SetMarkerColor(ROOT.kBlue)
    graph.SetMarkerSize(0.5)
    graph.SetMarkerStyle(20)
    graph.SetLineColor(ROOT.kBlack)
    graph.SetTitle(hdum.GetTitle())
    graph.GetXaxis().SetTitleOffset(toffset)
    graph.GetYaxis().SetTitleOffset(toffset)
    graph.SetLineWidth(0)
    graph.GetXaxis().SetTickLength(-0.03)
#	    graph.Draw('PZ')

    #
    # Error bands
    #

    if not opts.no_errbands:
        nbins = 100
        dx = (xmax - xmin)/nbins

        # Content -> Array
        b_list_0   = []
        b_list_avg = []
        b_list_mu  = []
        b_list_lo3 = []
        b_list_lo2 = []
        b_list_lo1 = []
        b_list_hi1 = []
        b_list_hi2 = []
        b_list_hi3 = []
        for i in xrange(nbins+1):
            b_list_0  .append( 0 )
            b_list_avg.append( h_avg )
            b_list_mu .append( xmin + i*dx )
            b_list_lo3.append( 3.*h_rms )
            b_list_lo2.append( 2.*h_rms )
            b_list_lo1.append( 1.*h_rms )
            b_list_hi1.append( 1.*h_rms )
            b_list_hi2.append( 2.*h_rms )
            b_list_hi3.append( 3.*h_rms )

        # List -> Array
        b_arr_0   = array.array('f', b_list_0)
        b_arr_avg = array.array('f', b_list_avg)
        b_arr_mu  = array.array('f', b_list_mu)
        b_arr_lo3 = array.array('f', b_list_lo3)
        b_arr_lo2 = array.array('f', b_list_lo2)
        b_arr_lo1 = array.array('f', b_list_lo1)
        b_arr_hi1 = array.array('f', b_list_hi1)
        b_arr_hi2 = array.array('f', b_list_hi2)
        b_arr_hi3 = array.array('f', b_list_hi3)

        # Array -> Graph
        b_graph_pm3 = ROOT.TGraphAsymmErrors(len(b_list_mu), b_arr_mu, b_arr_avg, b_arr_0, b_arr_0, b_arr_lo3, b_arr_hi3)
        b_graph_pm2 = ROOT.TGraphAsymmErrors(len(b_list_mu), b_arr_mu, b_arr_avg, b_arr_0, b_arr_0, b_arr_lo2, b_arr_hi2)
        b_graph_pm1 = ROOT.TGraphAsymmErrors(len(b_list_mu), b_arr_mu, b_arr_avg, b_arr_0, b_arr_0, b_arr_lo1, b_arr_hi1)
        b_graph_pm3.SetTitle(';;')
        b_graph_pm2.SetTitle(';;')
        b_graph_pm1.SetTitle(';;')
        b_graph_pm3.SetFillStyle(1001)
        b_graph_pm2.SetFillStyle(1001)
        b_graph_pm1.SetFillStyle(1001)
        b_graph_pm3.SetFillColor(ROOT.kCyan)
        b_graph_pm2.SetFillColor(ROOT.kYellow)
        b_graph_pm1.SetFillColor(ROOT.kGreen)
        b_graph_pm3.SetLineWidth(0)
        b_graph_pm2.SetLineWidth(0)
        b_graph_pm1.SetLineWidth(0)
        b_graph_pm3.Draw('L3')
        b_graph_pm2.Draw('L3')
        b_graph_pm1.Draw('L3')

    #
    # Draw again
    #

    # Don't draw error on difference plot ('Z' draws it without feet)
    graph.Draw('PX SAME')

    # Line at y=0
    tline = ROOT.TLine()
    tline.SetLineColor(ROOT.kRed)
    tline.SetLineWidth(4)
    tline.DrawLine(xmin, h_avg, xmaxfit, h_avg)

    # Set symmetric y-axis
    set_symmetric_yaxis(graph)

    pad.RedrawAxis()
    pad.Update()


    ###############################################################################
    # Pad top
    ###############################################################################

    ipad = 0
    pad = pads[ipad].cd()

    iname = 'xaccept,yaccept'
    oname = 'xaccept,yaccept,zaccept'
    if opts.no_refit:
        oname = 'xaccept,yaccept'

    graph = graphbank[iname]
    fitfn = fnbank[oname]

    numlb = graph.GetN()

#	    xaxis   = graph.GetXaxis()
#	    firstbin= xaxis.GetFirst()
#	    lastbin = xaxis.GetLast()
#	    xmin    = xaxis.GetBinLowEdge(firstbin)
#	    xmax    = xaxis.GetBinLowEdge(lastbin)+xaxis.GetBinWidth(lastbin)

   # Dummy plot for axis
    ymin = graph.GetYaxis().GetXmin()
    ymax = graph.GetYaxis().GetXmax()
    if opts.ymin: ymin = float(opts.ymin)
    if opts.ymax: ymax = float(opts.ymax)
    hdum2 = ROOT.TH2F('hdum2', '', 1, xmin, xmax, 1, ymin, ymax)
    hdum2.SetTitle(';;trig. cross-section (%s)' % opts.yunits)
    hdum2.SetLabelSize(0,'X')
    hdum2.Draw()

    graph.GetXaxis().SetTickLength(0.02)
    graph.GetXaxis().SetLimits(xmin, xmax)
    graph.SetMarkerColor(ROOT.kBlue)
    graph.SetMarkerSize(0.5)
    graph.SetMarkerStyle(20)
    graph.SetLineColor(15) # Dark gray
    graph.SetLineWidth(0)
    graph.SetTitle(';;trig. cross-section (%s)' % opts.yunits)
    graph.GetYaxis().SetTitleOffset(toffset)
    graph.Draw('PZ')
#	    graph.DrawCopy('APZ')
#	    ttext.DrawTextNDC(xpos, ypos, 'Close-up')

    #
    # Error bands
    #

    if not opts.no_errbands:

        iname = 'xaccept,yaccept,zaccept'
        fitfn = fnbank[iname]

        # Content -> Array
        t_list_0   = []
        t_list_avg = []
        t_list_mu  = []
        t_list_lo3 = []
        t_list_lo2 = []
        t_list_lo1 = []
        t_list_hi1 = []
        t_list_hi2 = []
        t_list_hi3 = []
        for i in xrange(nbins+1):
            mu = xmin + i*dx
            y = fitfn.Eval(mu)
            t_list_0  .append( 0 )
            t_list_avg.append( y )
            t_list_mu .append( mu )
            t_list_lo3.append( 3.*y*h_rms/100. )
            t_list_lo2.append( 2.*y*h_rms/100. )
            t_list_lo1.append( 1.*y*h_rms/100. )
            t_list_hi1.append( 1.*y*h_rms/100. )
            t_list_hi2.append( 2.*y*h_rms/100. )
            t_list_hi3.append( 3.*y*h_rms/100. )

        # List -> Array
        t_arr_0   = array.array('f', t_list_0)
        t_arr_avg = array.array('f', t_list_avg)
        t_arr_mu  = array.array('f', t_list_mu)
        t_arr_lo3 = array.array('f', t_list_lo3)
        t_arr_lo2 = array.array('f', t_list_lo2)
        t_arr_lo1 = array.array('f', t_list_lo1)
        t_arr_hi1 = array.array('f', t_list_hi1)
        t_arr_hi2 = array.array('f', t_list_hi2)
        t_arr_hi3 = array.array('f', t_list_hi3)

        # Array -> Graph
        t_graph_pm3 = ROOT.TGraphAsymmErrors(len(t_list_mu), t_arr_mu, t_arr_avg, t_arr_0, t_arr_0, t_arr_lo3, t_arr_hi3)
        t_graph_pm2 = ROOT.TGraphAsymmErrors(len(t_list_mu), t_arr_mu, t_arr_avg, t_arr_0, t_arr_0, t_arr_lo2, t_arr_hi2)
        t_graph_pm1 = ROOT.TGraphAsymmErrors(len(t_list_mu), t_arr_mu, t_arr_avg, t_arr_0, t_arr_0, t_arr_lo1, t_arr_hi1)
        t_graph_pm3.SetTitle(';;')
        t_graph_pm2.SetTitle(';;')
        t_graph_pm1.SetTitle(';;')
        t_graph_pm3.SetFillStyle(1001)
        t_graph_pm2.SetFillStyle(1001)
        t_graph_pm1.SetFillStyle(1001)
        t_graph_pm3.SetFillColor(ROOT.kCyan)
        t_graph_pm2.SetFillColor(ROOT.kYellow)
        t_graph_pm1.SetFillColor(ROOT.kGreen)
        t_graph_pm3.SetLineWidth(0)
        t_graph_pm2.SetLineWidth(0)
        t_graph_pm1.SetLineWidth(0)
        t_graph_pm3.Draw('L3')
        t_graph_pm2.Draw('L3')
        t_graph_pm1.Draw('L3')

    #
    # Draw again
    #

    graph.SetLineWidth(0)
    graph.Draw('PZ SAME')
    fitfn.Draw('SAME')

#	    # Draw rejects
#	    oname = 'xyzreject'
#	    graph = graphbank[oname]
#	    graph.SetMarkerColor(ROOT.kRed)
#	    graph.Draw('PZ SAME')


    # Add x-axis bunch lumi on top (uses local coord) 
    # Use mu = 71.5 * bunchlumi / 11.24558
    bunchlumimin = xmin/71.5*11.24558
    bunchlumimax = xmax/71.5*11.24558
    topaxis = ROOT.TGaxis(xmin, ymin, xmax, ymin, bunchlumimin, bunchlumimax, 1005, '+')
    topaxis.SetLabelOffset(-0.095)
    topaxis.SetLabelSize(0.06)
    topaxis.Draw()

    ttext = ROOT.TLatex()
    ttext.SetTextSize(0.06)
    ttext.SetNDC()
    ttext.DrawLatex(0.36, 0.114,'Inst. lumi. per p bunch (10^{30}cm^{-2}s^{-1})')

    pad.RedrawAxis()
    pad.Update()



    ###############################################################################
    # Top Legend
    ###############################################################################

    ipad = 0
    pad = pads[ipad].cd()

#	    # Add timestamp
#	    timestamp
    lydelta = 0.10
    lxmin = 0.23 #0.27
    lymax = 0.98

    #   if opts.exp_only: lymax -= lydelta
    if opts.xlegend: lxmin = float(opts.xlegend)
    if opts.ylegend: lymax = float(opts.ylegend)

    lymin  = lymax - 2*lydelta - 0.05
    #   if not opts.show_exclusion: lymin += lydelta

    tlegend = ROOT.TLegend(lxmin, lymin, lxmin+0.2, lymax, '', 'NDC')
    tlegend.SetBorderSize(0)
    tlegend.SetFillStyle(0)
    tlegend.SetFillColor(0)
    tlegend.SetTextSize(0.06)
#	    tlegend.SetNColumns(2)
    graph = graphbank['xaccept,yaccept,zaccept,fitdiff']
    graph.SetLineWidth(1)
    graph.SetLineWidth(3)
    ltitle = 'Exp\'d'

    # Print stats
    parvals = []
    parerrs = []
    oname = 'xaccept,yaccept,zaccept'
    if opts.no_refit:
        oname = 'xaccept,yaccept'

    fitfn = fnbank[oname]
    for i in xrange(fitfn.GetNumberFreeParameters()):
        parvals.append(fitfn.GetParameter(i))
        parerrs.append(fitfn.GetParError(i))

    # Header
    tname = yname.split('__')[1]
    headers = []
    headers.append('%s' % tname)
    if opts.extra_header: headers.append(opts.extra_header)
    header = ' '.join(headers)
    if len(header)>0:
        # Header
        tlegend.SetHeader(header)

        # Line 1
        graph.SetFillColor(0)
        graph.SetLineColor(12)
        graph.SetLineWidth(1)

        runsuffix = 's' if len(run.split('_'))>1 else ''
        ltext = '#color[4]{Run%s %s' % (runsuffix, run.replace('_',', '))
        if not opts.no_timestamp and timestamp:
            ltext += ' on %s' % timestamp
        ltext += ' (%d LBs, %d bunches)}' % (numlb, nbunches)
        tlegend.AddEntry(graph, ltext, 'PF') # P=marker, L=line, F=Fill

        # Line 2
        graph_clone = graph.Clone(graph.GetName()+'_clone')
        graph_clone.SetLineWidth(3)
        graph_clone.SetLineColor(ROOT.kRed)
        graph_clone.SetFillColor(ROOT.kGreen)
        valform, errform = get_fit_text(parvals, parerrs)
        ltext = '#color[2]{"%s" %s}' % (opts.fitfn, valform)
        tlegend.AddEntry(graph_clone, ltext, 'LF') # P=marker, L=line, F=Fill

    tlegend.Draw()

    # Debug
    if opts.debug:
        print 'Fitted', yname

    ###############################################################################
    # Bottom Legend
    ###############################################################################

    ipad = 1
    pad = pads[ipad].cd()

    color = ROOT.kBlack #colors_exp[excl_arrow]
    ttn = 3
    ttx = 0.47
    tty = 0.45 # 0.95

    tt = ROOT.TPaveText()       ; tt2 = ROOT.TPaveText()
    tt.SetX1NDC(ttx)            ; tt2.SetX1NDC(ttx)
    tt.SetX2NDC(ttx + 0.4)      ; tt2.SetX2NDC(ttx + 0.4)
    tt.SetY2NDC(tty)            ; tt2.SetY2NDC(0.96)
    tt.SetY1NDC(tty - ttn*0.075); tt2.SetY1NDC(0.96 - 2*0.075)
    tt.SetTextColor(ROOT.kBlack); tt2.SetTextColor(ROOT.kBlack)
    tt.SetTextSize(0.06)        ; tt2.SetTextSize(0.06)
    tt.SetFillStyle(0)          ; tt2.SetFillStyle(0)
    tt.SetBorderSize(0)         ; tt2.SetBorderSize(0)
    tt.AddText('Percent diff. w/ RMS bands')
    tt.AddText('#color[2]{Mean (%.2f #pm %.2f)%%}' % (h_avg, h_avgerr))
    tt.AddText('#color[3]{RMS (%.2f #pm %.2f)%%}' % (h_rms, h_rmserr))
    #tt2.AddText('Status : %s' %statcode)
    tt2.AddText('Chi2 / Ndof = %d / %d' % (int(chi2), ndof))

#	    ttext = ROOT.TLatex()
#	    ttext.SetNDC()
#	    ttext.SetTextSize( 0.04 )
#	    ttext.SetTextColor(ROOT.kRed)
#	    ttext.SetTextColor(ROOT.kRed)
#	    ttext.SetTextAngle(270)
#	    ttext.SetTextSize(0.17)
#	    ttext.DrawLatex(0.65, 0.50, 'avg: %.2f #pm %.2f' % (h_avg, h_avgerr))
#	    ttext.DrawLatex(0.45, 0.50, 'rms: %.2f #pm %.2f' % (h_rms, h_rmserr))

    tt.Draw()
    tt2.Draw('SAME')



    ###############################################################################
    # Fit text
    ###############################################################################

#	    # dotted line every 5%
#	    ny = int(ymin*100 % 5)
#	    for i in xrange(1, ny+1):
#	        tline.SetLineStyle(2)
#	        tline.DrawLine(xmin, -0.05*i, xmax, -0.05*i)
#	        tline.DrawLine(xmin, 0.05*i, xmax, 0.05*i)

    # text
#	    ttext.DrawTextNDC(xpos, ypos, 'Fractional difference')
#	    ttext.DrawTextNDC(xpos, ypos-1*ygap, 'Chi2 / Ndof = %.1f / %d' %(chi2, ndof))

#	    ipad = 2
#	    pad = pads[ipad].cd()
#
#	    iname = 'all'
#	    oname = 'xaccept,yaccept,zaccept'
#
#	    graphbank[iname].SetMarkerColor(ROOT.kRed)
#	    draw_graph(graphbank[iname], True, True)
#	    draw_graph(graphbank[oname], False, True)
#	    fnbank[oname].Draw('SAME')
#
#	    ttext.SetTextColor(ROOT.kBlue)
#	    ttext.SetTextSize( 0.05 )
#	    ttext.DrawTextNDC(xpos, ypos, 'Entire range (red points not fitted)')

#	    valform, errform = get_fit_text(parvals, parerrs)
#
#	    ttext.DrawTextNDC(xpos+0.05, ypos-3*ygap, 'Fit status = %s' %statcode)
#	    ttext.DrawTextNDC(xpos+0.05, ypos-4*ygap, 'Chi2 / NDOF = %f / %d = %f' % (chi2, ndof, chi2/ndof))

#	    ttext.DrawTextNDC(xpos+0.05, ypos-1*ygap, 'f(x) = %s'   %valform)
#	    ttext.DrawTextNDC(xpos+0.05, ypos-2*ygap, 'err = [%s]'  %errform)

    #
    # Save
    #

    save_canvas(yname, canvas)
    if opts.xml:
        print '<trigger>'
        print ' <run>%s</run>'              % run
        print ' <xaxis>%s</xaxis>'          % xname[5:] # strip 'evt__'
        print ' <yaxis>%s</yaxis>'          % yname[5:] # strip 'xbp__'
        print ' <xunit>%s</xunit>'          % 'avg interactions'
        print ' <yunit>%s</yunit>'          % 'nb'
        print ' <ynotes>%s</ynotes>'        % yname[:3] # expand_abbrev( 'xbp' ) #yname[:3] )
        print ' <xnotes>%s</xnotes>'        % 'using 71.5mb'
        print ' <function>%s</function>'    % farg
        print ' <rms>%f</rms>'              % h_rms
        print ' <rmserr>%f</rmserr>'        % h_rmserr
        print ' <mean>%f</mean>'            % h_avg
        print ' <meanerr>%f</meanerr>'      % h_avgerr
        print ' <status>%s</status>'        % statcode
        for i in xrange(len(parvals)): print ' <par id="%d">%f</par>' % (i, float(parvals[i]))
        for i in xrange(len(parerrs)): print ' <err id="%d">%f</err>' % (i, float(parerrs[i]))
        print ' <chi2>%f</chi2>'            % float(chi2)
        print ' <ndof>%d</ndof>'            % ndof
        print ' <chi2perdof>%f</chi2perdof>'% float(chi2/ndof)
        print '</trigger>'

    return True


#----------------------------------------------------------------------
def get_ytitle(yname):
    ytitle = ''
    ytokens = yname.split('__')
    ytitle += ' '.join(ytokens[1:])
    yprefix = ytokens[0]
    if   yprefix[0] == 'x': ytitle += ' xsec'
    elif yprefix[0] == 'r': ytitle += ' rate'
    elif yprefix[0] == 't': ytitle += ' cnts'
    if   yprefix[1:] == 'bp': ytitle += ' before ps'
    elif yprefix[1:] == 'ap': ytitle += ' after ps'
    elif yprefix[1:] == 'av': ytitle += ' after veto'
    return ytitle #.replace('_', '\\_')


#----------------------------------------------------------------------
def set_same_xrange(default_graph, new_graph):
    xaxis = default_graph.GetXaxis()
    firstbin = xaxis.GetFirst()
    lastbin = xaxis.GetLast()
    xmin = xaxis.GetBinLowEdge(firstbin)
    xmax = xaxis.GetBinLowEdge(lastbin)+xaxis.GetBinWidth(lastbin)
    default_graph.GetXaxis().SetLimits(xmin, xmax) # NB: Limits for x-axis
    new_graph.GetXaxis().SetLimits(xmin, xmax) # NB: Limits for x-axis
    return xmin, xmax


#----------------------------------------------------------------------
def set_symmetric_yaxis(graph):
    yaxis = graph.GetYaxis()
    firstbin = yaxis.GetFirst()
    lastbin = yaxis.GetLast()
    ymin = yaxis.GetBinLowEdge(firstbin)
    ymax = yaxis.GetBinLowEdge(lastbin)+yaxis.GetBinWidth(lastbin)
    tmax = math.fabs(ymin)
    if math.fabs(ymax) > math.fabs(tmax):
        tmax = ymax

    ymin = -1*math.fabs(tmax)
    ymax = math.fabs(tmax)
    yaxis.SetRangeUser(ymin, ymax) # NB: RangeUser for y-axis
    return


#----------------------------------------------------------------------
def expand_abbrev(abbrev):
    if abbrev=='tbp':       return 'count before prescale'
    if abbrev=='tap':       return 'count after prescale'
    if abbrev=='tav':       return 'count at output'
    if abbrev=='rbp':       return 'rate before prescale'
    if abbrev=='rap':       return 'rate after prescale'
    if abbrev=='rav':       return 'rate at output'
    if abbrev=='xbp':       return 'cross-section before prescale'
    if abbrev=='xap':       return 'cross-section after prescale'
    if abbrev=='xav':       return 'cross-section at output'
    if abbrev=='bunchlumi': return 'luminosity per bunch'
    if abbrev=='lumi':      return 'luminosity per crossing'
    if abbrev=='mu':        return 'mu'
    return abbrev


#----------------------------------------------------------------------
def save_canvas(yname, canvas):

    if opts.test:
        yname += '_test'

    # Save eps
#	    if not opts.test:
#	        eps = os.path.join(opts.outdir, 'fit__' + yname + '.eps')
#	        if opts.debug: print 'Saving', eps
#	        canvas.SaveAs(eps)

    # Save png
    png = os.path.join(opts.outdir, 'fit__' + yname + '.png')
    if opts.test:
        print 'Printing', png

    if opts.debug: print 'Saving', png
    canvas.SaveAs(png)

#	    # Save gif
#	    gif = eps[:-3] + 'gif'
#	    os.system('convert -resize x%s -antialias -colors 64 -format gif %s %s' % (opts.imageh, eps, gif) )
#
#	    # Save gif thumb
#	    gif = eps[:-3] + 'thumb.gif'
#	    os.system('convert -resize x%s -antialias -colors 64 -format gif %s %s' % (opts.thumbh, eps, gif) )


#----------------------------------------------------------------------
def get_fit_text(parvals, parerrs):

    valform = ''
    errform = ''

    for i, parval in enumerate(parvals):
        sign = ''
        if i > 0 and parval >= 0:
            sign += '+'

        if 'pol' == opts.fitfn[:len('pol')]:
            if   i==0:              valform += '%.3g'        %parval
            elif i==1:              valform += '%s%.3g*x'    %(sign, parval)
            else:                   valform += '%s%.3g*x^%d' %(sign, parval, i)
        elif 'expo' == opts.fitfn[:len('expo')]:
            if   i==0:              valform += 'e^('
            if   i==0:              valform += '%.3g'        %parval
            elif i==1:              valform += '%s%.3g*x'    %(sign, parval)
            else:                   valform += '%s%.3g*x^%d' %(sign, parval, i-2)
            if   i==len(parvals)-1: valform += ')'
        elif 'pow' == opts.fitfn[:len('pow')]:
            if   i==0:              valform += '%.3g'        %parval
            elif i==1:              valform += '%s%.3g*x^('  %(sign, parval)
            elif i==2:              valform += '%.3g'        %parval
            elif i==3:              valform += '%s%.3g*x'    %(sign, parval)
            elif i>3:               valform += '%s%.3g*x^%d' %(sign, parval, i-2)
            if   i==len(parvals)-1: valform += ')'
        elif 'zexp' == opts.fitfn[:len('zexp')]:
            if   i==0:              valform += '%.3g+e^('    %parval
            elif i==1:              valform += '%.3g'        %parval
            elif i==2:              valform += '%s%.3g*x'    %(sign, parval)
            elif i>2:               valform += '%s%.3g*x^%d' %(sign, parval, i-1)
            if   i==len(parvals)-1: valform += ')'
        elif 'met' == opts.fitfn[:len('met')]:
            if   i==0:              valform += '%.3g'        %parval
            elif i==1:              valform += '%s%.3g*e^('  %(sign, parval)
            elif i==2:              valform += '%.3g/x'      %parval
            elif i==3:              valform += '%s%.3g*x'    %(sign, parval)
            elif i>3:               valform += '%s%.3g*x^%d' %(sign, parval, i-2)
            if   i==len(parvals)-1: valform += ')'
        elif 'just' == opts.fitfn[:len('just')]:
#	            if   i==0:              valform += '%.3g'        %parval
#	            elif i==1:              valform += '%s%.3g*x'    %(sign, parval)
#	            elif i==2:              valform += '%s%.3g*e^('  %(sign, parval)
#	            elif i==3:              valform += '%.3g*x'      %parval
#	            elif i>3:               valform += '%s%.3g*x^%d' %(sign, parval, i-1)
#	            if   i==len(parvals)-1: valform += ')'
            if   i==0:              valform += '%.3g'        %parval
            elif i==1:              valform += '%s%.3g*x+e^('%(sign, parval)
            elif i==2:              valform += '%.3g'        %parval
            elif i==3:              valform += '%s%.3g*x'    %(sign, parval)
            elif i>3:               valform += '%s%.3g*x^%d' %(sign, parval, i-1)
            if   i==len(parvals)-1: valform += ')'


    # Errors
    for i, parval in enumerate(parvals):
        parerr = parerrs[i]
        if   i==0: errform += '%.3g' %parerr
        else:      errform += ', %.3g' %parerr

    valform = valform.replace('e+0','e').replace('e-0','e-')
    errform = errform.replace('e+0','e').replace('e-0','e-')
    return valform, errform

#----------------------------------------------------------------------
def get_chi2(graph):

    chi2 = 0
    ndof = 0 #graph.GetN()+1
    xval = ROOT.Double(0)
    yval = ROOT.Double(0)
    for i in xrange(1, graph.GetN()+1):
        graph.GetPoint(i, xval, yval)
        yerr = graph.GetErrorY(i)
        if yerr > 0:
            chi2 += yval*yval / (yerr*yerr)
            ndof += 1

    return chi2, ndof


#----------------------------------------------------------------------
def convert_graph_to_hist(graph, ymin, ymax, nbins=5, xminfit=None, xmaxfit=None):

    pname = graph.GetName() + '_yhist%d_%dbins' % (ymax, nbins)
    if xminfit: pname += '_xmin%f' % xminfit
    if xmaxfit: pname += '_xmax%f' % xmaxfit

    hist = ROOT.TH1F(pname, pname, nbins, ymin, ymax)
    xval = ROOT.Double(0)
    yval = ROOT.Double(0)
    for i in xrange(1, graph.GetN()+1):
        graph.GetPoint(i, xval, yval)
        if xminfit:
            if xval < xminfit:
                continue

        if xmaxfit:
            if xval > xmaxfit:
                continue

        yerr = graph.GetErrorY(i)
        hist.Fill(yval, 1)

    hist.SetMaximum( 1.2 * hist.GetMaximum() )
    hist.SetFillColor(1)
    return hist


#----------------------------------------------------------------------
# zdata = difference between the fit & ydata
#
def find_data_in_set(x, y, zipdata):

    # Unpack data
    xdata, ydata, zdata, xdataerr, ydataerr, zdataerr = unzip_data(zipdata)

    # Look to match x, y
    idx = -1
    for i in xrange(len(xdata)):
        if x==xdata[i] and y==ydata[i]:
            idx = i
            break

    # Did we find it?
    found = i > -1

    # Pop found element
    if found:
        if opts.debug:
            print 'Found', i, x, y
        xdata.pop(i)
        ydata.pop(i)
        zdata.pop(i)
        xdataerr.pop(i)
        ydataerr.pop(i)
        zdataerr.pop(i)
        rezipdata = zip_data(xdata, ydata, zdata, xdataerr, ydataerr, zdataerr)
    else:
        if opts.debug:
            print 'Not found', i, x, y
        rezipdata = zipdata

    return found, rezipdata


#----------------------------------------------------------------------
# zdata = difference between the fit & ydata
#
def filter_reject(zipdata_all, zipdata):

    # Unpack data
    xdata_all, ydata_all, zdata_all, xdataerr_all, ydataerr_all, zdataerr_all = unzip_data(zipdata_all)

    # Find it
    xdata = []
    ydata = []
    zdata = []
    xdataerr = []
    ydataerr = []
    zdataerr = []
    for i in xrange(len(xdata_all)):
        if len(zipdata) < 1:
            break

        found, rezipdata = find_data_in_set(xdata_all[i], ydata_all[i], zipdata)
        if found:
            zipdata = rezipdata
        else:
            xdata.append( xdata_all[i] )
            ydata.append( ydata_all[i] )
            zdata.append( zdata_all[i] )
            xdataerr.append( xdataerr_all[i] )
            ydataerr.append( ydataerr_all[i] )
            zdataerr.append( zdataerr_all[i] )

    zipdata_reject = zip_data(xdata_all, ydata_all, zdata_all, xdataerr_all, ydataerr_all, zdataerr_all)
    return zipdata_reject


#----------------------------------------------------------------------
# zdata = difference between the fit & ydata
#
def fill_zdata(graph, fitfn, zipdata, scale=1, diffonly=False):

    # Compute difference
    zdata = []
    zdataerr = []
    xval = ROOT.Double(0)
    yval = ROOT.Double(0)
    for i in xrange(1, graph.GetN()+1):
        graph.GetPoint(i, xval, yval)
        yerr = graph.GetErrorY(i)
        yfit = fitfn.Eval(xval)
        zval = -1
        zerr = 0
        if yval > 0:
            zval = yval - yfit
            zerr = yerr

            # Do this for % diff, but not chi2
            if not diffonly:
                zval /= yval
                zerr /= yval

            # Scale by 1 or 100 for %
            zval *= scale
            zerr *= scale

        zdata   .append( zval )
        zdataerr.append( zerr )

    # Unpack original data with dummy z-information
    xdata, ydata, zdata_dummy, xdataerr, ydataerr, zdataerr_dummy = unzip_data(zipdata)

    # Make sure array sizes are the same
    if len(zdata) != len(zdata_dummy):
        print 'ERROR - fill_zdata'
        sys.exit(1)

    # Repack with filled z-info
    zipdata_zdata = zip_data( xdata, ydata, zdata, xdataerr, ydataerr, zdataerr)

    return zipdata_zdata


#----------------------------------------------------------------------
def get_data(ttree, xname, yname):
    #
    # vtos = observed counts after prescale
    # cnts = vtos / prescale-factor
    # rate = cnts / time-period
    # xsec = rate / lumi
    #
    if len(xname) < 1 or len(yname) < 1:
        if opts.debug: 'ERROR - get_data -- null xname, yname', xname, yname
        return None

    nentries = ttree.GetEntries()
    if nentries < 1:
        if opts.debug: 'ERROR - get_data -- null nentries', nentries
        return None

    xdata = []
    ydata = []
    zdata = []
    xdataerr = []
    ydataerr = []
    zdataerr = []
    icounter = 0
    for ientry in xrange(nentries):
        ttree.GetEntry(ientry)
        xpoint = ttree.__getattr__(xname) # 'evt__mu'
        ypoint = ttree.__getattr__(yname) # 'xbp__L1_XE30'
        Xpoint = ttree.__getattr__('evt__lumi') # bugged: 'evt__bunchlumi') * 1e3
        lb     = ttree.__getattr__('evt__lb')
        global nbunches
        if not nbunches:
            nbunches = ttree.__getattr__('evt__bunches')

        #
        # Cuts
        #

        # xsec = rate/bunchlumi
        if Xpoint>0:
            ypoint /= Xpoint
        else:
            continue

        # Lumiblock
        if opts.lbmin:
            if lb < opts.lbmin:
                continue

        if opts.lbmax:
            if lb > opts.lbmax:
                continue

        if opts.nmax :
            if ientry > opts.nmax:
                continue

        # Time
        tpoint = ttree.__getattr__('evt__lblength')
        if tpoint<55 or tpoint>65: continue

        # x-value
        if opts.xmin:
            if xpoint < float(opts.xmin):
                continue

        if opts.xmax:
            if xpoint > float(opts.xmax):
                continue

        # After veto
        yname_tav = ''
        try:
            yname_tav = 'tav__' + yname.split('__')[1]
        except:
            print 'ERROR - invalid trigger tav name for the y-axis', yname
            n_errors += 1
            return None

        # After prescale
        yname_tap = ''
        try:
            yname_tap = 'tap__' + yname.split('__')[1]
        except:
            print 'ERROR - invalid trigger tap name for the y-axis', yname
            n_errors += 1
            return None

#        if opts.debug: print ientry, xpoint, ypoint

        # Error on y-axis
        ypointerr = 0
        errcnt = ttree.__getattr__(yname_tav) # tav__L1_XE30'
        if errcnt > 0:
            ypointfracerr = 1./math.sqrt(errcnt)
            ypointerr = ypoint * ypointfracerr

        # L1 deadtime = After veto / After prescale
        # Use these triggers for L1 deadtime correction
        trig_tav = 'tav__'+opts.l1dt_trig
        trig_tap = 'tap__'+opts.l1dt_trig
        if 'L1' == yname_tav.split('__')[0][:2]:
            trig_tav = yname_tav
            trig_tap = yname_tap

        l1_tav = ttree.__getattr__(trig_tav)
        l1_tap = ttree.__getattr__(trig_tap)
        l1_dt  = 1
        if opts.no_l1dt==False:
            if l1_tav > 0:
                l1_dt = l1_tap / l1_tav
                ypoint    *= l1_dt
                ypointerr *= l1_dt
#	                if opts.debug: print 'l1_dt for ', trig_tav, l1_dt

        # Units
        ypoint    /= yunits
        ypointerr /= yunits

        # Cut on min number of events
        if errcnt < opts.tap_min:
            continue

        # Append
        xdata.append( xpoint )
        ydata.append( ypoint )
        zdata.append( None )
        xdataerr.append( 0.0 )
        ydataerr.append( ypointerr )
        zdataerr.append( None )
        icounter += 1

    zipdata = zip_data(xdata, ydata, zdata, xdataerr, ydataerr, zdataerr)
    return zipdata


#----------------------------------------------------------------------
def get_graph(gname, xname, yname, zipdata):
    if len(zipdata) < 1:
        return None

    # Unpack data
    xdata, ydata_dummy, zdata_dummy, xdataerr, ydataerr_dummy, zdataerr_dummy = unzip_data(zipdata)

    # Configure
    ptitle = ''
    xtitle = ''
    if 'bunchlumi' in xname:
        xtitle = 'inst. lumi. per proton bunch (10^{30} cm^{-2} s^{-1})'
    elif 'mu' in xname:
        xtitle = '<mu> = <interactions> / bunch crossing'

    ytitle = '(trigger rate) / (bunch luminosity) [mb]'
    ydata = ydata_dummy
    ydataerr = ydataerr_dummy
    if 'diff' in gname:
        ytitle = '(data - fit) / data'
        ydata = zdata_dummy
        ydataerr = zdataerr_dummy

    # Fill graph
    graph = ROOT.TGraphErrors(len(xdata),
                              array.array('f', xdata),
                              array.array('f', ydata),
                              array.array('f', xdataerr),
                              array.array('f', ydataerr) )

#	    # Patch from Elliot & Freddy
#	    if opts.yerr_min:
#	        for i in range(0,graph.GetN()):
#	            graph.SetPointError(i,0,float(opts.yerr_min))

    # Let me take ownership over ROOT for interative plot for 1 trigger
    if opts.interactive and len(opts.triggers.split(','))==1:
        print opts.triggers
        ROOT.SetOwnership(graph, False)

    # Customize
    graph.SetName( '%s__%s__vs__%s' % (gname, xname, yname) )
    graph.SetTitle('%s;%s;%s' % (ptitle, xtitle, ytitle) )
#	    graph.SetMarkerSize(0.8)
#	    graph.SetMarkerStyle(20)
#	    graph.SetLineWidth(1)
#	    graph.GetXaxis().SetLabelSize(0.05)
#	    graph.GetXaxis().SetTitleSize(0.05)
#	    graph.GetYaxis().SetLabelSize(0.05)
#	    graph.GetYaxis().SetTitleSize(0.05)
#	    if 'diff' in gname:
#	        graph.GetXaxis().SetLabelSize(0.045)
#	        graph.GetXaxis().SetTitleSize(0.045)
#	        graph.GetYaxis().SetLabelSize(0.045)
#	        graph.GetYaxis().SetTitleSize(0.045)

    color = ROOT.kBlack
    graph.SetMarkerColor(color)
    graph.SetMarkerColor(color)
    graph.SetLineColor(ROOT.kCyan)
    graph.SetFillColor(color)
#	    graph.GetXaxis().SetTitleOffset(0.95)
#	    graph.GetYaxis().SetTitleOffset(1.5)
#	    graph.GetYaxis().SetNdivisions(506)
#	    graph.GetXaxis().SetNdivisions(506)

#	    if 'diff' in gname:
#	        graph.GetYaxis().SetNdivisions(505)

    return graph


#----------------------------------------------------------------------
def draw_graph(graph, first=True, rightside=False):

    # Options
    gopt = 'PZ'
    if rightside and not opts.old_style:
        print 'Right side-axis'
        gopt += 'Y+'

    # Draw
    if first:
        graph.Draw('A'+gopt)
    else:
        graph.Draw(gopt+' SAME')

    # Draw again
    graph.SetLineWidth(0)
    graph.Draw(gopt+' SAME')
    return


#----------------------------------------------------------------------
# Return fit function
#
def get_fn(xmin, xmax, fitfn='pol2'):
    farg = fitfn

    #
    # pow_0 = a + b*x^c
    # pow_1 = a + b*x^(c + d*x)
    # pow_2 = a + b*x^(c + d*x + e*x*x)
    #
    if 'pow' in fitfn:

        order = 0
        if fitfn.count('_') > 0:
            order = int(fitfn.split('_')[1])

        farg = '[0]+[1]*pow(x,[2]'
        for i in xrange(order):
            farg += '+[%d]*' %(i+3)
            farg += '*'.join(['x' for i in xrange(i+1)])

        farg += ')'

    #
    # zexp_1 = a + b*expo^(c*x)
    # zexp_2 = a + b*expo^(c*x + d*x*x)
    #
    elif 'zexp' in fitfn:

        order = 0
        if fitfn.count('_') > 0:
            order = int(fitfn.split('_')[1])

        farg = '[0]+exp([1]'
        for i in xrange(order):
            farg += '+[%d]*' %(i+2)
            farg += '*'.join(['x' for i in xrange(i+1)])

        farg += ')'

    #
    # met_0 = a + b*expo^(b/x)
    # met_1 = a + b*expo^(b/x + d*x)
    #
    elif 'met' in fitfn:

        order = 0
        if fitfn.count('_') > 0:
            order = int(fitfn.split('_')[1])

        farg = '[0]+[1]*exp([2]/x'
        for i in xrange(order):
            farg += '+[%d]*' %(i+3)
            farg += '*'.join(['x' for i in xrange(i+1)])

        farg += ')'

    #
    # just_1 = a + bx + c*expo^(d*x)
    # just_2 = a + bx + c*expo^(d*x + e*x*x)
    #
    elif 'just' in fitfn:

        order = 0
        if fitfn.count('_') > 0:
            order = int(fitfn.split('_')[1])

        farg = '[0]+[1]*x+exp([2]'
        for i in xrange(order):
#	            if i>0:
#	                farg += '+'
            farg += '+[%d]*' %(i+3)
            farg += '*'.join(['x' for i in xrange(i+1)])

        farg += ')'


    if opts.debug:
        print 'Fit function is', farg

    fitfn = ROOT.TF1('fitfn', farg, xmin, xmax)
    for i in xrange(fitfn.GetNumberFreeParameters()):
        if   i==0: fitfn.SetParameter(i, float(opts.par0)) ; fitfn.SetParError(i, float(opts.err0))
        elif i==1: fitfn.SetParameter(i, float(opts.par1)) ; fitfn.SetParError(i, float(opts.err1))
        elif i==2: fitfn.SetParameter(i, float(opts.par2)) ; fitfn.SetParError(i, float(opts.err2))
        elif i==3: fitfn.SetParameter(i, float(opts.par3)) ; fitfn.SetParError(i, float(opts.err3))
        elif i==4: fitfn.SetParameter(i, float(opts.par4)) ; fitfn.SetParError(i, float(opts.err4))
        elif i==5: fitfn.SetParameter(i, float(opts.par5)) ; fitfn.SetParError(i, float(opts.err5))
        else     : fitfn.SetParameter(i, 1) ; fitfn.SetParError(i, 1)
        if opts.verbose:
            print 'Parameter %f Error %f' %(fitfn.GetParameter(i), fitfn.GetParError(i))

    # set style
    fitfn.SetLineColor(ROOT.kRed)
    fitfn.SetLineWidth(4)

    return (fitfn, farg)


#----------------------------------------------------------------------
def get_plotextrema(zipdata, fitfn, sigma=3):
    xdata, ydata, zdata, xdataerr, ydataerr, zdataerr = unzip_data(zipdata)
    ymaxset = [ y + sigma*ey for y, ey in zip(ydata, ydataerr) ]
    yminset = [ y - sigma*ey for y, ey in zip(ydata, ydataerr) ]
    for y in ydata:
        f = fitfn.Eval(y)

    ymin = min(yminset)
    ymax = max(ymaxset)
    return ymin, ymax


#----------------------------------------------------------------------
def get_yextrema(zipdata, sigma=3):
    xdata, ydata, zdata, xdataerr, ydataerr, zdataerr = unzip_data(zipdata)
    ymaxset = [ y + sigma*ey for y, ey in zip(ydata, ydataerr) ]
    yminset = [ y - sigma*ey for y, ey in zip(ydata, ydataerr) ]
    ymin = min(yminset)
    ymax = max(ymaxset)
    return ymin, ymax


#----------------------------------------------------------------------
def get_xextrema(zipdata):
    xdata, ydata, zdata, xdataerr, ydataerr, zdataerr = unzip_data(zipdata)
    xmin = min(xdata)
    xmax = max(xdata)
    return xmin, xmax


#----------------------------------------------------------------------
# Returns a zip form of a 6-tuple sorted by the first argument
def zip_data(xdata, ydata, zdata, xdataerr, ydataerr, zdataerr):
    zipdata = zip(xdata, ydata, zdata, xdataerr, ydataerr, zdataerr)
    zipdata.sort()
    return zipdata


#----------------------------------------------------------------------
# Returns a 6-tuple: xdata, ydata, zdata, xdataerr, ydataerr, zdataerr
def unzip_data(zipdata):
    return map(list, zip(*zipdata))


#----------------------------------------------------------------------
def filter_1dim(zipdata, axis, threshold, nbins=20, returnreject=False):

    # unpack input into arrays
    xdata, ydata, zdata, xdataerr, ydataerr, zdataerr = unzip_data(zipdata)

    # axes
    data = None
    if   axis=='x': data = xdata
    elif axis=='y': data = ydata
    elif axis=='z': data = zdata

    # declare histogram
    hmin = min(data)
    hmax = max(data)
    hwidth = (hmax - hmin)/(nbins+1)
    hist = ROOT.TH1F('hist', 'hist', nbins, hmin, hmax+hwidth)

    # fill histogram
    for i, val in enumerate(data):
        hist.Fill(val)
#	        if opts.debug: print i, val

    # find bin with largest content
    maxbincontent = 0
    for bin in range(1, hist.GetNbinsX()+1):
        bincontent = hist.GetBinContent(bin)
        if bincontent > maxbincontent:
            maxbincontent = bincontent

    #
    # Restrict min, max to those above threshold
    #

    # set cut-off at threshold of the maxbincontent
    bincutoff = maxbincontent * float(threshold)

    # find the first bin to have ybincontent > bincutoff
    binmin_filt = 0
    for i in xrange(1, hist.GetNbinsX()+1):
        if hist.GetBinContent(i) > bincutoff:
            binmin_filt = i
            break

    # find the last bin to have ybincontent > bincutoff
    binmax_filt = 0
    for i in xrange(1, hist.GetNbinsX()+1):
        if hist.GetBinContent(i) > bincutoff:
            binmax_filt = i

    if axis=='z' and opts.debug:
        print hist.Print('all')
        print 'maxbincontent =', bincontent
        print 'binmin        =', binmin_filt
        print 'binmax        =', binmax_filt
        c = ROOT.TCanvas()
        hist.DrawCopy()
        l = ROOT.TLine()
        l.DrawLine(hist.GetXaxis().GetXmin(), bincutoff, hist.GetXaxis().GetXmax(), bincutoff)
        c.SaveAs("~/www/tmp/a.png")
        if opts.interactive:
            c.WaitPrimitive()

    # convert bin-center to x-value
    min_filt = hist.GetBinLowEdge(binmin_filt)
    max_filt = hist.GetBinLowEdge(binmax_filt)+hist.GetBinWidth(binmax_filt)

    # split original list into 'accept' or 'reject'
    accept_xdata    = []
    accept_ydata    = []
    accept_zdata    = []
    accept_xdataerr = []
    accept_ydataerr = []
    accept_zdataerr = []
    reject_xdata    = []
    reject_ydata    = []
    reject_zdata    = []
    reject_xdataerr = []
    reject_ydataerr = []
    reject_zdataerr = []
    for idx_val, val in enumerate(data):
        if val < min_filt or val > max_filt:
            reject_xdata    .append( xdata      [idx_val] )
            reject_ydata    .append( ydata      [idx_val] )
            reject_zdata    .append( zdata      [idx_val] )
            reject_xdataerr .append( xdataerr   [idx_val] )
            reject_ydataerr .append( ydataerr   [idx_val] )
            reject_zdataerr .append( zdataerr   [idx_val] )
        else:
            accept_xdata    .append( xdata      [idx_val] )
            accept_ydata    .append( ydata      [idx_val] )
            accept_zdata    .append( zdata      [idx_val] )
            accept_xdataerr .append( xdataerr   [idx_val] )
            accept_ydataerr .append( ydataerr   [idx_val] )
            accept_zdataerr .append( zdataerr   [idx_val] )

    zipdata_accept = zip_data(accept_xdata, accept_ydata, accept_zdata, accept_xdataerr, accept_ydataerr, accept_zdataerr)
    zipdata_reject = zip_data(reject_xdata, reject_ydata, reject_zdata, reject_xdataerr, reject_ydataerr, reject_zdataerr)

    if returnreject:
        return zipdata_accept, zipdata_reject

    return zipdata_accept


#	#----------------------------------------------------------------------
#	def zipdata_to_tcanvas():
#	    pass
#
#
#	#----------------------------------------------------------------------
#	def get_canvas(yname, npad = 3):
#	    cname = 'canvas__' + yname
#	    canvasw = 1000
#	    canvash = 1000
#	    npadw   = 2
#	    npadh   = 2
#	    marginw = 0.15
#	    gapw    = 0.02
#	    if opts.old_style:
#	        canvasw = 500
#	        canvash = 1500
#	        npadw   = 1
#	        npadh   = 3
#
#	    margintop = 0.08
#	    marginbot = 0.10
#
#	    canvas = ROOT.TCanvas(cname, cname, canvasw, canvash)
#	    canvas.Divide(npadw, npadh, 0, 0, 0)
#	    canvas.SetFrameBorderSize(0)
#	    canvas.SetMargin(0, 0, 0, 0)
#	    canvas.SetFillColor(ROOT.kWhite)
#	    for i in xrange(1, npadw*npadh+1):
#	        pad = canvas.cd(i)
#	        pad.SetFillColor(ROOT.kWhite)
#	        pad.SetLeftMargin(gapw)
#	        pad.SetRightMargin(marginw)
#	        pad.SetBottomMargin(marginbot)
#	        pad.SetTopMargin(margintop)
#	#	        pad.SetTickx()
#	#	        pad.SetTicky()
#
#	    if not opts.old_style:
#	        # Left
#	        for i in [1, 3]:
#	            pad = canvas.cd(i)
#	            pad.SetLeftMargin(marginw)
#	            pad.SetRightMargin(gapw)
#
#	        # Top
#	        for i in [3, 4]:
#	            pad.SetTopMargin(margintop+marginbot)
#	            pad.SetBottomMargin(0)
#
#	        # Bottom
#	        for i in [3, 4]:
#	            pad.SetTopMargin(0)
#	            pad.SetBottomMargin(margintop+marginbot)
#
#	        pad = canvas.cd(3)
#	        pad.SetFrameLineColor(1)
#
#	    return canvas


#======================================================================
if __name__ == '__main__':

    # File
    tfile = None
    ttree = None
    fname = '%s/%s_%s.root' % (opts.rootdir, opts.run, opts.db)
    if opts.rootfile: fname = opts.rootfile
    if opts.test:     fname = 'test.root'

    tfile = ROOT.TFile(fname)
    ttree = tfile.Get('ntp1')
    if not ttree:
        print 'ERROR - could not open ROOT file', fname
        sys.exit(1)

    # Main
    main(ttree)

#eof
