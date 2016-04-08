################################################################################
# Package: JobTransforms
################################################################################

# Declare the package name:
atlas_subdir( JobTransforms )

# External dependencies:
find_package( PythonLibs )

athena_env( SET T_INCPATH \\\${CMTINSTALLAREA}/share/JT/include
            SET T_RELEASE \\\${JobTransforms_project_release} )

