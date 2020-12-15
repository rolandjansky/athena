#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQueryHTML.py
# Project: AtlRunQuery
# Purpose: Library with HTML code creation
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Apr 19, 2010
# ----------------------------------------------------------------
#
from __future__ import with_statement

import datetime

from CoolRunQuery.AtlRunQueryRun         import Run

from CoolRunQuery.utils.AtlRunQueryUtils import addKommaToNumber, filesize
from CoolRunQuery.utils.AtlRunQueryTimer import timer

from CoolRunQuery.html.AtlRunQueryPageMaker        import PageMaker as PM
from CoolRunQuery.selector.AtlRunQuerySelectorBase import DataKey


class ResultPageMaker:

    @classmethod
    def makePage(cls, pageinfo, doDQPickle=True):

        # turn dict pageinfo into local variables
        class PI:
            pass
        for x in pageinfo:
            setattr (PI, x, pageinfo[x])


        # put in link to cache
        cachelink = PI.datapath[PI.datapath.rfind('arq_'):].rstrip('/')
        cachelinkline = '[ <a style="font-size:11px;font-weight:100;font-style:normal" href="query.py?q=%s"><i>cached link to this result page</i></a> ]' % cachelink

        extraReplace = [ ('<!--INSERTLINKCACHE-->', cachelinkline) ]

        # START WITH THE PAGE
        body = u''

        # overall summary output if requested
        if PI.makeSummary:
            body += cls._OverallSummary( pageinfo )

        # overall summary output if requested
        if PI.makeDQSummary or PI.makeDQPlots:
            body += cls._DQSummaryWrap( pageinfo, doPickle=doDQPickle )
            extraReplace += [ ('<!--CONTACT_START-->.*<!--CONTACT_END-->', '<a href="http://consult.cern.ch/xwho/people/572921">Jessica Leveque</a>') ]
            
        # initial information (query, selection steps, totNEv)
        pageinfo['selstr'] = cls._makeSelectionSteps( pageinfo )
        body += cls._InitialSummary( pageinfo )

        if PI.makeDQSummary or PI.makeDQPlots:
            # only the global tooltips are needed
            body += cls._defineToolTips( pageinfo, globalOnly=True )
        else:
            # horizontal line
            body += '<hr style="width:95%; background-color: #999999; height:1px; margin-left:14px; border:0">\n<p>\n'

            # results table
            pageinfo['sumstr'] = cls._makeYellowLine( pageinfo ) # the yellow summary line
            body += cls._makeResultsTable( pageinfo )

            # tooltips
            body += cls._defineToolTips( pageinfo )

            # bottom table
            body += '<p>\n'
            body += cls._makeBottomTable(pageinfo)

        PM.makePage(body, PI.origQuery, extraReplace=extraReplace, removeExamples = (PI.makeDQSummary or PI.makeDQPlots))



    @classmethod
    def _makeResultsTable( cls, pageinfo ):
        class PI:
            pass
        for x in pageinfo:
            setattr (PI, x, pageinfo[x])

        # run results table
        resultstable = '<table class="resulttable" id="resulttable" style="margin-left: 13px">\n'

        # table head
        resultstable += Run.header()

        # runs
        with timer("print the runs"):
            for r in PI.runlist:
                with timer("print run %i" % r.runNr):
                    resultstable += str(r)

        # summary
        headlist = Run.headerkeys()
        resultstable += "  <tr class=\"space\"><td style=\"text-align: left;\" colspan=\"%i\"><font size=-2><i>Summary</i>:</font></td></tr>\n" % (len(headlist)-1)
        resultstable += "  <tr class=\"sum\">" + PI.sumstr + "</tr>\n"
        resultstable += "</table>\n"
        return resultstable


    @classmethod
    def _defineToolTips( cls, pageinfo, globalOnly=False ):
        class PI:
            pass
        for x in pageinfo:
            setattr (PI, x, pageinfo[x])

        tooltips = u''
        with timer("print the tooltips"):
            dw_call  = ['<script type="text/javascript">']
            dw_call += ["if(dw_Tooltip.content_vars==undefined) {dw_Tooltip.content_vars = {}; };"]
            dw_call += Run.GlobalTooltips
            if not globalOnly:
                for r in PI.runlist:
                    dw_call += r.tooltips
            dw_call += ['</script>']
            for ttip in dw_call:
                tooltips += ttip + '\n'
        return tooltips



    # makes the bottom table (between yellow line and footer
    @classmethod
    def _makeBottomTable( cls, pageinfo ):
        class PI:
            pass
        for x in pageinfo:
            setattr (PI, x, pageinfo[x])

        bottomTable = '<table cellspacing="0" style="color: #777777; font-size: 80%; margin-left: 13px">\n'
        bottomTable += '<tr>\n<td valign="top">\n'
        bottomTable += cls._Description()
        bottomTable += '<tr><td colspan="2"><p><hr style="width:40%; color:#999999; background-color: #999999; height:1px; margin-left:0; border:0"/>\n<p>\n</td></tr>'        

        # XML output (official format for input to good-run lists)
        bottomTable += cls._XMLFormat(PI.datapath, PI.xmlfilename, PI.xmlhtmlstr)

        # pickled output
        bottomTable += cls._PickledOutput(PI.datapath)

        # ROOT output
        bottomTable += cls._RootOutput(PI.roothtmlstr, PI.datapath)

        # end of bottom table
        bottomTable += '<tr><td colspan="2"><p>\n<p>\n</td></tr>'
        bottomTable += '</table>'
        return bottomTable
        

    # create DQ efficiency summary
    @classmethod
    def _makeDQeff( cls, pageinfo ):
        class PI:
            pass
        for x in ["dicsum", "dic", "datapath", "makeDQeff"]:
            setattr (PI, x, pageinfo[x])

        if PI.dicsum and PI.makeDQeff:
            with timer("make the DQ efficiency summary"):
                from CoolRunQuery.utils.AtlRunQueryDQUtils import MakeDQeffHtml
                return MakeDQeffHtml( PI.dic, PI.dicsum, PI.datapath )
        else:
            return ''

    # build the yellow summary line
    @classmethod
    def _makeYellowLine( cls, pageinfo ):
        class PI:
            pass
        for x in ["dicsum"]:
            setattr (PI, x, pageinfo[x])

        with timer("make the yellow summary line"):
            sumdic = {}
            for key, summary in PI.dicsum.items():
                if key.Type == DataKey.STREAM:
                    s = addKommaToNumber(summary[0])+' <BR><font size="-2">(%s)</font>' % filesize(summary[1])
                elif key=='Run':
                    s = addKommaToNumber(summary) + "&nbsp;runs"
                else:
                    s = addKommaToNumber(summary)
                sumdic[key.ResultKey] = s.strip()
        sumstr = ''
        headlist = Run.headerkeys()
        for title in headlist:
            sumstr += '<td style="text-align:right;">%s</td>' % (sumdic[title] if title in sumdic else '')
        return sumstr


    # selection steps
    @classmethod
    def _makeSelectionSteps( cls, pageinfo ):
        class PI:
            pass
        for x in ["selout"]:
            setattr (PI, x, pageinfo[x])

        selstr = '<table style="color: #777777; font-size: 100%" cellpadding="0" cellspacing="0">'
        for sel in PI.selout:
            if 'SELOUT' == sel[0:6]:
                selout = sel[7:].split('==>')
                if len(selout)==1:
                    selstr += '<tr><td width="400" style="vertical-align:top">%s</td></tr>' % selout[0]
                else:
                    selstr += '<tr><td width="400" style="vertical-align:top">%s</td><td width="20" style="vertical-align:top">:</td><td style="vertical-align:top">%s</td></tr>' % tuple(selout[0:2])
        selstr += '</table>'
        return selstr




    @classmethod
    def _prettyNumber( cls, n, width=-1, delim=',',decimal='.' ):
        """Converts a float to a string with appropriately placed commas"""
        if width >= 0:
            s = "%.*f" % (width, n)
        else:
            s = str(n)
        dec = s.find(decimal)
        if dec == -1:
            dec = len(s)
        threes = int((dec-1)/3) 
        for i in range(threes):
            loc = dec-3*(i+1)
            s = s[:loc] + delim + s[loc:]
        return s


    @classmethod
    def _OverallSummary( cls, pageinfo ):
        class PI:
            pass
        for x in pageinfo:
            setattr (PI, x, pageinfo[x])

        with timer("make the summary"):
            from CoolRunQuery.html.AtlRunQuerySummary import MakeSummaryHtml
            overallsummarystr = MakeSummaryHtml( PI.dic, PI.dicsum, PI.datapath )
        if overallsummarystr == '':
            return ''
        s = '''<table width="95%" cellpadding="5" style="margin-left: 13px">
        <tr><td colspan=2 bgcolor=gainsboro><font size=+1><b>Search Result Summary</b></font></td></tr>
        </table>'''
        s += '<p></p>'
        s += overallsummarystr
        s += '<p></p>'
        return s


    @classmethod
    def _DQSummaryWrap( cls, pageinfo, doPickle=True ):
        from CoolRunQuery.utils.AtlRunQueryUtils import runsOnServer
        if not runsOnServer() and doPickle:
            import pickle
            from CoolRunQuery.AtlRunQueryQueryConfig import QC
            f = open('%s/dqsum_pi.pickle' % QC.datapath, "w")
            pickle.dump(pageinfo, f)
            f.close()

        class PI:
            pass
        for x in pageinfo:
            setattr (PI, x, pageinfo[x])

        with timer("make the DQ summary"):
            from CoolRunQuery.html.AtlRunQueryDQSummary import DQSummary
            dqsummary = DQSummary.makeHTML( PI.dic, PI.dicsum, doDQSummary=PI.makeDQSummary, doDQPlots=PI.makeDQPlots, dqsumGRL=PI.dqsumgrl, dbbTag=PI.dbbtag)

        if dqsummary == '':
            return ''
        s = '''<table width="95%" cellpadding="5" style="margin-left: 13px">
        <tr><td colspan=2 bgcolor=gainsboro><font size=+1><b>Data Quality Summary</b></font></td></tr>
        </table>'''
        s += '<p></p>'
        s += dqsummary
        s += '<p></p>'
        return s


    @classmethod
    def _InitialSummary(cls, pageinfo):
        class PI:
            pass
        for x in pageinfo:
            setattr (PI, x, pageinfo[x])

        error = False
        totEv,naEv = Run.totevents[0:2]
        s_table = '''<table width="95%" cellpadding="5" style="margin-left: 13px">
        <tr><td colspan=2 bgcolor=gainsboro><font size=+1><b>Search Result</b></font></td></tr>
        </table>'''
        s_table += '<table width="95%" cellpadding="0" cellspacing="3" style="font-size: 90%; margin-left: 13px">\n'
        s_table += '<tr height="10"></tr>\n'
        s_table += '<tr><td height="10" width="130" style="vertical-align:top"><i>Selection&nbsp;rule:</i></td><td width=10></td><td valign=top>%s</td></tr>\n' % PI.origQuery
        if 'erbose' in PI.fullQuery:
            s_table += '<tr><td height="10" style="vertical-align: top; color: #777777"><i>Query&nbsp;command:</i></td><td width=10 style="color: #777777"></td><td style="color: #777777">'
            s_table += """<a href="javascript:animatedcollapse.toggle('AtlRunQueryCmd')">"""
            s_table += '<font color="#777777">[ Click to expand/collapse command... ]</font></a>'
            s_table += '<div id="AtlRunQueryCmd" style="width: 100%; background: #FFFFFF; color: #777777; display:none">'
            s_table += '%s' % (PI.fullQuery)
            s_table += '</div></td></tr>\n'
            s_table += '<tr><td height="10" style="vertical-align: top; color: #777777"><i>Selection&nbsp;sequence:</i></td><td width=10 style="color: #777777"></td><td style="vertical-align: top">%s</td></tr>\n' % (PI.selstr)
        if not error:
            s_table += '<tr><td height="10" style="vertical-align: top"><i>No.&nbsp;of&nbsp;runs&nbsp;selected:</i></td><td></td><td valign="top">%s</td></tr>\n' % len(PI.runlist)
            if totEv >= 0:
                if naEv >= 0:
                    sr = 'run'
                    if naEv > 1:
                        sr += 's'
                    if naEv == 0:
                        s_table += '<tr><td height="10" valign="top"><i>Total&nbsp;no.&nbsp;of&nbsp;events:</i></td><td></td><td valign="top">%s</td></tr>\n' % (cls._prettyNumber(totEv))
                    else:
                        s_table += '<tr><td height="10" valign="top"><i>Total&nbsp;no.&nbsp;of&nbsp;events:</i></td><td></td><td valign="top">%s (excluding %i %s without available "#Events" information)</td></tr>\n' % (cls._prettyNumber(totEv), naEv, sr)
                else:
                    s_table += '<tr><td height="10" valign="top"><i>Total&nbsp;no.&nbsp;of&nbsp;events:</i></td><td></td><td valign="top">%s</td></tr>\n' % (cls._prettyNumber(totEv))
            s_table += '<tr><td height="10" valign="top"><i>Execution&nbsp;time:</i></td><td></td><td valign="top">%.1f sec</td></tr>\n' % round(PI.querytime,1)
            s_table += '<tr><td height=5 valign=top></td></tr>\n'
        s_table += '</table>'
        return s_table


    @classmethod
    def _PickledOutput(cls, datapath):
        return '''<tr><td style="vertical-align: top"><img vspace=0 src="images/download.gif">&nbsp;&nbsp;</td><td>
        <a href="./%s/atlrunquery.pickle" target=_blank title="Query results as serialised python dictionary">
        Result table as pickled <b>python dictionary</b> (right-click link to download)</a> 
        <br>
        use as follows:<br>
        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&minus;
        open file (f) in python and deserialise ("unpickle")
        via: dico = pickle.load(f)<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&minus; the dictionary keys are equal to the column titles,
        the lines are stored as list entries for each key
        </td></tr>''' % datapath


    @classmethod
    def _RootOutput(cls, roothtmlstr, datapath):
        if roothtmlstr is not None:
            return '''
            <tr><td style="vertical-align: top"><img vspace=0 width="17" src="images/tree_icon.gif">&nbsp;&nbsp;</td><td>
            <a href="./%s/atlrunquery.root" title="Query results as ROOT TTree">Result table as <b>TTree</b> in ROOT file (right-click link to download)</a></td></tr>
            <tr><td style="vertical-align: top"><img vspace=0 src="images/statistics.gif">&nbsp;&nbsp;</td><td style="vertical-align: top">
            <font color="#333333"><b>Graphs</b> of table columns versus run numbers:</font>
            </td></tr>
            <tr><td style="vertical-align: top"></td><td style="vertical-align: top">%s
            </td></tr>''' % (datapath, roothtmlstr)
        else:
            return '''
            <tr><td style="vertical-align: top"><img vspace=0 width="17" src="images/tree_icon.gif">&nbsp;&nbsp;</td><td style="color: #000064;">
            Not available. <font color="#333333">The creation of a ROOT file with the results has been disabled by default. Please use option <b><i>root</i></b> as shown in the examples under <font style="color:red">Xtras</font></font></td></tr>
            <tr><td style="vertical-align: top"><img vspace=0 src="images/statistics.gif">&nbsp;&nbsp;</td><td style="vertical-align: top">
            <font color="#333333">The creation of <b>Graphs</b> has been disabled by default. Please use option <b><i>root</i></b> as shown in the examples under <font style="color:red">Xtras</font></font>
            </td></tr>'''


    @classmethod
    def _Description(cls):
        return '''<tr style="line-height: 6.5"><td colspan="2"></td></tr>
        <tr><td style="vertical-align: top"><img vspace=0 src="images/info.gif">&nbsp;&nbsp;</td><td height="55"><i>n.a.</i>
        in the table stands for <i>not available</i>:<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&minus;
        in case of <i>#Events</i> this mostly indicates unproperly closed runs<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&minus;
        in case of <i>data quality flags</i> it indicates that the corresponding flag was not set<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&minus;
        <i><font color="red">runs in red</font></i> indicate that their selection is questionable due to n.a. information
        (n.a. fields pass the corresponding selection query)
        </td></tr>'''


    @classmethod
    def _PrintFormat(cls, s_table, resstr):
        s  = '<tr><td><img vspace=0 src="images/printtopic.gif">&nbsp;&nbsp;</td>\n'
        s += '<td><a href="javascript:openWindow('
        s += "'Print','<!DOCTYPE html PUBLIC &quot;-//W3C//DTD HTML 4.01 Transitional//EN&quot;><html xmlns:&quot;my&quot;><head><title>Run query result</title><LINK href=&quot;atlas-runquery.css&quot; rel=&quot;stylesheet&quot; type=&quot;text/css&quot;><body><table style=&quot;font-family: sans-serif; font-size: 85%%&quot;>"
        s += '<tr><td>%s</td></tr>' % s_table.replace('"','&quot;')
        s += '<tr><td>' + resstr.replace('resulttable','resulttabletext').replace('"','quot;') + '</td></tr>'
        s += '<tr><td><hr color=&quot;red&quot; size=1></td></tr><tr><td><font color=&quot;#777777&quot;><font size=&quot;-1&quot;><i><font size=&quot;-2&quot;>Created by AtlRunQuery on: %s</font></i></font></td></tr></table><script type=&quot;text/javascript&quot;></script></body></html>' % str(datetime.datetime.now())
        s += "')"
        s += '" title="Query results in text format for easy printing">Print version of results</a>'
        s += '</td></tr>'
        return s


    @classmethod
    def _XMLFormat(cls, datapath, xmlfilename, xmlhtmlstr):
        if xmlhtmlstr is not None:
            s = '''<tr><td style="vertical-align: top"><img vspace=0 src="images/xml-small.gif">&nbsp;&nbsp;</td><td>
            <a href="./%s/%s" target="_blank" title="Query result as LB collection in standard XML good-run-list format">Query result as standard <b>Good Run-LB List in XML format</b> (right-click link to download)</a> /
            <a href="./LumiRangeCollection.dtd" target="_blank" title="Corresponding DTD file">DTD file</a>
            </td></tr>''' % (datapath, xmlfilename)
            s += '<tr><td style="vertical-align: top"></td><td>'
            s += '<a href="javascript:openTextWindow('
            s += "'Print','<!DOCTYPE html PUBLIC &quot;-//W3C//DTD HTML 4.01 Transitional//EN&quot;><html xmlns:&quot;my&quot;><head><title>Run query result</title><LINK href=&quot;atlas-runquery.css&quot; rel=&quot;stylesheet&quot; type=&quot;text/css&quot;><body><table style=&quot;padding: 10px;font-family: sans-serif; font-size: 95%%&quot;>" 
            s += '<tr><td>%s</td></tr>' % xmlhtmlstr
            s += '</table></body></html>'
            s += "')"
            s += '" title="Query result in text format">Query result as standard <b>Good Run-LB List in pretty-text format</b> for visual inspection (click for pop-up)</a>'    
            s += '</td></tr>'
            return s
        else:
            return '''<tr><td style="vertical-align: top;"><img vspace=0 src="images/xml-small.gif">&nbsp;&nbsp;</td><td style="color: #000064;">
            Not available. <font color="#333333">The creation of <b>GRLs</b> based on the query result has been disabled by default. Please use option <b><i>grl</i></b> as shown in the examples under <font style="color:red">Xtras</font></font></td></tr>'''


