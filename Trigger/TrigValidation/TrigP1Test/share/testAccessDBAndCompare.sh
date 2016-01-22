
echo $SHELL

#INT8R not in Frontier yet
#source ../UploadMenuKeys/exportMenuKeys.sh

db=TRIGGERDBREPR
smk=261
l1psk=63
hltpsk=172

cmd="TrigConf2XMLApp --trigdb $db --configkey $smk --prescalekeylvl1 $l1psk --prescalekeyhlt $hltpsk --outputfile oracle"
echo $cmd
$cmd >& download_oracle.log


export TRIGGER_USE_FRONTIER=YES
cmd="TrigConf2XMLApp --trigdb $db --configkey $smk --prescalekeylvl1 $l1psk --prescalekeyhlt $hltpsk --outputfile frontier"
echo $cmd
$cmd >& download_frontier.log


echo "comparing L1 menus from oracle and from frontier download"
diff L1Menu_oracle.xml L1Menu_frontier.xml > l1_diff.txt
l1_status=$?

echo "comparing HLT menus from oracle and from frontier download"
diff HLTMenu_oracle.xml HLTMenu_frontier.xml > hlt_diff.txt
hlt_status=$?

if [ $l1_status == 0 ]; then
    echo "L1 menus are identical"
else
    echo "ERROR L1 menus are not identical"
    cat l1_diff.xml
fi

if [ $hlt_status == 0 ]; then
    echo "HLT menus are identical"
else
    echo "ERROR HLT menus are not identical"
    cat hlt_diff.xml
fi

if [ $l1_status != 0 -o $hlt_status != 0 ]; then
    exit 1
fi

exit 0
