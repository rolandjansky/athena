# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Provide a  look up tables that connects logical algortihm names to
factory functions and arguments.

The table is used to determine the function to used to return an Algorithm
proxy object."""

try:
    from AthenaCommon.SystemOfUnits import GeV
except ImportError:
    GeV = 1000.


class Alg(object):
    """Proxy class for ATLAS python configuration class (APCC). Can
    by converted to an APCC using an instantiator."""

    def __init__(self, factory, args, kargs):

        # factory function used to instantiate the APCC. Usually the class
        # name of the APCC
        self.factory = factory

        # arguments and kewword arguments for the factory function
        n_args = []
        for a in args:
            try:
                n_args.append(int(a))
            except ValueError:
                try:
                    n_args.append(float(a))
                except ValueError:
                    n_args.append(a)

        self.args = tuple(n_args)

        self.kargs = []
        for k in sorted(kargs.keys()):
            self.kargs.append('%s=%s' % (k, kargs[k]))

    def getName(self):
        return self.factory

    def __repr__(self):
        return self.factory

    def __str__(self):
        text = [self.factory]
        text.extend(['arg: %s' % a for a in self.args])
        text.extend(['karg: %s' % k for k in self.kargs])
        return '\n'.join(text)

    def asString(self):
        args = list(self.args)
        args.extend(self.kargs)
        args = [str(a) for a in args]
        args = ', '.join(args)
        # import pdb; pdb.set_trace()
        s = '%s(%s)' % (self.factory, args)
        return s


# alg_dict supplies the algorithms to which names are assigned.
# this allows for changes in algorithms to be coherently propogated
# to all the algorithm clients.


def l15_unpacking_TT():
    return Alg('T2L1Unpacking_TT', (), {})


def l15_antikt4_TT_em():
    return Alg('T2CaloFastJet_a4TT', (), {})


def l15_antikt4_TT_had():
    return Alg('T2CaloFastJet_a4TT_JESCalib', (), {})


def l15_antikt10_TT_em():
    return Alg('T2CaloFastJet_a10TT', (), {})


def roi():
    return Alg('DummyAlgo', ('"RoiCreator"',), {})


def l15_hypo_implicit0(menu_data):

    hypo = menu_data.hypo
    assert len(hypo.jet_attributes) == 1

    etaMin = hypo.jet_attributes[0].eta_min
    etaMax = hypo.jet_attributes[0].eta_max

    kargs = {'multiplicity': len(hypo.jet_attributes),
             'l2_thrs': [j.threshold * GeV for j in hypo.jet_attributes],
             'etaMin': etaMin,
             'etaMax': etaMax}

    str_mult = hypo.jet_attributes_tostring()
    # import pdb; pdb.set_trace()
    # assert False
    return Alg('L2FullScanMultiJetAllTE',
               ('"L2JetAllTE_FullScan_l15_%s"' % str_mult,),
               kargs)


def l15_hypo_implicit1(menu_data):

    hypo = menu_data.hypo

    etaMin = hypo.jet_attributes[0].eta_min
    etaMax = hypo.jet_attributes[0].eta_max

    kargs = {'multiplicity': len(hypo.jet_attributes),
             'l2_thrs': [j.threshold * GeV for j in hypo.jet_attributes],
             'etaMin': etaMin,
             'etaMax': etaMax}

    str_mult = hypo.jet_attributes_tostring()
    # import pdb; pdb.set_trace()
    # assert False
    return Alg('L2FullScanMultiJetAllTE',
               ('"L2JetAllTE_FullScan_l15_%s"' % str_mult,),
               kargs)


def jr_antikt4_tc_em():
    """Does not exist yet - for dev"""
    # return Alg('TrigJetRec_AntiKt4_topo', (), {})
    return Alg('TrigHLTJetRec_AntiKt04', (), {})


def jr_antikt4_tc_had():
    # return Alg('TrigJetRec_AntiKt4_lctopo', (), {})
    return Alg('TrigHLTJetRec_AntiKt04', (), {})


def jr_antikt10_tc_em():
    # return Alg('TrigJetRec_AntiKt10_topo', (), {})
    return Alg('TrigHLTJetRec_AntiKt10', (), {})


def jr_antikt10_tc_had():
    # return Alg('TrigJetRec_AntiKt10_lctopo', (), {})
    return Alg('TrigHLTJetRec_AntiKt10', (), {})


# def jr_hypo_single(menu_data):
#
#    hd = menu_data.hypo
#    assert len(hd.jet_attributes) == 1
#    ja = hd.jet_attributes[0]
#    region = ja.region
#    return {'j': jr_hypo_single_j,
#            'fj': jr_hypo_single_fj}.get(region)(menu_data)


def jr_hypo_single(menu_data):

    hd = menu_data.hypo
    assert len(hd.jet_attributes) == 1
    ja = hd.jet_attributes[0]

    chain_name = hd.chain_name
    chain_name = chain_name.replace("_bperf","")
    chain_name = chain_name.replace("_EFID","")

    arg0 = '"EFJetHypo_ef_%s"' % chain_name
    etaMin = str(ja.eta_min)
    etaMax = str(ja.eta_max)
    kargs = {}
    
    args = [arg0, str(GeV * ja.threshold), etaMin, etaMax]
    
    return Alg('EFJetHypo', args, kargs)


# def jr_hypo_single_j(menu_data):
#
#    hd = menu_data.hypo
#    assert len(hd.jet_attributes) == 1
#    ja = hd.jet_attributes[0]
#    assert ja.region == 'j'
# 
#    new_chain_name = hd.chain_name
#    new_chain_name = new_chain_name.replace("_bperf","")
#    new_chain_name = new_chain_name.replace("_EFID","")
#    arg0 = '"EFJetHypo_ef_%s"' % new_chain_name
#    kargs = {}
#
#    args = [arg0, str(GeV * ja.threshold)]
# 
#    return Alg('EFCentJetHypo', args, kargs)
#
# def jr_hypo_single_fj(menu_data):
#
#    hd = menu_data.hypo
#    assert len(hd.jet_attributes) == 1
#    ja = hd.jet_attributes[0]
#    assert ja.region == 'fj'
#
#    threshold = int(ja.threshold)
#
#    return Alg('EFFwdJetHypo',
#               ('"EFJetHypo_ef_%s"' % hd.chain_name,
#                str(GeV * threshold)),
#               {})
#
#
#def jr_hypo_multi(menu_data):
#
#    hypo = menu_data.hypo
#
#    kargs = {'multiplicity': len(hypo.jet_attributes),
#             'ef_thrs': [j.threshold * GeV for j in hypo.jet_attributes]}
#
#    str_mult = hypo.jet_attributes_tostring()
#
#    return Alg(
#        'EFCentFullScanMultiJetHypo',
#        ('"EFjetHypo_ef_%s"' % str_mult,),
#        kargs)
#
#
def jr_hypo_multi(menu_data):

    hypo = menu_data.hypo

    etaMin = hypo.jet_attributes[0].eta_min
    etaMax = hypo.jet_attributes[0].eta_max
    
    kargs = {'multiplicity': len(hypo.jet_attributes),
             'ef_thrs': [j.threshold * GeV for j in hypo.jet_attributes],
             'etaMin': etaMin,
             'etaMax': etaMax,
             }

    str_mult = hypo.jet_attributes_tostring()

    return Alg(
        'EFCentFullScanMultiJetHypo',
        ('"EFjetHypo_ef_%s"' % str_mult,),
        kargs)


def jr_hypo_LArNoiseBurst(params):

    assert len(params.hypo_data) == 1
    hd = params.hypo_data[0]

    return Alg('EFJetHypoNoiseConfig',
               ('"EFJetHypoNoise_%s"' % hd.sig, str(GeV * hd.threshold)),
               {})


def jr_hypo_LArNoiseBurstT(params):

    assert len(params.hypo_data) == 1
    hd = params.hypo_data[0]

    return Alg('EFJetHypoNoiseConfig',
               ('"EFJetHypoNoise_%s"' % hd.sig,
                str(GeV * hd.threshold)),
               {})


 # NEEDS UPDATING FOR ETA RANGE HANDLING
 # def jr_hypo_testCleaning(params):
 # 
 #     assert len(params.hypo_data) == 1
 #     hd = params.hypo_data[0]
 # 
 #     if hd.eta_region == 'j':
 #         factory = 'EFJetHypo_doBasicCleaning'
 #         arg0 = '"EFJetHypo_doBasicCleaning_%s"' % hd.sig
 #     elif hd.eta_region == 'fj':
 #         factory = 'EFFwdJetHypo_doBasicCleaning'
 #         arg0 = '"EFFwdJetHypo_doBasicCleaning_%s"' % hd.sig
 #     else:
 #         assert False
 # 
 #     return Alg(factory, (arg0, str(GeV * hd.threshold)), {})
 # 
 # 
def superRoIMaker():
    factory = 'SeededAlgo'
    return Alg(factory,
               (),
               {'UseRoiSizes':False,'EtaHalfWidth':0.5,'PhiHalfWidth':0.5})


def cellMaker_superPS_topo():
    return Alg('TrigCaloCellMaker_jet_super',
               ('"CellMakerSuperPartialCalo_topo"',
                'doNoise=0',
                'AbsE=True'),
               {})


def cellMaker_fullcalo_topo():
    return Alg('TrigCaloCellMaker_jet_fullcalo',
               ('"CellMakerFullCalo_topo"',
                'doNoise=0',
                'AbsE=True',
                'doPers=True'),
               {})


def topoClusterMaker_partial():
    return Alg('TrigCaloClusterMaker_topo',
               ('"TrigCaloClusterMaker_topo_partial"',),
               {'doMoments': True,
                'doLC': True})
    

def topoClusterMaker_fullcalo():
    return Alg('TrigCaloClusterMaker_topo',
               ('"TrigCaloClusterMaker_topo_fullscan"',),
               {'doMoments': True,
                'doLC': True})


def roiDiagnostics(params):
    factory = 'TrigHLTRoIDiagnostics'
    return Alg(factory, (), {})


def jetRecDiagnostics(params):
    chain_name = params.name
    factory = 'TrigHLTJetDiagnostics_named'
    kwds = {'name': "'TrigHLTJetDiagnostics_%s'" % chain_name,
            'chain_name': "'%s'" % chain_name}
    return Alg(factory, (), kwds)


def jetHypoDiagnostics(params):
    chain_name = params.name
    factory = 'TrigHLTHypoDiagnostics_named'
    kwds = {'name': "'TrigHLTHypoDiagnostics_%s'" % chain_name,
            'chain_name': "'%s'" % chain_name}
    return Alg(factory, (), kwds)


def clusterDiagnostics(params):
    chain_name = params.name
    factory = 'TrigHLTClusterDiagnostics_named'
    kwds = {'name': "'TrigHLTClusterDiagnostics_%s'" % chain_name,
            'chain_name': "'%s'" % chain_name}
    return Alg(factory, (), kwds)


def cellDiagnostics(params):
    chain_name = params.name
    factory = 'TrigHLTCellDiagnostics_named'
    kwds = {'name': "'TrigHLTCellDiagnostics_%s'" % chain_name,
            'chain_name': "'%s'" % chain_name}
    return Alg(factory, (), kwds)


# look up table for function to use to construct a
# Algorithm proxy object.
alg_dispatcher = {
    'l15_unpacking_TT': l15_unpacking_TT,
    'l15_antikt4_TT_em': l15_antikt4_TT_em,
    'l15_antikt4_TT_had': l15_antikt4_TT_had,
    'l15_antikt10_TT_em': l15_antikt10_TT_em,
    'roi': roi,
    'superRoIMaker': superRoIMaker,

    # CellMaker care about fullcalo can be optimized for full or RoI retrieval
    'cellMaker_superPS_topo':cellMaker_superPS_topo,
    'cellMaker_fullcalo_topo': cellMaker_fullcalo_topo,

    # look into why topocluster_maker cares whether it is full calo PS 5/3/2014
    'topoClusterMaker_partial': topoClusterMaker_partial,
    'topoClusterMaker_fullcalo': topoClusterMaker_fullcalo,

    'l15_hypo_implicit0': l15_hypo_implicit0,
    'l15_hypo_implicit1': l15_hypo_implicit1,

    # partial scan does not yet - use full scan until available PS 5/3/2014
    'jr_antikt4_tc_em': jr_antikt4_tc_em,
    'jr_antikt4_tc_had': jr_antikt4_tc_had,
    'jr_antikt10_tc_em': jr_antikt10_tc_em,
    'jr_antikt10_tc_had': jr_antikt10_tc_had,
    'jr_hypo_single': jr_hypo_single,
    'jr_hypo_multi': jr_hypo_multi,
    'jetRecDiagnostics': jetRecDiagnostics,
    'jetHypoDiagnostics': jetHypoDiagnostics,
    'clusterDiagnostics': clusterDiagnostics,
    'cellDiagnostics': cellDiagnostics,
    'roiDiagnostics': roiDiagnostics,
    }
