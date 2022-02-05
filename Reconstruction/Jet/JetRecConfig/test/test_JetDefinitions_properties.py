#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# A test of the property setting/getting on JetDefinition classes
# We want the defining attributes on each class to trigger resets of
# derived attributes, such as container names, labels etc

from JetRecConfig.JetDefinition import *
import unittest

def printJetConstit(jetconstit):
    print(f"{jetconstit.name} constituents")
    print(f"    basetype:        {jetconstit.basetype}")
    print(f"    modifiers list:  {str(jetconstit.modifiers)}")
    print(f"    label:           {jetconstit.label}")
    print(f"    input container: {jetconstit.inputname}")
    if isinstance(jetconstit, JetInputConstitSeq):
        print(f"    output container: {jetconstit.containername}")
    print()


def printJetDefinition(desc, jetdef):
    print(f"{desc} jets")
    print(f"    fullname:       {jetdef.fullname()}")
    print(f"    algorithm:      {jetdef.algorithm}")
    print(f"    radius:         {jetdef.radius}")
    print(f"    input label:    {jetdef.inputdef.label}")
    print(f"    basename:       {jetdef.basename}")
    print(f"    modifiers list: {str(jetdef.modifiers)}")
    print()

# Start with a basic constituent and print some information

class TestJetDef(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.CHSPFlow = JetInputConstitSeq("chspflow",xAODType.FlowElement, [],
                                          "InputPFlow", "OutputPFlow", jetinputtype="EMPFlow"
                                          )
        cls.AntiKt4EMPFlow = JetDefinition("AntiKt",0.4,cls.CHSPFlow,ptmin=10e3,modifiers=["bla","bli"])
        

    def test_0build_JetInputConstitSeq(self):
        printJetConstit(self.CHSPFlow)

    def test_1setattr_JetInputConstitSeq(self):
        # make sure we can't set attributes which are not declared properties
        with self.assertRaises(AttributeError):
            # nameAA is not a property
            self.CHSPFlow.nameAA = 'not set'
            
    def test_2clone_JetInputConstitSeq(self):
        CSSKPFlow = self.CHSPFlow.clone(name="Simple chspflow clone")
        printJetConstit(CSSKPFlow)
        CSSKPFlow.modifiers += ["CS", "SK"]
        print("   --> after modifiers change, modifiers=", CSSKPFlow.modifiers)
        CSSKPFlow = self.CHSPFlow.clone(
            name="chspflow clone with args", modifiers=["CS", "SK"]
        )
        printJetConstit(CSSKPFlow)

        
        # make sure locked containers are locked
        CSSKPFlow_l = CSSKPFlow.clone(lock=True)
        with self.assertRaises(Exception):
            CSSKPFlow_l.modifiers = []

    def test_3print_JetDef(self):
        printJetDefinition("basic jetdef",self.AntiKt4EMPFlow)

    def test_4setattr_JetDef(self):
        # make sure we can't set attributes which are not declared properties
        with self.assertRaises(AttributeError):
            # nameAA is not a property
            self.AntiKt4EMPFlow.nameAA = 'not set'

    def test_4clone_JetDef(self):
        Ak4cloned = self.AntiKt4EMPFlow.clone( modifiers = ["Calib:T0:mc","Sort"] )
        printJetDefinition("cloned jet def",Ak4cloned)
        Ak4cloned_l = self.AntiKt4EMPFlow.clone( modifiers = ["Calib:T0:mc","Sort"] , lock=True)
        printJetDefinition("cloned - locked jet def",Ak4cloned)

        self.assertEqual( Ak4cloned.modifiers , Ak4cloned_l.modifiers) 

        with self.assertRaises(Exception):
            Ak4cloned_l.modifiers = []

    def test_5changename_JetDef(self):
        Ak4 = self.AntiKt4EMPFlow
        n0 = Ak4.fullname()
        print(" -> jet fullname ",n0)
        Ak4.radius = Ak4.radius*2
        n1 = Ak4.fullname()
        print(" -> jet fullname ",n1)
        Ak4.algorithm = 'Kt'
        n2 = Ak4.fullname()
        print(" -> jet fullname ",n2)
        self.assertNotEqual(n0,n1)
        self.assertNotEqual(n1,n2)

if __name__ == '__main__':
    unittest.main()
