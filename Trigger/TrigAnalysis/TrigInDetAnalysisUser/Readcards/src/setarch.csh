#!/bin/csh -f


   
if ( `uname` == Darwin ) then 
  if ( `uname -p` == powerpc ) then  
    set arch = ppc
  else 
    if ( `uname -p` == i386 ) then  
      set arch = x86
    else
      set arch = unknown
    endif   
  endif
  
  set version = `uname -r | sed 's/\.//g'`
  echo ${arch}_darwin_${version}
  exit
else 
  if ( -e /usr/bin/fs ) then 
    fs sysname | awk '{print $4}' | sed "s/'//g"
  else
    echo `uname -p`-`uname -s`-`uname -r`
  endif
endif


