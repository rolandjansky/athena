## Filling histograms

The predicted fake background yield returned by `getTotalYield()` is a single number. One might however be interested in obtaining predictions for different regions, or for binned distributions of particular variables. Setting up a dedicated tool instance for each bin would be rather inefficient. The tools therefore provide support for binned predictions, managed via ROOT histograms (TH1 or TH2). The interest of this feature is mainly for methods that are not able to provide an event weight (i.e. tools not derived from `CP::ILinearFakeBkgTool`, meaning for now LhoodMM_tools), since otherwise the task is trivial and can be done directly by the user by filling histograms event by event with the computed weight. 

Histograms to be filled should be registered with the tool prior to the first call to `addEvent()`, together with the variable(s) being displayed in the *x* (and *y*) axis. Updating these variables event by event is the responsibility of the user. 
```c++
ATH(CHECK( tool->initialize() );
TH1F hMET("hMET", ";MET [GeV]", 15, 0, 300);
float missingET;
ATH_CHECK( tool->register1DHistogram(&hMET, &missingET) );
{ /// event loop
    missingET = ???; /// update with value in that event
    ATH_CHECK( tool->addEvent(particles) );
}
float unused;
ATH_CHECK( tool->getTotalYield(unused,unused,unused) ); /// mandatory! fills the histograms
hMET.Draw();
}
```
Two-dimensional histograms can be handled as well:
```c++
TH2F hPt1Pt2("hMET", ";MET [GeV]", 15, 0, 300);
float lepton1Pt, lepton2Pt;
ATH_CHECK( tool->register2DHistogram(&hPt1Pt2, &lepton1Pt, &lepton2Pt) );
```

### Uncertainties

Histogram bin errors correspond only to the (symmetrized) statistical uncertainty in the predicted yield. To retrieve systematic uncertainties, one should use `applySystematicVariation()`, which will change the bin contents accordingly to the requested up/down variation (see [current limitations for LhoodMM_tools](handle_uncertainties.md)). Do not forget to call `getTotalYield()` each time to update the histogram:
```c++
std::unordered_map<CP::SystematicSet, TH1F> hMETSysts;
for(auto& systvar : tool->recommendedSystematics())
{
    CP::SystematicSet systset{systvar};
    ATH_CHECK( tool->applySystematicVariation(systset) );
    ATH_CHECK( tool->getTotalYield(unused, unused, unused) ); // this changes the registered hMET
    hMETSysts[systset].Copy(hMET);
}
```

### Likelihood matrix tool and histogram normalization

Due to the non-linear nature of the procedure, one will find in general that the sum of the predictions in each bin does not match the overall unbinned prediction. This is normal! 

But it might be annoying. The tool therefore gives the option to rescale the binned prediction such that the sum of bins matches the unbinned estimate. This can be enabled by setting the dedicated property *FixHistogramNormalization* to true.
