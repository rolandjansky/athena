# Dealing with uncertainties

The fake lepton background prediction is subject to a set of systematic uncertainties. These originate from the propagation of statistical and systematic uncertainties in the measured efficiencies, which are provided by the user in the input files; more details about the latter can be found in the format descriptions for [ROOT](import_root.md) and [XML](import_xml.md) configuration files. 

### Retrieving uncertainties

The tools derive from `CP::ISystematicTool`, therefore up and down variations of the total yields (or event weight) according to each propagated source of uncertainty, can be obtained in an usual way. For example:

```c++
/// after the event loop
auto sysvars = tool->affectingSystematics();
for(auto& sysvar : sysvars)
{
    float yield, unused;
    ATH_CHECK( tool->applySystematicVariation({sysvar}) );
    ATH_CHECK( tool->getTotalYield(yield, unused, unused) );
}
```

It can also be done similarly at the level of event weights:

```c++
tool->initialize();
auto sysvars = tool->affectingSystematics();
{ /// event loop
    ATH_CHECK( tool->addEvent(particles) ); /// call only once per event!
    for(auto& sysvar : sysvars)
    {
        float weight;
        ATH_CHECK( tool->applySystematicVariation({sysvar}) );
        ATH_CHECK( tool->getEventWeight(weight, "2T", ">=1F") );
    }
}
```
As stated in the example above, `addEvent()` must be called **only once per event**, otherwise there will be double-counting. Looping over systematic variations should only be done for calls to `getTotalYield()` and `getEventWeight()`.


The number of variations depends on the configuration (number of efficiency bins, different sources of systematics, correlations...) therefore methods such as `affectingSystematics()` and related can only be called once the tool has been initialized. 

### Identifying variations

The set of affecting `CP::SystematicVariation` have rather dull, non-informative, names:
```
FAKEBKG_SYST_VARn, FAKEBKG_STAT_VARm (n, m = 0 ... number of uncertainties)
```
To help identifying which source of uncertainty each of these variations correspond to, one can use the following methods to obtain more details:

```c++
#include "FakeBkgTools/IFakeBkgSystDescriptor.h"

for(auto& sysvar : tool->affectingSystematics())
{
    tool->getSystDescriptor()->printUncertaintyDescription(sysvar);
}
```
Beside the method above which displays some information to the standard output stream, the `IFakeBkgSystDescriptor` helper interface also provides other useful methods to identify / classify variations with less human intervention:
```c++
std::string getUncertaintyDescription(const CP::SystematicVariation&);
bool isSystematicUncertainty(const CP::SystematicVariation&);
bool isStatisticalUncertainty(const CP::SystematicVariation&);
bool affectsElectrons(const CP::SystematicVariation&);
bool affectsMuons(const CP::SystematicVariation&);
bool affectsRealEfficiencies(const CP::SystematicVariation&);
bool affectsFakeEfficiencies(const CP::SystematicVariation&);
bool affectsFakeFactors(const CP::SystematicVariation&);
```
as well as a method to find all variations corresponding to source(s) of uncertainty with a particular name in the efficiency input files:
```c++
auto vars = tool->getSystDescriptor()->affectingSystematicsFor("bias");
```


