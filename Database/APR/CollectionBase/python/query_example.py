# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT

colsvc = ROOT.pool.CollectionService()
read_only_flag = True
col=colsvc.handle('SimplePoolCollection1', 'RootCollection', '', read_only_flag)

des = col.description()
for c in range(0, des.numberOfAttributeColumns()):
    print "Attribute: ", des.attributeColumn(c, 0).name(), " type:", des.attributeColumn(c, 0).type()
for c in range(0, des.numberOfTokenColumns()):
    print "Reference: ", des.tokenColumn(c, 0).name()

query = col.newQuery()

#query.selectAll();
#query.addToOutputList( des.attributeColumn(0, 0).name() )
#query.addToOutputList( des.attributeColumn(1, 0).name() )

cursor = query.execute()

while( cursor.next() ):
    row = cursor.currentRow()
    tlist = row.tokenList()
    print tlist.size()
    print tlist[0].key()

    #  this crashes python  ???  MN
    #alist = row.attributeList()
    #print alist[0].data()
    #print "Attributes ", alist.size()

col.close()
