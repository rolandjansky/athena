import sys
import os
from os.path import join
import string
from getopt import getopt, GetoptError

def test():
    lines = []
    lines.append(' library -s/absolute/somewhere -a=option xxxx yyyy\n')
    lines.append('a line that does not interest us\n')
    lines.append(' library -s/absolute/somewhereElse -a=option aaa\n')
    doit(lines)

def notest():
   lines = os.popen('cmt show constituents').readlines()
   doit(lines)

def getSourceDirectory(tokens):
    sDir = ''
    for token in tokens:
        if token.startswith('-s'): sDir = token[2:]
    return sDir
   
def doit(lines):
    files = '' # the final list of files
    for line in lines:
        tokens = string.split(line) # split line into tokens
        tokens = [string.strip(token) for token in tokens] # clean-up whitespace
        if tokens[0] != 'library': continue # a library?
        # get the source dir if any (given by '-s=sDir')
        sDir = getSourceDirectory(tokens)
        # remove option tokens (have form '-o=xxxx'. first token is 'library', second is
        # library name - so start from the third
        tokens = [join(sDir,token) for token in tokens[2:]  if not token.startswith('-')]
        # accumulate the list of files from the different libraries
        files += string.join(tokens)+' '

    print files # output the list by printing it

if __name__ == '__main__':
    try:
        opts,args = getopt(sys.argv[1:], 't')
    except GetoptError:
        print 'unknown option(s)', opt, args

    testMode = False
    for o,a in opts:
        print o
        print a
        if o == '-t': testMode=True

    if testMode:
        print 'testing'
        test()
    else:
        notest()

