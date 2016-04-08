################################################################################
# Package: PackDist
################################################################################

# Declare the package name:
atlas_subdir( PackDist )

# this line failed automatic conversion in cmt2cmake :
# set_remove PACKOPTS "-w"

# this line failed automatic conversion in cmt2cmake :
# set_append PACKOPTS " -w "

athena_env( SET PACKCFG ../config/pack-nightly-bugfix.cfg
            SET PACKOPTS \\\${PACKOPTS} )

