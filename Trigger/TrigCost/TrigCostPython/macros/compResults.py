#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from optparse import OptionParser
import os, sys, string, math, shutil

#---------------------------------------------------------------------
# Object to store rates
class chain:
    def __init__(self):
        pass

    def Print(self):
        print 'name =',self.name
        print '   ps       =',self.ps
        print '   pt       =',self.pt
        print '   nevt     =',self.nevt
        print '   nevt_err =',self.nevt_err
        print '   rate     =',self.rate
        print '   rate_err =',self.rate_err
        print '   eff      =',self.eff
        print '   pseff    =',self.pseff
        
    name      = ""
    ps        = -1
    pt        = -1
    nevt      = -1
    nevt_err  = -1
    rate      = -1.0
    rate_err  = -1.0
    eff       = -1
    pseff     = -1
    
#---------------------------------------------------------------------
def ReadXmlFile(filename, key_list, ignore_list):

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
                return svalue.strip()
            else:
                raise Exception('Wrong child list')
            
        raise Exception('Failed to find data')
        return ""

    #copy results to chain objects
    rates={}

    # create and initialize parser
    import xml.dom.minidom
    
    try:
        myf = open(filename)
    except:
        print "Failed to open file:",filename
        return rates

    try:
        dom = xml.dom.minidom.parse(myf)
    except:
        print "Failed to parse file:",filename
        return rates        

    print 'Parsing XML file:',filename

    for level in dom.getElementsByTagName("level"):
        for sig in level.getElementsByTagName("signature"):
            ch=chain()

            ch.name     = GetData(sig.childNodes, "sig_name")
            ch.ps       = string.atof(GetData(sig.childNodes, 'prescale'))
            ch.pt       = string.atof(GetData(sig.childNodes, 'passthrough'))
            ch.nevt     = string.atof(GetData(sig.childNodes, 'evts_passed_weighted'))
            ch.rate     = string.atof(GetData(sig.childNodes, 'rate'))
            ch.rate_err = string.atof(GetData(sig.childNodes, 'rate_err'))
            ch.eff      = string.atof(GetData(sig.childNodes, 'efficiency'))
            ch.pseff    = string.atof(GetData(sig.childNodes, 'prescaled_efficiency'))

            if ch.nevt < 0:
                ch.nevt_err = math.sqrt(ch.nevt)
            else:
                ch.nevt_err = 0
                
            match_chain  = len(key_list) < 1
            ignore_chain = False
            
            for key in key_list:
                if ch.name == key or ch.name.count(key) > 0:
                    match_chain = True
                    break

            for ikey in ignore_list:
                if ch.name == ikey or ch.name.count(ikey) > 0:
                    ignore_chain = True
                    break

            if match_chain and not ignore_chain:
                rates[ch.name]=ch

    return rates

#---------------------------------------------------------------------
# Functions to access ROOT modules
#
def GetCanvas(name, title = ""):

    global options

    if options.text: return None

    import ROOT

    ROOT.gStyle.SetOptStat('rmei')
    ROOT.gStyle.SetFillColor(10)
    ROOT.gStyle.SetFrameFillColor(10)
    ROOT.gStyle.SetCanvasColor(10)
    ROOT.gStyle.SetPadColor(10)
    ROOT.gStyle.SetTitleFillColor(10)
    ROOT.gStyle.SetStatColor(10)
    ROOT.gROOT.SetBatch(True)
    
    canv = ROOT.TCanvas(name, title)
    return canv

def GetHist(name, varname, option):

    if options.text: return None

    import ROOT
    
    if varname == 'rate' and option == 'diff':
        h = ROOT.TH1D(name, name, 100, -20.0, 20.0)
        h.GetXaxis().SetTitle('rate difference (Hz)')
    
    if varname == 'rate' and option == 'ratio':
        h = ROOT.TH1D(name, name, 100, -3.0, 3.0)
        h.GetXaxis().SetTitle('rate difference/statistical error')
    
    if varname == 'nevt' and option == 'diff':
        h = ROOT.TH1D(name, name, 100, -2.0e2, 2.0e2)
        h.GetXaxis().SetTitle('number of events difference')        
    
    if varname == 'nevt' and option == 'ratio':
        h = ROOT.TH1D(name, name, 100, -3.0, 3.0)
        h.GetXaxis().SetTitle('number of events difference/statistical error')
        
    h.GetXaxis().CenterTitle()
    return h

#---------------------------------------------------------------------
# Compare two results
def fillBlanks(var, nblanks):
    rstr = str(var)
    for i in range(len(str(var)), nblanks+1):
        rstr += ' '

    return rstr
    
#---------------------------------------------------------------------
# Compare two results
def CompareRates(opath, level, varname):

    ofile = open(opath+'compare_'+varname+'_'+level+'.html', 'w')
    global newRates, oldRates, options

    ofile.write('<pre><table border="0" width="500">')
    ofile.write('<tr>')
    ofile.write('<th>chain</th>')
    
    if options.show_pspt:
        ofile.write('<th>PS</th>')
        ofile.write('<th>PT</th>')
        
    ofile.write('<th>1</th>')
    if options.show_err1:
        ofile.write('<th>err</th>')
    
    ofile.write('<th>2</th>')
    if options.show_err2:
        ofile.write('<th>err</th>')
        
    ofile.write('<th>1/2</th>')
    ofile.write('<th>1/2 err</th>')

    if options.show_diff:
        ofile.write('<th>diff %\n')
        
    ofile.write('</tr>')
    
    rateTable = []
        
    for newChain in newRates.values():
        if len(newChain.name) < 2 or newChain.name[0:2] != level:
            continue
        
        if newChain.name not in oldRates.keys():
            print "CompareAttr error: chain not found:",newChain.name
            continue

        oldChain = oldRates[newChain.name]

        oldVar = getattr(oldChain, varname)
        oldErr = getattr(oldChain, varname+"_err")
        newVar = getattr(newChain, varname)
        newErr = getattr(newChain, varname+"_err")

        if options.use_mhz:
            oldVar *= 1000
            oldErr *= 1000
            newVar *= 1000
            newErr *= 1000

        if oldVar + newVar > 0.0:
            diffVar = 200.0*(newVar-oldVar)/(oldVar + newVar)
        else:
            diffVar = 0.0

        if oldVar > 0.0 and newVar > 0.0:
            ratioVar = newVar/oldVar
            ratioErr = ratioVar*(newErr/newVar + oldErr/oldVar)
        else:
            ratioVar = 0.0
            ratioErr = 0.0

        if options.latex:
            tdEnd = '\\\\</td>'
            tdStr = '&</td>'
        else:
            tdEnd = '</td>'
            tdStr = '</td>'
            
        lineS = '<tr>'

        if options.latex:
            lineS += '<td>'+newChain.name.replace('_','\\_')+tdStr
        else:
            lineS += '<td>'+newChain.name+tdStr
        
        if options.show_pspt:
            lineS += '<td>'+newChain.ps+tdStr
            lineS += '<td>'+newChain.pt+tdStr
            
        lineS += '<td>'      +'%g'% newVar    +tdStr
        if options.show_err1:
            lineS += '<td>'  +"%g"%(newErr)   +tdStr
            
        lineS += '<td>'      +"%g"%(oldVar)   +tdStr
        if options.show_err2:
            lineS += '<td>'  +"%g"%(oldErr)   +tdStr
            
        if options.show_diff:
            lineS += '<td>'+ "%.2f"%(ratioVar)+tdStr
            lineS += '<td>'+ "%.2f"%(ratioErr)+tdStr
            lineS += '<td>'+"%+.2f"%(diffVar) +tdEnd
        else:
            lineS += '<td>'+ "%.2f"%(ratioVar)+tdStr
            lineS += '<td>'+ "%.2f"%(ratioErr)+tdEnd

        lineS += '</tr>\n'

        rateTable += [ [ newVar, lineS] ]

    rateTable.sort(lambda x, y: cmp(y[0], x[0]))

    for iline in rateTable:
        ofile.write(iline[1])

    ofile.write('</table></pre>')
    ofile.close()
        
#---------------------------------------------------------------------
# Compare two results
def CompareAttr(gpath, level, varname, option):
    
    hname = varname+'_'+option+'_'+level    
    hist = GetHist(hname, varname, option)
    canv = GetCanvas(hname, hname)

    global newRates, oldRates

    for newChain in newRates.values():
        if len(newChain.name) < 2 or newChain.name[0:2] != level:
            continue
        
        if newChain.name not in oldRates.keys():
            print "CompareAttr error: chain not found:",newChain.name
            continue

        oldChain = oldRates[newChain.name]

        if option.count("noPT") > 0 and newChain.pt > 0.0:
            continue

        oldAtt = getattr(oldChain, varname)
        newAtt = getattr(newChain, varname)
        diffAtt = newAtt-oldAtt

        if varname == 'nevt' and oldAtt > 0.0 and math.fabs(diffAtt)/oldAtt > 0.1:
            print newChain.name,':',newAtt,'-',oldAtt,'=',newAtt-oldAtt
 
        if option.count('ratio') > 0:
            oldErr = getattr(oldChain, varname+'_err')
            newErr = getattr(newChain, varname+'_err')
           
            if oldErr > 0.0:
                diffAtt = diffAtt/oldErr
            else:
                pass

            if diffAtt > 3.0 and False:
                ps = newChain.ps
                pt = newChain.pt

        if hist != None:
            hist.Fill(diffAtt)        

    if canv != None:
        canv.cd()
        canv.Draw()
        hist.Draw()
        canv.Update()
        canv.Print(gpath+canv.GetName()+".gif")

#---------------------------------------------------------------------    
# Compare two results
def ComparePS_PT():

    compResult = True
    for newChain in newRates.values():
        if newChain.name not in oldRates.keys():
            print "ComparePS_PT - chain not found:",newChain.name
            compResult = False
            continue

        oldChain = oldRates[newChain.name]
        
        if newChain.ps != oldChain.ps:
            print newChain.name,' PS difference:',newChain.ps,'!=',oldChain.ps
            compResult = False
            continue
        
        if newChain.pt != oldChain.pt:
            print newChain.name,' PT difference:',newChain.pt,'!=',oldChain.pt
            compResult = False            
            continue
        
    return compResult

#---------------------------------------------------------------------        
# Read XML files using function above
if __name__ == "__main__":
    
    p = OptionParser(usage="usage: <XMK rate file> <XML rate file>  -o <output path>")
    
    p.add_option( "-o", type    = "string", default = "",
                  dest = "output_path",
                  help = "path to output directory" )

    p.add_option( "", "--key-list", type    = "string", default = "",
                  dest = "key_list",
                  help = "list of keys for matching chain names" )

    p.add_option( "", "--ignore", type    = "string", default = "EMPTY,BGRP,win",
                  dest = "ignore",
                  help = "list of keys for ignoring chain names" )

    p.add_option( "-t", "--text", action  = "store_true", default = False,
                  dest = "text",
                  help = "produce text only" )

    p.add_option( "", "--show-pspt", action  = "store_true", default = False,
                  dest = "show_pspt",
                  help = "show PS and PT" )

    p.add_option( "", "--show-err1", action  = "store_true", default = False,
                  dest = "show_err1",
                  help = "show error 1" )

    p.add_option( "", "--show-err2", action  = "store_true", default = False,
                  dest = "show_err2",
                  help = "show error 2" )

    p.add_option( "", "--use-mhz", action  = "store_true", default = False,
                  dest = "use_mhz",
                  help = "use mHz unnits" )

    p.add_option( "", "--latex", action  = "store_true", default = False,
                  dest = "latex",
                  help = "make latex table" )

    p.add_option( "", "--show-diff", action  = "store_true", default = False,
                  dest = "show_diff",
                  help = "show difference" )
                
    (options, args) = p.parse_args()

    if len(args) != 2:
        p.error("need path to two input files")    

    if not os.path.isdir(options.output_path):
        os.mkdir(options.output_path.replace(',','-'))
        print "Created output directory:",options.output_path

    if options.key_list == "physics":
        key_list = ['L1_EM3', 'L1_TAU', 'L1_MU', 'L1_XE', 'L1_J', 'L1_TE']
    elif options.key_list == "minbias":
        key_list = ['L1_MBTS']
    else:
        key_list = options.key_list.split(',')
        
        
    newRates = ReadXmlFile(args[0], key_list, options.ignore.split(','))
    oldRates = ReadXmlFile(args[1], key_list, options.ignore.split(','))

    gpath = options.output_path+"/"
    
    shutil.copyfile(args[0], gpath+'TriggerRates_0.xml')
    shutil.copyfile(args[1], gpath+'TriggerRates_1.xml')

    levels = ['L1','L2','EF']
    option = "noPS"
    
    ComparePS_PT()

    for level in levels:
        
        CompareAttr(gpath, level, 'rate', 'diff')
        CompareAttr(gpath, level, 'nevt', 'diff')
        CompareAttr(gpath, level, 'rate', 'ratio')
        CompareAttr(gpath, level, 'nevt', 'ratio')
        
        CompareRates(gpath, level, 'rate')
        CompareRates(gpath, level, 'nevt')
