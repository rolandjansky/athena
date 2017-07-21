/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PDFSCALEFACTORCALCULATOR
#define PDFSCALEFACTORCALCULATOR

#include <memory>
#include <iostream>
#include <string>
#include <unordered_map>

// Framework include(s):
#include "AsgTools/AsgTool.h"

// save diagnostic state
#pragma GCC diagnostic push
// turn off the sily warnings in Boost.
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include "LHAPDF/LHAPDF.h"
// turn back on the warnings
#pragma GCC diagnostic pop
#include "LHAPDF/PDFSet.h"

#include "TopEvent/Event.h"

namespace top {

  class TopConfig;

  /**
   * @brief For testing PDF reweighting with LHAPDF6.  Not serious, just a toy.
   *
   * Lots of stuff is available on cvmfs, set this:
   * export LHAPDF_DATA_PATH=$ROOTCOREBIN/data/Asg_Lhapdf_LHAPDF:/cvmfs/sft.cern.ch/lcg/external/lhapdfsets/current/:$LHAPDF_DATA_PATH
   */
  class PDFScaleFactorCalculator final : public asg::AsgTool {
  public:
    explicit PDFScaleFactorCalculator( const std::string& name );
  
    virtual ~PDFScaleFactorCalculator(){};

    // Delete Standard constructors
    PDFScaleFactorCalculator(const PDFScaleFactorCalculator& rhs) = delete;
    PDFScaleFactorCalculator(PDFScaleFactorCalculator&& rhs) = delete;
    PDFScaleFactorCalculator& operator=(const PDFScaleFactorCalculator& rhs) = delete;

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

    int numberInSet(const std::string& name) const {
      const LHAPDF::PDFSet set(name);
      return set.size();
    }

    /**
     * @brief Print a list of the available PDFs to the screen.
     *
     * I think it looks in the directories in the LHAPDF_DATA_PATH environment
     * variable.
     */
    void printAvailablePDFs() const {
      std::cout << "List of available PDFs:" << std::endl;
      for (const std::string& pdfname : LHAPDF::availablePDFSets())
	std::cout << "    " << pdfname << std::endl;
    }


  private:

    std::shared_ptr<top::TopConfig> m_config;

    std::vector< std::string > m_pdf_names;
    std::string m_base_pdf_name;
    LHAPDF::PDF* m_basepdf = 0;

    // Small helper class to hold the information we need
    class PDFSet {

    public:

      PDFSet(){;};

      explicit PDFSet( const std::string& name ){
	
	LHAPDF::mkPDFs( name, pdf_members );
	unsigned int n_members = pdf_members.size();
	event_weights.resize( n_members );
	sum_of_event_weights.resize( n_members );

      };
           
      std::vector< std::unique_ptr<const LHAPDF::PDF> > pdf_members = {};

      std::vector< float > event_weights = {};
      
      std::vector< float > sum_of_event_weights = {};
      
    };

    std::unordered_map< std::string, PDFSet > m_pdf_sets;
    
  };

}

#endif
