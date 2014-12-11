#!/usr/bin/env python

import sys
import config

csh = False

if len(sys.argv) < 2:
    print "Usage: ",sys.argv[0], " database [ variableSetName ]"
    sys.exit(1)
    
db = config.Configuration(sys.argv[1])

if len(sys.argv) > 2:
    if db.test_object('VariableSet', sys.argv[2]):
        vs = db.get_dal('VariableSet', sys.argv[2])
    else:
        print "No such VariableSet as:",sys.argv[2]
        sys.exit(1)
else:
    all_vs = db.get_dals('VariableSet')

    if len(all_vs) == 0:
        print "No VariableSet in database"
        sys.exit(0)
        
    if len(all_vs) > 1:
        print "More than one VariableSet in database, please specify which one to use:"
        for x in all_vs:
            print x.id

        sys.exit(1)
    else:
        vs = all_vs[0]

for v in vs.Contains:
    if csh:
        print 'setenv %s "%s"' % (v.Name, v.Value)
    else:
        print '%s="%s"; export %s' % (v.Name, v.Value, v.Name)
    
