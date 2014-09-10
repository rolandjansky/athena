#!/bin/csh -f


# current working directory
if ( $#argv == 0 ) then 
  echo $cwd
else
  # just the directory name, not the leading path
  if ( "$1" == "-s" || "$1" == "--no-leading" ) then  
    echo $cwd:t
    exit
  endif

  # the parent directory
  if ( "$1" == "-p" || "$1" == "--parent" ) then  
    echo $cwd:h
    exit
  endif

  # just the name of the parent directory, 
  # not the leading path
  if ( "$1" == "-h" || "$1" == "--parent-no-leading" ) then  
    echo $cwd:h:t
    exit
  endif

  # usage message
  # if ( "$1" == "--help" ) then  
     echo "Usage: cwd.sh [-s | -p | -h]"
     echo "This is a directory name information provider for use with make"
     echo 
     echo "    <no option>                   print the name of the cwd"
     echo "       -s, --no-leading           print the name of the cwd, no leading path"
     echo "       -p, --parent               print the parent directory"
     echo "       -h, --parent-no-leading    parent directory name, no leading path"
     echo
  # endif

endif


  
