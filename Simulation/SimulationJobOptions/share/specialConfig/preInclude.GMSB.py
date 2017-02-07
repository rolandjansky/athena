#################################################################
#	preInclude.GMSB.py - Sascha Mehlhase, 6 Jul 2016            	#
#################################################################

def get_and_fix_PDGTABLE(replace):
    import os, shutil, re

    # Download generic PDGTABLE (overwrite existing one if it exists)
    os.system('get_files -remove -data PDGTABLE.MeV')
    shutil.move('PDGTABLE.MeV', 'PDGTABLE.MeV.org')

    # an example line to illustrate the fixed format, see PDGTABLE.MeV for more details
    # M 1000022                          0.E+00         +0.0E+00 -0.0E+00 ~chi(0,1)     0

    lines = open('PDGTABLE.MeV.org').readlines()
    for pdgid,mass,name,charge in replace:
        if not re.search(r'[MW]\s+'+str(pdgid)+'\s+\S+', ''.join(lines)):
            lines.append('M' + str(pdgid).rjust(8) +''.ljust(26) +
                         ('%11.5E' % mass).ljust(15) +
                         '+0.0E+00'.ljust(9) + '-0.0E+00'.ljust(9) +
                         name.strip() + ''.ljust(6) + charge.strip() + '\n')
            lines.append('W' + str(pdgid).rjust(8) +''.ljust(26) +
                         '0.E+00'.ljust(15) + '+0.0E+00'.ljust(9) + '-0.0E+00'.ljust(9) +
                         name.strip() + ''.ljust(6) + charge.strip() + '\n')
        else:
            for i in xrange(len(lines)):
                if re.search(r'M\s+'+str(pdgid)+'\s+\S+', lines[i]):
                    l = lines[i]
                    lines[i] = l[0:35] + ('%11.5E' % mass).ljust(14) + l[49:]

    update = open('PDGTABLE.MeV', 'w')
    update.write(''.join(lines))
    update.close()
    
    print 'modfied PDGTABLE\n%s\n' % ''.join(lines)

def load_files_for_GMSB_scenario(simdict):

    GMSBIndex = int(simdict["GMSBIndex"])
    
    if GMSBIndex == 1:
        # get_and_fix_PDGTABLE([('M', '1000022', eval(simdict["GMSBNeutralino"])), ('M', '1000039', eval(simdict.get("GMSBGravitino",'0')))])
        get_and_fix_PDGTABLE([
                              (1000022, eval(simdict["GMSBNeutralino"]), '~chi(0,1)', '0'),
                              (1000039, eval(simdict.get("GMSBGravitino",'0')), '~G', '0')
                            ])

    elif GMSBIndex == 2:
        m_stau    = eval(simdict["GMSBStau"])
        m_slepton = eval(simdict["GMSBSlepton"])
        # get_and_fix_PDGTABLE([('M', '1000015', m_stau), ('M', '2000011', m_slepton), ('M', '2000013', m_slepton)])
        get_and_fix_PDGTABLE([
                              (1000015, m_stau, '~tau(L)', '-'),
                              (2000011, m_slepton, '~e(R)', '-'),
                              (2000013, m_slepton, '~mu(R)', '-')
                            ])

    elif GMSBIndex == 3:
        m_stau = eval(simdict["GMSBStau"])
        m_slepton = eval(simdict["GMSBSlepton"])
        m_squark = eval(simdict["SQUARKMASS"])
        m_neutralino = eval(simdict["NEUTRALINOMASS"])
        m_gluino = eval(simdict["GLUINOMASS"])
        # get_and_fix_PDGTABLE([('M', '1000001', m_squark), ('M', '2000001', m_squark), ('M', '1000002', m_squark), \
        #                       ('M', '2000002', m_squark), ('M', '1000022', m_neutralino), ('M', '1000021', m_gluino), \
        #                       ('M', '1000003', 1.00E+04 ), ('M', '2000003', 1.00E+04 ), ('M', '1000004', 1.00E+04 ), \
        #                       ('M', '2000004', 1.00E+04 ), ('M', '1000005', 1.00E+04 ), ('M', '2000005', 1.00E+04 ), \
        #                       ('M', '1000006', 1.00E+04 ), ('M', '2000006', 1.00E+04 ), ('M', '1000011', 2.50E+02 ), \
        #                       ('M', '1000012', 1.00E+04 ), ('M', '1000013', 2.50E+02 ), ('M', '1000014', 1.00E+04 ), \
        #                       ('M', '1000015', m_stau ), ('M', '1000016', 1.00E+04 ), ('M', '2000011', m_slepton ), \
        #                       ('M', '2000013', m_slepton ), ('M', '2000015', 2.50E+02 ), ('M', '1000023', 1.00E+04 ), \
        #                       ('M', '1000024', 1.00E+04 ), ('M', '1000025', -1.0E+04 ), ('M', '1000035', 1.00E+04 ), \
        #                       ('M', '1000037', 1.00E+04 ) ])
        get_and_fix_PDGTABLE([
                              (1000001, m_squark, '~d(L)', '-1/3'), (2000001, m_squark, '~d(R)', '-1/3'),
                              (1000002, m_squark, '~u(L)', '+2/3'), (2000002, m_squark, '~u(R)', '+2/3'),
                              (1000003, 1.00E+04, '~s(L)', '-1/3'), (2000003, 1.00E+04, '~s(R)', '-1/3'),
                              (1000004, 1.00E+04, '~c(L)', '+2/3'), (2000004, 1.00E+04, '~c(R)', '+2/3'),
                              (1000005, 1.00E+04, '~b(1)', '-1/3'), (2000005, 1.00E+04, '~b(2)', '-1/3'),
                              (1000006, 1.00E+04, '~t(1)', '+2/3'), (2000006, 1.00E+04, '~t(2)', '+2/3'),
                              (1000011, 2.50E+02, '~e(L)', '-'), (2000011, m_slepton, '~e(R)', '-'),
                              (1000012, 1.00E+04, '~nu(e,L)', '0'),
                              (1000013, 2.50E+02, '~mu(L)', '-'), (2000013, m_slepton, '~mu(R)', '-'),
                              (1000014, 1.00E+04, '~nu(e,L)', '0'),
                              (1000015, m_stau, '~tau(L)', '-'), (2000015, 2.50E+02, '~tau(R)', '-'),
                              (1000016, 1.00E+04, '~nu(tau,L)', '0'),
                              (1000021, m_gluino, '~g', '0'),
                              (1000022, m_neutralino, '~chi(0,1)', '0'),
                              (1000023, 1.00E+04, '~chi(0,2)', '0'),
                              (1000024, 1.00E+04, '~chi(+,1)', '+'),
                              (1000025, -1.00E+04, '~chi(0,3)', '0'),
                              (1000035, 1.00E+04, '~chi(0,4)', '0'),
                              (1000037, 1.00E+04, '~chi(+,2)', '+')
                            ])
        
    elif GMSBIndex == 4:
        # get_and_fix_PDGTABLE([('M', '1000015', eval(simdict["GMSBStau"]))])
        get_and_fix_PDGTABLE([
                              (1000015, m_stau, '~tau(L)', '-')
                            ])

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
        from G4AtlasApps import AtlasG4Eng
        simdict = AtlasG4Eng.G4Eng.Dict_SpecialConfiguration
except:
    from G4AtlasApps import AtlasG4Eng
    simdict = AtlasG4Eng.G4Eng.Dict_SpecialConfiguration

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
        
        def modify_slepton_mass(name, mass):
            slepton = AtlasG4Eng.G4Eng.gbl.ParticleDataModifier(name)
            slepton.SetParticleMass(mass)


        if GMSBIndex == 1: # generic neutralino to photon scenario

            GMSBNeutralino = eval(AtlasG4Eng.G4Eng.Dict_SpecialConfiguration["GMSBNeutralino"])
            GMSBTime = eval(AtlasG4Eng.G4Eng.Dict_SpecialConfiguration["GMSBLifeTime"])
            AtlasG4Eng.G4Eng.log.info("Generic GMSB neutralino to photon scenario (mass=%s MeV/time=%s ns)" % (GMSBNeutralino, GMSBTime))

            AtlasG4Eng.G4Eng._ctrl.load("Gauginos")
            NLsp=AtlasG4Eng.G4Eng.gbl.ParticleDataModifier("s_chi_0_1")
            NLsp.SetParticleMass(GMSBNeutralino)
            NLsp.SetParticleLifeTime(GMSBTime)
            NLsp.Stable(False)
            NLsp.AddDecayChannel("s_chi_0_1",1.,"s_G=gamma")

            if AtlasG4Eng.G4Eng.Dict_SpecialConfiguration.has_key("GMSBGravitino"):
                GMSBGravitino = eval(AtlasG4Eng.G4Eng.Dict_SpecialConfiguration["GMSBGravitino"])
                AtlasG4Eng.G4Eng.log.info("Adding gravitino mass (gravitino mass=%s keV)" % (GMSBGravitino*1E3))

                Lsp=AtlasG4Eng.G4Eng.gbl.ParticleDataModifier("s_G")
                Lsp.SetParticleMass(GMSBGravitino)

                del GMSBGravitino

            del GMSBNeutralino, GMSBTime
            
        elif GMSBIndex == 2 or GMSBIndex == 3 or GMSBIndex == 4: # generic stau scenario

            GMSBStau    = eval(AtlasG4Eng.G4Eng.Dict_SpecialConfiguration["GMSBStau"])
            AtlasG4Eng.G4Eng.log.info("Generic GMSB stau scenario (stau=%s MeV)" % GMSBStau)

            AtlasG4Eng.G4Eng._ctrl.load("Sleptons")
            modify_slepton_mass("s_tau_plus_1",  GMSBStau)
            modify_slepton_mass("s_tau_minus_1", GMSBStau)

            if AtlasG4Eng.G4Eng.Dict_SpecialConfiguration.has_key("GMSBSlepton"):
                GMSBSlepton = eval(AtlasG4Eng.G4Eng.Dict_SpecialConfiguration.get("GMSBSlepton", None))
                AtlasG4Eng.G4Eng.log.info("Adding slepton mass to GMSB stau scenario (slepton=%s MeV)" % GMSBSlepton)

                modify_slepton_mass("s_mu_plus_R",   GMSBSlepton)
                modify_slepton_mass("s_mu_minus_R",  GMSBSlepton)
                modify_slepton_mass("s_e_plus_R",    GMSBSlepton)
                modify_slepton_mass("s_e_minus_R",   GMSBSlepton)

                del GMSBSlepton

            del GMSBStau

        del GMSBIndex


    simFlags.InitFunctions.add_function("preInitPhysics", gmsb_setparams)

    def gmsb_applycalomctruthstrategy():
    ## Applying the MCTruth strategies: add decays in the Calorimeter
        from G4AtlasApps import AtlasG4Eng
        myDecay = AtlasG4Eng.G4Eng.Dict_MCTruthStrg.get('Decay')
        myDecay.add_Volumes('CALO::CALO', 1)

    simFlags.InitFunctions.add_function("postInit", gmsb_applycalomctruthstrategy)

del doG4SimConfig, simdict

