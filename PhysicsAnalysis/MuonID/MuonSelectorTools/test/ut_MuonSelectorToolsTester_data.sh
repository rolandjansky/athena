#!/bin/sh

echo "Unit test for MuonSelectorTools on data"
echo "Test files:"
env | grep ASG_TEST_FILE_ | sort
echo "Will now run this command: MuonSelectorToolsTester $ASG_TEST_FILE_DATA"
MuonSelectorToolsTester $ASG_TEST_FILE_DATA
