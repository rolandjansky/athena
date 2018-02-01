#!/usr/bin/env bash
set -e
echo "Testing TrigGlobalEfficiencyCorrectionTool internal logic (standalone only)"
TrigGlobEffCorrValidation --quiet --fast --skip-if-athena
echo "Testing TrigGlobalEfficiencyCorrectionTool interactions with CP tools"
TrigGlobEffCorrExample4 --quiet
echo "Test successful."
exit 0
