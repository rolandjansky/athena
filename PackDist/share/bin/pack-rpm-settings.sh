
if [ ${pkg[0]} = AtlasSetup ]; then
    _pre ${pkg[2]}/${pkg[0]} ${pkg[0]} ${pkg[1]} >|${tmppre}
else
    _pre ${pkg[2]}/${pkg[0]}/${pkg[1]} ${pkg[0]} ${pkg[1]} >|${tmppre}
fi
    $PACKRPM/bin/pack-rpm.py "$name ${pkg[0]} ${pkg[1]} noarch" "" "" ${tmppre} $(_getver $name)
