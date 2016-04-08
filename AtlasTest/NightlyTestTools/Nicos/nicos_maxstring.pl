#!/usr/local/bin/perl
my $pick="";
my $lengthmax=0;
my $i=0;

for (@ARGV) 
{
if( length > $lengthmax )
{$lengthmax=length; $pick=$_;}
$i=$i+1;
}
print "$pick";
exit;
