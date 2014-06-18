#!/usr/bin/awk -f

# This script selects from the wrapper file a particular training event based on event number
# and dumps it to screen
# Example usage:
# ./dump_event.awk -v evnum=12345 /path/to/wrapper/file.dat > myevent.dat

BEGIN{
  # can be overridden on command line via awk -v evnum=XXX
  if(evnum=="") evnum=780;
  print "evnum:",evnum > "/dev/stderr"
  dump=0
}

$1=="F" && evnum==$2{
  dump=1
}

dump==1{
  print $0
}

dump==1 && $1=="L" && evnum==$2{
  dump=0
  exit(0);
}
