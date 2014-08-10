##
## @file sercnvcnv.mk
##
## @brief This makefile fragment generates C++ code for Trigger
## converters for serialization 
##
##
## @author Jiri Masik <Jiri.Masik@cern.ch>
## a copy of poolcnv by RD Schaffer <R.D.Schaffer@cern.ch>
##
## $Id: sercnv.mk,v 1.1.1.1 2008-05-22 21:33:31 masik Exp $
##

../sercnv${libtag}/${CONSTITUENT}SerCnv.stamp :: ../sercnv${libtag}/${libtag}${NAME}SerCnv.h.stamp
	@touch ../sercnv${libtag}/${CONSTITUENT}SerCnv.stamp

# Setup a reset stamp to trigger a cleanup each time a new header file
# is added
../sercnv${libtag}/${CONSTITUENT}SerCnvReset.stamp :: $(bin)${libtag}${NAME}SerCnv.stamp  $(sercnvcnv_reset_stamp)
	@echo "----- RESET sercnv converter generation -----"
	@/bin/rm -rf ../sercnv${libtag}
	@if test ! -d ../sercnv${libtag} ; then mkdir -p ../sercnv${libtag}; fi
	@touch ../sercnv${libtag}/${CONSTITUENT}SerCnvReset.stamp

# Stamp per input .h file - used for reset
$(bin)${libtag}${NAME}SerCnv.stamp :: dirs
	@if test ! -f $(bin)${libtag}${NAME}SerCnv.stamp ; then touch $(bin)${libtag}${NAME}SerCnv.stamp; fi


# For each file, add converter and a line in _entries.cxx
../sercnv${libtag}/${libtag}${NAME}SerCnv.h.stamp :: ../sercnv${libtag}/${CONSTITUENT}SerCnvBegin.stamp  
	@echo "----- Create Sercnv converter for ${NAME} --------"
	@echo "----- 		        fullname ${FULLNAME} --------"
	@echo "----- 		     constituent ${CONSTITUENT} --------"
	@echo "----- types: ${types_with_namespace_${libtag}} --------"
	@if test ! -f ${FULLNAME}; then echo "===> ERROR: file ${FULLNAME} does not exist"; \
	exit 1; fi
	@if test ! -d ../sercnv${libtag} ; then mkdir -p ../sercnv${libtag}; fi
	@PKG=`dirname ${FULLNAME}`; PKG=`basename $${PKG}`; \
        NAMESPACE="" ; \
	if test ! -z "${types_with_namespace_${libtag}}" ; then \
	    for class_name in ${types_with_namespace_${libtag}} ; do  \
		TYPE=`echo $${class_name} | sed 's/^[a-zA-Z_0-9]*:://'` ; \
	        if test $${TYPE} = ${NAME} ; then \
	            NAMESPACE=`echo $${class_name} | sed 's/::[a-zA-Z_0-9]*$$/::/'` ; \
	        fi ; \
	    done ; \
	fi ; \
	if test ! -z "${mult_chan_types_}" ; then \
	    for class_name in ${mult_chan_types_} ; do  \
		TYPE=`echo $${class_name} | sed 's/^[a-zA-Z_0-9]*:://'` ; \
	        if test $${TYPE} = ${NAME} ; then \
	            MULTCHANTYPE="true" ; \
	        fi ; \
	    done ; \
	fi ; \
	if test ! -z "$${NAMESPACE}" ; then echo "Using namespace $${NAMESPACE}" ; fi ; \
	if test ! -z "$${MULTCHANTYPE}" ; then echo "----- Generating CondMultChanCollection converter for ${NAME} --------" ; fi ; \
	if test ! -f ../src/${NAME}SerCnv.cxx ;  then \
	    if test ! -z "$${MULTCHANTYPE}" ; then \
	        $(cmtexe) expand model "<ExampleMultChanClassSerCnv.h type=$${NAMESPACE}${NAME} name=${libtag}${NAME} package=$${PKG} />" >| ../sercnv${libtag}/${libtag}${NAME}SerCnv.h ; \
	    else \
	        $(cmtexe) expand model "<ExampleClassSerCnv.h type=$${NAMESPACE}${NAME} libtag=${libtag} name=${NAME} package=$${PKG} />" >| ../sercnv${libtag}/${libtag}${NAME}SerCnv.h ; \
	    fi ; \
	fi ; \
	#$(cmtexe) expand model "<ExampleItemList_joboptions.py item=$${NAMESPACE}${NAME} />" >> ../sercnv${libtag}/${package}ItemList_joboptions.py
	@if test ! -f ../src/${libtag}${NAME}SerCnv.cxx ;  then $(cmtexe) expand model '<ExampleClassSerCnv.cxx name=${libtag}${NAME} />' >| ../sercnv${libtag}/${libtag}${NAME}SerCnv.cxx ; fi 
	@$(cmtexe) expand model '<ExamplePackageSerCnv_entries_element.cxx libtag=${libtag} name=${libtag}${NAME} />' >> ../sercnv${libtag}/${libtag}${package}SerCnv_entries.cxx 
	@echo "#include \"${libtag}${NAME}SerCnv.h\"" >> ../sercnv${libtag}/${libtag}${package}SerCnv_entries.h
	@echo 'DECLARE_CONVERTER_FACTORY(${libtag}${NAME}SerCnv)' >> ../sercnv${libtag}/${libtag}${package}SerCnv_entries.h
	@echo "----- OK for ${NAME} converter --------"
	@touch ../sercnv${libtag}/${libtag}${NAME}SerCnv.h.stamp


