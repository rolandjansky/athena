#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# @file   check_statuscode.py
# @brief  Check the log file for unchecked StatusCodes
# @author Frank Winklmeier
# $Id: check_statuscode.py,v 1.1 2008-08-06 12:33:08 fwinkl Exp $

import sys
import optparse

def checklog(logfile, quiet=False):
   """
   Check the log file for unchecked StatusCodes.
   Return: 0 if no unchecked StatusCodes found
           1 if unchecked StatusCodes were found
           2 other error
   """

   try:
      f = open(logfile,"r")
   except:
      if not quiet: print "Cannot open %s" % logfile
      return 2

   statuscodes = None
   for line in f:
      # Start of StatusCodesSvc report
      if line.startswith("Num | Func"):
         statuscodes = [line]
         continue

      # Continued or end of report
      if statuscodes:
         if line != "\n": statuscodes += [line]
         else: break

   if statuscodes:
      if not quiet:
         for l in statuscodes: print l,
      return 1
   else:
      if not quiet: print "No unchecked StatusCodes found."
      return 0
   
   
def main():

   parser = optparse.OptionParser(usage="%prog LOGFILE",
                                  description="Check log file for unchecked StatusCodes.")

   parser.add_option("-q", "--quiet", default = False,
                     action = "store_true",
                     help = "Do not print any messages")
   
   (opts, args) = parser.parse_args()

   if len(args)!=1:
      parser.error("Log file name required")

   return checklog(args[0], opts.quiet)


if __name__=="__main__":
   sys.exit(main())
   
