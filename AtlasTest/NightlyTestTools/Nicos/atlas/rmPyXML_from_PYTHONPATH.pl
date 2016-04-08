#!/usr/local/bin/perl
#
#
#
use Env;

my $PYTHONPATH="$PYTHONPATH";

@el=split(/:/,$PYTHONPATH);
@el_trun=();

for ($m=0; $m <= $#el; $m++){
    if ( $el[$m] !~ /\/PyXML\// ){
    push(@el_trun, $el[$m]);
    }
}
$return_val=join(':', @el_trun);
print $return_val;

                                                                                
