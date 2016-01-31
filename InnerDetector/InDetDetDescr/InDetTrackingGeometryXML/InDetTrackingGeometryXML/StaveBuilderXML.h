/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// StaveBuilderXML.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef STAVEBUILDERXML_H
#define STAVEBUILDERXML_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// STD
#include <vector>

namespace Trk {
  class AlpineStave;
}

namespace InDet {
  class XMLReaderSvc;
  class IModuleProvider;
  class ModuleTmp;
  class StaveTmp;

  static const InterfaceID IID_StaveBuilderXML("StaveBuilderXML", 1, 0);

  class StaveBuilderXML : virtual public AthAlgTool {
  public:
    StaveBuilderXML(const std::string& t, const std::string& n, const IInterface* p);
    virtual ~StaveBuilderXML();

    // Interface ID for python stuff to work
    static const InterfaceID& interfaceID() { return IID_StaveBuilderXML; }

    // AlgTool initialize and finalize methods
    StatusCode initialize();
    StatusCode finalize();

    // Set Pixel or SCT case
    inline void setPixelCase(bool isPixel) { m_pixelCase = isPixel; }

    // Create a new stave
    Trk::AlpineStave *createStave(InDet::StaveTmp* staveTmp, int istave, int nstaves,  
				  double R, double ztiltstave, double zoffsetstave, 
				  double phiOffset, bool isOuterMost);

    // Get stave template for Pixel/SCT
    std::vector<InDet::StaveTmp*> getStaveTemplate(int ilayer);

  private:
    bool  m_pixelCase; // flag for pixel/sct
    ServiceHandle<InDet::XMLReaderSvc> m_xmlReader;
    ToolHandle<IModuleProvider> m_moduleProvider;
    InDet::StaveTmp *m_prevStaveTmp;
    std::vector<double> *m_mountainPosPositions, *m_mountainNegPositions;
  };
}

#endif //STAVEBUILDERXML_H

