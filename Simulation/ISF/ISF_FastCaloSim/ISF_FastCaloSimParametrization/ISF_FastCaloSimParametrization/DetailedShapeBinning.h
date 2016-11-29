/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetailedShapeBinning_h
#define DetailedShapeBinning_h

class DetailedShapeBinning
{
  public:
  	
    DetailedShapeBinning();
    virtual ~DetailedShapeBinning() {}

    bool run(bool,bool,TChain*, int, float, std::string, int, std::string, std::string, float&,float&, float&, float&, float&, float&, vector<float>);
    
    bool fill(bool,TTree*, int, Float_t*, int, Float_t*, float, std::string, float,TH1F&,TH1F&,float,bool,std::string,float&,float&,float&,float&,float&,vector<int>,bool,std::string,vector<float>);
    void unitsmm(float, float&, float&, float, float);
    void getcellgeom(float, float, float, float&,float&, float&, float&, float&);
    std::vector<float> fVecBin(TH1F*, int, bool);
    bool checkgradient(std::string,TH1F&, std::string, std::string,bool);
    bool checkgranularity(float, float, float, float, float, float, Float_t*, int, float&);
    void fillValidationPlot(float, float, bool, bool, std::string, std::string);


  private:

  ClassDef(DetailedShapeBinning,1);
  
};
#endif

