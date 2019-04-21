#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Test for Py3 incompatible print statements

print "Hello"  # Fail
print("Hello")
print("a","b") # Fail
print(1)
print("%s %s" % ("a","b"))
print  # Fail
