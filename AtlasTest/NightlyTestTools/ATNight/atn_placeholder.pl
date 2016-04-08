#!/usr/local/bin/perl
#
#
#
use Env;
#@output = ();
 
#-> parse arguments
#    print " ---------- $#ARGV $ARGV[0] \n";
if ($#ARGV != 1) {
    print " two args req: infile, outfile \n";
    exit 1;}
 
my $infile = "$ARGV[0]";
my $outfile = "$ARGV[1]";
my $oldstring = "\QAtlasRelease/*/cmt\E";
my $newstring = "Atlas*Release/cmt"; 
my $oldstring1 = "\QAthenaCommon/*/share\E";
my $newstring1 = "AthenaCommon/share";
 
open(WRITEDATA,">$outfile");
open(FILE,"<$infile");
$nrec = 0;
    while (<FILE>)
{
    $nrec++;
    chomp;
#    print " ------- $_ \n ";
    if( $_ =~ "$oldstring" )
    {
        ( $string_generated = $_ ) =~ s/$oldstring/$newstring/;
        print WRITEDATA "$string_generated\n";
    }
    elsif( $_ =~ "$oldstring1" )
	  {
	      ( $string_generated = $_ ) =~ s/$oldstring1/$newstring1/;
	      print WRITEDATA "$string_generated\n";
	  }
    else
    {print WRITEDATA "$_\n"; 
#     print "$_\n";
    }
}
 
 
 
