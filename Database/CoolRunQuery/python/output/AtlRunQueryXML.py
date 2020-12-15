#!/usr/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQueryXML.py
# Project: AtlRunQuery
# Purpose: XML file output of good run lists
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: May 6, 2009
# ----------------------------------------------------------------
#
# ---------------------------------------------------------------------------------------------------
# XML making
# ---------------------------------------------------------------------------------------------------

from __future__ import with_statement
from CoolRunQuery.utils.AtlRunQueryTimer import timer

import datetime, sys
from xml.dom.minidom import Document, DocumentType, Element
from CoolRunQuery.utils.AtlRunQueryUtils import prettyNumber

# ---------------------------------------------------------------------------------------------------
# Creation of XML output file
# ---------------------------------------------------------------------------------------------------

# This is an Element, but does not print sub-elements. It is designed
# to hold attributes and CharacterData, and prints everything in one
# line (unlike Element.writexml(), which always adds a 'newline'
class TextElement(Element):
    def __init__(self,name,data,doc):
        Element.__init__(self,name)
        if not isinstance(data, str):
            raise TypeError ("node contents must be a string")
        self.tagName = name
        self.data = data
        self.ownerDocument = doc
        self.attOrder = []
        
    def setAttribute(self, attname, value):
        if attname not in self.attOrder:
            self.attOrder += [attname]
        Element.setAttribute(self, attname, value)
        
    def writexml(self, writer, indent="", addindent="", newl=""):
        attrs = self._get_attributes()
        atstr = ''
        for a_name in self.attOrder:
            atstr += ' %s="%s"' % (a_name, attrs[a_name].value)

        if self.data:
            writer.write("%s<%s%s>%s</%s>%s" % (indent, self.tagName, atstr, self.data, self.tagName, newl ) )
        else:
            writer.write("%s<%s%s/>%s" % (indent, self.tagName, atstr, newl ) )

def CreateRunStreamAndNeventsList( runlist ):

    # open SFO DB connection
    from CoolRunQuery.utils.AtlRunQueryUtils import coolDbConn
    from CoolRunQuery.AtlRunQuerySFO   import GetSFO_NeventsAllPhysics

    cursor = coolDbConn.GetSFODBConnection().cursor()
    cursor.arraysize=1000

    # find streams
    runnrlist = [r.runNr for r in runlist]
    with timer('get SFO number of events AllPhysics'):
        runstreamevents = GetSFO_NeventsAllPhysics( cursor, runnrlist )  # { runnr: { stream: [(LUMIBLOCKNR, NREVENTS)] } }
    return runstreamevents, ",".join(['%i' % r for r in runnrlist])



def CreateXMLFile( runlist, options, origQuery, datapath, xmlfname, xmllabel, svnversion='Unknown'  ):
    """
    """


    with timer('create RunStreamAndNeventsList'):

        # show number of events per stream per run ?
        ShowNumberOfEventsPerStreamPerRun  = False
        ShowNumberOfEventsPerStreamSummary = True


        # find streams
        runstreamevents, runnrliststr = CreateRunStreamAndNeventsList( runlist ) # { runnr: { stream: [(LUMIBLOCKNR, NREVENTS)] } }

    with timer('prepare document'):
        ####################################

        ## create XML file of GRL

        ####################################    

        doc = Document()

        docType = DocumentType('LumiRangeCollection')
        docType.systemId = 'http://atlas-runquery.cern.ch/LumiRangeCollection.dtd'
        doc.appendChild(docType)

        # number of comments
        txt = ' Good-runs-list created by %s on %s ' % (sys.argv[0].split('/')[-1], str(datetime.datetime.now()))
        doc.appendChild( doc.createComment( txt ) )

        # root element
        lrc = doc.createElement('LumiRangeCollection')
        doc.appendChild(lrc)

        # NamedLumiRange
        namedLR = doc.createElement('NamedLumiRange')
        lrc.appendChild(namedLR)

        # name of NamedLumiRange
        namedLR.appendChild(TextElement('Name',xmllabel,doc))

        # version of NamedLumiRange
        namedLR.appendChild(TextElement('Version','2.1',doc))

        # metadata of NamedLumiRange
        metadata = {
            'Query'      : origQuery.split('/')[0],
            'RQTSVNVersion' : svnversion,
            'RunList'    : runnrliststr
            }

        for md in metadata:
            mdelm = TextElement('Metadata', metadata[md], doc)
            mdelm.setAttribute('Name', md)
            namedLR.appendChild(mdelm)

    with timer('ShowNumberOfEventsPerStreamSummary'):
        if ShowNumberOfEventsPerStreamSummary:
            strsummdelm = doc.createElement('Metadata')
            strsummdelm.setAttribute('Name','StreamListInfo')
            namedLR.appendChild(strsummdelm)


    # lumiblock collections of NamedLumiRange
    streams_sum = {}
    streams_byrun = {}
    with timer('Loop over all runs'):
        for run in runlist:
            lbc = doc.createElement('LumiBlockCollection')
            # run number
            runnrelm = TextElement('Run',str(run.runNr),doc)


            if len(run.stats['SMK']['random'])==2:
                (rd0,rd1) = run.stats['SMK']['random'][0:2]
                # protect against missing information
                if rd0 == 'n.a.':
                    rd0 = 0
                if rd1 == 'n.a.':
                    rd1 = 0
                runnrelm.setAttribute('PrescaleRD0',0x1<<(3+rd0))
                runnrelm.setAttribute('PrescaleRD1',0x1<<(3+rd1))
            else:
                (rd0,rd1,rd2,rd3) = run.stats['SMK']['random'][0:4]
                # protect against missing information
                if rd0 == 'n.a.':
                    rd0 = 0
                if rd1 == 'n.a.':
                    rd1 = 0
                if rd2 == 'n.a.':
                    rd2 = 0
                if rd3 == 'n.a.':
                    rd3 = 0
                runnrelm.setAttribute('Cut0', rd0)
                runnrelm.setAttribute('Cut1', rd1)
                runnrelm.setAttribute('Cut2', rd2)
                runnrelm.setAttribute('Cut3', rd3)

            lbc.appendChild(runnrelm)

            # streams (initialisation)
            streams = {}
            streams_byrun[run.runNr] = streams
            if run.runNr in runstreamevents: # protection in case the run does not have any stream
                for stream in runstreamevents[run.runNr].keys():
                    if 'physics_' == stream[:8]:
                        streams[stream] = [0,0] # only for physics streams
                        if stream not in streams_sum:
                            streams_sum[stream] = [0,0]
                        # total number of events in stream
                        for (nlb,nev) in runstreamevents[run.runNr][stream]:
                            streams[stream][0] += nev 

            # lumiblock ranges

            for lbrange in run.data.getLBRanges(activeOnly=True):
                lbrelm = TextElement('LBRange','',doc)
                lbrelm.setAttribute('Start',lbrange[1])
                lbrelm.setAttribute('End',lbrange[2]-1)
                lbc.appendChild(lbrelm)
                # count nevents in streams
                if run.runNr in runstreamevents: # protection in case the run does not have any stream
                    for stream, lbnevts in runstreamevents[run.runNr].items():
                        if 'physics_' == stream[:8]:
                            for (nlb,nev) in lbnevts:
                                if nlb>=lbrange[1] and nlb<lbrange[2]:
                                    streams[stream][1] += nev

            # append stream element
            strselm = doc.createElement('StreamsInRun')

            for stream in sorted (streams.keys()):
                nevts  = streams[stream]
                if ShowNumberOfEventsPerStreamPerRun:
                    strelm = TextElement('Stream','',doc)
                    strelm.setAttribute('Name', stream)
                    strelm.setAttribute('TotalNumOfEvents',    nevts[0])
                    strelm.setAttribute('NumOfSelectedEvents', nevts[1])
                    strselm.appendChild(strelm)
                eff = 0
                if nevts[0] > 0:
                    eff = nevts[1]/float(nevts[0])*100.0

                # collect total number of events
                streams_sum[stream][0] += nevts[0]
                streams_sum[stream][1] += nevts[1]

            # append streams
            if ShowNumberOfEventsPerStreamPerRun:
                lbc.appendChild(strselm)

            # append LumiBlickCollection
            namedLR.appendChild(lbc)

    with timer('Streams'):
        for stream in sorted(streams_sum.keys()):
            nevts  = streams_sum[stream]
            if ShowNumberOfEventsPerStreamSummary: 
                strelm = TextElement('Stream','',doc)
                strelm.setAttribute('Name', stream)
                strelm.setAttribute('TotalNumOfEvents',    nevts[0])
                strelm.setAttribute('NumOfSelectedEvents', nevts[1])
                strsummdelm.appendChild(strelm)    


    with timer('Save GRL'):

        filename = '%s/%s' % (datapath, xmlfname)
        #print "Writing",filename
        xmlfile = open( filename, mode="w" )         
        xmlfile.write( doc.toprettyxml('   ') )
        xmlfile.close()

    with timer('Create HTML'):

        ####################################

        ## create HTML

        ####################################    

        # provide also pretty html text output
        htmltext = ''
        htmltext += '<table style=&quot;width: auto; border: 0px solid; border-width: margin: 0 0 0 0; 0px; border-spacing: 0px; border-collapse: separate; padding: 0px;&quot; font-family: sans-serif; font-size: 85%%&quot;>\n'
        htmltext += '<tr><td colspan=&quot;2&quot;><b><font color=&quot;#999999&quot;>' + txt.strip() + '</font></b></td></tr>\n'
        htmltext += '<tr><td style=&quot;vertical-align:top&quot;>SVN&nbsp;Version: </td><td> ' + svnversion + '</td></tr>\n'    
        htmltext += '<tr><td style=&quot;vertical-align:top&quot;>Query&nbsp;string:</td><td><b>' + origQuery.split('/')[0] + '</b></td></tr>\n'    
        htmltext += '<tr><td style=&quot;vertical-align:top&quot;>Run list:</td><td>' + runnrliststr + '</td></tr>\n'    
        htmltext += '</table>'
        htmltext += '<hr color=&quot;#000000&quot; size=1><font color=&quot;#777777&quot;>\n'
        htmltext += '<table style=&quot;width: auto; border: 0px solid; border-width: margin: 0 0 0 0; 0px; border-spacing: 0px; border-collapse: separate; padding: 0px;&quot; font-family: sans-serif; font-size: 90%%&quot;>\n'


        # lumiblock collections of NamedLumiRange
        for run in runlist:
            # run number
            htmltext += '<tr><td style=&quot;text-align:left;height:25px;vertical-align:bottom&quot;>Run <b>%i</b>:</td><td></td></tr>\n' % run.runNr

            # lumiblock ranges

            for lbrange in run.data.getLBRanges(activeOnly=True):
                htmltext += '<tr><td></td><td style=&quot;text-align:left&quot;>LB range: [%5i-%5i]</td></tr>\n' % (lbrange[1],lbrange[2]-1)

            # append stream element
            htmltext += '<tr><td></td><td style=&quot;text-align:left&quot;></td></tr>'
            htmltext += '<tr><td></td><td style=&quot;text-align:left&quot;><table style=&quot;width: auto; border: 0px solid; border-width: margin: 0 0 0 0; 0px; border-spacing: 0px; border-collapse: separate; padding: 0px;font-family: sans-serif; font-size: 90%&quot;><tr><td>Stream name</td><td>#Events total</td><td>&nbsp;&nbsp;&nbsp;#Events selected</td><td>&nbsp;&nbsp;&nbsp;Sel. fraction (%)</td></tr>\n'

            streams = streams_byrun[run.runNr]
            for stream in sorted(streams.keys()):
                nevts  = streams[stream]
                eff    = (nevts[1]/float(nevts[0])*100.0) if (nevts[0] > 0) else 0
                htmltext += '<tr><td style=&quot;text-align:left&quot;><i>%s</i></td><td style=&quot;text-align:right&quot;>%s</td><td style=&quot;text-align:right&quot;>%s</td><td style=&quot;text-align:right&quot;>%.4g</td></tr>\n' % (stream, prettyNumber(nevts[0]),prettyNumber(nevts[1]),eff)

            htmltext += '</table></td></tr>\n'


        # append stream element
        htmltext += '</table>'
        htmltext += '<hr color=&quot;#000000&quot; size=1><font color=&quot;#777777&quot;>\n'
        htmltext += '<b>Stream summary for all selected runs:</b><br>\n'
        htmltext += '<table style=&quot;width: auto; border: 0px solid; border-width: margin: 0 0 0 0; 0px; border-spacing: 0px; border-collapse: separate; padding: 0px;font-family: sans-serif; font-size: 95%&quot;><tr><td>Stream name</td><td>#Events total</td><td>&nbsp;&nbsp;&nbsp;#Events selected</td><td>&nbsp;&nbsp;&nbsp;Sel. fraction (%)</td></tr>\n'
        for stream in sorted(streams_sum.keys()):
            nevts  = streams_sum[stream]
            eff = 0
            if nevts[0] > 0:
                eff = nevts[1]/float(nevts[0])*100.0
            htmltext += '<tr><td style=&quot;text-align:left&quot;><i>%s</i></td><td style=&quot;text-align:right&quot;>%s</td><td style=&quot;text-align:right&quot;>%s</td><td style=&quot;text-align:right&quot;>%.4g</td></tr>\n' % (stream, prettyNumber(nevts[0]),prettyNumber(nevts[1]),eff)
        htmltext += '</table>\n'

    #print """========================================================
    #%r
    #===========================================================
    #""" % htmltext


    # provide also text output
    return htmltext
