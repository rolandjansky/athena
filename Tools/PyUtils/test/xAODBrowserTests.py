#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
from xAODBrowserTestElement import ElementTests
from xAODBrowserTestUtils import UtilsTests
from xAODBrowserTestRoot import OutputElementTests, OutputGeneratorTests
from xAODBrowserTestLogger import LoggerTests

if __name__ == '__main__':
    
    element_suite = unittest.TestLoader().loadTestsFromTestCase(ElementTests)
    
    utils_suite = unittest.TestLoader().loadTestsFromTestCase(UtilsTests)
    
    output_element_suite = unittest.TestLoader().loadTestsFromTestCase(OutputElementTests)
    output_generator_suite = unittest.TestLoader().loadTestsFromTestCase(OutputGeneratorTests)
    root_suite = unittest.TestSuite([output_element_suite, output_generator_suite])
    
    logger_suite = unittest.TestLoader().loadTestsFromTestCase(LoggerTests)
    
    # all_tests = unittest.TestSuite([element_suite, utils_suite, output_element_suite, output_generator_suite, logger_suite])
    # unittest.TextTestRunner(verbosity=2).run(all_tests)
    
    print('\n\nelement.py tests\n')
    unittest.TextTestRunner(verbosity=2).run(element_suite)

    print('\n\nutils.py tests\n')
    unittest.TextTestRunner(verbosity=2).run(utils_suite)

    print('\n\nroot.py tests\n')
    unittest.TextTestRunner(verbosity=2).run(root_suite)

    print('\n\nlogger.py tests\n')
    unittest.TextTestRunner(verbosity=2).run(logger_suite)
