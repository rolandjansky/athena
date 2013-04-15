#!/usr/bin/env python

cmdExample = "PrintAODSelect.py inputAODFile.pool.root"


def interpret(sysArgv):
    # Simple sanity check
    if len(sysArgv) != 2:
        print "Syntax error. Please use: %s" % cmdExample
        sys.exit(-1)
        pass

    # Removing the 'PrintAODSelect.py' in the argument list
    sysArgv.pop(0)

    # Should only be the input AOD/ESD file
    inputFile = sysArgv.pop(0)

    # Sanity checks
    if len(sysArgv) > 0:
        print "Syntax error. Please use: %s" % cmdExample
        sys.exit(-1)
        pass
    if inputFile.__contains__('='):
        print "Syntax error. Please use: %s" % cmdExample
        sys.exit(-1)
        pass

    # Build final command
    preCmd = '-l FATAL -c "'
    if inputFile:
        preCmd += "inputFiles=\'" + inputFile + "\'"
        pass
    preCmd += '"'

    return preCmd



if __name__ == '__main__':
    import sys,os
    preCommand = interpret(sys.argv)

    # Actually, call athena to run the AODSelect code
    cmd = "athena.py " + preCommand + " AODSelect/PrintAODSelect_topOptions.py" 
    print "Going to run: %s" % cmd
    print "   This may take a while (a minute or two)..."
    sys.exit(os.system(cmd))
    pass
