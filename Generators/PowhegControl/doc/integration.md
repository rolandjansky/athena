# Integration and timing

[[_TOC_]]

## Validation and approximate generation time

Using the default settings, the **approximate** time taken to generate
5000 events (including the integration, event generation and
hadronisation steps) is given below. As described in
[PowhegForATLAS#Re_using_integration_files](powheg_for_atlas#Re_using_integration_files),
the integration step can be skipped by re-using the integration grids.
If the integration step is takes longer than **3 hours** to run this
should be considered. If this step takes longer than **6 hours** it
might become mandatory in future to re-use the integration files in
production. Anything that takes longer than **24 hours** may well be
impossible to run in production without re-use of the integration grids.

Before generating events, POWHEG-BOX performs an integration over a
multi-dimensional parameter space. This can be quite slow and so the
integration parameters must be chosen in order to get a balance between
speed and precision. The values used in `PowhegControl` have been chosen
in line with the following guidelines recommended by the POWHEG-BOX
authors.


| Test description          | Requirement                  |
| :------------------------ | :--------------------------- |
| Cross section uncertainty | \< 1% of total cross section |
| Negative weight events    | \< 1% of events              |
| Upper bound failures      | \< 1% of events              |


Wherever possible, we choose default settings that will pass all of
these tests. The tests are automatically performed by PowhegControl, so
in the `log.generate` file produced when running the jobOptions you will
see output like

```terminal
Py:PowhegControl INFO Integration test :: cross-section uncertainty : 0.30%
19:25:39 Py:PowhegControl INFO Integration test :: negative weight fraction : 0.52%
19:25:39 Py:PowhegControl INFO Integration test :: upper bound violations : 0.82%
19:25:39 Py:PowhegControl INFO -> in inclusive cross-section : 0.82%
19:25:39 Py:PowhegControl INFO -> in generation of radiation : 0.00%
```

In the table below, you can see the results of each of these tests for
each supported process. In each case, if the test result is less than 1%
this is indicated by a :white_check_mark:, if it's between 1% and 10%
then it's preceded by a :exclamation:, while those greater than 10%
are preceded by a :bangbang:. If you generate events with
processes where any of the tests have failed, you should check carefully
whether the events that pass your analysis selection give physical
(non-zero) results in a variety of sensible observables.

| PowhegControl name | nCores | Approximate integration time (hrs) | Estimated generation time for 5000 events (hrs) | Cross-section uncertainty                        | Negative weight fraction                          | Upper bound violations                            | Physics validation |
| :----------------- | :----- | :--------------------------------- | :---------------------------------------------- | :----------------------------------------------- | :------------------------------------------------ | :------------------------------------------------ | :----------------- |
| `bb`               | 1      | 1                                  | 0.05                                            | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `bbH`              | 10     | 34                                 | 4                                               | :bangbang: \~45%                                 | :bangbang: \~55%                                  | :bangbang: \~55%                                  | NO                 |
| `bblvlv`           | 10     | 10.5                               | 1                                               | :white_check_mark:                               | :exclamation: \~2.5%                              | :white_check_mark:                                | NO                 |
| `chi0chi0`         | 1      | 10                                 | 1                                               | :white_check_mark:                               | :bangbang: \~14%                                  | :white_check_mark:                                | NO                 |
| `chi0chi1`         | 1      | 0.05                               | 0.01                                            | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `chi1chi1`         | 1      | 3                                  | 0.05                                            | :white_check_mark:                               | :bangbang: \~12%                                  | :white_check_mark:                                | NO                 |
| `DMGG`             | 1      | 0.5                                | 0.5                                             | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `DMS_tloop`        | 1      | 0.1                                | 0.01                                            | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `DMV`              | 1      | 2.5                                | 0.05                                            | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `ggF_H`            | 1      | 0.1                                | 0.01                                            | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `ggF_HH`           | 10     | 7                                  | 1.5                                             | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `ggF_HZ`           | 1      | 4.5                                | 0.5                                             | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `Hj`               | 1      | 16                                 | 1                                               | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `Hjj`              | 10     | 20                                 | 4                                               | :white_check_mark:                               | :bangbang: \~11%                                  | :white_check_mark:                                | NO                 |
| `HWj`              | 1      | 13                                 | 1                                               | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `HWj_EW`           | 10     | 12                                 | 0.5                                             | :white_check_mark:                               | :white_check_mark:                                | :exclamation: \~3%                                | NO                 |
| `HZj`              | 10     | 8.5                                | 1                                               | :white_check_mark:                               | :white_check_mark:                                | :exclamation: \~1.5%                              | NO                 |
| `HZj_EW`           | 10     | 29                                 | 0.5                                             | :white_check_mark:                               | :exclamation: \~1.5%                              | :exclamation: \~2.5%                              | NO                 |
| `jj`               | 1      | 8.5                                | 1                                               | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `jjj`              | 10     | 26                                 | 1.5                                             | :white_check_mark:                               | :bangbang: \~14%                                  | :white_check_mark:                                | NO                 |
| `ssWWjj`           | 10     | 18.5                               | 8                                               | :bangbang: \~40%                                 | :white_check_mark:                                | :bangbang: \~65%                                  | NO                 |
| `t_sch`            | 1      | 0.5                                | 0.01                                            | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `t_tch_4FS`        | 10     | 4.5                                | 1                                               | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `tt`               | 1      | 0.05                               | 0.01                                            | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `tt_NLOdecays`     | 1      | 2                                  | 0.1                                             | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `ttH`              | 1      | 1                                  | 0.1                                             | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `ttj`              | 1      | 20                                 | 4.5                                             | :white_check_mark:                               | :bangbang: \~36%                                  | :exclamation: \~2%                                | NO                 |
| `VBF_H`            | 1      | 4                                  | 0.1                                             | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `VBF_ssWW`         | 10     | 16.5                               | 0.1                                             | :white_check_mark:                               | :exclamation: \~4%                                | :white_check_mark:                                | NO                 |
| `VBF_W`            | 10     | 5.5                                | 0.1                                             | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `VBF_Z`            | 10     | 14                                 | 0.5                                             | :white_check_mark:                               | :bangbang: \~9                                    | :white_check_mark:                                | NO                 |
| `W`                | 1      | 0.05                               | 0.01                                            | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `W_EW`             | 1      | 0.5                                | 0.01                                            | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `Wbb`              | 1      | 13                                 | 0.2                                             | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `Wbbj`             | 10     | 31                                 | 0.5                                             | :white_check_mark:                               | :bangbang: \~4%                                   | :white_check_mark:                                | NO                 |
| `Wj`               | 10     | 10                                 | 0.1                                             | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `Wjj`              | 10     | 30                                 | 0.5                                             | :white_check_mark:                               | :bangbang: \~20%                                  | :white_check_mark:                                | NO                 |
| `Wt_DR`            | 1      | 0.2                                | 0.01                                            | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `Wt_DS`            | 1      | 12                                 | 0.1                                             | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `WW`               | 1      | 0.1                                | 0.01                                            | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `Wy`               | 10     | 12                                 | 0.1                                             | :white_check_mark:                               | :bangbang: \~8%                                   | :white_check_mark:                                | NO                 |
| `WZ`               | 1      | 2                                  | 0.05                                            | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `Z`                | 1      | 0.1                                | 0.01                                            | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `Z_EW`             | 1      | 2                                  | 0.2                                             | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `Zj`               | 10     | 8                                  | 0.2                                             | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
| `Zjj`              | 10     | 30                                 | 0.5                                             | :white_check_mark:                               | :bangbang: \~35%                                  | :white_check_mark:                                | NO                 |
| `ZZ`               | 1      | 0.2                                | 0.01                                            | :white_check_mark:                               | :white_check_mark:                                | :white_check_mark:                                | NO                 |
