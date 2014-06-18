#!/usr/bin/awk -f

# this script reduces the number of subregions in pattern banks in factors of two
# Eg, from 4M to 2M, convert (sub0,sub2)->sub0; (sub1,sub3)->sub1

BEGIN {
  L=-1;
  if(maxpat=="" || maxpat=="-1") maxpat=100000000;
  if(factor=="") factor=2;
  if(unsplit_sectors=="") unsplit_sectors=0;
  ifile=-1;
  i=0;
  split("",out);
  print "BEGIN" > "/dev/stderr";
  print "BEGIN: unsplit_sectors = ", unsplit_sectors > "/dev/stderr"; 
  print "BEGIN: factor = ", factor > "/dev/stderr"; 
  print "BEGIN: maxpat = ", maxpat > "/dev/stderr"; 
};

NF==2{
  if(L!=-1 && L!=$2) {
    print "ERROR: inconsistent nlayers across files" > "/dev/stderr";
    exit(-1);
  }
  L=$2;
  ipat=0;
  if(ifile>=factor) {
    print "ERROR: number of files exceeds reduction factor" > "/dev/stderr";
    exit(-1);
  }
  ifile++;
}

NF>3 && ipat<maxpat{
  ## fix sector number
  if (unsplit_sectors==0) 
    $(L+2) = $(L+2)*factor + ifile;
  # fix ipat index (not that it matters at this point)
  $1 = i;
  print $0
  ipat++;
  i++;
}

END {
  print L > "info.L";
  print i > "info.N";
  print "DONE" > "/dev/stderr";
};
