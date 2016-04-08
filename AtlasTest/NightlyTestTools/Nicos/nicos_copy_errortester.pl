#!/usr/local/bin/perl
#
#-----------------------------------------------------------------------------
#
# nicos_copy_errortester.pl <file>
#
# Shell script to test logfiles of copy command (rsync)
#
# Arguments:   <package>  file name (with full path)
#
# Author:      A. Undrus
#-----------------------------------------------------------------------------
#
#   Define a grep command to avoid possible path problems when trying
#   to access gnu grep... (to be able to use -a flag...)
#
#
#   Get arguments:
#
use Env;
my $NICOS_HOME="$NICOS_HOME";
#
#-> check for correct number of arguments
#
    if ( "$#ARGV" ne 0 ){
    print "nicos_errortester:\n";
    print "One argument is required: name of logfile\n";
    exit 2;
    }
$filename=$ARGV[0];

my @e_patterns = ("can not", "permission denied", "Permission denied", "nicos_copy: problem");
my @e_ignore = ("CVBFGG", "CVBFGG", "CVBFGG", "CVBFGG");
my @e_count = (0,0,0,0);
if ( -f $filename ){
open(FL, "<$filename");
while (<FL>){ 
      chomp;
      $line=$_;
      for ($m=0; $m <= $#e_patterns; $m++){
        if ( $line =~ /\Q$e_patterns[$m]\E/ && $line !~ /\Q$e_ignore[$m]\E/) 
        {$e_count[$m]++; last;}
      }
} # while
      $eeee="";
      for ($m=0; $m <= $#e_patterns; $m++){
      $count=$e_count[$m];
      next if ($count <= 0); 
      $eeee="$e_patterns[$m]"; last;
      }
print "$eeee";
} else { # if ( -f $filename )
print "logfile not found: $filename";
}
 
