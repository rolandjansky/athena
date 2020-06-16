# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# $Id: CaloClusterSwGetterBase.py,v 1.3 2007/05/26 03:14:56 ssnyder Exp $
#
# File: CaloRec/python/CaloClusterSwGetterBase.py
# Created: Mar 2007, sss
# Purpose: Base class for EM sliding window cluster Getter's.
#


from CaloRec.CaloClusterGetterBase import CaloClusterGetterBase
from AthenaCommon.JobProperties import jobproperties as jp
from CaloRec.CaloRecMakers import \
     make_CaloClusterBuilderSW, make_CaloClusterCopier
from CaloClusterCorrection.CaloSwCorrections import make_CaloSwCorrections
from AthenaCommon.SystemOfUnits import GeV


#
# This class serves as a base class for Getter's for EM sliding window
# clustering algorithms.  This derives from CaloClusterGetterBase;
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
# The EM SW clustering is a little tricky to set up, since we want
# to find the cluster seeds only once.  Hence, we run the SW cluster
# builder only for the _first_ Getter that we configure.  For subsequent
# ones, we copy the list of clusters from the original one, and then
# rerun the corrections.
#
# It's probably easiest to use this with make_CaloClusterSwGetter.
#
# We store the first Getter that gets configured here.
#
_first_sw_getter = None
class CaloClusterSwGetterBase (CaloClusterGetterBase):
    _corrargs = {}
    
    # Extends the base class configure.
    def configure (self):
        # Decide whether we run the clustering or just copy.
        global _first_sw_getter
        if _first_sw_getter:
            # We have a previous Getter.
            # So just copy, using that as the input.
            self._inputGetter = \
                           "CaloRec.CaloClusterSwGetterBase._first_sw_getter"
            self.makeMakerTools = self.makeMakerTools_copier
        else:
            # No previous Getter.
            # Save this one as the first Getter that we see.
            # Configure to run the SW clustering, using the towers as input.
            _first_sw_getter = self.__class__
            self._inputGetter = "CaloRec.CaloTowerEmGetter.CaloTowerEmGetter"
            self.makeMakerTools = self.makeMakerTools_sw

        # Run the base class's configure.
        return CaloClusterGetterBase.configure (self)

    # Create a maker tool list to run SW clustering.
    def makeMakerTools_sw (self, input_key):
        return [
            make_CaloClusterBuilderSW ('slw',
                                       input_key,
                                       eta_size = 3,
                                       phi_size = 5,
                                       e_threshold = 2.5*GeV,
                                       FillClusterCells = False)
            ]

    # Create a maker tool list to copy another cluster list.
    def makeMakerTools_copier (self, input_key):
        return [
            make_CaloClusterCopier (self._copierName, input_key)
            ]

    # Create the correction tool list for SW clustering.
    def makeCorrectionTools (self):
        corrargs = self._corrargs.copy()
        if 'cells_name' not in  self._corrargs:
            cells = self.getInputGetter(jp.CaloRecFlags.clusterCellGetterName())
            corrargs['cells_name'] = cells.outputKey()
        return make_CaloSwCorrections (self._clusterKey,
                                       suffix = self._suffix,
                                       version = self._corrversion,
                                       **corrargs)

    @classmethod
    def setCorrargs (cls, **kw):
        cls._corrargs = kw
        return


#
# Create a Getter class for configuring EM sliding window clustering.
# NAME is the name of the Getter class.
# TYP is the type of cluster (not including the size), like `ele' or `gam'.
# ETA_SIZE and PHI_SIZE give the cluster size.
# SUFFIX is an optional additional string to add to the names to
# make them unique.
# CORRVERSION allows overriding the name of the correction list to use.
#
def make_CaloClusterSwGetter (name, typ, eta_size, phi_size, suffix='',
                              corrversion = None,
                              towrite = False):
    # Handle naming conventions.
    if eta_size > 9 or phi_size > 9:
        sizekey = "%d_%d" % (eta_size, phi_size)
    else:
        sizekey = "%d%d" % (eta_size, phi_size)
    key = typ
    if key == '':
        key = 'ele'
    key = key + sizekey

    typsuffix = typ + sizekey

    # Special cases for backwards compatibility with historical practice.
    if typsuffix.startswith('ele'): typsuffix = typsuffix[3:]
    if typsuffix.endswith('35'): typsuffix = typsuffix[:-2]

    typsuffix = typsuffix + suffix
    algname = "LAr%sClusterMaker" % typsuffix
    copiername = "larClusterCopier" + typsuffix
    output_name = "LArClusterEM" + typsuffix

    # Create the class, and install the names.
    cls = type (name, (CaloClusterSwGetterBase,), {})
    cls._outputKey = output_name
    cls._name = algname
    cls._clusterKey = key
    cls._copierName = copiername
    cls._suffix = suffix
    cls._corrversion = corrversion
    cls._towrite = towrite

    return cls

