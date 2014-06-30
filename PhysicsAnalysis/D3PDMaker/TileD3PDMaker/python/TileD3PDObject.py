# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# File: TileAnalysisD3PDObject.py
# Author: Marco van Woerden <mvanwoer@cern.ch>
# Creation date: September 2012
# Update: August 2013
#

# IMPORT MODULES
import TileD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject                      import D3PDObject
from D3PDMakerConfig.D3PDMakerFlags                     import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject                      import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.D3PDObject                      import make_Void_D3PDObject
from D3PDMakerCoreComps.D3PDObject                      import make_SG_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation               import SimpleAssociation
from D3PDMakerCoreComps.IndexAssociation                import IndexAssociation
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
from D3PDMakerCoreComps.IndexMultiAssociation           import IndexMultiAssociation
from AthenaCommon.AlgSequence                           import AlgSequence
from TileD3PDMaker                                      import *
from CaloIdentifier                                     import SUBCALO 
from AthenaCommon.AppMgr                                import ServiceMgr as svcMgr

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

######################################################
# RETRIEVE D3PD OBJECT CONTAINING SELECTED TRACKS #
######################################################
#TileTrackD3PDObject = D3PDObject(make_Track_D3PDVectorFiller, 'tracks_' , 'TracksD3PDObject')
#TileTrackD3PDObject.defineBlock(0, 'TrackParticleDump', TileD3PDMaker.TileTrackFillerTool)

######################################################
# RETRIEVE D3PD OBJECT CONTAINING SELECTED CALOCELLS #
######################################################
#TileCellD3PDObject = D3PDObject(make_Cell_D3PDVectorFiller, 'cells_' , 'CellsD3PDObject')
#TileCellD3PDObject.defineBlock(0, 'CaloCellDump', TileD3PDMaker.TileCellFillerTool)

#TileMBTSD3PDObject = D3PDObject(make_MBTS_D3PDVectorFiller, 'mbts_' , 'MBTSD3PDObject')
#TileMBTSD3PDObject.defineBlock(0, 'MBTSDump', TileD3PDMaker.TileMBTSFillerTool)

######################################################
# RETRIEVE D3PD OBJECT CONTAINING SELECTED CLUSTERS #
######################################################
#TileClusterD3PDObject = D3PDObject(make_Cluster_D3PDVectorFiller, 'clusters_' , 'ClustersD3PDObject')
#TileClusterD3PDObject.defineBlock(0, 'CaloClusterDump', TileD3PDMaker.TileCaloClusterFillerTool)

#########################################
# RETRIEVE D3PD OBJECT CONTAINING MUONS #
#########################################
#TileStacoMuonD3PDObject = make_SGDataVector_D3PDObject ('Analysis::MuonContainer','StacoMuonCollection','mu_', 'TileStacoMuonD3PDObject')
#TileStacoMuonD3PDObject.defineBlock(0, 'StacoDump', TileD3PDMaker.TileMuonFillerTool)

#TileMuidMuonD3PDObject = make_SGDataVector_D3PDObject ('Analysis::MuonContainer','MuidMuonCollection','muid_', 'TileMuidMuonD3PDObject')
#TileMuidMuonD3PDObject.defineBlock(0, 'MuidDump', TileD3PDMaker.TileMuonFillerTool)

#######################################
# EVENT BY EVENT INFORMATION TO STORE #
#######################################
#TileEventD3PDObject = make_SG_D3PDObject( "EventInfo", D3PDMakerFlags.EventInfoSGKey(), 'evt_', "TileEventD3PDObject" )
#TileEventD3PDObject.defineBlock(0, 'EventDump', TileD3PDMaker.TileEventFillerTool)


######################
# SETUP ASSOCIATIONS #
######################
#IndexMultiAssociation(parent=TileCellD3PDObject,assoctool=TileD3PDMaker.TileCellMuAssociationTool,target="mu_",prefix="cellsmu_",level=0,blockname="AssocCellToMuon")

#IndexMultiAssociation(parent=TileMuonD3PDObject,assoctool=TileD3PDMaker.TileMuCellAssociationTool,target="cells_",prefix="mucells_",level=0,blockname="AssocMuonToCell")

#IndexMultiAssociation(parent=TileClusterD3PDObject,assoctool=TileD3PDMaker.TileClusterCellAssociationTool,target="cells_",prefix="clustercells_",level=0,blockname="AssocClusterToCell")

#IndexMultiAssociation(parent=TileTrackD3PDObject,assoctool=TileD3PDMaker.TileTrackCellAssociationTool,target="cells_",prefix="trackcells_",level=0,blockname="AssocTrackToCell")

#IndexMultiAssociation(parent=TileTrackD3PDObject,assoctool=TileD3PDMaker.TileTrackClusterAssociationTool,target="clusters_",prefix="trackclusters_",level=0,blockname="AssocTrackToCluster")























