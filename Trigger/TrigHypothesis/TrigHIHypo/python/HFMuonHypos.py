# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigHIHypo.TrigHIHypoConf import HFMuonHypo

hiHFMuonHypos  = { 'hf': HFMuonHypo('HFMuonHypo_nominal'),
                   'hfperf': HFMuonHypo('HFMuonHypo_perf', ElossCut=0.02) }
