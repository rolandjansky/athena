# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#
#
# File: CaloRec/python/CaloClusterTopoEMGetters.py
# Created: Mar 2007, sss
# Purpose: Define Getter's for topo EM clustering.
#


from CaloRec.CaloClusterGetterBase import CaloClusterGetterBase
from AthenaCommon.JobProperties import jobproperties as jp
from CaloRec.CaloRecMakers import \
     make_CaloTopoClusterSplitter, make_CaloTopoClusterMaker
from CaloClusterCorrection.CaloTopoEMCorrections \
     import make_CaloTopoEMCorrections
from AthenaCommon.SystemOfUnits import GeV


class CaloClusterTopoEMGetterBase (CaloClusterGetterBase):
    _inputGetter = jp.CaloRecFlags.clusterCellGetterName()
    _corrargs = {}

    # Must be defined by derived class.
    # _name = 'EMTopoClusterMaker'
    # _outputKey = 'EMTopoCluster'
    # _seedThreshold = 6.0
    # _neighborThreshold = 3.0
    # _cellThreshold = 3.0
    # _clusterEtorAbsEtCut = 5*GeV
    # _corrkey = 'ele633'
    # _corrversion = None
    # _suffix = ''
    # _towrite = False

    def makeMakerTools (self, input_key):
        return [
            make_CaloTopoClusterMaker (
              'EMTopoCluster',
              CellsName = input_key,
              CalorimeterNames = ["LAREM"],
              # Cells from the following samplings will be able to form seeds.
              # By default only the Tile, HEC and FCAL samplings are excluded.
              SeedSamplingNames = [
                "PreSamplerB", "EMB1", "EMB2", "EMB3",
                "PreSamplerE", "EME1", "EME2", "EME3"],
              NeighborOption  = "all3D",
              # note E or AbsE 
              CellThresholdOnEorAbsEinSigma  =    self._cellThreshold,
              NeighborThresholdOnEorAbsEinSigma = self._neighborThreshold, 
              SeedThresholdOnEorAbsEinSigma  =    self._seedThreshold,
                       
              # The following cut must be set to TRUE in order to make
              # double sided cuts on the seed and the cluster level
              #
              #SeedCutsInAbsE                 = False,
              ClusterEtorAbsEtCut            = self._clusterEtorAbsEtCut

              # The following cuts must be set to FALSE in order to make
              # single sided cuts on the neighbor and cell level
              #
              #NeighborCutsInAbsE             = True,
              #CellCutsInAbsE                 = True,
              #
              # The following Et thresholds are ignored in case
              # UsePileUpNoise is TRUE
              #
              # CellThresholdOnEtorAbsEt       =    0.0*MeV,
              # NeighborThresholdOnEtorAbsEt   =  100.0*MeV,
              # SeedThresholdOnEtorAbsEt       =  200.0*MeV # note Et or AbsEt
              ),

            make_CaloTopoClusterSplitter (
              'EMTopoSplitter',
              ShareBorderCells = True,

              # cells from the following samplings
              # will be able to form local maxima.
              # The excluded samplings are
              # PreSamplerB, EMB1, PreSamplerE,
              # EME1, and all samplings from Tile,
              # HEC and FCAL
              SamplingNames = [
                "EMB2", "EMB3",
                "EME2", "EME3"],
              SecondarySamplingNames = [
                "EMB1",  "EME1"],

              # The following options are not set,
              # since these are the default values
              #NumberOfCellsCut = 4,
              #EnergyCut        = 500*MeV
              )
            ]

    def makeCorrectionTools (self):
        return make_CaloTopoEMCorrections (self._corrkey,
                                           suffix = self._suffix,
                                           version = self._corrversion,
                                           **self._corrargs)

    @classmethod
    def setCorrargs (cls, **kw):
        cls._corrargs = kw
        return
    

def make_CaloClusterTopoEMGetter (name,
                                  seedThreshold,
                                  neighborThreshold,
                                  cellThreshold,
                                  keystr = None,
                                  typ = 'ele',
                                  clusterEtorAbsEtCut = 5*GeV,
                                  suffix = '',
                                  corrversion = None,
                                  outputKey = None,
                                  towrite = False):

    if keystr is None:
        keystr = "%d%d%d" % (int(seedThreshold),
                             int(neighborThreshold),
                             int(cellThreshold))
    cls = type (name, (CaloClusterTopoEMGetterBase,), {})
    cls._seedThreshold = seedThreshold
    cls._neighborThreshold = neighborThreshold
    cls._cellThreshold = cellThreshold
    cls._corrkey = typ + keystr
    cls._clusterEtorAbsEtCut = clusterEtorAbsEtCut
    cls._corrversion = corrversion
    cls._suffix = suffix
    cls._towrite = towrite

    if keystr == '633':
        keystr = ''
    cls._name = name

    if outputKey is None:
        outputKey = 'EMTopoCluster' + keystr + suffix
    cls._outputKey = outputKey

    return cls


def _make_topogetter (*args, **kw):
    g = make_CaloClusterTopoEMGetter (*args, **kw)
    globals()[g.__name__] = g
    return g

# Define Getter's for a few standard choices.
_make_topogetter ("CaloClusterTopoEM633Getter", 6, 3, 3)
_make_topogetter ("CaloClusterTopoEM630Getter", 6, 3, 0)
_make_topogetter ("CaloClusterTopoEM422Getter", 4, 2, 2, corrversion='none')
_make_topogetter ("CaloClusterTopoEM420Getter", 4, 2, 0, corrversion='none')
_make_topogetter ("CaloClusterTopoEM420LowcutGetter", 4, 2, 0,
                  clusterEtorAbsEtCut = 0.1*GeV, corrversion='none')
_make_topogetter ("CaloClusterTopoEM430Getter", 4, 3, 0,
                  clusterEtorAbsEtCut = 0.3*GeV, corrversion='none')
