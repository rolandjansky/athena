# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#

from AthenaCommon.JobProperties import jobproperties as jp
from CaloRec.CaloClusterGetterBase import CaloClusterGetterBase
from CaloRec.CaloRecTopoEM35Flags import jobproperties

from CaloRec.CaloRecMakers import make_CaloClusterCopier
from CaloClusterCorrection.CaloSwCorrections import make_CaloSwCorrections

class CaloClusterTopoEMFixedSizeGetter (CaloClusterGetterBase):
    _inputGetter = 'CaloRec.CaloClusterTopoEMGetters.CaloClusterTopoEM430Getter'
    _outputKey = 'CaloTopoEM35'
    _name = 'EMTopo35ClusterMaker'
    _towrite = False
    _corrargs = {}
    _copierName = "TopoEMCopier35"
    _EtCut = jobproperties.CaloRecTopoEM35Flags.EtSeedCut()

    def makeMakerTools (self, input_key):
        input_getter = self.getInputGetter (self._inputGetter)
        return [
            make_CaloClusterCopier (self._copierName, input_getter.outputKey(), use_ClusterPosition=True, etCut=self._EtCut)
            ]

    def makeCorrectionTools (self):
        corrargs = self._corrargs.copy()
        if 'cells_name' not in  self._corrargs:
            cells = self.getInputGetter(jp.CaloRecFlags.clusterCellGetterName())
            corrargs['cells_name'] = cells.outputKey()
        return make_CaloSwCorrections (key = 'gam35',
                                       suffix = 'topo35',
                                       version = 'pi0_v2_noc',
                                       **corrargs)

    @classmethod
    def setCorrargs (cls, **kw):
        cls._corrargs = kw
        return
