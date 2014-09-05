# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import xml.dom.minidom
import sys

ps_def = [('L1_MBTS', 10),
          ('L1_EM', 100)]



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
    PS = ""
    PT = ""


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
            tr.PS = GetData(sig.childNodes, "chain_prescale") 
            tr.PT = GetData(sig.childNodes, "passthrough") 
            triggers.setdefault(level, []).append(tr)

    myfL.close()

    return triggers

#----------------------------------------------------------------
#  Write the xml file that defines the triggers 
#----------------------------------------------------------------
def WriteXmlFile(triggers) :

    #doc = xml.dom.minidom.Document()
    xmlfile = open(options.output_dir, "w")

    xmlfile.write("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n")
    xmlfile.write("<?xml-stylesheet type=\"text/xsl\" href=\"trigger_rates.xsl\"?>\n")
    xmlfile.write("<trigger>\n")
   
    for level, trigs in triggers.iteritems() :
        
        xmlfile.write("\t<level>\n")
        xmlfile.write("\t\t<lvl_name>%s</lvl_name>\n" %level)
        for tr in trigs :
            xmlfile.write('\t\t<signature>\n')
            xmlfile.write('\t\t\t<sig_name>%s</sig_name>\n' %tr.name)
            xmlfile.write('\t\t\t<chain_prescale>%s</chain_prescale>\n' %tr.PS)
            xmlfile.write('\t\t\t<passthrough>%s</passthrough>\n' %tr.PT)
            xmlfile.write('\t\t</signature>\n')

        xmlfile.write("\t</level>\n")
    xmlfile.write("</trigger>\n")
        
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
    xmlfile.close()
     
tr_ps = ReadXmlFile(sys.argv[1]) 

for level, list in tr_ps.iteritems() :
    for tr in list :
        for key, ps in ps_def :
            if tr.name.count(key) > 0 :
                tr.PS = ps
    
    

WriteXmlFile(tr_ps)
