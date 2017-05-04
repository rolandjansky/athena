# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Unit testing for the rule classes"""

import xml.etree.cElementTree as ET
import RuleClasses
import RepresentationClasses
import unittest

empty_xml_text = "    <signature>\n\
    </signature>\n\
"

proper_l1_xml_text = "    <signature>\n\
      <sig_name>L1_J15</sig_name>\n\
      <sig_counter>0</sig_counter>\n\
      <evts_passed>0</evts_passed>\n\
      <evts_passed_weighted>0</evts_passed_weighted>\n\
      <rate>10.1</rate>\n\
      <rate_err>0</rate_err>\n\
      <prescale>1.0</prescale>\n\
      <passthrough>0</passthrough>\n\
      <lower_chain_name></lower_chain_name>\n\
      <efficiency>0</efficiency>\n\
      <efficiency_err>0</efficiency_err>\n\
      <prescaled_efficiency>0</prescaled_efficiency>\n\
      <prescaled_efficiency_err>0</prescaled_efficiency_err>\n\
    </signature>\n\
"

proper_l2_xml_text = "    <signature>\n\
      <sig_name>L2_j30</sig_name>\n\
      <sig_counter>0</sig_counter>\n\
      <evts_passed>0</evts_passed>\n\
      <evts_passed_weighted>0</evts_passed_weighted>\n\
      <rate>1.5</rate>\n\
      <rate_err>0</rate_err>\n\
      <prescale>5.0</prescale>\n\
      <passthrough>0.0</passthrough>\n\
      <lower_chain_name>L1_J15</lower_chain_name>\n\
      <efficiency>0</efficiency>\n\
      <efficiency_err>0</efficiency_err>\n\
      <prescaled_efficiency>0</prescaled_efficiency>\n\
      <prescaled_efficiency_err>0</prescaled_efficiency_err>\n\
    </signature>\n\
"

proper_ef_xml_text = "    <signature>\n\
      <sig_name>EF_j35_jetNoEF</sig_name>\n\
      <sig_counter>0</sig_counter>\n\
      <evts_passed>0</evts_passed>\n\
      <evts_passed_weighted>0</evts_passed_weighted>\n\
      <rate>0.43</rate>\n\
      <rate_err>0</rate_err>\n\
      <prescale>1.0</prescale>\n\
      <passthrough>0.0</passthrough>\n\
      <lower_chain_name>L2_j30</lower_chain_name>\n\
      <efficiency>0</efficiency>\n\
      <efficiency_err>0</efficiency_err>\n\
      <prescaled_efficiency>0</prescaled_efficiency>\n\
      <prescaled_efficiency_err>0</prescaled_efficiency_err>\n\
    </signature>\n\
"


class TestTriggerRuleClass(unittest.TestCase):
    """Test the TriggerRule class"""
    def test_empty(self):
        """Empty rules are not allowed"""
        #No creating empty rules
        self.assertRaises(ValueError, RuleClasses.TriggerRule)

    def test_collision(self):
        """Mutually contradicting terms should not be used"""
        #No double specifying rule items
        self.assertRaises(KeyError, RuleClasses.TriggerRule, **{'PS' : 1, 'rate' : 5})
        self.assertRaises(KeyError, RuleClasses.TriggerRule, **{'PS' : 1, 'fraction' : 5})

    def test_subdefined(self):
        """Qualifiers must not be specified without their bases"""
        #Must fully define a rule
        self.assertRaises(KeyError, RuleClasses.TriggerRule, **{'fraction' : 'L1_J5'})
        self.assertRaises(KeyError, RuleClasses.TriggerRule, **{'maxRate' : 5})

    def test_values(self):
        """Check 1 to 1 orrespondence of input to the rule content"""
        #Rule items should be set correctly
        first_values = dict(rate=1, maxRate=20, depends_on='L1_J5')
        second_values = dict(PS=5)

        first_rule = RuleClasses.TriggerRule(**first_values)
        self.assertTrue(first_rule.rules == first_values)

        second_rule = RuleClasses.TriggerRule(**second_values)
        self.assertTrue(second_rule.rules == second_values)

class TestTriggerSignatureClass(unittest.TestCase):
    """Test the TriggerSignature class"""
    def setUp(self):
        self.empty_xml = ET.fromstring(empty_xml_text)
        self.proper_xml = ET.fromstring(proper_ef_xml_text)
    

    def test_empty(self):
        """Disallow empty signatures, or signatures missing some required items"""
        self.assertRaises(KeyError, RepresentationClasses.TriggerSignature, self.empty_xml, 100)

    def test_values(self):
        """Check 1 to 1 correspondence of input to the signature content"""
        signature = RepresentationClasses.TriggerSignature(self.proper_xml, 100)
        for elem in self.proper_xml:
            if elem.tag not in signature.required_keys:
                continue

            func = str
            if elem.tag in signature.float_keys:
                func = float

            self.assertTrue(func(elem.text) == signature[elem.tag])

class TestTriggerNodeClass(unittest.TestCase):
    """Test the TriggerNode class"""

    def test_daughter(self):
        """Test daughter node handling"""
        node = RepresentationClasses.TriggerNode("parent")
        daughters = []
        ndaughters = 5
        for i in range(ndaughters):
            tmp_node = RepresentationClasses.TriggerNode("daughter_%s" % i)
            daughters.append(tmp_node)
            node.add_daughter(tmp_node)
            
        self.assertTrue(node.ndaughters() == ndaughters)
        
        counter = 0
        for (source, daughter) in zip(daughters, node.daughters):
            self.assertTrue(source == daughter)

            
    def test_parent(self):
        """Test parent node handling"""
        node = RepresentationClasses.TriggerNode("node")
        parent_1 = RepresentationClasses.TriggerNode("parent_1")
        parent_2 = RepresentationClasses.TriggerNode("parent_2")
        
        node.add_parent(parent_1)
        self.assertTrue(node.nparents() == 1)
        self.assertTrue(node.parents[0] == parent_1)
        self.assertRaises(KeyError, node.add_parent, parent_2)

    def test_total(self):
        """Test PS propagation up the tree"""
        node = RepresentationClasses.TriggerNode("node")
        parent = RepresentationClasses.TriggerNode("parent")
        
        node.add_parent(parent)
        node.ps = 5
        parent.ps = 10
        
        self.assertTrue(node.total_ps() == 50)
        self.assertTrue(parent.total_ps() == 10)

    def test_types(self):
        """Test lumi type scaling identifies"""
        node_multiseed = RepresentationClasses.TriggerNode("L1_multiseed")

        self.assertTrue(node_multiseed.is_multiseed())
        self.assertTrue(not node_multiseed.is_filled())
        self.assertTrue(not node_multiseed.is_empty())
        self.assertTrue(not node_multiseed.is_unpaired())
        self.assertTrue(not node_multiseed.is_unpaired_iso())
        self.assertTrue(not node_multiseed.is_unpaired_noniso())

if __name__ == '__main__':
    suite = unittest.TestSuite()
    loader = unittest.TestLoader()
    suite.addTest(loader.loadTestsFromTestCase(TestTriggerRuleClass))
    suite.addTest(loader.loadTestsFromTestCase(TestTriggerSignatureClass))
    suite.addTest(loader.loadTestsFromTestCase(TestTriggerNodeClass))
    unittest.TextTestRunner(verbosity=2).run(suite)
