#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# @author Marcin Nowak
# @date 08.2012
# @brief example of how to extract data from Collections in Python
#
# this example/utility prints EventNumber and all file GUIDs for this Event
# supports queries (without trigger bits decoding though)
#

import ROOT
ROOT.gROOT.SetBatch(True)
import re,sys

def toiter (beg, end):
     while beg != end:
         #yield beg.__deref__()
         yield beg
         beg.__preinc__()
     return
 
def toiter1 (c):
     return toiter (c.begin(), c.end())


query_string = ''
for i in range(1, len(sys.argv) ):
    if sys.argv[i] == '-src':
        collection_name = sys.argv[i+1]
        i += 1
        collection_name = re.sub('\.root$', '', collection_name)
    if sys.argv[i] == '-query':
        query_string = sys.argv[i+1]
        i += 1
    if sys.argv[i] == '-h':
        print "Currently implemented options: -src -query"
    

colsvc = ROOT.pool.CollectionService()
read_only_flag = True
col=colsvc.open(collection_name, 'RootCollection', '', read_only_flag)

des = col.description()
attributes = map( lambda n: des.attributeColumn(n, 0).name(), range(0, des.numberOfAttributeColumns()) )
if 'RunNumber' not in attributes or 'EventNumber' not in attributes:
    print "RunNumber/EventNumber attribute not found in the collection"
    exit(1)

#for c in range(0, des.numberOfAttributeColumns()):
#    print "Attribute: ", des.attributeColumn(c, 0).name(), " type:", des.attributeColumn(c, 0).type()
#for c in range(0, des.numberOfTokenColumns()):
#    print "Reference: ", des.tokenColumn(c, 0).name()

query = col.newQuery()
query.selectAll();
query.setCondition( query_string )
#query.addToOutputList( des.attributeColumn(0, 0).name() )
#query.addToOutputList( des.attributeColumn(1, 0).name() )

cursor = query.execute()

if( cursor.next() ):
   row = cursor.currentRow()
   
   tlist = row.tokenList()
   alist = row.attributeList()
   print "Attributes # = ", alist.size(), "  References # = ", tlist.size()

   for i in range(0,alist.size()):
       attr_name = alist[i].specification().name()

   queryopt = ['RunNumber', 'EventNumber']

   attrdata_func_name = {}
   line = ''
   for atr in queryopt:
       attrdata_func_name[atr] = 'data<' + alist[atr].specification().typeName() + '>' 
       line +=  "%10.10s " % atr
   for t in toiter1(tlist):
       line +=  "%38.38s " % t.tokenName()
   print line

   has_row = True
   while has_row:
       alist = cursor.currentRow().attributeList()
       tlist = cursor.currentRow().tokenList()
       line = ''
       for atr in queryopt:
           line += "%10.10s " % str(getattr(alist[atr], attrdata_func_name[atr])())
       for i in range(0, tlist.size()):
           line +=  "%38.38s " % tlist[i].dbID()
       print line
       has_row = cursor.next()
    
col.close()
