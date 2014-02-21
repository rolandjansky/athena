#!/usr/bin/perl -w

$opt_debug = 0;

die "Not enough arguments\n" if $#ARGV != 1;

my $inputFull = $ARGV[0];
my $inputFast = $ARGV[1];

my %timesFull = parseLogFile($inputFull);
my %timesFast = parseLogFile($inputFast);

# print summary
for my $label (sort keys %timesFull) {
    print "Label = :$label: @{$timesFull{$label}}\n" if $opt_debug;

}

my @myLabels = ('Px', 'Tk', 'Service', 'Cry',
    'Sol', 'EMB', 'EMB0', 'EMEC', 'EMEC0', 'LAr', 'FC1', 'FC23', 'FC0',
    'HEC', 'HCB', 'Mu', 'Gas', 'Other', 'Dead');

print "Times per events (" .scalar(@{$timesFull{'Event'}})."/".scalar(@{$timesFast{'Event'}}).")\n";
for my $label (@myLabels) {
    my @a = (@{$timesFull{$label}});
    my @b = (@{$timesFast{$label}});
    printf "%10s: %8.3f %8.3f\n", $label, $a[$#a]/$#a, $b[$#b]/$#b;
}
for my $label ('Event', 'Run') {
    my @a = (@{$timesFull{$label}});
    my @b = (@{$timesFast{$label}});
    printf "%10s: %8.2f %8.2f\n\n", $label, $a[$#a], $b[$#b];
}

print "Fraction of time per events\n";
my @eventA = @{$timesFull{'Run'}}; my $eventA = $eventA[$#eventA];
my @eventB = @{$timesFast{'Run'}}; my $eventB = $eventB[$#eventB];
for my $label (@myLabels) {
    my @a = (@{$timesFull{$label}});
    my @b = (@{$timesFast{$label}});
    printf "%10s: %8.2f %8.2f\n", $label, $a[$#a]/$eventA*100, $b[$#b]/$eventB*100;
}

exit 0;

sub parseLogFile
{
    my $input = $_[0];

    if ($input =~ /\.gz$/) {
	open (INPUT, "gzip -dc $input |") or die "Can't unzip file $input: $!\n";
    } else {
	open (INPUT, "$input") or die "Can't open file $input: $!\n";
    }

    my $found = 0;
    my @labels;
    my %times = ();

    while (<INPUT>) {

	next unless /TestActionTimer::/;

	if (/TestActionTimer::Constructor: Labels (.*)$/) {
	    ++$found;
	    for my $label (split(' ', $1)) {
		# print "Debug: label $label\n" unless $opt_debug;
		push @labels, $label;
	    }
	    print "Debug: labels @labels :" . $labels[0] . ":\n" if $opt_debug;
	}
	
	if (/TestActionTimer::EndOfEventAction: Timer (.*)$/) {
	    my $i = 0;
	    for my $time (split(/ /, $1)) {
		# print "Debug: label $labels[$i] time $time\n" if $opt_debug;
		push @{$times{$labels[$i]}}, $time;
		++$i;
	    }
	}
    }
    
    close(INPUT);

    if ($found == 0) {
	die "Couldn't find Timer information in file $input\n";
    }

    return %times;
}
