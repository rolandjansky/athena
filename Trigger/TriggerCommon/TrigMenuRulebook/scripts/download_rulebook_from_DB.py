# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#"""Usage
#eg. 
# for default rule book="Physics_pp_v2_rules" and only --active rules are dumped 
# > python download_rulebook_from_DB.py
# for other menus like Cosmic_pp_v2, Standby_pp_v2 etc
# > python download_rulebook_from_DB.py "Cosmic_pp_v2" 
#"""

#!/usr/bin/env python2.6
import sys
import commands
import os

if len(sys.argv) > 1:
    run_type=sys.argv[1]
    output_file=run_type+'_RULES.py'
else:
    run_type="Physics_pp_v2"
    output_file="Physics_pp_v2_RULES.py"
    
print 'RuleBook download from the DB for: %s' %(run_type)

command="python RuleClasses.py --run_type=%s --download=%s --active" % (run_type,output_file)
os.system(command)
