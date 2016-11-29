#!/bin/bash

set -e
set -u

test -f $ROOTCOREBIN/data/EventLoop/test_ntuple0.root
test -f $ROOTCOREBIN/user_scripts/EventLoop/batch_worker.C
