# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
import xml.etree.ElementTree as ET
import sys
import string
import ROOT

theApp.EvtMax = 0
printfunc (sys.version)
#tree = ET.parse("tesr.xml")
listhis = [] #list for combine perf his 
listhisIndetG = [] #list for indet global his 
printfunc ("############# reading combine performance ")
tree = ET.parse("/afs/cern.ch/user/y/yunju/working/yunju/public/Rtt_ohpxml/CombPerf.xml")
#tree = ET.parse("ohp.xml")
root = tree.getroot()
for neighbor in root.getiterator('histos'):
    #printfunc (neighbor.attrib)
    #printfunc (neighbor.text)
    listpaths=neighbor.text
    printfunc ("info: %s %s" % (neighbor,listpaths))
    n1 = neighbor.text.split(',')
    printfunc ("list:%s" %n1)
    for stringth in n1:
        printfunc (stringth.find("Global_ISSERVER"))
        if stringth.find("Global_ISSERVER")!=-1:
            finalpath=string.replace(stringth,"${Global_ISSERVER}/${Global_PROVIDER}","run_302347")
            printfunc (stringth.find("MET"))
            if stringth.find("MET")!=-1:
                finalpath=string.replace(finalpath,"run_302347","run_302347/lb_432")
            finalpath2=string.replace(finalpath," ", "")
            finalpath2=string.replace(finalpath2,"\t","")#remove tab 
            finalpath3=string.replace(finalpath2,"\n","")
            listhis.append(finalpath3)        
printfunc (listhis )
printfunc ("############# reading indetglobal ")
treeindetglobal = ET.parse("/afs/cern.ch/user/y/yunju/working/yunju/public/Rtt_ohpxml/globaldq-indet_20_11_0_25_1.xml")
#treeindetglobal = ET.parse("ohp.xml")
rootindetglobal = treeindetglobal.getroot()
for neighbor in rootindetglobal.getiterator('histos'):
    #printfunc (neighbor.attrib)
    #printfunc (neighbor.text)
    listpaths=neighbor.text
    printfunc ("info: %s %s" % (neighbor,listpaths))
    n1 = neighbor.text.split(',')
    printfunc ("list:%s" %n1)
    for stringth in n1:
        printfunc (stringth.find("Global_ISSERVER"))
        if stringth.find("Histogramming-Global-iss/GM-Gatherer-MIG:GlobalMonitoringSegment")!=-1:
            finalpath=string.replace(stringth,"Histogramming-Global-iss/GM-Gatherer-MIG:GlobalMonitoringSegment","run_302347")
            printfunc (stringth.find("${ID_AL_BASE_PATH}"))
            if stringth.find("${ID_AL_BASE_PATH}")!=-1:
                finalpath=string.replace(finalpath,"${ID_AL_BASE_PATH}","ExtendedTracks_NoTriggerSelection") #defined in idgl-options.xml
            printfunc ("info with empty: %s" % ([finalpath]))
            finalpath2=string.replace(finalpath,"\t","")#remove tab
            finalpath2=string.replace(finalpath2," ","")#remove space
            printfunc ("info with out empty: %s" % (finalpath2))
            finalpath3=string.replace(finalpath2,"\n","")
            listhisIndetG.append(finalpath3)        
printfunc (listhisIndetG )
f = ROOT.TFile("Monitor.root")
printfunc ("############# checking combine perf ")
for ith in listhis:
    printfunc ("get his %s" %ith)
    histCount = f.Get(ith)
    if  histCount :
        histCount.Draw()
    else: 
        printfunc ("missing combined perf %s " %ith )


printfunc ("############# checking indetglobal ")
for ith in listhisIndetG:
    printfunc ("get his %s" %ith)
    histCount = f.Get(ith)
    if  histCount :
        histCount.Draw()
    else: 
        printfunc ("missing indet global %s " %ith )

