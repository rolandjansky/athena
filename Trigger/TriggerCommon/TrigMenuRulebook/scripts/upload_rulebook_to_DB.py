# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#"""Usage
#eg. 
# for default rule book="Physics_pp_v2_rules" and memo="rules upload"
# > python upload_rulebook_to_DB.py
# for other rule files and more fancy memos
# > python upload_rulebook_to_DB.py "your_rule_file.py" "EF_mbSpTrk rules updated"
#"""

#!/usr/bin/env python2.6
import sys
import commands
import os

if len(sys.argv) > 1:
    rulebook=sys.argv[1]
    run_type=sys.argv[1].split("_")
else:
    rulebook="Cosmic_pp_v2_rules.py"
    run_type="Cosmic_pp_v2"
    
if len(sys.argv) > 2:
    memo=sys.argv[2]
else:
    memo="rules update"

print 'RuleBook upload to the DB for: %s with comment: %s and run type: %s' %(rulebook,memo,run_type[0])

command="python RuleClasses.py --upload=%s --memo=%s --run_type=%s'_pp_v2'" % (rulebook,memo,run_type[0])
os.system(command)
