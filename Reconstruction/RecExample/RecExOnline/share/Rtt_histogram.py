# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import xml.etree.ElementTree as ET
import sys
import string
import ROOT

theApp.EvtMax = 0
print (sys.version)
#tree = ET.parse("tesr.xml")
listhis = [] #list for combine perf his 
listhisIndetG = [] #list for indet global his 
print "############# reading combine performance "
tree = ET.parse("/afs/cern.ch/user/y/yunju/working/yunju/public/Rtt_ohpxml/CombPerf.xml")
#tree = ET.parse("ohp.xml")
root = tree.getroot()
for neighbor in root.getiterator('histos'):
    #print neighbor.attrib
    #print neighbor.text
    listpaths=neighbor.text
    print "info: %s %s" % (neighbor,listpaths)
    n1 = neighbor.text.split(',')
    print "list:%s" %n1
    for stringth in n1:
        print stringth.find("Global_ISSERVER")
        if stringth.find("Global_ISSERVER")!=-1:
            finalpath=string.replace(stringth,"${Global_ISSERVER}/${Global_PROVIDER}","run_302347")
            print stringth.find("MET")
            if stringth.find("MET")!=-1:
                finalpath=string.replace(finalpath,"run_302347","run_302347/lb_432")
            finalpath2=string.replace(finalpath," ", "")
            finalpath2=string.replace(finalpath2,"\t","")#remove tab 
            finalpath3=string.replace(finalpath2,"\n","")
            listhis.append(finalpath3)        
print listhis 
print "############# reading indetglobal "
treeindetglobal = ET.parse("/afs/cern.ch/user/y/yunju/working/yunju/public/Rtt_ohpxml/globaldq-indet_20_11_0_25_1.xml")
#treeindetglobal = ET.parse("ohp.xml")
rootindetglobal = treeindetglobal.getroot()
for neighbor in rootindetglobal.getiterator('histos'):
    #print neighbor.attrib
    #print neighbor.text
    listpaths=neighbor.text
    print "info: %s %s" % (neighbor,listpaths)
    n1 = neighbor.text.split(',')
    print "list:%s" %n1
    for stringth in n1:
        print stringth.find("Global_ISSERVER")
        if stringth.find("Histogramming-Global-iss/GM-Gatherer-MIG:GlobalMonitoringSegment")!=-1:
            finalpath=string.replace(stringth,"Histogramming-Global-iss/GM-Gatherer-MIG:GlobalMonitoringSegment","run_302347")
            print stringth.find("${ID_AL_BASE_PATH}")
            if stringth.find("${ID_AL_BASE_PATH}")!=-1:
                finalpath=string.replace(finalpath,"${ID_AL_BASE_PATH}","ExtendedTracks_NoTriggerSelection") #defined in idgl-options.xml
            print "info with empty: %s" % ([finalpath])
            finalpath2=string.replace(finalpath,"\t","")#remove tab
            finalpath2=string.replace(finalpath2," ","")#remove space
            print "info with out empty: %s" % (finalpath2)
            finalpath3=string.replace(finalpath2,"\n","")
            listhisIndetG.append(finalpath3)        
print listhisIndetG 
f = ROOT.TFile("Monitor.root")
print "############# checking combine perf "
for ith in listhis:
    print "get his %s" %ith
    histCount = f.Get(ith)
    if  histCount :
        histCount.Draw()
    else: 
        print "missing combined perf %s " %ith 


print "############# checking indetglobal "
for ith in listhisIndetG:
    print "get his %s" %ith
    histCount = f.Get(ith)
    if  histCount :
        histCount.Draw()
    else: 
        print "missing indet global %s " %ith 

