# JetTileCorrectionTool

The presence of masked modules in the ATLAS Hadronic Tile Calorimeter for Run2 constitute a problem that affects the data collected. There were two permanently masked modules in TileCal during 2015 (now recovered) and a recent module (LBA5) has been masked for the remaining of 2016. There might be more over the data-taking period ahead of course. 
The energy deposited in these regions is not measured, and therefore the reconstruction of jets is affected: The energy measured for the jets is smaller than their true energy. 
Consequently, the events in which there is a jet falling in one of these masked regions have to be either treated or eliminated. In order to avoid fiducial cuts that would significantly reduce the statistics, a correction for these jets is needed. The JetTileCorrectionTool applies such correction to the affected jets.

The correction applied is derived comparing simulated jets in the scenario of the real calorimeter (with two dead modules) and the healthy calorimeter, and depends on the position of the jet and its pt. 
All the related studies are documented in [ATL-COM-PHYS-2016-064](https://cds.cern.ch/record/2126928).

-------------

### Usage and examples

The tool reads a .root file that contains the parametrizations of the corrections for the jets, depending on their position and pt, and the systematic uncertainties of the correction. The modules mentioned above have been already added to the internal database for the affected run periods (data/Tile_maskedDB_Run2.conf), but the user can also provide a list of extra modules via the `UserMaskedRegions` property.

The public methods of the tool are the following (i.e. those of a common `CP::CorrectionTool`):

  * `virtual CorrectionCode applyCorrection(xAOD::Jet& object)` 
  This method checks if the jet was affected by any of the modules provided by the TileTripReader tool, and corrects it if needed, returning an xAOD::Jet object with modified four-momentum and two decorations 

  * `virtual CorrectionCode correctedCopy(const xAOD::Jet& input, xAOD::Jet& output)` 
  Returns a corrected copy of the jet (output) with the correction applied, while leaving the original (input) jet untouched

And the usual functions to handle the systematic uncertainties:

  * `virtual bool isAffectedBySystematic(const SystematicVariation& systematic)`
     Returns whether the tool is affected by the given systematic.

  * `virtual SystematicSet affectingSystematics() const`
     Returns the list of systematics the tool can be affected by

  * `virtual SystematicSet recommendedSystematics() const`
     Returns the list of systematics the tool recomends to use

  * `const CP::SystematicSet& appliedSystematics() const`
     Returns the currently applied systematics


### Basic usage

A full RootCore working example can be found in <code>util/JetTileCorrectionTester.cxx</code>. To run it, just compile your setup and run:

    JetTileCorrectionTester <YourFavouriteDxAOD.root> 

Two unit tests were also added to run on both MC and data input files

    rc test_ut --pkg=JetTileCorrection

The same can be achieved in CMT by running 

    athena.py JetTileCorrection/JetTileCorrectionAlgJobOptions.py

### Dependencies 

The interface class lives in JetCPInterfaces at the moment, for which we don't have the needed tag for. So you would need to get the trunk version:

   //in RootCore
   rc checkout_pkg atlasoff/Reconstruction/Jet/JetCPInterfaces/trunk

   //in CMT
   pkgco.py -A Reconstruction/Jet/JetCPInterfaces/

#### More details

You can get the code by doing:

    git clone ssh://git@gitlab.cern.ch:7999/tripiana/JetTileCorrection.git
   
To initialize the tool you just need to set the (optional) properties `UserMaskedRegions`, where the masked modules should be listed, and `CorrectionFileName`, which contains the name of the .root file with the parametrizations.

```cpp
    //--- JetTileCorrectionTool                                                                                                                                                                                                                                                   
    tool_jtc = new CP::JetTileCorrectionTool("JetTileCorrectionTool");
    std::string parfile="JetTile_pFile_010216.root"
    std::vector<std::string> dead_modules = {"3 20", "0 9"};
    ATH_CHECK( tool_jtc->setProperty("CorrectionFileName",maindir+"JetTileCorrection/"+parfile) );
    ATH_CHECK( tool_jtc->setProperty("UserMaskedRegions",dead_modules) );
    ATH_CHECK( tool_jtc->initialize() );
```

assuming the tools are members of your algorithm class

```cpp
    CP::JetTileCorrectionTool* tool_jtc; //!
```

When the tool is called two decorations are added to the jet. The `TileStatus` decoration contains the information of how the jet was affected or if it was not affected at all, as an unisgned integer that can be 0, 1 or 2, meaning not affected jet, affected but only in the edge, or affected in the core, respectively. The `Ptraw` decortion saves the jet pt before applying the correction. To be able to extract the decorations of the jet, a couple of accessors have to be defined:
```cpp
    static SG::AuxElement::Accessor<unsigned int> acc_tileok("TileStatus");
    static SG::AuxElement::Accessor<float> acc_ptraw("Ptraw");
```
To correct the 4-momentum of your jets (in case they are affected), and access their decorations you would do e.g.:

```cpp
    //--- ...
    xAOD::JetContainer* jets_sc(0);
    xAOD::ShallowAuxContainer* jets_scaux(0);

    // get your jet container, e.g. via SUSYTools (tool_st here) you do:
    CHECK( tool_st->GetJets(jets_sc, jets_scaux) );

    for( const auto& jet : *jets_sc ){

        //--- apply tile dead module correction
        if ( tool_jtc->applyCorrection(*jet) != CP::CorrectionCode::Ok )
            Error("loop()", "Failed to apply JetTileCorrection!");

        unsigned int j_status = acc_tileok(*jet);
      
        Info(APP_NAME, "Jet Pt before correction = %.3f GeV", acc_ptraw(*jet)*0.001);

        if(j_status == (unsigned int)JTC::TS::GOOD)
	    Info(APP_NAME, "Jet not affected by dead modules");
        else if(j_status == (unsigned int)JTC::TS::EDGE)
	    Info(APP_NAME, "Jet-edge affected by dead modules");
        else //if(j_status == JTC::CORE)
	    Info(APP_NAME, "Jet-core affected by dead modules");

        Info(APP_NAME, "Jet Pt after correction =  %.3f GeV", jet->pt()*0.001);
    }
```
