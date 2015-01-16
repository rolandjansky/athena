# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from unittest import (TestSuite,
                      TestLoader,
                      TextTestRunner)

from AlgFactory_test import TestAlgFactory
from jetDefAlgToString_test import TestJetDefAlgToString
from Sequence_test import TestSequence
from JetSequencesBuilder_test import TestJetSequencesBuilder
from JetDef_test import TestJetDef
from InstantiatorFactory_test import TestInstantiatorFactory
from hypo_factory_test import Test_hypo_factory
from jetDefInstantiator_test import Test_jetDefInstantiator
from fexparams_factory_test import Test_fexparams_factory

test_cases = (TestAlgFactory,
              TestJetDefAlgToString,
              TestSequence,
              TestJetSequencesBuilder,
              TestJetDef,
              TestInstantiatorFactory,
              Test_hypo_factory,
              Test_jetDefInstantiator,
              Test_fexparams_factory,
          )

loader = TestLoader()
suite = TestSuite([loader.loadTestsFromTestCase(tc) for tc in test_cases])
runner = TextTestRunner(verbosity=2)
runner.run(suite)
