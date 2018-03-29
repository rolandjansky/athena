


# get the list of jobs from the main RTT configuration job

JOBS = $(shell grep include  ../TIDAbuild/TrigInDetValidation_TestConfiguration_main.xml | grep -v "<\!-"  | sed 's|.* \"||g' | sed 's|\".*||g' ) 

# generate the target files with the tedious, pointless and arbitrary "only lower case" naming convention 

TARGETS=$(shell echo $(JOBS) | tr '[:upper:]' '[:lower:]' | sed 's|trigindet|../test/test_trigindet|g' | sed 's|\.xml|\.sh|g')

# default target

all : $(TARGETS)

# allow remapping of the proper job names to the lower case names
# NB: extraction of the data set now done in the mkart script

define MAPPER
../test/test_$(2) : ../TIDAbuild/$(1)
	../utils/mkart $$< -o .
	cp test_$(2) $$@
	chmod 755 $$@
endef


# actually make all the make rules

$(foreach i,$(JOBS),$(eval $(call MAPPER,$(i),$(shell echo $(i) | tr '[:upper:]' '[:lower:]' | sed 's|\.xml|\.sh|g'))))


clean: 
	rm -f ../test/test_*

test:
	echo $(TARGETS)




