# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys, string, math, shutil
import xml.dom.minidom

#-----------------------------------------------------------------------
#  Options definitions
#-----------------------------------------------------------------------
from optparse import OptionParser

p = OptionParser(usage='<rates xml> -o <output location>')

p.add_option('-o', '--output', type = 'string', default = './GroupRatesTables.html',
             dest = 'outfile', help = 'output file for html' )

(options, args) = p.parse_args()
    
#---------------------------------------------------------------------
# Object to store rates
class group:
    def __init__(self):
        pass

    def Print(self):
        print 'name =',self.name
        print '   rate  =',self.rate
        print '   rerr  =',self.rerr
        print '   uniq =',self.uniq
    
    def __eq__(self, rhs) :
        return self.name == rhs
    
    name      = ""
    rate      = ""
    rerr      = ""
    uniq      = ""
    
#---------------------------------------------------------------------
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
    myf = open(filename)
    dom = xml.dom.minidom.parse(myf)    

    group_list = []
    overlap_list = dict()
    for top in dom.getElementsByTagName('trigger'):
        for sig in top.getElementsByTagName('cplx_signature'):
            if GetData(sig.childNodes, 'type') == 'Group' :

                gr=group()            

                gr.name    = GetData(sig.childNodes, 'sig_name')
                gr.rate    = string.atof(GetData(sig.childNodes, 'rate'))
                gr.rerr    = string.atof(GetData(sig.childNodes, 'rate_err'))
                gr.uniq    = string.atof(GetData(sig.childNodes, 'unique_rate'))
              
                print 'adding ' + gr.name + ' to groups' 
                group_list.append(gr)
            if GetData(sig.childNodes, 'type') == 'Overlap' : 

                gr = group()
                gr.rate    = string.atof(GetData(sig.childNodes, 'rate'))

                names = []
                for comp in sig.getElementsByTagName('components') :
                    for gsig in comp.getElementsByTagName('sig') :
                        names.append(GetData(gsig.childNodes, 'sig_name'))
                gr.name = names[-1]
                if names[0].count('BW:') > 0 and names[1].count('BW:') > 0 :
                    overlap_list[(names[0],names[1])] = gr
                
    return group_list, overlap_list 

#------------------------------------------------------------------------------------
def WriteTable(file, level, group_list, key="") :

    if key == "" :
        file.write('<a name=%sFull><p><b>%s Rates</b></p></a>\n' %(level, level))
    else :
        file.write('<a name=%s%s><p><b>%s Rates</b></p></a>\n' %(level, key, level))
    file.write('<table border=1 frame=box rules=cols>\n')
    file.write('\t<tr>\n')
    file.write('\t\t<td align=CENTER><b>Group Name</b></td>\n')
    file.write('\t\t<td align=CENTER><b>Links</b></td>\n')
    file.write('\t\t<td align=CENTER><b>Rate</b></td>\n')
    file.write('\t\t<td align=CENTER><b>Rate Error</b></td>\n')
    file.write('\t</tr>\n')
    level_group = group()
    for gr in group_list :
        if gr.name == level :
            level_group = gr
            continue
        if gr.name.count(key) == 0 or gr.name.count('cumu') > 0:
            continue
        file.write('\t<tr>\n')
        file.write('\t\t<td align=LEFT><a href=GRP_%s.html><b>%s</b></a></td>\n' %(gr.name,gr.name))
        file.write('\t\t<td align=LEFT><a href=CumuRatesTables.html#%s><b>Cumulative Table</b></a></td>\n' %gr.name)
        file.write('\t\t<td align=RIGHT><b>%s</b></td>\n' %gr.rate)
        file.write('\t\t<td align=RIGHT><b>%s</b></td>\n' %gr.rerr)
        file.write('\t</tr>\n')
        
    file.write('\t<tr>\n')
    file.write('\t\t<td align=LEFT><b>Total %s</b></td>\n' %level_group.name)
    file.write('\t\t<td align=LEFT></td>\n')
    file.write('\t\t<td align=RIGHT><b>%s</b></td>\n' %level_group.rate)
    file.write('\t\t<td align=RIGHT><b>%s</b></td>\n' %level_group.rerr)
    file.write('\t\t<td align=RIGHT><b></b></td>\n')
    file.write('\t</tr>\n')
        
    file.write('</table>\n')

#------------------------------------------------------------------------------------
def WriteUniqueTable(file, level, group_list) :

    order = [] 
    if level == 'L1' :
        order = [ ('Tau' , ['Tau']),
                  ('EM' ,  ['EM']),
                  ('Muon',  ['Muon']), 
                  ('Jet' ,  ['Jet', 'FJet']),
                  ('Energy',  ['MissingEt', 'TotalEnergy', 'JetEnergy']),
                  ('MinBias', ['Random'])]
    else :
        order = [  ('Tau',   ['Tau']), 
                   ('Egamma',    ['Electron', 'EM', 'Photon']),
                   ('Muon',  ['Muon']),
                   ('Jet',   ['Jet', 'FJet']),
                   ('Met',  ['MissingEt', 'TotalEnergy', 'JetEnergy']),
                   ('BJet',  ['BJet']),
                   ('BPhys',  ['BPhys'] ) ]
                   
        
    group_types = ['Single', 'Multi', 'Combined']
    
    file.write('<a name=%sUnique><p><b>%s Bandwidth Rates</b></p></a>\n' %(level, level))
    file.write('<table border=1 RULES=GROUPS FRAME=BOX>\n')
    file.write('\t<tr>\n')
    file.write('\t\t<td align=CENTER><b>Group Name</b></td>\n')
    file.write('\t\t<td align=CENTER><b>Links</b></td>\n')
    file.write('\t\t<td align=CENTER><b>Rate</b></td>\n')
    file.write('\t\t<td align=CENTER><b>Rate Error</b></td>\n')
    file.write('\t</tr>\n')
    level_group = group()
    bw_groups = []
    used_groups = []
    for bwtype, supergroups in order :
        bw_groups.append((bwtype,[]))
        index = bw_groups.index((bwtype,[]))
        for groups in supergroups :
            for type in group_types :
                for gr in group_list :
                    if gr.name.count(groups) > 0 and gr.name.count(type) > 0 and gr.name.count('RATE') == 0 and gr.name.count('cumu') == 0:
                        if gr not in used_groups :
                            bw_groups[index][1].append(gr)
                            used_groups.append(gr)

    
    for supername, groups in bw_groups :
        file.write('\t<TBODY>\n')
        for gr in groups :
            file.write('\t<tr>\n')
            file.write('\t\t<td align=LEFT><a href=GRP_%s.html><b>%s</b></a></td>\n' %(gr.name,gr.name))
            file.write('\t\t<td align=LEFT><a href=CumuRatesTables.html#%s><b>Cumulative Table</b></a></td>\n' %gr.name)
            file.write('\t\t<td align=RIGHT><b>%s</b></td>\n' %gr.rate)
            file.write('\t\t<td align=RIGHT><b>%s</b></td>\n' %gr.rerr)
            file.write('\t</tr>\n')
        file.write('\t</TBODY>\n') 
        
    file.write('</table>\n')

def WriteOverlapTable(file, level, olap_list) :

    file.write('<a name=%sOverlap><p><b>%s Overlap Rates</b></p></a>\n' %(level,level))
    file.write('<table border=1 frame=box rules=all>\n')
    file.write('\t<tr>\n')
    file.write('\t\t<td align=CENTER><b>Triggers</b></td>\n')
    group_list = set()
    for name1, name2 in olap_list.keys() :
        #if name1.count('BW:') > 0 :
        group_list.add(name1)
        #if name2.count('BW:') > 0 :
        group_list.add(name2)
        print 'adding ' + name1 
        
    for name in group_list :
        file.write('\t\t<td align=CENTER><b>%s</b></td>\n' %name)
        
    file.write('\t</tr>\n')
   
    counter = 0
    for name1 in group_list : 
        file.write('\t\t<td align=LEFT><b>%s</b></td>\n' %name1)
        for name2 in group_list :
            if name1 == name2 :
                file.write('\t\t<td></td>\n')
                continue 
            oname1 = name1
            oname2 = name2
            if (name1, name2) not in olap_list.keys() :
                oname1 = name2
                oname2 = name1
            if (oname1, oname2) not in olap_list.keys() :
                file.write('\t\t<td align=RIGHT></td>\n')
                print 'no overlap for ' + oname1 + ' and ' + oname2    
                continue
            gr = olap_list[(oname1,oname2)]
            file.write('\t\t<td align=RIGHT><b>%.02f</b></td>\n' %float(gr.rate))

        file.write('\t</tr>\n')        
    file.write('</table>\n')


if len(sys.argv) == 1 :
    print 'Please indicate a trigger rates xml file'
    sys.exit(-1)

groups, overlaps = ReadXmlFile(sys.argv[1])
    
gr_L1 = []
gr_L2 = []
gr_EF = []

for gr in groups :
    start = 0
    if gr.name.find(':') != -1 :
        start = gr.name.find(':')+1
    if gr.name.count('L1',start,start+2) > 0 :
        gr_L1.append(gr)
    elif gr.name.count('L2',start,start+2) > 0 :
        gr_L2.append(gr)
    elif gr.name.count('EF',start,start+2) > 0 :
        gr_EF.append(gr)

print 'size of overlaps = ' + str(len(overlaps))
ol_L1 = dict()
ol_L2 = dict()
ol_EF = dict()
for name, olap in overlaps.iteritems() :
    start = 0
    if name[0].find(':') != -1 :
        start = name[0].find(':')+1
    if name[0].count('L1',start,start+2) > 0 :
        ol_L1[name] = olap
    elif name[0].count('L2',start,start+2) > 0 :
        ol_L2[name] = olap
    elif name[0].count('EF',start,start+2) > 0 :
        ol_EF[name] = olap
            

html_file = open(options.outfile, 'w')

html_file.write('<html>\n<head>\n')
html_file.write('<TITLE>Rate Tables</TITLE>\n')
html_file.write('<body bgcolor=#FFFFFF text=#000000 link=#0000FF vlink=#000080 alink=#800000>\n')
html_file.write('<div align=\'center\'>\n')

html_file.write('<a href=#L1BW:>Level 1 Bandwitdh Group Table</a><br />\n')
html_file.write('<a href=#L2BW:>Level 2 Bandwidth Group Table</a><br />\n')
html_file.write('<a href=#EFBW:>Event Filter Bandwidth Group Table</a><br />\n')
html_file.write('<a href=#L1Overlap>Level 1 Bandwidth group overlaps Table</a><br />\n')
html_file.write('<a href=#L2Overlap>Level 2 Bandwidth group overlaps Table</a><br />\n')
html_file.write('<a href=#EFOverlap>Event Filter Bandwidth group overlaps Table</a><br />\n')
html_file.write('<a href=#L1Unique>Level 1 Group Table by Bandwitdh Group</a><br />\n')
html_file.write('<a href=#L2Unique>Level 2 Group Table by Bandwitdh Group</a><br />\n')
html_file.write('<a href=#EFUnique>Event Filter Group Table by Bandwitdh Group</a><br />\n')
html_file.write('<a href=#L1Full>Level 1 Group Table</a><br />\n')
html_file.write('<a href=#L2Full>Level 2 Group Table</a><br />\n')
html_file.write('<a href=#EFFull>Event Filter Group Table</a><br />\n')

WriteTable(html_file, 'L1', gr_L1, 'BW:')
WriteTable(html_file, 'L2', gr_L2, 'BW:')
WriteTable(html_file, 'EF', gr_EF, 'BW:')
WriteOverlapTable(html_file, 'L1', ol_L1)
WriteOverlapTable(html_file, 'L2', ol_L2)
WriteOverlapTable(html_file, 'EF', ol_EF)
WriteUniqueTable(html_file, 'L1', gr_L1)
WriteUniqueTable(html_file, 'L2', gr_L2)
WriteUniqueTable(html_file, 'EF', gr_EF)
WriteTable(html_file, 'L1', gr_L1)
WriteTable(html_file, 'L2', gr_L2)
WriteTable(html_file, 'EF', gr_EF)

html_file.close()


