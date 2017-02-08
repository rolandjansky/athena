#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import string
import re
import argparse

from hist_bookkeep_utils import *

parser = argparse.ArgumentParser(
    description='''
    take two files and find overlapping histogram definitions.
    Write out overlapping, different (in 1st file and 2nd file),
    and exclusive (only 1st file, only 2nd file'''
)

parser.add_argument('--xml1',
                    dest='in_xml1',
                    type=open,
                    required=True,
                    help='xml 1 file with histogram definitions')

parser.add_argument('--xml2',
                    dest='in_xml2',
                    type=open,
                    required=True,
                    help='xml 2 file with histogram definitions')

args = parser.parse_args()

## ========================================================
# helpers
## --------------------------------------------------------
def compare_idline(_l1,_l2):
    _t1=[ get_val("id",_l1), get_val("type",_l1)]
    _t2=[ get_val("id",_l2), get_val("type",_l2)]
    check=compare_textrange(_t1,_t2,[],[])
    if ("common"!=check):
        return "error"
    return compare_textrange(_t1,_t2,[],[])
## --------------------------------------------------------

## --------------------------------------------------------
def compare_textrange(_t1,_t2,_r1,_r2):
    if (_r1!=_r2):
        return "rangediff"
    elif (_t1!=_t2):
        return "textdiff"
    else:
        return "common"
## --------------------------------------------------------

## --------------------------------------------------------   
def compare_xyline(_l1,_l2):
    _t1=[get_val("title",_l1)]
    _t2=[get_val("title",_l2)]
    _r1=[get_val("lo",_l1),get_val("hi",_l1),get_val("n",_l1)]
    _r2=[get_val("lo",_l2),get_val("hi",_l2),get_val("n",_l2)]
    return compare_textrange(_t1,_t2,_r1,_r2)
## --------------------------------------------------------

## --------------------------------------------------------
def compare_hbuffs(blist):
    # strip definitions of comments
    _buff1=(get_comm_def(blist[0])[1])
    _buff2=(get_comm_def(blist[1])[1])

    textdiff=False
    rangediff=False

    if not _buff1:
        return [-1,2]
    elif not _buff2:
        return [1,-1]

    test=compare_idline(_buff1[0],_buff2[0])   
    if ("error"==test):
        return [-1,-1]
    elif ("textdiff"==test):
        textdiff=True    

    type1=get_val("type",_buff1[0])
    type2=get_val("type",_buff2[0])

    testx=compare_xyline(_buff1[1],_buff2[1])
    testy=compare_xyline(_buff1[2],_buff2[2])
    if ("error"==testx or "error"==testy):
        return [-1,-1]
    elif ("textdiff"==testx or "textdiff"==testy):
        textdiff=True
    elif ("rangediff"==testx or "rangediff"==testy):
        rangediff=True
    if rangediff:
        return [3,4]
    elif textdiff:
        return [5,6]
    else:
        return [0,-1]
## --------------------------------------------------------

##=========================================================
# main     
infname1=args.in_xml1.name
infname2=args.in_xml2.name

outfname_common=infname1.split(".")[0]+"_"+infname2.split(".")[0]+".xml"
outfname_1only=infname1.split(".")[0]+"_only.xml"
outfname_2only=infname2.split(".")[0]+"_only.xml"
outfname_1diffrange=infname1.split(".")[0]+"_diffrange.xml"
outfname_2diffrange=infname2.split(".")[0]+"_diffrange.xml"
outfname_1difftext=infname1.split(".")[0]+"_difftext.xml"
outfname_2difftext=infname2.split(".")[0]+"_difftext.xml"

print ''
print ' reading histogram xml definitions in ', infname1, infname2
print ' writing histograms only in ', infname1, 'to', outfname_1only
print ' writing histograms only in ', infname2, 'to', outfname_2only
print ' writing histograms range diff in ', infname1, 'to', outfname_1diffrange
print ' writing histograms range diff in ', infname2, 'to', outfname_2diffrange
print ' writing histograms text diff in ', infname1, 'to', outfname_1difftext
print ' writing histograms text diff in ', infname2, 'to', outfname_2difftext
print ' writing histograms common to both to', outfname_common
print ''

outf_common=open(outfname_common, 'w')
outf_1only=open(outfname_1only, 'w')
outf_2only=open(outfname_2only, 'w')
outf_1diffrange=open(outfname_1diffrange, 'w')
outf_2diffrange=open(outfname_2diffrange, 'w')
outf_1difftext=open(outfname_1difftext, 'w')
outf_2difftext=open(outfname_2difftext, 'w')

## --------------------------------------------------------
## helper dictionary for writing out results to the right files
buff_comp_dict={0:outf_common,
                1:outf_1only,2:outf_2only,
                3:outf_1diffrange,4:outf_2diffrange,
                5:outf_1difftext,6:outf_2difftext}

## --------------------------------------------------------
buff1=[]
buff2=[]
## handle all definitions in 1st file:
for line in args.in_xml1:
    if line.strip().startswith("<h"):
        line=weed(line)
        id=get_val("id",line)
        buff1=get_hbuff(id,infname1,True)
        buff2=get_hbuff(id,infname2,True)
        # compare buffers
        buffs=[buff1,buff2]
        compvals=compare_hbuffs(buffs)
        # write contents acc to the outcome of buffer-buffer comparison
        for _val,_buff in zip(compvals,buffs):
            for dkey,dval in buff_comp_dict.iteritems():
                if (_val==dkey):
                    for _bitem in _buff:
                        dval.write(_bitem)
        # clear all the buffers for the next comparison
        buff1=[]
        buff2=[]
## --------------------------------------------------------

## --------------------------------------------------------
## handle all definitions only in 2nd file:
buff1=[]
buff2=[]
for line in args.in_xml2:
    line=weed(line)
    if line.strip().startswith("<h"):
        id=get_val("id",line)
        buff1=get_hbuff(id,infname1,True)
        buff2=get_hbuff(id,infname2,True) 
        if not buff1:
            for _bitem in buff2:
                buff_comp_dict[2].write(_bitem)
        # clear all the buffers for the next comparison
        buff1=[]
        buff2=[]
## --------------------------------------------------------

## --------------------------------------------------------
## all done, clean-up
for key,of in buff_comp_dict.iteritems():
    of.close()

print ''
print 'all done'
print ''
