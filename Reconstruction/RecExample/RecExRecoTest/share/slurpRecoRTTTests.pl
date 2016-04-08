#!/usr/bin/perl

# # #!/usr/bin/env perl

use strict;

use Storable;
use Digest::MD5 qw(md5_hex);

my %rttHashArray = ();
my %pdate = ();
my %ptag  = ();
my %savannahArray = ();
my %savannahSummary = ();
my %FPESummary = ();

my %rttPackageInfos = ();
my $now="";
my @packages = ();

my % checks = ();

sub sortMessages
{
    my ( $TMPRef ) = @_;
    my %TArray = ();
    my ( @TMP    ) = @$TMPRef;
    foreach my $mline (@TMP)
    {
        $mline =~ s/\s+$//;
        if ( ! exists($TArray{$mline}) )
        {
            $TArray{$mline} = 0;
        }
        $TArray{$mline} +=  1;
    }
    return %TArray;
}

sub processLog
{
    my ( $file ) = @_;
    
    open(FILE,"<$file") || die "Could not open $file !!\n\n";
    my @lines=<FILE>;
    close(FILE);
    
    my $stage="athena";
    $checks{'Stage'}{'result'}="athena";
    
    foreach my $line ( @lines )
    {
	for my $href ( keys %checks )
	{
	    if ( $line =~ /$checks{$href}{'test'}/ )
	    {
		my $r0=$line;
		$r0   =~ s/$checks{$href}{'test'}/\1/g;
		$r0   =~ s/\s+$//;
		if ( $href eq "Stage" )
		{
		    my $r1=$checks{$href}{'result'};
		    # for Reco_trf stages, we actually find something here, reset to empty string
		    $r1=""  if ( $r1 eq "athena" );
		    # print "S : $href : $stage : $r1 + $r0\n" ;
		    $checks{$href}{'result'}="$r1<$r0>";
		    $stage=$r0;
		}
		else
		{
		    # print "R0 : $href : $stage : $r0\n" ;
		    if ( $href =~ m/^#/ )
		    {
			if ( ! exists($checks{$href}{'result'}{$stage}{$r0}) )
			{
			    $checks{$href}{'result'}{$stage}{$r0} = 0;
			}
			$checks{$href}{'result'}{$stage}{$r0} +=  1;
		    }
		    else
		    {
			my $r1=$checks{$href}{'result'}{$stage};
			# print "R1 : $href : $stage : $r1 + $r0\n" ;
			$checks{$href}{'result'}{$stage}="$r1<$r0>";
		    }
		}
	    }
	}
    }
    if ( $checks{ "CoreDump" }{'result'}{$stage} ne "" )
    {
	my $r0="\n";
	my $r1=76;
	foreach my $line ( @lines )
	{
	    $r1 = 75 if ( $line =~ m/^Core dump from CoreDumpSvc on / );
	    if ( $r1 > 0 && $r1 < 76 )
	    {
		$r0 = $r0 . ">>>>" . $line;
		$r1--;
	    }
	}
	$checks{ "CoreDump" }{'result'}{$stage} = $r0;
    }
    return scalar(@lines);
}

$checks{ "ReturnCode" }  = { test => '^Py:Athena\s+INFO leaving with code(.+)$' };
$checks{ "FPEAuditor"}   = { test => '^FPEAuditor\s+WARNING FPE (.+)$' };
$checks{ "Stage" }       = { test => '^Py:[A-Z].* *INFO Executing ([A-Za-z]*)\:.+$' };
$checks{ "Events" }      = { test => '^Py:PerfMonSvc *INFO Events processed: (\d+)' };
$checks{ "Algorithms" }  = { test => '^Py:PerfMonSvc *INFO job had (\[.*\]) algorithms' };
$checks{ "Memory1" }     = { test => '^Py:PerfMonSvc *INFO *first-evt vmem: (.*)' };
$checks{ "Memory2" }     = { test => '^Py:PerfMonSvc *INFO *10th -evt vmem: (.*)' };
$checks{ "Memory3" }     = { test => '^Py:PerfMonSvc *INFO *last -evt vmem: (.*)' };
$checks{ "MemLeak" }     = { test => '^Py:PerfMonSvc *INFO *.* fitted vmem-slope \(\d* points\): (.*)' };
$checks{ "CPU" }         = { test => '^Py:PerfMonSvc *INFO Si2k:.*>: (.*)' };
$checks{ "Chrono" }      = { test => '^ChronoStatSvc *INFO Time User   : Tot=(.*)\s+#=  1$' };
$checks{ "CoreDump" }     = { test => '^Core dump from CoreDumpSvc on ' }; 
$checks{ "#VERBOSE" }     = { test => '^(\S+)\s+VERBOSE .+$' };
$checks{ "#DEBUG" }       = { test => '^(\S+)\s+DEBUG .+$' };
$checks{ "#INFO" }        = { test => '^(\S+)\s+INFO .+$' };
$checks{ "#WARNING" }     = { test => '^(\S+)\s+WARNING .+$' };
$checks{ "#RECOVERABLE" } = { test => '^(\S+)\s+RECOVERABLE .+$' };
$checks{ "#ERROR" }       = { test => '^(\S+)\s+ERROR .+$' };
$checks{ "#FATAL" }       = { test => '^(\S+)\s+FATAL .+$' };

my $file="/afs/cern.ch/atlas/project/RTT/prod/Results/rtt_j_r/rel_3/devval/build/i686-slc5-gcc43-opt/offline/RecJobTransformTests/amitag_q122/amitag_q122_log";
if ( $#ARGV >= 0 )
{
    $file = $ARGV[0];
}
print " processing logfile $file\n";
my $lines = processLog($file);
print " done processing logfile with $lines lines\n";

my $logfilename="parsedlogfile.txt";
open(LOGFILE,">" . $logfilename);


print LOGFILE ":LogFileName : $file\n";

print LOGFILE ":Stages : $checks{'Stage'}{'result'}\n";
for my $s ( split(/>/, $checks{'Stage'}{'result'}) )
{
    $s =~ s/^<//;
    $s =~ s/\s+$//;
    print LOGFILE "Stage : $s\n";
    for my $href ( keys %checks )
    {
	if ( $href ne "Stage" )
	{
	    if ( $href =~ m/^#/ )
	    {
		print LOGFILE " $s " . substr($href,1) . "\n";
		my $t=0;
		my $tt=0;
		for my $m ( sort { $checks{$href}{'result'}{$s}{$b} <=> $checks{$href}{'result'}{$s}{$a} } keys %{ $checks{$href}{'result'}{$s} } )
		{
		    if ( $tt < 10 )
		    {
			print LOGFILE "   $checks{$href}{'result'}{$s}{$m} $m\n";
			$tt++;
		    }
		    $t += $checks{$href}{'result'}{$s}{$m};
		}
		if ( $t > 0 )
		{
		    print LOGFILE "   $t " . substr($href,1) . "\n";
		}
	    }
	    else
	    {
		print LOGFILE " $s $href $checks{$href}{'result'}{$s}\n";
	    }
	}
    }
}
exit(0);
