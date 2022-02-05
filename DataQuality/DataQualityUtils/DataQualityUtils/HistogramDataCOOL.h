/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqutilsHistogramDataCOOL_h
#define dqutilsHistogramDataCOOL_h

// Protect CINT from some system definitions that cause problems
#ifndef __CINT__
  //COOL API include files (CoolKernel)
  #include "CoolKernel/pointers.h"
  #include "CoolKernel/ValidityKey.h"
#else
  namespace cool {
    class IDatabasePtr;
    class IFolderPtr;
  }
#endif


#include <iostream>
#include <string>
#include <cstdlib>

#include <TObject.h>

//CORAL API include files
#include "CoralBase/AttributeList.h"

//COOL API include files (CoolApplication)
#include "CoolApplication/Application.h"
// --> limits.h is needed for CoolKernel/types.h
#include <limits.h>
#include "CoolKernel/types.h"
#include "CoolKernel/ChannelId.h"
#include "CoolKernel/RecordSpecification.h"
#include "CoolKernel/ChannelSelection.h"

#include <sstream>
#include <fstream>
#include <TCanvas.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TIterator.h>
#include <TKey.h>
#include <TLegend.h>
#include <TProfile.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TMath.h>
#include <TTree.h>

namespace coral {
  class AttributeList;
}

namespace cool {
  class RecordSpecification;
  class ChannelSelection;
}


namespace dqutils {

class HistogramDataCOOL : public cool::Application, public TObject {
private:

// Protect CINT from some system definitions that cause problems
// CINT does not need to know about these private variables
#ifndef __CINT__
    cool::ValidityKey m_since;
    cool::ValidityKey m_until;
    cool::IDatabasePtr m_coolDb;
    cool::IFolderPtr m_coolFolder;
    cool::IFolderPtr m_coolFolderH;
#endif

public:
    // Connects to the database. Throws a "DatabaseDoesNotExis" exception if database does not exist.
    cool::IDatabasePtr coolDbInstance(std::string dbStr, bool readOnly);
    
    // Browses the COOL folder. Throws a "FolderNotFound" exception if folder does not exist.
    cool::IFolderPtr coolFolderInstance(std::string folderStr);

    // Various methods to set and print the intervall of validity.
    void setSince(cool::Int64 run, cool::Int64 lumi);
    void setUntil(cool::Int64 run, cool::Int64 lumi);
    void printIOV();
    void setIOV(cool::Int64 runS, cool::Int64 lumiS, cool::Int64 runU, cool::Int64 lumiU);
    void setIOV(cool::Int64 run);

    // Methods needed to come up to COOL framework.
    cool::RecordSpecification createSpec();
    coral::AttributeList createPayload(int colourCode, float dfrac, float thrust, const 
                                       cool::RecordSpecification& spec);
    cool::RecordSpecification createSpecH();
    coral::AttributeList createPayloadH(int colourCode, std::string algo, int entries, float par1, float par2, float par3, float par4, float par5, const 
					cool::RecordSpecification& specH);
   
    // Constructors and Destructors.
    HistogramDataCOOL(std::string dbStr, std::string folderStr, int runS, int lumiS, int runU, int lumiU);
    HistogramDataCOOL(int runS, int lumiS, int runU, int lumiU);
    HistogramDataCOOL();
    virtual ~HistogramDataCOOL ();

    // Methods of "interest". To insert a status flag you can use either ChannelId or ChannelName.
    void dump(cool::ChannelSelection selection, std::string tag_name);
    std::string dumpHisto(cool::ChannelId channelId, std::string field, std::string tag_name);
    int dumpCode(std::string channelName, std::string tag_name);
    void dumpall(std::string tag_name);

    void insertH(cool::ChannelId channelId, int code, std::string algo, int entries, float par1, float par2, float par3, float par4, float par5, std::string tag_name);
    void insertH(std::string channelName, int code, std::string algo, int entries, float par1, float par2, float par3, float par4, float par5, std::string tag_name);

    virtual void   ntupleDB( int HistoId, std::string nameHisto, std::string configuration, int Irun, int Frun );
    virtual void   historyDB( int HistoId, std::string nameHisto, std::string tag_name );
    std::string defParName( std::string Algorithm, std::string nameHisto, int i);
    virtual void formatGraph( TCanvas* c, TGraphErrors* gr ) const;

    // Some further getters.
    cool::IFolderPtr getCoolFolderH();
    cool::IDatabasePtr getCoolDb();


    // Needed for the ROOT interface.
    ClassDef( HistogramDataCOOL, 0 ) // A class for modifying DQ info in the COOL database
};

}

#endif
