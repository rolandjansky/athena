#!/usr/local/bin/perl
#
# NICOS - NIghtly COntrol System
# Author Alex Undrus <undrus@bnl.gov>
# 
# ----------------------------------------------------------
# nicos_errorhandler.pl
# ----------------------------------------------------------
#
use Env;
use Cwd;
use File::Basename;
use Mail::Mailer;

sub container_extractor{
    $testdirx=$_[0];
    @testd=split(/\//,$testdirx);
    if ( "$CMTSTRUCTURINGSTYLE" eq "without_version_directory" ){
    for ($m=0; $m <= $#testd; $m++){
        $testj=join('/', @testd);
        $filfil="${NICOS_RELHOME}/${testj}/cmt/version.cmt";
        last if ( -f $filfil );
        pop @testd;
        }
    $return_val=join('_', @testd);
    return $return_val;
    }
    else{
    @testd_trun=();
    for ($m=0; $m <= $#testd; $m++){
        last if ( $testd[$m] =~ /^.+[0-9][0-9]-[0-9][0-9]-[0-9][0-9]$/ );
        push(@testd_trun, $testd[$m]);
	}
    $return_val=join('_', @testd_trun);
    return $return_val;
    }
}

sub nicos_testhandler{
# if $_[0] = 1 -> qa tests, if $_[0] != 1 int and unit tests
$par=$_[0];
$type="test";
$type="qatest" if ($par == 1);
my $NICOS_TESTLOG = "$NICOS_TESTLOG";
my $NICOS_QALOG = "$NICOS_QALOG";
my $NICOS_RELHOME = "$NICOS_RELHOME";
my $NICOS_INTTESTS_DIR = "$NICOS_INTTESTS_DIR";
my $NICOS_TEST_DBFILE_GEN = "$NICOS_TEST_DBFILE_GEN";
my $NICOS_QA_DIR = "$NICOS_QA_DIR";
my $NICOS_INTTESTS_FILES = "$NICOS_INTTESTS_FILES";
my $NICOS_COPY_HOME = "$NICOS_COPY_HOME";
my $CMTSTRUCTURINGSTYLE = "$CMTSTRUCTURINGSTYLE";

$NICOS_TESTLOGDIR=dirname(${NICOS_TESTLOG});
$NICOS_TESTLOGDIR=dirname(${NICOS_QALOG}) if ($par == 1);
$NICOS_TESTLOGDIR_COPY=${NICOS_TESTLOGDIR};
   if ( "$NICOS_COPY_HOME" ne "" ){  
   $testlog_base=basename(${NICOS_TESTLOGDIR}); 
   $NICOS_TESTLOGDIR_COPY="${NICOS_COPY_HOME}/${NICOS_PROJECT_RELNAME_COPY}/${testlog_base}";
   }
$NICOS_TESTLOGDIR_TEMP="$NICOS_TESTLOGDIR/temp";
$testprepage="nicos_testprepage";
$testprepage="nicos_qaprepage" if ($par == 1);
$dirdir="${NICOS_RELHOME}/${NICOS_INTTESTS_DIR}";
$dirdir="${NICOS_RELHOME}/${NICOS_QA_DIR}" if ($par == 1);

         opendir(DD, ${NICOS_TESTLOGDIR});
         @allfiles = grep /^.+log$/, readdir DD;
         closedir DD;
         @list = grep !/^nicos_.+$/, @allfiles;
         @listfiles = sort { -M $a <=> -M $b } @list;

         my @test_db=(); 
         if ( "$NICOS_TEST_DBFILE_GEN" ne "" ){
         if ( -f "$NICOS_TEST_DBFILE_GEN" ){
         $fname="$NICOS_TEST_DBFILE_GEN";
         open(UT, "$fname");
         chomp(@test_db=<UT>);
         close(UT);
         }
         }

         $filet = "${NICOS_WORK_AREA}/$testprepage";
         $filet = "${NICOS_WORK_AREA}/$qaprepage" if ($par == 1); 
         if ( -f $filet ){ unlink "$filet";}

         open(TF, ">$filet"); 

foreach $listfile (@listfiles){
        ($lin1 = $listfile) =~ s/ //g;
        next if (length($lin1) eq 0);
        next if $listfile =~ /^\.\.?$/;
        next if $listfile =~ /~$/;
        $testname=(split(/\./,$listfile))[0];
        @fields = split(/_/, $testname);
        @first_fields=();
        @last_fields=();
        $testdir="";
        $m_last=0;

        for ($m=0; $m <= $#fields; $m++){
	push(@first_fields, $fields[$m]);
        $testdir1=join("\/", @first_fields);
        if ( $testdir1 eq "." || $testdir1 eq "\/" || $testdir1 eq "\/." ){
	$testdir1="";
        } 
        $direc="${NICOS_RELHOME}/$testdir1";
        if ( ! -d $direc ) {$m_last=$m; last;}
        $testdir="$testdir1";
        }

        for ($m_last; $m <= $#fields; $m++){
        push(@last_fields, $fields[$m]);
        }
        $testname_base=join("_", @last_fields);

#       searching for suite
        $testsuite="undefined";   
        @addr_t=();

if ( $par != 1 ){ 
foreach (@test_db){
        next if ($_ =~ /^#/ );
        ( $lin1 = $_ ) =~ s/ //g;
        next if (length($lin1) eq 0);
        #   remove trailing leading stretches whitespaces
        $lll = join(' ', split(' ', $_));
        @fields = split(" ", $lll);
        $testname1 = $fields[0];
	$testname2=(split(/\./,$testname1))[0];
        $testdir1 = $fields[2];
        $testdir2=container_extractor($testdir1);
        $testname_compar="${testdir2}_${testname2}";

        $testsui = $fields[4];
		 if ( $testname_compar eq $testname ){
                  for ($m=6; $m <= $#fields; $m++){
                  push(@addr_t, $fields[$m]);
                  }
                  $testsuite="$testsui";
                  $testdir="$testdir2";
                  $testname_base="$testname2";
                  last;  
	     }
}
}

if ( $testname =~ G4 ) {push(@addr_t,"undrus\@bnl.gov");}
#        if ($par != 1){ 
#        print "TTTTEST $testname\n";
#        print "WRN @wrn_pat\n";
#        print "SCS @scs_pat\n";
#        print "ERR @err_pat\n";
#        print "ADR @addr_t\n";       
#}

        my @strng1=();
        if ( $par == 1 ){ 
        @strng1=`${NICOS_HOME}/nicos_errortester.pl -esq ${testname} ${release}`;
#        print " qa_tester: ${testname} ${release} : @strng1 \n";
        } else {
        @strng1=`${NICOS_HOME}/nicos_errortester.pl -est ${testname} ${release}`;
#        print " test_tester: ${testname} ${release} : @strng1 \n";
        }
        $strng=@strng1[0];

        @wrn_pat=();
        @err_pat=();
        @scs_pat=();

        @fieldstr = split(" ", $strng);
        @patterns=();
             for ($m=3; $m <= $#fieldstr; $m++){
             push(@patterns, $fieldstr[$m]);
             }
        $tproblems=0;
        if ( $fieldstr[0] eq "W" && $strng ne "" ) {
        $tproblems=1;
        print "nicos_testhandler.pl: test warnings in $testname !!!\n";
        print "                      pattern to blame: @patterns !!!\n"; 
        }
        if ( $fieldstr[0] eq "G" && $strng ne "" ) {
	$tproblems=2;
	print "nicos_testhandler.pl: test problems in $testname !!!\n";
	print "                      pattern to blame: @patterns !!!\n";

   $var="${NICOS_TESTLOGDIR_COPY}/$listfile"; 
   $varvar="${NICOS_WEBPAGE}/NICOS_TestLog_${release_copy}/${listfile}";
   @body=();
push(@body," ===========================================================\n");
push(@body," This message is generated by NICOS build for \n");
push(@body," project $project , release $release_copy . \n");
push(@body," NICOS system found possible problems with ${type} \n ${testname}\n");
#push(@body," pattern to blame: @patterns !!!\n");  
push(@body," ===========================================================\n");
push(@body," The ${type} logfile is available at: \n $var \n and(or) \n $varvar \n");
push(@body," ===========================================================\n");
push(@body," For further information please refer to $project NICOS webpage \n");
push(@body," ${NICOS_WEBPAGE}/index.html\n");

    foreach $address (@addr_t){
        if ( $address ne "" ){
           if ( $address =~ /@/  ){

if ( $address =~ somebody ){
print "nicos_errorhandler.pl: Problems with ${type} $testname !!! Mail is NOT sent to such $address code $nomail_t[0] \n";                            
}
else{
print "nicos_errorhandler.pl: Problems with ${type} $testname !!! Mail is sent to $address code $nomail_t[0] \n";
if ( $nomail_t[0] eq 0 ){
    $from_address="atnight";
    $to_address="$address";
    $subject="NICOS: ${NICOS_PROJECT_NAME}: problems in ${release_copy} with ${type} ${testname}";
    $mailer = Mail::Mailer->new("sendmail");
    $mailer->open({ From => "${from_address}",
                      To => "${to_address}",
                 Subject => "${subject}",
          })
    or die "Mailer can not open: $!\n";
    print $mailer @body;
    $mailer->close();
}
} #if ( $address =~ somebody
} #if ( $address =~ /@/  )
} #if ( $address ne "" )
} #foreach $address

        } # if ( $fieldstr[0] eq "G"
        print TF "$testname $testsuite $listfile $tproblems $testdir $testname_base\n";
} # done
close(TF);

$WTLogdir="NICOS_TestLog_${release_copy}";
$WTLogdir="NICOS_QALog_${release_copy}" if ($par == 1);
$WTLogdir_full="${NICOS_WEBDIR}/${WTLogdir}";
    if ( ! -d $WTLogdir_full ){
    system("mkdir","-p","$WTLogdir_full");
    }

opendir(DDD, ${NICOS_TESTLOGDIR});
@allcpfiles = readdir DDD;
closedir DDD;
  
@listf = grep !/^.+\.loglog.+$/, @allcpfiles;
@allcpfiles=("");
foreach $lf (@listf){
        next if $lf =~ /^\.\.?$/;
        next if $lf =~ /~$/;
        system("cp -pf ${NICOS_TESTLOGDIR}/${lf} ${WTLogdir_full}/.");
}
@listf=("");
} #end of sub

#print "------------------------------------------------------------\n";
#print "   Starting NICOS error analysis\n";
#print "------------------------------------------------------------\n";
my $NICOS_WORK_AREA="$NICOS_WORK_AREA";
my $NICOS_DBFILE = "$NICOS_DBFILE";
my $NICOS_DBFILE_GEN = "$NICOS_DBFILE_GEN";
my $NICOS_MAIL = "$NICOS_MAIL";
my $NICOS_MAIL_UNIT_TESTS = "$NICOS_MAIL_UNIT_TESTS";
my $NICOS_MAIL_INT_TESTS = "$NICOS_MAIL_INT_TESTS";
my $NICOS_MAIL_QA_TESTS = "$NICOS_MAIL_QA_TESTS";
my $NICOS_PROJECT_RELNAME = "$NICOS_PROJECT_RELNAME";
my $NICOS_PROJECT_RELNAME_COPY = "$NICOS_PROJECT_RELNAME_COPY";
my $NICOS_PROJECT_NAME = "$NICOS_PROJECT_NAME";
my $NICOS_LOGDIR = "$NICOS_LOGDIR";
my $NICOS_TESTLOG = "$NICOS_TESTLOG";
my $NICOS_PROJECTBUILD_DIR = "$NICOS_PROJECTBUILD_DIR";
my $NICOS_WEBDIR = "$NICOS_WEBDIR";
my $NICOS_WEBPAGE = "$NICOS_WEBPAGE";
my $NICOS_TITLE_QA = "$NICOS_TITLE_QA";
my $NICOS_COORDINATOR = "$NICOS_COORDINATOR";
my $NICOS_RELEASE_COORDINATOR = "$NICOS_RELEASE_COORDINATOR";
my $NICOS_TESTLOGDIR = "";
$NICOS_TESTLOGDIR = dirname(${NICOS_TESTLOG});
$prevdir=cwd;

$nomail=1;
if ( $NICOS_MAIL eq "yes" ){ $nomail=0; }
@nomail_t=(1, 1, 1);
if ( $NICOS_MAIL_QA_TESTS eq "yes" ){ $nomail_t[1]=0; }
if ( $NICOS_MAIL_UNIT_TESTS eq "yes" ){ $nomail_t[2]=0; }
if ( $NICOS_MAIL_INT_TESTS eq "yes" ){ $nomail_t[0]=0; }

$fileorig="${NICOS_DBFILE}";
$base_file=basename($NICOS_DBFILE);
$filename="${NICOS_WORK_AREA}/${base_file}_gen";
$filename_res="${NICOS_WORK_AREA}/${base_file}_gen_res";
if ( -f $filename_res ) { $filename="${filename_res}"; } 
$prepage="nicos_prepage";
$testprepage="nicos_testprepage";
$qaprepage="nicos_qaprepage";
$release="${NICOS_PROJECT_RELNAME}";
$release_copy="${NICOS_PROJECT_RELNAME_COPY}";
$project="${NICOS_PROJECT_NAME}";
$NICOS_LOGDIR=dirname($NICOS_LOG);

nicos_testhandler(1);
nicos_testhandler(2);
$ndir = "${NICOS_RELHOME}/${NICOS_PROJECTBUILD_DIR}";
chdir "$ndir";
 
$WLogdir="NICOS_Log_${release_copy}";
$WTLogdir="NICOS_TestLog_${release_copy}";
$WQLogdir="NICOS_QALog_${release_copy}";
$ndir = "${NICOS_WEBDIR}/${WLogdir}";
if ( ! -d $ndir ){ 
system("mkdir","-p","${ndir}");
}

opendir(DDD, ${NICOS_LOGDIR});
@allcpfiles = readdir DDD;
closedir DDD;

@listf = grep !/^.+\.loglog.+$/, @allcpfiles;
@allcpfiles=("");
foreach $lf (@listf){
    next if $lf =~ /^\.\.?$/;
    next if $lf =~ /~$/;
    system("cp -pf ${NICOS_LOGDIR}/${lf} ${NICOS_WEBDIR}/${WLogdir}/.");
}
@listf=("");
#remove nicos_general.loglog and nicos_build.loglog from web directory
system("rm -f ${NICOS_WEBDIR}/${WLogdir}/nicos_build.loglog");
system("rm -f ${NICOS_WEBDIR}/${WLogdir}/nicos_general.loglog");

$filet = "${NICOS_WORK_AREA}/$testprepage";
$fileq = "${NICOS_WORK_AREA}/$qaprepage";
$file="${NICOS_WORK_AREA}/$prepage";
if ( -f $file ){ unlink "$file"; }

open(DBF,"$filename");
chomp(my @dbc=<DBF>);
close(DBF);
my @fieldstr=();
my @patterns=();

open(FL,">$file");
foreach (@dbc)
{
#       Ignore comments
        next if ($_ =~ /^#/);
        ( $lin1 = $_ ) =~ s/ //g;
        next if (length($lin1) eq 0);
#       remove trailing leading stretches whitespaces
                 $lll = join(' ', split(' ', $_));
                 @fields = split(" ", $lll);
                 $package = $fields[0];
                 $tag = $fields[1];
         @addr=();
         for ($m=2; $m <= $#fields; $m++){
         push(@addr, $fields[$m]);
         }
	 ( $pkgn = $package ) =~ s/\//_/g;
         if ( $pkgn eq "" ){ $pkgn="$package"; }
         @strng1=`${NICOS_HOME}/nicos_errortester.pl -es ${pkgn} ${release} ${filename}`; 
	 $strng=@strng1[0];
	 @fieldstr = split(" ", $strng);
             @patterns=();
             for ($m=3; $m <= $#fieldstr; $m++){
             push(@patterns, $fieldstr[$m]);
             }
  
         $problems=0;
         if ( $fieldstr[0] eq "G" && $strng ne "" ) { 
         $problems=2;   
         print "nicos_errorhandler.pl: build problems with $pkgn !!!\n";
         }
          
# Determine if there is a problem with unit and qa tests
my @test_problems=(0, 0, 0);
my @type=("", "qatest", "test"); 
for($i=1; $i<=2; $i++){
####################################
# i=1 : qa tests; i=2 : unit tests #
####################################
         if ($i == 1) { open(TTT,"<$fileq");}
         else { open(TTT,"<$filet");}
	 chomp(my @ttpp=<TTT>);
	 close(TTT);
	 $comp1 = lc($pkgn);
	 $test_problems[$i]=0;

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
                 $trecent_logfiles = $fields[2];
                 $tproblems = $fields[3];
                 @xxxx=();
                    for ($m=3; $m <= $#fields; $m++){
                    push(@xxxx, $fields[$m]);
		 }
                 $comp2 = lc($testname);
		 if ( $comp1 eq $comp2 ){
		     $test_problems[$i]=$tproblems;
                     break
                     }
} #foreach (@ttpp)
         
	if ($test_problems[$i] eq 1){
        print "nicos_errorhandler.pl: $type[$i] warnings with ${pkgn} !!!\n";}
        if ($test_problems[$i] ge 2){
        print "nicos_errorhandler.pl: $type[$i] problems with ${pkgn} !!!\n";}
} #end of loop that process unit and qa tests

	 opendir(DD, ${NICOS_LOGDIR});
 	 @allfiles = readdir DD;
         closedir DD;
         $stg="${pkgn}.loglog";
	 @listx = grep /^${stg}$/, @allfiles;
         @list = grep !/^\.\.?$/, @listx;
         $listfilesf = (sort { -M $a <=> -M $b } @list)[0];
#        print " ======= $listfilesf  =======\n";

  if ( $listfilesf eq "" ){
  $lf = "${NICOS_LOGDIR}/${pkgn}.loglog";
  open(FF, ">$lf"); 
  print FF " NICOS determined that make did nothing for this package.\n";
  print FF " Error in package configuration or structure is possible.\n";
  close(FF); 
  system("cp -pf ${NICOS_LOGDIR}/${pkgn}.loglog ${NICOS_WEBDIR}/${WLogdir}/."); 
  $listfilesf="$NICOS_LOGDIR/${pkgn}.loglog";
  $problems=2;
  }

  $listfiles=basename($listfilesf);
  $recent_logfile="$listfiles";
#  print " ======= $listfiles  =======\n";
#  print " --  $pkgn -- $recent_logfile -- $problems\n";
   $pkgcontainer=dirname($package);
#  print " --  $package --- $pkgcontainer --- $pkgn -- $recent_logfile -- $problems\n";
   if ( $pkgcontainer eq "." ){ $pkgcontainer="N/A"; }
   $pkgbase=basename($package);

   if ( $fieldstr[0] eq "G" && $strng ne "" ){ 
   
   $pkg1 = "$fieldstr[1]"; 
   $drctr = "$fieldstr[2]";
   $nameer = "$fieldstr[3]";
   $drctr_copy=$drctr;
   if ( "$NICOS_COPY_HOME" ne "" ){
   $drctr_base=basename(${drctr});
   $drctr_copy="${NICOS_COPY_HOME}/${NICOS_PROJECT_RELNAME_COPY}/${drctr_base}";
}
   $var="${drctr_copy}/${pkgn}.loglog";
   $varvar="${NICOS_WEBPAGE}/${WLogdir}/${pkgn}.loglog"; 
   @body=();
push(@body," ===========================================================\n");
push(@body," This message is generated by NICOS build for \n");
push(@body," project $project , release $release_copy . \n");
push(@body," NICOS system found possible problems with package \n $package\n");
push(@body," ===========================================================\n");
push(@body," The build logfile is available at: \n $var \n and(or) \n $varvar \n");
push(@body," ===========================================================\n");
push(@body," For further information please refer to $project NICOS webpage \n");
push(@body," ${NICOS_WEBPAGE}/index.html\n");

    foreach $address (@addr){
    #$address =~ s/([@])/\\$1/g;
	if ( $address ne "" ){
           if ( $address =~ /@/  ){

print "nicos_errorhandler.pl: Problems with $package !!! Mail is sent to $address code $nomail\n";  
if ( $nomail eq 0 ){  
    $from_address="atnight";
    $to_address="$address";
    $subject="NICOS: ${NICOS_PROJECT_NAME}: problems in ${release_copy} with ${package}";
    $mailer = Mail::Mailer->new("sendmail");
    $mailer->open({ From => "${from_address}",
                      To => "${to_address}",
                 Subject => "${subject}",
          })
    or die "Mailer can not open: $!\n";
    print $mailer @body;
    $mailer->close();
}
$from_address="atnight";
$to_address="undrus\@bnl.gov";
$subject="NICOS: ${NICOS_PROJECT_NAME}: problems in ${release_copy} with ${package}";
#$mailer = Mail::Mailer->new("sendmail");
#$mailer->open({ From => "${from_address}",
#                      To => "${to_address}",
#                 Subject => "${subject}",
#	    })
#    or die "Mailer can not open: $!\n";
#print $mailer @body;
#$mailer->close();

} #if ( $address =~ /@/  )
} #if ( $address ne "" )
} #foreach $address
} #if fieldstrng

for($i=1; $i<=2; $i++){
####################################
# i=1 : qa tests; i=2 : unit tests #
####################################
if ( $test_problems[$i] ge 1 ){

$dirtest=dirname(${NICOS_TESTLOG});
$dirtest=dirname(${NICOS_QALOG}) if ($i == 1);
$dirtest_copy=$dirtest;
   if ( "$NICOS_COPY_HOME" ne "" ){
   $dirtest_base=basename(${dirtest});
   $dirtest_copy="${NICOS_COPY_HOME}/${NICOS_PROJECT_RELNAME_COPY}/${dirtest_base}";
   }
$var="${dirtest_copy}/${pkgn}.loglog";
$varvar="${NICOS_WEBPAGE}/${WTLogdir}/${pkgn}.loglog";
$varvar="${NICOS_WEBPAGE}/${WQLogdir}/${pkgn}.loglog" if ($i == 1);
@body=();
$typet=$type[$i];
if ($i == 1){
  if ( "${NICOS_TITLE_QA}" ne "" ){
  $typet="${NICOS_TITLE_QA}"; 
  } 
}
push(@body," ===========================================================\n");
push(@body," This message is generated by NICOS build for \n");
push(@body," project $project , release $release_copy . \n");
push(@body," NICOS system found possible problems with ${typet}s for package \n $package\n");
push(@body," ===========================================================\n");
push(@body," The ${typet} logfile is available at: \n $var \n and(or) \n $varvar \n");
push(@body," ===========================================================\n");
push(@body," For further information please refer to $project NICOS webpage \n");
push(@body," ${NICOS_WEBPAGE}/index.html\n");

    foreach $address (@addr){
    #$address =~ s/([@])/\\$1/g;
        if ( $address ne "" ){
           if ( $address =~ /@/  ){

print "nicos_errorhandler.pl: Problems with ${typet}s of  $package !!! Mail is sent to $address code $nomail_t[$i]\n";
if ( $nomail_t[$i] eq 0 ){
    $from_address="atnight";
    $to_address="$address";
    $subject="NICOS: ${NICOS_PROJECT_NAME}: ${typet}s problems in ${release_copy} with ${package}";
    $mailer = Mail::Mailer->new("sendmail");
    $mailer->open({ From => "${from_address}",
                      To => "${to_address}",
                 Subject => "${subject}",
          })
    or die "Mailer can not open: $!\n";
    print $mailer @body;
    $mailer->close();
}
$from_address="atnight";
$to_address="undrus\@bnl.gov";
$subject="NICOS: ${NICOS_PROJECT_NAME}: ${typet}s problems in ${release_copy} with ${package}";
#$mailer = Mail::Mailer->new("sendmail");
#$mailer->open({ From => "${from_address}",
#                      To => "${to_address}",
#                 Subject => "${subject}",
#           })
#    or die "Mailer can not open: $!\n";
#print $mailer @body;
#$mailer->close();

} #if ( $address =~ /@/  )
} #if ( $address ne "" )
} #foreach $address
} #if test_problems
} # end of loop over test types

tr/A-Z/a-z/ for @addr;
$lin1 = join(" ",@addr);
($line = $lin1 ) =~ s/ //g; 

if ( "$line" != "" && "$line" != "-" ){
print FL "$pkgbase $pkgcontainer $recent_logfile $problems @addr\n";
#print " UUUUUUU $package $pkgcontainer $recent_logfile @addr\n";
}
else{
print FL "$pkgbase $pkgcontainer $recent_logfile $problems @addr\n";
#print " UUUUUUE $package $pkgcontainer $recent_logfile @addr\n";
}
} # foreasch (@dbc
close (FL);

  open(FL,"<$file");
  $scount=0; 
  while (<FL>) {
  chomp;
  ( $lin1 = $_ ) =~ s/ //g;
  next if (length($lin1) eq 0);
#   remove trailing leading stretches whitespaces
  $lll = join(' ', split(' ', $_));
  @fields = split(" ", $lll);
  if ( $fields[3] ne 0 ){ $scount++; }
print "nicos_errorhandler.pl: problem code: $fields[1]/$fields[0] --- $fields[3]\n";
  if ( "$scount" eq 10 ){
  @bod=();
  push(@bod," !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
  push(@bod," NICOS message to $project coordinator: \n");
  push(@bod," !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
  push(@bod," Number of packages with errors in release $release_copy is \n");
  push(@bod," greater or equal 10 in the Linux debug $release_copy build \n");
   if ( $nomail eq 0 ){   
      $from_address="atnight";
      $to_address="${NICOS_COORDINATOR}";
      $subject="NICOS: ${NICOS_PROJECT_NAME}: large number of errors in $release_copy";
      $mailer = Mail::Mailer->new("sendmail");
      $mailer->open({ From => "${from_address}",
                      To => "${to_address}",
                 Subject => "${subject}",
		  })
	  or die "Mailer can not open: $!\n";
      print $mailer @bod;
      $mailer->close();
      $from_address="atnight";
      $to_address="${NICOS_RELEASE_COORDINATOR}";
      $subject="NICOS: ${NICOS_PROJECT_NAME}: large number of errors in $release_copy";
      $mailer = Mail::Mailer->new("sendmail");
      $mailer->open({ From => "${from_address}",
                      To => "${to_address}",
                 Subject => "${subject}",
                  })
          or die "Mailer can not open: $!\n";
      print $mailer @bod;
      $mailer->close();
  }
  last;
  }
  } # while(<FL>)
  close(FL);
chdir "${prevdir}";






