#!/bin/sh

# This is not an ART test, but a unit test

# This is a unit test of HLT Control Flow and should be moved to TriggerMenuMT

athena.py -l DEBUG --imf --threads=1 TrigUpgradeTest/EmuStepProcessingTest.py
