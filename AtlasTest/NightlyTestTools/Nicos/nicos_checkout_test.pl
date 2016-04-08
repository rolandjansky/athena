#!/usr/local/bin/perl
#
# NICOS - NIghtly COntrol System
# Author Alex Undrus <undrus@bnl.gov>
# 
# ----------------------------------------------------------
# nicos_checkout
# ----------------------------------------------------------
#
use Env;
use Cwd;
use File::Basename;

sub newrel_link{
     # cvs checkouts
    my $newver = $_[0];
    my $package1 = $_[1];
    $prevdirr=cwd;
    chdir "${NICOS_PKGDIR}";
    $dcwd = cwd;

    if ( -d ${package1} && ${NICOS_CHECKOUT_COMMAND} eq "cmt" ){
    system("rm","-rf","${package1}");
    } 

     if ( -d ${package1} ){
     chdir "${package1}";
     $infoword="updated"; 
        #
        #   update:
        #
        if ( $newver eq "recent" ){
	    $stat=system("cvs","-d","${CVSROOT}","update","-d");}
        else{
            $stat=system("cvs","-d","${CVSROOT}","update","-dr","${newver}");}
     }
     else{
     $infoword="checked out"; 
     $dpack=dirname(${package1});

if ( ${NICOS_CHECKOUT_COMMAND} eq "cmt" ){

          if ( ${package1} eq "ReleaseTest" || ${package1} eq "TestRelease" ){
          system("mkdir","-p","${package1}");
          chdir "${package1}";
          $npack=basename(${package1});
          #
          if ( "$newver" eq "recent" ){
          $newver="${package1}-00-00-01";
          $stat=system("cvs","-d","${CVSROOT}","co","-d","${newver}","${cvsloc}/${package1}");
          }
          else{
          $stat=system("cvs","-d","${CVSROOT}","co","-r","${newver}","-d","${newver}","${cvsloc}/${package1}");
          }
          } # if ( ${package1} eq "ReleaseTest" 
          else{
             if ( "$newver" eq "recent" ){
             $stat=system("cmt","co","${package1}");
             }
             else
             {
             $stat=system("cmt","co","-r","${newver}","${package1}");
             }
          } # else ... if ( ${package1} eq "ReleaseTest"
} 
else{
     if ( ! -d ${dpack} ){ system("mkdir","-p","${dpack}");}
     chdir "${dpack}";
     $npack=basename(${package1}); 
          #
          #   Checkout from scratch:
          #
          if ( "$newver" eq "recent" ){
          $stat=system("cvs","-d","${CVSROOT}","co","-d","${npack}","${cvsloc}/${package1}");
          }
          else
          {
          $stat=system("cvs","-d","${CVSROOT}","co","-r","${newver}","-d","${npack}","${cvsloc}/${package1}");
          }
} # else ${NICOS_CHECKOUT_COMMAND} eq "cmt
  
     }  # if [ -d ${package1} ]

#   print "$stat\n";
    if ( $stat ne 0 ){
    print "nicos_checkout.pl: CVS ERROR when $package1 of version $newver $infoword\n";
    $cvserr++;
    }
    else
    {
    print "nicos_checkout.pl: version $newver for $package1 is successfully $infoword\n";
    }
    chdir $prevdirr or die "Can not cd to $prevdirr: $!\n";
}

################################################

my $NICOS_PKGDIR = "$NICOS_PKGDIR";
my $NICOS_WORK_AREA="$NICOS_WORK_AREA";
my $NICOS_RELHOME="$NICOS_RELHOME";
my $NICOS_CVSDIR="$NICOS_CVSDIR";
my $NICOS_PKGDIR="$NICOS_PKGDIR";
my $NICOS_TAGSTORAGE_METHOD="$NICOS_TAGSTORAGE_METHOD";
my $NICOS_DBFILE="$NICOS_DBFILE";
my $NICOS_DBFILE_GEN="$NICOS_DBFILE_GEN";
my $NICOS_HOME="$NICOS_HOME";
my $CVSROOT="$CVSROOT";
my $NICOS_CHECKOUT_COMMAND="$NICOS_CHECKOUT_COMMAND";

my $cvserr=0;
my $prevdir=cwd;
my $newver="";

my $policy_to_use_tag_even_not_found="yes";

if ( ! -d ${NICOS_RELHOME} ){
print " nicos_checkout: problem: directory for the project is not created: \n";
print " ${NICOS_RELHOME} \n";
exit 1
}

#system("cd","${NICOS_RELHOME}");
#chdir "${NICOS_RELHOME}" or die "Can not cd to ${NICOS_RELHOME}: $!\n";

if ( ! -d ${NICOS_PKGDIR} ){
system("mkdir","-p","${NICOS_PKGDIR}");
}

$cvsavail=1;
if ( ${CVSROOT} =~ /kserver/ || ${CVSROOT} =~ /pserver/ || ${CVSROOT} =~ /ext/ ){
$cvsavail=0; }
$cvsloc="${NICOS_CVSDIR}";
$pfile="${NICOS_DBFILE_GEN}";
$base_pfile=basename($NICOS_DBFILE_GEN);
$pfilegen="${NICOS_DBFILE_GEN}"."_res";
$base_pfilegen=basename($pfilegen);
$relname="${NICOS_PROJECT_RELNAME}";
    my @arr = split("_",$relname);
    my $lastel = $#arr;
    my $relnum=$arr[$lastel];
    my $relnum_suffix = "_$relnum";
    if ( $relnum eq "" ) { $relnum_suffix = ""; }
    if ( $#arr eq 0 ) { $relnum_suffix = ""; }
$pfilegen_copy="${NICOS_WEBDIR}/${base_pfilegen}${relnum_suffix}";

if ( ! -f $pfile ){
print " nicos_checkout: problem: package tags can not be retrived from file:\n";
print " $pfile : it does not exist\n";
exit 1
}

print "------------------------------------------------------------ \n";
print " nicos_checkout: info: package tags are retrived from file:\n";
print " $pfile \n";
print "------------------------------------------------------------ \n";

if ( -f $pfilegen ){
unlink ($pfilegen) or die "nicos_checkout: can not delete ${pfilegen}: $!\n";
} 
open(OUT,">$pfilegen");

open(IN,"<$pfile")
    or die "Couldn't open $pfile for reading: $!\n";
while (<IN>) {
    chomp;
    next if ($_ =~ /^#/ );
    ( $lin1 = $_ ) =~ s/ //g;
    next if (length($lin1) eq 0);
#   remove trailing leading stretches whitespaces
    $lll = join(' ', split(' ', $_));
    @fields = split(" ", $lll);
    $package1 = $fields[0];
    $newver = $fields[1];
    $code = $fields[2];
    @addr=();
    for ($m=3; $m <= $#fields; $m++){
	 push(@addr, $fields[$m]);
    }
	 $packagebase=basename($package1);
	 ($package2 = $package1 ) =~ s/\//_/g;
    @elem=split("/",$package1);
    $package=@elem[0];
   print " -++- $package1 -- $package2 -- $code -- @addr --v-- $newver\n";

#   checking CVSROOT modules for non-stand locations
    $file = "${CVSROOT}/CVSROOT/modules";
    if ( -f $file ){
    open(MD,"<${CVSROOT}/CVSROOT/modules");
    $cvslalt="";
    $cvsloc="";
while (<MD>){
    ( $lin1 = $_ ) =~ s/ //g;
    next if (length($lin1) eq 0);
    $llw = join(' ', split(' ', $_));
    @flds = split(" ", $llw);
    $mname=@flds[0];
    if ( $mname eq $packagebase ) {$cvslalt = splice(@flds, -1);}
} # while (<MD>)
    close (MD);

    if ( "$cvslalt" ne "" ){ $cvsloc=dirname($cvslalt);}
    if ( "$cvsloc" eq "" ){ $cvsloc=${NICOS_CVSDIR};}

#    print "----------------- $cvsloc\n";
    } # if ( -f ${CVSROOT}/CVSROOT/modules

    if ( "$newver" eq "delete" ) {
        print "nicos_checkout.pl: Deleting package directory from release\n";
        unlink(${package}) or die "Can not delete ${package}: $!\n";
    }
    else{ # if [ "$newver" = "delete" ]

        $diroffset=".";
        $emergtag="recent";

        if ( $cvsavail eq 1 && $newver ne "recent" ){
        @arrr=("${CVSROOT}/${cvsloc}/${package1}/${packagebase}",
               "${CVSROOT}/${cvsloc}/${package1}/src",
               "${CVSROOT}/${cvsloc}/${package1}/cmt");
        foreach $item (@arrr){
        @files = glob("*,v");
        if($#files > 0){
        $diroffset="$item";
        last;
        }
        }

        $file="${cvsloc}/${package1}/${diroffset}";
#         $file="seal/Scripting/PyLCGDict";
        @emerg=`$NICOS_HOME/listtag.pl -d ${CVSROOT} ${file} 2>&1`;
#	print "VBVBVBVB ${cvsloc}/${package1}/$diroffset ${CVSROOT} @emerg\n";
        $emergtag="";
        foreach (@emerg){
        next if (  m/tags/ ||  m/defined/ ||  ! m/:/);
        s/symbols//g;
        s/symbol//g;
        $emergtag=$_;
        last;
        }
             if ( "$emergtag" eq "" ){
             $emergtag="recent";
             }
             else
             {
             @arrr=split(":",$emergtag);
             $emergtag=basename(@arrr[0]);
             $emergtag =~ s/ //g;
             $emergtag =~ s/[^a-zA-Z0-9_\-\$\.\*]//g;
             }

        } # if [ "$cvsavail" 

        if ( $newver eq "official" && $cvsavail eq 1 ) {

        @recent7=`$NICOS_HOME/listtag.pl -d ${CVSROOT} ${cvsloc}/${package1}/$diroffset 2>&1`;
        $recent="";
        foreach (@recent7){
        next if ( /tags/ ||  /defined/ || ! /:/ || ! /${packagebase}[-][0-9]{1,2}[-][0-9]{1,2}[-][0-9]{1,2}/ );
        s/symbols//g;
        s/symbol//g;
        @arrr=split(":",$_);
        $recent=basename(@arrr[0]);
	$recent =~ s/ //g;
	$recent =~ s/[^a-zA-Z0-9_\-\$\.\*]//g;
        last;
        }

#       print " OOOOOOOOOO  $recent7 --- $package --- $package1\n";
#       print " OOOOOOOOOA  $recent\n";

             if ( "$recent" eq "" ){
             $recent=${emergtag};
             }
             $newver=$recent;

        } #if [ "$newver" = "official" ]; then

        if ( $newver ne "official" && $newver ne "current" && $newver ne "development" && $newver ne "recent" ){

        if ( $cvsavail eq 1 ){
        @recent7=`$NICOS_HOME/listtag.pl -d ${CVSROOT} ${cvsloc}/${package1}/$diroffset 2>&1`;
        print "DDDD ${cvsloc}/${package1}/$diroffset @recent7\n";
        $recent="";
        foreach (@recent7){
        next if ( /tags/ || /defined/ || !  /:/ || ! /${newver}/ );
        s/symbols//g;
        s/symbol//g;
        @arrr=split(":",$_);
        $recent=basename(@arrr[0]);
        $recent =~ s/ //g;
        $recent =~ s/[^a-zA-Z0-9_\-\$\.\*]//g;
        last;
        }
           print " OOOOOOOOOX  $recent7 --- $package --- $package1\n";
           print " OOOOOOOOOY  $recent\n";

             if ( "$policy_to_use_tag_even_not_found" ne "yes" ){
                if ( "$recent" eq "" ){
                $recent=${emergtag};
                }
                $newver=basename($recent);
             }
        } # if [ "$cvsavail

        } # if [ "$newver" != "official" ......

        if ( $newver eq "development" && $cvsavail eq 1 ){
        $newver=$emergtag;
        } # if [ "$newver" = "development" ]; then

        newrel_link($newver, $package1);

          if ( $newver eq "official" || $newver eq "recent" || $newver eq "development" ){
#         print "*** $package1: using new $newver\n";
	      if ( $code eq 0 ) { printf OUT "%-54s%-1s%-25s%-1s%-70s\n", $package1, " ", $newver, " ", @addr;}
          } 
          else
          {
#         print "*** $package1: using requested $newver\n";
	      if ( $code eq 0 ) { printf OUT "%-51s%-1s%-25s%-1s%-70s\n", $package1, " ", $newver, " ", @addr;}
          } 
}   # if [ "$newver" eq "delete" ]; then
}   # done 

close (IN);
close (OUT);

system("cp -p ${pfilegen} ${pfilegen_copy}");

chdir "$prevdir" or die "Can not cd to $previr: $!\n";                    

if ( $cvserr <= 3 ){
    exit 0;}
else{ 
    exit $cvserr;}







