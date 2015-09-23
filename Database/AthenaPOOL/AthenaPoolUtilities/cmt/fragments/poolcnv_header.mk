##
## @file poolcnv_header.mk
##
## @brief This makefile fragment generates C++ code for AthenaPOOL
## converters and POOL IO handlers needed for element links
##
## There are three files: poolcnv_header.mk, poolcnv_trailer.mk, and
## poolcnv.mk. The header does the global initialization, e.g. the
## creation of the first part of files. The poolcnv.mk processes each
## file (${NAME}) listed in the document statement in the
## requirements file. The trailer defines the clean target. 
##
## @author RD Schaffer <R.D.Schaffer@cern.ch>
##
## $Id: poolcnv_header.mk,v 1.6 2007-08-18 17:35:47 schaffer Exp $
##

template_dir = ${ATLASPOOLROOT}/cmt/templates

${CONSTITUENT} : ../pool/${CONSTITUENT}CnvEnd.stamp ;

#Begin: create dir and first part of required files
../pool/${CONSTITUENT}CnvBegin.stamp :  ../pool/${CONSTITUENT}CnvReset.stamp
	$(echo) "----- BEGIN pool converter generation -----"
	if test ! -d ../pool ; then mkdir -p ../pool; fi
	$(cmtexe) expand model '<ExamplePackageCnv_entries.h/>' >| ../pool/${package}Cnv_entries.h
	$(cmtexe) expand model '<ExamplePackageCnv_entries.cxx package=${package} />' >| ../pool/${package}Cnv_entries.cxx
	$(cmtexe) expand model '<ExamplePackageCnv_load.cxx package=${package} />' >| ../pool/${package}Cnv_load.cxx
	$(cmtexe) expand model "<ExampleItemList_joboptionsHdr.py package=${package} />" >| ../pool/${package}ItemList_joboptions.py
	if test ! -f ../share/${package}_joboptions.py ;  then \
	    echo '# Just a dummy joboption - cnvs are auto-loaded' >> ../pool/${package}_joboptions.py ; \
	fi ; 
	touch ../pool/${CONSTITUENT}CnvBegin.stamp

# Setup a reset stamp to trigger a cleanup each time a new header file
# is added
../pool/${CONSTITUENT}CnvReset.stamp : $(cmt_final_setup_${CONSTITUENT})
	$(echo) "----- RESET pool converter generation -----"
	/bin/rm -rf ../pool && \
	  mkdir -p ../pool && \
	  touch ../pool/${CONSTITUENT}CnvReset.stamp

../pool/${CONSTITUENT}Cnv.stamp :
	$(silent)touch ../pool/${CONSTITUENT}Cnv.stamp

#End: create final part of required files
../pool/${CONSTITUENT}CnvEnd.stamp : ../pool/${CONSTITUENT}Cnv.stamp 
	$(echo) "----- END pool converter generation -----"
	if [ "$(OBJS)" ]; then \
	  cat ../pool/$(package)_*Cnv_entries_h >>../pool/${package}Cnv_entries.h && \
	  cat ../pool/${package}_*Cnv_entries_cxx >>../pool/${package}Cnv_entries.cxx && \
	  cat  ../pool/${package}_*ItemList_joboptions_py >>../pool/${package}ItemList_joboptions.py && \
	  \rm -f ../pool/$(package)_*Cnv_entries_h \
	  ../pool/${package}_*Cnv_entries_cxx \
	  ../pool/${package}_*ItemList_joboptions_py; \
	fi
	echo '}' >> ../pool/${package}Cnv_entries.cxx
	touch ../pool/${CONSTITUENT}CnvEnd.stamp


#=============================================
# begin
# nameCnv
# const
# End
#=============================================


#=============================================
# P_Cnv_entries.h : H F F ...
#
# P_Cnv_entries.cxx : H F F ... T
#
# P_Cnv_load.cxx : H
#
# N_Cnv.h : F
# N_Cnv.cxx : F
#=============================================
