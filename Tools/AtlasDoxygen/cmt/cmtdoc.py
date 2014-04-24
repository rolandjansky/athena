# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#------------------------------------------------------------
#
# Script to generate HTML representation of the doxygen-like comments written in
# a CMT requirements file
#
# All documentation comments considered by this script must be prefixed by '##'
#
# Special actions are triggered by special keywords:
#
#   o a keyword must be the first word of a documentation comment.
#   o all documentation lines related with a given keyword must appear in
#     a single comment block (ie all lines in this block are prefixed with '##'
#   o a keyword always start with a '@' sign
#
#--------------------------------------------
#    KEYWORDS
#--------------------------------------------
#  @pattern <name>
#  doc line
#  doc line
#  doc line
#  @arg <name> <description>
#
#  Describe a CMT pattern and its arguments
#
#--------------------------------------------
#    USAGE
#--------------------------------------------
# o Since this script reads the requirements file, it must be activated while
#  being in the cmt directory of the package
# o The HTML format is generated to standard output
#
#  Typical usage:
#
# > cd ..../cmt
# > python /afs/cern.ch/atlas/scripts/cmtdoc.py >../doc/cmtdoc.html
# > vi ../doc/mainpage.h
# @htmlinclude cmtdoc.html
# > gmake doxygen
#
#------------------------------------------------------------
import sys
import string
import re


lines = []
in_pattern = False
in_define_pattern = False
docs = dict ()
defs = dict ()
arg_lists = dict ()
pattern_name = ''

comment_prefix = '##'
comment_prefix_length = len (comment_prefix)


def get_doc (lines):
    doc = []
    for line in lines:
        doc.append (string.rstrip (string.lstrip (line, '#')))
    return (doc)
    # end show_pattern

def html_begin_table ():
    print '<table width="100%" border="0" cellspacing="1" bgcolor="#000000" cellpadding="2">'
    print ''
    print '<tr valign="top" bgcolor="#eeeebb">'
    print '  <td><center><i>name</i></center></td>'
    print '  <td><center><i>description</i></center></td>'
    print '  <td><center><i>args</i></center></td>'
    print '</tr>'
    print ''

def html_pattern_row (name, docs, arg_lists):
    print '<tr valign="top" bgcolor="#ffffff">'
    print '  <td><tt><a href="#pattern ' + name + '">' + name + '</a></tt></td>'
    print '  <td>'
    if docs.has_key (name):
        for line in docs[name]:
            line = re.sub ('<', '&lt;', line)
            line = re.sub ('>', '&gt;', line)
            print line
    else:
        print '&nbsp;'
    print '</td>'
    print '  <td>'
    if arg_lists.has_key (name):
        print '<table>'
        arg_list = arg_lists[name]
        for arg in arg_list:
            line = arg_list[arg]
            line = re.sub ('<', '&lt;', line)
            line = re.sub ('>', '&gt;', line)
            print '<tr valign="top"><td width="60"><tt>' + arg + '</tt></td><td>' + line + '</td></tr>'
        print '</table>'
    else:
        print '&nbsp;'
    print "</tr>"

def html_end_table ():
    print '</table>';

def html_pattern_def (name, defs):
    print '<h2><a name="pattern ' + name + '"></a><tt>' + name + '</tt></h2>'
    print ''
    if defs.has_key (name):
        print '<pre><FONT color="green">'
        lines = defs[name]
        for line in lines:
            line = re.sub ('<', '&lt;', line)
            line = re.sub ('>', '&gt;', line)
            print line
        print '</FONT></pre>'
        print ''

#
# Main
#

f = open ('requirements')
for line in f.readlines ():
    w = string.split (line)
    if len(w) == 0:
        continue

    if line[0:comment_prefix_length] == comment_prefix:
        #
        # Doxygen comments may be inserted into CMT comment lines
        #
        #  First word may be a parameter
        #
        key = ' '
        if len(w) > 1:
            key = w[1]

        if key[0:1] == '@':
            #
            # Check the keyword
            #
            keyword = w[1]
            if keyword == '@pattern':
                if in_pattern:
                    # Any other keyword finishes that section
                    docs[pattern_name] = get_doc (lines)
                    in_pattern = False
                    lines = []

                # Syntax : @pattern <name>
                in_pattern = True
                pattern_name = w[2]
                continue
            if keyword == '@arg':
                if in_pattern:
                    # Syntax : @arg <name> <description>
                    if not arg_lists.has_key (pattern_name):
                        arg_lists[pattern_name] = dict ()

                    arg = w[2]
                    arg_line = string.join (w[3:])
                    arg_lists[pattern_name][arg] = arg_line
                    continue
        
            if in_pattern:
                # Any other keyword finishes that section
                docs[pattern_name] = get_doc (lines)
                in_pattern = False
                lines = []

        if in_pattern:
            lines.append (line)

        continue

    #
    # This is a normal CMT statement
    #
    if in_pattern:
        #
        # Previous line was inside a doxygen comment block, we must finish it
        #
        docs[pattern_name] = get_doc (lines)
        in_pattern = False
        lines = []
        
    #
    # Running definition
    #
    if in_define_pattern:
        line = string.rstrip (line)
        lines.append (line)
        if line[-1] == '\\':
            in_define_pattern = True
        else:
            in_define_pattern = False
            defs[pattern_name] = lines;
            lines = []
        continue
    
    #
    # Check the CMT keyword
    #
    if w[0] == 'pattern':
        line = string.rstrip (line)
        lines.append (line)
        name = w[1]
        if name == '-global':
            name = w[2]
                
        if line[-1] == '\\':
            pattern_name = name
            in_define_pattern = True
        else:
            in_define_pattern = False
            defs[name] = lines;
            lines = []
        continue


#------------------------------------------------------------
# The parsing is now finished
# Now generate HTML
#

#
# Generate the pattern table
#
html_begin_table ()
for key in defs:
    html_pattern_row (key, docs, arg_lists)
html_end_table ()

#
# Generate the pattern definition list
#
for key in defs:
    html_pattern_def (key, defs)
