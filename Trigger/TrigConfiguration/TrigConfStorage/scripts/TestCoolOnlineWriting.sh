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
    echo "  dbcool        ..... TriggerDB(ATLR) -> Cool"
    ;;

dbcool)
    rm -f trigconfonltest.db
    echo "Creating trigconfonltest.db"
    TrigConf2COOLApp -e create --cooldb 'sqlite://;schema=trigconfonltest.db;dbname=TRIGCONF' >&! cooldbcreation.log
    echo "Filling trigconfonltest.db, log: db2coolonltest.log"

    #valgrind --trace-children=yes --num-callers=8 --show-reachable=yes \
    TrigConf2COOLOnlineTestApp -e write --cooldb 'sqlite://;schema=trigconfonltest.db;dbname=TRIGCONF' \
    --run 52290 --trigdb TRIGGERDB --configkey 634 --prescalekeyhlt 818 820 821 --prescalekeylvl1 1 \
    >&! db2coolonltest.log
    ;;

esac

# AtlCoolConsole.py  'sqlite://;schema=trigconfonltest.db;dbname=TRIGCONF'
