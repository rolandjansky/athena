#
# $Id: CaloCluster_corrections_class.py,v 1.3 2008-01-25 04:14:22 ssnyder Exp $
#
# File: CaloCluster_corrections_class.py
# Created: Jul 2004, sss
# Purpose: Define classes for managing sets of cluster correction constants.
#
# These classes are used to manage sets of self-consistent cluster corrections.
#
# For each correction set, create an instance of CaloCluster_corrections_class.
# It gets passed a list of the names for all the corrections in this
# set, in the order in which they should be applied.
# Example:
#
#    my_corrections = CaloCluster_corrections_class (['phicorr_b',
#                                                     'phicorr_e',
#                                                     'ecorr'])
#
# You then create objects for individual corrections with
# CaloCluster_correction_class.  Pass to this the name of the C++ class
# that implements the correction.  This will be used when creating
# the tool.  These correction objects should be assigned
# as attributes to the CaloCluster_corrections_class instance; the names
# should match those passed into the constructor.
# Example:
#
#   my_corrections.phicorr_b = CaloCluster_correction_class ('Phicorr')
#   my_corrections.phicorr_e = CaloCluster_correction_class ('Phicorr')
#   my_corrections.ecorr     = CaloCluster_correction_class ('Ecorr')
#
# Then assign the correction parameters as attributes of the
# CaloCluster_corrections_class instance.
# Example:
#
#   my_corrections.phicorr_b.offset = 1
#   my_corrections.phicorr_e.offset = -1
#   my_corrections.ecorr.scale = 1.5
#
# The user of your set of corrections can then apply them in several
# different ways, depending on the degree of control desired.
# To use all the corrections in the set as defined, use the function:
# CaloCluster_corrections_class.set.  For example, if ClusterMaker55
# is the algorithm, then do
#
#    my_corrections.set (ClusterMaker55)
#
# to have this maker use all the corrections defined in the set.
# An optional suffix argument may be supplied to add to the end
# of the tool names to make them unique:
# 
#    my_corrections.set (ClusterMaker55, '_55')
#
# An additional optional argument allows the user to explicitly
# specify the list of corrections to be applied:
#
#    my_corrections.set (ClusterMaker55, corrlist=['ecorr'])
#
# A list element may also be a 2-tuple, in which case the first element
# is the correction name and the second element is the suffix:
#
#    my_corrections.set (ClusterMaker55, corrlist=[('ecorr', '55')])
#
# Corrections may be added individually, using the add method
# of CaloCluster_correction_class; for example:
#
#    my_corrections.phicorr_b.add (ClusterMaker55, '_55')
#
# will add just this single correction to the end of the
# correction list.
#
# Finally, the set method may be used if the user wants to use
# a different C++ class with the correction.  Example:
#
#    ClusterMaker55.ClusterCorrectionTools = ['PhicorrSpecial/phicorr_b55']
#    my_corrections.phicorr_b.set (ClusterMaker55.phicorr_b55)
#


class CaloCluster_corrections_class:
    def __init__ (self, corrlist):
        self._corrlist = corrlist
    def corrlist (self):
        return self._corrlist
    def set (self, clustermaker, suffix='', corrlist=None):
        clustermaker.ClusterCorrectionTools = []
        if not corrlist:
            corrlist = self._corrlist
        for c in corrlist:
            getattr (self, c).add (clustermaker, suffix)

    def __setattr__ (self, name, value):
        self.__dict__[name] = value
        if name[0] != '_' and isinstance (value, CaloCluster_correction_class):
            value.setname (name)
        return
    

class CaloCluster_correction_class:
    def __init__ (self, athenaclass):
        self._athenaclass = athenaclass
    def athenaclass (self):
        return self._athenaclass
    def setname (self, name):
        self._name = name
    def set (self, obj):
        for (k, v) in self.__dict__.items():
            if k[0] != '_':
                setattr (obj, k, v)

    def add (self, clustermaker, suffix=''):
        name = self._name + suffix
        clustermaker.ClusterCorrectionTools += ['%s/%s'%(self._athenaclass,
                                                         name)]

        tool = AlgTool (clustermaker.jobOptName() + '.' + name)
        self.set (tool)
        clustermaker.children().append (tool)

        return
    
