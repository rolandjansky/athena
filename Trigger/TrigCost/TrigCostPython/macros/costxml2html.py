#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import optparse
import string
import os,sys
from TrigCostPython import TrigCostAnalysis,TrigCostTRP,TrigCostCool,TrigCostXML

#
# parse options
#
preamble="Usage: costxml2html <xml> <htmlpath> \n" +\
          "Makes html files from TriggerCosts.xml files, htmlpath will be a directory"

optparser = optparse.OptionParser(usage=preamble)

optparser.add_option('', '--header',     type="string",  default="",
                     help="Put text at the top of each page")

optparser.add_option('', '--decimals',     type="int",  default=1,
                     help="Decimals after the dot")

(opts, args) = optparser.parse_args()

if len(args)!=2:
    print "Must have exactly two arguments"
    print preamble
    sys.exit(-1)

xmlin=args[0]
htmlpath=args[1]

#
# Make directory
#
if not os.path.isdir(htmlpath):
    os.mkdir(htmlpath)
    print "Created output directory:",htmlpath

if not os.path.isfile(htmlpath.rstrip('/')+'/sorttable.js'):
    from subprocess import Popen
    
    try :
        print "argv0 ",sys.argv[0]
        sortPath = sys.argv[0].replace("costxml2html.py","../TrigCostPython/sorttable.js")
        pCt = Popen('cp ' + sortPath + ' ' + htmlpath, shell=True)
        pCt.wait()  
    except TypeError :
        print 'Could not get TestArea to locate sorttable.js - it will not be copied to output location.' 

#
# Read XML file
# 
result=TrigCostXML.ReadXmlFile(xmlin)

#
# Write index
# 
levels = ['L1','L2','EF']
file = open(htmlpath+'/index.html', 'w')
if opts.header != "":
    file.write('<h2>'+opts.header+'</h2>\n')
for level in levels:
    file.write('<a href="chain_rates_'+level+'.html">'+level+' Chains</a> &nbsp;&nbsp;&nbsp; <a href="group_rates_'+level+'.html">'+level+' Groups</a></br>')



#
# Write HTML
# 
for level in levels:

    #
    # Chains table 
    #
    
    #open
    file = open(htmlpath+'/chain_rates_'+level+'.html', 'w')

    # table setup
    file.write('<style type="text/css">')
    file.write('table,  th {border:1px solid black;}')
    file.write('th { padding:10px; background-color:grey; color:white; }')
    file.write('td {padding-left:15px;padding-right:15px;}')
    file.write('</style>')
    file.write('<script src="sorttable.js"></script>\n')
    file.write('<table border=1 frame=box rules=cols class="sortable">\n')

    file.write('<h2>'+level+' Chain Rates</h2>\n')
    if opts.header != "":
        file.write('<h3>'+opts.header+'</h3>\n')

    # head row of table
    file.write('<tr>')
    file.write('<th>Chain Name</th>')
    if level=="L1":
        file.write('<th>L1 PS</th>')
        file.write('<th>L1 TBP</th>')
    if level=="L2":
        file.write('<th>L1PS*L2PS</th>')
        file.write('<th>L2PT</th>')
    elif level=="EF":
        file.write('<th>L1PS*L2PS*EFPS</th>')
        file.write('<th>EF PT</th>')
        file.write('<th>L2 PT</th>')
    file.write('<th>Rate</th>')
    file.write('</tr>')

    # data rows
    for chname in result.GetChainNames(level):
        ch=result.GetChain(chname)

        #if TrigCostTRP.NonChainTRP(chname):
        #    continue

        # product PS for L2 and EF
        prodps=ch.GetPrescale()
        l2pt=None
        try:
            if level=="L2" or level=="EF":
                lower_chain = result.GetChain(ch.GetLowerChain())
                prodps *= lower_chain.GetPrescale()
                if level=="EF":
                    l2pt=lower_chain.GetPassthrough()
                    lower_lower_chain = result.GetChain(lower_chain.GetLowerChain())
                    prodps *= lower_lower_chain.GetPrescale()
        except:
            prodps=None
            l2pt=None
            #print "Missing lower chain ",chname
        
        file.write('<td>'+ch.GetName()+'</td>')
        if level=="L1":
            file.write('<td>%10.1f</td>' % (ch.GetPrescale(),))
            file.write('<td>%10.1f</td>' % (ch.GetTBPRate(),))
        if level=="L2" or level=="EF":
            file.write('<td>%10.1f</td>' % (ch.GetPrescale(),))
            if prodps!=None:
                file.write('<td>%10.1f</td>' % (prodps,))
            else:
                file.write('<td></td>')

            pt = -9999
            try:
                pt = ch.GetPassthrough()
            except TrigCostAnalysis.VarNotAvailable,e:
                pt = -9999
                file.write('<td>%10.1f</td>' % (pt,))
            if level=="EF":
                if l2pt!=None:
                    file.write('<td>%10.1f</td>' % (l2pt,))
                else:
                    file.write('<td></td>')

        form='<td>%10.'+str(opts.decimals)+'f</td>'

        file.write(form % (ch.GetRate(),))
        file.write('</tr>')

    # end table
    file.write('</table>\n')
    file.write('<META HTTP-EQUIV=\"PRAGMA\" CONTENT=\"NO-CACHE\">')
    file.close()

    #
    # Groups table 
    #
    
    #open
    file = open(htmlpath+'/group_rates_'+level+'.html', 'w')

    # table setup
    file.write('<style type="text/css">')
    file.write('table,  th {border:1px solid black;}')
    file.write('th { padding:10px; background-color:grey; color:white; }')
    file.write('td {padding-left:15px;padding-right:15px;}')
    file.write('</style>')
    file.write('<script src="sorttable.js"></script>\n')
    file.write('<table border=1 frame=box rules=cols class="sortable">\n')
    file.write('<h2>'+level+' Chain Rates</h2>\n')
    if opts.header != "":
        file.write('<h3>'+opts.header+'</h3>\n')

    # head row of table
    file.write('<tr>')
    file.write('<th>Group Name</th>')
    file.write('<th>Rate</th>')
    file.write('</tr>')

    # data rows
    for ch in result.GetChains():
        if string.count(ch.GetName(),level+"_RATE:") == 1:            
            file.write('<tr>')
            file.write('<td>'+ch.GetName()+'</td>')
            form='<td>%10.'+str(opts.decimals)+'f</td>'
            file.write(form % (ch.GetRate(),))
            file.write('</tr>')

    # end table
    file.write('</table>\n')
    file.write('<META HTTP-EQUIV=\"PRAGMA\" CONTENT=\"NO-CACHE\">')
    file.close()
