# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# @file MissingETGoodness/python/METGoodnessGetter.py
# @author scott snyder <snyder@bnl.gov>
# @date May, 2010
# @brief Configure algorithms to calculate missing ET goodness.
#


from RecExConfig.Configured    import Configured
from RecExConfig.ObjKeyStore   import cfgKeyStore
from AthenaCommon.AlgSequence  import AlgSequence

class METGoodnessGetter ( Configured )  :
    def __init__ (self,
                  seq = AlgSequence(),
                  *args, **kw):
        self.seq = seq
        Configured.__init__ (self, *args, **kw)
        return

        
    def configure(self):
        from AthenaCommon.Logging import logging
        mlog = logging.getLogger( 'METGoodnessGetter::configure:' )
        mlog.info ('entering')

        import traceback
        try:
            from MissingETGoodness.MissingETGoodnessConf import \
                 EtmissGoodnessManagerAlg, \
                 JetGoodiesFiller, \
                 MetGoodiesFiller, \
                 MuonGoodiesFiller,\
                 METCollisionFilter

            # MET goodness - this can be used to configure and lock(!)
            # the MET goodness criteria.
            # also needed to reset the MET::goodies map for each event.
            manager = EtmissGoodnessManagerAlg (SetAndLockQuality = 0, # loose
                                                SetAndLockVersion = 20100415)
            self.seq += manager

            from MissingETGoodness.JetVarToolConfig import JetVarToolConfig
            JetVarToolConfig (self.seq)

            # Execute met and jet goodies fillers
            if cfgKeyStore.isInInput ('JetCollection', 'AntiKt4TopoJets'):
                primary_jet_alg = 'AntiKt4TopoJets'
                secondary_jet_alg = 'AntiKt4TopoJets'
            elif cfgKeyStore.isInInput ('JetCollection', 'Cone4H1TopoJets'):
                primary_jet_alg = 'AntiKt4TopoJets'
                secondary_jet_alg = 'AntiKt4TopoJets'
            else:
                primary_jet_alg = 'AntiKt4TopoEMJets'
                secondary_jet_alg = 'AntiKt4TopoEMJets'

            self.seq += JetGoodiesFiller(NamePrimaryJetAlg = primary_jet_alg,
                                         NameSecondaryJetAlg = secondary_jet_alg)
            self.seq += MetGoodiesFiller()

            muon_key = 'StacoMuonCollection'
            if not cfgKeyStore.isInInput ('Analysis::MuonContainer',
                                          muon_key):
                muon_key = 'Muons'

            self.seq += MuonGoodiesFiller(MuonContainerKey = muon_key)

            # collision filter, calo timing
            do_calo_time = False
            if cfgKeyStore.isInInput ('CaloCellContainer', 'AllCalo'):
                from LArCellRec.LArCollisionTimeGetter \
                     import LArCollisionTimeGetter
                LArCollisionTimeGetter (self.seq)
                do_calo_time = True

            self.seq += METCollisionFilter (DoCaloTimeFilter = do_calo_time)

            self._handle = manager

        except:
            mlog.error ("Error configuring METGoodness.") 
            traceback.print_exc()

        return True


    def handle (self):
        return self._handle

    # would work only if one output object type
    def outputKey(self):
        return None

    def outputType(self):
        return None

