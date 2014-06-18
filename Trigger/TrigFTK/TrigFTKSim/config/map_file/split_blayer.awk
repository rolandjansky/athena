#!/usr/bin/awk -f 

# This script updates the rmap file so that barrel blayer sectors are split in the middle of the module
# effectively making 44 modules along phi (instead of only 22).

# usage: ./split_blayer.awk raw_11L.rmap > raw_11L.rmap.blaysplit

NF>3{
  $1=$1*1;
}

# double barrel blayer module number:
(NF>3 && $1==1 && $2==0 && $3==0) {
  $4=$4*2;
  $5=$5*2+1;
}

{
  print $0;
}
