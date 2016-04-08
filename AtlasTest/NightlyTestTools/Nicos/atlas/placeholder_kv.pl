#!/usr/local/bin/perl
#
#
#
use Env;
use File::Basename;
use File::Glob;

#
if ($#ARGV != 0) {
    print " one arg req: testname \n";
    exit 1;}
 
my $testnam = "$ARGV[0]";
my $NICOS_RELHOME="$NICOS_RELHOME";
@dname=glob("$NICOS_RELHOME/Tools/KitValidation/*/etc");
$conf_file="$dname[0]/KitValidation.conf";
$copy_file="$dname[0]/KitValidation.conf_copy";
#
if ($testnam eq "back")
{
    system("cp -a $copy_file $conf_file");
}
else
{
system("cp -a $conf_file $copy_file");
#system("rm $conf_file");
open(WRITEDATA,">$conf_file");
open(FILE,"<$copy_file");
$nrec = 0;
    print WRITEDATA "#!/bin/sh\n";
    while (<FILE>)
{
    $nrec++;
    chomp;
    next if ($_ =~ /^#/ );
    next if (length($_) eq 0);
    if ( $_ =~ /^ALL/ )
    {
    if ( $_ =~ /^ALL:.+:${testnam}\.kvt.+$/ )
	     {
		 print WRITEDATA "$_\n";
	     }
    }
    else
    {
    print WRITEDATA "$_\n";
    }

}
}
