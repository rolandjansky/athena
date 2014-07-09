#!/bin/zsh

testname=help
if [[ $# -gt 0 ]]; then
    testname=$1
fi

case $testname in
help)
    echo "Usage:"
    echo "  $0 <option>\n"
    echo "<option> can be"
    echo "  dbxml        ..... xml jo from db"
    ;;
dbxml)

    TrigConf2XMLApp --trigdb TRIGGERDB --configkey 634 --prescalekeyhlt 818 --prescalekeylvl1 1 \
    >&! db2xmljo.log
    ;;
esac

