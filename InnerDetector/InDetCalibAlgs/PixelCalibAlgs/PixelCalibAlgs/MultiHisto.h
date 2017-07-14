/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MultiHisto_h
#define MultiHisto_h

#include <TObject.h>


class string;
class TCanvas;
template < class T, class Allocator > class vector;

template <class ht>
class MultiHisto: public TObject{

public:

	MultiHisto(const ht &histo_model,const std::vector<std::string> &nameDiv,
			const std::vector<int> &nDiv,const std::vector<double *> &binsDiv);

	MultiHisto(const ht &histo_model,const std::vector<std::string> &nameDiv,
			const std::vector<std::vector <float> > &binsDiv);

	~MultiHisto();
	
	// function to fill the histograms
	int Fill(double xvar, double yvar,const std::vector<double> &pars);
	int Fill(double xvar, double yvar,const std::vector<float> &pars);
	int FillFromFile(TDirectory *histodir = 0);

	// funtions to write histograms
	int Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0) const;
        int Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0);
	int WriteAndCount(int *nhistos, int *nbins) const;

	// functions to get informations
	virtual const char *GetTitle() const;
	virtual const char *GetName() const;
	unsigned int GetNhistos() const;

	// functions to draw histograms
	virtual void Draw(const Option_t* option = "");
	virtual void Draw(const int color,const Option_t* option = "");	
	virtual void Draw(std::vector <TCanvas*> &c1,const int color, const Option_t* option = "");	

	// functions to get global / detailed indexes
	int GetGlobalIndex(const std::vector<int> &indexes) const;
	int GetGlobalIndex(const std::vector<double> &pars) const;
	int GetGlobalIndex(const std::vector<float> &pars) const;
	std::vector<int> GetDivisionsIndexes(int globalindex) const;
	std::vector<int> GetDivisionsIndexes(const std::vector<double> &pars) const;	
	std::vector<int> GetDivisionsIndexes(const std::vector<float> &pars) const;	
	
	// functions to get histograms
	ht* GetHisto(int globalindex);
	ht* GetHisto(const std::vector<int> &indexes);

private:
	MultiHisto(){};

	// the histograms
	const ht *m_model;
	std::vector<ht*> m_myhistos;
	
	// std::vector of bins for divisions
	std::vector<int> m_div_Nbins;
	std::vector<std::string> m_div_names;
	std::vector<double*> m_div_bins;

	// functions to compose titles & names during allocation
	std::string ComposeName(const std::vector<int> &indexes);
	std::string ComposeTitle(const std::vector<int> &indexes);
	std::string TitleString(int idiv, int index);
	std::string NameString(int idiv, int index);
	
};

#endif

