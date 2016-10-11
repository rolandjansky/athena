#################################################################
#	preInclude.GMSB.py - Chiara Debenedetti, 3 Jun 2011	#
#################################################################

def get_and_fix_PDGTABLE(replace):
    import os, shutil

    # Download generic PDGTABLE (overwrite existing one if it exists)
    os.system('get_files -remove -data PDGTABLE.MeV')
    shutil.move('PDGTABLE.MeV', 'PDGTABLE.MeV.org')

    # an example line to illustrate the fixed format, see PDGTABLE.MeV for more details
    # M 1000022                          0.E+00         +0.0E+00 -0.0E+00 ~chi(0,1)     0

    update = open('PDGTABLE.MeV', 'w')
    for l in open('PDGTABLE.MeV.org'):

        for r in replace:
            if l.find(r[1]) > -1:
                ll = l.split()

                if ll[0] == r[0] and ll[1] == r[1]:
                    l = l[0:35] + ('%11.5E' % r[2]).strip().ljust(14) + l[49:]
                    continue

        update.write(l)
    update.close()

def load_files_for_GMSB_scenario(simdict):

    GMSBIndex = int(simdict["GMSBIndex"])

    if GMSBIndex == 1:
        get_and_fix_PDGTABLE([('M', '1000022', eval(simdict["GMSBNeutralino"])), ('M', '1000039', eval(simdict.get("GMSBGravitino",'0')))])

    elif GMSBIndex == 2:
        m_stau    = eval(simdict["GMSBStau"])
        m_slepton = eval(simdict["GMSBSlepton"])
        get_and_fix_PDGTABLE([('M', '1000015', m_stau), ('M', '2000011', m_slepton), ('M', '2000013', m_slepton)])

    elif GMSBIndex == 3:
        m_stau    = eval(simdict["GMSBStau"])
        m_slepton = eval(simdict["GMSBSlepton"])
        m_squark    = eval(simdict["SQUARKMASS"])
        m_neutralino    = eval(simdict["NEUTRALINOMASS"])
        m_gluino    = eval(simdict["GLUINOMASS"])
        get_and_fix_PDGTABLE([('M', '1000001', m_squark), ('M', '2000001', m_squark), ('M', '1000002', m_squark), \
                              ('M', '2000002', m_squark), ('M', '1000022', m_neutralino), ('M', '1000021', m_gluino), \
                              ('M', '1000003', 1.00E+04 ), ('M', '2000003', 1.00E+04 ), ('M', '1000004', 1.00E+04 ), \
                              ('M', '2000004', 1.00E+04 ), ('M', '1000005', 1.00E+04 ), ('M', '2000005', 1.00E+04 ), \
                              ('M', '1000006', 1.00E+04 ), ('M', '2000006', 1.00E+04 ), ('M', '1000011', 2.50E+02 ), \
                              ('M', '1000012', 1.00E+04 ), ('M', '1000013', 2.50E+02 ), ('M', '1000014', 1.00E+04 ), \
                              ('M', '1000015', m_stau ), ('M', '1000016', 1.00E+04 ), ('M', '2000011', m_slepton ), \
                              ('M', '2000013', m_slepton ), ('M', '2000015', 2.50E+02 ), ('M', '1000023', 1.00E+04 ), \
                              ('M', '1000024', 1.00E+04 ), ('M', '1000025', -1.0E+04 ), ('M', '1000035', 1.00E+04 ), \
                              ('M', '1000037', 1.00E+04 ) ])

    elif GMSBIndex == 4:
        get_and_fix_PDGTABLE([('M', '1000015', eval(simdict["GMSBStau"]))])

    else:
        print 'GMSBIndex %i not supported' % GMSBIndex
        raise

doG4SimConfig = True
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
import PyUtils.AthFile as af
try:
    f = af.fopen(athenaCommonFlags.FilesInput()[0])

    if "StreamHITS" in f.infos["stream_names"]:
        from Digitization.DigitizationFlags import digitizationFlags
        simdict = digitizationFlags.specialConfiguration.get_Value()
        doG4SimConfig = False
    else:
        from G4AtlasApps.SimFlags import simFlags
        simdict = simFlags.specialConfiguration.get_Value()
except:
    from G4AtlasApps.SimFlags import simFlags
    simdict = simFlags.specialConfiguration.get_Value()

assert "GMSBIndex" in simdict
#if 2525 == simdict["GMSBIndex"]:

load_files_for_GMSB_scenario(simdict)

if doG4SimConfig:
    from G4AtlasApps.SimFlags import simFlags
    def gmsb_processlist():
        from G4AtlasApps import AtlasG4Eng
        AtlasG4Eng.G4Eng.gbl.G4Commands().process.list()

    simFlags.InitFunctions.add_function("postInit", gmsb_processlist)

    def gmsb_setparams():
        from G4AtlasApps import AtlasG4Eng
        from GaudiKernel.SystemOfUnits import GeV, ns

        ## Assuming that GMSBIndex is an int here...
        GMSBIndex = int(AtlasG4Eng.G4Eng.Dict_SpecialConfiguration["GMSBIndex"])

        if GMSBIndex == 1: # generic neutralino to photon scenario
            from G4AtlasApps.SimFlags import simFlags
            simFlags.PhysicsOptions += ["GauginosPhysicsTool"]

        elif GMSBIndex == 2 or GMSBIndex == 3 or GMSBIndex == 4: # generic stau scenario
            from G4AtlasApps.SimFlags import simFlags
            simFlags.PhysicsOptions += ["SleptonsPhysicsTool"]

        del GMSBIndex


    simFlags.InitFunctions.add_function("preInitPhysics", gmsb_setparams)

    def gmsb_applycalomctruthstrategy():
    ## Applying the MCTruth strategies: add decays in the Calorimeter
        from G4AtlasApps import AtlasG4Eng
        myDecay = AtlasG4Eng.G4Eng.Dict_MCTruthStrg.get('Decay')
        myDecay.add_Volumes('CALO::CALO', 1)

    simFlags.InitFunctions.add_function("postInit", gmsb_applycalomctruthstrategy)

del doG4SimConfig, simdict
