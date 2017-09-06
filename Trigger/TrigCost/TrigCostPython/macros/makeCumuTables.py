#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os, sys, string, math, shutil
import xml.dom.minidom
import ROOT



#-----------------------------------------------------------------------
#  Options definitions
#-----------------------------------------------------------------------
from optparse import OptionParser

p = OptionParser(usage='<rates xml> -o <output location>')

p.add_option('-o', '--output', type = 'string', default = './GroupRatesTables.html',
             dest = 'outfile', help = 'output file for html' )

p.add_option('', '--hist-dir', type = 'string', default = 'CumulativeHistograms',
             dest = 'hist_dir', help = 'Directory into which histograms are written' )

p.add_option('-p', '--do-hist', default='False', action='store_true', 
                    dest='do_hist', help='Enable cumulative plots' )

(options, args) = p.parse_args()

sys.argv.append('-b')
    
#---------------------------------------------------------------------
# Object to store rates
class group:
    def __init__(self):
        self.triggers  = []
        self.cumu_group      = dict()
        self.rev_cumu_group  = dict()

    def Print(self):
        print 'name =',self.name
        print '   rate  =',self.rate
        
    name      = ''
    rate      = ''
    triggers  = []
    cumu_group      = dict()
    rev_cumu_group  = dict()
    

class Trigger :

    def __init__(self) :
        pass

    def Print(self) :
        print 'name =',self.name
        print '   rate      =',self.rate
        print '   rate err  =',self.rerr
        print '   PS        =',self.PS
        
    
    name = ''
    PS   = ''
    rate = ''
    rerr = ''
       
#---------------------------------------------------------------------
def ReadXmlFile(filename):

    def GetData(nodelist, name):

        for val in nodelist:
            if val.nodeName != name:
                continue

            svalue = ''
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
    myf = open(filename)
    dom = xml.dom.minidom.parse(myf)    
    
    trig_map = dict()
    group_list = dict() 
    for top in dom.getElementsByTagName('trigger') :
        #Get triggers, store info
        for lev in top.getElementsByTagName('level') :
            for trsig in lev.getElementsByTagName('signature') :
               
                tr = Trigger() 
                tr.name = GetData(trsig.childNodes, 'sig_name')
                tr.rate = GetData(trsig.childNodes, 'rate')
                tr.rerr = GetData(trsig.childNodes, 'rate_err')
                tr.PS   = GetData(trsig.childNodes, 'prescale')

                trig_map[tr.name] = tr
                    


        #Get groups
        for sig in top.getElementsByTagName('cplx_signature'):
            if GetData(sig.childNodes, 'type') == 'Group' :

                gr=group()            

                gr.name    = GetData(sig.childNodes, 'sig_name')
                gr.rate    = string.atof(GetData(sig.childNodes, 'rate'))
               
                group_list[gr.name] = gr
        #Now get cumulative rates
        for sig in top.getElementsByTagName('cplx_signature'):
            if GetData(sig.childNodes, 'type').find('Cumulative') != -1 :

                cumu_name    = GetData(sig.childNodes, 'sig_name')
                group_name = cumu_name[:cumu_name.find('cumu')-1]
                cumu_index = -1
                try :
                    cumu_index = int(cumu_name.split('_')[-1])
                except ValueError :
                    print 'ERROR - Trailing character on cumulative group is not an integer'
                    continue;

                cumutr = group()
                cumutr.name = cumu_name
                cumutr.rate =  GetData(sig.childNodes, 'rate')
               
                #Get triggers in this group
                for comp in sig.getElementsByTagName('components') :
                    for compsig in comp.getElementsByTagName('sig') :
                        cumutr.triggers.append(trig_map[GetData(compsig.childNodes, 'sig_name')])


                if len(cumutr.triggers) != cumu_index + 1 :
                    print 'Error in parsing triggers for group ' + cumu_name
                    continue
                        
                if cumu_name.count('rev') > 0 :
                    group_list[group_name].rev_cumu_group[cumu_index] = cumutr
                else :
                    group_list[group_name].cumu_group[cumu_index] = cumutr
                     
    return group_list, trig_map 

#------------------------------------------------------------------------------------
def WriteTable(file, level, group_list) :

    for gr in group_list.values() :
        
        if gr.name == level :
            continue
        
        if len(gr.cumu_group) != len(gr.rev_cumu_group) :
            print 'Error - Cumulative and CumulativeReverse groups have different sizes'
            continue;
        
        file.write('<a name=%s ><p><b>%s Cumulative Rates</b></p></a>\n' %(gr.name,gr.name))
        
        attrs = ['cumu_group', 'rev_cumu_group']

        for type in attrs :
            file.write('<table border=1 frame=box rules=cols>\n')
            file.write('\t<tr>\n')
            file.write('\t\t<td align=CENTER><b>Trigger Name</b></td>\n')
            file.write('\t\t<td align=CENTER><b>Prescale</b></td>\n')
            file.write('\t\t<td align=CENTER><b>Rate</b></td>\n')
            file.write('\t\t<td align=CENTER><b>Rate Error</b></td>\n')
            file.write('\t\t<td align=CENTER><b>Cumulative Rate</b></td>\n')
            file.write('\t</tr>\n')
       

            for index in range(0, len(getattr(gr, type))) :
                uniq_trig = Trigger()
                #want to print a list of all the triggers in this group with their unique rate
                #each entry is a group containing one more trigger than the last
                #to get the additional trigger make a set differece between this group and the previous
                if index == 0 :
                    uniq_trig = getattr(gr, type)[index].triggers[0]
                else :
                    uniq_trig_set = set(getattr(gr, type)[index].triggers) - set(getattr(gr, type)[index-1].triggers)
                    if len(uniq_trig_set) == 1 :
                        uniq_trig = uniq_trig_set.pop()
                    else :
                        print 'Set difference found multiple triggers'
                        continue
                    
                file.write('\t<tr>\n')
                file.write('\t\t<td align=LEFT><b>%s</b></td>\n' %uniq_trig.name)
                file.write('\t\t<td align=RIGHT><b>%s</b></td>\n' %uniq_trig.PS)
                file.write('\t\t<td align=RIGHT><b>%s</b></td>\n' %uniq_trig.rate)
                file.write('\t\t<td align=RIGHT><b>%s</b></td>\n' %uniq_trig.rerr)
                file.write('\t\t<td align=RIGHT><b>%s</b></td>\n' %getattr(gr, type)[index].rate)
                file.write('\t</tr>\n')
            
            file.write('\t<tr>\n')
            file.write('\t\t<td align=LEFT><b>Total Rate</b></td>\n')
            file.write('\t\t<td align=RIGHT><b></b></td>\n')
            file.write('\t\t<td align=RIGHT><b></b></td>\n')
            file.write('\t\t<td align=RIGHT><b></b></td>\n')
            file.write('\t\t<td align=RIGHT><b>%s</b></td>\n' %gr.rate)
            file.write('\t</tr>\n')
            
            file.write('</table>\n')
        
def MakeCumuPlot(group_list) :

    for gr in group_list.values() :
    
        rateBar = ROOT.TH1F('rates_%s' %gr.name, "", len(getattr(gr, 'cumu_group')), 0, len(getattr(gr, 'cumu_group')))
        cumuBar = ROOT.TH1F('cumu_%s' %gr.name, "", len(getattr(gr, 'cumu_group')), 0, len(getattr(gr, 'cumu_group')))
        can     = ROOT.TCanvas('rates_cumu_%s' %gr.name, 'rates_cumu_%s' %gr.name)
        
        
        rateBar.SetBarOffset(0.1)
        rateBar.SetBarWidth(0.45)
        rateBar.SetFillColor(3)
        rateBar.SetYTitle('Rate (Hz)')
        rateBar.SetStats(0)
    
        cumuBar.SetBarOffset(0.55)
        cumuBar.SetBarWidth(0.45)
        cumuBar.SetFillColor(38)
        cumuBar.SetTitle(gr.name)    
        cumuBar.SetYTitle('Rate (Hz)')
        cumuBar.SetStats(0)
    
        can.SetFillColor(10)
    
        max_label_width = 0
        for index in range(0, len(getattr(gr, 'cumu_group'))) :
            uniq_trig = Trigger()
            #want to print a list of all the triggers in this group with their unique rate
            #each entry is a group containing one more trigger than the last
            #to get the additional trigger make a set differece between this group and the previous
            if index == 0 :
                uniq_trig = getattr(gr, 'cumu_group')[index].triggers[0]
            else :
                uniq_trig_set = set(getattr(gr, 'cumu_group')[index].triggers) - set(getattr(gr, 'cumu_group')[index-1].triggers)
                if len(uniq_trig_set) == 1 :
                    uniq_trig = uniq_trig_set.pop()
                else :
                    print 'Set difference found multiple triggers'
                    continue
        
            if uniq_trig.name in triggers :
                cumuBar.SetBinContent(len(getattr(gr, 'cumu_group'))-index, float(getattr(gr, 'cumu_group')[index].rate))
                rateBar.SetBinContent(len(getattr(gr, 'cumu_group'))-index, float(uniq_trig.rate))
                cumuBar.GetXaxis().SetBinLabel(len(getattr(gr, 'cumu_group'))-index, uniq_trig.name)
                if len(uniq_trig.name) > max_label_width :
                    max_label_width = len(uniq_trig.name) 
        can.SetLeftMargin(max_label_width*1.2/100.0)
        can.SetTopMargin(0.20)
        cumuBar.Draw('hbar2')
        rateBar.Draw('hbar2 same')

        leg = ROOT.TLegend(0.75,0.85,0.95,0.95)
        leg.AddEntry(rateBar, 'Rate', 'f')
        leg.AddEntry(cumuBar, 'Cumulative Rate', 'f' )
        leg.SetFillColor(10)
        leg.Draw()
        can.SaveAs('%s/rates_cumu_%s.root' %(options.hist_dir, gr.name))
#----------------------------------------------------------------------------------------    

if len(sys.argv) == 1 :
    print 'Please indicate a trigger rates xml file'
    sys.exit(-1)

groups, triggers = ReadXmlFile(sys.argv[1])
    
gr_L1 = dict()
gr_L2 = dict()
gr_EF = dict()

for gr in groups.values() :
    start = 0
    if gr.name.find(':') != -1 :
        start = gr.name.find(':')+1
    if gr.name.count('L1', start,start+2) > 0 :
        gr_L1[gr.name] = gr
    elif gr.name.count('L2', start,start+2) > 0 :
        gr_L2[gr.name] = gr
    elif gr.name.count('EF', start,start+2) > 0 :
        gr_EF[gr.name] = gr

html_file = open(options.outfile, 'w')

html_file.write('<html>\n<head>\n')
html_file.write('<TITLE>Cumulative Rate Tables</TITLE>\n')
html_file.write('<body bgcolor=#FFFFFF text=#000000 link=#0000FF vlink=#000080 alink=#800000>\n')
html_file.write('<div align=center>\n')

WriteTable(html_file, 'L1', gr_L1)
WriteTable(html_file, 'L2', gr_L2)
WriteTable(html_file, 'EF', gr_EF)

if options.do_hist :
    MakeCumuPlot(groups)
print "closing html"
html_file.close()

#------------------------------------------------------------------------------------
