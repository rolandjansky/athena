#!/usr/bin/perl

# # #!/usr/bin/env perl

use strict;

use Storable;
use Digest::MD5 qw(md5_hex);
use FileHandle;

# use GD::Graph::histogram;

my $logfile="out.valgrind.log.gz";
my $dirname=".";

my %counters = ();
my %releases = ();

sub constructSVNLink
{
    my ($base, $relr, $project, $reln, $packpath, $package, $platform) = @_;
    # print "ROLF e : $base, $relr, $project, $reln, $packpath, $package, $platform\n";
    if ( ! exists($releases{$project}) )
    {
	open(RFILE, "< $base/$relr/$project/$reln/$project" . "Release/cmt/requirements" );
	my @rcontent = <RFILE>;
	close(RFILE);
	
	foreach my $line (@rcontent)
	{
	    if ( $line =~ m/^use /i )
	    {
		my ($rpackage,$rpacktag,$rpackpath) = ( $line =~ /^use\s+(\S+)\s+(\S+)\s+(\S+)/ );
		$rpackpath =~ s/\s+$//;
		$rpackage  =~ s/\s+$//;
		$rpacktag  =~ s/\s+$//;
		$releases{$project}{$rpackpath . "/" . $rpackage} = $rpacktag;
	    }
	}
    }
    my $string="";
    $string="https://svnweb.cern.ch/trac/atlasoff/browser/$packpath/$package/tags/" . $releases{$project}{$packpath . "/" . $package} if ( $releases{$project}{$packpath . "/" . $package} ne "" );
    return $string;
}

sub open_html
{
    my ($file,$name) = @_;
    $file->open("> " . $name);
    $counters{'HTMLName'}{$file} = $name;
}

sub prepare_html_report
{
    my ($file, $c1, $c2, $c3, $c4, $c5, $c6, $c7, $c8, $c9, $cr, $info) = @_;
    my $now = time();
    
    print $file "<!DOCTYPE doctype PUBLIC \"-//w3c//dtd html 4.0 transitional//en\">\n<html>";
    print $file "<head>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">";
    print $file "<title>Valgrind DHat Report:</title>\n</head>\n\n<body bgcolor=\"#ffffff\">";
    print $file "<center>\n<h1>Valgrind new dynamic allocation analysis tool:</h1></center>\n<h3>\n";
    print $file "<table cellpadding=\"10\">\n<tbody><tr align=\"center\">\n\n";
    
    print $file "reports:<\/br>";
    print $file "<table border=\"0\">\n";
    print $file "<tr><td>  <\/td><td><a href=NoAccess.html>No Access<\/a></td> <td> $c1 <\/td>";
    print $file "<td>allocated memory never accessed<\/td></tr>\n";
    print $file "<tr><td>or<\/td><td><a href=NoRead.html>No Read<\/a></td> <td> $c2 <\/td>\n";
    print $file "<td>allocated memory written to but never read from<\/td></tr>\n";
    print $file "<tr><td>or<\/td><td><a href=NoWrite.html>No Write<\/a></td> <td> $c3 <\/td>\n";
    print $file "<td>allocated memory never written to<\/td></tr>\n";
    print $file "<tr><td>or<\/td><td><a href=NoFree.html>No Free<\/a></td> <td> $c4 <\/td>\n";
    print $file "<td>not freed memory<\/td></tr>\n";
    print $file "<tr><td>or<\/td><td><a href=LowRead.html>Low Read<\/a></td> <td> $c5 <\/td>\n";
    print $file "<td>low reads from allocated memory<\/td></tr>\n";
    print $file "<tr><td>or<\/td><td><a href=LowWrite.html>Low Write<\/a></td> <td> $c6 <\/td>\n";
    print $file "<td>low writes to allocated memory<\/td></tr>\n";
    print $file "<tr><td>or<\/td><td><a href=MoreWrite.html>More Write than Read<\/a></td> <td> $c7 <\/td>\n";
    print $file "<td>more writes than reads<\/td></tr>\n";
    print $file "<tr><td>or<\/td><td><a href=LongLifetime.html>Long Lifetime<\/a></td> <td> $c8 <\/td>\n";
    print $file "<td>memory with long lifetime<\/td></tr>\n";
    print $file "<tr><td>or<\/td><td><a href=Rest.html>Rest<\/a></td> <td> $cr <\/td>\n";
    print $file "<td>remaining reports<\/td></tr>\n";
    print $file "</table>\n\n";
    print $file "NOTE: any report will show up only once, in topmost category.</table>\n\n";
    print $file "<pre><code>\n$info\n</code></pre>\n\n";
    print $file "</body>\n</html>\n"
}

sub prepare_html_jobinfo
{
    my ($file, $info) = @_;
    my $now = time();
    
    print $file "<!DOCTYPE doctype PUBLIC \"-//w3c//dtd html 4.0 transitional//en\">\n<html>";
    print $file "<head>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">";
    print $file "<title>Valgrind DHat JobInfo:</title>\n</head>\n\n<body bgcolor=\"#ffffff\">";
    print $file "<center>\n<h1>Valgrind new dynamic allocation analysis tool:</h1></center>\n<h3>\n";
    print $file "<table cellpadding=\"10\">\n<tbody><tr align=\"center\">\n\n";
    
    print $file "<pre><code>\n$info\n</code></pre>\n\n";
    
    print $file "<a href=Memory.eps><img src=Memory_sm.jpg align=\"right\" vspace=\"10\" hspace=\"20\" ></a>\n";
    print $file "<a href=MemoryAccess.eps><img src=MemoryAccess_sm.jpg align=\"right\" vspace=\"10\" hspace=\"20\" ></a>\n";
    print $file "<a href=Lifetime.eps><img src=Lifetime_sm.jpg align=\"right\" vspace=\"10\" hspace=\"20\" ></a>\n\n";
    print $file "</body>\n</html>\n"
}

sub prepare_html2
{
    my ($file, $fname) = @_;
    my $string="";
    my $now = time();
    
    $string = $string . "<!DOCTYPE doctype PUBLIC \"-//w3c//dtd html 4.0 transitional//en\">\n<html>";
    $string = $string . "<head>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">";
    $string = $string . "<title>Valgrind DHat Report: $fname</title>\n</head>\n\n<body bgcolor=\"#ffffff\">";
    $string = $string . "<center>\n<h1>Valgrind reported as $fname:</h1></center>\n<center>\n<h3>\n";
    $string = $string . "<table cellpadding=\"10\">\n<tbody><tr align=\"center\">\n\n";
    $string = $string . "<a href=Report.html>Back to main</a>\n\n";
    
    $string = $string . "<td> running number\n<td> in function\n<td> in library\n";
    print $file $string;
}

sub prepare_html
{
    my ($file, $fname, $name) = @_;
    my $string="";
    my $now = time();
    
    $string = $string . "<!DOCTYPE doctype PUBLIC \"-//w3c//dtd html 4.0 transitional//en\">\n";
    $string = $string . "<html>\n";
    $string = $string . "<head>\n";
    $string = $string . "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n\n";
    $string = $string . "<title>Valgrind DHat Report of " . $fname . "</title>\n";
    $string = $string . "</head>\n\n";
    $string = $string . "<body bgcolor=\"#ffffff\">\n";
    
    $counters{'Counter'}{$file} = 0;
    $counters{'FileName'}{$file} = $fname;
    $counters{'HTMLName'}{$file} = $name;

    print $file $string;
}

sub finish_html
{
    my ($file) = @_;
    print $file "</body></htm>";
}

sub print_html
{
    my ($file, $file2, $string, $pid) = @_;
    
    $string =~ s/</\&lt;/g;s/>/\&gt;/g;
    
    my @glines = grep { s/^==$pid==    by 0x.+: (.+) \((.+)\)$/\1\[\2\]/g } split '\n', $string;
    my $function = "";
    my $library  = "";
    foreach my $s ( reverse( @glines ) )
    {
	$function = $s if ( ( ! ( $s =~ /^std::/ ) ) and ( $s ne "???") );
    }
    
    ($function,$library) = ( $function =~ /^(.+)\[(.+)\]/ );
    $library  =~ s/^in //g;
    print $file2 "</td></tr>\n<tr align=\"center\">\n";
    print $file2 "<td>$counters{'Counter'}{$file}\n";
    print $file2 "<td><a href=\"$counters{'HTMLName'}{$file}#$counters{'FileName'}{$file}_$counters{'Counter'}{$file}\">";
    print $file2 "$function</a></h2>";
    print $file2 "<td>$library\n";
    
    print $file "<h2><a name=\"$counters{'FileName'}{$file}_$counters{'Counter'}{$file}\"";
    print $file " href=\"#$counters{'FileName'}{$file}_$counters{'Counter'}{$file}\">";
    print $file "$counters{'FileName'}{$file} $counters{'Counter'}{$file} in function \"$function\"</a></h2>\n";
    print $file "<pre><code>";
    @glines = split '\n', $string;
    foreach my $s ( @glines )
    {
	$function = $s if ( ( ! ( $s =~ /^std::/ ) ) and ( $s ne "???") );
	my ($function2,$library2) = ( $function =~ /^(.+)\((.+)\)/ );
	$library2 =~ s/^in //g;
	
	if ( $s =~ m"/afs/cern.ch/atlas/software/builds/nightlies/" )
	{
	    my $base;
	    my $relr;
	    my $project;
	    my $reln;
	    my $packpath;
	    my $package;
	    my $platform;
	    my $last;
	    ($base,$relr,$project,$reln,$packpath,$package,$platform,$last) = ( $library2 =~ m"(/afs/cern.ch/atlas/software/builds/nightlies)/(.+)/(.+)/(rel_.)/(.+)/(.+)/(.+)/lib(.+)\.so" );
	    
	    my $svn = "";
	    $svn = constructSVNLink($base, $relr, $project, $reln, $packpath, $package, $platform) if ( $base ne "" );
	    
	    if ( $svn ne "" )
	    {
		$library2="<a href=$svn target=\"_blank\">[SVN]</a> ";
	    }
	    else
	    {
		$library2="";
	    }
	}
	else
	{
	    $library2="";
	}
	$s =~ s/^(==$pid==    by 0x.+: .+) \((.+)\)$/\1 $library2\(\2\)/g;
	print $file $s . "\n";
    }
    
    print $file "</code></pre>\n";
    
    $counters{'Counter'}{$file}++;
}

print "DHAT postprocess >> $logfile \n";

open(FILE, "gunzip -c $dirname/$logfile | " );
my @content = <FILE>;
close(FILE);

my $init=0;
my $total_instructions=0;
my $max_live=0;
my $max_live_blocks=0;
my $tot_alloc=0;
my $tot_alloc_blocks=0;
my $report_max_live=0;
my $report_max_live_blocks=0;
my $report_tot_alloc=0;
my $report_tot_alloc_blocks=0;
my $report_dealloc=0;
my $report_dealloc_age=0;
my $report_acc_ratio_r=0;
my $report_acc_ratio_w=0;
my $report_acc_total_r=0;
my $report_acc_total_w=0;
my $top_allocs=0;
my $ordering="";
my %Summary=();
my @reports=();
my $nreport0=0;
my $nreport1=0;
my $fullline="";
my $pid="4230";

($pid) = ( $content[0] =~ /^==([0-9]+)== DHAT, a dynamic heap analysis tool/ );
print "got <pid> = $pid from line zero:" . $content[0] . ":\n";

open(FILE, "cat $dirname/top.log | " );
my @memcontent = <FILE>;
close(FILE);

my @memorylines = grep { s/^\s*$pid\s+\w+\s+\d+\s+\d+\s+([\w\.]+)\s+([\w\.]+)\s+[\w\.]+\s+\S+\s+([\w\.]+)\s+[\w\.]+\s+([\w\.:]+).+/\1 \2 \3 \4/g } @memcontent;

print "found " . scalar @memorylines . " lines for memory monitoring\n";

foreach my $line (@content)
{
    if ( $line =~ m/^==$pid== ======== SUMMARY STATISTICS ========/i )
    {
	$init=1;
	$fullline = "";
    }
    
    if ( $line =~ m/^==$pid== ======== ORDERED BY decreasing .+ allocators ========/i )
    {
	($ordering,$top_allocs) = ( $line =~ /^==$pid== ======== ORDERED BY decreasing "(.+)": top (.+) allocators ========/ );
	$init=2;
    }
    
    if ( $init == 1 )
    {
	if ( $line =~ m/^==$pid== guest_insns: /i )
	{
	    ($total_instructions) = ( $line =~ /^==$pid== guest_insns:\s+(.+)/ );
	    $total_instructions =~ s/,//g;
	}
	if ( $line =~ m/^==$pid== max_live: /i )
	{
	    ($max_live,$max_live_blocks) = ( $line =~ /^==$pid== max_live:\s+(.+) in\s+(.+) blocks/ );
	    $max_live        =~ s/,//g;
	    $max_live_blocks =~ s/,//g;
	}
	if ( $line =~ m/^==$pid== tot_alloc: /i )
	{
	    ($tot_alloc,$tot_alloc_blocks) = ( $line =~ /^==$pid== tot_alloc:\s+(.+) in\s+(.+) blocks/ );
	    $tot_alloc        =~ s/,//g;
	    $tot_alloc_blocks =~ s/,//g;
	}
#==4230== guest_insns:  122,798,380,987
#==4230== 
#==4230== max_live:     255,458,117 in 6,435,493 blocks
#==4230== 
#==4230== tot_alloc:    2,668,402,686 in 63,814,946 blocks
#==4230== 
#==4230== insns per allocated byte: 46
#==4230== 
#==4230== 
#==4230== ======== ORDERED BY decreasing "max-bytes-live": top 25000 allocators ========
    }
    if ( $line =~ m/^==$pid== -------------------- .+ of $top_allocs --------------------/i )
    {
	push @{ reports }, { String=>$fullline,
			     Class=>0,
			     MaxLive=>$report_max_live,
			     MaxLiveBlocks=>$report_max_live_blocks,
			     TotalAlloc=>$report_tot_alloc,
			     TotalAllocBlocks=>$report_tot_alloc_blocks,
			     Dealloc=>$report_dealloc,
			     DeallocAge=>$report_dealloc_age,
			     AccRatioR=>$report_acc_ratio_r,
			     AccRatioW=>$report_acc_ratio_w,
			     AccTotalR=>$report_acc_total_r,
			     AccTotalW=>$report_acc_total_w,
			     Reported=>0
	};
	if ( ! exists($Summary{'Reports'}) )
	{
	    $Summary{'Reports'} = [ { String=>$fullline,
				      Class=>0,
				      MaxLive=>$report_max_live,
				      MaxLiveBlocks=>$report_max_live_blocks,
				      TotalAlloc=>$report_tot_alloc,
				      TotalAllocBlocks=>$report_tot_alloc_blocks,
				      Dealloc=>$report_dealloc,
				      DeallocAge=>$report_dealloc_age,
				      AccRatioR=>$report_acc_ratio_r,
				      AccRatioW=>$report_acc_ratio_w,
				      AccTotalR=>$report_acc_total_r,
				      AccTotalW=>$report_acc_total_w,
				      Reported=>-1
				    } ];
	}
	else
	{
	    push @{ $Summary{'Reports'} }, { String=>$fullline,
					     Class=>0,
					     MaxLive=>$report_max_live,
					     MaxLiveBlocks=>$report_max_live_blocks,
					     TotalAlloc=>$report_tot_alloc,
					     TotalAllocBlocks=>$report_tot_alloc_blocks,
					     Dealloc=>$report_dealloc,
					     DeallocAge=>$report_dealloc_age,
					     AccRatioR=>$report_acc_ratio_r,
					     AccRatioW=>$report_acc_ratio_w,
					     AccTotalR=>$report_acc_total_r,
					     AccTotalW=>$report_acc_total_w,
					     Reported=>0
	    };
	}
	
	($nreport0) = ( $line =~ /^==$pid== -------------------- (.+) of $top_allocs --------------------/ );
	$nreport1++;
	$fullline = "";
    }
    if ( $init > 0 )
    {
	if ( $line =~ m/^==$pid==/i )
	{
	    $fullline = $fullline . $line if ( $line =~ m/^==$pid== ./i );
	}
	if ( $line =~ m/^==$pid== ==============================================================/i )
	{
	    push @{ reports }, { String=>$fullline,
				 Class=>0,
				 MaxLive=>$report_max_live,
				 MaxLiveBlocks=>$report_max_live_blocks,
				 TotalAlloc=>$report_tot_alloc,
				 TotalAllocBlocks=>$report_tot_alloc_blocks,
				 Dealloc=>$report_dealloc,
				 DeallocAge=>$report_dealloc_age,
				 AccRatioR=>$report_acc_ratio_r,
				 AccRatioW=>$report_acc_ratio_w,
				 AccTotalR=>$report_acc_total_r,
				 AccTotalW=>$report_acc_total_w,
				 Reported=>0
	    };
	    push @{ $Summary{'Reports'} }, { String=>$fullline,
					     Class=>0,
					     MaxLive=>$report_max_live,
					     MaxLiveBlocks=>$report_max_live_blocks,
					     TotalAlloc=>$report_tot_alloc,
					     TotalAllocBlocks=>$report_tot_alloc_blocks,
					     Dealloc=>$report_dealloc,
					     DeallocAge=>$report_dealloc_age,
					     AccRatioR=>$report_acc_ratio_r,
					     AccRatioW=>$report_acc_ratio_w,
					     AccTotalR=>$report_acc_total_r,
					     AccTotalW=>$report_acc_total_w,
					     Reported=>0
	    };
	    $init = -1;
	}
	if ( $init > 1 )
	{
#==4230== max-live:    256 in 1 blocks
#==4230== tot-alloc:   256 in 1 blocks (avg size 256.00)
#==4230== deaths:      1, at avg age 105,272,295,842
#==4230== acc-ratios:  10.29 rd, 2.50 wr  (2,636 b-read, 641 b-written)
	    if ( $line =~ m/^==$pid== max-live: /i )
	    {
		# print "ROLF :: $line";
		($report_max_live,$report_max_live_blocks) = ( $line =~ /^==$pid== max-live:\s+(.+) in\s+(.+) blocks/ );
		$report_max_live        =~ s/,//g;
		$report_max_live_blocks =~ s/,//g;
	    }
	    if ( $line =~ m/^==$pid== tot-alloc: /i )
	    {
		($report_tot_alloc,$report_tot_alloc_blocks) = ( $line =~ /^==$pid== tot-alloc:\s+(.+) in\s+(.+) blocks/ );
		$report_tot_alloc        =~ s/,//g;
		$report_tot_alloc_blocks =~ s/,//g;
	    }
	    if ( $line =~ m/^==$pid== deaths: /i )
	    {
		($report_dealloc,$report_dealloc_age) = ( $line =~ /^==$pid== deaths:\s+(.+), at avg age (.+)/ );
		$report_dealloc        =~ s/,//g;
		$report_dealloc_age   =~ s/,//g;
		
		if ( $line =~ m/^==$pid== deaths:      none \(none of these blocks were freed\)/i )
		{
		    $report_dealloc        = "never";
		    $report_dealloc_age   = "never";
		}
		else
		{
		    $fullline =~ s/\s+$//;
		    $fullline = $fullline . " (" . sprintf("%.1f", 100 * $report_dealloc_age / $total_instructions ) . " per cent of total instr.)\n";
		}
	    }
	    if ( $line =~ m/^==$pid== acc-ratios: /i )
	    {
		($report_acc_ratio_r,
		 $report_acc_ratio_w,
		 $report_acc_total_r,
		 $report_acc_total_w) = ( $line =~ /^==$pid== acc-ratios:\s+(.+) rd,\s+(.+) wr\s+\((.+) b-read, (.+) b-written\)/ );
		$report_acc_total_r =~ s/,//g;
		$report_acc_total_w =~ s/,//g;
	    }
	}
    }
}

print "total instructions : $total_instructions\n";
print "maximal live       : $max_live in $max_live_blocks blocks\n";
print "total allocations  : $tot_alloc in $tot_alloc_blocks blocks\n";
print "ordering by        : $ordering\n";
print "print n allocators : $top_allocs\n";

print "no of reports: " . $nreport1 . "\n";
print "Summary:\n";
print $reports[0]{'String'} . "\n";
print "REPORT number 1:\n";
print $reports[1]{'String'} . "\n";
print "last REPORT:\n";
print ">\n$fullline<\n";

print "got these numbers from it:\n";
print "max-live  (in n blocks) : $report_max_live ($report_max_live_blocks)\n";
print "tot-alloc (in n blocks) : $report_tot_alloc ($report_tot_alloc_blocks)\n";
print "dealloc   (when)        : $report_dealloc ($report_dealloc_age)\n";
print "acc-ratios              : $report_acc_ratio_r $report_acc_ratio_w $report_acc_total_r $report_acc_total_w\n";

# my $hashFile="DHAT.db";
# store \%Summary, $hashFile;

#
#plot [-6:6] sin(x)
#gnuplot> set terminal postscript
#Terminal type set to 'postscript'
#Options are 'landscape monochrome "Courier" 14'
#gnuplot> set output "sin.ps"
#gnuplot> replot

my $x=3.2;
my $y=3.2;
my $t=3.2;
my $l10=log(10);

print "creating plots\n";
# open (PIPE, "|/afs/cern.ch/sw/lcg/app/releases/ROOT/5.22.00a/i686-slc5-gcc34-opt/root/bin/root -l -b - > log 2>&1") or die "no root";
# open (PIPE, "|/atlas/Software/root/bin/root -l -b - > log 2>&1") or die "no root";
open (PIPE, "|root -l -b - > log 2>&1") or die "no root";
# open (PIPE, ">log") or die "no root";
## force buffer to flush after each write
use FileHandle;
PIPE->autoflush(1);
print PIPE "TCanvas can(\"c1\",\"c1\",1200,800);\n";
print PIPE "TH2D h1(\"h1\",\"h1\",180,-2.5,6.5,180,-2.5,6.5);\n";
print PIPE "TH1D h2(\"h2\",\"h2\",100,-10.0,0.0);\n";
print PIPE "TH1D m1(\"m1\",\"m1\"," . scalar @memorylines . ",0.0," . scalar @memorylines . ".0);\n";
print PIPE "TH1D m2(\"m2\",\"m2\"," . scalar @memorylines . ",0.0," . scalar @memorylines . ".0);\n";
print PIPE "TH1D m3(\"m3\",\"m3\"," . scalar @memorylines . ",0.0," . scalar @memorylines . ".0);\n";
print PIPE "TH1D m4(\"m4\",\"m4\"," . scalar @memorylines . ",0.0," . scalar @memorylines . ".0);\n";
print PIPE "h1.SetStats(0);\n";
print PIPE "h2.SetStats(0);\n";
print PIPE "h1.SetTitle(\"read vs write ratios;log_{10}(R_{read});log_{10}(R_{write})\");\n";
print PIPE "h2.SetTitle(\"average lifetime of allocations;log_{10}(rel. lifetime)\");\n";
for my $index ( 0 .. $#{ $Summary{'Reports'} } )
# for my $index ( 0 .. 3000 )
{
    print "  " . $index . "\n" if $index % 1000 == 0;
    $x = -2.45;
    $y = -2.45;
    $t = 1;
    $x = log($Summary{'Reports'}[$index]{'AccRatioR'}) / $l10 if ($Summary{'Reports'}[$index]{'AccRatioR'} > 0 );
    $y = log($Summary{'Reports'}[$index]{'AccRatioW'}) / $l10 if ($Summary{'Reports'}[$index]{'AccRatioW'} > 0 );
    $t = log($Summary{'Reports'}[$index]{'DeallocAge'} / $total_instructions) / $l10 if ($Summary{'Reports'}[$index]{'DeallocAge'} > 0 );
    print PIPE "h1.Fill($x,$y);\n";
    print PIPE "h2.Fill($t);\n";
}
print PIPE "h1.Draw(\"box\");\n";
print PIPE "double x0[] = { 6.5, -2.5 };\n";
print PIPE "double y0[] = { 6.5, -2.5 };\n";
print PIPE "TGraph *gr0 = new TGraph(2,x0,y0);\n";
print PIPE "gr0->SetLineColor(2);\n";
print PIPE "gr0->SetLineWidth(1001);\n";
print PIPE "gr0->SetFillStyle(3005);\n";
print PIPE "gr0->SetFillColor(2);\n";
print PIPE "gr0->Draw(\"C\");\n";
print PIPE "double x1a[] = { -2.45, -2.45001 };\n";
print PIPE "double y1a[] = { 6.5, -2.45 };\n";
print PIPE "TGraph *gr1a = new TGraph(2,x1a,y1a);\n";
print PIPE "gr1a->SetLineColor(3);\n";
print PIPE "gr1a->SetLineWidth(1001);\n";
print PIPE "gr1a->SetFillStyle(3005);\n";
print PIPE "gr1a->SetFillColor(3);\n";
print PIPE "gr1a->Draw();\n";
print PIPE "double x1b[] = { -2.45,  6.5 };\n";
print PIPE "double y1b[] = { -2.45, -2.45 };\n";
print PIPE "TGraph *gr1b = new TGraph(2,x1b,y1b);\n";
print PIPE "gr1b->SetLineColor(3);\n";
print PIPE "gr1b->SetLineWidth(1001);\n";
print PIPE "gr1b->SetFillStyle(3005);\n";
print PIPE "gr1b->SetFillColor(3);\n";
print PIPE "gr1b->Draw();\n";
print PIPE "double x2[] = { -1, -1.00001 };\n";
print PIPE "double y2[] = { 6.5, -2.45 };\n";
print PIPE "TGraph *gr2 = new TGraph(2,x2,y2);\n";
print PIPE "gr2->SetLineColor(4);\n";
print PIPE "gr2->SetLineWidth(1001);\n";
print PIPE "gr2->SetFillStyle(3005);\n";
print PIPE "gr2->SetFillColor(4);\n";
print PIPE "gr2->Draw();\n";
print PIPE "double x3[] = {  -2.45, 6.5 };\n";
print PIPE "double y3[] = { -1, -1 };\n";
print PIPE "TGraph *gr3 = new TGraph(2,x3,y3);\n";
print PIPE "gr3->SetLineColor(5);\n";
print PIPE "gr3->SetLineWidth(1001);\n";
print PIPE "gr3->SetFillStyle(3005);\n";
print PIPE "gr3->SetFillColor(5);\n";
print PIPE "gr3->Draw();\n";
print PIPE "h1.Draw(\"boxsame\");\n";

print PIPE "TLegend leg2(0.3,0.9,0.6,0.75);\n";
print PIPE "leg2.AddEntry(gr1b,\"No Access\",\"f\");\n";
print PIPE "leg2.AddEntry(gr2,\"Low Read\",\"f\");\n";
print PIPE "leg2.AddEntry(gr3,\"Low Write\",\"f\");\n";
print PIPE "leg2.AddEntry(gr0,\"More Write than Read\",\"f\");\n";
print PIPE "leg2.Draw();\n";

print PIPE "c1.Print(\"MemoryAccess.jpg\");\n";
print PIPE "c1.Print(\"MemoryAccess.eps\");\n";

print PIPE "h2.Draw();\n";
print PIPE "double x9[] = {  -2, -2 };\n";
print PIPE "double xn=h2.GetMaximum();\n";
print PIPE "double y9[] = { 0, 1.1 * xn };\n";
print PIPE "TGraph *gr9 = new TGraph(2,x9,y9);\n";
print PIPE "gr9->SetLineColor(2);\n";
print PIPE "gr9->SetLineWidth(1001);\n";
print PIPE "gr9->SetFillStyle(3005);\n";
print PIPE "gr9->SetFillColor(2);\n";
print PIPE "gr9->Draw();\n";
print PIPE "h2.Draw(\"same\");\n";

print PIPE "TLegend leg(0.3,0.9,0.6,0.75);\n";
print PIPE "leg.AddEntry(gr9,\"Long Lifetime\",\"f\");\n";
print PIPE "leg.Draw();\n";

print PIPE "c1.Print(\"Lifetime.jpg\");\n";
print PIPE "c1.Print(\"Lifetime.eps\");\n";

my $memory_max=0;
for my $index ( 0 .. scalar(@memorylines)-1 )
{
    my ($m1,$m2,$m3,$m4) = ( $memorylines[$index] =~ /(\S+)\s(\S+)\s(\S+)\s(\S+)/ );
    my $in = $index+1;
    if ( $m1 =~ m/g$/i )
    {
	$m1 =~ s/g$//g;
	$m1 *= 1024*1024;
    }
    if ( $m1 =~ m/m$/i )
    {
	$m1 =~ s/m$//g;
	$m1 *= 1024;
    }
    if ( $m2 =~ m/g$/i )
    {
	$m2 =~ s/g$//g;
	$m2 *= 1024*1024;
    }
    if ( $m2 =~ m/m$/i )
    {
	$m2 =~ s/m$//g;
	$m2 *= 1024;
    }
    my ($m4m,$m4s) = ( $m4 =~ /(\d+)\:([\d\.]+)/ );
    $m4 = 60 * $m4m + $m4s;
    print PIPE "m1.SetBinContent($in,$m1);\n";
    print PIPE "m2.SetBinContent($in,$m2);\n";
    print PIPE "m3.SetBinContent($in,$m3);\n";
    print PIPE "m4.SetBinContent($in,$m4);\n";
    $memory_max = $m4  if ( $memory_max < $m4 );
}
print PIPE "m1.SetStats(0);\n";
print PIPE "m1.SetLineColor(kRed);\n";
print PIPE "m1.SetTitle(\"Memory/CPU usage: #color[1]{RSS} #color[2]{VMem} #color[3]{%CPU} #color[4]{Time}\");\n";

print PIPE "can->cd();\n";
print PIPE "TPad *pad1 = new TPad(\"pad1\",\"\",0,0,1,1);\n";
print PIPE "TPad *pad2 = new TPad(\"pad2\",\"\",0,0,1,1);\n";
print PIPE "TPad *pad3 = new TPad(\"pad3\",\"\",0,0,1,1);\n";
print PIPE "pad3->SetFillStyle(4000);\n";
print PIPE "pad2->SetFillStyle(4000);\n";
print PIPE "pad1->Draw();\n";
print PIPE "pad1->cd();\n";
print PIPE "m1.Draw();\n";
print PIPE "m2.Draw(\"same\");\n";
print PIPE "pad1->Update();\n";

print PIPE "Double_t ymin = 0;\n";
print PIPE "Double_t ymax = 120;\n";
print PIPE "Double_t dy = (ymax-ymin)/0.8;\n";
print PIPE "Double_t xmin = 0;\n";
print PIPE "Double_t xmax = " . scalar(@memorylines) . ";\n";
print PIPE "Double_t dx = (xmax-xmin)/0.8;\n";
print PIPE "pad2->Range(xmin-0.1*dx,ymin-0.1*dy,xmax+0.1*dx,ymax+0.1*dy);\n";
print PIPE "pad2->Draw();\n";
print PIPE "pad2->cd();\n";
print PIPE "m3.SetStats(0);\n";
print PIPE "m3.SetLineColor(kGreen);\n";
print PIPE "m3.Draw(\"same\");\n";
print PIPE "pad2->Update();\n";
print PIPE "TGaxis *axis2 = new TGaxis(xmax,ymin,xmax,ymax,ymin,ymax,50510,\"+L\");\n";
print PIPE "axis2->SetLabelColor(kGreen);\n";
print PIPE "axis2->Draw();\n";

print PIPE "ymax = " . 1.1 * $memory_max . ";\n";
print PIPE "dy = (ymax-ymin)/0.8;\n";
print PIPE "pad3->Range(xmin-0.1*dx,ymin-0.1*dy,xmax+0.1*dx,ymax+0.1*dy);\n";
print PIPE "pad3->Draw();\n";
print PIPE "pad3->cd();\n";
print PIPE "m4.SetStats(0);\n";
print PIPE "m4.SetLineColor(kBlue);\n";
print PIPE "m4.Draw(\"same\");\n";
print PIPE "pad3->Update();\n";
print PIPE "TGaxis *axis3 = new TGaxis(xmax+0.05*dx,ymin,xmax+0.05*dx,ymax,ymin,ymax,50510,\"+L\");\n";
print PIPE "axis3->SetLabelColor(kBlue);\n";
print PIPE "axis3->Draw();\n";

print PIPE "c1.Print(\"Memory.jpg\");\n";
print PIPE "c1.Print(\"Memory.eps\");\n";

print PIPE ".q\n";
close PIPE;

my @args1 = ("convert", "-scale", "50%", "MemoryAccess.jpg", "MemoryAccess_sm.jpg");
system(@args1) == 0 or die "system @args1 failed: $?";
my @args2 = ("convert", "-scale", "50%", "Lifetime.jpg", "Lifetime_sm.jpg");
system(@args2) == 0 or die "system @args2 failed: $?";
my @args3 = ("convert", "-scale", "50%", "Memory.jpg", "Memory_sm.jpg");
system(@args3) == 0 or die "system @args3 failed: $?";

print "done...\n";

print "creating html files\n";

my $name="";
mkdir "html" || die "Cannot make directory new !\n";
$name="html/no_access.html";     open(F1, '> '.$name);   prepare_html(*F1, "NoAccess", $name);
$name="html/no_read.html";       open(F2, '> '.$name);   prepare_html(*F2, "NoRead", $name);
$name="html/no_write.html";      open(F3, '> '.$name);   prepare_html(*F3, "NoWrite", $name);
$name="html/no_free.html";       open(F4, '> '.$name);   prepare_html(*F4, "NoFree", $name);
$name="html/low_read.html";      open(F5, '> '.$name);   prepare_html(*F5, "LowRead", $name);
$name="html/low_write.html";     open(F6, '> '.$name);   prepare_html(*F6, "LowWrite", $name);
$name="html/more_write.html";    open(F7, '> '.$name);   prepare_html(*F7, "MoreWriteThanRead", $name);
$name="html/long_lifetime.html"; open(F8, '> '.$name);   prepare_html(*F8, "LongLifetime", $name);
$name="html/info.html";          open(F9, '> '.$name);   prepare_html(*F9, "Info", $name);
$name="html/rest.html";          open(FR, '> '.$name);   prepare_html(*FR, "Rest", $name);

open(F1p, '> NoAccess.html');
open(F2p, '> NoRead.html');
open(F3p, '> NoWrite.html');
open(F4p, '> NoFree.html');
open(F5p, '> LowRead.html');
open(F6p, '> LowWrite.html');
open(F7p, '> MoreWrite.html');
open(F8p, '> LongLifetime.html');
open(F9p, '> Info.html');
open(FRp, '> Rest.html');

prepare_html2(*F1p, "NoAccess");
prepare_html2(*F2p, "NoRead");
prepare_html2(*F3p, "NoWrite");
prepare_html2(*F4p, "NoFree");
prepare_html2(*F5p, "LowRead");
prepare_html2(*F6p, "LowWrite");
prepare_html2(*F7p, "MoreWriteThanRead");
prepare_html2(*F8p, "LongLifetime");
prepare_html2(*F9p, "Info");
prepare_html2(*FRp, "Rest");

my $c1=0;
my $c2=0;
my $c3=0;
my $c4=0;
my $c5=0;
my $c6=0;
my $c7=0;
my $c8=0;
my $c9=0;
my $cr=0;
my $cinfo;

# report all memory allocations from report
for my $index ( 0 .. $#{ $Summary{'Reports'} } )
{
    # never read from or written to
    if ( $Summary{'Reports'}[$index]{'AccTotalR'} == 0 and
	 $Summary{'Reports'}[$index]{'AccTotalW'} == 0 and
	 $Summary{'Reports'}[$index]{'Reported'} == 0 )
    {
	print_html(*F1, *F1p, $Summary{'Reports'}[$index]{'String'},$pid);
	$Summary{'Reports'}[$index]{'Reported'}=1;
	$c1++;
    }
    # written, but never read from
    if ( $Summary{'Reports'}[$index]{'AccTotalR'} == 0 and
	 $Summary{'Reports'}[$index]{'AccTotalW'} > 0 and
	 $Summary{'Reports'}[$index]{'Reported'} == 0 )
    {
	print_html(*F2, *F2p, $Summary{'Reports'}[$index]{'String'},$pid);
	$Summary{'Reports'}[$index]{'Reported'}=1;
	$c2++;
    }
    # read, but never written to
    if ( $Summary{'Reports'}[$index]{'AccTotalR'} > 0 and
	 $Summary{'Reports'}[$index]{'AccTotalW'} == 0 and
	 $Summary{'Reports'}[$index]{'Reported'} == 0 )
    {
	print_html(*F3, *F3p, $Summary{'Reports'}[$index]{'String'},$pid);
	$Summary{'Reports'}[$index]{'Reported'}=1;
	$c3++;
    }
    # never freed
    if ( $Summary{'Reports'}[$index]{'Dealloc'} eq "never" and
	 $Summary{'Reports'}[$index]{'Reported'} == 0 )
    {
	print_html(*F4, *F4p, $Summary{'Reports'}[$index]{'String'},$pid);
	$Summary{'Reports'}[$index]{'Reported'}=1;
	$c4++;
    }
    # low read
    if ( $Summary{'Reports'}[$index]{'AccRatioR'} < 0.1 and
	 $Summary{'Reports'}[$index]{'Reported'} == 0 )
    {
	print_html(*F5, *F5p, $Summary{'Reports'}[$index]{'String'},$pid);
	$Summary{'Reports'}[$index]{'Reported'}=1;
	$c5++;
    }
    # low write
    if ( $Summary{'Reports'}[$index]{'AccRatioW'} < 0.1 and
	 $Summary{'Reports'}[$index]{'Reported'} == 0 )
    {
	print_html(*F6, *F6p, $Summary{'Reports'}[$index]{'String'},$pid);
	$Summary{'Reports'}[$index]{'Reported'}=1;
	$c6++;
    }
    # more write than read
    if ( $Summary{'Reports'}[$index]{'AccTotalR'} <
	 $Summary{'Reports'}[$index]{'AccTotalW'} and
	 $Summary{'Reports'}[$index]{'Reported'} == 0 )
    {
	print_html(*F7, *F7p, $Summary{'Reports'}[$index]{'String'},$pid);
	$Summary{'Reports'}[$index]{'Reported'}=1;
	$c7++;
    }
    # long lifetime
    if ( $Summary{'Reports'}[$index]{'DeallocAge'} / $total_instructions > 0.01 and
	 $Summary{'Reports'}[$index]{'Reported'} == 0 )
    {
	print_html(*F8, *F8p, $Summary{'Reports'}[$index]{'String'},$pid);
	$Summary{'Reports'}[$index]{'Reported'}=1;
	$c8++;
    }
    # info
    if ( $Summary{'Reports'}[$index]{'Reported'} < 0 )
    {
	print_html(*F9, *F9p, $Summary{'Reports'}[$index]{'String'},$pid);
	$Summary{'Reports'}[$index]{'Reported'}=1;
	$c9++;
	$cinfo = $Summary{'Reports'}[$index]{'String'};
    }
    # rest
    if ( $Summary{'Reports'}[$index]{'Reported'} == 0 )
    {
	print_html(*FR, *FRp, $Summary{'Reports'}[$index]{'String'},$pid);
	$Summary{'Reports'}[$index]{'Reported'}=1;
	$cr++;
    }
}

open(FRp, '> Report.html');
prepare_html_report(*FRp, $c1, $c2, $c3, $c4, $c5, $c6, $c7, $c8, $c9, $cr, $cinfo);
open(FIp, '> JobInfo.html');
prepare_html_jobinfo(*FIp, $cinfo);

finish_html(*F1);
finish_html(*F2);
finish_html(*F3);
finish_html(*F4);
finish_html(*F5);
finish_html(*F6);
finish_html(*F7);
finish_html(*F8);
finish_html(*F9);
finish_html(*FR);

print "done...\n";
print "bye...\n";
