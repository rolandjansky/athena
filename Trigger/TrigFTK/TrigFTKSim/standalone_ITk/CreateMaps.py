#!/usr/bin/env python2

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
This script is designed to read in a list of module positions from a text file
and produce plots of the detector geometry as well as plane and tower maps for
TrigFTKSim.

The input module position list is nominally produced by following these
instructions:
https://twiki.cern.ch/twiki/bin/view/Atlas/FTKL1TrackSoftwareIntegration#Step_1_Plot_All_of_the_Detector

PXL SL BEC LD MPHI META PHIID ETAID HASH X Y Z SinT W L ISBAD
1 0 -2 0 0 0 0 0 0 -167.614 -29.5549 -826 0 33.8 40.2 0
...

Columns are tab separated.
"""

import ROOT
import math
import numpy as np
import pandas as pd

# This will stop python from trying to draw plots on the screen
import matplotlib as mpl
mpl.use('Agg')

import matplotlib.pyplot as plt



# Load the input file into a pandas DataFrame structure, which
# is basically just a fancy table.
print "Loading input file"
mod_all = pd.DataFrame.from_csv( '/afs/cern.ch/work/j/jwebster/public/FTKShare/mapfile.txt' , index_col=None , sep='\t' )

# Add an additional column with radius R parameter
mod_all['R'] = np.power( np.square(mod_all['X']) + np.square(mod_all['Y']) , 0.5 )

# Add additional columns for eta/phi coordinates
print "Computing eta and phi from each module (x,y,z) centroid"
mod_all['PHI']   = np.arctan2( mod_all['Y'] , mod_all['X'] )
mod_all['THETA'] = np.arctan2( mod_all['R'] , mod_all['Z'] )
mod_all['ETA']   = -1.0 * np.log( np.tan( mod_all['THETA']/2.0 ) )

# Create some sub-DataFrames that just include particular regions of the detector
mod_pixel_barrel = mod_all.loc[ (mod_all['PXL']==1) & (mod_all['BEC']==0) ]
mod_pixel_endcap = mod_all.loc[ (mod_all['PXL']==1) & (mod_all['BEC']!=0) ]
mod_strip_barrel = mod_all.loc[ (mod_all['PXL']==0) & (mod_all['BEC']==0) ]
mod_strip_endcap = mod_all.loc[ (mod_all['PXL']==0) & (mod_all['BEC']!=0) ]

# Draw an R-Z plot of barrel + endcaps
print
print 'Plotting modules in R-Z plane'
f , ax = plt.subplots( figsize=(9,9) )
plt.scatter( mod_pixel_barrel['Z'] , mod_pixel_barrel['R'] , s=4 , c='b' , label='Pixel barrel modules' )
plt.scatter( mod_strip_barrel['Z'] , mod_strip_barrel['R'] , s=4 , c='r' , label='Strips barrel modules' )
plt.scatter( mod_pixel_endcap['Z'] , mod_pixel_endcap['R'] , s=4 , c='g' , label='Pixel EC modules' )
plt.scatter( mod_strip_endcap['Z'] , mod_strip_endcap['R'] , s=4 , c='c' , label='Strips EC modules' )
plt.xlabel( 'z [mm]' )
plt.ylabel( 'r [mm]' )
plt.legend( loc=3 )
plt.savefig( 'Modules_RZ.png' )
plt.close()

# Draw an X-Y plot of barrel
print 'Plotting barrel modules in X-Y plane'
f , ax = plt.subplots( figsize=(9,9) )
plt.scatter( mod_pixel_barrel['X'] , mod_pixel_barrel['Y'] , s=4 , c='b' , label='Pixel barrel modules' )
plt.scatter( mod_strip_barrel['X'] , mod_strip_barrel['Y'] , s=4 , c='r' , label='Strips barrel modules' )
plt.xlabel( 'x [mm]' )
plt.ylabel( 'y [mm]' )
plt.legend( loc=2 )
plt.savefig( 'Modules_XY.png' )
plt.close()

# List all of the layers/disks/sides/module ranges for each detector region,
# just to get an idea of what we're working with in this particular geometry...
def print_layer_info( mod , name ):
    print
    print name+' information:'
    print '    Layer IDs =', mod['LD'].unique()
    print '    Sides per layer =', mod['SL'].unique()
    print '    Min and max phi module IDs by layer ='
    for l in mod['LD'].unique():
        print '         L'+str(l),':',mod.loc[ mod['LD']==l ]['MPHI'].min(),mod.loc[ mod['LD']==l ]['MPHI'].max()
    print '    Min and max eta module IDs by layer ='
    for l in mod['LD'].unique():
        print '         L'+str(l),':',mod.loc[ mod['LD']==l ]['META'].min(),mod.loc[ mod['LD']==l ]['META'].max()

print_layer_info( mod_pixel_barrel , 'Pixel barrel' )
print_layer_info( mod_pixel_endcap , 'Pixel endcap' )
print_layer_info( mod_strip_barrel , 'Strips barrel' )
print_layer_info( mod_strip_endcap , 'Strips endcap' )
print



# Now that we've produced some plots and dumped some relevant info it's time
# to actually generate tower/plane maps. 

# ===========================================================================
# PLANE MAP generation
# ===========================================================================

# Open file
pmap = open( 'Output_PlaneMap.pmap' , 'w' )
print 'Generating plane map in file', pmap, '...'

# Start by listing layer counts
pmap.write( '%i pixel barrel\n' % len(mod_pixel_barrel['LD'].unique()) )
pmap.write( '%i pixel endcap\n' % len(mod_pixel_endcap['LD'].unique()) )

# Note that the strips layers are double-sided (axial and stereo layers)
# so we need to put 2 FTK layers in the map for each ATLAS layer ID. FTK
# treats each side as a separate logical layer.
pmap.write( '%i SCT barrel\n' % (2*len(mod_strip_barrel['LD'].unique())) )
pmap.write( '%i SCT endcap\n' % (2*len(mod_strip_endcap['LD'].unique())) )

#
# Now we list information for each individual layer
#

for l in mod_pixel_barrel['LD'].unique():
    pmap.write( "pixel %-5i %-5i %-5i dim 2 plane %-5i phi %-5i eta %-5i\n" % (
        0 , # 0 => barrel
        -1 , # disk #, 0 in barrel
        l , # layer #
        -1 , # FTK logical layer #, must be set by hand
        len( mod_pixel_barrel.loc[ mod_pixel_barrel['LD']==l ]['MPHI'].unique() ) , # nPhi modules
        len( mod_pixel_barrel.loc[ mod_pixel_barrel['LD']==l ]['META'].unique() ) , # nEta modules
        ))

for l in mod_pixel_endcap['LD'].unique():
    pmap.write( "pixel %-5i %-5i %-5i dim 2 plane %-5i phi %-5i eta %-5i\n" % (
        1 , # 1 => endcap
        l , # disk #
        l , # layer #
        -1 , # FTK logical layer #, must be set by hand
        len( mod_pixel_endcap.loc[ mod_pixel_endcap['LD']==l ]['MPHI'].unique() ) , # nPhi modules
        len( mod_pixel_endcap.loc[ mod_pixel_endcap['LD']==l ]['META'].unique() ) , # nEta modules
        ))

for l in mod_strip_barrel['LD'].unique():
    for s in [ 0 , 1 ]:
        pmap.write( "SCT   %-5i %-5i %-5i dim 1 stereo %-1i plane %-5i phi %-5i eta %-5i\n" % (
            0 , # 0 => barrel
            -1 , # disk #, 0 in barrel
            (l*2)+s , # layer #
            s , # side
            -1 , # FTK logical layer #, must be set by hand
            len( mod_strip_barrel.loc[ mod_strip_barrel['LD']==l ]['MPHI'].unique() ) , # nPhi modules
            len( mod_strip_barrel.loc[ mod_strip_barrel['LD']==l ]['META'].unique() ) , # nEta modules
        ))

for l in mod_strip_endcap['LD'].unique():
    for s in [ 0 , 1 ]:
        pmap.write( "SCT   %-5i %-5i %-5i dim 1 stereo %-1i plane %-5i phi %-5i eta %-5i\n" % (
            1 , # 1 => endcap
            l , # disk #
            (l*2)+s , # layer #
            s , # side
            -1 , # FTK logical layer #, must be set by hand
            len( mod_strip_endcap.loc[ mod_strip_endcap['LD']==l ]['MPHI'].unique() ) , # nPhi modules
            len( mod_strip_endcap.loc[ mod_strip_endcap['LD']==l ]['META'].unique() ) , # nEta modules
        ))
    
pmap.close()

print "IMPORTANT WARNING: The plane map constructed here has all layers disabled. In order"
print "   to get it to work with the simulation you will need to turn on layers by"
print "   hand by replacing -1 values after the \"plane\" column with FTK layer numbers"



# ===========================================================================
# TOWER MAP generation
# ===========================================================================
# The approach here is to first define the layers and parameter ranges that we
# want to include in the map. The script should produce an output file according
# to these inputs.

# Open file
tmap = open( 'Output_TowerMap.tmap' , 'w' )
print 'Generating tower map in file', tmap, '...'

# Tower eta/phi ranges
eta_min   = 0.0
eta_max   = 1.0
eta_step  = 0.2
eta_width = 0.4
phi_min   = 0.0
phi_max   = 3.0
phi_step  = 0.2
phi_width = 0.4

# Next we define the layers we want to include in the tower map.
# To do this we create an ordered list with one element of for each layer.
# The element contains a subset of the modules just in the layer.
#
# For now I just include barrel strips layers
tmapLayers = [
    mod_strip_barrel.loc[ (mod_strip_barrel['LD']==0) & (mod_strip_barrel['SL']==0) ] ,
    mod_strip_barrel.loc[ (mod_strip_barrel['LD']==0) & (mod_strip_barrel['SL']==1) ] ,
    mod_strip_barrel.loc[ (mod_strip_barrel['LD']==1) & (mod_strip_barrel['SL']==0) ] ,
    mod_strip_barrel.loc[ (mod_strip_barrel['LD']==1) & (mod_strip_barrel['SL']==1) ] ,
    mod_strip_barrel.loc[ (mod_strip_barrel['LD']==2) & (mod_strip_barrel['SL']==0) ] ,
    mod_strip_barrel.loc[ (mod_strip_barrel['LD']==2) & (mod_strip_barrel['SL']==1) ] ,
    mod_strip_barrel.loc[ (mod_strip_barrel['LD']==3) & (mod_strip_barrel['SL']==0) ] ,
    mod_strip_barrel.loc[ (mod_strip_barrel['LD']==3) & (mod_strip_barrel['SL']==1) ] ,
]

nTowersPhi = 0
nTowers = 0
tmapStr = '' # We will write the map to this string and then dump to file

# Loop over the phi slices
for phi in np.arange(phi_min,phi_max+0.001,phi_step):

    nTowersPhi += 1
    phi_lo = phi - (phi_width/2.0)
    phi_hi = phi + (phi_width/2.0)

    # Loop over eta slices
    for eta in np.arange(eta_min,eta_max+0.001,eta_step):
        
        eta_lo = eta - (eta_width/2.0)
        eta_hi = eta + (eta_width/2.0)

        tmapStr += '%i\n' % (nTowers)
        nTowers += 1

        # Loop over each layer and determine which modules are present in the
        # given slice
        for il,l in enumerate(tmapLayers):

            mod = l.loc[ (l['PHI']>=phi_lo) & (l['PHI']<=phi_hi) & (l['ETA']>=eta_lo) & (l['ETA']<=eta_hi) ]
            phi_min_mod = mod['MPHI'].min()
            phi_max_mod = mod['MPHI'].max()
            eta_min_mod = mod['META'].min()
            eta_max_mod = mod['META'].max()
            nphi = len(l['MPHI'].unique())
            neta = len(l['META'].unique())

            # Correction for phi branch point
            if phi_max_mod - phi_min_mod > (nphi/2):
                phi_max_mod , phi_min_mod = phi_min_mod , phi_max_mod

            try:
                tmapStr += '%i %i %i %i %i %i %i %i %i\n' % (
                    0 , # 0 => strips
                    0 , # 0 => barrel
                    il , # FTK layer number
                    phi_min_mod ,
                    phi_max_mod ,
                    nphi ,
                    eta_min_mod ,
                    eta_max_mod ,
                    neta )
            except TypeError:
                print "FATAL : Ran into a TypeError when trying to produce the tower map."
                print "   This is typically caused by a layer not having a single module centroid"
                print "   in a particular eta/phi tower. You could try making your eta_width and"
                print "   phi_width per tower slightly larger and running again."
                print "   Problematic tower:"
                print "     tower ID =", nTowers-1
                print "     phi_min_mod =", phi_min_mod
                print "     phi_max_mod =", phi_min_mod
                print "     eta_min_mod =", eta_min_mod
                print "     eta_max_mod =", eta_min_mod
                exit()
            
        tmapStr += '\n'
            
tmap.write( 'ITKtowers %i phi %i\n\n' % (nTowers,nTowersPhi) )
tmap.write( tmapStr )
            
tmap.close()


# ===========================================================================
# SS MAP generation
# ===========================================================================

"""
To get this to work we need to modify the DetectorTool that produces mapfile.txt so that
it also dumps the number of eta/phi pixels in each module. Right now we don't have enough
information...
However, there's template code shown below for when DetectorTool is updated.

# Open file
ssmap = open( 'Output_SSMap.ss' , 'w' )
print 'Generating superstrip map in file', ssmap, '...'

ssmap.write( 'Version 9\n' )

# Loop over the layers that we included in the tower map. For each layer
# we just need to compute the number of pixels per module.
for il,l in enumerate(tmapLayers):

    # NOTE: I am assuming here that the number of pixels per module is constant for all
    # modules in a given layer
    ssmap.write( '%i %i %i PHISS %i ETASS %i\n' % (
        0 , # 0 => strips
        0 , # 0 => barrel
        il , # FTK layer number
        l['PHIINDEXMAX'].max()+1 ,
        l['ETAINDEXMAX'].max()+1
    ))

ssmap.close()

"""
