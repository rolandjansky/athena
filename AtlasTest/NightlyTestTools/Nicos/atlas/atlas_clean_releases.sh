#!/bin/sh
if [ "$NICOS_PROJECT_RELNUMB" != "2" -a "$NICOS_PROJECT_RELNUMB" != "1" ]; then rm -rf ${NICOS_PROJECT_HOME}/atlrel_1/*; fi
if [ "$NICOS_PROJECT_RELNUMB" != "3" -a "$NICOS_PROJECT_RELNUMB" != "2" ]; then rm -rf ${NICOS_PROJECT_HOME}/atlrel_2/*; fi
if [ "$NICOS_PROJECT_RELNUMB" != "4" -a "$NICOS_PROJECT_RELNUMB" != "3" ]; then rm -rf ${NICOS_PROJECT_HOME}/atlrel_3/*; fi
if [ "$NICOS_PROJECT_RELNUMB" != "5" -a "$NICOS_PROJECT_RELNUMB" != "4" ]; then rm -rf ${NICOS_PROJECT_HOME}/atlrel_4/*; fi
if [ "$NICOS_PROJECT_RELNUMB" != "6" -a "$NICOS_PROJECT_RELNUMB" != "5" ]; then rm -rf ${NICOS_PROJECT_HOME}/atlrel_5/*; fi
if [ "$NICOS_PROJECT_RELNUMB" != "0" -a "$NICOS_PROJECT_RELNUMB" != "6" ]; then rm -rf ${NICOS_PROJECT_HOME}/atlrel_6/*; fi
if [ "$NICOS_PROJECT_RELNUMB" != "1" -a "$NICOS_PROJECT_RELNUMB" != "0" ]; then rm -rf ${NICOS_PROJECT_HOME}/atlrel_0/*; fi