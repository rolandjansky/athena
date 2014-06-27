#!/bin/bash
#
# TDAQ_APPLICATION_NAME is the name of your application as it appears in the Run Control tree.
# For simple applications it is identical to the object ID in the OKS configuration.
# For template applications it corresponds to "object_id:parent_segment:host:instance_number".
# If TDAQ_APPLICATION_NAME is undefined the current job is probably just a stand-alone test.
#
# The string matching supports glob patterns (shell wildcards), but not regular expressions.

if test -z "${TDAQ_APPLICATION_NAME}" ; then echo "TDAQ_APPLICATION_NAME is undefined. Not sleeping." ; exit 0 ; fi

DELAY=5   # Default: let PMG finish its work first, then rush for AtlasSetup and everything else.

case "${TDAQ_APPLICATION_NAME}" in
    GM-Application:*:1 )        DELAY=5 ;;
    GM-Application:*:2 )        DELAY=180 ;;
    GM-Application:*:3 )        DELAY=360 ;;
    GM-Application:*:4 )        DELAY=540 ;;
    GM-Application:*:5 )        DELAY=720 ;;
    GM-Application:*:6 )        DELAY=900 ;;
    GM-Application:*:7 )        DELAY=1080 ;;

    TRT-Monitoring-Noise )      DELAY=5 ;;
    TRT-Monitoring-Beam )       DELAY=5 ;;
    TRT-Monitoring-01 )         DELAY=120 ;;
    TRT-Monitoring-02 )         DELAY=120 ;;
    TRT-Monitoring-03 )         DELAY=240 ;;
    TRT-Monitoring-04 )         DELAY=240 ;;
    TRT-Monitoring-05 )         DELAY=360 ;;
    TRT-Monitoring-06 )         DELAY=360 ;;
esac

echo -n "Sleeping for ${DELAY} seconds ..."
sleep "${DELAY}" && echo " Done."
