#!/bin/sh
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# flake8: noqa
# This is just a hacky link to a renamed test script, which is needed until the test name in CI configuration is updated
test_trigAna_RDOtoRDOTrig_v1Dev_build.py
exit_code=$?
exit ${exit_code}
