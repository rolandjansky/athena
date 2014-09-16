#!/usr/bin/perl -w
# these lines must match trigtest.pl:
$summaryout = "summary.log";
%ERROR = (
          OK => 0,
          SCRIPT_MISC => 1,
          POST_COMMAND1_BAD_EXIT => 2,
          ROOTCOMP_MISMATCH => 4,
          ATHENA_BAD_EXIT => 8,
          ATHENA_NO_LOG => 16,
          ATHENA_ERROR_IN_LOG => 32,
          ATHENA_REGTEST_FAILED => 64,
          ATN_TIME_LIMIT => 128
          );
sub main();
main();
exit;

sub process_error_codes(@) {
    my (@error_strings) = @_;
    my $exitcode=0;
    # | sort | uniq
    my %saw;
    undef %saw;
    @saw{@error_strings} = ();
    my @error_strings_uniq = sort keys %saw;
    print "$prog: debug: all statuscodes: @error_strings\n" if ($debug);
    print "$prog: debug: uniq statuscodes: @error_strings_uniq\n" if ($debug);
    # convert strings, keep numbers as they are, add together
    print "=== These errors occured: ";
    open SUMMARY, ">$summaryout";
    for (@error_strings_uniq){
        my $code;
        if (/^\d+$/){
            # it's already a number
            $code = $_;
        } else {
            # translate string
            $code = $ERROR{$_};
        }
        $exitcode += $code;
        if ($code != 0){ # print all except ok.
            print "$_ ($code) ";
            print SUMMARY "$_ ($code)\n";
        }
    }
    print " exitcode = $exitcode\n";
    print SUMMARY " exitcode = $exitcode\n";
    close SUMMARY;
    return $exitcode;
}
sub main(){
    my @errors = qw(ATHENA_ERROR_IN_LOG);
    process_error_codes(@errors);
}
