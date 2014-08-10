##
## @file sercnvcnv_header.mk
##
## @brief This makefile fragment generates C++ code for trigger converters 
##     for serialization
##
## There are three files: sercnv_header.mk, sercnv_trailer.mk, and
## sercnv.mk. The header does the global initialization, e.g. the
## creation of the first part of files. The sercnv.mk processes each
## file (${NAME}) listed in the document statement in the
## requirements file. The trailer defines the clean target. 
##
## @author Jiri Masik <Jiri.Masik@cern.ch>
## a copy of poolcnv @author RD Schaffer <R.D.Schaffer@cern.ch>
##
## $Id: sercnv_header.mk,v 1.1.1.1 2008-05-22 21:33:31 masik Exp $
##

#template_dir = ${ATLASPOOLROOT}/cmt/templates

${CONSTITUENT} :: ../sercnv${libtag}/${CONSTITUENT}SerCnvEnd.stamp

#Begin: create dir and first part of required files
../sercnv${libtag}/${CONSTITUENT}SerCnvBegin.stamp ::  ../sercnv${libtag}/${CONSTITUENT}SerCnvReset.stamp
	@echo "----- BEGIN sercnv converter generation -----"
	@if test ! -d ../sercnv${libtag} ; then mkdir -p ../sercnv${libtag}; fi
	@$(cmtexe) expand model '<ExamplePackageSerCnv_entries.h/>' >| ../sercnv${libtag}/${libtag}${package}SerCnv_entries.h
	@$(cmtexe) expand model '<ExamplePackageSerCnv_entries.cxx libtag=${libtag} package=${package} />' >| ../sercnv${libtag}/${libtag}${package}SerCnv_entries.cxx
	@$(cmtexe) expand model '<ExamplePackageSerCnv_load.cxx package=${package} />' >| ../sercnv${libtag}/${libtag}${package}SerCnv_load.cxx
	@if test ! -f ../share/${libtag}${package}_joboptions.py ;  then \
	    echo '# Just a dummy joboption - cnvs are auto-loaded' >> ../sercnv${libtag}/${libtag}${package}_joboptions.py ; \
	fi ; 
	@touch ../sercnv${libtag}/${CONSTITUENT}SerCnvBegin.stamp


#End: create final part of required files
../sercnv${libtag}/${CONSTITUENT}SerCnvEnd.stamp :: ../sercnv${libtag}/${CONSTITUENT}SerCnv.stamp 
	@echo "----- END sercnv converter generation -----"
	@echo '}' >> ../sercnv${libtag}/${libtag}${package}SerCnv_entries.cxx
	@touch ../sercnv${libtag}/${CONSTITUENT}SerCnvEnd.stamp


#=============================================
# begin
# nameSerCnv
# const
# End
#=============================================


#=============================================
# P_SerCnv_entries.h : H F F ...
#
# P_SerCnv_entries.cxx : H F F ... T
#
# P_SerCnv_load.cxx : H
#
# N_SerCnv.h : F
# N_SerCnv.cxx : F
#=============================================
