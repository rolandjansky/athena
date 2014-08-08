
    $PACKRPM/bin/pack-rpm.py "$name $package $version $platform" "$tmpreq" "" "" $(_getver $name) "${src[*]}" "${dst[*]}"
