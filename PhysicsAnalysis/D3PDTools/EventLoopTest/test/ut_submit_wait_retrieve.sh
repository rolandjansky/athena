#!/bin/bash

set -e
set -u

rc root -l -b -q $ROOTCOREBIN/user_scripts/EventLoop/submit.C'("submitDir")'
el_wait submitDir
el_retrieve submitDir
