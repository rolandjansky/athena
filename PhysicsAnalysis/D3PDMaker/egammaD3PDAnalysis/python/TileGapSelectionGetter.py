# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file egammaD3PDAnalysis/python/TileGapSelectionGetter.py
# @author scott snyder <snyder@bnl.gov>
# @date Mar, 2011
# @brief Configure algorithm to select calorimeter cells for tile gap sum.
#


from RecExConfig.Configured                  import Configured
from AthenaCommon.AlgSequence                import AlgSequence
from D3PDMakerCoreComps.resolveSGKey         import resolveSGKey
from D3PDMakerCoreComps                      import SGObjGetterTool
import egammaD3PDAnalysis


class TileGapSelectionGetter ( Configured )  :
    _outputType = "CaloCellContainer"
    _output = { _outputType : "TileGapCells" }
    _cellsSGKey = 'AllCalo,AODCellContainer'


    def __init__ (self,
                  seq = AlgSequence(),
                  *args, **kw):
        self.seq = seq
        Configured.__init__ (self, *args, **kw)
        return

        
    def configure(self):
        from AthenaCommon.Logging import logging
        mlog = logging.getLogger( 'TileGapSelectionGetter::configure:' )
        mlog.info ('entering')

        import traceback
        try:
            from RecExConfig.ObjKeyStore import objKeyStore
            cellsname = resolveSGKey ('CaloCellContainer', self._cellsSGKey)
            alg = egammaD3PDAnalysis.TileGapSelectionAlg \
                    ('TileGapSelection',
                     Getter = SGObjGetterTool ('TileGapSelectionGetter',
                                               SGKey = cellsname,
                                               TypeName = 'CaloCellContainer'),
                     OutputCellContainerName = self.outputKey())

            self.seq += alg

            # register output in objKeyStore
            objKeyStore.addTransient (self.outputType(),self.outputKey())

            self._handle = alg

        except Exception:
            mlog.error ("Error configuring TileGapSelectionAlg.")
            traceback.print_exc()

        return True


    def handle (self):
        return self._handle

    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType
