#!/usr/bin/env bash
set -e
echo "Testing TrigGlobalEfficiencyCorrectionTool internal logic"
TrigGlobEffCorrValidation --quiet --fast
echo "Testing TrigGlobalEfficiencyCorrectionTool interactions with CP tools"
TrigGlobEffCorrExample4 --quiet
echo "Test successful."
exit 0
