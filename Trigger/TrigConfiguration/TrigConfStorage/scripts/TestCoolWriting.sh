#!/bin/zsh

testname=help
if [[ $# -gt 0 ]]; then
    testname=$1
fi

release=16.1.1
lvl1config=LVL1config_Physics_pp_v1_${release}.xml
hltconfig=HLTconfig_Physics_pp_v1_${release}.xml

case $testname in
help)
    echo "Usage:"
    echo "  $0 <option>\n"
    echo "<option> can be"
    echo "  xmlcoolxml        ..... xml menu from release -> Cool -> xml -> compare xmls"
    echo "  dbcool            ..... db menu -> Cool"
    ;;
xmlcoolxml)
    rm -f trigconf.db
    #rm -f ${lvl1config}
    rm -f ${hltconfig}
    rm -f L1Menu_FromCOOL.xml
    rm -f HLTMenu_FromCOOL.xml
    get_files -xmls -symlink ${lvl1config}
    get_files -xmls -symlink ${hltconfig}
    TrigConf2COOLApp -e createwrite --cooldb 'sqlite://;schema=trigconf.db;dbname=TRIGCONF' \
    --lvl1config ${lvl1config} --hltmenu ${hltconfig} --run '52280-52299' >&! xml2cool.log
    
    TrigConf2COOLApp -e read --cooldb 'sqlite://;schema=trigconf.db;dbname=TRIGCONF' --run 52290 --xml FromCOOL >&! cool2xml.log
    
    CompMenus.py -e cool ${lvl1config}  L1Menu_FromCOOL.xml  
    CompMenus.py -e cool ${hltconfig}   HLTMenu_FromCOOL.xml
    ;;

xmlcool)
    rm -f trigconf.db
    #rm -f ${lvl1config}
    rm -f ${hltconfig}
    get_files -xmls -symlink ${lvl1config}
    get_files -xmls -symlink ${hltconfig}
    TrigConf2COOLApp -e createwrite --cooldb 'sqlite://;schema=trigconf.db;dbname=TRIGCONF' \
    --lvl1config ${lvl1config} --hltmenu ${hltconfig} --run '52280-52299' >&! xml2cool.log
    ;;

coolxml)
    rm -f L1Menu_FromCOOL.xml
    rm -f HLTMenu_FromCOOL.xml

    TrigConf2COOLApp -e read --cooldb 'sqlite://;schema=trigconf.db;dbname=TRIGCONF' --run 52290 --xml FromCOOL >&! cool2xml.log
    
    CompMenus.py -e cool ${lvl1config}  L1Menu_FromCOOL.xml  
    CompMenus.py -e cool ${hltconfig}   HLTMenu_FromCOOL.xml

    ;;


dbcool)
    rm -f trigconfdb.db
    rm -f L1Menu_FromDb.xml
    rm -f HLTMenu_FromDb.xml
    TrigConf2COOLApp -e createwrite --cooldb 'sqlite://;schema=trigconfdb.db;dbname=TRIGCONF' \
    --trigdb TRIGGERDB --configkey 668 --prescalekeylvl1 1003 --prescalekeyhlt 923 --run '52280-52299' >&! db2cool.log

    print "done! Log file in db2cool.log"
    print "check cool file with"
    print "AtlCoolConsole.py  'sqlite://;schema=trigconfdb.db;dbname=TRIGCONF'"
    ;;

esac

# AtlCoolConsole.py  'sqlite://;schema=trigconf.db;dbname=TRIGCONF'
