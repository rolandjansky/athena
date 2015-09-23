##
## @file poolcnv.mk
##
## @brief This makefile fragment generates C++ code for AthenaPOOL
## converters and POOL IO handlers needed for element links
##
## See complete documentation in poolcnv_header.mk
##
## @author RD Schaffer <R.D.Schaffer@cern.ch>
##
## $Id: poolcnv.mk,v 1.8 2009-03-03 17:00:25 alibrari Exp $
##

../pool/${CONSTITUENT}Cnv.stamp : ../pool/${cnv_pfx}${NAME}Cnv.h.stamp

# Setup a reset stamp to trigger a cleanup each time a new header file
# is added
../pool/${CONSTITUENT}CnvReset.stamp : $(bin)${NAME}Cnv.stamp

# Stamp per input .h file - used for reset
$(bin)${NAME}Cnv.stamp :
	$(silent)touch $(bin)${NAME}Cnv.stamp

# For each file, add converter and a line in _entries.cxx
../pool/${cnv_pfx}${NAME}Cnv.h.stamp : ../pool/${CONSTITUENT}CnvBegin.stamp  
	$(echo) "----- Create Pool converter for ${NAME} --------"
	if test ! -f ${FULLNAME}; then echo "===> ERROR: file ${FULLNAME} does not exist"; \
	exit 1; fi
	PKG=`dirname ${FULLNAME}`; PKG=`basename $${PKG}`; \
        NAMESPACE="" ; \
	    for class_name in ${types_with_namespace_} ; do  \
		TYPE=`echo $${class_name} | sed 's/^[a-zA-Z_0-9]*:://'` ; \
	        if test $${TYPE} = ${NAME} ; then \
	            NAMESPACE=`echo $${class_name} | sed 's/::[a-zA-Z_0-9]*$$/::/'` ; \
	        fi ; \
	    done ; \
	    for class_name in ${mult_chan_types_} ; do  \
		TYPE=`echo $${class_name} | sed 's/^[a-zA-Z_0-9]*:://'` ; \
	        if test $${TYPE} = ${NAME} ; then \
	            MULTCHANTYPE="true" ; \
	        fi ; \
	    done ; \
	if test ! -z "$${NAMESPACE}" ; then echo "Using namespace $${NAMESPACE}" ; fi ; \
	if test ! -z "$${MULTCHANTYPE}" ; then echo "----- Generating CondMultChanCollection converter for ${NAME} --------" ; fi ; \
	if test ! -f ../src/${cnv_pfx}${NAME}Cnv.h ;  then \
	    if test ! -z "$${MULTCHANTYPE}" ; then \
	        $(cmtexe) expand model "<ExampleMultChanClassCnv.h type=$${NAMESPACE}${NAME} cnv_pfx=${cnv_pfx} name=${NAME} package=$${PKG} />" >| ../pool/${cnv_pfx}${NAME}Cnv.h ; \
	    else \
	        $(cmtexe) expand model "<ExampleClassCnv.h type=$${NAMESPACE}${NAME} cnv_pfx=${cnv_pfx} name=${NAME} package=$${PKG} />" >| ../pool/${cnv_pfx}${NAME}Cnv.h ; \
	    fi ; \
	fi ; \
	$(cmtexe) expand model "<ExampleItemList_joboptions.py item=$${NAMESPACE}${NAME} />" >| ../pool/${package}_$(NAME)ItemList_joboptions_py
	if test ! -f ../src/${cnv_pfx}${NAME}Cnv.h ;  then $(cmtexe) expand model '<ExampleClassCnv.cxx name=${NAME} />' >| ../pool/${cnv_pfx}${NAME}Cnv.cxx ; fi 
	$(cmtexe) expand model '<ExamplePackageCnv_entries_element.cxx name=${cnv_pfx}${NAME} />' >| ../pool/${package}_$(NAME)Cnv_entries_cxx 
	if [ -f ../pool/$(package)_$(NAME)Cnv_entries_h ]; then \rm -f ../pool/$(package)_$(NAME)Cnv_entries_h; fi && printf "#include \"${cnv_pfx}${NAME}Cnv.h\"\nDECLARE_CONVERTER_FACTORY(${cnv_pfx}${NAME}Cnv)\n" > ../pool/$(package)_$(NAME)Cnv_entries_h
	$(echo) "----- OK for ${NAME} converter --------"
	touch ../pool/${cnv_pfx}${NAME}Cnv.h.stamp
