# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

def TrigMuonRoIToolCfg():
    tool = CompFactory.TrigMuonRoITool('TrigMuonRoITool')
    tool.DaqMuCTPiROBid = 0x760000
    tool.DecodeMuCTPiFromROB = False
    tool.MUCTPILocation = 'MUCTPI_RDO'

    # TODO: Relying on a Converter to provide MUCTPI_RDO may be not thread safe.
    # Run-3 MUCTPI should provide a decoder algorithm which should be scheduled before any alg using this tool.
    # For now, still using the Run-2 Converter-based solution:
    if tool.MUCTPILocation and not tool.DecodeMuCTPiFromROB:
        rdoType = 'MuCTPI_RDO'

        # Tell SGInputLoader to load MUCTPI_RDO from the input 
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if not hasattr(topSequence,'SGInputLoader'):
            raise RuntimeError('Cannot configure TrigMuonRoITool because SGInputLoader is missing from topSequence')
        topSequence.SGInputLoader.Load += [( rdoType, 'StoreGateSvc+'+tool.MUCTPILocation )]

        # Enable using the Converter to load MUCTPI_RDO from ByteStream
        from AthenaCommon.GlobalFlags import globalflags
        if globalflags.InputFormat.is_bytestream():
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            if not hasattr(svcMgr, 'ByteStreamAddressProviderSvc'):
                raise RuntimeError('Cannot configure TrigMuonRoITool because ByteStreamAddressProviderSvc is missing from svcMgr')
            svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ rdoType+'/'+tool.MUCTPILocation ]

    return tool
