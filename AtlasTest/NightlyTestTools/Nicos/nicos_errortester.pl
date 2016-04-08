#!/usr/local/bin/perl
#
#-----------------------------------------------------------------------------
#
# nicos_errortester.pl <package> <release>
#
# Shell script to test logfiles
#    code system. used to determine if rotation can be performed.
#
# Arguments:   <package>  Package name
#              <release>  Release name
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
use Cwd;
use File::Basename;
#
$short=0;
$specformat=0;
$testtesting=0;
$qatesting=0;

if ($#ARGV>=0) {
    $_ = $ARGV[0];
    /^-[qste]+$/ && /s/ && do { $short=1; };
    /^-[qste]+$/ && /e/ && do { $specformat=1; };
    /^-[qste]+$/ && /t/ && do { $testtesting=1; };
    /^-[qste]+$/ && /q/ && do { $qatesting=1; };
    /^-[qste]+$/ && shift;
    }
#
#-> check for correct number of arguments
#
my $NICOS_LOG="$NICOS_LOG";
my $NICOS_TESTLOG="$NICOS_TESTLOG";
my $NICOS_HOME="$NICOS_HOME";

if ( $testtesting eq 0 && $qatesting eq 0 ){
    $type="package";
    $NICOS_TESTLOGDIR=dirname($NICOS_LOG);
    if ( "$#ARGV" ne 2 ){
    print "nicos_errortester:\n";
    print "Three arguments required: names of package, release, pkg file\n";
    exit 2;
    }
}
else{
    $type="test";
    $type="qatest" if ($qatesting == 1);
    $NICOS_TESTLOGDIR=dirname(${NICOS_TESTLOG});
    $NICOS_TESTLOGDIR=dirname(${NICOS_QALOG}) if ($qatesting == 1);
    $NICOS_TESTLOGDIR_TEMP="$NICOS_TESTLOGDIR/temp";
    if ( "$#ARGV" ne 1 ){
    print "nicos_errortester:\n";
    print "Two arguments required: names of test, release @ARGV\n";
    exit 2;
}
}

$filename=$ARGV[2];
$compname=$ARGV[0];
$release=$ARGV[1];

my $TEST_SUCCESS_PATTERN="$NICOS_TEST_SUCCESS_PATTERN";
my $TEST_FAILURE_PATTERN="$NICOS_TEST_FAILURE_PATTERN";
my $TEST_WARNING_PATTERN="$NICOS_TEST_WARNING_PATTERN";
if ($qatesting == 1) {
    $TEST_SUCCESS_PATTERN="$NICOS_QA_SUCCESS_PATTERN";
    $TEST_FAILURE_PATTERN="$NICOS_QA_FAILURE_PATTERN";
    $TEST_WARNING_PATTERN="$NICOS_QA_WARNING_PATTERN";
}
my @e_patterns = ("not in", "Error ", "No rule to make target", "Error: path_not_found", "not remade because of errors");
my @e_ignore = ("CVBFGG", "ignored", "CVBFGG", "CVBFGG", "CVBFGG");
my @e_failure = ("${TEST_FAILURE_PATTERN}", "Traceback (most recent", "No rule to make target", "Symbol referencing errors", "No such file or directory");
my @e_warnings = ("${TEST_WARNING_PATTERN}", "CVBFGG", "CVBFGG", "CVBFGG", "CVBFGG");
my @e_success = ( "CVBFGG", "${TEST_SUCCESS_PATTERN}", "CVBFGG", "CVBFGG", "CVBFGG");
my @e_count = (0,0,0,0,0);
my @w_count = (0,0,0,0,0);
my @s_count = (0,0,0,0,0);
if ( $testtesting eq 1 || $qatesting eq 1 ){ 
@s_count = (0,1,0,0,0); 
$NICOS_TESTLOGDIR_TEMP="$NICOS_TESTLOGDIR/temp";
$file_er="${NICOS_TESTLOGDIR_TEMP}/${compname}_ERROR_MESSAGE";
$file_wa="${NICOS_TESTLOGDIR_TEMP}/${compname}_WARNING_MESSAGE";
$file_su="${NICOS_TESTLOGDIR_TEMP}/${compname}_SUCCESS_MESSAGE";
#print "$file_er $file_wa $file_su\n";
     if (-f ${file_er} ){
           open(UT, "$file_er"); 
           while (<UT>) {
           chomp;
           ( $lin1 = $_ ) =~ s/ //g;
           next if (length($lin1) eq 0);
           ( $lin2 = $_ ) =~ s/\n//g;
#           print "EEEE =$lin2=\n"; 
           push(@e_failure,$lin2);  
           $a=0;
           push(@e_count,$a);  
           close(UT);
           }
     }
     if (-f ${file_wa} ){
           open(UT, "$file_wa");
           while (<UT>) {
           chomp;
           ( $lin1 = $_ ) =~ s/ //g;
           next if (length($lin1) eq 0);
           ( $lin2 = $_ ) =~ s/\n//g;
           push(@e_warnings,$lin2);
           $a=0;
           push(@w_count,$a);
           close(UT);
           }
     }
     if (-f ${file_su} ){
           open(UT, "$file_su");
           while (<UT>) {
           ( $lin1 = $_ ) =~ s/ //g;
           next if (length($lin1) eq 0);
           ( $lin2 = $_ ) =~ s/\n//g;
           push(@e_success,$lin2);
#           print "SSSS ===${lin2}=====\n";
           $a=1;
           push(@s_count,$a);
           close(UT);
           } 
     }
}

#print " e_failure @e_failure : @e_count \n";
#print " e_warnings @e_warnings : @w_count : $TEST_WARNING_PATTERN\n";
#print " e_success @e_success : @s_count \n";

if ( $specformat eq 0 ){
    print "====================================================================\n";
    print "=== CHECK logfiles related to $compname in $release\n";
    print "====================================================================\n";
    print " \n";
}

if ( $testtesting eq 0 && $qatesting eq 0 ){ 
open(DBF,"$filename");
chomp(my @dbc=<DBF>);
close(DBF);
@list = grep /[^a-zA-Z0-9_]${compname}[^a-zA-Z0-9_]/, @dbc; 
$lll = join(' ', split(' ', $list[0]));
$tag = (split(" ", $lll))[1];
}

#
#
opendir(DD, ${NICOS_TESTLOGDIR});
@allfiles = readdir DD;
closedir DD;
$stg="${compname}.loglog";
@list = grep /^${stg}$/, @allfiles;
$listfiles = (sort { -M $a <=> -M $b } @list)[0];
#print " --- $compname --- $NICOS_TESTLOGDIR -- $listfiles\n";

   $file="$NICOS_TESTLOGDIR/$listfiles"; 
   if ( -f $file ){
   open(FL, "<$file");
   while (<FL>){ 
      chomp;
      $line=$_;
      if ( ${testtesting} eq 0 && ${qatesting} eq 0 ){
      for ($m=0; $m <= $#e_patterns; $m++){
        if ( $line =~ /\Q$e_patterns[$m]\E/ && $line !~ /\Q$e_ignore[$m]\E/) 
        {$e_count[$m]++;}
      }
      }
      else{     
        for ($m=0; $m <= $#e_failure; $m++){
        if ( $e_failure[$m] ne "" && $line =~ /\Q$e_failure[$m]\E/ ) 
                              {$e_count[$m]++;}
#        print "X $line\n";
#        print "YY $e_count[$m] XXX $e_failure[$m]\n";
      } # for
   } # if ( ${testtesting} eq 0

    if ( ${testtesting} ne 0 || ${qatesting} ne 0 ){

    for ($m=0; $m <= $#e_warnings; $m++){
        if ( $e_warnings[$m] ne "" && $line =~ /\Q$e_warnings[$m]\E/ )
	{$w_count[$m]++;}
    } # for

    for ($m=0; $m <= $#e_success; $m++){
#        print "X $line\n";
        if ( $line =~ /\Q$e_success[$m]\E/ && $s_count[$m] > -1 ) {$s_count[$m]--;}
#        print "YS $line VV $m $s_count[$m] XXX $e_success[$m]\n";
    } # for
    } # if ( ${testtesting} 

   } # while

      $eeee="";
      if ( ${testtesting} eq 0 && ${qatesting} eq 0 ){
      for ($m=0; $m <= $#e_patterns; $m++){
      $count=$e_count[$m];
      next if ($count <= 0); 
      $eeee="$e_patterns[$m]"; last;
      }
      }
      else
      {
      for ($m=0; $m <= $#e_failure; $m++){
      $count=$e_count[$m];
      next if ($count <= 0);
      if ( $e_failure[$m] ne "CVBFGG" ){
	$eeee="$e_failure[$m]"; last;
        }
      }
      }   

      $ssss="";
      for ($m=0; $m <= $#e_success; $m++){
      $count=$s_count[$m];
      next if ($count <= 0);
      if ( ${testtesting} ne 0 || ${qatesting} ne 0 ){
      $ssss="$e_success[$m]"."(ABSENSE OF)"; last;
      }
      }

      $wwww="";
      for ($m=0; $m <= $#e_warnings; $m++){
      $count=$w_count[$m];
      next if ($count <= 0);
             if ( $e_warnings[$m] ne "CVBFGG" ){
	     $wwww="$e_warnings[$m]"; last;
             }
      }
                if ( $eeee ne "" ){  
                if ( $specformat eq 0 ){
                    print "====================================================================\n";
                    print " $type $compname has problem. See \n";
                    print " $file \n";
                    print " for pattern(s)  ------ $eeee -----\n";
                    print "====================================================================\n";
		    } 
	        else{
#                    $varbase = basename($var);
                    print "G $compname $NICOS_TESTLOGDIR $eeee";
                }
                if ( $short eq 0 ){
                    exit 2;
                }
                } 
                elsif ( $ssss ne "" ) #if ( $eeee ne ""
                {
         	    if ( $specformat eq 0 ){
                    print "===================================================\
================\n";
                    print " $type $compname has problem. See \n";
                    print " $file \n";
                    print " for pattern(s)  ------ $ssss -----\n";
                    print "====================================================================\n";
	            }
		    else{
#			$varbase = basename($var);
			print "G $compname $NICOS_TESTLOGDIR $ssss";
		    }
		    if ( $short eq 0 ){
			exit 2;
		    }

		}
                else #if ( $eeee ne "" 
	        {
                if ( $wwww ne "" )
                {
		    if ( $specformat eq 0 ){
			print "====================================================================\n";
			print " $type $compname has warning. See \n";
			print " $file \n";
			print " for pattern(s)  ------ $wwww -----\n";
			print "====================================================================\n";
                    }
		    else{
#			$varbase = basename($var);
			print "W $compname $NICOS_TESTLOGDIR $wwww";
		    }
		    if ( $short eq 0 ){
			exit 2;
		    }
                } #if ( $wwww ne "" )
                } #if ( $eeee ne "" )

if ( $short eq 0 ){
    print "(..)(..)(..)(..)(..)(..)(..)(..)(..)(..)(..)(..)(..)(..)(..)\n";
    print "         Logfiles of $type $compname looks OK\n";
    print "(..)(..)(..)(..)(..)(..)(..)(..)(..)(..)(..)(..)(..)(..)(..)\n";
}
} # if file
else
{
    if ( $short eq 0 ){
    print "nicos_errortester.pl found problem/warning, logfile(s) : $file\n"
    }
}

