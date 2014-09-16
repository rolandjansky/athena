#!/bin/zsh

export dir1=initial
export dir2=final

echo  $dir1 $dir2

if [[ $# = 0 ]] then { 

  for i in $dir1/*.sql; 
  do 
    diff $i $dir2/`basename $i` > nule 
    set size=`wc -l nule`
    if [[ $argv[2] > 0 ]] then { echo `basename $i`}; fi;
    /bin/rm -f nule
  done;

# ACUT.sql
# AGDD.sql
# ALIN.sql
# ALMN.sql
# AMDC.sql
# APTP.sql
# ASMP.sql
# DBAM.sql

}; else {
 echo 'were removeved few scripts'

 /bin/rm -f $dir2/AGDD.sql 
 /bin/rm -f $dir2/ATYP.sql            
 /bin/rm -f $dir2/WMDT.sql            
 /bin/rm -f $dir2/WRPC.sql            
 /bin/rm -f $dir2/AWLN.sql            
 /bin/rm -f $dir2/WTGC.sql            
 /bin/rm -f $dir2/ATLN.sql            
 /bin/rm -f $dir2/GGLN.sql            
 /bin/rm -f $dir2/WCSC.sql            
 /bin/rm -f $dir2/WSPA.sql            
 /bin/rm -f $dir2/WDED.sql            
 /bin/rm -f $dir2/WCHV.sql            
 /bin/rm -f $dir2/WCMI.sql            
 /bin/rm -f $dir2/WCRO.sql            
 /bin/rm -f $dir2/WLBI.sql            
 /bin/rm -f $dir2/WSUP.sql

}; fi;
exit
