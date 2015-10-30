#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""Tag difference between two releases"""

__author__ = "Frank Winklmeier"

import sys
import os
import subprocess as sp
import datetime
from TrigValTools.HTML import *

projects = ['DetCommon',
            'Core',
            'Conditions',
            'Event',
            'Reconstruction',
            'Simulation',
            'Trigger',
            'Analysis',
            'HLT',
            'P1HLT',
            'CAFHLT',
            'Production']

def getTagDiff(asetup_old, asetup_new, project=''):
   """Return tag diff as dictionary {Package : (project,tag_old,tag_new)}"""
   
   import PyCmt.Cmt as Cmt

   if len(project)>0:
      asetup_old += ','+project
      asetup_new += ','+project
      
   diff = Cmt.get_tag_diff(asetup_old, asetup_new, verbose=False)
   diff = dict([(d['full_name'],(d['ref_proj'],d['chk_proj'],d['ref'],d['chk'])) for d in diff
                if d['ref_proj']!='N/A' and d['chk_proj']!='N/A'])
   
   return diff
   

def tracDiff(pkg,tag1,tag2):
   leaf = pkg.split('/')[-1]
   return 'https://svnweb.cern.ch/trac/atlasoff/changeset?old_path=%s/tags/%s-%s&new_path=%s/tags/%s-%s' % (pkg,leaf,tag1,pkg,leaf,tag2)


def websvnDiff(pkg,tag1,tag2):
   leaf = pkg.split('/')[-1]   
   return 'https://svnweb.cern.ch/cern/wsvn/atlasoff?op=comp&compare[]=/%s/tags/%s-%s/&compare[]=/%s/tags/%s-%s/' % (pkg,leaf,tag1,pkg,leaf,tag2)

def tag2int(tag):
   f = tag.split('-')
   if len(f)==3: tag += '-00'
   try:
      n = int(tag.replace('-',''))
   except:
      n = 0
   return n


def makeHTML(diff):
   """Return diff rows as HTML"""

   rows = []
   for pkg in sorted(diff):
      proj1,proj2,tag1,tag2 = diff[pkg]
      c = 'red' if tag2int(tag2)<tag2int(tag1) else 'black'
      rows.append(TR(TD(pkg.split('/')[-1],title=pkg,width='500')
                     + TD(htmlify(proj1)+'&nbsp&nbsp')
                     + TD(htmlify(proj2)+'&nbsp&nbsp')                     
                     + TD(htmlify(tag1)+'&nbsp&nbsp')
                     + TD(FONT(htmlify(tag2),color=c))
                     + TD(A('[trac]', href=tracDiff(pkg,tag1,tag2))
                          #+TEXT('&nbsp')+A('[websvn]', href=websvnDiff(pkg,tag1,tag2))
                          )
                     ))
      
   return Sum(rows)
   
 
def main():
   global projects
   
   import optparse
   parser = optparse.OptionParser(description=__doc__,
                                  usage='%prog [Options] REL1 REL2')

   parser.add_option('-t', '--today', action='store_true', default=False,
                     help="use today's nightly instead of specyfing rel_x")

   parser.add_option('-o', '--output', action='store',
                     help='html output file [default: stdout]')

   parser.add_option('-p', '--perProject', action='store',
                     help='do the tag diff for each project separately')
                     
                     
   (opts, args) = parser.parse_args()
   
   if len(args)!=2:
      parser.print_help()
      return 1

   old = args[0]
   new = args[1]
   
   if opts.today:
      nightly = 'rel_%d' % ((datetime.date.today().weekday()+1)%7)
      old += ','+nightly
      new += ','+nightly
      
   title = 'Tag differences %s vs. %s' % (old,new)
   html = HEAD(TITLE(title) +
               STYLE('tr:hover {background-color:#E6E6E6;}', type='text/css') +
               SCRIPT('',src='../js/sorttable.js'))
   
   html += H1(title)
   html += TEXT(I('Last update: %s' % datetime.datetime.today().strftime('%c')))
   html += BR() + BR()

   rows = []
   
   #for p in projects: html += A(p, href='#'+p) + TEXT('&nbsp')
   html += B('left: %s' % old)+BR()
   html += B('right: %s' % new)+BR()
   html += BR()

   if opts.perProject:
      diff = getTagDiff(old, new, 'AtlasHLT')
      diff2 = getTagDiff(old, new, 'AtlasAnalysis')
      diff3 = getTagDiff(old, new, 'AtlasSimulation')
      diff.update(diff2)
      diff.update(diff3)
   else:
      diff = getTagDiff(old, new)
      
   rows.append(TR(TD(B('Package')) + TD(B('left')) + TD(B('right')) + TD(B('left')) + TD(B('right'))))
   htmldiff = makeHTML(dict([x for x in diff.iteritems()]))
   if htmldiff!='': rows.append(htmldiff)

   out = '%s' % HTML(html+TABLE(Sum(rows),Class='sortable'))      
   if opts.output!=None:
      open(opts.output,'w').write(out)
   else:
      print out 
   
if __name__ == '__main__':
   try:
      sys.exit(main())
   except IOError, e:
      (code, msg) = e
      if (code==32): pass   # ignore broken pipe exception
      else: raise e
   except KeyboardInterrupt:
      sys.exit(1)
