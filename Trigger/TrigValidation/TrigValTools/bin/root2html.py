#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# @file:    root2html.py
# @purpose: Generate the index.htm for a list of root files
# @author:  Will Buttinger
#

html_template = """
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN">
<html lang="en">
   <head>
      <meta charset="UTF-8" http-equiv="X-UA-Compatible" content="IE=Edge">
      <title>Read a ROOT file</title>
      <link rel="shortcut icon" href="https://root.cern.ch/js/latest/img/RootIcon.ico">
      <!-- To make use of RequireJS, un-comment the following line and comment the second line below --> 
      <!--script type="text/javascript" src="//cdnjs.cloudflare.com/ajax/libs/require.js/2.1.19/require.min.js" data-main="scripts/JSRootCore.js"></script-->
      <script type="text/javascript" src="https://root.cern.ch/js/latest/scripts/JSRootCore.js?gui"></script>
   </head>
   <body>
      <div id="simpleGUI" path="./" files="%(file_list)s">
         loading scripts ...
      </div>
   </body>
</html>
"""


import sys

def main():
         
   # Parse arguments
   from optparse import OptionParser
   parser = OptionParser(usage = "usage: %prog [options] file1 [file2] ..." )
   
   (opts, args) = parser.parse_args()
   
   if len(args)<1:
      parser.print_help()
      return

   #create an index.htm with the input file and rootcomp.root
   d = {"file_list":";".join(args)}
   o_html = open('index.htm', 'w')
   o_html.writelines(html_template%d)
   o_html.flush()
   o_html.close()
   print("Wrote index.htm file")

   return 0


if __name__ == "__main__":
   sys.exit(main())
