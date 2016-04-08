#!/usr/local/bin/perl


for (@ARGV) {
    @modtime = localtime((stat($_))[9]);
    @curtime = localtime(time());
    $mtim=$modtime[7]+1;
    $mtimh=int ($modtime[2]/24*10);
    $ctim=$curtime[7]+1;
    $ctimh=int ($curtime[2]/24*10);
    $diftim=$ctim-$mtim;
    if($diftim < 0 ) {$diftim=$diftim + 366}
    $diftimprec=$diftim*10+$ctimh-$mtimh;
    print "$diftimprec";
}
exit;













