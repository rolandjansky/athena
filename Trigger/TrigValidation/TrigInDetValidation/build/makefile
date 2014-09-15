##########################################################################
#
#   File:         makefile
#
#                 Copyright (C) M.Sutton (sutt@cern.ch) 
#   Description:  simply builds a global xml file from a collection of 
#                 separate xml files  
#                 
#
#   Created:    M.Sutton (sutt@cern.ch) Sun  3 Aug 2014 13:45:02 CEST 
#   Modified:     
#                   
#                   
######################################################################### 


JOBS =  \
        TrigInDetValidation_el_single_e_7-80_pu40.xml                   \
        TrigInDetValidation_el_single_e_7-80_pu40_run2.xml              \
        TrigInDetValidation_el_single_e_7-80_IBL_run2.xml               \
        TrigInDetValidation_el_single_e_7-80_IBL_run2_oldSeeding.xml    \
        TrigInDetValidation_el_single_e_7-80_IBL_pu40_run2.xml          \
        TrigInDetValidation_el_Zee_pu46_run2.xml                        \
        TrigInDetValidation_el_Zee_IBL_pu40_run2.xml                    \
        TrigInDetValidation_el_Zee_IBL_pu40_run2_oldSeeding.xml         \
        TrigInDetValidation_el_single_e_5_run2.xml                      \
        TrigInDetValidation_el_Zee_pu46.xml                             \
                                                                        \
        TrigInDetValidation_cosmic.xml                                  \
        TrigInDetValidation_cosmic_v0.xml                               \
                                                                        \
        TrigInDetValidation_bjet_run2.xml                               \
        TrigInDetValidation_bjet_pileUp.xml                             \
        TrigInDetValidation_bjet_pu46_run2.xml                          \
        TrigInDetValidation_bjet_IBL_pu40_run2.xml                      \
        TrigInDetValidation_bjet_IBL_pu40_run2_oldSeeding.xml           \
                                                                        \
        TrigInDetValidation_tau_pu46.xml                                \
        TrigInDetValidation_tau_pu46_run2.xml                           \
        TrigInDetValidation_tau_IBL_pu46_run2.xml                       \
        TrigInDetValidation_tau_IBL_pu46_run2_oldSeeding.xml            \
        TrigInDetValidation_tau_pileup.xml                              \
                                                                        \
        TrigInDetValidation_mu_Zmumu_pu40.xml                           \
        TrigInDetValidation_mu_Zmumu_pu40_run2.xml                      \
        TrigInDetValidation_mu_Zmumu_IBL_pu40_run2.xml                  \
        TrigInDetValidation_mu_Zmumu_IBL_pu40_run2_oldSeeding.xml       \
                                                                        \
        TrigInDetValidation_mu_bphys_run2.xml                           \
        TrigInDetValidation_mu_bphys_IBL_run2.xml                       \
        TrigInDetValidation_mu_bphys_IBL_run2_oldSeeding.xml            \
        TrigInDetValidation_mu_single_mu_100_run2.xml                   \
                                                                        \
        TrigInDetValidation_minBias.xml                                 \
        TrigInDetValidation_minBias_mergeDataPrep.xml                   \
        TrigInDetValidation_minBias_pileUp.xml                          \
                                                                        \
        TrigInDetValidation_aux.xml




FILES   = TrigInDetValidation_TestConfiguration_main.xml
XMLFILE = TrigInDetValidation_TestConfiguration.xml


install : all
	mv ../test/$(XMLFILE) ../test/$(XMLFILE).bak
	mv $(XMLFILE) ../test	

all: $(XMLFILE)

$(FILES) : $(JOBS)

$(XMLFILE) : clean $(FILES) $(JOBS)
	cpp -traditional-cpp $(FILES) -DSTAR="*" | grep -v "\# " | grep -v "emacs:" > /tmp/_$@
	cat /tmp/_$@ | grep -v \# > $@
	python /afs/cern.ch/user/r/rtt/public/validateXML.py $@

clean:
	rm -f $(XMLFILE) 



