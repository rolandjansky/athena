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
        TrigInDetValidation_el_single_e_7-80_larged0.xml                \
        TrigInDetValidation_el_single_e_7-80_pu40_run1.xml              \
        TrigInDetValidation_el_single_e_7-80_pu40.xml                   \
        TrigInDetValidation_el_single_e_7-80_pu40_robustness.xml        \
        TrigInDetValidation_el_single_e_7-80_IBL.xml                    \
        TrigInDetValidation_el_single_e_7-80_IBL_pu40.xml               \
        TrigInDetValidation_el_single_e_7-80_IBL_pu40_robustness.xml    \
        TrigInDetValidation_el_Zee_pu46_run1.xml                        \
        TrigInDetValidation_el_Zee_IBL_pu40.xml                         \
        TrigInDetValidation_el_single_e_5.xml                           \
        TrigInDetValidation_el_Zee_pu46_run1.xml                        \
        TrigInDetValidation_el_ttbar_pu46.xml                           \
        TrigInDetValidation_el_ttbar_pu69.xml                           \
        TrigInDetValidation_el_ttbar_pu138.xml                          \
                                                                        \
        TrigInDetValidation_cosmic.xml                                  \
                                                                        \
        TrigInDetValidation_bjet.xml                                    \
        TrigInDetValidation_bjet_pu46.xml                               \
        TrigInDetValidation_bjet_IBL_pu40.xml                           \
                                                                        \
        TrigInDetValidation_tau_pu46_run1.xml                           \
        TrigInDetValidation_tau_pu46.xml                                \
        TrigInDetValidation_tau_IBL_pu46.xml                            \
                                                                        \
        TrigInDetValidation_mu_Zmumu_pu40_run1.xml                      \
        TrigInDetValidation_mu_Zmumu_pu40.xml                           \
        TrigInDetValidation_mu_Zmumu_IBL_pu40.xml                       \
                                                                        \
        TrigInDetValidation_mu_bphys.xml                                \
        TrigInDetValidation_mu_bphys_IBL.xml                            \
        TrigInDetValidation_mu_single_mu_100.xml                        \
        TrigInDetValidation_mu_single_mu_larged0.xml                    \
                                                                        \
        TrigInDetValidation_minBias.xml                                 \
        TrigInDetValidation_minBias_mergeDataPrep.xml                   \
        TrigInDetValidation_minBias_pileUp.xml                          \
                                                                        \
        TrigInDetValidation_FTK_mu_ttbar.xml                            \
        TrigInDetValidation_FTK_mu_tautau.xml                           \
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



