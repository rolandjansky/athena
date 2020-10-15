# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/test/config_t.py
# Created: Aug, 2020
# Purpose: Regression tests CaloCluterCorrection configuration
#

from __future__ import print_function


from PyUtils import coverage
c = coverage.Coverage ('CaloClusterCorrection.common')


from AthenaConfiguration.ComponentFactory import CompFactory
from CaloClusterCorrection import common
from CaloClusterCorrection.CaloSwCorrections import make_CaloSwCorrectionsCfg

class TestTool:
    def __init__ (self, name, **kw):
        self.name = name
        self.args = kw
        return

    def check (self, config):
        assert self.__class__.__name__ == config.__class__.__name__
        if self.name != config.getName():
            print ('name mismatch: ', self.__class__.__name__, self.name, config.getName())
        assert self.name == config.getName()
        props = config._properties.copy()
        for k, v in self.args.items():
            assert k in props
            if isinstance (v, TestTool):
                v.check (props[k])
            else:
                p = props[k]
                if isinstance (v, list):
                    p = list(p)
                    if v and isinstance (v[0], list):
                        p = [list(x) for x in p]
                if p != v:
                    print ('match fail: ', self.__class__.__name__, self.name, k, p, v)
                assert p == v
            del props[k]
        assert not props
        return


def checkTools (tools, expected):
    for i in range(min (len(tools), len(expected))):
        expected[i].check (tools[i])
    assert len(tools) == len(expected)
    return
        

class CaloFillRectangularCluster (TestTool): pass
class CaloClusterRemoveBad (TestTool): pass
class CaloScaleCluster (TestTool): pass
class CaloSwEtaoff_v3 (TestTool): pass
class CaloSwPhioff_v2 (TestTool): pass
class CaloClusterUpdate (TestTool): pass
class CaloSwGap_v2 (TestTool): pass
class CaloSwGap_v3 (TestTool): pass
class CaloSwCalibHitsCalibration (TestTool): pass
class CaloSwTransitionRegionsCorr (TestTool): pass
class CaloSwDeadOTX_ps (TestTool): pass
class CaloSwDeadOTX_back (TestTool): pass
class CaloSwPhimod_v2 (TestTool): pass
class CaloSwEtamod_v2 (TestTool): pass
class CaloSwTime (TestTool): pass
class CaloClusterBadChannelList (TestTool): pass
class CaloAffectedTool (TestTool): pass
class CaloTopoEMGap (TestTool): pass
class ToolConstantsCondAlg (TestTool): pass
class ProxyProviderSvc (TestTool): pass
class CondProxyProvider (TestTool): pass
class CondInputLoader (TestTool): pass
class IOVDbSvc (TestTool): pass
class PoolSvc (TestTool): pass
class CondSvc (TestTool): pass


# Default corrections, with fixed version.
def test1 (flags_in):
    common._poolfiles_seen = {}
    common._folders_used = {}
    common._alltools = {}

    flags = flags_in.clone()
    flags.lock()

    ca = make_CaloSwCorrectionsCfg (flags, version = 'v12_calh', key = 'ele35')
    tools = ca.popPrivateTools()

    checkTools \
      (tools,
       [CaloFillRectangularCluster ('layers_ele35',
                                    order=100,
                                    isDummy=False,
                                    fill_cluster=True,
                                    eta_size=3,
                                    phi_size=5),
        CaloClusterRemoveBad ('removebad_ele35', EMin=0.0),
        CaloScaleCluster ('rfac_v5_ele35',
                          prefix='ele35.',
                          DBHandleKey='CaloSwClusterCorrections.rfac-v5'),
        CaloSwEtaoff_v3 ('etaoffb1_v5_ele35',
                         prefix='ele35b1.',
                         DBHandleKey='CaloSwClusterCorrections.etaoff-v5'),
        CaloSwEtaoff_v3 ('etaoffe1_v5_ele35',
                         prefix='ele35e1.',
                         DBHandleKey='CaloSwClusterCorrections.etaoff-v5'),
        CaloSwEtaoff_v3 ('etaoffb2_v5_ele35',
                         prefix='ele35b2.',
                         DBHandleKey='CaloSwClusterCorrections.etaoff-v5'),
        CaloSwEtaoff_v3 ('etaoffe2_v5_ele35',
                         prefix='ele35e2.',
                         DBHandleKey='CaloSwClusterCorrections.etaoff-v5'),
        CaloSwPhioff_v2 ('phioffb2_v5_ele35',
                         prefix='ele35b2.',
                         DBHandleKey='CaloSwClusterCorrections.phioff-v5'),
        CaloSwPhioff_v2 ('phioffe2_v5_ele35',
                         prefix='ele35e2.',
                         DBHandleKey='CaloSwClusterCorrections.phioff-v5'),
        CaloClusterUpdate ('larupdate_ele35',
                           prefix='ele35.',
                           DBHandleKey='CaloSwClusterCorrections.larupdate'),
        CaloSwGap_v3 ('gap_v6_ele35',
                      prefix='ele35.',
                      DBHandleKey='CaloSwClusterCorrections.gap-v6'),
        CaloSwCalibHitsCalibration ('calhits_v9_ele35',
                                    prefix='ele35.',
                                    DBHandleKey='CaloSwClusterCorrections.calhits-v9'),
        CaloSwTransitionRegionsCorr ('trcorr_v5_1_ele35',
                                     prefix='ele35.',
                                     DBHandleKey='CaloSwClusterCorrections.trcorr-v5_1'),
        CaloSwDeadOTX_ps ('deadOTXps_v1_ele35',
                          prefix='ele35.',
                          DBHandleKey='CaloSwClusterCorrections.deadOTXps-v1',
                          AffectedTool=CaloAffectedTool('CaloAffectedToolForDeadOTX_ps')),
        CaloSwDeadOTX_back ('deadOTXback_v1_ele35',
                            prefix='ele35.',
                            DBHandleKey='CaloSwClusterCorrections.deadOTXback-v1',
                            AffectedTool=CaloAffectedTool('CaloAffectedForDeadOTX_back')),
        CaloSwPhimod_v2 ('phimod_v4_ele35',
                         prefix='ele35.',
                         DBHandleKey='CaloSwClusterCorrections.phimod-v4'),
        CaloSwEtamod_v2 ('etamod_v4_ele35',
                         prefix='ele35.',
                         DBHandleKey='CaloSwClusterCorrections.etamod-v4'),
        CaloSwTime ('time_ele35',
                    order=801,
                    isDummy=False),
        CaloClusterBadChannelList ('listBadChannels_ele35',
                                   order=820,
                                   isDummy=False),
        ])


    checkTools \
      (ca._conditionsAlgs,
       [ToolConstantsCondAlg ('ToolConstantsCondAlg_CaloSwClusterCorrections_rfac-v5',
                              DetStoreKey='CaloSwClusterCorrections.rfac-v5',
                              ToolConstantsKey='CaloSwClusterCorrections.rfac-v5'),
        ToolConstantsCondAlg ('ToolConstantsCondAlg_CaloSwClusterCorrections_etaoff-v5',
                              DetStoreKey='CaloSwClusterCorrections.etaoff-v5',
                              ToolConstantsKey='CaloSwClusterCorrections.etaoff-v5'),
        ToolConstantsCondAlg ('ToolConstantsCondAlg_CaloSwClusterCorrections_phioff-v5',
                              DetStoreKey='CaloSwClusterCorrections.phioff-v5',
                              ToolConstantsKey='CaloSwClusterCorrections.phioff-v5'),
        ToolConstantsCondAlg ('ToolConstantsCondAlg_CaloSwClusterCorrections_larupdate',
                              DetStoreKey='CaloSwClusterCorrections.larupdate',
                              ToolConstantsKey='CaloSwClusterCorrections.larupdate'),
        ToolConstantsCondAlg ('ToolConstantsCondAlg_CaloSwClusterCorrections_gap-v6',
                              DetStoreKey='CaloSwClusterCorrections.gap-v6',
                              ToolConstantsKey='CaloSwClusterCorrections.gap-v6'),
        ToolConstantsCondAlg ('ToolConstantsCondAlg_CaloSwClusterCorrections_calhits-v9',
                              DetStoreKey='CaloSwClusterCorrections.calhits-v9',
                              ToolConstantsKey='CaloSwClusterCorrections.calhits-v9'),
        ToolConstantsCondAlg ('ToolConstantsCondAlg_CaloSwClusterCorrections_trcorr-v5_1',
                              DetStoreKey='CaloSwClusterCorrections.trcorr-v5_1',
                              ToolConstantsKey='CaloSwClusterCorrections.trcorr-v5_1'),
        ToolConstantsCondAlg ('ToolConstantsCondAlg_CaloSwClusterCorrections_deadOTXps-v1',
                              DetStoreKey='CaloSwClusterCorrections.deadOTXps-v1',
                              ToolConstantsKey='CaloSwClusterCorrections.deadOTXps-v1'),
        ToolConstantsCondAlg('ToolConstantsCondAlg_CaloSwClusterCorrections_deadOTXback-v1',
                             DetStoreKey='CaloSwClusterCorrections.deadOTXback-v1',
                             ToolConstantsKey='CaloSwClusterCorrections.deadOTXback-v1'),
        ToolConstantsCondAlg ('ToolConstantsCondAlg_CaloSwClusterCorrections_phimod-v4',
                              DetStoreKey='CaloSwClusterCorrections.phimod-v4',
                              ToolConstantsKey='CaloSwClusterCorrections.phimod-v4'),
        ToolConstantsCondAlg ('ToolConstantsCondAlg_CaloSwClusterCorrections_etamod-v4',
                              DetStoreKey='CaloSwClusterCorrections.etamod-v4',
                              ToolConstantsKey='CaloSwClusterCorrections.etamod-v4'),
        ])


    checkTools \
      (ca.getServices(),
       [ProxyProviderSvc ('ProxyProviderSvc',
                          ProviderNames=['CondProxyProvider']),
        CondProxyProvider ('CondProxyProvider',
                           InputCollections=['FID:BC292F26-AE73-9041-BF5C-BCE6C5C651EC']),
        ])


    ca.wasMerged()
    return


# Default corrections, global COOL tag, MC.
def test2 (flags_in):
    common._poolfiles_seen = {}
    common._folders_used = {}
    common._alltools = {}

    flags = flags_in.clone()
    flags.Input.isMC = True
    flags.IOVDb.GlobalTag = 'OFLCOND-RUN12-SDR-31'
    flags.Input.Files = defaultTestFiles.RDO
    flags.lock()

    ca = make_CaloSwCorrectionsCfg (flags, version = '@GLOBAL',
                                    key = 'gam35_conv')
    tools = ca.popPrivateTools()

    checkTools \
      (tools,
       [CaloFillRectangularCluster ('layers_gam35_conv',
                                    order=100,
                                    isDummy=False,
                                    fill_cluster=True,
                                    eta_size=3,
                                    phi_size=5),
        CaloClusterRemoveBad ('removebad_gam35_conv', EMin=0.0),
        CaloScaleCluster ('rfac_v5_gam35_conv',
                          prefix='gam35.',
                          DBHandleKey='/CALO/Ofl/CaloSwClusterCorrections/rfac'),
        CaloSwEtaoff_v3 ('etaoffb1_v5_gam35_conv',
                         prefix='gam35b1.',
                         DBHandleKey='/CALO/Ofl/CaloSwClusterCorrections/etaoff'),
        CaloSwEtaoff_v3 ('etaoffe1_v5_gam35_conv',
                         prefix='gam35e1.',
                         DBHandleKey='/CALO/Ofl/CaloSwClusterCorrections/etaoff'),
        CaloSwEtaoff_v3 ('etaoffb2_v5_gam35_conv', prefix='gam35b2.',
                         DBHandleKey='/CALO/Ofl/CaloSwClusterCorrections/etaoff'),
        CaloSwEtaoff_v3 ('etaoffe2_v5_gam35_conv', prefix='gam35e2.',
                         DBHandleKey='/CALO/Ofl/CaloSwClusterCorrections/etaoff'),
        CaloSwPhioff_v2 ('phioffb2_v5data_gam35_conv', prefix='gam35b2.',
                         DBHandleKey='/CALO/Ofl/CaloSwClusterCorrections/phioff'),
        CaloSwPhioff_v2 ('phioffe2_v5data_gam35_conv',
                         prefix='gam35e2.',
                         DBHandleKey='/CALO/Ofl/CaloSwClusterCorrections/phioff'),
        CaloClusterUpdate ('larupdate_gam35_conv',
                           prefix='gam35.',
                           DBHandleKey='/CALO/Ofl/CaloSwClusterCorrections/larupdate'),
        CaloSwGap_v3 ('gap_v6_gam35_conv',
                      prefix='gam35.',
                      DBHandleKey='/CALO/Ofl/CaloSwClusterCorrections/gap'),
        CaloSwCalibHitsCalibration ('calhits_v9_gam35_conv',
                                    prefix='gam35_conv.',
                                    DBHandleKey='/CALO/Ofl/CaloSwClusterCorrections/calhits'),
        CaloSwTransitionRegionsCorr ('trcorr_v5_1_gam35_conv',
                                     prefix='gam35.',
                                     DBHandleKey='/CALO/Ofl/CaloSwClusterCorrections/trcorr'),
        CaloSwDeadOTX_ps ('deadOTXps_v1_gam35_conv',
                          prefix='gam35_conv.',
                          DBHandleKey='/CALO/Ofl/CaloSwClusterCorrections/deadOTXps',
                          AffectedTool=CaloAffectedTool('CaloAffectedToolForDeadOTX_ps')),
        CaloSwDeadOTX_back ('deadOTXback_v1_gam35_conv',
                            prefix='gam35_conv.',
                            DBHandleKey='/CALO/Ofl/CaloSwClusterCorrections/deadOTXback',
                            AffectedTool=CaloAffectedTool('CaloAffectedForDeadOTX_back')),
        CaloSwPhimod_v2 ('phimod_v4_gam35_conv',
                         prefix='gam35.',
                         DBHandleKey='/CALO/Ofl/CaloSwClusterCorrections/phimod'),
        CaloSwEtamod_v2 ('etamod_v4_gam35_conv',
                         prefix='gam35.',
                         DBHandleKey='/CALO/Ofl/CaloSwClusterCorrections/etamod'),
        CaloSwTime ('time_gam35_conv',
                    order=801,
                    isDummy=False),
        CaloClusterBadChannelList ('listBadChannels_gam35_conv',
                                   order=820,
                                   isDummy=False),
        ])

    checkTools \
      (ca._conditionsAlgs,
       [CondInputLoader ('CondInputLoader',
                         Load=[['CaloRec::ToolConstants',
                                '/CALO/Ofl/CaloSwClusterCorrections/rfac'],
                               ['CaloRec::ToolConstants',
                                '/CALO/Ofl/CaloSwClusterCorrections/etaoff'],
                               ['CaloRec::ToolConstants',
                                '/CALO/Ofl/CaloSwClusterCorrections/phioff'],
                               ['CaloRec::ToolConstants',
                                '/CALO/Ofl/CaloSwClusterCorrections/larupdate'],
                               ['CaloRec::ToolConstants',
                                '/CALO/Ofl/CaloSwClusterCorrections/gap'],
                               ['CaloRec::ToolConstants',
                                '/CALO/Ofl/CaloSwClusterCorrections/calhits'],
                               ['CaloRec::ToolConstants',
                                '/CALO/Ofl/CaloSwClusterCorrections/trcorr'],
                               ['CaloRec::ToolConstants',
                                '/CALO/Ofl/CaloSwClusterCorrections/deadOTXps'],
                               ['CaloRec::ToolConstants',
                                '/CALO/Ofl/CaloSwClusterCorrections/deadOTXback'],
                               ['CaloRec::ToolConstants',
                                '/CALO/Ofl/CaloSwClusterCorrections/phimod'],
                               ['CaloRec::ToolConstants',
                                '/CALO/Ofl/CaloSwClusterCorrections/etamod']]),
        ])
            

    exp = IOVDbSvc ('IOVDbSvc',
                    dbConnection='sqlite://;schema=mycool.db;dbname=OFLP200',
                    DBInstance='OFLP200',
                    CacheAlign=3,
                    GlobalTag='OFLCOND-RUN12-SDR-31',
                    Folders = ['/CALO/Ofl/CaloSwClusterCorrections/rfac<tag>CaloSwClusterCorrections.00-02-13-rfac-v5</tag><db>COOLOFL_CALO/OFLP200</db>',
                               '/CALO/Ofl/CaloSwClusterCorrections/etaoff<tag>CaloSwClusterCorrections.00-02-13-etaoff-v5</tag><db>COOLOFL_CALO/OFLP200</db>',
                               '/CALO/Ofl/CaloSwClusterCorrections/phioff<tag>CaloSwClusterCorrections.00-02-13-phioff-v5data</tag><db>COOLOFL_CALO/OFLP200</db>',
                               '/CALO/Ofl/CaloSwClusterCorrections/larupdate<tag>CaloSwClusterCorrections.00-02-13-larupdate</tag><db>COOLOFL_CALO/OFLP200</db>',
                               '/CALO/Ofl/CaloSwClusterCorrections/gap<tag>CaloSwClusterCorrections.00-02-13-gap-v6</tag><db>COOLOFL_CALO/OFLP200</db>',
                               '/CALO/Ofl/CaloSwClusterCorrections/calhits<tag>CaloSwClusterCorrections.00-02-13-calhits-v9</tag><db>COOLOFL_CALO/OFLP200</db>',
                               '/CALO/Ofl/CaloSwClusterCorrections/trcorr<tag>CaloSwClusterCorrections.00-02-13-trcorr-v5_1</tag><db>COOLOFL_CALO/OFLP200</db>',
                               '/CALO/Ofl/CaloSwClusterCorrections/deadOTXps<tag>CaloSwClusterCorrections.00-02-13-deadOTXps-v1</tag><db>COOLOFL_CALO/OFLP200</db>',
                               '/CALO/Ofl/CaloSwClusterCorrections/deadOTXback<tag>CaloSwClusterCorrections.00-02-13-deadOTXback-v1</tag><db>COOLOFL_CALO/OFLP200</db>',
                               '/CALO/Ofl/CaloSwClusterCorrections/phimod<tag>CaloSwClusterCorrections.00-02-13-phimod-v4</tag><db>COOLOFL_CALO/OFLP200</db>',
                               '/CALO/Ofl/CaloSwClusterCorrections/etamod<tag>CaloSwClusterCorrections.00-02-13-etamod-v4</tag><db>COOLOFL_CALO/OFLP200</db>'])
    exp.check (ca.getService ('IOVDbSvc'))

    ca.wasMerged()
    return


# Correction subset, version defaulted, with overrides.
def test3 (flags_in):
    common._poolfiles_seen = {}
    common._folders_used = {}
    common._alltools = {}
    
    flags = flags_in.clone()
    flags.Input.ProjectName = 'data18'
    flags.lock()

    from CaloClusterCorrection.CaloSwCorrections import layers, update, gap, calhits
    from CaloClusterCorrection.constants import CALOCORR_JO, CALOCORR_POOL
    from CaloClusterCorrection.poolfiles import poolfiles
    ca = make_CaloSwCorrectionsCfg (flags, key = 'gam35',
                                    corrlist = [[layers],
                                                [update],
                                                [gap, 'v3', ('degree', 2)],
                                                [calhits],
                                                ],
                                    layers_suffix = 'lsuff',
                                    layers_key = 'gam35_conv',
                                    layers_cells_name = 'xcells',
                                    layers_source = 'CaloSwLayers.CaloSwLayers_parms',
                                    gap_source = 'caloswcorr_pool',
                                    layers_order = 101,
                                    calhits_version = 'v8',
                                    calhits_update_sampling_energies = True)
    tools = ca.popPrivateTools()
    checkTools \
      (tools[:4],
       [CaloFillRectangularCluster ('layers_gam35_convlsuff',
                                    order=101,
                                    isDummy=False,
                                    fill_cluster=True,
                                    eta_size=3,
                                    phi_size=5,
                                    cells_name = 'xcells'),
        CaloClusterUpdate ('larupdate_GLOBAL_gam35',
                           prefix='gam35.',
                           DBHandleKey='/CALO/CaloSwClusterCorrections/larupdate'),
        CaloSwGap_v2 ('gap_v3_gam35',
                      prefix='gam35.',
                      DBHandleKey='CaloSwClusterCorrections.gap-v3',
                      degree=2),
        CaloSwCalibHitsCalibration ('calhits_v8_gam35',
                                    prefix='gam35_conv.',
                                    DBHandleKey='/CALO/CaloSwClusterCorrections/calhits',
                                    update_sampling_energies=True),
        ])

    exp = IOVDbSvc ('IOVDbSvc',
                    dbConnection='sqlite://;schema=mycool.db;dbname=CONDBR2',
                    DBInstance='CONDBR2',
                    CacheAlign=3,
                    GlobalTag='CONDBR2-BLKPA-2017-05',
                    Folders=['/CALO/CaloSwClusterCorrections/larupdate<tag></tag><db>COOLONL_CALO/CONDBR2</db>',
                             '/CALO/CaloSwClusterCorrections/calhits<tag>CaloSwClusterCorrections.00-02-13-calhits-v8</tag><db>COOLONL_CALO/CONDBR2</db>'])

    exp.check (ca.getService ('IOVDbSvc'))

    ca.wasMerged()

    return


# makecorr details 1
def test4 (flags_in):
    common._poolfiles_seen = {}
    common._folders_used = {}
    common._alltools = {}
    
    flags = flags_in.clone()
    flags.lock()

    from CaloClusterCorrection.constants import CALOCORR_SW
    from CaloClusterCorrection.common import makecorr
    from CaloClusterCorrection.CaloSwGap import CaloSwGap_versions
    from CaloClusterCorrection.CaloSwCorrections import CaloSwCorrections

    ca = makecorr (flags,
                   CaloSwGap_versions,
                   None,  # name
                   'gap',
                   None,  # suffix
                   '@xvers', # version
                   'ele35',  # key
                   None, # sampling
                   None, # source
                   None, # confclass
                   CALOCORR_SW)
    tools = ca.popPrivateTools()
    exp = CaloSwGap_v3 ('gap_xvers_ele35',
                        prefix='ele35.',
                        DBHandleKey='/CALO/CaloSwClusterCorrections/gap')
    exp.check (tools)
    ca.wasMerged()

    exp = IOVDbSvc('IOVDbSvc',
                   dbConnection='sqlite://;schema=mycool.db;dbname=CONDBR2',
                   DBInstance='CONDBR2',
                   CacheAlign=3,
                   GlobalTag='CONDBR2-BLKPA-2017-05',
                   Folders=['/CALO/CaloSwClusterCorrections/gap<tag>xvers</tag><db>COOLONL_CALO/CONDBR2</db>'])
    exp.check (ca.getService ('IOVDbSvc'))


    return
                                     


# makecorr details 2
def test5 (flags_in):
    common._poolfiles_seen = {}
    common._folders_used = {}
    common._alltools = {}
    
    flags = flags_in.clone()
    flags.lock()

    from CaloClusterCorrection.constants import CALOCORR_SW
    from CaloClusterCorrection.common import makecorr, CaloCorrectionConfigError
    from CaloClusterCorrection.CaloSwGap import CaloSwGap_versions
    from CaloClusterCorrection.CaloSwCorrections import CaloSwCorrections

    try:
        makecorr (flags,
                  CaloSwGap_versions,
                  None,  # name
                  'gap',
                  None,  # suffix
                  'lsdkfjsdklfj', # version
                  'ele35',  # key
                  None, # sampling
                  None, # source
                  None, # confclass
                  CALOCORR_SW)
        assert 0
    except CaloCorrectionConfigError as e:
        assert e.args[0] == "Can't find version `lsdkfjsdklfj' for correction named `gap'."

    return
                                     

# makecorr details 3
def test6 (flags_in):
    common._poolfiles_seen = {}
    common._folders_used = {}
    common._alltools = {}
    
    flags = flags_in.clone()
    flags.lock()

    from CaloClusterCorrection.constants import CALOCORR_SW
    from CaloClusterCorrection.common import makecorr, CaloCorrectionConfigError
    from CaloClusterCorrection.CaloSwGap import CaloSwGap_versions
    from CaloClusterCorrection.CaloSwCorrections import CaloSwCorrections

    try:
        makecorr (flags,
                  CaloSwGap_versions,
                  None,  # name
                  'gap',
                  None,  # suffix
                  None, #version
                  'ele35',  # key
                  None, # sampling
                  'asdasd', # source
                  None, # confclass
                  CALOCORR_SW)
        assert 0
    except CaloCorrectionConfigError as e:
        assert e.args[0] == "Can't find any source to configure tool `gap_v6_ele35'.  Sources: ['asdasd']"

    return
                                     

# makecorr details 4
def test7 (flags_in):
    common._poolfiles_seen = {}
    common._folders_used = {}
    common._alltools = {}
    
    flags = flags_in.clone()
    flags.lock()

    from CaloClusterCorrection.constants import CALOCORR_SW
    from CaloClusterCorrection.common import makecorr
    from CaloClusterCorrection.CaloSwGap import CaloSwGap_versions
    from CaloClusterCorrection.CaloSwCorrections import CaloSwCorrections

    ca = makecorr (flags,
                   CaloSwGap_versions,
                   None,  # name
                   'gap',
                   None,  # suffix
                   '@GLOBAL', # version
                   'ele35',  # key
                   None, # sampling
                   None, # source
                   None, # confclass
                   CALOCORR_SW)
    tools = ca.popPrivateTools()
    exp = CaloSwGap_v3('gap_GLOBAL_ele35',
                       prefix='ele35.',
                       DBHandleKey='/CALO/CaloSwClusterCorrections/gap')
    exp.check (tools)

    ca.wasMerged()
    exp = IOVDbSvc ('IOVDbSvc',
                    dbConnection='sqlite://;schema=mycool.db;dbname=CONDBR2',
                    DBInstance='CONDBR2',
                    CacheAlign=3,
                    GlobalTag='CONDBR2-BLKPA-2017-05',
                    Folders=['/CALO/CaloSwClusterCorrections/gap<tag></tag><db>COOLONL_CALO/CONDBR2</db>'])
    exp.check (ca.getService ('IOVDbSvc'))

    return
                                     


# makecorr details 5
def test8 (flags_in):
    common._poolfiles_seen = {}
    common._folders_used = {}
    common._alltools = {}
    
    flags = flags_in.clone()
    flags.lock()

    from CaloClusterCorrection.constants import CALOCORR_SW, CALOCORR_COOL
    from CaloClusterCorrection.common import makecorr
    from CaloClusterCorrection.CaloSwGap import CaloSwGap_versions
    from CaloClusterCorrection.CaloSwCorrections import CaloSwCorrections

    cls_v2 = CompFactory.CaloSwGap_v2  # CaloClusterCorrection
    ca = makecorr (flags,
                   [['@', cls_v2, [CALOCORR_COOL], ['ele35']]],
                   None,  # name
                   'gap',
                   None,  # suffix
                   '@xvers', # version
                   'ele35',  # key
                   None, # sampling
                   None, # source
                   None, # confclass
                   CALOCORR_SW)
    tools = ca.popPrivateTools()
    exp = CaloSwGap_v2 ('gap_xvers_ele35',
                        prefix='ele35.',
                        DBHandleKey='/CALO/CaloSwClusterCorrections/gap')
    exp.check (tools)

    ca.wasMerged()
    exp = IOVDbSvc ('IOVDbSvc',
                    dbConnection='sqlite://;schema=mycool.db;dbname=CONDBR2',
                    DBInstance='CONDBR2',
                    CacheAlign=3,
                    GlobalTag='CONDBR2-BLKPA-2017-05',
                    Folders=['/CALO/CaloSwClusterCorrections/gap<tag>xvers</tag><db>COOLONL_CALO/CONDBR2</db>'])
    exp.check (ca.getService ('IOVDbSvc'))

    return
                                     


# makecorr details 6
def test9 (flags_in):
    common._poolfiles_seen = {}
    common._folders_used = {}
    common._alltools = {}
    
    flags = flags_in.clone()
    flags.lock()

    from CaloClusterCorrection.constants import CALOCORR_SW, CALOCORR_COOL
    from CaloClusterCorrection.common import makecorr
    from CaloClusterCorrection.CaloSwGap import CaloSwGap_versions
    from CaloClusterCorrection.CaloSwCorrections import CaloSwCorrections

    ca = makecorr (flags,
                   CaloSwGap_versions,
                   None,  # name
                   'gap',
                   None,  # suffix
                   '@xvers', # version
                   '@VALID_KEYS',  # key
                   None, # sampling
                   None, # source
                   None, # confclass
                   CALOCORR_SW)
    assert (ca == ['ele55', 'ele35', 'ele37', 'gam55', 'gam35', 'gam37'])
    return
                                     


# makecorr details 7
def test10 (flags_in):
    common._poolfiles_seen = {}
    common._folders_used = {}
    common._alltools = {}
    
    flags = flags_in.clone()
    flags.lock()

    from CaloClusterCorrection.constants import CALOCORR_SW, CALOCORR_NOPOOL, CALOCORR_TOPOOL
    from CaloClusterCorrection.common import makecorr
    from CaloClusterCorrection.CaloSwGap import CaloSwGap_versions
    from CaloClusterCorrection.CaloSwCorrections import CaloSwCorrections

    cls_v2 = CompFactory.CaloSwGap_v2  # CaloClusterCorrection
    ca = makecorr (flags,
                   [['@', cls_v2, [CALOCORR_NOPOOL], ['ele35']]],
                   None,  # name
                   'gap',
                   None,  # suffix
                   '@xvers', # version
                   'ele35',  # key
                   None, # sampling
                   CALOCORR_TOPOOL, # source
                   None, # confclass
                   CALOCORR_SW)
    assert (ca is None)

    return


# makecorr details 8
def test11 (flags_in):
    common._poolfiles_seen = {}
    common._folders_used = {}
    common._alltools = {'gapb2_GLOBAL_ele35' : 1}
    
    flags = flags_in.clone()
    flags.lock()

    from CaloClusterCorrection.constants import CALOCORR_SW, EMB1, EMB2, CALOCORR_COOL
    from CaloClusterCorrection.common import makecorr
    from CaloClusterCorrection.CaloSwGap import CaloSwGap_versions
    from CaloClusterCorrection.CaloSwCorrections import CaloSwCorrections

    cls_v2 = CompFactory.CaloSwGap_v2  # CaloClusterCorrection
    cls_v3 = CompFactory.CaloSwGap_v3  # CaloClusterCorrection
    ca = makecorr (flags,
                   [['@', {EMB1 : cls_v2, EMB2: cls_v3}, [CALOCORR_COOL], ['ele35']]],
                   None,  # name
                   'gap',
                   None,  # suffix
                   '@GLOBAL', # version
                   'ele35',  # key
                   EMB2, # sampling
                   None, # source
                   None, # confclass
                   CALOCORR_SW)
    tools = ca.popPrivateTools()
    exp = CaloSwGap_v3 ('gapb2_GLOBAL_ele352',
                        prefix='ele35b2.',
                        DBHandleKey='/CALO/CaloSwClusterCorrections/gap')
    exp.check (tools)
    ca.wasMerged()

    ca = makecorr (flags,
                   CaloSwGap_versions,
                   None,  # name
                   'gap',
                   None,  # suffix
                   '@GLOBAL', # version
                   'ele35',  # key
                   EMB2, # sampling
                   None, # source
                   None, # confclass
                   CALOCORR_SW)
    tools = ca.popPrivateTools()
    exp = CaloSwGap_v3 ('gapb2_GLOBAL_ele353',
                        prefix='ele35b2.',
                        DBHandleKey='/CALO/CaloSwClusterCorrections/gap')
    exp.check (tools)
    ca.wasMerged()

    return
                                     
                                     
# makecorr details 9
def test12 (flags_in):
    common._poolfiles_seen = {}
    common._folders_used = {}
    common._alltools = {'gapb2_GLOBAL_ele35' : 1}
    
    flags = flags_in.clone()
    flags.lock()

    from CaloClusterCorrection.constants import CALOCORR_SW, EMB1, EMB2, CALOCORR_JO
    from CaloClusterCorrection.common import makecorr
    from CaloClusterCorrection.CaloSwPhioff import CaloSwPhioff_versions
    from CaloClusterCorrection.CaloSwCorrections import CaloSwCorrections

    ca = makecorr (flags,
                   CaloSwPhioff_versions,
                   None,  # name
                   'phioff',
                   None,  # suffix
                   None, # version
                   'ele35',  # key
                   EMB2, # sampling
                   CALOCORR_JO, # source
                   None, # confclass
                   CALOCORR_SW)
    tools = ca.popPrivateTools()
    tools.correction = [[[]]]
    tools.energies = []
    exp = CaloSwPhioff_v2 ('phioffb2_v5data_ele35',
                           prefix='ele35b2.',
                           isDummy=False,
                           region=1,
                           correction_coef=0.001,
                           interp_barriers=[0.8],
                           degree=3,
                           flip_phi=True,
                           energy_degree=2,
                           energies=[], correction=[[[]]])
    exp.check (tools)
    ca.wasMerged()

    return


# makecorr details 10  (config_from_jo)
def test13 (flags_in):
    common._poolfiles_seen = {}
    common._folders_used = {}
    common._alltools = {'gapb2_GLOBAL_ele35' : 1}
    
    flags = flags_in.clone()
    flags.lock()

    from CaloClusterCorrection.constants import CALOCORR_SW, CALOCORR_EMTOPO, EMB1, EMB2, CALOCORR_JO
    from CaloClusterCorrection.common import makecorr
    from CaloClusterCorrection.CaloSwGap import CaloSwGap_versions
    from CaloClusterCorrection.CaloSwCorrections import CaloSwCorrections

    ca = makecorr (flags,
                   CaloSwGap_versions,
                   None,  # name
                   'gap',
                   None,  # suffix
                   None, # version
                   'ele35_conv',  # key
                   EMB2, # sampling
                   CALOCORR_JO, # source
                   None, # confclass
                   CALOCORR_SW)
    tools = ca.popPrivateTools()
    tools.correctionGoodPhi = []
    tools.correctionBadPhi = []
    exp = CaloSwGap_v3 ('gapb2_v6_ele35_conv',
                        prefix='ele35_convb2.',
                        isDummy=False,
                        correctionGoodPhi=[],
                        correctionBadPhi=[],
                        etamin_crack=1.425,
                        etamax_crack=1.55,
                        degree=3,
                        use_raw_eta=False,
                        use_raw_eta_boundaries=True,
                        region=4)
    exp.check (tools)
    ca.wasMerged()


    from CaloClusterCorrection.CaloTopoEMgap import CaloTopoEMgap_versions
    ca = makecorr (flags,
                   CaloTopoEMgap_versions,
                   None,  # name
                   'gap',
                   None,  # suffix
                   None, # version
                   'ele633_conv',  # key
                   EMB2, # sampling
                   CALOCORR_JO, # source
                   None, # confclass
                   CALOCORR_EMTOPO)
    tools = ca.popPrivateTools()
    tools.correction = [[]]
    exp = CaloTopoEMGap ('gapb2_v1_ele633_conv',
                         prefix='ele633_convb2.',
                         isDummy=False,
                         region=5,
                         etamin_crack=1.425,
                         etamax_crack=1.55,
                         degree=3,
                         use_raw_eta=False,
                         correction=[[]])
    exp.check (tools)
    ca.wasMerged()

    from CaloClusterCorrection.common import _config_from_jo
    corr = CompFactory.CaloSwPhioff_v2 ('foo')
    _config_from_jo (corr, 'CaloSwPhioff_v5data.CaloSwPhioff_v5data_parms',
                     'gam33_conv', EMB2, [], 101)
    corr.correction = [[[]]]
    corr.energies = []
    exp = CaloSwPhioff_v2 ('foo',
                           order=101,
                           isDummy=False,
                           region=1,
                           correction_coef=0.001,
                           interp_barriers=[0.8],
                           degree=3,
                           flip_phi=True,
                           energy_degree=2,
                           energies=[], correction=[[[]]])
    exp.check (corr)
    return
                                     

# config_from_pool
def test14 (flags_in):
    flags = flags_in.clone()
    flags.lock()

    from CaloClusterCorrection.common import _config_from_pool
    corr = CompFactory.CaloSwPhioff_v2 ('foo')

    ca = _config_from_pool (flags, corr, None, None)
    assert ca is False
    return
                                     

# config_from_cool
def test15 (flags_in):
    flags = flags_in.clone()
    flags.lock()

    from CaloClusterCorrection.common import _config_from_cool
    corr = CompFactory.CaloSwPhioff_v2 ('foo')

    ca = _config_from_cool (flags, corr, '/CALO/Ofl/CaloSwClusterCorrections/phioff', 'tag1')
    exp = CaloSwPhioff_v2 ('foo',
                           DBHandleKey='/CALO/Ofl/CaloSwClusterCorrections/phioff')
    exp.check (corr)

    exp = IOVDbSvc ('IOVDbSvc',
                    dbConnection='sqlite://;schema=mycool.db;dbname=CONDBR2',
                    DBInstance='CONDBR2',
                    CacheAlign=3,
                    GlobalTag='CONDBR2-BLKPA-2017-05',
                    Folders=['/CALO/Ofl/CaloSwClusterCorrections/phioff<tag>tag1</tag><db>COOLOFL_CALO/CONDBR2</db>'])
    exp.check (ca.getService ('IOVDbSvc'))

    ca.wasMerged()

    ca = _config_from_cool (flags, corr, '/CALO/Ofl/CaloSwClusterCorrections/phioff', 'tag2')
    assert ca is False

    return


# longest_match
def test16 (flags_in):
    from CaloClusterCorrection.common import _longest_match

    assert _longest_match ('a', 'abc') == 'abc'
    assert _longest_match ('a', ['b', 'a', 'd']) == 'a'
    assert _longest_match ('abc', ['a', 'ab', 'ad']) == 'ab'

    return


# make_corrections details
def test17 (flags_in):
    common._poolfiles_seen = {}
    common._folders_used = {}
    common._alltools = {}
    
    flags = flags_in.clone()
    flags.Input.ProjectName = 'data18'
    flags.Calo.ClusterCorrection.caloSwWhichCorrection = 'v6_lwc'
    flags.lock()

    from CaloClusterCorrection.CaloSwCorrections import layers, update, gap, calhits
    from CaloClusterCorrection.constants import CALOCORR_JO, CALOCORR_POOL
    from CaloClusterCorrection.poolfiles import poolfiles
    ca = make_CaloSwCorrectionsCfg (flags, key = 'gam35',
                                    corrlist = [[gap]])
    tools = ca.popPrivateTools()

    checkTools \
      (tools,
       [CaloSwGap_v2 ('gap_v4_gam35',
                      prefix='gam35.',
                      DBHandleKey='CaloSwClusterCorrections.gap-v4')
        ])

    ca.wasMerged()

    return


# make_corrections details
def test18 (flags_in):
    common._poolfiles_seen = {}
    common._folders_used = {}
    common._alltools = {}
    
    flags = flags_in.clone()
    flags.Input.ProjectName = 'data18'
    flags.lock()

    from CaloClusterCorrection.CaloSwCorrections import layers, update, gap, calhits
    from CaloClusterCorrection.constants import CALOCORR_JO, CALOCORR_POOL, CALOCORR_COOL
    from CaloClusterCorrection.poolfiles import poolfiles
    ca = make_CaloSwCorrectionsCfg (flags, key = 'gam35',
                                    corrlist = [[gap]],
                                    version = 'v6_lwc',
                                    source = CALOCORR_POOL)
    tools = ca.popPrivateTools()

    checkTools \
      (tools,
       [CaloSwGap_v2 ('gap_v4_gam35',
                      prefix='gam35.',
                      DBHandleKey='CaloSwClusterCorrections.gap-v4')
        ])

    ca.wasMerged()

    return


# make_corrections details
def test19 (flags_in):
    common._poolfiles_seen = {}
    common._folders_used = {}
    common._alltools = {}
    
    flags = flags_in.clone()
    flags.Input.isMC = True
    flags.GeoModel.AtlasVersion = 'ATLAS-CSC-05-xx'
    flags.IOVDb.GlobalTag = 'OFLCOND-RUN12-SDR-31'
    flags.Calo.ClusterCorrection.caloSwGeneration = '00-02-01'
    flags.lock()

    from CaloClusterCorrection.CaloSwCorrections import layers, update, gap, calhits
    from CaloClusterCorrection.constants import CALOCORR_JO, CALOCORR_POOL, CALOCORR_COOL
    from CaloClusterCorrection.poolfiles import poolfiles
    ca = make_CaloSwCorrectionsCfg (flags, key = 'gam35',
                                    corrlist = [[gap]],
                                    source = [CALOCORR_POOL, CALOCORR_COOL])
    tools = ca.popPrivateTools()
    checkTools \
      (tools,
       [CaloSwGap_v2 ('gap_v4_gam35',
                      prefix='gam35.',
                      DBHandleKey='CaloSwClusterCorrections.gap-v4')
        ])
    ca.wasMerged()

    return


# lookup_version
def test20 (flags_in):
    flags = flags_in.clone()
    flags.lock()

    from CaloClusterCorrection.CaloSwCorrections import CaloSwCorrections, layers, update, removebad
    from CaloClusterCorrection.constants import CALOCORR_SW

    vlist = CaloSwCorrections.lookup_version (flags, 'none', CALOCORR_SW)

    from CaloClusterCorrection.common import CaloCorrectionConfigError
    try:
        vlist = CaloSwCorrections.lookup_version (flags, 'alsdjk', CALOCORR_SW)
        assert 0
    except CaloCorrectionConfigError as e:
        assert e.args[0] == "Can't find global correction version alsdjk."
    return


# geom_match
def test21 (flags_in):
    from CaloClusterCorrection.CaloSwCorrections import CaloSwCorrections

    ret = CaloSwCorrections.geom_match ('geant4', 'CSC-01')
    assert ret == ('v4_1_calh', True)

    ret = CaloSwCorrections.geom_match ('geant4', 'asdasd')
    assert ret == ('@GLOBAL', True)

    from CaloClusterCorrection.common import CaloClusterCorrSetup
    class TestCorr (CaloClusterCorrSetup):
        geom_versions = [ ['foo', 'NOHIER#bar'] ]
    testCorr = TestCorr()
    ret = testCorr.geom_match ('geant4', 'foo')
    assert ret == ('bar', False)

    return


# writing to pool
def test22 (flags_in):
    flags = flags_in.clone()
    flags.lock()

    from CaloClusterCorrection.CaloSwCorrections import CaloSwCorrections
    from CaloClusterCorrection.constants import CALOCORR_SW

    (corr_output_list, tag_list, ca) = \
                       CaloSwCorrections.config_for_pool (flags,
                                                          'ele35',
                                                          CALOCORR_SW)
    ca.wasMerged()
    # FIXME: validate return?
    return
    
    
# make_ForPool details
def test23 (flags_in):
    flags = flags_in.clone()
    flags.lock()

    def f1(*args, **kw): pass
    def f2(*args, **kw): pass

    from CaloClusterCorrection.common import CaloClusterCorrSetup
    class TestCorr1 (CaloClusterCorrSetup):
        correction_generation_default = '00-00-00'
        correction_generation_flag_name = 'caloSwGeneration'
        versions = {
            'a' : [[f1, 200],
                   [f2, 100]]}
    testCorr1 = TestCorr1()
    from CaloClusterCorrection.common import CaloCorrectionConfigError
    try:
        testCorr1.make_ForPool (flags, ['asd'], 'asd')
        assert 0
    except CaloCorrectionConfigError as e:
        assert e.args[0] == "Tools for global version a are not in order! 100(f2) < 200(f1)"

    class TestCorr2 (CaloClusterCorrSetup):
        correction_generation_default = '00-00-00'
        correction_generation_flag_name = 'caloSwGeneration'
        versions = {
            'a' : [[f1, 100],
                   [f2, 200]],
            'b' : [[f1, 100],
                   [f2, 201]],
            }
    testCorr2 = TestCorr2()
    try:
        testCorr2.make_ForPool (flags, ['asd'], 'asd')
        assert 0
    except CaloCorrectionConfigError as e:
        assert e.args[0] == "Tool f2 has order 201 in b but order 200 in a!"
    return


from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior = 1

from AthenaConfiguration.AllConfigFlags import ConfigFlags
ConfigFlags.loadAllDynamicFlags()
ConfigFlags.Input.Files = defaultTestFiles.RAW

test1 (ConfigFlags)
test2 (ConfigFlags)
test3 (ConfigFlags)
test4 (ConfigFlags)
test5 (ConfigFlags)
test6 (ConfigFlags)
test7 (ConfigFlags)
test8 (ConfigFlags)
test9 (ConfigFlags)
test10 (ConfigFlags)
test11 (ConfigFlags)
test12 (ConfigFlags)
test13 (ConfigFlags)
test14 (ConfigFlags)
test15 (ConfigFlags)
test16 (ConfigFlags)
test17 (ConfigFlags)
test18 (ConfigFlags)
test19 (ConfigFlags)
test20 (ConfigFlags)
test21 (ConfigFlags)
test22 (ConfigFlags)
test23 (ConfigFlags)


c.analyze()
