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


# TIDV = $(ATLAS_TEST_AREA)/Trigger/TrigValidation/TrigInDetValidation/TrigInDetValidation-00-07-18/build
# TIDV = $(ATLAS_TEST_AREA)/InstallArea/share/TIDAbuild
TIDV = ../../TrigInDetValidation/TIDAbuild


JOBS =  \
        $(TIDV)/TrigInDetValidation_el_Zee_IBL_pu40.xml                         \
        $(TIDV)/TrigInDetValidation_bjet_IBL_pu40.xml                           \
        $(TIDV)/TrigInDetValidation_tau_IBL_pu46.xml                            \
        $(TIDV)/TrigInDetValidation_mu_Zmumu_IBL_pu40.xml                       \
        $(TIDV)/TrigInDetValidation_mu_bphys_IBL.xml                            \
        $(TIDV)/TrigInDetValidation_minBias.xml                                 \
        $(TIDV)/TrigInDetValidation_FTK_mu_ttbar.xml                            


AUXFILES = $(TIDV)/TrigInDetValidation_aux.xml


VPATH += $(TIDV)
VPATH += scripts

IFILES = -I$(TIDV)

FILES   = TrigInDetValidationLight_TestConfiguration_main.xml
XMLFILE = TrigInDetValidationLight_TestConfiguration.xml

SCRIPTDIR = ../scripts

install : all
	mv ../test/$(XMLFILE) ../test/$(XMLFILE).bak
	mv $(XMLFILE) ../test	

all: $(XMLFILE)

$(FILES) : $(JOBS)

	cat /tmp/_$@ | grep -v \# > $@
	validateXML.py $@


$(XMLFILE) : clean $(FILES) $(JOBS) $(AUXFILES)
	cpp -traditional-cpp $(IFILES) $(FILES) | sed 's|STAR|\*|g' | grep -v "\# " | grep -v "emacs:" > /tmp/_$@
	cat /tmp/_$@ | grep -v \# > $@
	python /afs/cern.ch/user/r/rtt/public/validateXML.py $@


move : 
	mv ../test/$(XMLFILE) ../test/$(XMLFILE).bak
	mv $(XMLFILE) ../test	


scripts : $(patsubst %.xml,$(SCRIPTDIR)/%.sh,$(JOBS))

$(SCRIPTDIR)/%.sh : %.xml
	../utils/mkargs -o $(SCRIPTDIR) $< 


duff: 
	echo $(patsubst %.xml,../scripts/%.sh,$(JOBS))

clean:
	rm -f $(XMLFILE) 



