# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: TileAnalysisD3PDObject.py
# Author: Marco van Woerden <mvanwoer@cern.ch>
# Creation date: September 2012
# Update: August 2013
#

# IMPORT MODULES
import D3PDMakerCoreComps
from D3PDMakerConfig.D3PDMakerFlags                     import D3PDMakerFlags


# VECTORFILLER DEFINITIONS
def make_Cell_D3PDVectorFiller(name, prefix, object_name, getter = None,sgkey = None,label = None):

    # GET STOREGATE DATAVECTORS FOR CALOCELLS AND MUONS
    calocell_getter = D3PDMakerCoreComps.SGDataVectorGetterTool('cell_getter',       TypeName = 'CaloCellContainer',\
                                                                SGKey = "SelectedCells",Label = "cells_")
    # CREATE SELECTED
    return D3PDMakerCoreComps.VectorFillerTool('cell_Getter',Prefix = 'cells_',Getter = calocell_getter,
                                               ObjectName = 'calocells',SaveMetadata = D3PDMakerFlags.SaveObjectMetadata())
                                               
def make_MBTS_D3PDVectorFiller(name, prefix, object_name, getter = None,sgkey = None,label = None):

    # GET STOREGATE DATAVECTORS FOR CALOCELLS AND MUONS
    calombts_getter = D3PDMakerCoreComps.SGDataVectorGetterTool('mbts_getter',       TypeName = 'TileCellContainer',\
                                                                SGKey = "MBTSContainer",Label = "mbts_")
    # CREATE SELECTED
    return D3PDMakerCoreComps.VectorFillerTool('mbts_Getter',Prefix = 'mbts_',Getter = calombts_getter,
                                               ObjectName = 'calombtss',SaveMetadata = D3PDMakerFlags.SaveObjectMetadata())


def make_Track_D3PDVectorFiller(name, prefix, object_name, getter = None,sgkey = None,label = None):

    # GET STOREGATE DATAVECTORS FOR TRACKS 
    trackparticle_getter = D3PDMakerCoreComps.SGDataVectorGetterTool('track_getter',       TypeName = 'Rec::TrackParticleContainer',\
                                                                SGKey = "SelectedTracks",Label = "tracks_")

    # CREATE SELECTED
    return D3PDMakerCoreComps.VectorFillerTool('track_Getter',Prefix = 'tracks_',Getter = trackparticle_getter,
                                               ObjectName = 'trackparticles',SaveMetadata = D3PDMakerFlags.SaveObjectMetadata())


def make_Cluster_D3PDVectorFiller(name, prefix, object_name, getter = None,sgkey = None,label = None):

    # GET STOREGATE DATAVECTORS FOR CLUSTERS 
    calocluster_getter = D3PDMakerCoreComps.SGDataVectorGetterTool('cluster_getter',       TypeName = 'CaloClusterContainer',\
                                                                SGKey = "CaloCalTopoCluster",Label = "clusters_")

    # CREATE SELECTED
    return D3PDMakerCoreComps.VectorFillerTool('cluster_Getter',Prefix = 'clusters_',Getter = calocluster_getter,
                                               ObjectName = 'caloclusters',SaveMetadata = D3PDMakerFlags.SaveObjectMetadata())
