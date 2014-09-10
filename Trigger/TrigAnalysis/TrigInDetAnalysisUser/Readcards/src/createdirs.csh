#!/bin/csh -f

set ARCH_TYPE = `./setarch.sh`

foreach dir ( obj lib exe )
# echo "checking ../$dir" 
  if !( -e ../$dir ) then
#   echo "making ../$dir" 
    mkdir ../$dir
  endif
  
# echo "checking ../$dir/$ARCH_TYPE"
  if !( -e ../$dir/$ARCH_TYPE ) then 
#   echo "making ../$dir/$ARCH_TYPE"
    mkdir ../$dir/$ARCH_TYPE
  endif
end


 
