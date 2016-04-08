#!/bin/sh

${HOME}/public/kerbcomm
if [ "${NICOS_PROJECT_RELNAME_COPY}" != "" ]; then
${NICOS_HOME}/atlas/run_doxygen_project_nightlies.sh -r ${NICOS_PROJECT_RELNAME_COPY}
else
${NICOS_HOME}/atlas/run_doxygen_project_nightlies.sh -r ${NICOS_PROJECT_RELNAME}
fi