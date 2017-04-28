///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Author: Danilo Ferreira de Lima <dferreir@cern.ch>
///////////////////////////////////////////////////////////////////

#ifndef PDFWEIGHT_H
#define PDFWEIGHT_H

// Include the base class
#include "ReweightUtils/WeightToolBase.h"

namespace LHAPDF {
class PDF;
}

class PDFWeight : public WeightToolBase
  {
    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS2( PDFWeight, IWeightTool, IObservableTool )

   public:
    /// The default constructor with the instance name of the tool class
    PDFWeight( const std::string& name );

    /// Default destructor
    virtual ~PDFWeight() {};

    StatusCode initialize() override;

    /// returns: the value that was calculated from the xAOD::IParticle (composite built event object for instance)
    virtual double evaluate( const xAOD::IParticle* part ) const override;

    /// returns: the value that was calculated from the usual Athena storegate
    virtual double getWeight() const override;


   private:
    //internal function "computing" the weight
    double computeWeight(const xAOD::EventInfo*) const;

    std::string m_pdfName;
    std::string m_truthEventContainerName;
    std::string m_eventInfoName;
    float m_weight;

    LHAPDF::PDF* m_pdf = 0;

    int m_index;
};

#endif //> !PDFWEIGHT_H
