from array import array
import math
from LayerMaterialUtils import *
from TrackingVolumeUtils import *
from ROOT import *

# the ATLAS style 
atlasStyle = TStyle('ATLAS','Atlas style')
# use plain black on white colors
icol=0
atlasStyle.SetFrameBorderMode(icol)
atlasStyle.SetCanvasBorderMode(icol)
atlasStyle.SetPadBorderMode(icol)
atlasStyle.SetPadColor(icol)
atlasStyle.SetCanvasColor(icol)
atlasStyle.SetStatColor(icol)
# do not display any of the standard histogram decorations
atlasStyle.SetOptTitle(0)
atlasStyle.SetOptStat(0)
atlasStyle.SetOptFit(0)
# put tick marks on top and RHS of plots
atlasStyle.SetPadTickX(1)
atlasStyle.SetPadTickY(1)
atlasStyle.SetPadRightMargin(0.2)
atlasStyle.SetPalette(1)
# default palette, looks cool
ncontours = 999 
ncontours = 999
stops = [0.00, 0.34, 0.61, 0.84, 1.00]
red   = [0.00, 0.00, 0.87, 1.00, 0.51]
green = [0.00, 0.81, 1.00, 0.20, 0.00]
blue  = [0.51, 1.00, 0.12, 0.00, 0.00]
s = array('d', stops)
r = array('d', red)
g = array('d', green)
b = array('d', blue)
npoints = len(s)
TColor.CreateGradientColorTable(npoints, s, r, g, b, ncontours)
atlasStyle.SetNumberContours(ncontours)
gROOT.SetStyle('ATLAS')


DetDescrVersion = 'ATLAS-GEO-16-00-00'

# tracking geometry files
tgMapFile       = TFile('AtlasMaterialMap-'+DetDescrVersion+'.root')
lmiDirectory  = tgMapFile.Get('LayerMaterialInspector')
lmiListOfKeys = lmiDirectory.GetListOfKeys()

for key in lmiListOfKeys :
   curLayerMap  = lmiDirectory.Get(key.GetName())
   print 'Processing: ', curLayerMap.GetName()
   if curLayerMap.GetName().find('CylinderLayer') > -1 :        
      layershname = curLayerMap.GetName().split('Cylinder')[1]
      g4Tree = TChain(layershname)
      g4Tree.Add('AtlasMaterialG4-*.root')
      tgTree = TChain(layershname)
      tgTree.Add('AtlasMaterialTG-*.root')       
      drawCylinderLayer(g4Tree,curLayerMap,tgTree,'img/')
   if curLayerMap.GetName().find('DiscLayer') > -1 : 
       layershname = curLayerMap.GetName().split('Disc')[1]
       g4Tree = TChain(layershname)
       g4Tree.Add('AtlasMaterialG4-*.root')
       tgTree = TChain(layershname)
       tgTree.Add('AtlasMaterialTG-*.root')
       drawDiscLayer(g4Tree,curLayerMap,tgTree,'img/')
       

tvFile = TFile('AtlasMaterialG4-'+DetDescrVersion+'.root')  
tvListOfKeys = tvFile.GetListOfKeys()
for key in tvListOfKeys :
    if  key.GetName().find('TVolume') > -1 :
        g4Tree = TChain(key.GetName())
        g4Tree.Add('AtlasMaterialG4-*.root')
        tgTree = TChain(key.GetName())
        tgTree.Add('AtlasMaterialTG-*.root')
        drawTrackingVolume(g4Tree,tgTree,'img/')
        