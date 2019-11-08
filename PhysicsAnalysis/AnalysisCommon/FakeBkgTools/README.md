This package provides support for a few data-driven methods to estimate the fake lepton background, and is developed under supervision of the Isolation / Fake Forum group. Currently three different tools are available:
- `ApplyFakeFactor`, implementing the fake-factor method;
- `AsymptMatrixTool`, implementing the standard matrix method;
- `LhoodMM_tools`, implementing an improved, likelihood-based, version of the matrix method.

These different tools derive from the same interface `CP::IFakeBkgTool` ([header](../../Interfaces/AsgAnalysisInterfaces/IFakeBkgTool.h)) so they can be used interchangeably, and the results obtained with different methods compared. Note that the interface headers are stored in a separate package, [PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces](../../Interfaces/AsgAnalysisInterfaces), which is generally the one you should link against. 

A minimal example of usage of the tools in analysis code would look like this:
```c++
asg::AnaToolHandle<CP::IFakeBkgTool> tool("CP::AsymptMatrixTool/Tool1");
ATH_CHECK(tool.setProperty("InputFiles", std::vector<string>{"real_effs.root", "fake_effs.root"}));
ATH_CHECK(tool.setProperty("Selection", ">=1T")); /// region of interest = events with >= 1 tight lepton
ATH_CHECK(tool.setProperty("Process", ">=1F")); /// fake lepton background = all events with >= 1 fake lepton (tight, by default)
ATH_CHECK(tool.initialize());
{ /// in the event loop:
    xAOD::IParticleContainer particles;
    for(xAOD::Electron* p : electrons)
    {
        p->auxdata<char>("Tight") = ???;
        particles.push_back(p);
    }
    /// ... and same for muons; then:
    ATH_CHECK(tool->addEvent(particles));
}
float yield, statUp, statDown;
ATH_CHECK(tool->getTotalYield(yield, statUp, statDown));
```

The types of particles currently supported are xAOD::Electron, xAOD::Muon and xAOD::TauJet. 

All these methods rely on auxiliary measurements (efficiencies for prompt/fake leptons to satisfy a tighter selection, or the closely-related fake factors). These must be performed independently and provided as input to the tool (ROOT or XML files) via the property `"InputFiles"`. There is a large flexibility in the parametrization of these efficiencies (and their uncertainties) as function of kinematic variables. 

Some of the methods (fake factor, asymptotic matrix tool) are in addition able to provide event-by-event weights, e.g. to store into ROOT ntuples. One can use this feature through the dedicated interface `CP::ILinearFakeBkgTool` which they also derive from:
```c++
asg::AnaToolHandle<CP::ILinearFakeBkgTool> tool("CP::AsymptMatrixTool/Tool1");
///... in the event loop:
    ATH_CHECK(tool->addEvent(particles));
    float weight, statUp, statDown;
    ATH_CHECK(tool->getEventWeight(yield, "=1T", ">=1F"));
    ATH_CHECK(tool->getEventWeight(yield, ">=2T", ">=1F")); /// can be called as many times as needed
```

More documentation is provided on the following topics:
- [recognized format for input ROOT files](doc/import_root.md)
- [recognized format for input XML files](doc/import_xml.md)
- [purpose and syntax of the `selection` argument](doc/arg_selection.md)
- [purpose and syntax of the `process` argument](doc/arg_process.md)
- [filling binned histograms instead of obtaining a single total yield](doc/fill_histograms.md)
- [dealing with uncertainties](doc/handle_uncertainties.md)
- [combining results after running with several parallel jobs](doc/merging_jobs.md)
- [list of configurable properties of the tools](doc/list_of_properties.md)
- [compilation and usage of the tools outside of AnalysisBase](doc/standalone_compilation.md)
