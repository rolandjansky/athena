/*
  Copyright (C) 2021 CERN for the benefit of the ATLAS collaboration
*/

template <class Htype> void InDetPlotBase::book (Htype* & pHisto, 
                                const std::string& histoIdentifier, 
                                const std::string & nameOverride, 
                                const std::string& folder){
  const SingleHistogramDefinition hd = retrieveDefinition(histoIdentifier, folder, nameOverride);
  if (hd.empty()) {
    ATH_MSG_WARNING("Histogram definition is empty for identifier " << histoIdentifier);
  }
  book(pHisto, hd);
}
