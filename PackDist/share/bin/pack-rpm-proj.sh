
	if [ ${pltfm} == bin ]; then
	    cmtver=$(awk '$2 == "CMT" { print $3; exit }' ${tmpreq}) || return 1
	    _pre ${1} ${2} ${3} >|${tmppre} || return 1
	    _post ${1} ${2} ${3} >|${tmppost} || return 1
	    _preun ${1} ${2} ${3} >|${tmppreun} || return 1
	    $PACKRPM/bin/pack-rpm.py "$name ${2} ${3} $pltfm" $tmpreq ${tmppost} ${tmppre} $(_getver $name) "" "" ${tmppreun} || return 1
	else
	    $PACKRPM/bin/pack-rpm.py "$name ${2} ${3} $pltfm" $tmpreq "" "" $(_getver $name) || return 1
	fi
