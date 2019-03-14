#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""Usage: trigconf_property.py [Options] COMPONENT PROPERTY SMK [SMK2 ...]')

Read trigger configuration properties from DB. SQL wildcards ('%') can be used to
print all matching components/properties.

Example: trigconf_property.py Trig% OutputLevel 1178
"""

__author__ = "Frank Winklmeier"

import sys
import os
from TrigConfigSvc.TrigConfigSvcUtils import getPropertyFromDB

def myhelp(option, opt_str, value, parser):
   """Custom help callback since optparse does not preserve newlines"""

   print __doc__   
   parser.print_help()
   sys.exit(1)

   
def main():
   import optparse
   parser = optparse.OptionParser(usage='', add_help_option=False)

   parser.add_option('-d', '--db', type='string', action='store', default='TRIGGERDB',
                     help='Trigger DB alias [%default]')

   parser.add_option('-s', '--short', action='store_true',
                     help='Only print property values')

   parser.add_option('--l2', action='store_true', help='Only show L2')
   parser.add_option('--ef', action='store_true', help='Only show EF')      
   parser.add_option("-h", "--help", action="callback", callback=myhelp)
   
   (opts, args) = parser.parse_args()
   
   if len(args)<3:
      parser.print_help()
      return 1

   comp = args[0]
   prop = args[1]
   smk = args[2:]
   
   res = getPropertyFromDB(opts.db, smk, comp, prop)
   if len(res)==0:
      print('Cannot find property in DB')
      return 2

   # Filter L2/EF
   if opts.l2==True and not opts.ef: res = filter(lambda x: x[4]=='L2',res)
   if opts.ef==True and not opts.l2: res = filter(lambda x: x[4]=='EF',res)
                              
   for p in res:
      if opts.short:
         print(p[3])
      else:
         print('%s %d %s %s %s' % (p[4],p[0],p[1],p[2],p[3]))
      
   
if __name__ == '__main__':
   try:
      sys.exit(main())
   except KeyboardInterrupt:
      sys.exit(1)
