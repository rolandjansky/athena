/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class TreeShapeErrorGetter
   @brief Liquid Argon base class for shape information
*/

#ifndef LArSamples_TreeShapeErrorGetter_H
#define LArSamples_TreeShapeErrorGetter_H

#include "LArSamplesMon/AbsShapeErrorGetter.h"

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TVectorD.h"
#include "LArCafJobs/Definitions.h"
#include <vector>

class TH2D;

namespace LArSamples {

  class ShapeErrorData;
  class ResidualCalculator;
  class Interface;
  
  class TreeShapeErrorGetter : public AbsShapeErrorGetter {
  
    public:
      
      TreeShapeErrorGetter(const TString& fileName, bool recreate = false);
      virtual ~TreeShapeErrorGetter();

      ShapeErrorData* shapeErrorData(unsigned int hash, CaloGain::CaloGain gain, const Residual* toExclude = 0) const;
      ShapeErrorData* phiSymShapeErrorData(short ring, CaloGain::CaloGain gain, const Residual* toExclude = 0) const;
      
      int addCell(const ResidualCalculator& calc, CaloGain::CaloGain gain);
      int addRing(const ResidualCalculator& calc, CaloGain::CaloGain gain);

      void dump(CaloGain::CaloGain gain) const;
      TH2D* correlate(const TreeShapeErrorGetter& other, CaloGain::CaloGain gain, unsigned short sample, bool xip, 
                      unsigned int nBins, double xMin, double xMax); 
 
      bool compare(const TreeShapeErrorGetter& other, const TString& fileName, const Interface* tmpl = 0);
      
      const ResidualCalculator* cellCalc() const { return m_cellCalc; }
      const ResidualCalculator* ringCalc() const { return m_ringCalc; }
      
      TTree* cellTree(CaloGain::CaloGain gain) const;
      TTree* ringTree(CaloGain::CaloGain gain) const;
      
      static bool merge(const TString& listFile, const TString& outputFile);
      static bool merge(const std::vector<const TreeShapeErrorGetter*>& getters, const TString& outputFile);
      
      TFile* file() const { return m_file; }
      
    private:
      
      TFile* m_file;
      std::vector<TTree*> m_cellTrees;
      std::vector<TTree*> m_ringTrees;    
      mutable ResidualCalculator* m_cellCalc, *m_ringCalc;
  };
}
#endif
