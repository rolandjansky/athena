#!/bin/sh
#/** @file post.sh
# @brief sh script that check the return code of an executable and compares 
# its output with a reference (if available).
# @param test_name 
#
# @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration.
# $Id: post_check.sh 542173 2013-03-24 05:46:57Z ssnyder $
# **/

PP="adummythatshouldntmatch"

#GUIDS
PP="$PP"'|[0-9A-F]\{8\}-[0-9A-F]\{4\}-[0-9A-F]\{4\}-[0-9A-F]\{4\}-[0-9A-F]\{12\}'
