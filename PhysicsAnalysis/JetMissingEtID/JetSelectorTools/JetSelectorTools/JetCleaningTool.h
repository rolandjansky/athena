/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __JETCLEANINGTOOL__
#define __JETCLEANINGTOOL__

/**
   @class JetCleaningTool
   @brief Class for selecting jets that pass cleaning cuts

   @author Zach Marshall
   @date   Feb 2014
*/

// Stdlib includes
#include <string>

// Base classes
#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetSelector.h"

// The xAOD jet type
#include "xAODJet/Jet.h"

// Return object
#include "PATCore/TAccept.h"

class JetCleaningTool : public asg::AsgTool , virtual public IJetSelector
{

  ASG_TOOL_CLASS(JetCleaningTool,IJetSelector)

  public: 
    /** Levels of cut */
    enum CleaningLevel{ VeryLooseBad , LooseBad , MediumBad , TightBad , UnknownCut };

    /** Standard constructor */
    JetCleaningTool(const std::string& name="JetCleaningTool");

    /** Cut-based constructor */
    JetCleaningTool(const CleaningLevel alevel);

    /** Cut and string based constructor */
    JetCleaningTool(const std::string& name , const CleaningLevel alevel);

    /** Standard destructor */
    ~JetCleaningTool() {}

    /** Copy constructor */
    JetCleaningTool(const JetCleaningTool& jct) : asg::AsgTool( jct.name() ) , m_cutName(jct.m_cutName) {}

    /** Initialize method */
    virtual StatusCode initialize();

    /** The main accept method: the actual cuts are applied here */
    const Root::TAccept& accept( const double emf,
                 const double hecf,
                 const double larq,
                 const double hecq,
                 const double time,     //in ns
                 const double sumpttrk, //in MeV, same as sumpttrk
                 const double eta,      //emscale Eta  
                 const double pt,       //in GeV, same as sumpttrk
                 const double fmax,
                 const double negE ,     //in MeV
                 const double AverageLArQF ) const;

    /** The D3PDReader accept method */
    const Root::TAccept& accept( const xAOD::Jet& jet) const;

    int keep(const xAOD::Jet& jet) const final { return accept(jet); }

    /** Helpers for cut names */
    CleaningLevel getCutLevel( const std::string ) const;
    std::string   getCutName( const CleaningLevel ) const;

  private:
    /** Name of the cut */    
    std::string m_cutName;    
    CleaningLevel cutLevel;   

    /** Previous decision */
    mutable Root::TAccept m_accept;

}; // End: class definition

#endif
