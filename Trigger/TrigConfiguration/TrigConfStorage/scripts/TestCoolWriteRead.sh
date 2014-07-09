TrigConf2COOLApp -e write --cooldb 'sqlite://;schema=outputHLTconfig.db;dbname=COMP200' --trigdb TRIGGERDB --run 1000 --configkey 1543 --prescalekeyhlt 7770 --prescalekeylvl1 9860

TrigConf2COOLApp -e read --cooldb 'sqlite://;schema=outputHLTconfig.db;dbname=COMP200' --run 1000 --xml readback

echo "Wrote COOL database outputHLTconfig.db (COMP200) from SMK 1543, and read back into L1/HLTMenu_readback.xml"

echo AtlCoolConsole.py \'sqlite://;schema=outputHLTconfig.db;dbname=COMP200\'

