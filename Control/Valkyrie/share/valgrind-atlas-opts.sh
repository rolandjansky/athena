#!/usr/bin/env bash
#
# Set VALGRIND_OPTS to default ATLAS options and suppression files:
#
#   source ${Athena_DIR}/bin/valgrind-atlas-opts.sh
#                     or
#   source `which valgrind-atlas-opts.sh`
#

_vgopts=("--suppressions=${Athena_DIR}/data/Valkyrie/valgrind-python.supp"
         "--suppressions=${Athena_DIR}/data/Valkyrie/valgrind-atlas.supp"
         "--suppressions=${ROOTSYS}/etc/valgrind-root.supp"
         "--suppressions=${ROOTSYS}/etc/valgrind-root-python.supp"
         "--smc-check=all")

# Add the option if not already present
for o in ${_vgopts[@]}; do
    if [[ $VALGRIND_OPTS != *"$o"* ]]; then
        export VALGRIND_OPTS="${VALGRIND_OPTS:+"$VALGRIND_OPTS "}$o"
    fi
done

unset _vgopts
