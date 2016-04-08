#!/usr/local/bin/perl
use Env;
use File::Basename;

my $NICOS_DBFILE="$NICOS_DBFILE";
my $NICOS_DBFILE_GEN="$NICOS_DBFILE_GEN";
my $NICOS_PROJECT_RELNAME="$NICOS_PROJECT_RELNAME";
my $NICOS_WEBDIR="$WEBDIR";

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
print " nicos_dbfile_transformer: problem: package tags can not be retrived from file:\n";
print " $pfile : it does not exist\n";
exit 1
}

print "------------------------------------------------------------ \n";
print " nicos_dbfile_transformer: info: produces final db file from:\n";
print " $pfile \n";
print " this function should be called only when checkout is bypassed \n";
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
	     if ( $code eq 0 ) { printf OUT "%-54s%-1s%-25s%-1s%-70s\n", $package1, " ", $newver, " ", @addr;}
	 }   # done

 
