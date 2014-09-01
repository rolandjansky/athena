#!/bin/sh -xv
#/** @file dumAttr.sh
# @Dump out attributes from NewEventCollection1 to output log and run post.sh
#
# @author RD Schaffer <R.D.Schaffer@cern.ch> - ATLAS Collaboration.
# $Id: diff_check.sh,v 1.1 2005-03-11 20:46:49 cranshaw Exp $
# **/
test=$1
select=$2
# check exit status
joblog=${test}.log
CollListAttrib.exe -src NewEventCollection2 RootCollection NewEventCollection1 RootCollection >> ${joblog}
source ../test/post_check_with_select.sh ${test} ${select}
status=$?

exit $status

