#--------------------------------------------------------------------------------
# Config
#--------------------------------------------------------------------------------

# Create an rmap file and populate it with eta\phi regions sliced by Z in a key layer
# run: python keylayer_zslicemap.py path/to/Extrapolation/file.root

# Extrapolation Engine files are stored on eos:
# /eos/atlas/atlascerngroupdisk/det-htt/HTTsim/user/abkahn/ExtrapolationEngine/

# For information on map configuration files see the README in TrigHTTMaps

import argparse
import numpy as np
import ROOT
import time
import sys
import glob

from PyJobTransforms.trfLogger import msg

parser = argparse.ArgumentParser(description='Flattens Wrapper Files')
parser.add_argument('inname', metavar='inputfile', type=str,
                    help='input file name (should be HTT Wrapper File)')
args = parser.parse_args()

# initialize empty sets and lists
phi_pb, phi_pe, phi_sb, phi_se = set(),set(),set(),set()
eta_pb, eta_pe, eta_sb, eta_se = set(),set(),set(),set()
printPB, printPE, printSB, printSE = [[] for i in range(7)],[[] for i in range(7)],[[] for i in range(7)],[[] for i in range(7)] # 7 slices

# create rmap file
rmap = open('alleta_allphi_kl_strip2_ATLAS-P2-ITK-23-00-01.rmap', 'w')

'''
# get list of files
fileList = glob.glob("%s/*.root" % args.inname)
# make TChain
Data = ROOT.TChain("ExtrapolationEngineTest")
# add files to TChain
for file in fileList:
    msg.info("Adding file " + file)
    Data.AddFile(file)
'''

# get single file
f = ROOT.TFile.Open(args.inname)
Data =  f.Get("ExtrapolationEngineTest")

sys.stdout.write("start\n")
sys.stdout.flush()

#--------------------------------------------------------------------------------
# parse geometry
#--------------------------------------------------------------------------------
sys.stdout.write("reading track input\n")
sys.stdout.flush()
t0 = time.clock()

hits = {} # hit dictionary
num_entries = Data.GetEntries()
counter, events, perc  = 0,0,0
for trk in Data:
    events = events + 1
    hit_idx = 0
    for dic_idx in np.arange(counter, counter + len(trk.SensitivePosX)):
        hits[dic_idx] = {}
        hits[dic_idx]["geom"] = trk.SensitiveIsPixel[hit_idx]
        hits[dic_idx]["ec"] = trk.SensitiveBarrelEndcap[hit_idx]
        if trk.SensitiveIsPixel[hit_idx] == 0: #strip
            layer = 2*trk.SensitiveLayerDisc[hit_idx]+trk.SensitiveSide[hit_idx]
        else: #pixel
            layer = trk.SensitiveLayerDisc[hit_idx]
        '''
        # If you want to use the layer scheme in the pixel endcap where each disk is a unique layer:
        if trk.SensitiveIsPixel[hit_idx] == 0: # strip
            layer = 2*trk.SensitiveLayerDisc[hit_idx]+trk.SensitiveSide[hit_idx]
        else:
            if trk.SensitiveBarrelEndcap[hit_idx] == 0: # pixel barrel
                layer = trk.SensitiveLayerDisc[hit_idx]
            else: # pixel endcap
                # number of disks per layer in 23-00-01:  [15, 29, 6, 11, 8, 8, 9, 9] --> [0, 15, 44, 50, 61, 69, 77, 86]
                # number of disks per layer in 22-02-00:  [17, 30, 11, 8, 9] --> [0,17,47,58,66]
                etas_per_layer = [0, 15, 44, 50, 61, 69, 77, 86]
                layer = etas_per_layer[trk.SensitiveLayerDisc[hit_idx]] + trk.SensitiveEtaModule[hit_idx] 
        '''
        hits[dic_idx]["layer"] = layer
        hits[dic_idx]["z"] = trk.SensitivePosZ[hit_idx]
        hit_idx = hit_idx + 1
    # print status
    counter = counter + len(trk.SensitivePosX)
    perc = 100 * float(events)/float(num_entries)
    if events % 5000 == 0:
        sys.stdout.write("%d %% complete\n" % perc)
        sys.stdout.flush()

t1 = time.clock()
sys.stdout.write("parse geom took %d seconds\n" % (t1-t0))
sys.stdout.flush()

#--------------------------------------------------------------------------------
# fill slices
#--------------------------------------------------------------------------------
sys.stdout.write("\nfilling slices\n")
sys.stdout.flush()
t2 = time.clock()

# make key layer dictionary - strip barrel layer 2
kl = {k:v for (k,v) in hits.items() if hits[k]['geom'] == 0 if hits[k]['ec'] == 0 if hits[k]['layer'] == 2}
max_z = max(int(l['z']) for l in kl.values())    
min_z = min(int(l['z']) for l in kl.values())    
slices = np.linspace(min_z,max_z, 7)

# array of slice dictionaries, each one hold the hits in that slice
sliceDicts = [dict() for x in range(6)]

for i in range(6): # loop over slices
    sys.stdout.write("\nstarting slice %d \n" % i)
    sys.stdout.write("%.1f mm < z < %.1f mm \n" % (slices[i],slices[i+1]))
    sys.stdout.flush()
    s = {}
    dic_idx = 0 # index that counts the number of hits stored in each slice dictionary
    for trk in Data:
        fillHits = False

        for hit in range(len(trk.SensitivePosX)): # look for keyhit, then find appropriate slice
            isPix = trk.SensitiveIsPixel[hit]
            bEC = trk.SensitiveBarrelEndcap[hit]
            if isPix == 0: # strip hit                     
                layer = 2*trk.SensitiveLayerDisc[hit]+trk.SensitiveSide[hit]
            else: # pixel hit
                layer = trk.SensitiveLayerDisc[hit]

            z = trk.SensitivePosZ[hit]  
            # if hit is in key layer, add all hits in track to appropriate slice dictionary
            if isPix == 0 and bEC == 0 and layer == 2 and z >= slices[i] and z <= slices[i+1]:
                fillHits = True

        if fillHits == True:
            for hit in range(len(trk.SensitivePosX)):
                s[dic_idx] = {}
                s[dic_idx]["geom"] = trk.SensitiveIsPixel[hit]
                s[dic_idx]["ec"] = trk.SensitiveBarrelEndcap[hit]
                if trk.SensitiveIsPixel[hit] == 0:
                    layer = 2*trk.SensitiveLayerDisc[hit]+trk.SensitiveSide[hit]
                else:
                    layer = trk.SensitiveLayerDisc[hit]
                s[dic_idx]["layer"] = layer
                s[dic_idx]["phi"] = trk.SensitivePhiModule[hit]
                s[dic_idx]["eta"] = trk.SensitiveEtaModule[hit]
                strip = trk.SensitivePhiIndex[hit]
                # calculate row from strip
                if trk.SensitiveIsPixel[hit] == 0 and layer <= 3: #First four sct HTT layers have 4 rows per module
                    if strip >= 0 and strip < 1280:
                        row = 0
                    elif strip >= 1280 and strip < 2560:
                        row = 1
                    elif strip >= 2560 and strip < 3840:
                        row = 2
                    elif strip >= 3840 and strip <= 5120:
                        row = 3 
                    else:
                        row = 999 # so it's obvious if something went wrong
                    s[dic_idx]["eta"] = 4*s[dic_idx]["eta"] + row 
                elif trk.SensitiveIsPixel[hit] == 0 and layer >1: # Outer two ITK sct layers have 2 rows per module
                    if strip >= 0 and strip < 1280:
                        row = 0
                    elif strip >= 1280 and strip <= 2560:
                        row = 1
                    else: 
                        row = -999
                    s[dic_idx]["eta"] = 2*s[dic_idx]["eta"] + row    
                dic_idx = dic_idx + 1
    sliceDicts[i] = s

t3 = time.clock()
sys.stdout.write("Find key hits and fill slices took %d seconds\n" % (t3-t2))
sys.stdout.flush()

#--------------------------------------------------------------------------------
# print map
#--------------------------------------------------------------------------------
for i in range(0,6):

    s = sliceDicts[i]

    #make pixel barrel dict
    pb = {k:v for (k,v) in s.items() if s[k]['geom'] == 1 if s[k]['ec'] == 0}
    #make pixel endcap dict
    pe = {k:v for (k,v) in s.items() if s[k]['geom'] == 1 if s[k]['ec'] != 0}
    #make strip barrel dict
    sb = {k:v for (k,v) in s.items() if s[k]['geom'] == 0 if s[k]['ec'] == 0}
    #make strip endcap dict
    se = {k:v for (k,v) in s.items() if s[k]['geom'] == 0 if s[k]['ec'] != 0}

    try:
        pbmax = max(int(l['layer']) for l in pb.values()) + 1
    except ValueError:
        msg.warning("pixel barrel had no hits in slice %d" % i)
        pbmax = -1
    try:
        pemax = max(int(l['layer']) for l in pe.values()) + 1
    except ValueError:
        msg.warning("pixel endcap had no hits in slice %d" % i)
        pemax = -1
    try:
        sbmax = max(int(l['layer']) for l in sb.values()) + 1
    except ValueError:
        msg.warning("strip barrel had no hits in slice %d" % i)
        sbmax = -1
    try:
        semax = max(int(l['layer']) for l in se.values()) + 1
    except ValueError:
        msg.warning("strip endcap had no hits in slice %d" % i)
        semax = -1

    for lyr in range(max(int(l['layer']+1) for l in s.values())):
        phi_pb.clear()
        phi_pe.clear()
        phi_sb.clear()
        phi_se.clear()
    
        eta_pb.clear()
        eta_pe.clear()
        eta_sb.clear()
        eta_se.clear()

        # Pixel Barrel
        if lyr < pbmax:
            for key in pb:
                if pb[key]['layer'] == (lyr):
                    phi_pb.add(pb[key]["phi"])
                    eta_pb.add(pb[key]["eta"])
            printPB[i].append("1 0 %d %d %d %d %d %d %d" % (lyr,min(phi_pb),max(phi_pb),len(phi_pb),min(eta_pb),max(eta_pb),len(eta_pb)))
        
        # Pixel Endcap
        if lyr < pemax:
            for key in pe:
                if pe[key]['layer'] == (lyr):            
                    phi_pe.add(pe[key]["phi"])
                    eta_pe.add(pe[key]["eta"])
            printPE[i].append("1 1 %d %d %d %d %d %d %d" % (lyr,min(phi_pe),max(phi_pe),len(phi_pe),min(eta_pe),max(eta_pe),len(eta_pe))) 
        
        # Strip Barrel
        if lyr < sbmax:
            for key in sb:
                if sb[key]['layer'] == (lyr):
                    phi_sb.add(sb[key]["phi"])
                    eta_sb.add(sb[key]["eta"])
            printSB[i].append("0 0 %d %d %d %d %d %d %d" % (lyr,min(phi_sb),max(phi_sb),len(phi_sb),min(eta_sb),max(eta_sb),len(eta_sb)))

        #Strip Endcap
        if lyr < semax:
            for key in se:
                if se[key]['layer'] == (lyr):
                    phi_se.add(se[key]["phi"])
                    eta_se.add(se[key]["eta"])
            printSE[i].append("0 1 %d %d %d %d %d %d %d" % (lyr,min(phi_se),max(phi_se),len(phi_se),min(eta_se),max(eta_se),len(eta_se)))

            
    # Print empty pe layers
    if pemax == -1:
        for j in range(4): # hardcoded
            printPE[i].append("0 1 %d 0 0 0 0 0 0" % j)

    # Print empty se layers
    if semax == -1:
        for j in range(12): # hardcoded
            printSE[i].append("0 1 %d 0 0 0 0 0 0" % j)
            

rmap.write("towers 6 phi 16\n\n")

for z in range(0,6):
    rmap.write("\n%d \n" % z)

    for i in range(0,len(printPB[z])):
        rmap.write(printPB[z][i])
        rmap.write("\n")
    for i in range(0,len(printPE[z])):
        rmap.write(printPE[z][i])
        rmap.write("\n")
    for i in range(0,len(printSB[z])):
        rmap.write(printSB[z][i])
        rmap.write("\n")
    for i in range(0,len(printSE[z])):
        rmap.write(printSE[z][i])
        rmap.write("\n")
    
rmap.close()
