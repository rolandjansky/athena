#!/usr/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQuerySave.py
# Project: AtlRunQuery
# Purpose: Utility to save pickled dictionary 
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Dec 2, 2008
# ----------------------------------------------------------------
#
# ---------------------------------------------------------------------------------------------------
# Creation of Pickled dictionary for output
# ---------------------------------------------------------------------------------------------------
from __future__ import with_statement, print_function
from CoolRunQuery.AtlRunQueryQueryConfig import QC
from CoolRunQuery.selector.AtlRunQuerySelectorBase import DataKey

import pickle,sys,os
from CoolRunQuery.AtlRunQueryRun import Run

def CreateResultDict( runlist ):

    if len(runlist)==0:
        return {}, {}

    print("Creating result in path '%s'" % QC.datapath )

    # output directory
    if QC.datapath=='' and 'data' not in os.listdir('.'):
        os.mkdir('data')

    # define the header
    header = []
    excludelist = []
    if Run.showrunnr:
        header += [DataKey(x) for x in ['Run', 'Links', '#LB']]
    if Run.showtime:
        header += [DataKey('Start and endtime')]
    if Run.showduration:
        header += [DataKey('Duration')]
    header += [k for k in Run.ShowOrder if k not in excludelist]

    AddUpEvents(runlist)

    SaveResultTxt(runlist, header)

    dic = CreateDic(runlist, header)
    
    dic_basic = CreateDicBasicTypes(dic)

    SaveTypelessPickleResult(dic_basic)

    summary = CreateSummary(dic)

    return dic, summary


def AddUpEvents(runlist):
    # Sum up total event numbers
    for run in runlist:
        try:
            Run.totevents[0] += int(run.result["#Events"])
        except ValueError:
            Run.totevents[1] += 1



def SaveResultTxt(runlist, header):
    # write header to text file
    f = open( '%s/QueryResult.txt' % QC.datapath, 'w' )
    print ("data keys:", ', '.join([h.ResultKey for h in header]), file=f)
    print ('number of runs: %i' % len(runlist), file=f)

    # now get the values for each run and write to file
    for r in runlist:
        line = []
        if Run.showrunnr:
            line += ["%i" % r.runNr, "", "%i" % r.lastlb]
        if Run.showtime:
            line += ["%s" % r.timestr('seconds')]
        if Run.showduration:
            line += ["%s" % r.durationstr()]
        for k in Run.ShowOrder:
            line += [r.data[k.ResultKey]]
        for head,item in zip(header,line):
            if isinstance(item,tuple):
                item = '|'.join([str(x) for x in item])
            print ('%40s: %s' % (head.ResultKey, item), file=f)
        print ('\n', file=f)
    f.close()




def SaveTypelessPickleResult(pdic, filename = 'atlrunquery.pickle'):
    # write pickle output
    pf = open( '%s/atlrunquery.pickle' % QC.datapath, 'wb' )
    try:
        pickle.dump(pdic, pf)
    except Exception as e:
        print ('ERROR: could not pickle results dictionary: "%r"' % e)
        sys.exit(1)
    pf.close()



def CreateDic(runlist, header):
    # create keys for pickle dictionary
    dic = {}
    for r in runlist:
        for k in header:
            if   k == 'Run':
                scontent = r.runNr
            elif k == 'Links':
                scontent = ""
            elif k == '#LB':
                scontent = (r.lastlb, [(lbtime[0]-r.lbtimes[0][0])*1e-9 for lbtime in r.lbtimes] + [ (r.lbtimes[-1][1]-r.lbtimes[0][0])*1e-9 ] )
            elif k == 'Start and endtime':
                scontent = r.timestr('seconds')
            elif k == 'Duration':
                scontent = r.durationstr()
            else:
                scontent = r.data[k.ResultKey]
            dic.setdefault(k,[]).append(scontent)
    return dic


def basic(v):
    if hasattr(v,'pickled'):
        return v.pickled()
    return v

def CreateDicBasicTypes(dic):
    dic_basic = {'Run':dic[DataKey('Run')]}

    for i,r in enumerate(dic_basic['Run']):
        dic_basic[r] = dict([ ( k.pickled(), basic(v[i]) ) for k,v in dic.items()])

    return dic_basic
    



    
def CreateSummary(dic):
    # create summary
    dicsum = {}
    for key, results in dic.items():
        if key.ResultKey in ['SMK','HLT PSK','L1 PSK','TorCurrent','SolCurrent','BGS Key','#LB']:
            continue
        if key.Type==DataKey.DETECTOR:
            continue
        for r in results:
            if key=='Run':
                dicsum.setdefault(key,0)
                dicsum[key] += 1
            elif key.Type == DataKey.STREAM:
                entry = r[0]
                if entry is None or entry.value == 'n.a.':
                    continue
                dicsum.setdefault(key,[0,0])
                dicsum[key][0] += entry.value[0]
                dicsum[key][1] += entry.value[1]
            else:
                try:
                    ir = int(r)
                    if key not in dicsum:
                        dicsum[key] = 0
                    dicsum[key] += ir
                except (ValueError, TypeError):
                    pass
    return dicsum
