
NEEDS_F90 := yes

CXXFLAGS += -DHAVE_NEW_IOSTREAMS -DHAVE_CXX_STDC_HEADERS

# Source files to compile
FILES := mb_modules/*.F90
FILES += mb_gen_vol/*.F90
FILES += mb_xml/*.F90
FILES += mb_bool/*.F90
FILES += mb_uti/*.F90
FILES += mb_rec/*.F90
FILES += mb_fit/*.F90
FILES += mb_amd/*.F90
FILES += mb_mat/*.F90
FILES += mb_demo/*.F90
FILES += mb_covmat/*.F90
FILES += mb_resolu/*.F90
FILES += cpp_any/*.F90     cpp_any/*.cxx
FILES += mb_std_alone/aa_persint.F90
FILES += mb_std_alone/aa_conddb.F90
FILES += mb_std_alone/readmbcontrol.F90
FILES += mb_std_alone/readtags.F90
FILES += mb_std_alone/getevt.F90
FILES += mb_std_alone/gevtdumpdi.F90
FILES += mb_std_alone/keepstation.F90
FILES += mb_std_alone/sethitfile.F90
FILES += mb_std_alone/getevtdefault.F90
FILES += mb_std_alone/rfttubcs.F90
FILES += mb_std_alone/downloadcdb.cxx
FILES += mb_std_alone/downloadcdb.h
FILES += mb_std_alone/getcdbvar.cxx
FILES += mb_std_alone/getcdbvar.h
FILES += mb_std_alone/cdbvar.F90
FILES += mb_std_alone/MuonboyBFStand.h
FILES += mb_std_alone/MuonboyBFStand.cxx

USEWILDCARD := 1

# Header files to use for dictionary generation
# In this module: none
DICTFILES := 

# Executable files
PROGRAMS := 

# Set version appropriately
ifeq "$(VERSION)" "0.0" # default value from the Makefile
  VERSION := $(shell grep '^ *Iversion' MuonboyCore/mb_rec/get_muonboy_version.F90 | awk '{print $$3}')
endif

# This module needs LAPACK
#NEEDS_LAPACK := yes
