#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ftkutils
import sys
import os.path

    
# setup the environment
name7L = sys.argv[1]
nsubregions = int(sys.argv[2])

try:
    import ROOT
    hasROOT = True
except ImportError :
    print "ROOT library not found, histogram book keeping disabled"
    hasROOT = False

# load the sector from the 1st file (7-layer bank)
print "Load sector in", name7L
sectbank7L = ftkutils.PatternBank(name7L)
sectbank7L.LoadPatterns()
# create a KD-Tree representation for this bank
kdtreebank7L = sectbank7L.BuildKDTree()

# get the subregion ID
regid = None
subid = None
name7L_items = name7L.split("_")
for item in name7L_items :
    if item[0:3] == "reg" :
        regid = int(item.split(".")[0][3:])
        print "Region", regid
    elif item[0:3] == "sub" :
        subid = int(item.split(".")[0][3:])
        print "Sub-region", subid

# set the output filename
name711_conn = os.path.basename(name7L).replace(".patt",".conn")

# strip additional extensions, files are small, compression not needed
if name711_conn[-3:] == "bz2" :
    name711_conn = name711_conn[0:-4]
elif name711_conn[-2:] == "gz" :
    name711_conn = name711_conn[0:-3]
similar_list = open(name711_conn,"w")
print "Output in:", name711_conn

if hasROOT :
    rootfile = ROOT.TFile.Open("relation_histo_reg%d_sub%d.root" % (regid,subid),"recreate")
    histo_nrelations = ROOT.TH1F("histo_nrelations","Relations",
                                8,-.5,7.5);
    histo_nrelations.SetXTitle("# 7L #rightarrow 11L relations")
    histo_nrelations.SetYTitle("# 7L sectors")
    histo_nrelsub = []
    for i in xrange(nsubregions) :
        histo_nrelsub.append(ROOT.TH1F("histo_nrelsub%d" % i, "Relations sub %d" % i, 8,-.5,7.5))

# load sector from the 2nd file (11-layer bank)
sectbank11L = []
kdtreebank11L = []
for i in xrange(nsubregions) :
    name11L = name7L.replace("7L","11L").replace("sub%d" % subid,"sub%d" % i)
    print "Load sector in", name11L
    tmpbank = ftkutils.PatternBank(name11L)
    tmpbank.LoadPatterns()
    sectbank11L.append( tmpbank )
    # create a KD-Tree representation for this bank
    kdtreebank11L.append( tmpbank.BuildKDTree() )

# standard convertion map from 7L to 11L
conv7to11 = [0,1,2,3,None,4,None,5,None,6,None]
inversion_list = [4,6] # list of 7L layer that in 11L ECP map are stereo

# create the elements used to search the similar
tocheck= ftkutils.Pattern(11)
# loop of the reference 7L bank
for pattern7L in sectbank7L.patterns :
    print "Check: ",
    pattern7L.PrintToFile(sys.stdout)
    # expand the 7L in an 11L pattern
    stereoID = [4,6,8,10]
    for i in xrange(11) :
        if conv7to11[i] == None :
            tocheck.ssid[i] = -1
        else :
            tocheck.ssid[i] = pattern7L.ssid[conv7to11[i]]
    # check if the SCT layers relation needs correction
    for i in xrange(4) :
        # check if is is an endcap module and has inverted axial-stereo layers
        if pattern7L.ssid[3+i]%100>=20 and (3+i) in inversion_list :
            offset = 3+2*i # base 11L layer position
            # invert the axial-stereo sequence for this pair
            pair = tocheck.ssid[offset:offset+2]
            tocheck.ssid[offset]   = pair[1]
            tocheck.ssid[offset+1] = pair[0]
            # the stereo module precedes the axial one
            stereoID[i] -= 1
    print "Check using: ",
    tocheck.PrintToFile(sys.stdout)
    print "Stereo layers:", stereoID
    
    nodelist = []
    # loop over all the subregions 11L banks
    for isub in xrange(nsubregions) :
        # return the list of similar patterns
        nodelist_sub = kdtreebank11L[isub].FindNode(tocheck)

        if hasROOT :
            histo_nrelsub[isub].Fill(len(nodelist_sub))
            
        # add the similar pattern in this sector in the global list
        for node in nodelist_sub :
            isin = False
            for iold in xrange(len(nodelist)) :
                if nodelist[iold][1].pattern.coverage < node.pattern.coverage :
                    nodelist[iold:iold] = [[isub,node]]
                    isin = True
                    break
            if not isin :
                nodelist.append([isub,node])

    if hasROOT :
        histo_nrelations.Fill(len(nodelist))
            
    # print the connecton for this 7L pattern
    similar_list.write("%d %d" % (pattern7L.pid, len(nodelist)))
    for simsec in nodelist :
        print "Find: ",
        simsec[1].pattern.PrintToFile(sys.stdout)
        similar_list.write(" %d %d" % (simsec[0],simsec[1].pattern.pid))
        for ip in xrange(4) :
            similar_list.write(" "+str(simsec[1].pattern.ssid[stereoID[ip]]))
    similar_list.write("\n")

if hasROOT :
    rootfile.Write()
    rootfile.Close()
similar_list.close()

