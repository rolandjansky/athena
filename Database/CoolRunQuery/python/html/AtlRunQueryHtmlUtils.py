# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import time, datetime

from CoolRunQuery.AtlRunQueryRun import Run
from CoolRunQuery.AtlRunQueryQueryConfig import QC

def WrapIntoHTML(content, title="Run query result", extracss=None):
    wrap  = '<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">'
    wrap += '<html xmlns:"my"><head><title>%s</title>' % title
    wrap += '<LINK href="atlas-runquery-lb.css" rel="stylesheet" type="text/css">'
    if extracss:
        for css in extracss:
            wrap += '<LINK href="%s" rel="stylesheet" type="text/css">' % css
    wrap += '</head>'
    wrap += '<body>%s</body></html>' % content
    return wrap


def OpenWindow(content, title="Run query result", extracss=None, size="normal"):
    wrap = WrapIntoHTML(content, title, extracss)
    openWindowCmd = 'openWindow'
    if size=='large':
        openWindowCmd = 'openLargeWindow'
    return "<a href=\"javascript:%s('Print', '%s')\">" % ( openWindowCmd, wrap.replace('"','&quot') )


def CreatePopupHtmlPage(name,wincontent):
    filename = 'popupContent_%s.html' % name
    outfn = '%s/%s' % (QC.datapath, filename) 
    fh = open(outfn,"w")
    if type(wincontent)==list:
        for line in wincontent:
            print (line, file = fh)
    else:
        print (wincontent, file = fh)
    fh.close()
    return outfn



def CreateLBTable(run):
    body  = '<table class="outer">'
    body += '<tr><td><b>LB start times (%s) and durations for run %i:</b><br>' % (QC.tzdesc(),run.runNr)
    body += '<font size="-1" color="#888888">(Begin/end of run: %s)</font>' % run.timestr('html', run.runNr != Run.runnropen)
    body += '<hr color="black" size=1>'
    body += '<table class="lb">'
    body += '<tr><th>LB</th><th>Start time (%s)</th><th>Duration (sec)</th></tr>' % QC.tzdesc()
    body += '<tr>'
    for idx,(lbtime,lbendtime) in enumerate(run.lbtimes):
        lb=idx+1
        timetuple = time.localtime(lbtime/1.E9) if QC.localtime else time.gmtime(lbtime/1.E9)
        body += '<tr><td>%s</td><td>%s</td><td>%.2f</td></tr>' % (lb, time.strftime('%X',timetuple), (float(lbendtime)-float(lbtime))/1.E9)
    if run.runNr == Run.runnropen: # run still ongoing
        body += '<tr><td style="text-align:left"colspan="3"><i>&nbsp;&nbsp;Run still ongoing ...</i></td></tr>'
    body += '</tr></table>'
    body += '<hr color="red" size=1><font color="#777777"><font size="-1"><i><font size="-2">Created by AtlRunQuery on: %s</font></i></font></td></tr></table>' % str(datetime.datetime.now())
    body += '</td></tr></table>'
    return body.replace('"','&quot')



def CreateLBTooltip(run):
    if len(run.lbtimes)==0:
        return
    
    content = '<strong><b>LB start times (%s) and durations for run %i:</b></strong><br>Begin/end of run: %s' % (QC.tzdesc(), run.runNr, run.timestr('html', run.runNr != Run.runnropen))
    content += '<hr style="width:100%; background-color: #BBBBBB; height:1px; margin-left:0; border:0"/>'
    content += '<table style="width: auto; border: 0px solid; border-width: margin: 0 0 0 0; 0px; border-spacing: 0px; border-collapse: separate; padding: 0px; font-size: 90%">'
    content += '<tr>'
    if len(run.lbtimes) > 150:
        content += '<td><i>Too many LBs to show ... (click instead!)</i></td>'
    else:
        for idx,(lbstart, lbend) in enumerate(run.lbtimes):
            lb=idx+1
            timetuple = time.localtime(lbstart/1.E9) if QC.localtime else time.gmtime(lbstart/1.E9)
            content += '<td style="text-align:right">&nbsp;&nbsp;%s:&nbsp;%s&nbsp;</td><td style="text-align:right">(%.1f&nbsp;sec)</td>' % (lb, time.strftime("%X",timetuple), (float(lbend)-float(lbstart))/1.E9)
            if lb%6==0:
                content += '</tr><tr>'
    content += '</tr></table>'
    content += '<hr style="width:100%; background-color: #BBBBBB; height:1px; margin-left:0; border:0"/>'
    content += '<font color="#AA0000">Click to obtain full list in independent window!</font>'
    run.addToolTip( 'LBStart_%i' % run.runNr, content )



def CreateDQTooltip(run):
    if 'DQ' not in Run.ShowOrder:
        return

    def_with_primaries = run.stats['DQ']["primaries"]
    condensed = True
    if not condensed:
        for x in sorted(def_with_primaries.keys()):
            l_ready = [y for y in def_with_primaries[x] if run.data.isReady( (y.since,y.until) )]
            if len(l_ready)==0:
                continue
            content  = '<strong><b>%s: comments for run %i:</b></strong><br><span style="font-size: 80%%">ATLAS READY in LBs %s</span>' % (x,run.runNr,', '.join(["%i&minus;%i" % (r.startlb, r.endlb-1) for r in run.data.isReady()]))
            content += '<hr width="100%%">'
            content += '<table style="width: auto; white-space: nowrap; border: 0px solid; margin: 0 0 0 0; border-spacing: 0px; border-collapse: separate; padding: 0px;">'
            for y in l_ready:
                s = "%i&minus;%i" % (y.since,y.until-1) if y.until-y.since!=1 else "%i" % (y.since)
                content += '<tr><td style="color:blue; padding-right: 5px;">LB %s</td>' % s # the LB info
                content += '<td style="font-weight:bold; padding-right: 5px;">%s</td><td><i>%s</i></td></tr>' % (y.description.split("->")[-1], y.comment)  # the primary and the comment
            content += '</table>'
            run.addToolTip("dqdefect_%i_%s" % (run.runNr,x), content)
    else:
        from itertools import groupby
        from operator import attrgetter
        gr = {}
        for k in def_with_primaries:
            d = {}
            for p,v in groupby(sorted(def_with_primaries[k]), key=attrgetter('description', 'comment')):
                d[p] = [(e.since, e.until) for e in v]
            gr[k] = d

        for x in sorted(gr):
            content  = '<strong><b>%s: comments for run %i:</b></strong><br><span style="font-size: 80%%">ATLAS READY in LBs %s</span>' % (x,run.runNr,', '.join(["%i&minus;%i" % (r.startlb, r.endlb-1) for r in run.data.isReady()]))
            content += '<hr width="100%%">'
            content += '<table style="width: auto; white-space: nowrap; border: 0px solid; margin: 0 0 0 0; border-spacing: 0px; border-collapse: separate; padding: 0px;">'
            for pdef in sorted(gr[x]):
                l_ready = [y for y in gr[x][pdef] if run.data.isReady( y )]
                if len(l_ready)==0:
                    continue
                s = ", ".join(["%i&minus;%i" % y if y[1]-y[0]!=1 else "%i" % (y[0]) for y in l_ready])
                content += '<tr><td colspan="2" style="color:blue; padding-right: 5px; max-width: 130;">LB %s</td></tr>' % s # the LB info
                content += '<tr><td style="font-weight:bold; padding-right: 5px;">%s</td><td><i>%s</i></td></tr>' % (pdef[0].split("->")[-1], pdef[1])  # the primary and the comment
            content += '</table>'
            run.addToolTip("dqdefect_%i_%s" % (run.runNr,x), content)


def CreateStreamOverlapTooltip(run,k):
    if not run.stats[k.ResultKey]['StrOverlap']:
        ovstr = '<tr><td width="200"><b><i>None (or not available for this run)</i></b></td></tr>'
    else:
        ovstr = ''
        nacc=0
        for stream, fraction in run.stats[k.ResultKey]['StrOverlap']:
            nacc+=1
            tdstr = '<td class="tdov%i">' % (nacc%2+1)
            fs = "%.2g" % fraction
            if fraction==100:
                fs = "100"
            ovstr += '<tr>%s%s</td>%s&nbsp;=&nbsp;</td>%s%s%%</td></tr>' % (tdstr, stream, tdstr, tdstr, fs)
            strpairs = [('STR:physics_MuonswBeam',   'physics_L1Calo'),
                        ('STR:physics_MinBias', 'physics_L1Calo'),
                        ('STR:physics_Muons',   'physics_Egamma'),
                        ('STR:physics_Muons',   'physics_JetTauEtmiss'),
                        ('STR:physics_Egamma',  'physics_JetTauEtmiss'),
                        ('STR:physics_MinBias', 'physics_JetTauEtmiss')]
            for st in strpairs:
                if k==st[0] and stream==st[1]:
                    s1 = st[0].replace('STR:','')
                    fname = 'data_' + s1.strip() + '_' + st[1].strip() + '.txt'
                    f = open(QC.datapath + '/' + fname,'a')
                    f.write('%i   %f\n' % (run.runNr,fraction))
                    f.close()
                    break

    boxcontent = '<table class="streamtiptable"><tr><td>'
    boxcontent += '<strong><b>Info&nbsp;for&nbsp;stream:&nbsp;<font color="#AA0000">%s</font></b></strong>' % k.Header[4:]
    boxcontent += '</td></tr><tr><td>'
    boxcontent += '<hr style="width:100%; background-color: #999999; height:1px; margin-left:0; border:0"/>'
    boxcontent += '</td></tr>'
    boxcontent += '<tr><td>'
    boxcontent += '<strong><b>Stream&nbsp;overlaps</b></strong>&nbsp;(nonzero&nbsp;overlaps&nbsp;only):'
    boxcontent += '</td></tr><tr><td>'
    boxcontent += '<table class="overlaptable">%s</table><strong>' % ovstr
    boxcontent += '</td></tr>'

    # Tier-0 output            
    prodsteps = ['StrTier0TypesRAW', 'StrTier0TypesESD' ]
    allt0text = ''
    for p in prodsteps:
        if p in run.stats[k.ResultKey]:
            typelist = list(run.stats[k.ResultKey][p])
            if not typelist:
                continue

            typelist.sort()
            t0text = ''
            for i, t0out in enumerate(typelist):
                if 'TMP' not in t0out: # don't show temporary output
                    if i%4==0 and i>0:
                        t0text += '<br>'                            
                    if t0out == 'NTUP':
                        t0outtxt = 'NTUP_... <font size="-2">(types as above)</font>' # stands for all NTUP types in merge step
                    else:
                        t0outtxt = t0out
                    if run.stats[k.ResultKey][p][t0out]: # output on CAF
                        t0text += '<font color="#BB0000">%s</font>, ' % t0outtxt
                    else:
                        t0text += '%s, ' % t0outtxt

            if t0text != '':
                allt0text += '<strong><b><i>'
                if 'RAW' in p:
                    allt0text += 'Produced&nbsp;by&nbsp;reconstruction&nbsp;step'
                    if 'StrTier0AMI' in run.stats[k.ResultKey] and run.stats[k.ResultKey]['StrTier0AMI']:
                        if p in run.stats[k.ResultKey]['StrTier0AMI']:
                            allt0text += '&nbsp;(AMI&nbsp;tag:&nbsp;%s)' % run.stats[k.ResultKey]['StrTier0AMI'][p]
                        else:
                            allt0text += '&nbsp;(AMI&nbsp;tag:&nbsp;%s)' % 'UNKNOWN'
                else:
                    allt0text += 'Produced&nbsp;by&nbsp;merge&nbsp;step'

                allt0text += ':</i></b></strong>'

                allt0text += '<table class="overlaptable"><tr><td style="padding-left:10px">' + t0text[:len(t0text)-2] + '</td></tr></table>'
                if p != prodsteps[-1]:
                    allt0text += ''

    if allt0text != '':
        boxcontent += '<tr><td>'
        boxcontent += '<hr style="width:100%; background-color:#999999; height:1px; margin-left:0; border:0"/>'
        boxcontent += '</td></tr><tr><td>'
        boxcontent += '<strong><b>Tier-0 output types for this stream:</b></strong><br>(Datasets in <font color="#BB000"><strong><b>red</b></strong></font> are <font color="#BB000">replicated to CAF</font>'
        boxcontent += '</td></tr><tr><td>'
        boxcontent += '<table class="overlaptable" style="color:#222222"><tr><td>%s</td></tr></table>' % allt0text
        boxcontent += '</td></tr>'

    # events-per-LB information
    lbrecinfo = run.stats[k.ResultKey]['LBRecInfo']
    if not lbrecinfo:
        lbrecinfo = '<tr><td style="font-size:75%"><i>No LB information in SFO DB. <br> Probably because files for this stream <br>were not closed at LB boundaries by SFO.</i></td></tr>'
    else:
        ev = dict(lbrecinfo)
        for lb in ev:
            ev[lb]=0
        for lb,nev in lbrecinfo:
            ev[lb] += nev
        lbs = sorted(ev.keys())
        output = '<tr>'
        idx = 0
        while idx <len(lbs):
            output += '<td style="font-size:75%%;text-align:right">%i&nbsp;(%s)</td>' % (lbs[idx],ev[lbs[idx]])
            if len(lbs)>50 and idx==23:
                output += '</tr><tr><td style="font-size:75%; text-align:left" colspan="8">&nbsp;... <i>too many LBs ... show begin and end of run only...</i></td></tr>'
                idx = len(lbs) - 23
            elif (idx+1)%8==0:
                output += '</tr><tr>' # newline every 10
            idx += 1
        output += '</tr>'
        lbrecinfo = output

    boxcontent += '<tr><td>'
    boxcontent += '<hr style="width:100%; background-color:#999999; height:1px; margin-left:0; border:0"/>'
    boxcontent += '<strong><b>Recorded LB (#Events)</b></strong></strong>:'
    boxcontent += '</td></tr><tr><td>'
    boxcontent += '<table class="eventsperlbstreamtable" style="color:#222222">%s</table>' % lbrecinfo
    boxcontent += '</td></tr></table>'
    if 'No LB information' not in lbrecinfo:
        boxcontent += '<font color="#AA0000"><strong><b>Click to obtain the full LB list and plots in independent window!</b></strong></font>'

    run.addToolTip("STROV_%i_%s" % (run.runNr, k.ResultKey[4:]), boxcontent)





def createRatePopupWindow(v,run):
    from CoolRunQuery.output.AtlRunQueryRoot import makeRatePlot
    histoText = '' # upper left on above the histogramm

    lbduration = [(idx+1,lbtime,(lbendtime-lbtime)/1e9) for idx,(lbtime,lbendtime) in enumerate(run.lbtimes) ]

    duration = (run.lbtimes[-1][1] - run.lbtimes[0][0])/1e9
    averrate = []
    for tr in v:
        averrate.append((tr, sum([int(co[3]) for co in v[tr]])/duration))
    averrate.sort(lambda x,y: y[1]-x[1])

    paths = []
    wincmds = []
    triggergroups = []
    loopcntr = 0
    plotstart = 0
    plotrange = 8
    triggers_in_range = averrate[plotstart:plotstart+plotrange]
    while len(triggers_in_range)>0:
        path  = makeRatePlot( v, lbduration, triggers_in_range, averrate, 'Luminosity block number', 'Rate [Hz]',
                              'trigcounts%i_vs_lb_run_%i' % (loopcntr,run.runNr),
                              'Trigger Rates for run %i' % (run.runNr),
                              QC.datapath, histoText )
        paths += [path]

        wincmd = createRateWinContent(loopcntr, v, lbduration, triggers_in_range, run, path)
        wincmds += [wincmd]

        triggergroups += [triggers_in_range]

        loopcntr += 1
        plotstart += plotrange
        triggers_in_range = averrate[plotstart:plotstart+plotrange]

    return triggergroups,paths,wincmds




def createRateWinContent(loopcntr, v, lbduration, triggers_in_range, run, path):

    triggers = [tr[0] for tr in triggers_in_range]

    # create window and tooltip
    wincontent  = ['<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">']
    wincontent += ['<html xmlns:"my">']
    wincontent += ['  <head>']
    wincontent += ['    <title>Run query result for trigger rates</title>']
    wincontent += ['    <LINK href="atlas-runquery-lb.css" rel="stylesheet" type="text/css">']
    wincontent += ['  </head>']
    wincontent += ['  <body>']
    wincontent += ['    <table class="outer" style="padding: 5px">']
    wincontent += ['      <tr><td><strong><b>Trigger rates for run %s:</b></strong><br><font size="-1" color="#888888">(Begin/end of run: %s)</font></td></tr>' % (run.runNr, run.timestr('html', run.runNr != Run.runnropen))]
    wincontent += ['      <tr><td colspan="2"><hr color="black" size=1></td></tr>']
    wincontent += ['      <tr><td><img src="%s" align="left"></td>' % path.rsplit('/')[-1]]
    wincontent += ['      <td><img src="%s" align="left"></td></tr>' % path.rsplit('/')[-1].replace('.png','_log.png')]
    wincontent += ['      <tr><td colspan="2"><hr color="black" size=1></td></tr>']
    wincontent += ['      <tr><td colspan="2" style="font-size:70%; height:30px; text-align: right;  vertical-align: middle;"><b>TBP</b> - <i>trigger before prescale</i>,<b>TAV</b> - <i>trigger after veto</i>, <b>DTF</b> - <i>dead time fraction</i></td></tr>']
    wincontent += ['      <tr><td colspan="2">']
    wincontent += ['        <table class="LB">']
    header = '<th>LB</th><th>Start time</th><th>Duration</th>'
    header2 = '<th></th><th>(%s)</th><th>(s)</th>' % QC.tzdesc()
    for tr in triggers:
        if "UNPAIRED" in tr or "EMPTY" in tr or "FILLED" in tr:
            header += '<th colspan="3">%s<BR>_%s</th>' % tuple(tr.rsplit('_',1))
        else:
            header += '<th colspan="3">%s</th>' % tr
        header2 += '<th>TBP</th><th>TAV</th><th>DTF</th>'
    wincontent += ['          <thead>']
    wincontent += ['            <tr>%s</tr>' % header]
    wincontent += ['            <tr class="second">%s</tr>' % header2]
    wincontent += ['          </thead>']
    wincontent += ['          <tbody>']
    for lb, lbstart, dt in lbduration:
        timetuple = time.localtime(lbstart/1.E9) if QC.localtime else time.gmtime(lbstart/1.E9)
        linecontent = '<td>%i</td><td>%s</td><td class="dt">%3.1f</td>' % (lb,time.strftime("%X",timetuple),dt)
        for tr in triggers:
            _lb,bp,ap,av = v[tr][lb-1]
            try:
                avr=float(av)/dt
            except ValueError:
                avr=0
            try:
                bpr=float(bp)/dt
            except ValueError:
                bpr=0
            try:
                apr=float(ap)/dt
            except ValueError:
                apr=0
            deadtime=0
            bg = ""
            if apr>0:
                deadtime = ( apr - avr ) / apr
                if deadtime>0.90:
                    bg=' style="background-color: #ff0000;"'
                elif deadtime>0.30:
                    bg=' style="background-color: #ff3333;"'
                elif deadtime>0.10:
                    bg=' style="background-color: #ff6666;"'
                elif deadtime>0.05:
                    bg=' style="background-color: #ff9999;"'
                elif deadtime>0.01:
                    bg=' style="background-color: #ffcccc;"'
            linecontent += '<td>%3.1f</td><td>%3.1f</td><td%s class="dt">(%2.1f%%)</td>' % (bpr,avr,bg,100*deadtime)
        if lb%2!=0:
            col = '#eeeeee'
        else:
            col = '#ffffff'
        wincontent += ['            <tr style="background:%s">%s</tr>' % (col,linecontent)]

    if run.runNr == Run.runnropen: # run still ongoing
        wincontent += ['            <tr><td style="text-align:left"colspan="%i"><i>&nbsp;&nbsp;Run still ongoing ...</i></td></tr>' % (len(triggers)+1)]
    wincontent += ['          </tbody>']
    wincontent += ['        </table>']
    wincontent += ['      </td></tr>']
    wincontent += ['      <tr><td colspan="2"><hr color="red" size=1></td></tr>']
    wincontent += ['      <tr><td colspan="2">']
    wincontent += ['        <font color="#777777" size="-1"><i><font size="-2">Created by AtlRunQuery on: %s</font></i></font>' % str(datetime.datetime.now())]
    wincontent += ['      </td></tr>']
    wincontent += ['    </table>']
    wincontent += ['  </body>']
    wincontent += ['</html>']

    CreatePopupHtmlPage('trRates_%i_%i' % (run.runNr, loopcntr), wincontent)
    return ''





def makeSummaryPlotForLHC(run):
    from CoolRunQuery.output.AtlRunQueryRoot import makeLBPlotSummaryForLHC
    # make summary of all LHC information
    # requires: beam intensities, beam energy, stable beams
    # keys = [ 'olc:beam1intensity', 'olc:beam2intensity', 'lhc:beamenergy', 'olc:lumi:0' ]
    lbrange    = range(run.lastlb+1)
    yvec    = [(run.lastlb+1)*[0], (run.lastlb+1)*[0], (run.lastlb+1)*[-1]]  # note that this is not the same as 3*[[]]
    ymax    = 3*[-1]

    # stable beams
    hasStableBeamsInfo, xvecStb = run.getStableBeamsVector()
    for ik,k in enumerate( [ 'olc:beam1intensity', 'olc:beam2intensity', 'lhc:beamenergy' ] ):
        if k not in Run.ShowOrder:
            continue
        for entry in run.data[k]:
            if entry.startlb == 0:
                continue

            val = float(entry.value) if (entry.value!='n.a.') else 0

            if ik==0 or ik==1:
                if val > 1e30:
                    val = 0
                val *= 1.0 # beam intensities in 10^11 protons
            elif ik==2:
                if val >= 7864:
                    val = -1

            lastlb = min(entry.lastlb,run.lastlb)

            for lb in range(entry.startlb,lastlb+1):
                yvec[ik][lb] = val

            for ilb in range(entry.startlb,lastlb+1):
                if val > ymax[ik]:
                    if not hasStableBeamsInfo or ilb in xvecStb:
                        ymax[ik] = val # find max                

    histoText = ''
    path = makeLBPlotSummaryForLHC( lbrange, xvecStb, yvec, run.runNr, QC.datapath, histoText )

    return path, lbrange, yvec, ymax


def makeSummaryPageForLHC(run, yvec, path):
    # create window and tooltip
    wincontent  = '<table class=&quot;outer&quot; style=&quot;padding: 5px; font-family: sans-serif; font-size: 85%&quot;><tr><td>'
    wincontent += '<strong><b>LHC beam energy and intensities during run %i:</b></strong><br><font size=&quot;-1&quot;><font color=&quot;#888888&quot;>(Begin/end of run: %s)</font></font><hr color=&quot;black&quot; size=1>' % (run.runNr, run.timestr('html', run.runNr != Run.runnropen))
    wincontent += '<table style=&quot;padding: 0px&quot;><tr><td>'
    wincontent += '<img src=&quot;%s&quot; align=&quot;left&quot;></td>' % path
    wincontent += '</td></tr></table>'
    wincontent += '<hr color=&quot;black&quot; size=1>'
    wincontent += '<table class=&quot;lb&quot;>'
    wincontent += '<tr><th style=&quot;text-align:right&quot;>LB</th><th style=&quot;text-align:right&quot;>&nbsp;&nbsp;&nbsp;Start time<br> (%s)</th><th style=&quot;text-align:right&quot;>&nbsp;&nbsp;&nbsp;Duration<br>(sec)</th><th style=&quot;text-align:right&quot;>&nbsp;&nbsp;&nbsp;Beam energy<br>(GeV)</th><th style=&quot;text-align:right&quot;>&nbsp;&nbsp;&nbsp;Intensity Beam-1<br>(1e11&nbsp;protons)</th><th style=&quot;text-align:right&quot;>&nbsp;&nbsp;&nbsp;Intensity Beam-2<br>(1e11&nbsp;protons)</th>' % QC.tzdesc()
    wincontent += '</tr><tr style=&quot;background:%s&quot;>' % '#eeeeee'
    nbeam1intensity = len(yvec[0])
    nbeam2intensity = len(yvec[1])
    nbeamenergy     = len(yvec[2])
    for idx,(lbtime,lbendtime) in enumerate(run.lbtimes):
        lb=idx+1
        timetuple = time.localtime(lbtime/1.E9) if QC.localtime else time.gmtime(lbtime/1.E9)
        beam1intensity = yvec[0][idx] if idx<nbeam1intensity else 0
        beam2intensity = yvec[1][idx] if idx<nbeam2intensity else 0
        beamenergy     = yvec[2][idx] if idx<nbeamenergy else 0
        wincontent += '<td>%i</td><td>%s</td><td>%.2f</td><td>%i</td><td>%.4g</td><td>%.4g</td>' % (lb, time.strftime("%X",timetuple), (float(lbendtime)-float(lbtime))/1.E9, beamenergy, beam1intensity, beam2intensity)
        if idx%2!=0:
            col = '#eeeeee'
        else:
            col = '#ffffff'
        wincontent += '</tr><tr style=&quot;background:%s&quot;>' % col
    if run.runNr == Run.runnropen: # run still ongoing
        wincontent += '<tr><td style=&quot;text-align:left&quot;colspan=&quot;4&quot;><i>&nbsp;&nbsp;Run still ongoing ...</i></td></tr>'
    wincontent += '</tr></table>'
    wincontent += """<hr color=&quot;red&quot; size=1><font color=&quot;#777777&quot;><font size=&quot;-1&quot;><i><font size=&quot;-2&quot;>Created by AtlRunQuery on: %s</font></i></font></td></tr></table>""" % str(datetime.datetime.now())
    return wincontent
