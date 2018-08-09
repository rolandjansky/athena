##########################################################################
#
#   File:         makefile
#
#                  
#   Description:  simply builds a global xml file from a collection of 
#                 separate xml files  
#                 
#
#   Created:    M.Sutton (sutt@cern.ch) Sun  3 Aug 2014 13:45:02 CEST 
#   Modified:     
#                   
#                   
######################################################################### 

SHELL = /bin/bash



JOBS =  \
	TrigInDetValidation_all_ttbar_pu40.xml                        \
	TrigInDetValidation_all_ttbar_pu60.xml                        \
	TrigInDetValidation_all_ttbar_pu80.xml                        \
                                                                      \
	TrigInDetValidation_beamspot_ttbar_pu40.xml                   \
	TrigInDetValidation_beamspot_ttbar_pu40_zfinder.xml           \
	TrigInDetValidation_beamspot_ttbar_pu80_zfinder.xml           \
                                                                      \
	TrigInDetValidation_el_Jpsiee_pu40.xml                        \
	TrigInDetValidation_el_single_e_7-80_larged0.xml              \
	TrigInDetValidation_el_single_e_7-80_larged0_PU.xml           \
	TrigInDetValidation_el_single_e_7-80_IBL.xml                  \
	TrigInDetValidation_el_single_e_7-80_IBL_pu40.xml             \
	TrigInDetValidation_el_Zee_IBL_pu40.xml                       \
	TrigInDetValidation_el_Zee_IBL_pu40_offline.xml               \
	TrigInDetValidation_el_Zee_IBL_pu40_offline_rzMatcher.xml     \
	TrigInDetValidation_el_Zee_IBL_pu40_rzMatcher.xml             \
	TrigInDetValidation_el_ttbar_pu40.xml                         \
	TrigInDetValidation_el_ttbar_pu80.xml                         \
                                                                      \
	TrigInDetValidation_cosmic.xml                                \
                                                                      \
	TrigInDetValidation_bjet_IBL_pu40.xml                         \
	TrigInDetValidation_bjet_IBL_pu40_offline.xml                 \
	TrigInDetValidation_bjet_IBL_pu80.xml                         \
	TrigInDetValidation_bjet_IBL_pu40_2016config.xml              \
	TrigInDetValidation_bjet_IBL_pu40_offline_2016config.xml      \
                                                                      \
	TrigInDetValidation_tau_IBL_pu46.xml                          \
	TrigInDetValidation_tau_IBL_pu46_offline.xml                  \
                                                                      \
	TrigInDetValidation_mu_single_mu_larged0.xml                  \
	TrigInDetValidation_mu_single_mu_larged0_PU.xml               \
	TrigInDetValidation_mu_Zmumu_IBL_pu40.xml                     \
	TrigInDetValidation_mu_Zmumu_IBL_pu40_offline.xml             \
	TrigInDetValidation_mu_Zmumu_IBL_pu40_offline_rzMatcher.xml   \
	TrigInDetValidation_mu_Zmumu_IBL_pu40_rzMatcher.xml           \
	TrigInDetValidation_mu_bphys_IBL.xml                          \
                                                                      \
	TrigInDetValidation_minBias.xml                               \
	TrigInDetValidation_minBias_offline.xml                       \
                                                                      \
	TrigInDetValidation_FTK_mu_ttbar.xml                          \
	TrigInDetValidation_FTK_tau_tautau.xml                        \
	TrigInDetValidation_FTK_bjet_ttbar.xml                        \
	TrigInDetValidation_FTK_beamspot_ttbar.xml                    \
	TrigInDetValidation_FTK_mu_ttbar_offline.xml                  \
	TrigInDetValidation_FTK_bjet_ttbar_offline.xml                



AUXFILES = TrigInDetValidation_aux.xml                  \
           TrigInDetValidation_expertMonitoring.xml     \
           TrigInDetValidation_expertMonitoring_run.xml \
           TrigInDetValidation_extra.xml


VPATH += scripts

FILES   = TrigInDetValidation_TestConfiguration_main.xml
XMLFILE = TrigInDetValidation_TestConfiguration.xml

SCRIPTDIR = ../scripts

install : all 

all: $(XMLFILE) artfiles checkdata

$(FILES) : $(JOBS)

#	cpp -traditional-cpp $(FILES) -DSTAR="*" | grep -v "\# " | grep -v "emacs:" > /tmp/_$@

$(XMLFILE) : clean $(FILES) $(JOBS) $(AUXFILES)
	cpp -traditional-cpp $(FILES) | sed 's|STAR|\*|g' | grep -v "\# " | grep -v "emacs:" > /tmp/_$@
	cat /tmp/_$@ | grep -v \# > $@
	python /afs/cern.ch/user/r/rtt/public/validateXML.py $@
	mv ../test/$(XMLFILE) ../test/$(XMLFILE).bak
	mv $(XMLFILE) ../test	


scripts : $(patsubst %.xml,$(SCRIPTDIR)/%.sh,$(JOBS))

$(SCRIPTDIR)/%.sh : %.xml
	../utils/mkargs -o $(SCRIPTDIR) $< 

artfiles : 
	../utils/mkartlist

checkdata : artfiles 
	../utils/submit.sh


duff: 
	echo $(patsubst %.xml,../scripts/%.sh,$(JOBS))

clean:
	rm -f $(XMLFILE) 



