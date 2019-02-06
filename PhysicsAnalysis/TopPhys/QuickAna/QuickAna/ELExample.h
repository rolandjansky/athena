/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QuickAna_ELExample_H
#define QuickAna_ELExample_H

#ifdef ROOTCORE

#include <QuickAna/Global.h>

#include <EventLoop/Algorithm.h>
#include <QuickAna/Configuration.h>
#include <memory>

class TH1;

namespace ana
{
  class ELExample : public EL::Algorithm, public ana::Configuration
  {
    /// description: the quickana tool handle
  public:
    std::unique_ptr<IQuickAna> quickAna; //!

    /// description: the histograms we create and fill
  public:
    TH1 *muon_n; //!



    // this is a standard constructor
    ELExample ();

    // these are the functions inherited from Algorithm
    virtual EL::StatusCode setupJob (EL::Job& job);
    virtual EL::StatusCode fileExecute ();
    virtual EL::StatusCode histInitialize ();
    virtual EL::StatusCode changeInput (bool firstFile);
    virtual EL::StatusCode initialize ();
    virtual EL::StatusCode execute ();
    virtual EL::StatusCode postExecute ();
    virtual EL::StatusCode finalize ();
    virtual EL::StatusCode histFinalize ();

    // this is needed to distribute the algorithm to the workers
    ClassDef(ELExample, 1);
  };
}

#endif

#endif
