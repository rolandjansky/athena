# How to run the histogram updater
# root -l -q 'updateHists.C(input,stem,output="",outStem="")

#strip one histogram
#$ROOTSYS/bin/root -l -q 'updateHists.C("data09_900GeV.00140541.express_express.merge.HIST.f170_m255._0001.3","/run_140541/TAU/tauRec/tau_etaCalo")'

#strip one histogram to a different name
#$ROOTSYS/bin/root -l -q 'updateHists.C("data09_900GeV.00140541.express_express.merge.HIST.f170_m255._0001.3","/run_140541/TAU/tauRec/","","/run_140541/TAU/tauRec/tau_etaCalo_test")'

#strip a directory with a different name
#$ROOTSYS/bin/root -l -q 'updateHists.C("data09_900GeV.00140541.express_express.merge.HIST.f170_m255._0001.3","/run_140541/TAU/","","/run_test/TAU/")'

#strip a directory
#$ROOTSYS/bin/root -l -q 'updateHists.C("data09_900GeV.00140541.express_express.merge.HIST.f170_m255._0001.3","/run_140541/TAU/")'

#strip a directory and append it to a given file
#$ROOTSYS/bin/root -l -q 'updateHists.C("data09_900GeV.00140541.express_express.merge.HIST.f170_m255._0001.3","/run_140541/TAU/","output.root")'


root -l -q 'updateHists.C("'$1'","'$2'","","'$3'")'


