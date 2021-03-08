#!/bin/bash

echo "Checking" $1
han-config-print $1 | grep 'BEGIN ASSESSMENTS of "top_level"' > /dev/null || exit 0
echo "Web display are being put into the top-level directory, this is not allowed"
echo "This usually means that the output= is not correctly specified"
echo "Now printing list of offending plots"
han-config-print $1 | sed '/BEGIN ASSESSMENTS of "top_level"/,/END ASSESSMENTS of "top_level"/!d;//d' | grep HanConfigAssessor
exit 1