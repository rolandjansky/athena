#!/bin/csh -f

set arch = `root-config --libs | grep m64`
set arch = ( "$arch" `root-config --cflags | grep m64` )

if ( "$arch" != "" ) then
  echo "-m64"
else
  echo "-m32"
endif


