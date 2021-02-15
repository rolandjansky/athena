# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#
# File: egammaD3PDMaker/python/CorrectionClusterGetter.py
# Created: Oct 2012, sss
# Purpose: Getter to build clusters for cluster correction studies.
#


from D3PDMakerCoreComps.resolveSGKey         import resolveSGKey
from CaloRec.CaloClusterGetterBase           import CaloClusterGetterBase
from CaloRec.CaloRecMakers                   import make_CaloClusterCopier
from CaloClusterCorrection.CaloSwCorrections import make_CaloSwCorrections


#
# This class serves as a base class for Getter's for clusters used for
# correction studies .  This derives from CaloClusterGetterBase;
# you should further derive from this to produce Getter's for specific
# cluster types.  In the derived classes, you should define the
# following class variables:
#
#   _name:        The name of the algorithm being constructed.
#   _outputKey:   The StoreGate key for the cluster container which
#                 the algorithm will produce.
#   _clusterKey:  The string describing the type of cluster to produce,
#                 like `ele55' or `gam37'.
#   _copierName:  The name to use for the cluster copier tool,
#                 if one is needed.
#   _suffix:      An extra string to add to names, to make them unique.
#   _corrversion: The correction version to use (or None to take the default).
#   _towrite:     Cluster is to be written to ESD/AOD.
#
class CorrectionClusterGetterBase (CaloClusterGetterBase):
    _corrargs = {}
    
    # Create a maker tool list to copy another cluster list.
    def makeMakerTools (self, input_key):
        return [
            make_CaloClusterCopier (self._copierName, input_key)
            ]

    # Create the correction tool list for SW clustering.
    def makeCorrectionTools (self):
        corrargs = self._corrargs.copy()
        if 'cells_name' not in self._corrargs:
            cells = resolveSGKey ('CaloCellContainer',
                                  'AODCellContainer,AllCalo')
            corrargs['cells_name'] = cells
        return make_CaloSwCorrections (self._clusterKey,
                                       suffix = self._suffix,
                                       version = self._corrversion,
                                       **corrargs)

    @classmethod
    def setCorrargs (cls, **kw):
        cls._corrargs = kw
        return


class _InputDummy:
    def __init__ (self, key):
        self._key = key
    def outputKey (self):
        return self._key


#
# Create a Getter class for cluster correction studies.
# TYP is the type of cluster (not including the size), like `ele' or `gam'.
# ETA_SIZE and PHI_SIZE give the cluster size.
# CORRVERSION allows overriding the name of the correction list to use.
# INPUT_KEY is the SG key of the input clusters.
# NAME_BASE is the prefix to use for generated names.
# OUTPUT_KEY is the SG key for the output clusters; if None,
#  the name is formed from NAME_BASE and the cluster parameters.
# COPIER_NAME is the name for the cluster copier tool; if None,
#  the name is formed from NAME_BASE and the cluster parameters.
# ALGNAME is the name for the algorithm; if None,
#  the name is formed from NAME_BASE and the cluster parameters.
# SUFFIX is an optional additional string to add to the names to
#  make them unique.
# KEEP_EACH_CORRECTION controls whether a copy of the cluster list
#  should be saved to SG after each correction.
# TOWRITE controls whether the clusters should be written to the output file.
#
def make_CorrectionClusterGetter (typ, eta_size, phi_size,
                                  corrversion = None,
                                  input_key = 'egammaClusters',
                                  name_base = 'CorrectionCluster_',
                                  output_key = None,
                                  copier_name = None,
                                  algname = None,
                                  suffix = '',
                                  keep_each_correction = False,
                                  towrite = False):
    if eta_size > 9 or phi_size > 9:
        sizekey = "%d_%d" % (eta_size, phi_size)
    else:
        sizekey = "%d%d" % (eta_size, phi_size)

    if output_key is None:
        output_key = name_base + typ + sizekey + suffix

    if algname is None:
        algname = output_key

    if copier_name is None:
        copier_name = algname + '_copier'

    key = typ
    if key == '':
        key = 'ele'
    key = key + sizekey

    # Create the class, and install the names.
    cls = type (algname, (CorrectionClusterGetterBase,), {})
    cls._outputKey = output_key
    cls._clusterKey = key
    cls._name = algname
    cls._copierName = copier_name
    cls._suffix = suffix
    cls._corrversion = corrversion
    cls._towrite = towrite
    cls._inputGetter = _InputDummy (input_key)
    cls._keepEachCorrection = keep_each_correction

    return cls

