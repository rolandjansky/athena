#!/usr/local/bin/perl
#
# NICOS - NIghtly COntrol System
# Author Alex Undrus <undrus@bnl.gov>
# 
# ----------------------------------------------------------
# nicos_pagemaker.pl
# ----------------------------------------------------------
#

sub compartim {
    my $name = shift;
    my $tm = (stat("${NICOS_WEBDIR}/$name"))[9];
    return $tm;
}

sub comparc{
    my $strg=shift;
    ( $lin1 = $strg ) =~ s/ //g;
    if (length($lin1) eq 0){
        return 0;}
    else{
        $res1 = (split(" ", $strg))[1];
        $res2 = (split(" ", $strg))[0];
        $res = "$res1" . "$res2";
        if ($res eq "" || $res eq "N/A"){ $res="AAAAAA";}  
        return $res;
    }
}
sub compar0{
    my $strg=shift;
    ( $lin1 = $strg ) =~ s/ //g;
    if (length($lin1) eq 0){
        return 0;}
    else{
        $res1 = (split(" ", $strg))[0];
        $probl = (split(" ", $strg))[3];
        $qprobl = (split(" ", $strg))[5];
        $tprobl = (split(" ", $strg))[7];
        $res=""; 
        if ($probl ge 2 || $tprobl ge 2 || $qprobl ge 2)
        {$res = "AAAA" . "$res1";}
        else
        {$res = "BBBB" . "$res1";}  
        return $res;
    }
}
sub compart{
    my $strg=shift;
    ( $lin1 = $strg ) =~ s/ //g;
    if (length($lin1) eq 0){
        return 0;}
    else{
        $res1 = (split(" ", $strg))[0];
        $tprobl = (split(" ", $strg))[3];
        $res="";
        if ($tprobl ge 2)
        {$res = "AAAA" . "$res1";}
        elsif ($tprobl eq 1) 
        {$res = "BBBB" . "$res1";}
        else
        {$res = "CCCC" . "$res1";}
        return $res;
    }
}

use Env;
use Cwd;
use File::Basename;
#use ExtUtils::Command;                                                                               
print "------------------------------------------------------------\n";
print "   Starting creating NICOS webpages (final step)\n";
print "------------------------------------------------------------\n";
my $NICOS_WORK_AREA="$NICOS_WORK_AREA";
my $NICOS_RELHOME="$NICOS_RELHOME";
my $NICOS_WEBDIR="$NICOS_WEBDIR";
my $NICOS_HOME="$NICOS_HOME";
my ${NICOS_PROJECT_RELNAME}="${NICOS_PROJECT_RELNAME_COPY}";
my ${NICOS_PROJECT_RELNUMB_ORIG}="${NICOS_PROJECT_RELNUMB}";
my ${NICOS_PROJECT_RELNUMB}="${NICOS_PROJECT_RELNUMB_COPY}";
my ${NICOS_WEBDIR}="${NICOS_WEBDIR}";
my ${NICOS_INC_BUILD}="${NICOS_INC_BUILD}";
my $NICOS_TITLE_COMMENT="$NICOS_TITLE_COMMENT";
my $prepage="nicos_prepage";
my $testprepage="nicos_testprepage";
my $qaprepage="nicos_qaprepage";
my $release="${NICOS_PROJECT_RELNAME}";
#my $project="${NICOS_PROJECT_NAME}";
my $project="Nightly";
my $WLogdir="NICOS_Log_${release}";
my $WTLogdir="NICOS_TestLog_${release}";
my $WQLogdir="NICOS_QALog_${release}";
my $WWW_DIR="${NICOS_WEBDIR}";
my $WWW_WORK="${NICOS_WORK_AREA}";
my $prevdir=cwd;
my $LOGHANDLER_REPORT="${NICOS_WORK_AREA}/nicos_loghandler_report";
my $suffix="_${NICOS_PROJECT_RELNUMB}";
if ( ${NICOS_PROJECT_RELNUMB} eq "" ) { $suffix=""; }
my $outfile_test_pass="${WWW_DIR}/status_test_pass${suffix}.js";
my $outfile_test_fail="${WWW_DIR}/status_test_fail${suffix}.js";
my $outfile_unit_pass="${WWW_DIR}/status_unit_pass${suffix}.js";
my $outfile_unit_fail="${WWW_DIR}/status_unit_fail${suffix}.js";
chdir "$WWW_WORK";
#-------------

@tprepx=();
@uprepx=();
@tprepy=();
@prepx=();
@logrep=();

$ttp="${WWW_WORK}/${testprepage}";
open(TTT,"$ttp");
chomp(my @ttpp=<TTT>);
close(TTT);

$ttq="${WWW_WORK}/${qaprepage}";
open(TTT,"$ttq");
chomp(my @ttqq=<TTT>);
close(TTT);

$file = "$LOGHANDLER_REPORT";
open(REP,"<$file")
    or die "nicos_pagemaker.pl: couldn't open $file for reading: $!\n";
while (<REP>) {
    chomp;
    next if ($_ =~ /^#/ );
             ( $lin1 = $_ ) =~ s/ //g;
next if (length($lin1) eq 0);
$lll = join(' ', split(' ', $_));
@fields = split(" ", $lll);
$entry = $fields[0];
$container=dirname($entry);
if($container eq "."){$container="N/A";} 
$package=basename($entry);
$l1="$package $container\n";
push(@logrep, $l1);
} # while (<REP>)
close(REP);

%seen=();
@uniq = grep { ! $seen{$_} ++ } @logrep;
@logrep=@uniq;
@uniq=();

$file = "${WWW_WORK}/${prepage}";
open(IN,"<$file")
    or die "nicos_pagemaker.pl: couldn't open $file for reading: $!\n";
while (<IN>) {
    chomp;
    next if ($_ =~ /^#/ );
	     ( $lin1 = $_ ) =~ s/ //g;
	     next if (length($lin1) eq 0);
#   remove trailing leading stretches whitespaces
	     $lll = join(' ', split(' ', $_));
	     @fields = split(" ", $lll);
	     $package = $fields[0];
	     $container = $fields[1];
             $recent_logfiles = $fields[2];
             $problems = $fields[3];
             @addresses=();
	     for ($m=4; $m <= $#fields; $m++){
		 push(@addresses, $fields[$m]);
	     }
	     $pkgn1="${container}_${package}";
             if ( $container eq "N/A" ) { $pkgn1 = ${package}; }
             $pkgn11="${pkgn1}_${package}"; 
             ( $pkgn = $pkgn11 ) =~ s/\//_/g;
             $comp11 = lc($pkgn);
             ( $pkgn = $pkgn1 ) =~ s/\//_/g;
             $comp1 = lc($pkgn);
my @xxxx=();
my $treclog="0";
my $tprob=0;

foreach (@ttpp)
{
#       Ignore comments
        next if ($_ =~ /^#/);
                 ( $lin1 = $_ ) =~ s/ //g;
                 next if (length($lin1) eq 0);
#       remove trailing leading stretches whitespaces
                 $lll = join(' ', split(' ', $_));
                 @fields = split(" ", $lll);
                 $testname = $fields[0];
                 $testsuite = $fields[1];
                 $trecent_logfiles = $fields[2];
                 $tproblems = $fields[3];
                 $testdir = $fields[4];
                 $testname_base = $fields[5];
                 @xxxx=();
                    for ($m=6; $m <= $#fields; $m++){
                    push(@xxxx, $fields[$m]);
                    }
                 $comp2 = lc($testname);
  if ( $comp11 eq $comp2 ){
  $llll = "$testname $testsuite $trecent_logfiles $tproblems $testdir $testname_base @xxxx\n"; 
  push(@tprepx, $llll);
  $treclog = $trecent_logfiles;
  $tprob = $tproblems;
  last;
  }
  } #done

$qreclog="0";
$qprob=0;
foreach (@ttqq)
{
#       Ignore comments
        next if ($_ =~ /^#/);
                 ( $lin1 = $_ ) =~ s/ //g;
                 next if (length($lin1) eq 0);
#       remove trailing leading stretches whitespaces
                 $lll = join(' ', split(' ', $_));
                 @fields = split(" ", $lll);
                 $testname = $fields[0];
                 $testsuite = $fields[1];
                 $trecent_logfiles = $fields[2];
                 $tproblems = $fields[3];
                 $testdir = $fields[4];
                 $testname_base = $fields[5];
                 @xxxx=();
		 for ($m=6; $m <= $#fields; $m++){
		     push(@xxxx, $fields[$m]);
		 }
                 $comp2 = lc($testname);
		 if ( $comp1 eq $comp2 ){
                     $qreclog = $trecent_logfiles;
                     $qprob = $tproblems;
		     last;
          }
} #done

$llll = "$package $container $recent_logfiles $problems $qreclog $qprob $treclog $tprob @addresses\n";
push(@prepx, $llll);

} #done

#######

foreach (@ttpp)
{
#       Ignore comments
        next if ($_ =~ /^#/);
                 ( $lin1 = $_ ) =~ s/ //g;
                 next if (length($lin1) eq 0);
                 $nicos_pagemaker_data = 0;
#       remove trailing leading stretches whitespaces
                 $lll = join(' ', split(' ', $_));
                 @fields = split(" ", $lll);
                 $testname = $fields[0];
                 $testsuite = $fields[1];
                 $trecent_logfiles = $fields[2];
                 $tproblems = $fields[3];
                 $testdir = $fields[4];
                 $testname_base = $fields[5];
                 @xxxx=();
                    for ($m=6; $m <= $#fields; $m++){
                    push(@xxxx, $fields[$m]);
                    }

		 foreach $item (@tprepx) {
                     next if ($item =~ /^#/ );
                     ( $lin1 = $item ) =~ s/ //g;
		     next if (length($lin1) eq 0);
                     @fields = split(" ", $item);
                     $ttestname = $fields[0];
#		     print "XXX $testname $ttestname\n";
                     if ($testname eq "$ttestname"){
                     $nicos_pagemaker_data = 1;
                     last;
                     } 
		     }

if ( $nicos_pagemaker_data eq 0 ){
$llll = "$testname $testsuite $trecent_logfiles $tproblems $testdir $testname_base @xxxx\n";
push(@tprepy,$llll);
}

} #done
close(IN);

#creating web page with test results
$inttestpage="${WWW_WORK}/nicos_testsummary_${NICOS_PROJECT_RELNUMB}.html";
if ( "${NICOS_PROJECT_RELNUMB}" eq "" )
{
    $inttestpage="${WWW_WORK}/nicos_testsummary.html";
}
open(WP,">$inttestpage");
$prg = "${NICOS_HOME}/nicos_wwwgen.pl -h 22 ${project} ${release} ${NICOS_RELHOME} ${WLogdir}";
$pid = open(RM, "${prg} |");
while (<RM>){ print WP "$_"; }
close (RM);
#@testprepage_01 = sort @tprepy;
@testprepage_01 =  sort { compart($a) cmp compart($b) } @tprepy;
@testprepage_u_01 =  sort { compart($a) cmp compart($b) } @tprepx;
my $test_pass=0;
my $test_fail=0;
my $unit_pass=0;
my $unit_fail=0;

foreach (@testprepage_01)
{
#       Ignore comments
        next if ($_ =~ /^#/);
                 ( $lin1 = $_ ) =~ s/ //g;
                 next if (length($lin1) eq 0);
                 @fields = split(" ", $_);
                 $testname = $fields[0];
                 $testsuite = $fields[1];
                 $recent_logfiles = $fields[2];
                 $problems = $fields[3];
                 $testdir = $fields[4];
                 $testname_base = $fields[5];
                 @xxxx=();
		 for ($m=6; $m <= $#fields; $m++){
		     push(@xxxx, $fields[$m]);
		 }
		 if ( $testname ne "" ){
		     if ( $recent_logfiles eq "" ){ $recent_logfiles="N/A"; }
		     if ( $problems le 1 ){$test_pass++;}
                     else {$test_fail++;} 
    $prg = "${NICOS_HOME}/nicos_wwwgen.pl -a ${testdir} ${testname_base} ${testsuite} ${WTLogdir} ${recent_logfiles} ${problems}";
    $pid = open(RM, "${prg} |");
    while (<RM>){ print WP "$_";}
    close (RM);
}
} #done

	$prg = "${NICOS_HOME}/nicos_wwwgen.pl -u 0";
	$pid = open(RM, "${prg} |");
	while (<RM>){ print WP "$_";}
	close (RM);

foreach (@testprepage_u_01)
{
#       Ignore comments
        next if ($_ =~ /^#/);
                 ( $lin1 = $_ ) =~ s/ //g;
                 next if (length($lin1) eq 0);
                 @fields = split(" ", $_);
                 $testname = $fields[0];
                 $testsuite = $fields[1];
                 $recent_logfiles = $fields[2];
                 $problems = $fields[3];
                 $testdir = $fields[4];
                 $testname_base = $fields[5];
                 if ( $testname ne "" ){
                     if ( $recent_logfiles eq "" ){ $recent_logfiles="N/A"; }
                     if ( $problems le 1 ){$unit_pass++;}
                     else {$unit_fail++;}	
    $prg = "${NICOS_HOME}/nicos_wwwgen.pl -a ${testdir} ${testname_base} ${testsuite} ${WTLogdir} ${recent_logfiles} ${problems}";
    $pid = open(RM, "${prg} |");
    while (<RM>){ print WP "$_";}
    close (RM);
}
} #done

open(WRITEDATA,">$outfile_test_pass");
print WRITEDATA "function status_tp${suffix}(){return $test_pass}";
close(WRITEDATA);
open(WRITEDATA,">$outfile_test_fail");
print WRITEDATA "function status_tf${suffix}(){return $test_fail}";
close(WRITEDATA);
open(WRITEDATA,">$outfile_unit_pass");
print WRITEDATA "function status_up${suffix}(){return $unit_pass}";
close(WRITEDATA);
open(WRITEDATA,">$outfile_unit_fail");
print WRITEDATA "function status_uf${suffix}(){return $unit_fail}";
close(WRITEDATA);

$prg = "${NICOS_HOME}/nicos_wwwgen.pl -f";
$pid = open(RM, "${prg} |");
while (<RM>){ print WP "$_"; }
close (RM);

close(WP);

#loop produces web pages with different sorting  
my @sortind=("","1","2");
#		 my @sortind=("1");
#11------------

foreach $sortsuffix (@sortind){

$webpage="${WWW_WORK}/nicos_buildsummary${sortsuffix}_${NICOS_PROJECT_RELNUMB}.html";
if ( "${NICOS_PROJECT_RELNUMB}" eq "" )
{
$webpage="${WWW_WORK}/nicos_buildsummary${sortsuffix}.html";
}
open(WP,">$webpage");
$prg = "${NICOS_HOME}/nicos_wwwgen.pl -h 11 ${project} ${release} ${NICOS_RELHOME} ${WLogdir}";
   $pid = open(RM, "${prg} |");
   while (<RM>){ print WP "$_"; }
close (RM);

#114
if ($sortsuffix eq "1"){
$prg = "${NICOS_HOME}/nicos_wwwgen.pl -i 1 ${release} ${WLogdir}";
}
elsif ($sortsuffix eq "2"){
$prg = "${NICOS_HOME}/nicos_wwwgen.pl -i 2 ${release} ${WLogdir}";
}
else{
$prg = "${NICOS_HOME}/nicos_wwwgen.pl -i 0 ${release} ${WLogdir}";
}
$pid = open(RM, "${prg} |");
while (<RM>){ print WP "$_"; }
close (RM);

#115
        @prep_01 = ();
	if ($sortsuffix eq ""){
            @prep_01 =  sort { compar0($a) cmp compar0($b) } @prepx;
	}
	elsif ($sortsuffix eq "2"){
	    @prep_01 =  sort { comparc($a) cmp comparc($b) } @prepx;
	}
        else{
	    @prep_01 = ();
               foreach $item_y (@logrep)
                {
                next if ($item_y =~ /^#/ );
	        ( $lin1 = $item_y ) =~ s/ //g;
		@fields = split(" ", $item_y);
		$package_y = $fields[0];
		$container_y = $fields[1];
                foreach $item_x (@prepx)
		     {
                     next if ($item_x =~ /^#/ );
		     ( $lin1 = $item_x ) =~ s/ //g;
		     @fields = split(" ", $item_x);
		     $package_x = $fields[0];
		     $container_x = $fields[1];
                     if( $package_x eq $package_y && $container_x eq $container_y )
	             {push(@prep_01,$item_x);}
		     }
               }

            my $rslt=0;
            foreach $item_x (@prepx)
            {
            next if ($item_x =~ /^#/ );
	    ( $lin1 = $item_x ) =~ s/ //g;
	    @fields = split(" ", $item_x);
            $package_x = $fields[0];
	    $container_x = $fields[1];
	    $rslt=0;
   	       foreach $item_y (@logrep)
               {
               next if ($item_y =~ /^#/ );
               ( $lin1 = $item_y ) =~ s/ //g;
               @fields = split(" ", $item_y);
               $package_y = $fields[0];
               $container_y = $fields[1];                  
               if( $package_x eq $package_y && $container_x eq $container_y )
			{
                	$rslt=1;
                        last;
                        }
               }              
            if($rslt eq 0)
            {push(@prep_01,$item_x);}
            } 
	}
     
foreach (@prep_01)
{    next if ($_ =~ /^#/ );
	      ( $lin1 = $_ ) =~ s/ //g;
	      next if (length($lin1) eq 0);
	      @fields = split(" ", $_);
	      $package = $fields[0];
	      $container = $fields[1];
	      $recent_logfiles = $fields[2];
	      $problems = $fields[3];
              $qrecent_logfiles = $fields[4];
              $qproblems = $fields[5];
              $trecent_logfiles = $fields[6];
              $tproblems = $fields[7]; 
	      @addresses=();
	      for ($m=8; $m <= $#fields; $m++){
		  push(@addresses, $fields[$m]);
	      }
	      $pkgn1="${container}_${package}";
              if ( $container eq "N/A" ) { $pkgn1 = ${package}; }
              ( $pkgn = $pkgn1 ) =~ s/\//_/g;
 
if ( $package ne "" ){
if ( $recent_logfiles eq "" ){ $recent_logfiles="N/A"; }
if ( $recent_logfiles eq "0" ){ $recent_logfiles="N/A"; }
if ( $trecent_logfiles eq "" ){ $trecent_logfiles="N/A"; }
if ( $trecent_logfiles eq "0" ){ $trecent_logfiles="N/A"; }

if ( "$#addresses" < 0 ){ @addresses=("nobody"); }
for (@addresses) { s/'/\\'/g }

  $prg = "${NICOS_HOME}/nicos_wwwgen.pl -g ${package} ${container} ${WLogdir} ${recent_logfiles} ${problems}  ${WQLogdir} ${qrecent_logfiles} ${qproblems} ${WTLogdir} ${trecent_logfiles} ${tproblems} @addresses";
  $pid = open(RM, "${prg} |");
  while (<RM>){ print WP "$_"; }
  close (RM);

}
} #done 
 
     $prg = "${NICOS_HOME}/nicos_wwwgen.pl -f";
     $pid = open(RM, "${prg} |");
     while (<RM>){ print WP "$_"; }
     close (RM);

     close (WP);

#making copies
if ( "${NICOS_PROJECT_RELNUMB}" eq "" )
{
system("rm -f ${WWW_DIR}/nicos_buildsummary${sortsuffix}.html");
}
else
{
system("rm -f ${WWW_DIR}/nicos_buildsummary${sortsuffix}_${NICOS_PROJECT_RELNUMB}.html"); 
}

system("cp -p ${webpage} ${WWW_DIR}/.");
} #foreach $sortsuffix (@sortind)

@prepx=();
@tprepy=();

system("cp -p ${webpage} ${WWW_DIR}/.");
system("cp -p ${inttestpage} ${WWW_DIR}/.");
system("cp -p ${NICOS_HOME}/doc/images/tick.gif ${WWW_DIR}/.");
system("cp -p ${NICOS_HOME}/doc/images/cross_red.gif ${WWW_DIR}/.");
system("cp -p ${NICOS_HOME}/doc/images/yl_ball.gif ${WWW_DIR}/.");
system("cp -p ${NICOS_HOME}/doc/images/Back3.jpg ${WWW_DIR}/.");

#copy previous tag list
$pfilegen1="${NICOS_DBFILE_GEN}_res";
$pfilegen=basename($pfilegen1);
opendir(WWWS, ${NICOS_WEBDIR});
my @wwwli = sort { compartim($a) <=> compartim($b) }
grep { /^${pfilegen}/ }
readdir (WWWS);
closedir(WWWS);

if ( $NICOS_INC_BUILD eq "yes" )
{
    if ( ${NICOS_PROJECT_RELNUMB_ORIG} ne "" ){
        $file_to_copy="${pfilegen}_${NICOS_PROJECT_RELNUMB_ORIG}_prev";}
    else{
	$file_to_copy="${pfilegen}_res_prev";}
    system("cp -p ${NICOS_WEBDIR}/${file_to_copy} ${WWW_WORK}/${pfilegen}_prev");
}
else {   # if ( $NICOS_INC_BUILD eq "yes" 
if ( $#wwwli > -1 ){
my @wwwlis = reverse @wwwli;
$file_to_copy=$wwwlis[0];
$file_to_copy=$wwwlis[1] if ( $#wwwlis > 0 ); 
system("cp -p ${NICOS_WEBDIR}/${file_to_copy} ${WWW_WORK}/${pfilegen}_prev");
}
} # if ( $NICOS_INC_BUILD eq "yes" )

#making index and other pages
 
$prg = "${NICOS_HOME}/nicos_indexgen.pl ${WWW_WORK}/nicos_projectpage.html";
$pid = open(RM, "${prg} |");
while (<RM>){ print STDOUT "$_"; }
close (RM);

$prg = "${NICOS_HOME}/nicos_configgen.pl ${WWW_WORK}/nicos_configuration.html";
$pid = open(RM, "${prg} |");
while (<RM>){ print STDOUT "$_"; }
close (RM);

$prg = "${NICOS_HOME}/nicos_contentgen.pl ${WWW_WORK}/nicos_content_${NICOS_PROJECT_RELNUMB}.html";
if ( "${NICOS_PROJECT_RELNUMB}" eq "" )
{
    $prg = "${NICOS_HOME}/nicos_contentgen.pl ${WWW_WORK}/nicos_content.html";
}
$pid = open(RM, "${prg} |");
while (<RM>){ print STDOUT "$_"; }
close (RM);

system("cp -p ${WWW_WORK}/nicos_projectpage.html ${WWW_DIR}/.");
system("cp -p ${WWW_WORK}/nicos_configuration.html ${WWW_DIR}/.");

if ( -f "${WWW_WORK}/nicos_content.html" )
{
if ( "${NICOS_PROJECT_RELNUMB}" eq "" )
{
system("cp -p ${WWW_WORK}/nicos_content.html ${WWW_DIR}/.");
}
}

if ( -f "${WWW_WORK}/nicos_content_${NICOS_PROJECT_RELNUMB}.html" )
{
if ( "${NICOS_PROJECT_RELNUMB}" ne "" )
{
system("cp -p ${WWW_WORK}/nicos_content_${NICOS_PROJECT_RELNUMB}.html ${WWW_DIR}/.");
}
}

chdir "${WWW_DIR}";
unlink "index.html";
system("ln -s nicos_projectpage.html index.html");
unlink "nightly_builds.html";
system("ln -s nicos_projectpage.html nightly_builds.html");
chdir "${prevdir}";











