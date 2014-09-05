# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import xml.dom.minidom
import sys, string


#-----------------------------------------------------------------------
#  Options definitions
#-----------------------------------------------------------------------
from optparse import OptionParser

p = OptionParser(usage="<rates xml> -o <output location>")

p.add_option("-o", "--output", type = "string", default = "./GroupRatesDef.xml",
             dest = "output_dir", help = "output directory for html" )

(options, args) = p.parse_args()

    
    
#-----------------------------------------------------------------
# Class to hold trigger info 
#-----------------------------------------------------------------
class Trigger :
    def _init_(self) :
        pass

    name = ""
    level = ""
    nevt = -1
    rate = 0.0
    counter = 0


#-----------------------------------------------------------------
# Read triggers form the Level 1 and HLT trigger menus
#-----------------------------------------------------------------
def ReadXmlFile(filename):
    def GetData(nodelist, name):

        for val in nodelist:
            if val.nodeName != name:
                continue

            svalue = ""
            icount = int(0)
            for s in val.childNodes:
                icount = icount+1
                svalue = s.data
                
            if icount == 1:
                return svalue
            else:
                raise Exception('Wrong child list')
            
        raise Exception('Failed to find data')

    # create and initialize parser
    myfL = open(filename)
    dom = xml.dom.minidom.parse(myfL)    

    triggers = dict() 
    for lev in dom.getElementsByTagName("level") :
        level = GetData(lev.childNodes, "lvl_name")
        for sig in lev.getElementsByTagName("signature") :

            tr = Trigger()
            tr.name = GetData(sig.childNodes, "sig_name")
            tr.nevt = string.atof(GetData(sig.childNodes, "evts_passed") )
            tr.rate = string.atof(GetData(sig.childNodes, "rate")) 
            tr.counter = string.atoi(GetData(sig.childNodes, "sig_counter")) 
            tr.level = GetData(lev.childNodes, "lvl_name") 
            print "have level " + tr.level 
            triggers[tr.name] = tr

    myfL.close()

    return triggers

#----------------------------------------------------------------
#  Write the xml file that defines the triggers 
#----------------------------------------------------------------
def WriteXmlFile(triggers, level, xmlfile) :

    #doc = xml.dom.minidom.Document()

    xmlfile.write("\t<level>\n")
    xmlfile.write("\t\t<lvl_name>%s</lvl_name>\n" %level)
   
    for name, tr in triggers.iteritems() :
        
        xmlfile.write('\t\t<signature>\n')
        xmlfile.write('\t\t\t<sig_name>%s</sig_name>\n' %tr.name)
        xmlfile.write('\t\t\t<sig_counter>%d</sig_counter>\n' %tr.counter)
        xmlfile.write('\t\t\t<evts_passed>%d</evts_passed>\n' %tr.nevt)
        xmlfile.write('\t\t\t<evts_passed_weighted>%d</evts_passed_weighted>\n' %tr.nevt)
        xmlfile.write('\t\t\t<rate>%f</rate>\n' %tr.rate)
        xmlfile.write('\t\t</signature>\n')

    xmlfile.write("\t</level>\n")
        
    #top = doc.createElement("trigger")
    #doc.appendChild(top)

    #for level, trigs in triggers.iteritems() :
    #    lev = doc.createElement("level")
    #    top.appendChild(lev)
    #    lev_name = doc.createElement("lvl_name")
    #    lev_txt  = doc.createTextNode(level)
    #    lev_name.appendChild(lev_txt)
    #    lev.appendChild(lev_name)
    #    
    #    for tr in trigs :
    #        sig = doc.createElement("signature")
    #        sig_name = doc.createElement("sig_name")
    #        sig_txt = doc.createTextNode(tr.name)
    #        sig_name.appendChild(sig_txt)
    #        sig.appendChild(sig_name)

    #        sig_ps = doc.createElement("chain_prescale")
    #        ps_txt = doc.createTextNode(tr.PS) 
    #        sig_ps.appendChild(ps_txt)
    #        sig.appendChild(sig_ps)

    #        sig_pt = doc.createElement("passthrough")
    #        pt_txt = doc.createTextNode(tr.PT) 
    #        sig_pt.appendChild(pt_txt)
    #        sig.appendChild(sig_pt)

    #        lev.appendChild(sig)
    #
    #    

    #doc.writexml(xmlfile, indent='', addindent='\t', newl='\n', encoding='ISO-8859-1')
    
results = []
for input in args : 
    print input
    results.append(ReadXmlFile(input))


triggers = set ()
for res in results :
    for name in res.keys() :
        triggers.add(name)

summed_results = dict()

for tr in triggers :
    summed_results[tr] = Trigger()
    
    nevents = 0
    dt = 0.0
    for res in results :
        nevents += res[tr].nevt
        if res[tr].rate == 0.0 :
            continue
        dt += (res[tr].nevt/res[tr].rate)
        

    if dt == 0.0 :
        summed_results[tr].rate = 0.0
    else :
        summed_results[tr].rate = nevents/dt
    summed_results[tr].nevt = nevents
    summed_results[tr].counter = results[0][tr].counter
    summed_results[tr].name = results[0][tr].name
    summed_results[tr].level = results[0][tr].level

res_L1 = dict()
res_L2 = dict()
res_EF = dict()
for keys, tr in summed_results.iteritems() :

    if tr.level == "L1" :
        res_L1[keys] = tr
    elif tr.level == "L2" :
        res_L2[keys] = tr
    elif tr.level == "EF" :
        res_EF[keys] = tr

xmlfile = open(options.output_dir, "w")
xmlfile.write("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n")
xmlfile.write("<?xml-stylesheet type=\"text/xsl\" href=\"trigger_rates.xsl\"?>\n")
xmlfile.write("<trigger>\n")
WriteXmlFile(res_L1, "L1", xmlfile)  
WriteXmlFile(res_L2, "L2", xmlfile)  
WriteXmlFile(res_EF, "EF", xmlfile)  
xmlfile.write("</trigger>\n")
xmlfile.close()
    
