/**
 * @page tostpage TopObjectSelectionTools
 *
 * @section in Introduction
 *
 * The idea of this package is to have a few pre-defined object selections but
 * also the ability to add and modify them until you're happy.
 *
 * The first stop to configure your selection would be to take a look at
 * top::TopObjectSelection.  Where you can see that you can supply it with
 * various classes (even write your own) and options to change the object
 * selection.
 *
 * An example of a selection might go something a bit like this:
 *
 * @code
 * top::TopObjectSelection* objectSelection = new top::TopObjectSelection(topConfig->objectSelectionName());
 * top::check(objectSelection->setProperty( "config" , topConfig ) , "Failed to setProperty for top::TopObjectSelection" );
 * top::check(objectSelection->initialize() , "Failed to initialize top::TopObjectSelection" );
 *
 * objectSelection->electronSelection(new top::ElectronLikelihoodMC15(25000., true, LikeEnum::Tight, LikeEnum::Medium, new top::ApproxPTVarCone(0.05, 0.)));
 * objectSelection->muonSelection(new top::MuonMC15(25000., 2.5, new top::StandardIsolation())); //new top::ApproxPTVarCone(0.05, 0.)));
 * objectSelection->jetSelection(new top::JetMC15(25000., 2.5, 0.941)); // new jet vertex tagger cut
 *
 * objectSelection->overlapRemovalPostSelection(new top::OverlapRemovalASG());
 * @endcode
 *
 * If you don't want isolation then just specify nullptr instead.
 *
 * For DC14 we currently have:
 * - top::ElectronLikelihoodMC15: A cut on likelihood electrons
 * - top::ElectronCutBasedMC15: A cut based electron selection
 * - top::MuonMC15: Added the muon tool to select muons
 * - top::JetMC15: Cut on the jet pT and eta.  Configure those via constructor.
 * - top::OverlapRemovalIndices: A first guess at something a bit like 'standard' top overlap removal used in Run-I
 * - top::OverlapRemovalASG: Using the ASG overlap removal tool
 *
 * For isolation can you can your pick from
 *  - top::AbsoluteIsolationDC14: Only in DC14 derivations
 *  - top::RelativePTVarCone: For cutting on pTVarCone as a fraction of the charged lepton pT. Only in DC14 derivations.
 *  - top::ApproxPTVarCone:  While we wait for the real thing in DC14 ptvarcone, Danilo implemented this.
 *  - top::StandardIsolation: Standard et-cone and pt-cone for electrons. Like run-I, using ElectronIsolationSelectionTool.
 *
 * If you want to modify the cuts you can edit one of those classes and make
 * some changes, or you can create a new class following those as a template.
 * You'll need to  inherit from:
 * - top::ElectronSelectionBase
 * - top::MuonSelectionBase
 * - top::JetSelectionBase
 * - top::OverlapRemovalBase
 * - top::IsolationBase
 *
 * That way the compiler can check that you're using the correct objects (i.e.
 * that the electron tool really only takes electrons as an argument).  Using
 * the compiler to spot this kind of error is super useful, no?
 */
