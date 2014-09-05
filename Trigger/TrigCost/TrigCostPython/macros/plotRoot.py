# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# Tae Min Hong (tmhong@hep.upenn.edu)
#
# Plots (rates or xsection) from the output of makeRoot.py.
#
# Note:       Fancier version of simplePlotRoot.py
# 
# Usage:      python plotRoot.py [OPTIONS]
#
# What to edit: As a normal user, modify these functions:
#
#     - get_triglist()    defines the trigger groups
#     - get_trig_colors() defines the trigger colors

# Kept here as reference:
#
#   run = '190503_cool:58'      # 25ns trains on Oct. 6, 2011
#   run = '190504_cool:58'      # 25ns trains on Oct. 7, 2011
#   run = '190505_cool:58'      # 25ns trains on Oct. 7, 2011
#   run = '191628_trp:10'       # high-<mu> run on Oct. 25, 2011
#   run = '191715_cool:1854'    # 50ns trains
#   run = '191920_cool:1331'    # 50ns trains on Oct. 22, 2011
#   run = '189561_cool:1317'    # 50ns trains
#   run = '191426_cool:1332'    # 50ns trains : default
#
# Examples:
"""
python -i plotRoot.py --run=167607
python -i plotRoot.py --interactive --yaxis=xbp
"""

print 'Hello world plotRoot.py'
import sys
cmd = ' '.join(sys.argv)
print cmd

# Parse command-line options
import optparse
preamble='usage: %prog [options]'
p = optparse.OptionParser(usage=preamble, add_help_option=False, version='%prog')

# Arg't-required options
p.add_option('-r', '--run',         dest='run',     default=None,   help='Comma-separated run:nbunches:nbins [default: %default]')
p.add_option('-y', '--yaxis',       dest='yaxis',   default='rbp',  help='y type: rbp, xbp      [default: %default]')
p.add_option('-t', '--trig',        dest='trig',    default='test', help='--print-keys to list  [default: %default]')
p.add_option(      '--xminplot',    dest='xminplot',default=None,   help='x min plot value      [default: %default]')
p.add_option(      '--xmaxplot',    dest='xmaxplot',default=None,   help='x max plot value      [default: %default]')
p.add_option(      '--yminplot',    dest='yminplot',default=None,   help='y min plot value      [default: %default]')
p.add_option(      '--ymaxplot',    dest='ymaxplot',default=None,   help='y max plot value      [default: %default]')
p.add_option(      '--xmincut',     dest='xmincut', default=None,   help='x min cut value       [default: %default]')
p.add_option(      '--xmaxcut',     dest='xmaxcut', default=None,   help='x max cut value       [default: %default]')
p.add_option(      '--ymincut',     dest='ymincut', default=None,   help='y min cut value       [default: %default]')
p.add_option(      '--ymaxcut',     dest='ymaxcut', default=None,   help='y max cut value       [default: %default]')
p.add_option(      '--fitmin',      dest='fitmin',  default=None,   help='x fit min value       [default: %default]')
p.add_option(      '--fitmax',      dest='fitmax',  default=None,   help='x fit max value       [default: %default]')
p.add_option(      '--xunits',      dest='xunits',  default=1000,   help='x unit in mil, 1e30   [default: %default]')
p.add_option(      '--yunits',      dest='yunits',  default=1000,   help='y unit in mb,Hz       [default: %default]')
p.add_option(      '--plot',        dest='plot',    default='prof', help='prof[ile], gr[aph]    [default: %default]')
p.add_option(      '--xname',       dest='xname',   default='lumi', help='lumi, bunchlumi, time [default: %default]')
p.add_option(      '--rebin',       dest='rebin',   default='auto', help='auto, 1, 2, ...       [default: %default]')

# Arg't-not-required options
p.add_option('-?','--usage',        dest='help',       action='store_true', default=False, help='show this message and exit         [default=%default]')
p.add_option('-h','--help',         dest='help',       action='store_true', default=False, help='show this message and exit         [default=%default]')
p.add_option(     '--logy',         dest='logy',       action='store_true', default=False, help='log10 for y-axis                   [default: %default]')
p.add_option('-b','--bad-style',    dest='bad_style',  action='store_true', default=False, help='Bad "ATLAS" style                  [default: %default]')
p.add_option('-l','--print-keys',   dest='print_keys', action='store_true', default=False, help='Lists trig groups                  [default: %default]')
p.add_option('-i','--interactive',  dest='interactive',action='store_true', default=False, help='Interactive mode                   [default: %default]')
p.add_option('-d','--debug',        dest='debug',      action='store_true', default=False, help='Debug mode                         [default: %default]')
p.add_option('-v','--verbose',      dest='verbose',    action='store_true', default=False, help='Debug mode                         [default: %default]')

(opts, args) = p.parse_args()

# Help
if opts.help:
    p.print_help()
    sys.exit(1)

#
# Import packages after parsing options
#

import ROOT
import sys
import re
import math
import array

print 'python -i %s' %(' '.join(sys.argv))

# Root settings
ROOT.gErrorIgnoreLevel = 1001
ROOT.TGaxis.SetMaxDigits(3)
ROOT.gStyle.SetOptStat(False)
ROOT.gStyle.SetFrameFillColor(ROOT.kWhite)
ROOT.gStyle.SetFrameBorderMode(0) # this gets rid of the red line around the frame
ROOT.gStyle.SetLabelOffset(0.02, 'X')
ROOT.gStyle.SetLabelOffset(0.02, 'Y')
if not opts.bad_style: # Not bad "official" style
    ROOT.gStyle.SetFrameLineColor(ROOT.kWhite)
    ROOT.gStyle.SetTickLength(-0.02, 'X')
    ROOT.gStyle.SetTickLength(-0.02, 'Y')

if not opts.interactive:
    ROOT.gROOT.SetBatch(True)
else:
    print 'Remember the -i in `python -i plotRoot.py`'

# Set default values
xmin = 0
xmax = 3.4
ymin = 0
ymax = 12
if 'x' == opts.yaxis[0]:
    xmin = 0
    xmax = 35
    ymin = 0
    ymax = 4
    if   'single'   in opts.trig: ymax = 4.0
    elif 'combined' in opts.trig: ymax = 4.4
    elif 'all'      in opts.trig: ymax = 4.4
    elif 'jets'     in opts.trig: ymax = 4.0

#	    # Bad "Official" style
#	    if opts.bad_style:
#	        xmax = 3.5
    if opts.logy:
        ymin = 0.01
        ymax = 10.0

elif 'r' == opts.yaxis[0]:
    if   'jets'     in opts.trig: ymax = 2.0

    # Bad "Official" style
    if opts.bad_style:
        xmin = 1.0
        xmax = 3.5

# X-axis name
xname = 'evt__' + opts.xname

# Over-ride
if opts.xminplot: xmin = float(opts.xminplot)
if opts.xmaxplot: xmax = float(opts.xmaxplot)
if opts.yminplot: ymin = float(opts.yminplot)
if opts.ymaxplot: ymax = float(opts.ymaxplot)

#----------------------------------------------------------------------
def get_triglist(grpname):
    """
    Define trigger groups
    """

    triglist = []
    trigdict = {}
    trigdict['test'] = [
            'L1_EM16VH',
            'L1_MU11',
            ]
    trigdict['single'] = [
            'L1_EM16VH',
            'L1_MU11',
            'L1_TAU30',
            'L1_XE50',
            'L1_J75',
            ]
    trigdict['combined'] = [
            'L1_2TAU8_EM10VH',
            'L1_2TAU11',
            'L1_2MU4',
            'L1_2EM12',
            'L1_4J10',
            ]
    trigdict['all'] = [
            'L1_2TAU8_EM10VH',
            'L1_2TAU11',
            'L1_EM16VH',
            'L1_MU11',
            'L1_2MU4',
            'L1_TAU30',
            'L1_2EM12',
            'L1_4J10',
            'L1_XE50',
            'L1_J75',
            ]
    trigdict['all_alt'] = [
            'L1_2TAU11',
            'L1_EM16VH',
            'L1_MU11',
            'L1_2MU4',
            'L1_TAU30',
            'L1_2EM12',
            'L1_4J10',
            'L1_XE50',
            'L1_J75',
            ]
    trigdict['jets_all'] = [
            'L1_3J15',
            'L1_3J20',
            'L1_3J50',
            'L1_4J10',
            'L1_4J15',
            'L1_4J20',
            'L1_J75',
            'L1_XE50',
            ]
    trigdict['jets'] = [
            'L1_4J10',
            'L1_4J15',
            'L1_4J20',
            'L1_J75',
            'L1_XE50',
            ]

    # Bad "official" style
    if opts.bad_style:

        # Take out 'XE' from cross-section plots
        if 'x' == opts.yaxis[0]:
            for key in trigdict.keys():
                triglist = []
                for item in trigdict[key]:

                    if 'XE' not in item:
                        triglist.append( item )

                trigdict[key] = triglist


    # Print keys
    if opts.print_keys:
        for key in trigdict.keys():
            print '%s\n\t' %key, '\n\t'.join(trigdict[key])

        sys.exit(1)

    # Get list
    trignames = None
    for key in trigdict.keys():
        r = re.compile(key)
        if r.match(grpname):
            trignames = trigdict[grpname]
            break

    # Key not in list, so send user input
    if not trignames:
        trignames = opts.trig.split(',')

    return trignames

#----------------------------------------------------------------------
def get_trig_colors(trigname, idx=0):
    """
    Define trigger colors.  Note:
        enum EColor
        {
            kWhite=0,
            kBlack=1,
            kGray=920,
            kRed =632,
            kGreen=416,
            kBlue=600,
            kYellow=400,
            kMagenta=616,
            kCyan=432,
            kOrange=800,
            kSpring=820,
            kTeal=840,
            kAzure=860,
            kViolet=880,
            kPink=900
        };
    """
    color = None

    # Hard-coded color for these triggers
    colordict = {}
    colordict['L1_EM16VH']      = ROOT.kRed
    colordict['L1_2EM12']       = ROOT.kPink
    colordict['L1_MU11']        = ROOT.kBlue
    colordict['L1_2MU4']        = ROOT.kBlue - 7
    colordict['L1_TAU30']       = ROOT.kBlack
    colordict['L1_2TAU11']      = ROOT.kGray + 2
    colordict['L1_2TAU8_EM10VH']= ROOT.kGray + 3
    colordict['L1_XE50']        = ROOT.kGreen
    colordict['L1_J75']         = ROOT.kRed + 2
    colordict['L1_4J10']        = ROOT.kMagenta - 9
    colordict['L1_4J15']        = ROOT.kMagenta
    colordict['L1_4J20']        = ROOT.kMagenta + 2
    colordict['L1_3J15']        = ROOT.kCyan + 4
    colordict['L1_3J20']        = ROOT.kCyan + 2
    colordict['L1_3J50']        = ROOT.kCyan

    # Find the color for my trigger
    for key in colordict.keys():
        r = re.compile(key)
        if r.match(trigname):
            color = colordict[trigname]
            break

    # If my trigger not hard-coded, determine according to index
    colorlist = []
    colorlist.append(ROOT.kBlack)
    colorlist.append(ROOT.kRed)
    colorlist.append(ROOT.kBlue)
    colorlist.append(ROOT.kGreen)
    colorlist.append(ROOT.kMagenta)
    colorlist.append(ROOT.kGray)
    colorlist.append(ROOT.kOrange)
    colorlist.append(ROOT.kCyan)
    colorlist.append(12) # Dark gray
    colorlist.append(ROOT.kPink)
    colorlist.append(ROOT.kSpring)
    if not color:
        color = colorlist[ idx % len(colorlist) ]

    return color

#----------------------------------------------------------------------
def contrasting_color(plot):
    color = plot.GetMarkerColor()
    if color == ROOT.kRed           : return ROOT.kBlue
    if color == ROOT.kPink          : return ROOT.kCyan
    if color == ROOT.kBlue          : return ROOT.kCyan
    if color == ROOT.kViolet        : return ROOT.kCyan
    if color == ROOT.kBlack         : return ROOT.kCyan
    if color == ROOT.kGray + 2      : return ROOT.kBlack
    if color == ROOT.kGray + 3      : return ROOT.kBlack
    if color == ROOT.kGreen         : return ROOT.kBlack
    if color == ROOT.kPink          : return ROOT.kBlack
    if color == ROOT.Magenta - 4    : return ROOT.kBlue
    if color == ROOT.kMagenta       : return ROOT.kBlue
    if color == ROOT.kMagenta + 2   : return ROOT.kBlue
    if color == ROOT.kCyan + 4      : return ROOT.kRed
    if color == ROOT.kCyan + 2      : return ROOT.kRed
    if color == ROOT.kCyan          : return ROOT.kRed
    return ROOT.kBlack

#----------------------------------------------------------------------
def zip_data(xarray, yarray, zarray, xarrayerr, yarrayerr, zarrayerr):
    """
    Returns a zip form of a 6-tuple sorted by the first argument
    """
    datazip = zip(xarray, yarray, zarray, xarrayerr, yarrayerr, zarrayerr)
    datazip.sort()
    return datazip

#----------------------------------------------------------------------
def unzip_data(datazip):
    """
    Returns a 6-tuple: xarray, yarray, zarray, xarrayerr, yarrayerr, zarrayerr
    """
    return map(list, zip(*datazip))

#----------------------------------------------------------------------
def get_data_from_tree(ttree, run, nbunch, trignames, xname):
    """
    Returns dictionary: trigname -> 6-tuple : see zip_data()
    No cuts are made.
    Note:
        vtos = observed counts after prescale
        cnts = vtos / prescale-factor
        rate = cnts / time-period
        xsec = rate / lumi

    -- Method 1 -- Loop over TTree.
    We employ this method because we can get event-wise info as well
    as the counting errors for the corresponding trigger.

    -- Method 2 -- Quick & dirty inline method -> TGraph
    ttree.Project(hname, pname, cname)
    graph = ROOT.TGraph(ttrees[i].GetSelectedRows(), ttrees[i].GetV2(), ttrees[i].GetV1())

    -- Method 3 -- Quick & dirty inline method -> TH1F
    ttree.Project(hname, pname, cname)
    graph = ROOT.gROOT.FindObject(hname)
    assert isinstance(graph, ROOT.TH1F)
    """

    databank = dict()

    # Non-empty tree
    nentries = ttree.GetEntries()
    if nentries < 1:
        if opts.debug: 'ERROR: get_data -- null nentries', nentries
        return None

    # Dictionary of lists: trigname -> list, we will zip the list later
    xarray = {}
    yarray = {}
    zarray = {}
    xarrayerr = {}
    yarrayerr = {}
    zarrayerr = {}
    for trigname in trignames:
        xarray[trigname] = []
        yarray[trigname] = []
        zarray[trigname] = []
        xarrayerr[trigname] = []
        yarrayerr[trigname] = []
        zarrayerr[trigname] = []

    # Convert L1_EM16VH -> rbp__L1_EM16VH
    ynames = get_ynames(trignames)
    cnames = get_ynames(trignames, True)

    # Loop over entries
    for ientry in xrange(nentries):

        # Load event in memory
        ttree.GetEntry(ientry)

        # Retrieve locally: ttree -> dataarray
        dataarray = {}
        n='evt__lb';        dataarray[n] = ttree.__getattr__(n)
        n='evt__lblength';  dataarray[n] = ttree.__getattr__(n)
        n=xname;            dataarray[n] = ttree.__getattr__(n) # 'evt__lumi'
        for n in ynames:    dataarray[n] = ttree.__getattr__(n) # eg, 'rbp__L1_EM16VH'
        for n in cnames:    dataarray[n] = ttree.__getattr__(n) # eg, 'tav__L1_EM16VH'

        #
        # Process data: dataarray -> xarray, etc.
        #

        # Local values
        lb       = dataarray['evt__lb']
        lblength = dataarray['evt__lblength']
        yarraytmp= [ dataarray[n] for n in ynames ]
        x_raw    = dataarray[xname] / float(opts.xunits) # lumi
        x        = x_raw
        if 'x' == opts.yaxis[0]:
            x *= 71.5 * 1e6 /(11245. * nbunch)

        # Quality control on L1_EM16VH noise
        y = 0
        if dataarray['evt__lumi'] > 0:
            y = ttree.__getattr__('rbp__L1_EM16VH')/dataarray['evt__lumi']

        # Cut grl
        if not pass_grl(run, lb, y):
            continue

        # Cut quality
        if not pass_quality(run, lblength, yarraytmp, x, x_raw):
            continue

        # Loop over triggers
        for idx_trig, trigname in enumerate(trignames):

            # Variable name
            yname = ynames[idx_trig]
            cname = cnames[idx_trig]

            # Get data
            y_raw = dataarray[yname] / float(opts.xunits)
            c_raw = dataarray[cname] # counts

            # For cross-section: <mu> = lumi (10^30)*71.5mb/(11245Hz*nbunches)
            y = y_raw
            if 'x' == opts.yaxis[0]:
                y /= x_raw

            # Get 1/sqrt(N) frac error from "counts after veto"
            yfracerr = 0
            if c_raw > 0:
                yfracerr = 1. / math.sqrt(c_raw)

            # Fill array
            xarray[trigname].append( x )
            yarray[trigname].append( y )
            zarray[trigname].append( None )
            xarrayerr[trigname].append( 0. )
            yarrayerr[trigname].append( y * yfracerr )
            zarrayerr[trigname].append( None )

            # Debug
            if opts.debug:
                print 'get_data_from_tree --', ientry, x, y

        # Clean-up
        dataarray.clear()

    # Save: xarray, etc. -> databank
    for trigname in trignames:
        databank[trigname] = zip_data(xarray[trigname],
                                      yarray[trigname],
                                      zarray[trigname],
                                      xarrayerr[trigname],
                                      yarrayerr[trigname],
                                      zarrayerr[trigname])

        # Debug
        if opts.verbose:
            print 'get_data_from_tree -- Got `data` for',trigname

    return databank

#----------------------------------------------------------------------
def create_graph_from_datazip(gname, xname, trigname, datazip):
    """
    Convert datazip -> TGraph
    """
    if len(datazip) < 1:
        return None

    # Unpack data
    xarray, yarray, zarray, xarrayerr, yarrayerr, zarrayerr = unzip_data(datazip)

    # Use z-axis for 'difference'
    if 'diff' in gname:
        ytitle = '(data - fit) / data'
        yarray = zarray
        yarrayerr = zarrayerr

    # Fill graph
    graph = ROOT.TGraphErrors(len(xarray),
                              array.array('f', xarray),
                              array.array('f', yarray),
                              array.array('f', xarrayerr),
                              array.array('f', yarrayerr)
                              )
    graph.SetName(gname)
    graph.SetTitle(gname)

    # Let me take ownership over ROOT
    ROOT.SetOwnership(graph, False)

    return graph

#----------------------------------------------------------------------
def format_plot(plot, i=0, j=0):
    """
    i = tree index = per run
    j = trig index = per trigger
    """

    classname = plot.ClassName()

    # Set marker style depends on i
    marker = ROOT.kFullCircle
    if   i==0: marker = ROOT.kFullCircle # kOpenCircle
    elif i==1: marker = ROOT.kFullSquare
    elif i>=2: marker = 33 # kOpenDiamond # 33=FullDiamond not enum
    if 'r' == opts.yaxis[0]:
        marker = ROOT.kFullCircle

    plot.SetMarkerStyle(marker)

    # Set color depends on j
    trigname = plot.GetName().split('__')[-1]
    color = get_trig_colors(trigname, j)
    plot.SetMarkerColor(color)
    plot.SetLineColor(color)
    plot.SetFillColor(color)

    # Set axes ranges
    plot.GetXaxis().SetLimits(xmin, xmax)
    plot.GetYaxis().SetRangeUser(ymin, ymax)

    # Set line size
    plot.SetLineWidth(1)

    # Set marker size
    msize = 1
    if   marker==ROOT.kFullCircle:  msize = 0.5
    elif marker==ROOT.kOpenCircle:  msize = 0.8
    elif marker==ROOT.kFullSquare:  msize = 0.5
    elif marker==ROOT.kOpenDiamond: msize = 0.8
    elif marker==33:                msize = 0.8

    plot.SetMarkerSize(msize)

    # Set offsets
    plot.GetXaxis().SetTitleOffset(1.30)
    plot.GetYaxis().SetTitleOffset(1.20)
    plot.GetXaxis().SetNdivisions(505)
    plot.GetYaxis().SetNdivisions(505)

    # Set titles
    if 'x' == opts.yaxis[0]:
        plot.SetTitle(';<#mu> interactions per bunch crossing;Cross section (#mub)')
    else:
        yunits = '10^{30}'
        xunits = 'Hz'
        if float(opts.yunits) == 1000: yunits = '10^{33}'
        if float(opts.xunits) == 1000: xunits = 'kHz'
        plot.SetTitle(';Luminosity (%s cm^{-2} s^{-1});Rate (%s)' %(yunits, xunits))

#	    # Set font size
#	    plot.GetXaxis().SetLabelSize(0.05)
#	    plot.GetXaxis().SetTitleSize(0.05)
#	    plot.GetYaxis().SetLabelSize(0.05)
#	    plot.GetYaxis().SetTitleSize(0.05)

#	    # Class-specific
#	    if   'TGraph'   in classname: plot.SetMarkerSize(0.2)
#	    elif 'TProfile' in classname: plot.SetMarkerSize(1.0)

    return

#----------------------------------------------------------------------
def draw_plot(canvas, plot, first=False):

    canvas.cd()
    if opts.logy:
        canvas.SetLogy()

    classname = plot.ClassName()
    plotname = plot.GetName()

    # Options
    goptlist = []
    if 'TGraph' in classname:
        if first:
            goptlist.append('A') # Draw axis

        goptlist.append('P') # Draw marker
        goptlist.append('Z') # No foot on errors
    elif 'TProfile' in classname:
        goptlist.append('P') # Draw marker

    if not first:
        goptlist.append('SAME')

    # Draw
    gopt = ' '.join(goptlist)
    if   'TGraph'   in classname: plot.Draw(gopt)
    elif 'TProfile' in classname: plot.DrawCopy(gopt)

    # Verbose
    if opts.verbose:
        print '%s.Draw("%s")' % (plotname, gopt)

#	    plot.SetLineWidth(0)
#	    plot.Draw('PZ SAME')

    canvas.Update()
    return


#----------------------------------------------------------------------
def create_profile_from_graph(pname, graph, nbin):

    # Binning
    npoints = graph.GetN()

    # Verbose
    if opts.verbose:
        print 'run, nbin = %d %d' %(run, nbin)

    # Instantiate
    profile = ROOT.TProfile(pname, '', nbin, xmin, xmax)

    # Important error option!
    profile.SetErrorOption('G')

    # Set title
    profile.SetTitle(';%s;%s' % (graph.GetXaxis().GetTitle(),
                                 graph.GetYaxis().GetTitle()))

    # Fill
    for k in xrange(graph.GetN()+1):
        x = ROOT.Double(0)
        y = ROOT.Double(0)
        graph.GetPoint(k, x, y)
        yerr = graph.GetErrorY(k)

        yfactor = 1.
        if '4J10' in graph.GetName() and 'x'==opts.yaxis[0] and not opts.logy:
            yfactor = 2.

        y /= yfactor
        yerr /= yfactor

        weight = 0
        if yerr > 0:
            weight = 1. / (yerr*yerr)

        profile.Fill(x, y, weight)

    # Debug
    if opts.debug:
        profile.Print('all')

    # Let me take ownership over ROOT
    ROOT.SetOwnership(profile, False)

    return profile

#----------------------------------------------------------------------
def create_legend(trignames):

    # Default
    xpos = 0.82
    ypos = 0.925
    dx   = 0.15
    dy   = 0.07

    # Left
    if 'r' == opts.yaxis[0]:
        xpos = 0.16

    # "Official" style
    if opts.bad_style:
        ypos -= 0.03

    # Minimum depends on no. of triggers
    yposmin = ypos - dy * len(trignames)
    if yposmin < 0:
        yposmin = 0

    # Maximum
    xposmax = xpos+dx
    if xposmax > 1:
        xposmax = 1

    # Instantiate
    legend = ROOT.TLegend(xpos, yposmin, xposmax, ypos)
    legend.SetFillStyle(0)
    legend.SetBorderSize(0)
    legend.SetFillColor(ROOT.kWhite)
    legend.SetTextSize(0.04)

    # Number of columns
    if 'x' == opts.yaxis[0]:
        legend.SetNColumns(1)

    # Not "official" style
    if not opts.bad_style:
        if 'test'     in opts.trig: legend.SetHeader('Test')
        if 'all'      in opts.trig: legend.SetHeader('Triggers')
        if 'single'   in opts.trig: legend.SetHeader('Single obj')
        if 'combined' in opts.trig: legend.SetHeader('Combined')
        if 'jets'     in opts.trig: legend.SetHeader('Jet, E^{T}_{miss}')

    return legend

#----------------------------------------------------------------------
def get_ynames(trignames, is_counts=False):

    # Counts list
    if is_counts:
        yaxis   = 't'  # =counts
        ysuffix = 'av' # =after veto
        cnames  = [ yaxis+ysuffix+'__'+trigname for trigname in trignames ]
        return cnames

    # Rates list -- manual over-ride
    yaxis   = 'r'               # 'r'=rates, 'x'=cross-section
    ysuffix = opts.yaxis[1:] # 'bp'=before ps, 'ap'=after ps, 'av'=after veto
    ynames  = [ yaxis+ysuffix+'__'+trigname for trigname in trignames ]

    if opts.verbose:
        print 'get_ynames -- Y names:\n\t%s' %('\n\t'.join(ynames))

    return ynames

#----------------------------------------------------------------------
def pass_grl(run, lb, y):

    if run==191426:
        if not (lb > 190): return False
        if not (lb < 1052): return False
        if not (y < 3.24): return False  # Quality control by L1_EM
        if not (y > 3.14): return False  # Quality control by L1_EM
    elif run==191628:
        if not (lb > 129): return False
        if not (lb < 256): return False
    elif run==190503:
        if not (lb > 819): return False
        if not (lb < 862): return False
        if not (y < 3.24): return False  # Quality control by L1_EM
        if not (y > 3.14): return False  # Quality control by L1_EM
    elif run==190504:
        if not ((lb>4 and lb<26) or (lb>140 and lb<151)): return False
        if not (y < 3.24): return False  # Quality control by L1_EM
        if not (y > 3.14): return False  # Quality control by L1_EM
    elif run==190505:
        if not (lb >4): return False
        if not (lb <26): return False

    return True

#----------------------------------------------------------------------
def pass_quality(run, lblength, yarray, x, x_raw):

    # Positive lumi only
    if not (x > 0): return False

    # Within 5 of 60 seconds
    if not (lblength < 65): return False
    if not (lblength > 55): return False

    # Suppress low rates for every trigger
    for y in yarray:
        if 'x' == opts.yaxis[0]:
            if not (y/x_raw > 0.05): return False
        else:
            if not (y > 0.05): return False

    # For cross-section
    if 'x' == opts.yaxis[0]:
        if not (x > 0.01): return False

        # Cut <mu> by hand
        if run==191628 and not (x > 28.5): return False
        if run==191426 and not (x > 8.0): return False

    return True

#----------------------------------------------------------------------
def get_header(runs):

    # Determine which run in October, 2011
    datelist = []
    if 190503 in runs: datelist.append('6') # in October
    if 190504 in runs: datelist.append('7') # in October
    if 190505 in runs: datelist.append('7') # in October
    if 191426 in runs: datelist.append('22') # in October
    if 191628 in runs: datelist.append('25') # in October

    # Remove duplicates, sort
    datelist = dict.fromkeys(datelist).keys()
    datelist.sort(lambda x, y: cmp(int(x), int(y))) # numeric sort

    # Comma-separate except the last element
    datestr = ''
    if len(datelist) > 0: datestr += datelist[0]
    if len(datelist) > 2: datestr += ', ' + ', '.join(datelist[1:-1])
    if len(datelist) > 1: datestr += ' & ' + datelist[-1]

    # Prepend standard header
    headerlist = []
    headerlist.append('ATLAS')

#	    if opts.bad_style:
#	        headerlist.append('#font[72]{ATLAS}')
#	    else:
#	        headerlist.append('ATLAS')

    headerlist.append('Trigger Operations')

    # Full header
    if len(datestr)>0:
        header = '(Oct. %s, 2011)' %datestr
        headerlist.append(header)

    header = ' '.join(headerlist)

    # Determine x-position
    xpos = 0.15
    if 'x' == opts.yaxis[0]:
        xpos = 0.05

#	    xblock = int((len(header)-40)/4)
#	    if xblock > 4: xblock = 4
#	    if xblock > 0: xpos -= 0.05*xblock
#	    if opts.debug:
#	        print len(header), xblock, xpos

    return xpos, header

#----------------------------------------------------------------------
def create_text(tname = 'text'):
    text = ROOT.TLatex()
    text.SetName(tname)
    text.SetTextSize(0.04)
    return text

#----------------------------------------------------------------------
def create_canvas(cname = 'canvas'):

    # Settings
    color = ROOT.kWhite
    ww = 500
    wh = 500
    lgap = 60
    rgap = 40
    bgap = 60
    tgap = 40

    # Widen for legends & taller for train info
    if 'x' == opts.yaxis[0]:
        ww   += 160
        rgap += 160
        wh   += 80
        tgap += 80
    else:
        ww   += 80
        wh   += 80

    canvas = ROOT.TCanvas(cname, cname, ww, wh)
    canvas.SetMargin(lgap/float(ww), rgap/float(ww), bgap/float(wh), tgap/float(wh))
    canvas.SetFillColor(color)

    if opts.bad_style: # Is "official" style
        canvas.SetTickx()
        canvas.SetTicky()

    canvas.cd()
    return canvas

#----------------------------------------------------------------------
def create_function(trigname):
    ftype = 'pol1'
    if 'x' == opts.yaxis:
        if   '2TAU' in trigname: ftype = 'pol1'
        elif 'EM'   in trigname: ftype = 'pol1'
        elif '2MU'  in trigname: ftype = 'pol1'
        elif 'MU'   in trigname: ftype = 'pol1'
        elif 'TAU'  in trigname: ftype = 'pol1'
        elif 'J75'  in trigname: ftype = 'pol1'
        elif '4J'   in trigname: ftype = 'expo'
        elif 'XE'   in trigname: ftype = 'expo'
    else:
        if   '2TAU' in trigname: ftype = 'pol1'
        elif 'EM'   in trigname: ftype = 'pol1'
        elif '2MU'  in trigname: ftype = 'pol2'
        elif 'MU'   in trigname: ftype = 'pol1'
        elif 'TAU'  in trigname: ftype = 'pol1'
        elif 'J75'  in trigname: ftype = 'pol2'
        elif '3J'   in trigname: ftype = 'pol2'
        elif '4J'   in trigname: ftype = 'expo'
        elif 'XE'   in trigname: ftype = 'expo'

    function = ROOT.TF1('fn__'+trigname, ftype)
    function.SetParameter(0, 1)
    function.SetParameter(1, 0.5)
    function.SetLineColor( ROOT.kBlack )
    function.SetLineWidth( 1 )
    return function


#----------------------------------------------------------------------
def save_canvas(canvas):

    # Redraw axes
    canvas.cd()
    ROOT.gPad.RedrawAxis()
    canvas.Update()

    # Name
    cname = opts.yaxis + '__' + opts.trig
    print 'Saving', cname+'.eps'
    print 'Saving', cname+'.png'

    # Save
    canvas.SaveAs(cname+'.eps')
    canvas.SaveAs(cname+'.png')
    return


#======================================================================
if __name__ == '__main__':

    # Declare to persist after interactive plotting
    canvas = create_canvas()

    #
    # Command-line args -> TFiles, lists
    #

    # Input list
    runinputs = []
    if opts.run:
        runinputs = opts.run.split(',')
    else:
        # Default
        runinputs.append( '191426_cool:1332:200' )
        if 'x' == opts.yaxis[0]:
            runinputs.append( '191628_trp:10:40' ) # Add high-mu run for cross-section
            runinputs.append( '190503_cool:58:40' ) # Add 25ns
            runinputs.append( '190504_cool:58:40' ) # Add 25ns
            runinputs.append( '190505_cool:58:40' ) # Add 25ns

    tfiles   = []
    ttrees   = []
    runs     = []
    nbunches = []
    nbins    = []
    for runinput in runinputs:

        thisrun = thisdb = thisbunch = thisnbin = None

        # Parse "run:nbunches:nbins"
        tokens = runinput.split(':')
        if len(tokens) > 2: thisnbin  = tokens[2]
        if len(tokens) > 1: thisbunch = tokens[1]
        if len(tokens) > 0:
            tokenettos = tokens[0].split('_')
            if len(tokenettos) > 1: thisdb  = tokenettos[1]
            if len(tokenettos) > 0: thisrun = tokenettos[0]

        # Open file
        fname = '%s_%s.root' % (thisrun, thisdb)
        try:
            tfile = ROOT.TFile(fname)
            print '__main__ Opened %s\twith nbunches, nbins = %s, %s' %(fname, thisbunch, thisnbin)
        except:
            print '__main__ ERROR: Cannot open TFile for', (fname, thisrun, thisbunch, thisnbin)
            break

        # Get tree
        tfiles  .append( tfile )
        ttrees  .append( tfile.Get('ntp1')  )
        nbunches.append( int(thisbunch)     )
        runs    .append( int(thisrun)       )
        nbins   .append( int(thisnbin)      )

    # Info
    print '__main__ -- INFO: Plotting "%s"\n\t%s' % (opts.trig, '\n\t'.join(get_triglist(opts.trig)))

    #
    # Extract data: TTree -> databank (transient) -> graphbank
    #

    # Repository
    graphbank    = []
    profilebank  = []
    functionbank = []

    # Get trigger list
    trignames = get_triglist(opts.trig)

    # Legend
    legend = create_legend(trignames)

    # Loop over trees: retrieve data once for trigger list
    nplots = 0
    for i, ttree in enumerate(ttrees):

        run    = runs[i]
        nbunch = nbunches[i]
        nbin   = nbins[i]

        # Retrieve transient data: TTree -> data
        treename = ttree.GetCurrentFile().GetName()
        databank = get_data_from_tree(ttree, run, nbunch, trignames, xname)

        # Loop over triggers: data -> graph
        for j, trigname in enumerate(trignames):

            nplots += 1

            # The trigname must come at the end!
            basename = '__'.join([str(i), str(j), trigname])

            # Convert data -> graph -> profile
            datazip = databank[trigname]
            graph   = create_graph_from_datazip('graph__'+basename, xname, trigname, datazip)
            profile = create_profile_from_graph('profile__'+basename, graph, nbin)

            # Format: set colors, margins, etc.
            format_plot(graph,   i, j)
            format_plot(profile, i, j)

            # Save in bank
            graphbank  .append( graph )
            profilebank.append( profile )

            # Verbose
            if opts.verbose:
                print '__main__ -- Saving', basename

            # Keep a list for legend
            if i==0:
                plot = graph
                if 'prof' in opts.plot:
                    plot = profile

                # rbp__L1_2TAU8_EM10VH -> L1_2TAU8_EM10VH
                tokens = plot.GetName().split('__')[-1]

                # L1_2TAU8_EM10VH -> 2TAU8_EM10VH
                lname = '_'.join(tokens.split('_')[1:])

                # Special
                if '4J10' in graph.GetName() and 'x'==opts.yaxis[0] and not opts.logy:
                    lname += ' (#divide2)'

                # Special
                if 'x'==opts.yaxis[0] and '2TAU8_EM10VH' in lname:
                    legend.AddEntry( plot, '2TAU8', 'F' )
                    legend.AddEntry( '', '_EM10VH', '' )
                else:
                    legend.AddEntry( plot, lname, 'F' )

            # Fit
            function = None
            if i==0 and 'r'==opts.yaxis[0]:
                function = create_function(trigname)
                profile.Fit(function, 'RS', '', 1.3, 3.5)
            elif i==1 and 'x'==opts.yaxis[0]:
                if 'XE' in trigname or '4J10' in trigname and not opts.bad_style:
                    function = create_function(trigname)
                    profile.Fit(function, 'RS', '', 27, 33)

            functionbank.append( function )

    #
    # Draw plot
    #

    # Loop over graphs
    for i in xrange(nplots):

        # First in the list?
        is_first = i==0

        # Draw plot -> canvas
        if opts.plot in 'graph':
            graph = graphbank[i]
            draw_plot(canvas, graph, is_first)

        elif opts.plot in 'profile':
            profile = profilebank[i]
            draw_plot(canvas, profile, is_first)

        elif opts.plot in 'both':
            graph = graphbank[i]
            profile = profilebank[i]
            draw_plot(canvas, graph, is_first)
            profile.SetMarkerColor(contrasting_color(graph))
            draw_plot(canvas, profile)

        if opts.debug:
            print 'Plotting', graphbank[i].GetName()

    #
    # Draw misc.
    #

    # Another y-axis
    newaxis = None
    if 'x' == opts.yaxis[0]:
        plot = profilebank[-1]
        ndiv = plot.GetYaxis().GetNdivisions()
        labeloffset = plot.GetYaxis().GetLabelOffset()
        titleoffset = plot.GetYaxis().GetTitleOffset()
        xsec_to_rate = 10 # Conversion from ub -> kHz for 10^34
        chopt = '+L'
        titleoffset *= 1.2
        if not opts.bad_style:
            chopt = '-SL'
            labeloffset *= -1
            titleoffset *= -1
        if opts.logy:
            chopt += 'G'

        newaxis = ROOT.TGaxis(1.0*xmax, ymin, 1.0*xmax, ymax, ymin*xsec_to_rate, ymax*xsec_to_rate, ndiv, chopt)
        newaxis.SetLabelOffset(labeloffset)
        newaxis.SetTitleOffset(titleoffset)
        newaxis.SetTickSize(0.02)
        newaxis.SetTitle("L1 rate (kHz) at L = 10^{34}cm^{-2}s^{-1}");
        newaxis.Draw('SAME')
        canvas.Update()

    # Title
    text = create_text()
    xpos, header = get_header(runs)
    if opts.bad_style and 'r'==opts.yaxis:
        legend.SetHeader(header)
    else:
        ypos = 0.95
        text.DrawTextNDC(xpos, ypos, header)

    # Legend
    legend.Draw()
    canvas.Update()

    canvas.Update()

    # Additional for high-mu data
    if 'x'==opts.yaxis[0]:
        ypos = 0.86
        dypos = 0.05

        # For 25ns run: 190503 190504 190505
        text.DrawTextNDC(0.03, ypos+0.5*dypos, "10 trs, 25ns")
        text.DrawTextNDC(0.03, ypos-0.5*dypos, '58 bunches')

        # For normal 50ns run: 191426
        text.DrawTextNDC(0.24, ypos+0.5*dypos, '12 trains, 50ns')
        text.DrawTextNDC(0.24, ypos-0.5*dypos, '1332 bunches')

        # For high-mu runs: 191628
        text.DrawTextNDC(0.54, ypos+0.5*dypos, 'no trains')
        text.DrawTextNDC(0.54, ypos-0.5*dypos, '10 bunches')

        # Draw lines
        line = ROOT.TLine()
        line.SetLineStyle(ROOT.kDotted)
        if opts.logy:
#	            ypos = math.pow(10, math.log10(ypos*ymax) + dypos)
            ypos *= 4*ymax
        else:
            ypos = ypos*ymax + dypos*ymax

        xpos = 7.5 ; line.DrawLine(xpos, 0, xpos, ypos)
        xpos = 25  ; line.DrawLine(xpos, 0, xpos, ypos)

#	        ypos = 1.1*ymax
#	        dypos = 0.07*ymax
#	        # For 25ns run: 190503 190504 190505
#	        text.DrawLatex(-3, ypos+0.5*dypos, "10 trs, 25ns")
#	        text.DrawLatex(-3, ypos-0.5*dypos, '58 bunches')
#	
#	        # For normal 50ns run: 191426
#	        text.DrawLatex(9, ypos+0.5*dypos, '12 trains, 50ns')
#	        text.DrawLatex(9, ypos-0.5*dypos, '1332 bunches')
#	
#	        # For high-mu runs: 191628
#	        text.DrawLatex(26, ypos+0.5*dypos, 'no trains')
#	        text.DrawLatex(26, ypos-0.5*dypos, '10 bunches')
#	
#	        # Draw lines
#	        line = ROOT.TLine()
#	        line.SetLineStyle(ROOT.kDotted)
#	        xpos = 7.5 ; line.DrawLine(xpos, 0, xpos, ypos+1.0*dypos)
#	        xpos = 25  ; line.DrawLine(xpos, 0, xpos, ypos+1.0*dypos)

    # Save
    save_canvas(canvas)

# That's all!

#	    # Sort graph by yval at xmax
#	    counter = -1
#	    unsortedgraphs = []
#	    unsortedy = []
#	    addlegend = []
#	    for i, tfile in enumerate(tfiles):
#	        for j, yname in enumerate(ynames):
#	            counter += 1
#	            graph = graphs[counter]
#	            yvalmax = 0
#	            xvalmax = 0
#	            for k in xrange(graph.GetN()+1):
#	                xval = ROOT.Double(0)
#	                yval = ROOT.Double(0)
#	                graph.GetPoint(k, xval, yval)
#	                if yval > yvalmax:
#	                    yvalmax = yval
#	                    xvalmax = xval
#
#	            unsortedy.append(yvalmax)
#	            unsortedgraphs.append(graph)
#	            if i==0:
#	                addlegend.append( counter )
#	            else:
#	                addlegend.append( -1 )
#
#	    sorteddata = zip(unsortedy, unsortedgraphs, addlegend)
#	    sorteddata.sort()
#	    sorteddata.reverse()
#	    sortedy, sortedgraphs, addlegend = map(list, zip(*sorteddata))
#	    #	sortedgraphs = graphs
#
#	    # Draw graph
#	    counter = -1
#	    for counter, graph in enumerate(sortedgraphs):
#	        graph = graphs[counter]
#	        graph.GetXaxis().SetLimits(0, xmax)
#	        graph.GetYaxis().SetRangeUser(0, ymax)
#
#	        color = colors[counter % len(colors)]
#	        graph.SetMarkerColor(color)
#	        graph.SetFillColor  (color)
#	        graph.SetLineColor  (color)
#
#	        # Customize
#	        gopt = 'PZ, SAME'
#	        if counter==0:
#	            gopt = 'APZ'
#
#	        graph.Draw(gopt)
#	        canvas.Update()
#
#	    # Put text in legend
#	    for counter in xrange(len(sortedgraphs)):
#	        idx = addlegend[counter]
#	        if idx < 0:
#	            continue
#
#	        graph = sortedgraphs[idx]
#	        lname = graph.GetName()
#	        if 'xbp' == opts.yaxis:
#	            if '4J' in lname:
#	                lname += ' (#divide2)'
#
#	        legend.AddEntry( graph.GetName(), lname, 'F')
#
