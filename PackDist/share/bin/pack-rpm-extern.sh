
	if [ ${pltfm} = bin ]; then
	    $PACKRPM/bin/pack-rpm.py "$name $package $version $platform" "$tmpreq" "" "" $(_getver $name) "${src[*]}" "${dst[*]}"
	elif [ ${pltfm} = debuginfo ]; then
	    $PACKRPM/bin/pack-rpm.py "$name $package $version ${platform_debuginfo:-${CMTCONFIG}_debuginfo}" "$tmpreq" "" "" $(_getver $name) "${src[*]}" "${dst[*]}"
	fi
