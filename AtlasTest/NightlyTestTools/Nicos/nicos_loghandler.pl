#!/usr/local/bin/perl
#                                                                             
# NICOS - NIghtly COntrol System
# Author Alex Undrus <undrus@bnl.gov>
#
# ----------------------------------------------------------
# nicos_loghandler
# ----------------------------------------------------------
#
sub compar{
    my $strg=shift;
    ( $lin1 = $strg ) =~ s/ //g;
    if (length($lin1) != 0){
#       remove trailing leading stretches whitespaces
                 $lll = join(' ', split(' ', $strg));
                 @fields = split(" ", $lll);
                 $package1 = $fields[0];
                 return length($package1);
	     }
    else{
	return 0;
             }
}

sub maxstring {
my @list = @_;
my $lengthmax=0;
my $i=0;

for (@list)
{ 
if ( length > $lengthmax )
{$lengthmax=length; $pick=$i;}
}
$i++;
}

use Env;
use Cwd;
use File::Basename;

my $prevdir = cwd;

my $file_suffix="";
my $unittest=0;
my $qatest=0;
my $infile="";

if ($#ARGV>=0) {
    $_ = $ARGV[0];
    /^-[qufl]+$/ && /u/ && do { $unittest=1; };
    /^-[qufl]+$/ && /q/ && do { $qatest=1; };
    /^-[qufl]+$/ && /f/ && do { shift; $file_suffix="$ARGV[0]"; };
    /^-[qufl]+$/ && /l/ && do { shift; $infile="$ARGV[0]"; };
    /^-[qufl]+$/ && shift;
    }

my $NICOS_WORK_AREA="$NICOS_WORK_AREA";
my $NICOS_BUILDLOG="$NICOS_BUILDLOG";
my $NICOS_UNITTESTLOG="$NICOS_UNITTESTLOG";
my $NICOS_QALOG="$NICOS_QALOG";
my $NICOS_BUILDLOG_SEPARATOR="$NICOS_BUILDLOG_SEPARATOR";
my $NICOS_DBFILE = "$NICOS_DBFILE";
my $NICOS_DBFILE_GEN = "$NICOS_DBFILE_GEN";
my $NICOS_HOSTNAME = "$NICOS_HOSTNAME";
my $NICOS_USE_LAST_LOGFILE = "$NICOS_USE_LAST_LOGFILE";
# option for nicos_loghandler:
# if NICOS_FULLNAME_INSEPARATOR=yes then
# the package name should appear with its container
# in separator, for example LArCalorimeter/Recon/SomePackage
my $NICOS_FULLNAME_INSEPARATOR="$NICOS_FULLNAME_INSEPARATOR"; 
if ( "${NICOS_FULLNAME_INSEPARATOR}" ne "yes" )
{$NICOS_FULLNAME_INSEPARATOR="no";}

my $MAKELOG = "${NICOS_BUILDLOG}";
if ( $unittest eq 1 )
{ $MAKELOG = "$NICOS_UNITTESTLOG" };
if ( $qatest eq 1 )
{ $MAKELOG = "$NICOS_QALOG" };
if ( $infile ne "" )
{ $MAKELOG = "$infile" };

my $LOGDIR = dirname($MAKELOG);
my $LOGHANDLER_REPORT="$NICOS_WORK_AREA/nicos_loghandler_report";
if ( $unittest eq 1 )
{ $LOGHANDLER_REPORT="$NICOS_WORK_AREA/nicos_loghandler_report_unittest"; };
if ( $qatest eq 1 )
{ $LOGHANDLER_REPORT="$NICOS_WORK_AREA/nicos_loghandler_report_qatest"; };
my $cntr=0;
my $stat=0; 
my $dbfile = basename($NICOS_DBFILE_GEN);
my $dbfilegen = "${NICOS_WORK_AREA}/${dbfile}_res";
my $separator = "";
my $pkg = "";
#($separator = $NICOS_BUILDLOG_SEPARATOR) =~ s/[-]/\\\-/g;
( $separator = $NICOS_BUILDLOG_SEPARATOR ) =~ s/ //g;

open(DBF,"$dbfilegen");
chomp(my @dbc1=<DBF>);
close(DBF);

@dbc2 = sort { compar($a) <=> compar($b) } @dbc1;
@dbc = reverse (@dbc2);
#foreach (@dbc)
#{print "DDDD $_\n"};
@dbc1=(); @dbc2=();

open(REP,">$LOGHANDLER_REPORT");

my @dbc_norm;
foreach (@dbc)
{
#       Ignore comments
        next if ($_ =~ /^#/);
        $lin1 =~ s/ //g;
	next if (length($lin1) != 0);
#       remove trailing leading stretches whitespaces
                 $lll = join(' ', split(' ', $_));
                 @fields = split(" ", $lll);
                 $package1 = $fields[0];
                 @comp = split("/", $package1);
                 push (@dbc_norm,join('/', @comp)); 
}
#	my %seen = ();
#        my @dbc_cn_u;
#	foreach $item (@dbc_cn){
#	    push(@dbc_cn_u, $item) unless $seen{$item}++;
#        }        

        my $pkg_prev="";
        my $pkg="";
        my @entry=("","","","","");
        my @file_occurence=();
#
open(IN,"<$MAKELOG")
    or die "Couldn't open $MAKELOG for reading: $!\n";
while (<IN>) {
    chomp;
    $stat = 1; 
    $rec = $_;  
    ( $line = $rec ) =~ s/ //g;
    if ( length($line) != 0 ){  
    if ( $line =~ /$separator/ ) {$stat = 0;}

        if ( $stat eq 0 ){ 

	if ( $NICOS_FULLNAME_INSEPARATOR eq "yes" ){
	$label_p=0;
        $cntr=0;
        $pkg="";
        foreach (@dbc)
        {
#       Ignore comments 
        next if ($_ =~ /^#/ );
#       remove trailing leading stretches whitespaces 
		 $lll = join(' ', split(' ', $_));
                 @fields = split(" ", $lll);
                 $package1 = $fields[0]; 
                 next until ($rec =~ /[^0-9a-zA-Z]${package1}[^0-9a-zA-Z_]/);
	         $pkg=$package1;
                 $label_p=1;
                 last; 
        }
        }
        else
        {
            $label_p=0;
            $coinc=1; 
foreach (@dbc)
{
#       Ignore comments
        next if ($_ =~ /^#/ );
        ( $lin1 = $_ ) =~ s/ //g;
        next if (length($lin1) eq 0);
#       remove trailing leading stretches whitespaces
                 $lll = join(' ', split(' ', $_));
                 @fields = split(" ", $lll);
                 $package1 = $fields[0];
                 @comp = split("/", $package1);
                 for ($i=$#comp; $i >= 0; $i--){
                     $abcd = $comp[$i]; 
		     if ($rec =~ /[^0-9a-zA-Z]$abcd[^0-9a-zA-Z_]/){
		     $im=$#comp-$i; $entry[$im]=$abcd; $coinc=0;}
                 }  
}	    
        next if ($coinc eq 0);
        $frst=-1;
        $flst=-1;
        $ssn=1;
	for ($m=0; $m <= $#entry; $m++){
	    if ( $entry[$m] eq "" && $frst eq -1 ){
		$frst=$m; }
            if ( $entry[$m] ne "" ){
		$flst=$m; }
  	}
        if ( $flst ne "" && $frst > $flst ){ $ssn=0;} 

        if ($ssn eq 0){
	@pkgar=();
        for ($m=$#entry; $m >= 0; $m--){            
	    if ( $entry[$m] ne "") {push(@pkgar, $entry[$m]);}
        } 
        $pkg = join('/', @pkgar);
        @pkgar_prev = split('/',$pkg_prev);
        $optim="";            
        if ( $#pkgar_prev > 0 ){
        $r1=1; $r2=1; 
        foreach $item (@dbc_norm){
	$d=dirname($pkg_prev);
        @dar=split('/',$d);  
        @dar1=();
        if ($pkg eq $item) { $r1=0;}
        for ($l=0; $l <= $#dar; $l++){
	push(@dar1, $dar[$l]);
        $djoin=join('/', @dar1);
        $opti="$djoin/$pkg";
        if ( $opti eq $item ){ $r2=0; $optim=$opti; last;} 
        }        
        }         
        if ( $r2 eq 0 ) { $pkg="$optim"; }
        } 
        $label_p=1;
        @entry=("","","","","");
        }  
        } # if ( ${NICOS_FULLNAME_INSEPARATOR} == "yes" )
        } # if ($stat 

        $signat = 0;
        if ($pkg ne "") {$signat=1;}

        if ( $stat eq 0 && $signat ne 0 )
        {
        if ( $cntr eq 0 ) {$cntr=1;}
        if ( $cntr >= 2 ) {$cntr=1;}  
        }  
 
        if ( $cntr eq 1 && $stat eq 0 && $label_p eq 1 )
          {
          ($pkgn = $pkg) =~ s/\//_/g; 
          if ($pkgn eq "") { $pkgn = $pkg; }
          $pkgbase=basename($pkg); 
          $file = "$LOGDIR/${pkgn}.loglog${file_suffix}";
          $pkg_prev=$pkg;
          print "nicos_loghandler: found separator for package $pkg\n";
          $foc=1;
          foreach $fo (@file_occurence){
          if ($fo eq $pkgn) {$foc=0;}  
          }
          if ($foc eq 0 && $NICOS_USE_LAST_LOGFILE ne "yes") { open(OUT, ">> $file");}
          else { 
          if ( -f $file )
          { unlink ($file) or die "Can not delete ${file}: $!\n"; }
          open(OUT, "> $file"); 
          push(@file_occurence, $pkgn);
          }
          print OUT                                                           
"----------------------------------------------------------------------\n";
          print OUT
"build output for package $pkg (generated by nicos_loghandler)\n";
          print OUT
"performed on machine ${NICOS_HOSTNAME}\n";
          print OUT
"----------------------------------------------------------------------\n";

          close(OUT);
          print REP "$pkg \n";
          } # if [ "$cntr" eq 1 

        if ($cntr >= 1) 
        { 
	$file = "$LOGDIR/${pkgn}.loglog${file_suffix}";
        open(OUT, ">> $file"); 
        print OUT "${rec}\n";
        close(OUT);
        }

        if ($cntr ne 0) { $cntr++; }
     } # if length
} # while
close IN;
close REP;

chdir "$prevdir" or die "Can not cd to $previr: $!\n";
#end of Log maker
