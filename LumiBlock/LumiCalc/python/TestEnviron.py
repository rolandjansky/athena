#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import os

print ('Content-Type: text/html')
print () # Blank line, end of headers
print ('<html>')
for key in os.environ:
    print('<p><b>',key,':</b>',os.environ[key],'<p>')
print ('</html>')
