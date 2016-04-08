#!/usr/local/bin/perl
use Env;
use Cwd;
use File::Basename;
use File::stat;

    if ($#ARGV != 1) {
    print "nicos_path_cleaner: two arg req: variable and removal string \n";
    exit 1;}

    my $path1 = $ARGV[0];
    my $symb = $ARGV[1];
    @outp=();
    @fields = split(":", $path1);

    foreach $pth (@fields){
        ($lin1 = $pth) =~ s/ //g;
        next if (length($lin1) eq 0);
        next if $pth =~ /$symb/;
        push(@outp, $pth);  
        }
   
my $path2=join(':',@outp);
print $path2;  


