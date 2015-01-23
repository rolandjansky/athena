# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: egammaCluster.py 633848 2014-12-04 19:54:50Z ssnyder $
#
# @file egammaD3PDMaker/python/egammaCluster.py
# @author scott snyder <snyder@bnl.gov>
# @date Mar, 2013
# @brief Add cluster information to a egamma D3PD object.
#


from D3PDMakerCoreComps.SimpleAssociation    import SimpleAssociation
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
import egammaD3PDMaker
import EventCommonD3PDMaker
import CaloD3PDMaker
import D3PDMakerCoreComps


def egammaCluster (egamma, allSamplings = False, fwdEVars = False):
    ClusterAssoc = SimpleAssociation \
      (egamma,
       egammaD3PDMaker.egammaClusterAssociationTool)
    ClusterAssoc.defineBlock \
      (2, 'ClusterKin', EventCommonD3PDMaker.FourMomFillerTool,
       prefix = 'cl_',
       WriteE = True,
       WriteM = False)
    ClusterAssoc.defineBlock (
        2, 'ClusterTime',
        D3PDMakerCoreComps.AuxDataFillerTool,
        Vars = ['time'],
        prefix = 'cl_')

    ClusterAssoc.defineBlock \
      (2, 'PositionInCalo', CaloD3PDMaker.ClusterPositionInCaloFillerTool,
       prefix = 'cl_')
    ClusterAssoc.defineBlock \
      (2, 'Position0InCalo', CaloD3PDMaker.ClusterPositionInCaloFillerTool,
       prefix = 'cl_',
       FillSeedCoordinates=True)
    ClusterAssoc.defineBlock \
      (2, 'Samplings', CaloD3PDMaker.ClusterEMSamplingFillerTool)

    if allSamplings:
        ClusterAssoc.defineBlock \
          (2, 'AllSamplings', CaloD3PDMaker.ClusterSamplingFillerTool,
           EmHadEnergies = False,
           SamplingEnergies = True)
    if fwdEVars:
        ClusterAssoc.defineBlock (
            1, 'FwdEVars',
            D3PDMakerCoreComps.AuxDataFillerTool,
            Vars = ['firstEdens = FIRST_ENG_DENS < float: 0',
                    'cellmaxfrac = ENG_FRAC_MAX < float: 0',
                    'longitudinal = LONGITUDINAL < float: 0',
                    'secondlambda = SECOND_LAMBDA < float: 0',
                    'lateral = LATERAL < float: 0',
                    'secondR = SECOND_R < float: 0',
                    'centerlambda = CENTER_LAMBDA < float: 0',
                    ])

    # Allow writing cluster cells.
    # Off by default.
    Cells = ContainedVectorMultiAssociation (
        ClusterAssoc,
        CaloD3PDMaker.CaloClusterCellAssociationTool,
        "cell_",
        99,
        blockname = 'ClusterCells')
    Cells.defineBlock (4, 'CellKinematics',
                       EventCommonD3PDMaker.FourMomFillerTool,
                       WriteE  = True,  WriteM = False)
    Cells.defineBlock (4, 'CellRawPosition',
                       CaloD3PDMaker.CaloCellRawFillerTool)
    Cells.defineBlock (5, 'CellDetail1',
                       CaloD3PDMaker.CaloCellDetailsFillerTool,
                       SaveCellQuality=True,
                       SaveTimeInfo=True,
                       SaveDetInfo=True,
                       SaveCellGain=True,
                       SaveBadCellStatus=False,
                       SaveId =False,
                       SavePositionInfo=False,
                       )
    Cells.defineBlock (6, 'CellDetail2',
                       CaloD3PDMaker.CaloCellDetailsFillerTool,
                       SaveCellQuality=False,
                       SaveTimeInfo=False,
                       SaveDetInfo=False,
                       SaveCellGain=False,
                       SaveBadCellStatus=True,
                       SaveId =True,
                       SavePositionInfo=True,
                       )

    return ClusterAssoc
