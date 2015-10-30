#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# @file:    htmldiff.py
# @purpose: Produce HTML side-by-side diff (based on python's Tools/scripts/diff.py)
# @author:  Frank Winklmeier
#
# $Id: $

import sys, os, time, difflib, optparse

def main():

    usage = "usage: %prog [options] fromfile tofile"
    parser = optparse.OptionParser(usage)
    parser.add_option("-f", "--full", action="store_true", default=False,
                      help='Full diff instead of context diff')
    parser.add_option("-l", "--lines", type="int", default=3,
                      help='Number of context lines (default 3)')
    parser.add_option("-w", "--wrap", type="int", default=None,
                      help='Column number where lines are wrapped')
    
    (options, args) = parser.parse_args()

    if len(args) == 0:
        parser.print_help()
        sys.exit(1)
    if len(args) != 2:
        parser.error("need to specify both a fromfile and tofile")

    fromfile, tofile = args

    fromlines = open(fromfile, 'U').readlines()
    tolines = open(tofile, 'U').readlines()

    diff = difflib.HtmlDiff(wrapcolumn=options.wrap).make_table(fromlines,tolines,fromfile,tofile,
                                                                context=(not options.full),
                                                                numlines=options.lines)

    # The header and footer are slightly modified version of what
    # is created by HtmlDiff.make_file()
    print """
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
          "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
    <meta http-equiv="Content-Type"
          content="text/html; charset=ISO-8859-1" />
    <title></title>
    <style type="text/css">
        table.diff {font-family:monospace; border:medium;}
        .diff_header {background-color:#e0e0e0}
        td.diff_header {text-align:right}
        .diff_next {background-color:#c0c0c0}
        .diff_add {background-color:#aaffaa}
        .diff_chg {background-color:#ffff77}
        .diff_sub {background-color:#ffaaaa}
    </style>
</head>
<body>
"""
    # Main diff table
    sys.stdout.writelines(diff)
    print """
<hr>
<table class="diff" summary="Legends">
<tr> <th colspan="2"> Legends </th> </tr>
<tr> <td> <table border="" summary="Colors">
<tr><th> Colors </th> </tr>
<tr><td class="diff_add">&nbsp;Added&nbsp;</td></tr>
<tr><td class="diff_chg">Changed</td> </tr>
<tr><td class="diff_sub">Deleted</td> </tr>
</table></td>
<td> <table border="" summary="Links">
<tr><th colspan="2"> Links </th> </tr>
<tr><td>(f)irst change</td> </tr>
<tr><td>(n)ext change</td> </tr>
<tr><td>(t)op</td> </tr>
</table></td> </tr>

</body>
</html>
"""

if __name__ == '__main__':
    main()
