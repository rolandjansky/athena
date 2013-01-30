#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQuerySummary.py
# Project: AtlRunQuery
# Purpose: Library with the Summary maker
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Oct 6, 2009
# ----------------------------------------------------------------
#
import sys, time
from array import array                
from CoolRunQuery.utils.AtlRunQueryUtils  import prettyNumber, durationInSeconds, filesize, importroot
from CoolRunQuery.utils.AtlRunQueryLookup import DQChannelDict, DQSuperGroupsDict, DQGroupDict, DQChannels
from CoolRunQuery.output.AtlRunQueryRoot   import SetStyle, MakeHtml
from CoolRunQuery.selector.AtlRunQuerySelectorBase       import DataKey
importroot()
from ROOT import TCanvas, gPad, gStyle, TGraph, TColor, TGaxis, gROOT, TH1F, TLegend, TText, TLine, Double

# custom ROOT colours
c_White         = TColor.GetColor( "#ffffff" )
c_NovelBlue     = TColor.GetColor( "#2244a5" )
c_DarkOrange    = TColor.GetColor( "#ff6600" )
c_NovelRed      = TColor.GetColor( "#dd0033" )
c_DarkGreen     = TColor.GetColor( "#116600" )
c_Violet        = TColor.GetColor( "#aa11aa" )
c_LightBlue     = TColor.GetColor( "#66aaff" )
c_VDarkYellow   = TColor.GetColor( "#ffa500" )
c_Gray          = TColor.GetColor( "#888888" )
c_VLightGray    = TColor.GetColor( "#eeeeee" )
c_VDarkGray     = TColor.GetColor( "#333333" )

def MakeSummaryTableHtml( cornerstr, colstr, rowkeys, table, tablewidth ):

    # dimensions
    ncol = len(colstr)
    nrow = len(rowkeys)
    
    s = '<table class="resulttable" width="%i" style="margin-left: 14px">\n' % tablewidth
    # headline    
    s += '<tr>\n'
    if cornerstr != '': s += '   <th style="text-align:left">%s</th>  ' % cornerstr
    else:               s += '   <th class="emptycorner">&minus;</th>'
    for ic in range(0,ncol):
        s += '<th>%s</th>' % colstr[ic]
    s += '</tr>\n'
    for ir in range(0,nrow):
        if ir%2 == 0: color = 1
        else:         color = 2
        if  'debug_' in rowkeys[ir][1]: s += '<tr class="outRed%s">\n' % color   # special colours for debug stream
        else:                           s += '<tr class="out%s">\n' % color
        s += '   <th class="rowsum" style="text-align:left">%s&nbsp;</th>  ' % rowkeys[ir][1]
        for ic in range(ncol):
            if ic in rowkeys[ir][4]: # exclude from printing
                s += '<td style="text-align:center">&minus;</td'
                continue
            s += '<td style="text-align:right">'
            if isinstance(table[ir][ic],str):
                s += "%s" % table[ir][ic] # example filesize
            elif rowkeys[ir][3] == 'pretty-int':
                s += "%s" % prettyNumber(int(table[ir][ic])) # example '#Event'
            else:
                s += (rowkeys[ir][3] % table[ir][ic]) # format
            s += '%s</td>' % rowkeys[ir][2] # unit

        s += '</tr>\n'
    s += '</table>'

    return s

def MakeDQSummaryTableHtml( cornerstr, title, colstr, colchans, rowstr, rowstrNames, dic, nruns, labeloffset ):

    if nruns <= 0: return ''    

    # dimensions
    ncol = len(colstr) # flag types
    nrow = len(rowstr) # DQ channels

    # overlay pictures
    dw_call = '<script type="text/javascript">\n'
    dw_call += """if(dw_Tooltip.content_vars==undefined) {dw_Tooltip.content_vars = {}; };\n"""
    for ir in range(0,nrow):
        for ic in range(0,ncol):
            if dic[colchans[ic]][rowstr[ir]][0] > 0:
                var = 'var_%i_%i' % (ir,ic+labeloffset)
                dw_call += 'dw_Tooltip.content_vars["%s"] = {' % var
                dw_call += "content: '<strong><b>Corresponding&nbsp;run(s)&nbsp;[#&nbsp;=&nbsp;%i]:</b></strong><br> " % dic[colchans[ic]][rowstr[ir]][0]
                icc = 0
                for r in dic[colchans[ic]][rowstr[ir]][1]:
                    dw_call += '%s, ' % r
                    icc += 1
                    if icc%8 == 0: dw_call += '<br>'
                if dw_call[-1-3:] == '<br>': dw_call = dw_call[0:-1-3] # remove ', <br>'
                else:                        dw_call = dw_call[0:-1-1] # remove ', '
                dw_call += """ ' };\n"""
    dw_call += '</script>\n\n'

    s = '<table class="resulttable" width="auto" style="margin-left: 14px">\n'
    # headline    
    s += '<tr>\n'
    if cornerstr != '': s += '   <th class="rowsum" style="text-align:left" rowspan="2">%s</th>  ' % cornerstr
    else:               s += '   <th class="emptycorner" rowspan="2">&minus;</th>'
    s += '<th class="colsumL" colspan="%i">%s</th></tr><tr>' % (ncol, title)
    for ic in range(0,ncol):
        s += '<th class="colsum">%s</th>' % colstr[ic]
    s += '</tr>\n'
    for ir in range(0,nrow):
        if ir%2 == 0: color = 1
        else:         color = 2
        s += '<tr class="out%s">\n' % color
        s += '   <th class="rowsum" style="text-align:left">%s</th>  ' % rowstrNames[ir]
        for ic in range(0,ncol):            
            dqk = rowstr[ir]
            dqk_tdtype = dqk
            if dqk == 'n.a.': dqk_tdtype = 'NA'
            s += '<td class="td%s" style="text-align:center">' % dqk_tdtype
            if dic[colchans[ic]][rowstr[ir]][0] > 0: s += '<div class="showTip var_%i_%i" style="display:inline;cursor:pointer">' % (ir,ic+labeloffset)
            if dic[colchans[ic]][dqk][0]>0: s += '%i' % (dic[colchans[ic]][dqk][0])
            if dic[colchans[ic]][rowstr[ir]][0] > 0: s += '</div>'
            s += '</td>'

        s += '</tr>\n'
    s += '</table>\n\n'
    s += dw_call
    s += '\n'

    return s

def ComputeStats( vlist ):
    if len(vlist) == 0: return [0,0,0,0,0]
    tot = 0
    mn = +sys.maxint
    mx = -sys.maxint
    for v in vlist:
        tot += v
        mn = min(mn,v)
        mx = max(mx,v)
    mean = float(tot)/len(vlist)
    return [tot,mean,mn,mx,len(vlist)]

def DecodeStr( s, tpe ):
    if 'Duration' == tpe:
        return durationInSeconds(s)/float(3600)
    elif 'STR:physics_*' == tpe:
        s = s[0].value
        if len(s) <= 1:
            print 'Big troubles... in function "AtlRunQuerySummary::DecodeStr": invalid length for "%s"' % tpe
            sys.exit(1)
        if s[0] <= 0: return 0
        else:         return float(s[1])/s[0]/1.0e6

    # otherwise
    return int(s[0].value)

def DrawGraph( graph, col, width, style, legend, title,
               drawNumber=False, unit='M', text=TText() ):
    hist = graph.GetHistogram()
    hist.SetLineColor( col )
    graph.SetLineColor( col )
    hist.SetLineWidth( width )
    graph.SetLineWidth( width )
    hist.SetLineStyle( style )
    graph.SetLineStyle( style )
    graph.Draw("lsame")
    legend.AddEntry( hist, title, "L" )

    # draw max number
    if drawNumber:
        N = graph.GetN()
        minunit_ = Double(0)
        maxunit_ = Double(0)
        minnev_  = Double(0)
        maxnev_  = Double(0)
        x        = Double(0)
        y        = Double(0)
        graph.GetPoint( 0,   minunit_, minnev_ )
        graph.GetPoint( N-1, maxunit_, maxnev_ )
        dist    = maxunit_ - minunit_
        graph.GetPoint( N-1,x,y )

        text.SetTextAlign( 12 )
        text.SetTextSize( 0.030 )
        text.SetTextColor( c_NovelBlue )
        if y >= 100: yt = '%.0f'  % y
        else:        yt = '%#.3g' % y
        text.DrawText( x+0.015*dist, y, '%s %s' % (yt, unit) )

def SetFrameStyle( frame, scale = 1.0 ):

    c_VLightGray = TColor.GetColor( "#eeeeee" )
    c_VDarkGray  = TColor.GetColor( "#333333" )

    frame.SetLabelOffset( 0.012, "X" )
    frame.SetLabelOffset( 0.012, "Y" )
    frame.GetXaxis().SetTitleOffset( 1.25 )
    frame.GetYaxis().SetTitleOffset( 0.94 )
    frame.GetXaxis().SetTitleSize( 0.045*scale )
    frame.GetYaxis().SetTitleSize( 0.045*scale )
    labelSize = 0.04*scale
    frame.GetXaxis().SetLabelSize( labelSize )
    frame.GetYaxis().SetLabelSize( labelSize )
    gPad.SetTicks()
    gPad.SetBottomMargin( 0.120*scale  )
    gStyle.SetTitleFillColor( c_VLightGray )
    gStyle.SetTitleTextColor( c_VDarkGray )
    gStyle.SetTitleBorderSize( 1 )
    gStyle.SetTitleH( 0.06 )
    gStyle.SetTitleX( gPad.GetLeftMargin() )
    gStyle.SetTitleY( 1 - gPad.GetTopMargin() + gStyle.GetTitleH() )
    gStyle.SetTitleW( 1 - gPad.GetLeftMargin() - gPad.GetRightMargin() )

def CreateCanvasAndFrame( runmin, runmax, tminS, tmaxS, maxnev, type, unit ):
    # plots the graphs
    title = "ATLAS integrated numnber of events in run range %i - %i" % (runmin, runmax)
    c = TCanvas( "c", title, 0, 0, 800, 550 )
    c.GetPad(0).SetRightMargin(0.08)
    c.GetPad(0).SetLeftMargin(0.10)
    c.GetPad(0).SetTopMargin(0.07)
    htitle = "Events recorded by ATLAS between %s and %s" % (time.strftime('%b %d',tminS),
                                                             time.strftime('%b %d, %Y',tmaxS))

    frame = TH1F( "frame", htitle.strip(), 10000, runmin, runmax )

    SetFrameStyle( frame )
    TGaxis.SetMaxDigits(6)
    frame.GetYaxis().SetTitleOffset( 1.0 )
    frame.SetMinimum( 0 )
    frame.SetMaximum( maxnev*1.1 )
    frame.SetYTitle( "Number of events (in %s)" % unit )
    frame.SetXTitle( "Run number"  )
    frame.Draw()

    # legend for plot
    dx = 0.04
    dy = -0.05
    legend = TLegend( c.GetLeftMargin() + dx, 1 - c.GetTopMargin() + dy,
                      c.GetLeftMargin() + 0.48 + dx, 1 - c.GetTopMargin() - 0.38 + dy)
    legend.SetFillStyle( 1001 )
    legend.SetFillColor( c_VLightGray )
    legend.SetBorderSize(1)
    legend.SetMargin( 0.2 )
    legend.SetTextColor( c_VDarkGray )
    legend.Draw("same")

    return c, frame, legend

def MakeRootPlots( rootstreamdic, datapath ):

    # suppress output
    gROOT.SetBatch( 1 )

    # common style, and a few corrections
    SetStyle()

    colours = [ c_NovelRed, 4, c_DarkOrange, c_DarkGreen, c_Violet, c_LightBlue, 3, c_VDarkYellow, c_Gray,
                1, 2, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28 ]
    
    # run ranges (note: reverse order!)
    run_and_time = sorted(rootstreamdic[DataKey('Start and endtime')])
    runs = [x[0] for x in run_and_time]
    runmin = min(runs)
    runmax = max(runs)

    tmin = min([x[1][0] for x in run_and_time])
    tmax = max([x[1][1] for x in run_and_time])

    # time structs
    tminS  = time.gmtime(tmin)
    tmaxS  = time.gmtime(tmax)    

    # html references
    htmlstr = []
    
    # create graphs
    graphs = []
    sortdic = {}
    maxnev = { 'all_':-1, 'debug_':-1,'calibration_':-1 }
    for key, content in rootstreamdic.iteritems():
        if type(key)==DataKey: key = key.ResultKey
        if not 'Start and endtime' in key:
            graphs.append( TGraph( len(content) ) )
            graphs[-1].SetName( key.strip().replace(' ','_') )
            graphs[-1].SetTitle( key.strip() )
            cumulsum = 0
            for i in range(len(content)):
                j = len(content)-1-i # reverse order
                scale = 1.e6
                if 'debug_' in key: scale = 1.e3
                cumulsum += content[j][1]/scale
                graphs[-1].SetPoint( i, content[j][0], cumulsum ) # number of events in million
            for stype in maxnev.keys():
                if stype in key:
                    if cumulsum > maxnev[stype]: maxnev[stype] = cumulsum 
            if not 'debug_' in key and cumulsum > maxnev['all_']: maxnev['all_'] = cumulsum
            sortdic[len(graphs)-1] = cumulsum

    # sort dictionary into list
    sortedlist = sorted(sortdic, key=sortdic.__getitem__, reverse=True)

    # individual physics streams
    ic = 0
    for i in sortedlist:
        g = graphs[i]
        name = g.GetTitle()
        if 'physics_' in name or 'express_' in name:
            x = Double(0)
            y = Double(0)
            g.GetPoint( g.GetN()-1, x, y )
            c, frame, legend = CreateCanvasAndFrame( runmin, runmax, tminS, tmaxS, y*1.1, '', 'million' )
            legend.SetX2( legend.GetX2() - 0.2 );
            legend.SetY1( legend.GetY2() - 0.05 );
            DrawGraph( g, colours[ic], 2, 1, legend, name.replace('physics_','').replace('express_','').replace('express','Express'), True, 'M', TText() )
            ic += 1

            # redraw axes and update
            frame.Draw("sameaxis")   
            c.Update()
            
            # make plot
            fname = '%s/atlrunquery_%s.png' % (datapath, name)
            c.Print( fname )
            del frame
            
            # reference in html
            htmlstr.append( MakeHtml( ['Events in streams %s versus run' % name], [fname], True, 60, 5 ) )

    # physics streams only ------------------------------------------------------------
    c, frame, legend = CreateCanvasAndFrame( runmin, runmax, tminS, tmaxS, maxnev['all_'], '', 'million' )
    ic = 0
    for i in sortedlist:
        g = graphs[i]
        name = g.GetTitle()
        if '#Events (streamed)' in name: 
            DrawGraph( g, c_NovelBlue, 2, 2, legend, "Total number of streamed events" )
        elif '#Events' == name:            
            DrawGraph( g, c_NovelBlue, 5, 1, legend, "Total number of triggered events (excl. calib)", True, 'M', TText() )
        elif 'physics_' in name or 'express_' in name:
            DrawGraph( g, colours[ic], 2, 1, legend, name.replace('physics_','').replace('express_','').replace('express','Express') )
            ic += 1

    # redraw axes and update canvas
    frame.Draw("sameaxis")   
    c.Update()

    # make plot
    fname = '%s/atlrunquery_physicsstreams.png' % datapath
    c.Print( fname )
    del frame

    # reference in html
    htmlstr.append( MakeHtml( ['Events in physics streams versus run'], [fname], True, 60, 5 ) )

    # the other types can be 
    #             stream_type  nice text  unit
    plottypes = { 'debug_' :       [ 'Debug',        'thousand', 'k' ],
                  'calibration_' : [ 'Calibration', 'million',  'M' ] }

    # debug and calibration streams ----------------------------------------------------
    for key, cprop in plottypes.iteritems():
                 
        c, frame, legend = CreateCanvasAndFrame( runmin, runmax, tminS, tmaxS, maxnev[key], cprop[0], cprop[1] )
        legend.SetY1( legend.GetY1() + 0.15 );
        legend.SetX2( legend.GetX2() - 0.15 );
        ic = 0
        for i in sortedlist:
            g = graphs[i]
            name = g.GetTitle()
            if key in name:
                DrawGraph( g, colours[ic], 2, 1, legend, name, ic == 0, cprop[2], TText() )            
                ic += 1

        # redraw axes and update canvas
        frame.Draw("sameaxis")   
        c.Update()

        # make plot
        fname = '%s/atlrunquery_%sstreams.png' % (datapath, cprop[0].lower())
        c.Print( fname )
        del frame
        
        # reference in html
        htmlstr.append( MakeHtml( ['Events in %s streams versus run' % cprop[0].lower()], [fname], True, 60, 5 ) )

    # html code ---------------------------------------------------------------------------
    retstr    = '<table width="900" style="margin-left: 14px; background-color: #f0f0f0; border: 2px white solid; border-collapse: collapse;"><tr>'
    for i,s in enumerate(htmlstr):
        retstr += '<td style="padding: 4px">%s</td>' % s
        if (i+1)%5 == 0: retstr += '</tr><tr>'
    retstr += '</tr></table>'

    return retstr
            
def MakeSummaryHtml( dic, dicsum, datapath ):
    # run and time ranges
    runs   = dic[DataKey('Run')]
    nruns  = len(runs)
    times  = dic[DataKey('Start and endtime')]

    starttime = float(times[-1].partition(',')[0])
    endtime   = float(times[0].partition(',')[2])

    s = '<table style="color: #777777; font-size: 85%; margin-left: 14px" cellpadding="0" cellspacing="3">\n'
    # runs in reverse order
    s += '<tr><td><i>Number of runs selected:</i></td><td>&nbsp;&nbsp;%g</td><td></td></tr>\n' % (len(runs))
    s += '<tr><td><i>First run selected:</i></td><td>&nbsp;&nbsp;%s</td><td>&nbsp;&nbsp;(%s)</td></tr>\n' % (runs[-1], time.strftime("%a %b %d, %Y, %X",time.gmtime(starttime)))
    s += '<tr><td><i>Last run selected:</i></td><td>&nbsp;&nbsp;%s</td><td>&nbsp;&nbsp;(%s)</td></tr>\n'  % (runs[0], time.strftime("%a %b %d, %Y, %X",time.gmtime(endtime)))
    s += '</table>\n'
    
    # --------------------------------------------------------------------------------------
    # run summary table    
    s += '<p></p>\n'
    s += '<table style="margin-left: 14px">\n'
    s += '<tr><td><b>Run / Event Summary</b></td></tr>\n'
    s += '</table>\n'

    table  = []
    # format     Title in dico    Title in table     unit   format        which columns in table to exclude
    keys   = [ [ 'Duration',           'Run duration',    ' h',  '%.2f',       [] ],
               [ '#Events',            '#Events (excl.)', '',    'pretty-int', [] ],
               [ '#Events (streamed)', '#Events (incl.)', '',    'pretty-int', [] ],
               [ 'STR:physics_*',      'Event size <font size="-2">(RAW)</font>',' MB', '%.2f', [0] ]     # with wildcard
               ]
    for k in keys:
        v = []
        for dk, c in dic.iteritems():
            # exact check?
            Found = False
            if ('*' in k[0] and k[0].replace('*','') in dk.ResultKey) or (not '*' in k[0] and k[0] == dk.ResultKey):            
                for x in c:
                    try:
                        v.append( DecodeStr( x, k[0] ) )
                    except ValueError: # if n.a.
                        pass
        # sanity check

        ### what if just n.a. in one column
        ###if len(v) == 0:
        ###    print 'Big troubles... in function "AtlRunQuerySummary::MakeSummaryHtml": did not find key "%s" in dictionary' % k[0]
        ###    sys.exit(1)
        table.append( ComputeStats( v ) )
                
    s += MakeSummaryTableHtml( '', ['Total', 'Average', 'Min', 'Max'], keys, table, 500 )

    # --------------------------------------------------------------------------------------
    # stream summary table    
    s += '<p></p>\n'
    s += '<table style="margin-left: 14px">\n'
    s += '<tr><td><b>Streams / Events Summary</b> <font size=-2> [ Straight averages given, not weighted by number of events in run ]</font></td></tr>\n'
    s += '</table>\n'

    streamdic     = {}    
    streamsizedic = {}    
    streamfracdic = {}    
    streamratedic = {}    
    rootstreamdic = {}    
    v = []    
    for dk, c in dic.iteritems():
        # for root
        if '#Events' in dk.ResultKey:
            rootstreamdic[dk] = []
            for run,dataentries in zip(runs,c):
            #for i in range(len(c)):
                run = int(run)
                data = dataentries[0].value  # for events and time info there is only one entry in the list per run
                try:
                    rootstreamdic[dk].append([run, int(data)])
                except ValueError:
                    if data != 'n.a.': rootstreamdic[dk].append([run, data])
            
        if 'Start and endtime' in dk.ResultKey:
            rootstreamdic[dk] = []
            for run,data in zip(runs,c):
                if data != 'n.a.': rootstreamdic[dk].append([int(run), map(float,data.split(','))])
            
        # collect streams
        if 'STR:' in dk.ResultKey:
            # loop over runs
            sname = dk.ResultKey.replace('STR:','')
            streamdic[sname]     = []
            streamsizedic[sname] = 0
            streamfracdic[sname] = []
            streamratedic[sname] = []
            rootstreamdic[sname] = []
            #for i in range(len(c)):
            for run,t,dataentries in zip(runs,times,c):
                run = int(run)
                data = dataentries[0].value  # for stream data there is only one entry in the list per run
                if not isinstance(data, str): # not 'n.a.'
                    streamdic[sname].append(data[0])
                    streamfracdic[sname].append(data[0])
                    tmin_   = float(t.partition(',')[0])
                    tmax_   = float(t.partition(',')[2])
                    deltat = tmax_ - tmin_
                    if (deltat > 0): streamratedic[sname].append(data[0]/deltat)
                    else           : streamratedic[sname].append(-1)
                    streamsizedic[sname] += data[1]
                    try:
                        rootstreamdic[sname].append([run, data[0]])
                    except ValueError:
                        pass
                else: streamfracdic[sname].append('na')

    # sort (physics, debug, calibration)
    streamtypes = ['physics', 'express', 'debug', 'calibration']
    sortedstreams = []
    for stype in streamtypes:
        keys = {}
        for key, c in streamdic.iteritems():
            if stype in key: keys[key] = sum(c)    
        sortedstreams += sorted(keys, key=keys.__getitem__, reverse=True)
    
    # fill table
    keys = []
    table  = []
    newstreamfracdic = {}
    for stream in sortedstreams:        
        stats = ComputeStats( streamdic[stream] )
        table.append( stats[0:4] )

        # compute event fractions (not for calibration streams)
        if not 'calibration_' in stream: 
            v = []
            for i, evrun in enumerate(streamfracdic[stream]):
                if evrun != 'na':
                    evsum = float(0)
                    for stream2 in sortedstreams:
                        if not 'calibration_' in stream2:
                            if streamfracdic[stream2][i] != 'na': evsum += streamfracdic[stream2][i]
                    if evsum > 0: v.append( float(evrun)/evsum*100 ) # fraction in percent

            table[-1] += ['%.3g' % ComputeStats( v )[1]]
        else:
            table[-1] += ['&minus']

        # average rate
        table[-1] += ['%.3g' % ComputeStats( streamratedic[stream] )[1]]

        # put here #runs where the stream was produced
        table[-1] += [stats[4]]

        # event sizes
        if table[-1][0] > 0: table[-1] += [filesize(streamsizedic[stream]),
                                           filesize(float(streamsizedic[stream])/table[-1][0])] 
        else:                table[-1] += [filesize(streamsizedic[stream]),'&minus']

        prettystr = stream
        if 'debug' in stream: prettystr = '<font color="#C11B17">' + stream + '</font>'
        # format     Title in dico    Title in table     unit   format        which columns in table to exclude
        keys.append( [stream,         prettystr,         '',    'pretty-int', []] )

    s += MakeSummaryTableHtml( 'Stream',
                               ['Total', 'Average', 'Min', 'Max',
                                'Average<br>fraction&nbsp;(%)',
                                'Average<br>rate&nbsp;(Hz)',
                                '#Runs<br><font size="-2">(out&nbsp;of&nbsp;%i)</font>' % nruns,
                                'Total&nbsp;file<br>size&nbsp;<font size="-2">(RAW)</font>',
                                'Average&nbsp;event<br>size&nbsp;<font size="-2">(RAW)</font>'], keys, table, 900 )

    # make integrated events plots
    htmlstr = MakeRootPlots( rootstreamdic, datapath )
    s += htmlstr    

    return s

    # DQ summary not operational anymore since defects ?
    
    # --------------------------------------------------------------------------------------
    # DQ summary table
    s += '<p></p>\n'
    s += '<table style="margin-left: 14px">\n'
    s += '<tr><td><b>Data Quality Summary - SHIFTOFL (number of runs)</b> <font size=-2> [ No number signifies zero. Move mouse over number to see corresponding runs. ]</font></td></tr>\n'
    s += '</table>\n'

    dqflags      = ['G','Y','R','B','U','n.a.']
    dqflagsNames = ['Green', 'Yellow', 'Red', 'Black', 'Unknown', 'Not&nbsp;assigned&nbsp;']
    dqitems      = DQChannels()

    # DQ flags are grouped together according to their channel numbers
    # all groups with equal first two digits belong together

    UseGroups = False
    if UseGroups:
        ndq = {}
        for chan, dqgroup in DQGroupDict.items():
            ndq[chan] = {}
            for dqflag in dqflags:
                ndq[chan][dqflag] = 0

        for chan, dqchan in dqitems:
            if dic.has_key( dqchan ):
                grchan = int(chan/10)*10
                if grchan == 430: grchan = 420 # 'ad hoc' correction for TAU trigger
                for dqflag in dic[dqchan]:
                    ndq[grchan][dqflag] += 1

        colstr = []
        colchans = []
        for chan, dqgroup in DQChannelDict.items():
            colstr.append( dqgroup )
            colchans.append( chan )

    # here per channel
    colstr = []
    colchans = []
    ndq = {}
    for chan, dqchan in dqitems:
        dqchan = "SHIFTOFL_%s" % (dqchan)
        colstr.append( dqchan )
        colchans.append( chan )

        ndq[chan] = {}
        for dqflag in dqflags:
            ndq[chan][dqflag] = [0, []]

        if dic.has_key( dqchan ):
            for i, dqflag in enumerate(dic[dqchan]):
                ndq[chan][dqflag][0] += 1
                ndq[chan][dqflag][1].append(runs[i])


    # create summary table
    # break table after 'nc' entries
    nc = 15
    colstr_ = []
    colchans_ = []
    for i in range(len(colchans)):
        if (i+1)%nc == 0:
            labeloffset = i # required to avoid double-labels for tool-tips
            colstr_ = []
            colchans_ = []            
        else:
            colstr_.append(colstr[i])
            colchans_.append(colchans[i])

    # groups
    labeloffset = 0
    for dqgroup, content in DQSuperGroupsDict.iteritems():
        colstr_   = []
        colchans_ = []
        title     = content[0]
        for dqchan in content[1]:                        
            colstr_.append(dqchan)
            colchans_.append(DQChannelDict[dqchan])
        s += MakeDQSummaryTableHtml( '', title, colstr_, colchans_, dqflags, dqflagsNames, ndq, nruns, labeloffset )
        labeloffset += 1 # required to avoid double-labels for tool-tips
        s += '<p></p>'            

    # return full page string
    return s
