SUSY11TriggerSkimmingList = ['HLT_j[0-9][0-9]$', 'HLT_j[0-9][0-9][0-9]$']
SUSY11TriggerThinningList = SUSY11TriggerSkimmingList + ['^(?!.*_[0-9]*(mu|j|xe|ht|xs|te|FTK))(?!HLT_tau.*_[0-9]*tau.*)HLT_tau.*medium.*tracktwo.*']
