More info at https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/HLTConsistencyChecker

-----------------------------------------------------------------------------------------------
Introduction

This is a program designed to check the validity of the cuts made in egamma trigger algorithms 
If the ET cut used is inconsistent with the trigger name, an error is given.
If a cut is harder at L2 than EF, an error is given.
If a cut is harder at EF than offline, an error is given.
Details on trigger algorithms and parameters can be printed to screen.
Cuts of two triggers may be compared directly
Wildcarding is enabled
A photon (electron) trigger may be compared to all similar (in Et) electron (photon) triggers


-----------------------------------------------------------------------------------------------
Use

The program needs access to 3 xml files containing the trigger setup
AllMT_physicsV4_menu/l2_Default_setup.xml
AllPT_physicsV4_menu/ef_Default_setup.xml
AllPT_physicsV4_menu/outputHLTconfig_PT-1.xml
This can be done by linking to the ATN tests (see setup). If you want to look at different files, you can add them into the options


-----------------------------------------------------------------------------------------------
Setup on lxplus

> export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
> alias asetup='source $AtlasSetup/scripts/asetup.sh'
> asetup p1hlt,17.1.X.Y-VAL,Tue,here
And link to the ATN tests to gain access to the trigger xml files
> trigtestlink.py AllMT_physicsV4_menu
> trigtestlink.py AllPT_physicsV4_menu


-----------------------------------------------------------------------------------------------
How to run

The program is a one line python command
> python HLTConsistencyChecker.py -c 'EF_e24vhi_medium1'
where EF_e24vhi_medium1 can be any EF trigger where you'd like to check the L2 and EF cuts


-----------------------------------------------------------------------------------------------
Options

All the options can be seen by entering
python HLTConsistencyChecker.py -h

-t     The first trigger you would like to compare. Wildcards are allowed if you want to look at many triggers
-u     Optional. The second trigger you would like to compare. If not given, program will assume L2:EF comparison of first trigger
-l     The output level (DEBUG, INFO, WARNING, ERROR). Set to ERROR by default
-c     Optional. Use to turn on the cross electron/photon trigger chain comparisons. Best used with INFO logging
-o     Optional. Use to turn on the EF and offline comparisons.
-x     The HLT config xml file. Set to AllPT_physicsV4_menu/outputHLTconfig_PT-1.xml by default
-y     The L2 setup xml file. Set to AllMT_physicsV4_menu/l2_Default_setup.xml by default
-z     The EF setup xml file. Set to AllPT_physicsV4_menu/ef_Default_setup.xml by default


-----------------------------------------------------------------------------------------------
Technical info

In program sets up by getting a list of all the trigger chains from the config file. It also checks if the given trigger exists.
If the user has entered a wildcard, all the triggers matched to the wildcard will be examined.
If a second trigger (-u) has not been specified by the user, the program assumes a L2 vs EF check of the triggers.
If -l INFO has been given, the program will print all the parameter values compared for each trigger.
If -l DEBUG has been given, the program will print all the hypos and corresponding parameters for each trigger requested.
If -c and only one trigger has been given, the program will fetch all the similar electron (photon) triggers for that photon (electron) trigger. Only those similar in Et are compared. Differences will be printed in ERROR, but the output is best viewed in INFO for this option.
For each of the trigger chains under study, the Et cut applied is compared to the Et cut in the trigger name. An error is given if an inconsistency is found.
If -o has been given, the program will compare any EF trigger to the offline selection. If a cut is tighter at EF, an error will be printed.
For each of the trigger chains under study, the Hypo algorithms are fetched from the xml files.
Algorithms without Hypo in the name are ignored as they build the object and do not apply any cuts we are interested in. 
If a L2 vs EF comparison is needed, each Hypo algorithm at L2 is compared to each at EF using the compareAlgsL2EF function.
If a L2 vs L2 or EF vs EF comparison is needed, each Hypo algorithm is compared to another using the compareAlgs function.
