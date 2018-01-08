## To execute this program, you need to do :
## pyroot.py -c "MonNAME='MyFile.root'" egammaRTTInit.py if you want to change the Monitored File
## or
## pyroot.py -c "MonNAME='MyFile.root';expert=True" egammaRTTInit.py if you want to change the Monitored File and Expert Mode = True
## or
## pyroot.py egammaRTTInit.py if you want to use the default files

import user
import ROOT
import AthenaROOTAccess.transientTree

print "args:",sys.argv
MonNAME = globals().get('MonNAME', 'egamma.root')
ele = globals().get('ele',True)
pho = globals().get('pho',True)
expert = globals().get('expert',False)

#if isinstance(FNAME, basestring):
#   FNAME=[FNAME]
#   pass
# print "FNAME=",FNAME
g = MonNAME
print "monitored file",g
g = ROOT.TFile.Open(g)

#FNAME = ROOT.TFile.Open(FNAME)

doAll = True
#doAll = False
if doAll:
    import egammaValidation.egammaRTT_create #egammaValidation.egammaRTT_create
    egammaValidation.egammaRTT_create.execute(g,ele,pho,expert) #egammaValidation.
    #exit()
    pass
else :
    print ""
    print "You can do now :"
    print "import egammaRTT_create"
    print "egammaRTT_create.execute(g,options)"
    print "3 possibities for the options :"
    print "    - you want default choices, "
    print "      ie electrons = True, photons = True, expert = True , you write : "
    print "      egammaRTT_create.execute(g)"
    print "    - in the initialisation you had already entered choices, you write : "
    print "      egammaRTT_create.execute(g,ele,pho,expert)"
    print "    - you haven't entered choices yet, you can do by : "
    print "      egammaRTT_create.execute(g,ele=T/F,pho=T/F,expert=T/F), ex : (g,True,False,False)"
    print ""
    pass

