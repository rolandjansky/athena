#!/usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# $Id$
#
# Unit test for the MissingMassTool class.
#

## C/C++ style main function
import sys
import ROOT

# Helper function creating a vector<string> object out of a list of strings
def toVectorString( stringList ):

    # Create the result object:
    result = ROOT.vector( "string" )()

    # Fill it:
    for element in stringList:
        result.push_back( element )
        pass

    # Return the object:
    return result

def main():
    """
    Main function (c++ style)
    """

    # The name of the application
    import os
    APP_NAME = os.path.basename(__file__).rstrip('.py')
    print(APP_NAME)
    # Set up a logger object
    import logging
    logger = logging.getLogger(APP_NAME)
    logger.setLevel(logging.INFO)
    hdlr = logging.StreamHandler(sys.stdout)
    frmt = logging.Formatter('%(name)-14s%(levelname)8s %(message)s')
    hdlr.setFormatter(frmt)
    logger.addHandler(hdlr)

    # Setup the environment
    if ROOT.gROOT.Macro('$ROOTCOREDIR/scripts/load_packages.C'):
        logger.error("Couldn't load the RootCore packages")
        return 1

    if ROOT.xAOD.Init(APP_NAME).isFailure():
        logger.error('Failed to call xAOD::Init(...)')
        return 1
    
    # MissingMassTool import and setup
    from ROOT import MissingMassTool
    missingmass_tool = MissingMassTool('missingmass_tool')

    # Sanity checks of the properties - no need to setup since 
    # default values are specified in the c++ constructor
    missingmass_tool.setProperty(bool)('Decorate', 1).ignore()
    missingmass_tool.setProperty(int)('UseVerbose', 1).ignore()
    missingmass_tool.setProperty('std::string')('CalibSet', '2012').ignore()
    missingmass_tool.setProperty(int)('alg_version', 3).ignore()
    missingmass_tool.setProperty(int)('UseTailCleanup', 0).ignore()
    missingmass_tool.setProperty(int)('NiterFit2', 30).ignore()
    missingmass_tool.setProperty(int)('NiterFit3', 10).ignore()
    missingmass_tool.setProperty(int)('UseTauProbability', 0).ignore()
    

    sc = missingmass_tool.initialize()
    if sc.isFailure():
        logger.error('Failed to initialize the tool')
        return 1

    # Create a transient tree from a test file
    FNAME = os.environ['ASG_TEST_FILE_MC']

    ifile = ROOT.TFile.Open(FNAME, 'read')

    if not ifile:
        logger.error("Could not open input file: %s" % FNAME)
        return 1

    # Create the tree manager:
    mgr = ROOT.xAOD.TTreeMgr( ROOT.xAOD.TEvent.kAthenaAccess )
    if not mgr.readFrom( ifile ).isSuccess():
        logger.error( "Couldn't set up the reading of the input file" )
        return 1

    # Filter which containers to read:
    if not mgr.enableEventObj( toVectorString( [ "EventInfo", "TauJets", "AntiKt4LCTopoJets",
                                                 "MET_Reference_AntiKt4LCTopo" ] ) ).isSuccess():
        logger.error( "Couldn't set up the filtering of the input" )
        return 1

    evts_with_two_taus = 0
    for iev in xrange( int(1E6) ):
        if evts_with_two_taus > 10:
            break
        
        # Load the event:
        if mgr.eventTree().GetEntry( iev ) < 0:
            logger.error( "Couldn't load entry %i" % iev )
            return 1

        logger.info('## Event %s ##' % iev)
        taus = mgr.eventTree().TauJets
        if len(taus) < 2:
            continue
        evts_with_two_taus += 1
        print (taus)
        print (taus.__class__.__name__)
        tau1 = taus.at(0)
        tau2 = taus.at(1)
        # tau1, tau2 = taus[0], taus[1]
        jets = mgr.eventTree().AntiKt4LCTopoJets
        njets_25 = 0
        for i_jet in xrange(jets.size()):
            jet = jets.at(i_jet)
            if jet.pt() > 250000.:
                njets_25 += 1
        
        # In case you want to dump the available MET names
        #for i in xrange(mgr.eventTree().MET_Reference_AntiKt4LCTopo.size()):
            #MET = mgr.eventTree().MET_Reference_AntiKt4EMTopo.at(i)
            #print MET.name()
        
        met = mgr.eventTree().MET_Reference_AntiKt4LCTopo["FinalClus"]
        print (met, met.__class__.__name__)
        print (tau1, tau1.__class__.__name__)
        print (tau2, tau2.__class__.__name__)
        
        ei = mgr.eventTree().EventInfo
        missingmass_tool.apply(ei, tau1, tau2, met, njets_25).ignore()
        logger.info('MMC MAXW = %s GeV' % ei.auxdataConst('double')('mmc_maxw_mass'))
        logger.info('MMC MLM = %s GeV' % ei.auxdataConst('double')('mmc_mlm_mass'))
        logger.info('MMC MLNU3P = %s GeV' % ei.auxdataConst('double')('mmc_mlnu3p_mass'))

        reso = missingmass_tool.GetResonanceVec(0)
        logger.info('Resonance 4vec: (%s, %s, %s, %s)' % (reso.Pt(), reso.Eta(), reso.Phi(), reso.M()))

        met_vec = missingmass_tool.GetFittedMetVec(0)
        logger.info('Fitted Met vec: (%s, %s)' % (met_vec.Px(), met_vec.Py()))

        neutrino_1_vec = missingmass_tool.GetNeutrino4vec(0, 0)
        logger.info('Neutrino 1 4vec: (%s, %s, %s, %s)' % (neutrino_1_vec.Pt(), neutrino_1_vec.Eta(), neutrino_1_vec.Phi(), neutrino_1_vec.M()))
        neutrino_2_vec = missingmass_tool.GetNeutrino4vec(0, 1)
        logger.info('Neutrino 2 4vec: (%s, %s, %s, %s)' % (neutrino_2_vec.Pt(), neutrino_2_vec.Eta(), neutrino_2_vec.Phi(), neutrino_2_vec.M()))

        tau_1_vec = missingmass_tool.GetTau4vec(0, 0)
        logger.info('Tau 1 4vec: (%s, %s, %s, %s)' % (tau_1_vec.Pt(), tau_1_vec.Eta(), tau_1_vec.Phi(), tau_1_vec.M()))
        tau_2_vec = missingmass_tool.GetTau4vec(0, 1)
        logger.info('Tau 2 4vec: (%s, %s, %s, %s)' % (tau_2_vec.Pt(), tau_2_vec.Eta(), tau_2_vec.Phi(), tau_2_vec.M()))


    ifile.Close()
    # Return gracefully
    return 0


if __name__ == '__main__':
    sys.exit(main())


