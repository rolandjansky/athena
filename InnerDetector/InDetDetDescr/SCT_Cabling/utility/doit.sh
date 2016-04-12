MYCMD="/afs/cern.ch/user/s/sroe/MyAthena/db/Database/CoolConvUtilities/x86_64-slc6-gcc49-opt/openCoraCool.exe"
rm /tmp/sroe/mycool.db
$MYCMD "0-1230768000000000000"
mv /tmp/sroe/mycool.db /tmp/sroe/mycool1.db

${MYCMD} '1230768000000000000-1262304000000000000'
mv /tmp/sroe/mycool.db /tmp/sroe/mycool2.db

${MYCMD} '1262304000000000000-1288569600000000000'
mv /tmp/sroe/mycool.db /tmp/sroe/mycool3.db

${MYCMD} '1288569600000000000-1290686400000000000'
mv /tmp/sroe/mycool.db /tmp/sroe/mycool4.db

${MYCMD} '1290686400000000000-1293840000000000000'
mv /tmp/sroe/mycool.db /tmp/sroe/mycool5.db



${MYCMD} '1293840000000000000-1333588404000000000'
mv /tmp/sroe/mycool.db /tmp/sroe/mycool6.db

${MYCMD} '1333588404000000000-1333612447000000000'
mv /tmp/sroe/mycool.db /tmp/sroe/mycool7.db

${MYCMD} '1333612447000000000-1356998400000000000'
mv /tmp/sroe/mycool.db /tmp/sroe/mycool8.db

${MYCMD} '1356998400000000000-9223372036854775807'
mv /tmp/sroe/mycool.db /tmp/sroe/mycool9.db
