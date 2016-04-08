# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: HSG2VertexReconstruction.py 520030 2012-10-03 10:11:57Z krasznaa $
#
# The function is this module adds all the information to a D3PD stream
# about the lepton quadruplets and vertices reconstructed while making
# the HSG2 D3PDs.
#

##
# @short Function adding information about re-fitted 4-lepton vertices
#
# @param d3pdalg The D3PDMaker algorithm/stream to add the info to
# @param quadruplet_key The SG key of the reconstructed lepton quadruplets
# @param vertex_key The SG key of the re-fitted 4-lepton vertices
# @param electron_target Prefix of electrons taking part in the lepton combinations
# @param muid_target Prefix of the MuID muons taking part in the lepton combinations
# @param staco_target Prefix of the Staco muons taking part in the lepton combinations
# @param calo_target Prefix of the Calo muons taking part in the lepton combinations
def addHSG2VertexReconstruction( d3pdalg,
                                 quadruplet_key = "QuadrupletCandidates",
                                 vertex_key = "QuadrupletVertexCandidates",
                                 electron_target = "el_",
                                 muid_target = "mu_muid_",
                                 staco_target = "mu_staco_",
                                 calo_target = "mu_calo_",
                                 muon_target = "mu_muon_" ):

    # Add information about the vertices separately:
    from TrackD3PDMaker.VertexD3PDObject import PrimaryVertexD3PDObject
    d3pdalg += PrimaryVertexD3PDObject( 1, sgkey = vertex_key,
                                        prefix = "quad_vertex_" )

    # Add the information about the quadruplets:
    from HiggsD3PDMaker.HSG2QuadrupletD3PDObject import HSG2QuadrupletD3PDObject
    d3pdalg += HSG2QuadrupletD3PDObject( 10, sgkey = quadruplet_key,
                                         prefix = "quad_",
                                         VertexIndex_target = "quad_vertex_",
                                         ElectronIndex_target = electron_target,
                                         MuonMuidIndex_target = muid_target,
                                         MuonStacoIndex_target = staco_target,
                                         MuonCaloIndex_target = calo_target,
                                         MuonIndex_target = muon_target )

    return
