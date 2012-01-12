/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Example of TrigTauEvRateTool usage
//
// Must first define selectors to use (or select all)
// Second, fill as many samples as desired.  Unique tags will go into
// separate lines in the composite rate table.
// Third, combine unique tags into a summary (useful for J0-JX addition)
// Fourth, print out the results (or use the counter access and print your own)
//
 
testTrigTauEvRateTool() {

  TrigTauEvRateTool tr;

  if (1) {

    // Everything defined in TrigTauFlags
    tr.defineAllSelectors();

  } else {

    // Specific list of triggers
    tr.defineSelector( TrigTauFlags::TAU12);
    tr.defineSelector( TrigTauFlags::TAU16I);
    tr.defineSelector( TrigTauFlags::TAU20I);
    tr.defineSelector( TrigTauFlags::TAU29I);
    tr.defineSelector( TrigTauFlags::TAU38);
    tr.defineSelector( TrigTauFlags::TAU38I);
    tr.defineSelector( TrigTauFlags::TAU50);
    tr.defineSelector( TrigTauFlags::TAU84);
    tr.defineSelector( TrigTauFlags::TAU125);
    tr.defineSelector( TrigTauFlags::tau16i_xe30);

  }

  double weight = 1.51;
  tr.defineSample("MinBias");
  tr.fillSample("MinBias", "ntuples/EBias/EBias.RDO.v12003103.TTP11.merged.0.root", weight);

    //  tr.print();
    //  tr.printSelector("tau16i", true);
    //  tr.printSelector("tau50", true);

  tr.printRateSum("MinBias", true);
}
