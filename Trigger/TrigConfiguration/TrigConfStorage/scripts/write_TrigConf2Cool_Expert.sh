#!/bin/zsh

prog=$0

testname=help
if [[ $# -gt 0 ]]; then
    testname=$1
fi

help() {
    echo "Usage:"
    echo " write_TrigConfig_2_Cool.sh <option>\n"
    echo "<option> can be"
    echo "  -h|--help                ..... this help"
    echo "  -x|--examples            ..... show examples"
    echo "  -c|--createdb            ..... (re)create cool db"
    echo "  -w|--what <what>         ..... what=[bginfo|l1menu|hltmenu|l1ps|hltps]"
    echo "  -r|--run <runNr>         ..... run number"
    echo "  --lb <lb>                ..... LB number"
    echo "  -s|--smk <smk>           ..... SMK "
    echo "  --l1psk <runNr>          ..... L1 PSK"
    echo "  -b|--bgk <bgk>           ..... BG key"
    exit 0
}

examples() {
echo "Examples:

BG info:
./$prog -w bginfo -r 1000 -b 474

L1 menu:
./$prog -w l1menu -r 1000 -s 875  

L1 prescales (overwrites all L1 prescales up to the end of the run):
./$prog -w l1ps -r 1000 --lb 10 --l1psk 1960

HLT menu (overwrites all HLT prescales for this run):
./$prog -w hltmenu -r 1000 -s 875 --hltpsk 1798

HLT prescales (overwrites all HLT precales up to the end of the run):
./$prog -w hltps -r 1000 --lb 10 -s 875 --hltpsk 1798
"
exit 0
}

sqlitefile=trigconfdb.db
what=""
runnr=0
lbnr=0
smk=0
l1psk=0
hltpsk=0
bgk=0
cooldb="sqlite://;schema=$sqlitefile;dbname=CONDBR2"
triggerdb="TRIGGERDB"

while [ $# -gt 0 ]
do
    case $1 in
        -h|--help) help;;
        -x|--examples) examples;;
        -s|--smk) shift; smk=$1;;
        -w|--what) shift; what=$1;;
        -b|--lbk) shift; bgk=$1;;
        --l1psk) shift; l1psk=$1;;
        --hltpsk) shift; hltpsk=$1;;
        --lb) shift; lbnr=$1;;
        -r|--run) shift; runnr=$1;;
        -c|--create) create=1;;
        --) shift; break;;
        -*) help;;
    esac
    shift
done

case $what in
    bginfo|l1menu|hltmenu|l1ps|hltps) ;;
    *)
    echo "--what is not specified"
    help
esac


if [ -e $sqlitefile -a "$create" -eq 1 ];
then
rm $sqlitefile
TrigConf2COOLApp -e create --cooldb $cooldb
fi

case $what in
    l1menu)
    if [ "$smk" -ne 0 -a "$runnr" -ne 0 ]; then
        TrigConf2COOLApp -e write --cooldb $cooldb \
            --trigdb $triggerdb --configkey $smk --run $runnr --prescalekeylvl1 0 --prescalekeyhlt 0
    fi
    ;;
    l1ps)
    if [ "$l1psk" -ne 0 -a "$runnr" -ne 0 -a "$lbnr" -ne 0 ]; then
        TrigConf2COOLApp -e write --cooldb $cooldb \
            --trigdb $triggerdb --configkey 0 --run $runnr --lumiblock $lbnr --prescalekeylvl1 $l1psk --prescalekeyhlt 0
    fi
    ;;
    hltmenu)
    if [ "$smk" -ne 0 -a "$runnr" -ne 0 -a "$hltpsk" -ne 0 ]; then
        TrigConf2COOLApp -e writehlt --cooldb $cooldb \
            --trigdb $triggerdb --configkey $smk --run $runnr --prescalekeyhlt $hltpsk
    fi
    ;;
    hltps)
    if [ "$smk" -ne 0 -a "$hltpsk" -ne 0 -a "$runnr" -ne 0 -a "$lbnr" -ne 0 ]; then
        TrigConf2COOLApp -e writehlt --cooldb $cooldb \
            --trigdb $triggerdb --configkey $smk --run $runnr  --lumiblock $lbnr --prescalekeyhlt $hltpsk
    fi
    ;;
    bginfo)
    if [ "$smk" -ne 0 -a "$bgk" -ne 0 -a "$runnr" -ne 0 ]; then
        echo TrigConf2COOLApp -e writel1 --cooldb \'$cooldb\' \
            --trigdb $triggerdb --configkey $smk  --run $runnr  --bgkey $bgk
    else
        echo "SMK or run nr or bg key is missing"
    fi
    ;;
esac
echo AtlCoolConsole.py \'$cooldb\'
