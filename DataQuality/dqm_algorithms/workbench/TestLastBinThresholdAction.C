/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

void printHistogram(const TH1* histogam, const std::string & name) {
  std::cout << "===========>  BEGIN Histogram: " << name << "  <============" << std::endl;
  histogam->Print("All");
  std::cout << "------------>  END Histogram: " << name << "  <--------------" << std::endl;
}

template<class Exceed>
bool testLastBinThresholdNoAction(std::string name, double value, std::array<double, 3> badValues, double error = -999.) {
  using LastBinThresholdNoAction = dqm_algorithms::LastBinThresholdAction<Exceed,dqm_algorithms::NoAction>;
  auto algorithm = new LastBinThresholdNoAction(name);

  std::unique_ptr<TProfile> histogram(new TProfile("histogram", "Simple Profile", 5, 0, 5));
  histogram->SetDirectory(0);
  for (int i = 0; i < 3; ++i) {
    histogram->Fill(i, value);
  }

  std::unique_ptr<dqm_core::test::DummyAlgorithmConfig> config(new dqm_core::test::DummyAlgorithmConfig());

  config->addParameter("NBinsToWatch", 3);
  config->addParameter("BinThreshold", value);
  if (error > 0.) {
    config->addParameter("FixedError", error);
  }

  // Threshold for number of bins aways from threshold
  config->addGreenThreshold("NBins", 1);
  config->addRedThreshold("NBins", 3);

  std::cout << "Config [" << name << "]: ";
  config->print(std::cout);

  bool isTestOk = true;

  std::cout << std::endl << std::endl;
  histogram->Fill(0., badValues[0]);
  printHistogram(histogram.get(), "Green");
  std::unique_ptr<dqm_core::Result> greenResult(algorithm->execute( "test", *histogram, *config));
  std::cout << "Result: " << *(greenResult.get()) << std::endl;
  isTestOk &= (greenResult->status_ == dqm_core::Result::Green);

  std::cout << std::endl;
  histogram->Fill(1., badValues[1]);
  printHistogram(histogram.get(), "Yellow");
  std::unique_ptr<dqm_core::Result> yellowResult(algorithm->execute( "test", *histogram, *config));
  std::cout << "Result: " << *(yellowResult.get()) << std::endl;
  isTestOk &= (yellowResult->status_ == dqm_core::Result::Yellow);

  std::cout << std::endl;
  histogram->Fill(2., badValues[2]);
  printHistogram(histogram.get(), "Red");
  std::unique_ptr<dqm_core::Result> redResult(algorithm->execute( "test", *histogram, *config));
  std::cout << "Result: " << *(redResult.get()) << std::endl;
  isTestOk &= (redResult->status_ == dqm_core::Result::Red);

  return isTestOk;
}



bool testTileDataCorruption(std::string name, double value, std::array<double, 3> badValues) {
  using TileDataCorruption = dqm_algorithms::LastBinThresholdAction<dqm_algorithms::GreaterThan,dqm_algorithms::TileDQAction>;
  auto algorithm = new TileDataCorruption(name);

  std::unique_ptr<TProfile> histogram(new TProfile("TileDigiErrFracLBA56", "Simple Profile", 5, 0, 5));
  histogram->SetDirectory(0);
  for (int i = 0; i < 4; ++i) {
    histogram->Fill(i, value);
  }

  std::unique_ptr<dqm_core::test::DummyAlgorithmConfig> config(new dqm_core::test::DummyAlgorithmConfig());

  config->addParameter("NBinsToWatch", 3);
  config->addParameter("BinThreshold", value);
  config->addParameter("NBinsForAction", 3);
  config->addGenericParameter("Action", "echo");

  // Threshold for number of bins aways from threshold
  config->addGreenThreshold("NBins", 1);
  config->addRedThreshold("NBins", 4); // Given NBins > NBinsToWatch, do not give red status

  std::cout << "Config [" << name << "]: ";
  config->print(std::cout);

  bool isTestOk = true;

  std::cout << std::endl << std::endl;
  histogram->Fill(1., badValues[0]);
  histogram->Fill(2., badValues[1]);
  histogram->Fill(3., badValues[2]);
  printHistogram(histogram.get(), "Yellow");
  std::unique_ptr<dqm_core::Result> yellowResult(algorithm->execute( "test", *histogram, *config));
  std::cout << "Result: " << *(yellowResult.get()) << std::endl;
  isTestOk &= (yellowResult->status_ == dqm_core::Result::Yellow);

  return isTestOk;
}




void printTestStatus(const std::string & test, bool isTestOk) {
  std::cout << "------------------------------------------------------------" << std::endl;
  std::cout << "TEST [" << test << "]:\t" << (isTestOk ? "PASSED" : "FAILED") << std::endl;
  std::cout << "------------------------------------------------------------" << std::endl;
}

void TestLastBinThresholdAction(void) {

  bool isAllTestOk = true;

  std::string test = "LastBinLessThanThreshold";
  std::cout << std::endl << "=== >>>  TEST: " << test << " <<< ===" << std::endl;
  bool isTestOk = testLastBinThresholdNoAction<dqm_algorithms::LessThan>(test, 100., {10., 20., 30.});
  printTestStatus(test, isTestOk);
  isAllTestOk &= isTestOk;

  test = "LastBinGreaterThanThreshold";
  std::cout << std::endl << "=== >>>  TEST: " << test << " <<< ===" << std::endl;
  isTestOk = testLastBinThresholdNoAction<dqm_algorithms::GreaterThan>(test, 100., {200., 300., 400.});
  printTestStatus(test, isTestOk);
  isAllTestOk &= isTestOk;

  test = "LastBinDifferentThanThreshold";
  std::cout << std::endl << "=== >>>  TEST: " << test << " <<< ===" << std::endl;
  isTestOk = testLastBinThresholdNoAction<dqm_algorithms::DifferentThan>(test, 100., {10., 200., 50.}, 10);
  printTestStatus(test, isTestOk);
  isAllTestOk &= isTestOk;

  test = "TileDataCorruptionThanThreshold";
  std::cout << std::endl << "=== >>>  TEST: " << test << " <<< ===" << std::endl;
  isTestOk = testTileDataCorruption(test, 0., {10., 10., 10.});
  printTestStatus(test, isTestOk);
  isAllTestOk &= isTestOk;

  std::cout << endl;
  std::cout << "===================== >>>  SUMMARY <<< =====================" << std::endl;
  printTestStatus("ALL", isAllTestOk);
}
