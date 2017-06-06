/**
 * @page testpage TopEventSelectionTools
 *
 * These can be loaded via the configuration file, which calls this function.
 * If you write a new tool (in this package) you have to make sure you add it
 * to this, which is loaded by ROOT at run-time.
 *
 * - top::TopEventSelectionToolsLoader: Code that loads a tool from the name.
 *
 * @section es Event Selection Tools
 *
 * - Random tools
 *  - top::InitialSelector: Select all events.
 *  - top::SaveEventSelector: Save the event to the output Tree.
 *  - top::PrintEventSelector: Print some info about the selected events to the screen.
 *  - top::ExamplePlots: An example of plotting some kinematic variables.
 *  - top::GRLSelector: For data only accept events that are on the GRL.
 *  - top::JetCleaningSelector: Select events only if they have no bad jets.
 *  - top::TrigDecisionSelector: Select events based on the trigger decision.
 *  - top::TrigMatchSelector: Select events with reconstructed charged leptons matching the triggers
 *
 * - Selection based on object multiplicity
 *  - top::NElectronSelector: Cut on the number of electrons.
 *  - top::NMuonSelector: Cut on the number of muons.
 *  - top::NTauSelector: Cut on the number of taus.
 *  - top::NElectronNMuonSelector: Cut on the sum of electrons and muons.
 *  - top::NJetSelector: Cut on the number of jets.
 *  - top::IP3DSV1Selector: Cut on the number of jets passing a certain working point.
 *  - top::MV1Selector: Cut on the number of jets passing a certain working point.
 *  - top::MV2c20Selector: Cut on the number of jets passing a working point for MV2c20 (run-II start-up).
 *
 * - Selection based on event kinematic quantities
 *  - top::METSelector: Place a cut on MET.
 *  - top::MWTSelector: Place a cut on the W transverse mass.
 *  - top::METMWTSelector: Place a cut on the sum of MET and W transverse mass.
 *  - top::HTSelector: Place a cut on HT (electrons, muons and jets).
 *  - top::OSLeptonSelector: Require at least two opposite sign leptons (e, mu).
 *  - top::SSLeptonSelector: Require at least two same sign leptons (e, mu).
 *  - top::MLLSelector: Place a cut on the dilepton (e, mu) invariant mass.
 *  - top::MLLWindow: Place a window on the dilepton (e, mu) invariant mass.
 *
 * @section fd For Developers
 *
 * - top::EventSelectorBase: Base class for all tools that select events.
 * - top::SignValueSelector: For tools that need a sign (equality, inequality) and a value (number).
 * - top::PlotManager: For book-keeping our histograms.
 * - top::TreeManager: For book-keeping variables in a tree.
 * - top::ToolLoaderBase: Base class for tool loaders.
 */
