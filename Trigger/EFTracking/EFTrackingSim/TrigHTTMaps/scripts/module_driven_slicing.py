#----------------------------------------------------------------------
# Imports with Arg Parsing before ROOT import because of help conflict
#----------------------------------------------------------------------
from __future__ import print_function
import argparse

parser = argparse.ArgumentParser(description='Flattens Wrapper Files')
parser.add_argument('inname', metavar='inputfile', type=str, nargs='+',
                    help='input file name (should be HTT Wrapper File)')
parser.add_argument('--evts', action='store', type=int, 
                    help='number of events to process', default=(1<<30)) # default is large integer
parser.add_argument('--trim', action='store', type=float, 
                    help='trim modules with less than given fraction of tracks', default=None) # default is large integer
parser.add_argument('--nslices', action='store', type=int, 
                    help='number of slices to build', default=6) 
parser.add_argument('--keylayer', action='store', type=str, 
                    help='keylayer as det,bec,layer', default="strip,barrel,2")
parser.add_argument('--keylayer2', action='store', type=str, 
                    help='second keylayer subdivides slices', default=None)
parser.add_argument('--outfilelabel', action='store', type=str, 
                    help='label to start outfile with', default="Unlabled")
parser.add_argument('--etarange', action='store', type=str, 
                    help='range of eta for slice', default="-100,100")
parser.add_argument('--fullgran',  action='store_true')
parser.add_argument('--test1slice',  action='store_true')

args = parser.parse_args()


# imports
import ROOT
import time
import sys
import glob
import os
from collections import defaultdict
import bisect

# Setup core logging here
from PyJobTransforms.trfLogger import msg



#----------------------------------------------------------------------
#  Handy tree of dictionary tools
#----------------------------------------------------------------------
defaultdictlambda=lambda : defaultdict(defaultdictlambda) # amazing self referential lambda

def transformNodes(tree,func):
    retv={}
    for key,val in tree.items():        
        if 'dict' in type(val).__name__:
            retv[key]=transformNodes(val,func)
        else:
            if 'list' in type(val).__name__:
                retv[key]=[func(subval) for subval in val]
            else:
                retv[key]=func(val)
    return retv


#----------------------------------------------------------------------
#  Constants
#----------------------------------------------------------------------
dets=["pixel","strip"]
regs=["barrel","endcap"]
dirs=["phi","eta"]

int2det = {1:"pixel", 0:"strip"}
det2int = {v:k for k,v in int2det.items()}
int2reg = {0:"barrel", 2:"endcap"}
reg2int = {v:k for k,v in int2reg.items()}

# Number of rows per module
# diction levels: bec, layer, etamod
striprows={ 'barrel' : {0:[None]+[4]*14,
                        1:[None]+[4]*14,
                        2:[None]+[4]*14,
                        3:[None]+[4]*14,
                        4:[None]+[2]*14,
                        5:[None]+[2]*14,
                        6:[None]+[2]*14,
                        7:[None]+[2]*14},
            'endcap' : {i:[4,4,2,4,2,2] for i in range(12)}           
}

# Number of chips per row
# diction levels: bec, etamod, row  
abcchipsperrow={ 'barrel' : {i+1:[10,10,10,10] for i in range(14)}, # actually 2 rows for some but doesn't matter
                 'endcap' : {0:[8,8,9,9],
                             1:[10,10,11,11],
                             2:[12,12],
                             3:[14,14],
                             4:[8,8],
                             5:[9,9]} }

stripsperabc=128
stripsperrow=transformNodes(abcchipsperrow, lambda x: x * stripsperabc)

rowbounds={}
for reg in stripsperrow:
    rowbounds[reg]={}
    for mod in stripsperrow[reg]:
        rowbounds[reg][mod]=[sum(stripsperrow[reg][mod][0:i+1]) for i in range(len(stripsperrow[reg][mod]))]


#guess geom from input file name
geom="Unknown"
if "ITK-22" in args.inname:
    geom="ITK-22"
elif "ITK-23" in args.inname:
    geom="ITK-23"

# this is weird, number of layers in pixel endcaps?
layers={ "pixel": { "barrel":5, "endcap": 8 if geom=="ITK-22" else 8 }, "strip" : { "barrel":8, "endcap":12}}


#histograms
hists={}

#easy hist booking
def getHist(name,makehist):
    if not name in hists:
        hists[name]=makehist(name)
    return hists[name]


#----------------------------------------------------------------------
#  Open file(s) for reading
#----------------------------------------------------------------------
# get list of files
fileList=args.inname
if len(args.inname)==1 and os.path.isdir(args.inname[0]):    
    fileList = glob.glob("%s/*.root" % args.inname)

# make TChain
Data = ROOT.TChain("ExtrapolationEngineTest")
# add files to TChain
for file in fileList:
    msg.info("Adding file %s" %(file))
    Data.AddFile(file)

num_entries = min(args.evts,Data.GetEntries())
msg.info("reading track input: %d entries\n" % (num_entries))
t0 = time.clock()
    
#----------------------------------------------------------------------
#  Parse keylayer description from input args
#----------------------------------------------------------------------

def parseKeyLayer(keystring):
    key={}
    key_det,key_reg,key_lyr = keystring.split(",")
    key["lyr"]=int(key_lyr)
    key["det"]=det2int[key_det]
    key["reg"]=reg2int[key_reg]
    return key
        
key1 = parseKeyLayer(args.keylayer)
key2 = None
if args.keylayer2:
    key2 = parseKeyLayer(args.keylayer2)

#----------------------------------------------------------------------
#  Parse eta range description from input args
#----------------------------------------------------------------------
min_eta,max_eta = [float(s) for s in args.etarange.split(",")]


#----------------------------------------------------------------------
#  Functions for geometry conversion
#----------------------------------------------------------------------
def getLayer(trk,hit_idx):
    if trk.SensitiveIsPixel[hit_idx] == 0: #strip
        layer = 2*trk.SensitiveLayerDisc[hit_idx]+trk.SensitiveSide[hit_idx]
    else: #pixel
        layer = trk.SensitiveLayerDisc[hit_idx]
    return layer

def getRow(trk,hit_idx): # returns localrow,globalrow

    strip = trk.SensitivePhiIndex[hit_idx]
    bec=int2reg[abs(trk.SensitiveBarrelEndcap[hit_idx])]
    layer=getLayer(trk,hit_idx)
    etamod=trk.SensitiveEtaModule[hit_idx]
    localrow,globalrow=None,None    
    r=trk.SensitivePosR[hit_idx]
    z=trk.SensitivePosZ[hit_idx]
    
    # calculate row from strip
    if trk.SensitiveIsPixel[hit_idx] == 0:

        if not "strip_number" in hists:
            hists["strip_number"]=ROOT.TH2D("strip_number_hist",";module;strip",500,0,500,800,0,8000)
        hists["strip_number"].Fill(reg2int[bec]*100+layer*10+etamod,strip)    

        # this is a binned lookup to get the row number
        localrow = bisect.bisect_left(rowbounds[bec][abs(etamod)],strip)        
        
        rows_in_module=striprows[bec][layer][abs(etamod)]
        
        if localrow > rows_in_module:
           msg.error("Error row(%d) greater than number of rows in module %d" %(localrow,rows_in_module))
        globalrow=rows_in_module*etamod+localrow


        if globalrow==6:
            if not "z_mod6" in hists:
                hists["z_mod6"]=ROOT.TH2D("z_mod6",";z;lyr",2500,0,500,10,0,10)
            hists["z_mod6"].Fill(z,layer)
        
    else:        
        localrow=trk.SensitiveEtaModule[hit_idx]
        globalrow=localrow
        getHist("pixel_rz_layer"+str(layer),
                lambda n: ROOT.TH2D(n,";z[mm]; r[mm]",3000,0,+3000,1000,0,1000)).Fill(z,r)
         
    return localrow,globalrow

#--------------------------------------------------------------------------------
# Find module of track in key layer
#--------------------------------------------------------------------------------

def isKeyLayer(trk,hit_idx,key):
    if key["det"]!=trk.SensitiveIsPixel[hit_idx]:
        return False
    if key["reg"]!=abs(trk.SensitiveBarrelEndcap[hit_idx]):
        return False
    if getLayer(trk,hit_idx)!=key["lyr"]:
        return False
    return True

    
def findKeyLayerMod(trk,key):
    # return None for events with two hits in keylayer
    mixedrows=False
    row=None
    for hit_idx in range(len(trk.SensitivePosX)):
        if isKeyLayer(trk,hit_idx,key):
            rownew= getRow(trk,hit_idx)[1]
            if row and (rownew!=row):
                mixedrows=True
            row = rownew
    return row if not mixedrows else None




def trackToModules(trk):
    modules = defaultdict(defaultdictlambda) # indexed on layer

    for hit_idx in range(len(trk.SensitivePosX)):
        det = trk.SensitiveIsPixel[hit_idx]
        reg = abs(trk.SensitiveBarrelEndcap[hit_idx])
        lyr = getLayer(trk,hit_idx)
        row = getRow(trk,hit_idx)[1]
        phimod = trk.SensitivePhiModule[hit_idx]
        r=trk.SensitivePosR[hit_idx]
        z=trk.SensitivePosZ[hit_idx]
        if reg==reg2int['barrel']:
            if not lyr in modules[det][reg] or r < modules[det][reg][lyr][3]:
                modules[det][reg][lyr]=(row,phimod,hit_idx,r,z)
        else:
            if not lyr in modules[det][reg] or z < modules[det][reg][lyr][4]:
                modules[det][reg][lyr]=(row,phimod,hit_idx,r,z)

    for reg in modules[det2int['strip']]:
        if reg==reg2int['barrel']:
            varidx=3
        else:
            varidx=4
        
        varlast=10000 # large value
        lastlyr=10000 # large value
        for lyr in sorted(modules[det][reg].keys(),reverse=True):
            if varlast < modules[det][reg][lyr][varidx]:
                del modules[det][reg][lyr]
            elif lyr+1==lastlyr and lyr%2==0 and (varlast-modules[det][reg][lyr][varidx]) > 10.0: 
                del modules[det][reg][lyr+1] # remove outer hit
            else:
                lastlyr=lyr
                varlast=modules[det][reg][lyr][varidx]
                    
            
    return modules


#--------------------------------------------------------------------------------
# Parse Geometry
#--------------------------------------------------------------------------------


def parseGeometry():
    eta_vals_for_key_layer=set()
    events=0
    tracks_used=0
    for trk in Data:
        events = events + 1
        if events>args.evts:
            break

        # only process tracks in region
        if trk.StartEta > max_eta or trk.StartEta < min_eta:
            continue

        modules = trackToModules(trk)
        if not key1["lyr"] in modules[key1["det"]][key1["reg"]]:
            # no hit in layer
            continue

        eta_vals_for_key_layer.add(modules[key1["det"]][key1["reg"]][key1["lyr"]][0])

        tracks_used+=1
        
    msg.info("%d eta rows:",len(eta_vals_for_key_layer))
    msg.info(sorted(eta_vals_for_key_layer))
    msg.info("Tracks Used = %d", %(tracks_used))
    return eta_vals_for_key_layer


    
#--------------------------------------------------------------------------------
# Define key layer in terms of modules
#--------------------------------------------------------------------------------
def divideKeyLayerModulesIntoSlices(eta_vals_for_key_layer):
    key_modules_per_slice = float(len(eta_vals_for_key_layer))/nslices
    key_modules_for_slices=[[]]

    for i,row in enumerate(sorted(eta_vals_for_key_layer)):
        if i >= len(key_modules_for_slices)*key_modules_per_slice:
            key_modules_for_slices.append([])
        key_modules_for_slices[-1].append(row)

    msg.info("Modules in slices %d",%(key_modules_for_slices))


    
    # make a map of modules to slices
    return {m:i for i,s in enumerate(key_modules_for_slices) for m in s }



#--------------------------------------------------------------------------------
# Build List of Modules in Slices
#--------------------------------------------------------------------------------

def getModuleToSliceMap(mod_slice_map):
    slice_modules=defaultdict(lambda :
                              { det:{
                                  reg:{
                                      lyr:{
                                          dir:defaultdict(lambda:0) for dir in dirs}
                                      for lyr in range(layers[det][reg]) }
                                  for reg in regs }
                                for det in dets })

    slice_hists={}
    slice_tracks=defaultdict(lambda:0)
    sliceid2int={}

    events=0
    tracks_used=0
    missing_keylayer_hit=0
    for trk in Data:
        events = events + 1
        if events>args.evts:
            break

        # only process tracks in region
        if trk.StartEta > max_eta or trk.StartEta < min_eta:
            continue

        modules = trackToModules(trk)

        if not key1["lyr"] in modules[key1["det"]][key1["reg"]]:
            missing_keylayer_hit+=1
            continue
        mod1 = modules[key1["det"]][key1["reg"]][key1["lyr"]][0]
        slicenum = mod_slice_map[mod1]
        if args.test1slice and slicenum!=0:
            continue
            
        if key2:
            if not key2["lyr"] in modules[key2["det"]][key2["reg"]]:
                missing_keylayer_hit+=1
                continue
            mod2 = modules[key2["det"]][key2["reg"]][key2["lyr"]][0]
            slicenum=(slicenum,mod2)



        # add other modules to slice
        for det in modules:
            for reg in modules[det]:
                for lyr in modules[det][reg]:
                    modidxs=modules[det][reg][lyr]
                    slice_modules[slicenum][int2det[det]][int2reg[reg]][lyr]["eta"][modidxs[0]]+=1
                    slice_modules[slicenum][int2det[det]][int2reg[reg]][lyr]["phi"][modidxs[1]]+=1

                    hit_idx = modidxs[2]

                    if not slicenum in slice_hists:
                        sliceid2int[slicenum]=len(slice_modules)-1
                        slice_hists[slicenum]=ROOT.TH2D("rz"+str(sliceid2int[slicenum]),"rz"+str(slicenum)+"; z[mm]; r[mm]",800,-200,+600,1000,0,1000)
                    slice_hists[slicenum].Fill(trk.SensitivePosZ[hit_idx],trk.SensitivePosR[hit_idx])

                    hname=int2det[det]+"_"+int2reg[reg]+"_"+str(lyr)+"_"+str(modidxs[0])
                    if not hname in slice_hists:
                        slice_hists[hname]=ROOT.TH1D(hname,hname+"; z[mm]",800,-200,+600)
                    slice_hists[hname].Fill(trk.SensitivePosZ[hit_idx])


        tracks_used+=1
        slice_tracks[slicenum]+=1

    msg.info("Tracks Used=%d" %(tracks_used))
    msg.info("%d tracks with no keylayer hit found or multiple hits in keylayer" %(int(str(missing_keylayer_hit))))

    for hist in slice_hists.values():
        hist.Write()
    
    return slice_modules,sliceid2int,slice_tracks



#--------------------------------------------------------------------------------
# Trimming
#--------------------------------------------------------------------------------

def trimSlice(slice_modules,slice_tracks):

    for sliceid in slice_modules:
        for det in dets:
            for reg in regs:
                for lyr in slice_modules[sliceid][det][reg]:
                    for mod in list(slice_modules[sliceid][det][reg][lyr]["eta"]):
                        if slice_modules[sliceid][det][reg][lyr]["eta"][mod]/float(slice_tracks[sliceid]) < args.trim:
                            del  slice_modules[sliceid][det][reg][lyr]["eta"][mod]


#--------------------------------------------------------------------------------
# Write RMap File
#--------------------------------------------------------------------------------
def writeRMap(slice_modules,sliceid2int):
    nslices=len(sliceid2int)
    rmap = open(args.outfilelabel+'_KeyLayer-%s%s%s_NSlices-%d.rmap' % (
        args.keylayer.replace(",","_"),
        "_KeyLayer2-"+args.keylayer2.replace(",","_") if args.keylayer2 else "",
        ("_trim_%1.3f" % (args.trim,)).replace(".","_") if args.trim else "",
        nslices), 'w')

    rmap.write("towers "+str(nslices)+" phi 16\n\n")

    int2sliceid={v:k for k,v in sliceid2int.items()}
    for i in range(len(slice_modules)):
        rmap.write("\n%d \n" % i)
        sliceid=int2sliceid[i]

        mindefault = lambda x,default=0: min(x) if len(x) else default
        maxdefault = lambda x,default=0: max(x) if len(x) else default

        for det in dets:
            for reg in regs:
                for lyr in range(layers[det][reg]):
                    mlen = slice_modules[sliceid][det][reg][lyr]["phi"] # just need if non-zero
                    rmap.write("%d %d %d %d %d %d %d %d %d\n" % (
                        1 if det=="pixel" else 0,
                        1 if reg=="endcap" else 0,
                        lyr,
                        mindefault(slice_modules[sliceid][det][reg][lyr]["phi"].keys(),default=0),
                        maxdefault(slice_modules[sliceid][det][reg][lyr]["phi"].keys(),default=0),
                        len(slice_modules[sliceid][det][reg][lyr]["phi"]),
                        mindefault(slice_modules[sliceid][det][reg][lyr]["eta"].keys(),default=0),
                        maxdefault(slice_modules[sliceid][det][reg][lyr]["eta"].keys(),default=0),
                        len(slice_modules[sliceid][det][reg][lyr]["eta"])))


    rmap.close()
    
#--------------------------------------------------------------------------------
# Estimate duplication
#--------------------------------------------------------------------------------
def printOccupancies(slice_modules):

    duplication_count={
        det:{
            reg:{
                lyr:defaultdict(lambda: 0) for lyr in range(layers[det][reg]) }
            for reg in regs }
        for det in dets }

    for sliceid in slice_modules:
        for det in slice_modules[sliceid].keys():
            for reg in slice_modules[sliceid][det].keys():
                for lyr in slice_modules[sliceid][det][reg].keys():                
                    for etamod in slice_modules[sliceid][det][reg][lyr]["eta"].keys():
                        duplication_count[det][reg][lyr][etamod]+=1


    barrel_occupancies_4mrad_100mm={"pixel": [2.5,1.2,0.9,0.75,0.6],                         
                                    "strip": [0.4,0.4,0.3,0.3,0.2,0.2,0.2,0.2]}
    barrel_lengths={"pixel": [20,40,40,40,40],
                    "strip": [25,25,25,25,50,50,50,50]}
    occupancies_per_module = {det:{reg: [] for reg in regs} for det in dets}
    for det in dets:
        for i in range(len(barrel_lengths[det])):
            occupancies_per_module[det]['barrel'].append(barrel_occupancies_4mrad_100mm[det][i]*barrel_lengths[det][i]/float(100))

        occupancies_per_module[det]['endcap']=[0 for i in range(layers[det][reg])]
    
    layer_options = { "HTTlike" : { "pixel" : [4], "strip":[0,2,3,4,5,6,7]},
                      "Outer5" :  { "pixel" : [4], "strip":[0,2,4,6]},
                      "Inner5" :  { "pixel" : [0,1,2,4,5], "strip":[]},
                      "Mid5" :  { "pixel" : [2,4,5], "strip":[0,2]}}

    layer_options_occ = defaultdict(lambda:1)

    msg.info("%-5s %-6s %-5s %-12s %-10s %-10s"  % ("","","","","min/max","min/max",))
    msg.info("%-5s %-6s %-5s %-12s %-10s %-10s"  % ("det","reg","lyr","duplication","modules","occupancy",))
    for det in dets:
        for reg in regs:
            for lyr in range(layers[det][reg]):
                dup=-1
                if len(duplication_count[det][reg][lyr])!=0:
                    dup = sum(duplication_count[det][reg][lyr].values())/float(len(duplication_count[det][reg][lyr]))
                occs = [occupancies_per_module[det][reg][lyr]*len(slice_modules[sliceid][det][reg][lyr]["eta"]) for sliceid in slice_modules]
                lens = [len(slice_modules[sliceid][det][reg][lyr]["eta"]) for sliceid in slice_modules]

                for opt,config in layer_options.items():
                    if lyr in config[det]:
                        layer_options_occ[opt]*=sum(occs)/float(len(occs))

                msg.info("%5s %5s %-5d    %2.2f      [%2d,%2d]     [%2.2f,%2.2f]"  % (det,reg,lyr,dup,min(lens),max(lens),min(occs),max(occs)))

    msg.info("\nOccupancies:\n")
    for opt,occ in layer_options_occ.items():
        msg.info(opt,occ)



#--------------------------------------------------------------------------------
# Plot Slices
#--------------------------------------------------------------------------------
def plotSlices(sliceid2int,slice_modules):

    events=0
    for trk in Data:
        events = events + 1
        if events>args.evts:
            break
        
        for hit_idx in range(len(trk.SensitivePosX)):
            det = trk.SensitiveIsPixel[hit_idx]
            reg = abs(trk.SensitiveBarrelEndcap[hit_idx])
            lyr = getLayer(trk,hit_idx)
            row = getRow(trk,hit_idx)[1]
            r=trk.SensitivePosR[hit_idx]
            z=trk.SensitivePosZ[hit_idx]

            for sliceid,modules in slice_modules.items():
                hist = getHist("slice_{}_rz".format(sliceid2int[sliceid]),
                               lambda n: ROOT.TH2D(n,"; z[mm]; r[mm]",3500,-500,+3000,1000,0,1000))

                if row in modules[int2det[det]][int2reg[reg]][lyr]["eta"]:            
                    hist.Fill(z,r)

            

#--------------------------------------------------------------------------------
# Main
#--------------------------------------------------------------------------------

histfile = ROOT.TFile("test.root",'recreate')

eta_vals_for_key_layer=parseGeometry()

nslices=args.nslices

if args.fullgran:
    nslices=len(eta_vals_for_key_layer)

mod_slice_map = divideKeyLayerModulesIntoSlices(eta_vals_for_key_layer)

slice_modules,sliceid2int,slice_tracks= getModuleToSliceMap(mod_slice_map)
    
if key2:
    msg.info("%d, two layers slices found"%(len(slice_modules)))
    for x in slice_modules.keys():
        msg.info(x)

if args.trim:  trimSlice(slice_modules,slice_tracks)

writeRMap(slice_modules,sliceid2int)

printOccupancies(slice_modules)

plotSlices(sliceid2int,slice_modules)

for hist in hists.values():
    hist.Write()
histfile.Close()


