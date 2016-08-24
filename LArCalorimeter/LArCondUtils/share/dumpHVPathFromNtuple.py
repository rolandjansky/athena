#!/bin/env python

import os,re
import sys
from os import popen
import string
import commands
import ROOT

def DumpPathology(inf, hvline, outfile, append):

    try:
       if not inf.IsOpen():
          print 'File not open ?'
          return
    except:
          return

    try:
       if append:
          fout=open(outfile,'a')
       else:
          fout=open(outfile,'w')
    except:
          print 'Could not open output file ',outfile
          return

    tree=inf.Get("hv/mytree")
    # start to dump the HV for the asked HVLine, first collect one line per cell
    br_hvline=tree.GetBranch("HVline")
    hvdict={}
    for i in range(0,tree.GetEntries()):
       if i%10000 == 0: print i
       br_hvline.GetEntry(i)
       if tree.HVline != hvline: continue
       tree.GetEntry(i)
       bec=tree.barrel_ec
       side=tree.side
       FT=tree.FT
       slot=tree.slot
       channel=tree.channel
       idlist=[bec,side,FT,slot,channel]
       if str(idlist) in hvdict: continue
       hv=tree.hv
       ihv=int(hv)
       hvlist=[bec,side,FT,slot,channel,ihv]
       hvdict[str(idlist)]=hvlist
    pass
    #print hvdict
    # now write to a file
    if not append: fout.write('/LAR/HVPathologiesOfl/Pathologies\n')
    for k in hvdict.keys():
       ll=hvdict[k]
       if len(ll) != 6:
          print 'Wrong list: ',ll,' continue to next!!!'
          continue
       wstr=str(ll[0])+' '+str(ll[1])+' '+str(ll[2])+' '+str(ll[3])+' '+str(ll[4])+' '+str(hvline/1000)+' '+str(hvline%1000)+' '+str((ihv<<4)&0xFFF0)+'\n'
       fout.write(wstr)
    pass   
    fout.close()
    return

if __name__=="__main__":

    # Get the input parameter
    try:
      infile = sys.argv[1]
      inf = ROOT.TFile(infile,'read')
    except:
       print 'HV ntuple ',infile,' doesnt exist ?'
       sys.exit()

    print 'HV ntuple file: ',infile

    try:
      hvline = int(sys.argv[2])
    except:
       print 'HVline number doesnt exist ?'
       sys.exit()

    if hvline < 1000 or hvline > 400000:
       print 'wrong HV line number: ',hvline
       sys.exit()

    if len(sys.argv) > 3:
       outfile=sys.argv[3]
    else:
       outfile='hvpat.txt'

    print 'Output file: ',outfile

    if len(sys.argv) > 4:
       if int(sys.argv[4])==0: 
          append=False
       else:
          append=True
    else:
       append=False

    print 'Append to out file: ',append

    DumpPathology(inf, hvline, outfile, append)
    inf.Close()

  

