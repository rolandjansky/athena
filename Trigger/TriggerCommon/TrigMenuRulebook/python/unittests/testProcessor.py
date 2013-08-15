# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Run unit tests of the processRules classes, and process a simply
dummy menu"""

import processRules
import RuleClasses
import RepresentationClasses
from Physics_pp_v2_rules import rules
import sys
import unittest

configuration = {"l1_xml"                 : '/tmp/l1file.xml', 
                 "hlt_xml"                : '/tmp/hltfile.xml',
                 "scale_lumi"             : 1,
                 "target_lumi"            : 1000,
                 "target_filled"          : 10,
                 "target_empty"           : 3000,
                 "target_unpaired_iso"    : 80,
                 "target_unpaired_noniso" : 80,
                 "online_xml"             : 'TriggerCosts_1e33_v4.xml',
                 "online_lumi"            : 1000,
                 "online_filled"          : 1,
                 "online_empty"              : 3000,
                 "online_empty_after_filled" : 100,
                 "online_unpaired_iso"    : 8,
                 "online_unpaired_noniso" : 8,
                 "use_lowest_rule"        : True,
                 "quiet"                  : True,
                 "verbosity"              : 0,
                 }

dummy_l1 = ["L1_J30", "L1_RD0_FILLED", "L1_RD1_FILLED", "L1_RD0_EMPTY", "L1_RD1_EMPTY", "L1_RD0_UNPAIRED_ISO", "L1_RD0_FIRSTEMPTY"]
dummy_hlt = [ ("L2", "L2_j45", "L1_J30"),
              ("L2", "L2_b30", "L1_J30"),
              ("EF", "EF_j50_jetNoEF", "L2_j45"),
              ("EF", "EF_b30", "L2_b30"),
              ]

dummy_l1 += ["L1_TAU5_XE10"]
dummy_hlt += [ ("L2", "L2_tau12_loose_IdScan_xe15_noMu", "L1_TAU5_XE10"),
              ("L2", "L2_tau12_loose_xe15_noMu", "L1_TAU5_XE10"),
              ("L2", "L2_tau12_loose_xe20_noMu", "L1_TAU5_XE10"),
              ("L2", "L2_tauNoCut_hasTrk6_IdScan_xe20_noMu", "L1_TAU5_XE10"),
              ("L2", "L2_tauNoCut_hasTrk6_xe20_noMu", "L1_TAU5_XE10"),
              ("L2", "L2_tauNoCut_hasTrk9_xe20_noMu", "L1_TAU5_XE10"),
              ("EF", "EF_tau12_loose_IdScan_xe15_noMu", "L2_tau12_loose_IdScan_xe15_noMu"),
              ("EF", "EF_tau12_loose_xe15_noMu", "L2_tau12_loose_xe15_noMu"),
              ("EF", "EF_tau12_loose_xe20_noMu", "L2_tau12_loose_xe20_noMu"),
              ("EF", "EF_tauNoCut_hasTrk6_IdScan_xe20_noMu", "L2_tauNoCut_hasTrk6_IdScan_xe20_noMu"),
              ("EF", "EF_tauNoCut_hasTrk6_xe20_noMu", "L2_tauNoCut_hasTrk6_xe20_noMu"),
              ("EF", "EF_tauNoCut_hasTrk9_xe20_noMu", "L2_tauNoCut_hasTrk9_xe20_noMu")
              ]


def build_xml(dummy_l1, dummy_hlt, l1_file, hlt_file):
    """Build dummy menu XML files"""

    l1_file = open(l1_file, 'w')
    hlt_file = open(hlt_file, 'w')
    
    l1_file.write('<LVL1Config id="1" name="Physics_pp_v1">\n')
    l1_file.write('  <TriggerMenu name="Dummy_pp_v1">\n')
    for trig in dummy_l1:
        l1_file.write('    <TriggerItem name="%s">"\n' % trig)
        l1_file.write('    </TriggerItem>\n')
    l1_file.write('  </TriggerMenu>\n')
    l1_file.write('</LVL1Config>')

    hlt_file.write('<HLT_MENU>\n')
    hlt_file.write('  <CHAIN_LIST>\n')
    for (level, trig, lower) in dummy_hlt:
        hlt_file.write('    <CHAIN level="%s" chain_name="%s" lower_chain_name="%s">\n' % (level, trig, lower))
        hlt_file.write('    </CHAIN>\n')
    hlt_file.write('  </CHAIN_LIST>\n')
    hlt_file.write('</HLT_MENU>\n')
        
    l1_file.close()
    hlt_file.close()    

class TestReaderClass(unittest.TestCase):
    """Unit Testing for the rule reader"""
    def setUp(self):
        #Dummy configuration where everything has scaled by x10

        self.reader = processRules.RuleReader(configuration, rules)
    
    def test_items(self):
        """Check 1 to 1 correspondence of items in the reader and
        dummy menu"""

        self.assertTrue(set(self.reader.l1_items) == set(dummy_l1 
                                                         + ["L1_multiseed"]
                                                         + ["L1_unseeded"])
                        )

        self.assertTrue(set(self.reader.all_items) == set(dummy_l1
                                                          + [hlt for (lvl, hlt, lower) in dummy_hlt]
                                                          + ["L1_multiseed", "L1_unseeded", "L2_unseeded"])
                        )
    
    def test_ps_rule(self):
        """Check PS rules are applied correctly"""

        #PS rules should always return exactly their input
        dummy_node = RepresentationClasses.TriggerNode('L1_dummy', {'PS' : 1})
        self.assertTrue(self.reader.apply_rule(dummy_node, 1, 0.5, 1) == 1)

        dummy_node = RepresentationClasses.TriggerNode('L1_dummy', {'PS' : 5})
        self.assertTrue(self.reader.apply_rule(dummy_node, 1, 0.5, 1) == 5)

        #We're only keeping 3 sig figs
        dummy_node = RepresentationClasses.TriggerNode('L1_dummy', {'PS' : 12345})
        self.assertTrue(self.reader.apply_rule(dummy_node, 1, 0.5, 1) == 12300)

    def test_rate_rule(self):
        """Check the rate rules are applied correctly"""

        #Case A:
        #An item asking for 10 Hz, with 40 Hz input and a 50%
        #efficiency should produce a PS = 2
        dummy_node = RepresentationClasses.TriggerNode('L1_dummy', 
                                             RuleClasses.TriggerRule(rate=10)
                                             )
        self.assertTrue(self.reader.apply_rule(dummy_node, 40, 0.5, 1) == 2)

        #Case B:
        #The same as (A), asking for 50 Hz should be un-prescaled
        dummy_node = RepresentationClasses.TriggerNode('L1_dummy', 
                                             RuleClasses.TriggerRule(rate=50)
                                             )
        self.assertTrue(self.reader.apply_rule(dummy_node, 40, 0.5, 1) == 1)

        #Case C:
        #The same a (A), but with a maxRate above threshold should produce a PS = 1
        dummy_node = RepresentationClasses.TriggerNode('L1_dummy', 
                                             RuleClasses.TriggerRule(rate=10, maxRate=25)
                                             )
        self.assertTrue(self.reader.apply_rule(dummy_node, 40, 0.5, 1) == 1)

        #Case D:
        #The same a (C), but with a maxRate below threshold should produce a PS = 2
        dummy_node = RepresentationClasses.TriggerNode('L1_dummy', 
                                             RuleClasses.TriggerRule(rate=10, maxRate=15)
                                             )
        self.assertTrue(self.reader.apply_rule(dummy_node, 40, 0.5, 1) == 2)


    def test_input_rate_rule(self):
        """Check the inputRate rules are applied correctly"""

        #Case A:
        #An item asking for 10 Hz input with a 40 Hz L1 output should get a PS = 4
        dummy_node = RepresentationClasses.TriggerNode('L1_dummy', 
                                             RuleClasses.TriggerRule(inputRate=10)
                                             )
        self.assertTrue(self.reader.apply_rule(dummy_node, 40, 0.5, 1) == 4)

        #Case B:
        #An item asking for 10 Hz input with a 40 Hz L1 output only if the maxRate of 50 is exceeded should get PS = 1
        dummy_node = RepresentationClasses.TriggerNode('L1_dummy', 
                                             RuleClasses.TriggerRule(inputRate=10, inputMaxRate=50)
                                             )
        self.assertTrue(self.reader.apply_rule(dummy_node, 40, 0.5, 1) == 1)

        #Case B:
        #An item asking for 10 Hz input with a 40 Hz L1 output only if the maxRate of 30 is exceeded should get PS = 4
        dummy_node = RepresentationClasses.TriggerNode('L1_dummy', 
                                             RuleClasses.TriggerRule(inputRate=10, inputMaxRate=30)
                                             )
        self.assertTrue(self.reader.apply_rule(dummy_node, 40, 0.5, 1) == 4)

    def test_rerun_rule(self):
        """Check that rerun rules are properly handled"""
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy')
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy', RuleClasses.TriggerRule(PS=0, rerun=True))
        dummy_node_l1.ps = 1
        dummy_node_l2.ps = 1
        dummy_node_ef.ps = 0
        dummy_node_l2.add_daughter(dummy_node_ef)
        dummy_node_l1.add_daughter(dummy_node_l2)
        self.reader.optimize_node(dummy_node_l1)
        self.assertTrue(dummy_node_l1.ps == -1)
        self.assertTrue(dummy_node_l2.ps == 0)
        self.assertTrue(dummy_node_ef.ps == 0)
        
    def test_rule_errrors(self):
        """Check the error conditions are properly handled"""
        dummy_node = RepresentationClasses.TriggerNode('L1_multiseed', 
                                             RuleClasses.TriggerRule(rate=10)
                                             )
        self.assertRaises(ValueError, self.reader.apply_rule, dummy_node, 1, 0.5, 1)

    def test_optimization(self):
        """Check the optimization conditions"""
        #Case A:
        #An EF trigger has a PS, no lower chains have rules, so move
        #the PS to L1
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy')
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy', RuleClasses.TriggerRule(PS=10))
        dummy_node_ef.l1 = 1
        dummy_node_ef.l2 = 1
        dummy_node_ef.ps = 10
        dummy_node_l2.add_daughter(dummy_node_ef)
        dummy_node_l1.add_daughter(dummy_node_l2)
        self.reader.optimize_node(dummy_node_l1)
        self.assertTrue(dummy_node_l1.ps == 10)
        self.assertTrue(dummy_node_l2.ps == 1)
        self.assertTrue(dummy_node_ef.ps == 1)

        #Case B:
        #An EF trigger has a PS, so does the L1, so move the PS to L2
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy', RuleClasses.TriggerRule(PS=2))
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy')
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy', RuleClasses.TriggerRule(PS=10))
        dummy_node_l1.ps = 2
        dummy_node_ef.ps = 10
        dummy_node_l2.add_daughter(dummy_node_ef)
        dummy_node_l1.add_daughter(dummy_node_l2)
        self.reader.optimize_node(dummy_node_l1)
        self.assertTrue(dummy_node_l1.ps == 2)
        self.assertTrue(dummy_node_l2.ps == 10)
        self.assertTrue(dummy_node_ef.ps == 1)

        #Case D:
        #A node has two EF prescales, which are propagated down to L1,
        #taking into account rounding
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy')
        dummy_node_ef_a = RepresentationClasses.TriggerNode('EF_dummy_a', RuleClasses.TriggerRule(PS=5.66))
        dummy_node_ef_b = RepresentationClasses.TriggerNode('EF_dummy_b', RuleClasses.TriggerRule(PS=10))
        dummy_node_ef_a.ps = 5.66
        dummy_node_ef_b.ps = 10
        dummy_node_l2.add_daughter(dummy_node_ef_a)
        dummy_node_l2.add_daughter(dummy_node_ef_b)
        dummy_node_l1.add_daughter(dummy_node_l2)
        self.reader.optimize_node(dummy_node_l1)
        self.assertTrue(dummy_node_l1.ps == 5)
        self.assertTrue(dummy_node_l2.ps == 1.13)
        self.assertTrue(dummy_node_ef_a.ps == 1)
        self.assertTrue(dummy_node_ef_b.ps == 1.77)

        #Case E:
        #An EF trigger is disabled, it's the only daughter, so
        #propagate it
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy')
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy', RuleClasses.TriggerRule(PS=-1))
        dummy_node_ef.ps = -1
        dummy_node_l2.add_daughter(dummy_node_ef)
        dummy_node_l1.add_daughter(dummy_node_l2)
        self.reader.optimize_node(dummy_node_l1)
        self.assertTrue(dummy_node_l1.ps == -1)
        self.assertTrue(dummy_node_l2.ps == -1)
        self.assertTrue(dummy_node_ef.ps == -1)                

        #Case F:
        #An EF trigger is disabled, a second EF trigger's PS
        #propagates down, the disabled one stays put
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy')
        dummy_node_ef_a = RepresentationClasses.TriggerNode('EF_dummy_a', RuleClasses.TriggerRule(PS=400))
        dummy_node_ef_b = RepresentationClasses.TriggerNode('EF_dummy_b', RuleClasses.TriggerRule(PS=-1))
        dummy_node_ef_a.ps = 400
        dummy_node_ef_b.ps = -1
        dummy_node_l2.add_daughter(dummy_node_ef_a)
        dummy_node_l2.add_daughter(dummy_node_ef_b)
        dummy_node_l1.add_daughter(dummy_node_l2)
        self.reader.optimize_node(dummy_node_l1)
        self.assertTrue(dummy_node_l1.ps == 400)
        self.assertTrue(dummy_node_l2.ps == 1)
        self.assertTrue(dummy_node_ef_a.ps == 1)
        self.assertTrue(dummy_node_ef_b.ps == -1)

        #Case G:
        #A fractional EF trigger is propagated down to L1
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy')
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy', RuleClasses.TriggerRule(PS=5.33))
        dummy_node_ef.ps = 5.33
        dummy_node_l2.add_daughter(dummy_node_ef)
        dummy_node_l1.add_daughter(dummy_node_l2)
        self.reader.optimize_node(dummy_node_l1)
        self.assertTrue(dummy_node_l1.ps == 5)
        self.assertTrue(dummy_node_l2.ps == 1.07)
        self.assertTrue(dummy_node_ef.ps == 1)

        #Case H:
        #A propagate flag has expressely been set to true
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy', RuleClasses.TriggerRule(PS=5))
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy', RuleClasses.TriggerRule(PS=10, propagate=True))
        dummy_node_l1.ps = 1
        dummy_node_l2.ps = 5
        dummy_node_ef.ps = 10
        dummy_node_l1.add_daughter(dummy_node_l2)
        dummy_node_l2.add_daughter(dummy_node_ef)
        self.reader.optimize_node(dummy_node_l1)
        self.assertTrue(dummy_node_l1.ps == 50)
        self.assertTrue(dummy_node_l2.ps == 1)
        self.assertTrue(dummy_node_ef.ps == 1)

        #Case I:
        #A propagate flag has expressely been set to false
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy', RuleClasses.TriggerRule(PS=5, propagate=False))
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy', RuleClasses.TriggerRule(PS=10))
        dummy_node_l1.ps = 1
        dummy_node_l2.ps = 5
        dummy_node_ef.ps = 10
        dummy_node_l1.add_daughter(dummy_node_l2)
        dummy_node_l2.add_daughter(dummy_node_ef)
        self.reader.optimize_node(dummy_node_l1)
        self.assertTrue(dummy_node_l1.ps == 1)
        self.assertTrue(dummy_node_l2.ps == 5)
        self.assertTrue(dummy_node_ef.ps == 10)

        #Case J:
        #A PT request sits below a disabled chain
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy', RuleClasses.TriggerRule(PS=1))
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy', RuleClasses.TriggerRule(PTRate=5))
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy', RuleClasses.TriggerRule(PS=-1))
        dummy_node_l1.ps = 1
        dummy_node_l2.ps = 1
        dummy_node_ef.ps = -1
        dummy_node_l1.rate = 1000
        dummy_node_l2.rate = 200
        dummy_node_ef.rate = 5
        dummy_node_l2.add_parent(dummy_node_l1)
        dummy_node_ef.add_parent(dummy_node_l2)
        dummy_node_l1.add_daughter(dummy_node_l2)
        dummy_node_l2.add_daughter(dummy_node_ef)
        self.reader.optimize_node(dummy_node_l1)
        self.assertTrue(dummy_node_l1.ps == 1)
        self.assertTrue(dummy_node_l2.ps == -1)
        self.assertTrue(dummy_node_ef.ps == -1)

        #Case K:
        #A PT request is on a disabled chain
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy', RuleClasses.TriggerRule(PS=1))
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy', RuleClasses.TriggerRule(PTRate=5, PS=-1))
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy', RuleClasses.TriggerRule(PS=-1))
        dummy_node_l1.ps = 1
        dummy_node_l2.ps = -1
        dummy_node_ef.ps = -1
        dummy_node_l1.rate = 1000
        dummy_node_l2.rate = 200
        dummy_node_ef.rate = 5
        dummy_node_l2.add_parent(dummy_node_l1)
        dummy_node_ef.add_parent(dummy_node_l2)
        dummy_node_l1.add_daughter(dummy_node_l2)
        dummy_node_l2.add_daughter(dummy_node_ef)
        self.reader.optimize_node(dummy_node_l1)
        self.assertTrue(dummy_node_l1.ps == 1)
        self.assertTrue(dummy_node_l2.ps == -1)
        self.assertTrue(dummy_node_ef.ps == -1)                

    def test_dependencies(self):
        pass

    def test_defaults(self): 
        """Check default values are properly set for chains w/o rules"""
        #Case A:
        #No rules in a tree
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy')
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy')        

        dummy_node_l1.add_daughter(dummy_node_l2)
        dummy_node_l2.add_daughter(dummy_node_ef)

        self.reader.set_defaults( [dummy_node_l1] )

        self.assertTrue(dummy_node_l1.ps == -1)
        self.assertTrue(dummy_node_l2.ps == -1)
        self.assertTrue(dummy_node_ef.ps == -1)

        #Case B:
        #One rule at L1
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy', RuleClasses.TriggerRule(PS=10))
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy')
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy')        

        dummy_node_l1.add_daughter(dummy_node_l2)
        dummy_node_l2.add_daughter(dummy_node_ef)

        self.reader.set_defaults( [dummy_node_l1] )

        self.assertTrue(dummy_node_l1.ps != -1)
        self.assertTrue(dummy_node_l2.ps == -1)
        self.assertTrue(dummy_node_ef.ps == -1)

        #Case C:
        #One rule at L2
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy', RuleClasses.TriggerRule(PS=10))
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy')        

        dummy_node_l1.add_daughter(dummy_node_l2)
        dummy_node_l2.add_daughter(dummy_node_ef)

        self.reader.set_defaults( [dummy_node_l1] )

        self.assertTrue(dummy_node_l1.ps != -1)
        self.assertTrue(dummy_node_l2.ps != -1)
        self.assertTrue(dummy_node_ef.ps == -1)

        #Case D:
        #One rule at EF
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy')
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy', RuleClasses.TriggerRule(PS=10))

        dummy_node_l1.add_daughter(dummy_node_l2)
        dummy_node_l2.add_daughter(dummy_node_ef)

        self.reader.set_defaults( [dummy_node_l1] )

        self.assertTrue(dummy_node_l1.ps != -1)
        self.assertTrue(dummy_node_l2.ps != -1)
        self.assertTrue(dummy_node_ef.ps != -1)

        #Case E:
        #One sub-branch has no rules, another sub-branch has one rule
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2_a = RepresentationClasses.TriggerNode('L2_dummy_a')
        dummy_node_l2_b = RepresentationClasses.TriggerNode('L2_dummy_b')
        dummy_node_ef_a1 = RepresentationClasses.TriggerNode('EF_dummy_a1')
        dummy_node_ef_b1 = RepresentationClasses.TriggerNode('EF_dummy_b1', RuleClasses.TriggerRule(PS=10))
        dummy_node_ef_b2 = RepresentationClasses.TriggerNode('EF_dummy_b2')
        
        dummy_node_l1.add_daughter(dummy_node_l2_a)
        dummy_node_l1.add_daughter(dummy_node_l2_b)
        dummy_node_l2_a.add_daughter(dummy_node_ef_a1)
        dummy_node_l2_b.add_daughter(dummy_node_ef_b1)
        dummy_node_l2_b.add_daughter(dummy_node_ef_b2)
        
        self.reader.set_defaults( [dummy_node_l1] )

        self.assertTrue(dummy_node_l1.ps != -1)
        self.assertTrue(dummy_node_l2_a.ps == -1)
        self.assertTrue(dummy_node_l2_b.ps != -1)
        self.assertTrue(dummy_node_ef_a1.ps == -1)
        self.assertTrue(dummy_node_ef_b1.ps != -1)
        self.assertTrue(dummy_node_ef_b2.ps == -1)

        #Case F:
        #A parent specifies a non-rate rule
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy', RuleClasses.TriggerRule(PTRate=1))
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy')

        dummy_node_l1.add_daughter(dummy_node_l2)
        dummy_node_l2.add_daughter(dummy_node_ef)

        self.reader.set_defaults( [dummy_node_l1] )

        self.assertTrue(dummy_node_l1.ps == -1)
        self.assertTrue(dummy_node_l2.ps == -1)
        self.assertTrue(dummy_node_ef.ps == -1)


    def test_es_and_pt(self):
        #Case A:
        #An EF item with 50% efficiency asks for a PTRate
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy', RuleClasses.TriggerRule(rate=600))
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy', RuleClasses.TriggerRule(rate=2.5,PTRate=0.5))
        dummy_node_l1.signature = {"prescale" : 1.0, "rate" : 600,}
        dummy_node_l2.signature = {"prescale" : 1.0, "rate" : 600,}
        dummy_node_ef.signature = {"prescale" : 1.0, "rate" : 300,}
        dummy_node_l1.ps = 1
        dummy_node_l2.ps = 1
        dummy_node_ef.ps = 120
        dummy_node_l1.add_daughter(dummy_node_l2)
        dummy_node_l2.add_daughter(dummy_node_ef)
        self.reader.compute_ES_and_PT({"l1" :dummy_node_l1})
        self.assertTrue(dummy_node_l1.pt == 0)
        self.assertTrue(dummy_node_l2.pt == 0)
        self.assertTrue(dummy_node_ef.pt == 10)

        #Case B:
        #An EF item with 50% efficiency asks for a ESRate
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy', RuleClasses.TriggerRule(rate=600))
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy', RuleClasses.TriggerRule(rate=2.5,ESRate=0.5))
        dummy_node_l1.signature = {"prescale" : 1.0, "rate" : 600,}
        dummy_node_l2.signature = {"prescale" : 1.0, "rate" : 600,}
        dummy_node_ef.signature = {"prescale" : 1.0, "rate" : 300,}
        dummy_node_l1.ps = 1
        dummy_node_l2.ps = 1
        dummy_node_ef.ps = 120
        dummy_node_l1.add_daughter(dummy_node_l2)
        dummy_node_l2.add_daughter(dummy_node_ef)
        self.reader.compute_ES_and_PT({"l1" :dummy_node_l1})
        self.assertTrue(dummy_node_l1.es == 0)
        self.assertTrue(dummy_node_l2.es == 0)
        self.assertTrue(dummy_node_ef.es == 5)

        #Case C:
        #An EF item asks for a ES rate less than what it's output will be
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy', RuleClasses.TriggerRule(rate=600))
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy', RuleClasses.TriggerRule(rate=1,ESRate=2))
        dummy_node_l1.signature = {"prescale" : 1.0, "rate" : 600,}
        dummy_node_l2.signature = {"prescale" : 1.0, "rate" : 600,}
        dummy_node_ef.signature = {"prescale" : 1.0, "rate" : 300,}
        dummy_node_l1.ps = 1
        dummy_node_l2.ps = 1
        dummy_node_ef.ps = 300
        dummy_node_l1.add_daughter(dummy_node_l2)
        dummy_node_l2.add_daughter(dummy_node_ef)
        self.reader.compute_ES_and_PT({"l1" :dummy_node_l1})
        self.assertTrue(dummy_node_l1.es == 0)
        self.assertTrue(dummy_node_l2.es == 0)
        self.assertTrue(dummy_node_ef.es == 1)

        #Case E:
        #A PT prescale is defined by "PTValue"
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy', RuleClasses.TriggerRule(rate=600))
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy', RuleClasses.TriggerRule(rate=1,PTValue=2))
        dummy_node_l1.signature = {"prescale" : 1.0, "rate" : 600,}
        dummy_node_l2.signature = {"prescale" : 1.0, "rate" : 600,}
        dummy_node_ef.signature = {"prescale" : 1.0, "rate" : 300,}
        dummy_node_l1.ps = 1
        dummy_node_l2.ps = 1
        dummy_node_ef.ps = 300
        dummy_node_l1.add_daughter(dummy_node_l2)
        dummy_node_l2.add_daughter(dummy_node_ef)
        self.reader.compute_ES_and_PT({"l1" :dummy_node_l1})
        self.assertTrue(dummy_node_l1.pt == 0)
        self.assertTrue(dummy_node_l2.pt == 0)
        self.assertTrue(dummy_node_ef.pt == 2)

        #Case F:
        #A ES prescale is defined by "ESValue"
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy', RuleClasses.TriggerRule(rate=600))
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy', RuleClasses.TriggerRule(rate=1,ESValue=2))
        dummy_node_l1.signature = {"prescale" : 1.0, "rate" : 600,}
        dummy_node_l2.signature = {"prescale" : 1.0, "rate" : 600,}
        dummy_node_ef.signature = {"prescale" : 1.0, "rate" : 300,}
        dummy_node_l1.ps = 1
        dummy_node_l2.ps = 1
        dummy_node_ef.ps = 300
        dummy_node_l1.add_daughter(dummy_node_l2)
        dummy_node_l2.add_daughter(dummy_node_ef)
        self.reader.compute_ES_and_PT({"l1" :dummy_node_l1})
        self.assertTrue(dummy_node_l1.es == 0)
        self.assertTrue(dummy_node_l2.es == 0)
        self.assertTrue(dummy_node_ef.es == 2)

        #Case G:
        #An L2 item with 50% efficiency asks for a PTRate, sitting under a disabled EF item
        dummy_node_l1 = RepresentationClasses.TriggerNode('L1_dummy')
        dummy_node_l2 = RepresentationClasses.TriggerNode('L2_dummy', RuleClasses.TriggerRule(PTRate=0.5))
        dummy_node_ef = RepresentationClasses.TriggerNode('EF_dummy', RuleClasses.TriggerRule(PS=-1))
        dummy_node_l1.signature = {"prescale" : 1.0, "rate" : 600,}
        dummy_node_l2.signature = {"prescale" : 1.0, "rate" : 300,}
        dummy_node_ef.signature = {"prescale" : 1.0, "rate" : 100,}
        dummy_node_l1.ps = 1
        dummy_node_l2.ps = 1
        dummy_node_ef.ps = -1
        dummy_node_l1.add_daughter(dummy_node_l2)
        dummy_node_l2.add_daughter(dummy_node_ef)
        dummy_node_l2.add_parent(dummy_node_l1)
        dummy_node_ef.add_parent(dummy_node_l2)
        self.reader.compute_ES_and_PT({"l1" :dummy_node_l1})
        self.assertTrue(dummy_node_l1.pt == 0)
        self.assertTrue(dummy_node_l2.pt == 1200)
        self.assertTrue(dummy_node_ef.pt == 0)
        
if __name__ == '__main__':

    #A small dummy menu        
    build_xml(dummy_l1, dummy_hlt,
              configuration["l1_xml"], configuration["hlt_xml"])
        
    
    #Run the tests
    suite = unittest.TestSuite()
    loader = unittest.TestLoader()
    suite.addTest(loader.loadTestsFromTestCase(TestReaderClass))
    unittest.TextTestRunner(verbosity=2).run(suite)
    
    sys.argv.append("--quiet")
    sys.argv.append("--lvl1-xml=%s" % configuration["l1_xml"])
    sys.argv.append("--hlt-xml=%s" % configuration["hlt_xml"])
    sys.argv.append("--use_lowest_rule")
    sys.argv.append("--target_lumi=1000")
    sys.argv.append("--log=testProcessor_output.log")
#     processRules.main()
