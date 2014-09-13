# $Id: setup.csh,v 1.12 2007-02-05 22:16:41 jb Exp $
# csh style setup file to define environment variables for CoralDB

# For lxplus and other machines where you want to use the CERN AFS installation
set prefix=/afs/cern.ch/sw
set tnsnames=/afs/cern.ch/project/oracle/admin

# For machines with TDAQ installed
#set prefix=/daq
#set tnsnames=/afs/cern.ch/project/oracle/admin

set arch=slc3_ia32_gcc323
set lcgapp=$prefix/lcg/app/releases
set lcgext=$prefix/lcg/external

# The following environment variables will be used
setenv ARCH     $arch
setenv CORAL    $lcgapp/CORAL/CORAL_1_4_3
setenv SEAL     $lcgapp/SEAL/SEAL_1_8_1
# The following should not conflict with central settings on DAQ machines
setenv BOOST    $lcgext/Boost
setenv BOOSTVER $BOOST/1.33.1
setenv BOOSTINC $BOOSTVER/$ARCH/include/boost-1_33_1
setenv UUID     $lcgext/uuid/1.38
setenv PCRE     $lcgext/pcre/4.4
setenv XERCESC  $lcgext/XercesC/2.7.0
setenv SQLITE   $lcgext/sqlite/3.2.8
setenv ORACLE   $lcgext/oracle/10.2.0.1

# Setup run-time environment
setenv SEAL_PLUGINS $CORAL/$arch/lib/modules:$SEAL/$arch/lib/modules
if (! $?LD_LIBRARY_PATH) then
  setenv LD_LIBRARY_PATH
endif
setenv LD_LIBRARY_PATH $CORAL/$ARCH/lib:$SEAL/$ARCH/lib:$BOOSTVER/$ARCH/lib:$UUID/$ARCH/lib:$PCRE/$ARCH/lib:$XERCESC/$ARCH/lib:$SQLITE/$ARCH/lib:$ORACLE/$ARCH/lib:$LD_LIBRARY_PATH
setenv TNS_ADMIN $tnsnames

# Setup for PixLib clients (default setup scripts already assume that user sources are under
# $(HOME)/daq)
setenv CORAL_DB $HOME/daq/Applications/Pixel/CoralDB
