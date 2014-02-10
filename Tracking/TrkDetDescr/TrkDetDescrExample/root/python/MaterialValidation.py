#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# encoding: utf-8
"""
MaterialValidation.py

Created by Andreas Salzburger on 2009-04-16.
Updated by Wolfgang Lukas     on 2011-04-29.
Copyright (c) 2009-2011 The ATLAS Collaboration. All rights reserved.
"""

import time
import sys,os
import getopt
from TTreeHelper import *
from ROOT import TCanvas,TTree,TProfile,gDirectory,TColor,TH1F,THStack

help_message = '''
The help message goes here.
'''

class Usage(Exception):
    def __init__(self, msg):
        self.msg = msg

def main(argv=None):
    if argv is None:
        argv = sys.argv
    try:
        try:
            opts, args = getopt.getopt(argv[1:], "ho:v", ["help", "output="])
        except getopt.error, msg:
            raise Usage(msg)
    
        # option processing
        for option, value in opts:
            if option == "-v":
                verbose = True
            if option in ("-h", "--help"):
                raise Usage(help_message)
            if option in ("-o", "--output"):
                output = value
    
    except Usage, err:
        print >> sys.stderr, sys.argv[0].split("/")[-1] + ": " + str(err.msg)
        print >> sys.stderr, "\t for help use --help"
        return 2
         
    inputfiles  =''
    inputtrees  =''
    inputparams =''
    saveasfiles =''        
    # argument list given
    for iarg in range(len(argv)) :        
        if argv[iarg].find('files') >= 0:
            inputfiles = argv[iarg]
        elif argv[iarg].find('trees') >= 0:
            inputtrees = argv[iarg]
        elif argv[iarg].find('params') >= 0:
            inputparams = argv[iarg]
        elif argv[iarg].find('saveas') >= 0:
            saveasfiles = argv[iarg]
                
    filelist = splitInputArgs(inputfiles)
    treelist = splitInputArgs(inputtrees)
    parslist = splitInputArgs(inputparams)
    saveaslist = splitInputArgs(saveasfiles)

    # prepare the output directory
    if len(saveaslist) > 0:
        if not os.path.isdir('./output/'):
            os.mkdir('./output/')
        # make the output directory
        outputdir = './output/mval_'+str(time.time())+'/'
        if not os.path.isdir(outputdir):
            os.mkdir(outputdir)
        # save according to the given parameters 
        for saveas in saveaslist :
            # check whether the directory exists
            if not os.path.isdir(outputdir+saveas+'/'):
               os.mkdir(outputdir+saveas+'/')
                        
    # draw numbering    
    numPlots = len(parslist)
    numRows = 1
    numCols = numPlots
    if numPlots > 2 :
        numRows = 2
        numCols = numPlots//2 + numPlots%2
    
    print '[>] Files to be processed :', filelist
    print '[>] Trees to be drawn     :', len(treelist)
    print '[>] Profiles to be made   :', parslist
    print '[>] Going to be saved as  :', saveaslist        
    print '[>] Number of cols/rows   :',numCols,', ',numRows
    
    # get the file and the associated dictionaries
    canvasDict = {}
    canvasTotalDict = {}

    fileNum = 0
    #filename = argv[1]
    for filename in filelist :
        # get the tree selector and the dictionary
        singleSelector = TTreeSelector(filename)
        singleDict     = singleSelector.loadTrees(treelist[0])

        # create the canvas list
        for entry in singleDict :
            iden = entry.lstrip('TVolume_').replace('::','_')
            # create the canvas for this
            if iden not in canvasDict :
                canvas = TCanvas(iden,'TrackingVolume:'+iden,100,100,numCols*300,numRows*300)
                if numCols > 1 :
                    canvas.Divide(numCols,numRows)
                # fill into canvas dictionary
                canvasDict[iden] = canvas
            else :
                canvas = canvasDict[iden]

            # now loop over the parameter list
            pad = 0 
            if len(parslist) > 1 : pad = 1
        
            for par in parslist :
                tree = singleDict[entry]
                plotProfile(canvas,pad,tree,par,fileNum)
                pad += 1

            # save according to the given parameters 
            for saveas in saveaslist :
                canvas.SaveAs(outputdir+saveas+'/'+iden+'.'+saveas)

        # create the canvas list for stacked plots
        for entry in singleDict :
            iden = entry.lstrip('TVolume_').replace('::','_') + '_TOTAL'
            # take only mapped trees as a basis
            if '_UNMAPPED' in entry : continue
            # do stack only if unmapped trees are also available
            if (entry+'_UNMAPPED') not in singleDict : continue
            # create the canvas for this
            if iden not in canvasTotalDict :
                canvas = TCanvas(iden,'TrackingVolume:'+iden,100,100,numCols*300,numRows*300)
                if numCols > 1 :
                    canvas.Divide(numCols,numRows)
                # fill into canvas dictionary
                canvasTotalDict[iden] = canvas
            else :
                canvas = canvasTotalDict[iden]

            # now loop over the parameter list
            pad = 0 
            if len(parslist) > 1 : pad = 1

            for par in parslist :
                tree1 = singleDict[entry]
                tree2 = singleDict[entry+'_UNMAPPED']
                # TODO: also give saveaslist ... or rewrite and only give persistent THStack and save as above
                outpath = outputdir+saveas+'/'+iden+'.'+saveas
                plotStack(canvas,pad,tree1,tree2,par,fileNum,outpath)
                pad += 1


        # fileNum
        fileNum += 1
        print '[>] Next file, switching to same option (',fileNum,')'
                

def splitInputArgs(argstring):
    # split the keyword off
    splitlist = argstring.split('=')
    inputlist = []
    if len(splitlist) is 2 :
        inputlist = splitlist[1].split(",")
    return inputlist
    
    
def plotProfile(canvas,pad,tree,param,fileNum):
    # the name
    tprofname = tree.GetName()+param+str(fileNum)
    # draw it and get it
    tmpCanv = TCanvas(tprofname)
    tmpCanv.cd()
    tree.Draw(param+'>>'+tprofname+'(200)','','prof,goff')

    print '[>] In plotProfile() with option :',fileNum

    if pad > 0:
        canvas.cd(pad)
    else :
        canvas.cd()

    tprofile = gDirectory.Get(tprofname)

#    lower = tprofile.GetBinLowEdge(1)
#    upper = tprofile.GetBinLowEdge(201)
#    print lower,upper
#    h1 = tprofile.ProjectionX('h1')

#    if fileNum == 1 # for full comparison plot with 2 histos
#        tprofile.SetFillColor(5)
#        tprofile.Draw("hist,same")        
#    elif fileNum :
    title = tree.GetName().split('_')[1]
    color = 5
    if '_UNMAPPED' in tree.GetName() :
        title += ' UNMAPPED'
        color = 17
    tprofile.SetTitle(title)
    tprofile.GetXaxis().SetTitle(param.split(':')[1])
    tprofile.GetYaxis().SetTitle(param.split(':')[0])
#    h1.SetTitle(title)
#    h1.GetXaxis().SetTitle(param.split(':')[1])
#    h1.GetYaxis().SetTitle(param.split(':')[0])
    if fileNum :
        tprofile.SetLineColor(fileNum)
        tprofile.SetMarkerColor(fileNum)
        tprofile.SetMarkerStyle(8)
        tprofile.SetMarkerSize(0.35)
        tprofile.Draw("p,same")
#        h1.SetLineColor(fileNum)
#        h1.SetMarkerColor(fileNum)
#        h1.SetMarkerStyle(8)
#        h1.SetMarkerSize(0.35)
#        h1.Draw("p,same")
    else :
        tprofile.SetLineColor(1)
        tprofile.SetFillColor(color)
        tprofile.Draw('hist')
#        h1.SetFillColor(color)
#        h1.Draw('hist')


def plotStack(canvas,pad,tree1,tree2,param,fileNum,outpath):
    # the name
    tprofname1 = tree1.GetName()+param+str(fileNum)
    tprofname2 = tree2.GetName()+param+str(fileNum)
    # they already exist in gDirectory, get them
    tmpCanv = TCanvas(tprofname1+'stack')
    tmpCanv.cd()
    tprofile1 = gDirectory.Get(tprofname1)
    tprofile2 = gDirectory.Get(tprofname2)

    # get the overall upper and lower bin limits
    lower1 = tprofile1.GetBinLowEdge(1)
    lower2 = tprofile2.GetBinLowEdge(1)
#    lower2 = tprofile2.GetBinCenter(1) - tprofile2.GetXAxis.GetBinWidth()/2.
#    upper1 = tprofile1.GetBinCenter(200) + tprofile1.GetXAxis.GetBinWidth()/2.
    upper1 = tprofile1.GetBinLowEdge(201)
    upper2 = tprofile2.GetBinLowEdge(201)

    newmin = min(lower1,lower2)
    newmax = max(upper1,upper2)

    # draw the plots again with new binning
    tprofname1new = tprofname1 + 'rebin'
    tprofname2new = tprofname2 + 'rebin'
    tree1.Draw(param+'>>'+tprofname1new+'(200,'+str(newmin)+','+str(newmax)+')','','prof,goff')
    tree2.Draw(param+'>>'+tprofname2new+'(200,'+str(newmin)+','+str(newmax)+')','','prof,goff')
    tprofile1new = gDirectory.Get(tprofname1new)
    tprofile2new = gDirectory.Get(tprofname2new)

    print '[>] In plotStack() with option :',fileNum

    if pad > 0:
        canvas.cd(pad)
    else :
        canvas.cd()

    # projection to TH1F for stacking
    h1st = tprofile1new.ProjectionX(tprofname1new+'h1')
    h2st = tprofile2new.ProjectionX(tprofname2new+'h2')
    ts = THStack()

    title = tree1.GetName().split('_')[1] + ' TOTAL'
    ts.SetTitle(title)
    print '[>] Stack:', tree1.GetName().split('_')[1], '  newmin =', newmin, '  newmax =', newmax
    h1st.GetXaxis().SetTitle(param.split(':')[1])
    h1st.GetYaxis().SetTitle(param.split(':')[0])
    h2st.GetXaxis().SetTitle(param.split(':')[1])
    h2st.GetYaxis().SetTitle(param.split(':')[0])

    if fileNum :
        h1st.SetLineColor(fileNum)
        h1st.SetMarkerColor(fileNum)
        h1st.SetMarkerStyle(8)
        h1st.SetMarkerSize(0.35)
        h1st.Draw("p,same")
        h2st.SetLineColor(fileNum+10)
        h2st.SetMarkerColor(fileNum+10)
        h2st.SetMarkerStyle(8)
        h2st.SetMarkerSize(0.35)
        h2st.Draw("p,same")
        ts.Add(h1st)
        ts.Add(h2st)
        ts.Draw()
    else :
        h1st.SetLineColor(1)
        h1st.SetFillColor(5)
        h1st.Draw('hist')
        h2st.SetLineColor(1)
        h2st.SetFillColor(17)
        h2st.Draw('hist')
        ts.Add(h1st,'hist')
        ts.Add(h2st,'hist')
        ts.Draw()

    canvas.SaveAs(outpath)


if __name__ == "__main__":
    sys.exit(main())
