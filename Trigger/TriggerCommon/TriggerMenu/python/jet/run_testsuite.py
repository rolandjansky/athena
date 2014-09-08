# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from unittest import (TestSuite,
                      TestLoader,
                      TextTestRunner)

from algFactories_test import TestAlgFactories
from jetDefAlgToString_test import TestJetDefAlgToString
from Sequence_test import TestSequence
from JetSequenceRouter_test import TestJetSequenceRouter
from JetDef_test import TestJetDef
from InstantiatorFactory_test import TestInstantiatorFactory
from hypo_factory_test import Test_hypo_factory
from jetDefInstantiator_test import Test_jetDefInstantiator

test_cases = (TestAlgFactories,
              TestJetDefAlgToString,
              TestSequence,
              TestJetSequenceRouter,
              TestJetDef,
              TestInstantiatorFactory,
              Test_hypo_factory,
              Test_jetDefInstantiator)

loader = TestLoader()
suite = TestSuite([loader.loadTestsFromTestCase(tc) for tc in test_cases])
runner = TextTestRunner(verbosity=2)
runner.run(suite)
