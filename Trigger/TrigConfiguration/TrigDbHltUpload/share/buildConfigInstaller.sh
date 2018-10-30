#!/bin/zsh

decomp() {
cat <<EOF
#!/bin/bash
echo ""
echo "Self Extracting Menu TriggerDB Installer"
echo ""

export TMPDIR=\`mktemp -d /tmp/selfextract.XXXXXX\`
unset TRIGGER_EXP_CORAL_PATH 
export TRIGGER_EXP_CORAL_PATH_RUN2=/afs/cern.ch/user/a/attrgcnf/.dbauth/menuexperts

ARCHIVE=\`awk '/^__ARCHIVE_BELOW__/ {print NR + 1; exit 0; }' \$0\`

tail -n+\$ARCHIVE \$0 | tar xz -C \$TMPDIR

TIME=\`date +%F-%H-%M-%S\`

TARGET=./$1-\$TIME

rm -rf \$TARGET
mv \$TMPDIR \$TARGET

rm -rf \$TMPDIR

echo "Extracted configuration into \$TARGET"
echo ""
if [ "x$CONFIGNAME" == "x"  ]; then
echo "Note: the configuration is uploaded with the name taken from \$TARGET/hltMenu.xml."
echo "This behavior can be overwritten via 'export CONFIGNAME=<NewConfigName>' before running the extractor."
echo ""
fi

## Point 1 setup
p1ConfigTT=""
if [ -e /det/tdaq/scripts/setup_trigger_tool_autoupload  ]; then
    ##unset variable with afs path at point_1
    unset TRIGGER_EXP_CORAL_PATH
    source /det/tdaq/scripts/setup_trigger_tool_autoupload
    p1ConfigTT="-onl -p1TT -p1DB"
fi

_JAVA_OPTIONS="-Xms250m -Xmx1400m"
TTJAR=\${TRIGGERTOOL:-/afs/cern.ch/user/a/attrgcnf/TriggerTool/TriggerTool-new.jar}
TTSH=\${TRIGGERTOOL:-/afs/cern.ch/user/a/attrgcnf/TriggerTool/run_TriggerTool.sh}
DB=\${TRIGGERDB:-TRIGGERDBREPR}
NAME=\${CONFIGNAME:-USEXML}

CMD="\$TTSH -db \$DB -up -l1  \$TARGET/lvl1Menu.xml -hlt \$TARGET/hltMenu.xml -l1topo \$TARGET/l1topoMenu.xml -efs \$TARGET/efsetup.xml \$p1ConfigTT -rel $2 -o \$TARGET/$1-\$TIME-UploadTT.log -n \$NAME"
echo \$CMD

echo ""
echo "Would you like to upload the configuration into the database ? [y|N]"
read CHOICE

[[ (\$CHOICE != "y") && (\$CHOICE != "Y") ]] && exit 0

eval \$CMD

exit 0

__ARCHIVE_BELOW__
EOF
}

if [ $# -lt 2 ]; then 
     echo $0 needs two arguments
     echo $0 NAME RELEASE
     exit 1
fi

ARCHIVE=./payload_$1.tar.gz

tar zcf $ARCHIVE -C setup/$1 ./LVL1config.dtd ./lvl1Menu.xml ./hltMenu.xml ./l1topoMenu.xml ./efsetup.xml

SELFEXTRACTFILE=selfextract-$1-`date +%F-%H-%M-%S`.bsx

if [ -e "$ARCHIVE" ]; then
    decomp $1 $2 | cat < $ARCHIVE > $SELFEXTRACTFILE
    rm $ARCHIVE
    chmod +x $SELFEXTRACTFILE
else
    echo "$ARCHIVE does not exist"
    exit 1
fi

echo ""
echo ""
echo "Created self-extracting archive $SELFEXTRACTFILE . Just (copy to P1) execute in any directory."
echo ""
exit 0
