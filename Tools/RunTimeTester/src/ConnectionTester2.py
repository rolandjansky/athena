# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

if __name__ == '__main__':
    from TimedDataHist  import TimeHist
    from shellCommand   import shellCommand
    from exc2string     import exc2string
    import TestSuite2
    import time, sys, os

    hist = TimeHist()
    home = os.getcwd()
    longTest = True
    #
    # cmd = []
    # cmd.append(
    #    "source /afs/cern.ch/sw/contrib/CMT/v1r18p20050501/mgr/setup.sh;cmt config'>&/dev/null"
    #    )
    # cmd.append('cmt -quiet show macro_names _TestConfiguration')

    print TestSuite2.init()
    
    while (True):
        start = time.time()
        packages = []
        try:
            usr = TestSuite2.makeUserStuffRetriever()
            print 'paths', usr.paths
            print 'substring', usr.substring
            print 'accept', usr.accept
            print 'veto', usr.veto
            if longTest:
                files = usr.userStuff()
            else:
                usr.findMacros()
                # usr.extractPackages()
                # print 'UserStuffRetriever found',files
                delta = time.time()-start
        except Exception, e:
            print exc2string(sys.exc_info())
            delta = -1.
            files = []
           

        print 'packages:'
        for p in files: print p.__dict__,'\n\n'
    
        hist.newPoint(delta)

    
        fn = os.path.join(home,'ConnectionTimeData.txt')
        # print 'plots to ',fn
        try:
            of = open(fn, 'w')
            of.write(hist.valuesPlot())
            of.write(hist.timesPlot())
            of.close
        except Exception, e:
            print exc2string(sys.exc_info())
        
        time.sleep(30)
        print hist.valuesPlot()
        print hist.timesPlot()



