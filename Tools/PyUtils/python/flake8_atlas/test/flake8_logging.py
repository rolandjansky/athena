#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Test for logging related plugins

import logging
logging.basicConfig(level=logging.INFO)
log = logging.getLogger()
myfunnyloggername = log

log.info('This is %s logging practice' % 'bad')
log.info('This is %s logging practice', 'good')
myfunnyloggername.warning('Hello %s' % 'world')

log.info('This is %s logging practice: %d' % ('bad',42))
log.info('This is %s logging practice: %d' % ('bad',42)) # noqa
log.info('This is %s logging practice: %d', 'good', 42)

print("Hello world")
print("Hello world") # noqa
print "Hello world"

# Those should not trigger print warnings:
def myprint(): pass
def printSummary(): pass
def my_print_function(): pass
