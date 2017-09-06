/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: BinPrint.cxx,v 1.3 2009-01-22 13:53:56 ponyisi Exp $
// **********************************************************************

#include "dqm_algorithms/BinPrint.h"

#include <cmath>
#include <iostream>
#include <map>

#include <TH1.h>
#include <TClass.h>

#include "dqm_core/exceptions.h"
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_core/Result.h"

static dqm_algorithms::BinPrint staticInstance;

namespace dqm_algorithms {
	
	// *********************************************************************
	// Public Methods
	// *********************************************************************
	
	void
	BinPrint::
	printDescription(std::ostream& out)
	{
		std::string message;
		message += "\n";
		message += "Algorithm: \"" + m_name + "\"\n";
		message += "Description: Extracts bin contents from a TProfile, TH1, TH2 object without performing\n";
		message += "             any assessment; the status is always \"Undefined\"\n";
		message += "Optional Parameter:  UnMask_All    Default = -1\n";
		message += "            <0: Only unmask bins when Mask_## = 0\n";
		message += "           >=0: Unmask all bins and only mask bins when Mask_## = 1\n";
		message += "        # >= 0: Maximum number of bins printed is #\n";
		message += "             This setting is combined so that large bin lists do not occur accidentaly.\n";
		message += "Optional Parameter: UseValue:        Default = 0\n";
		message += "             0: Print all unmasked bins\n";
		message += "             1: Print bin contents >= Value\n";
		message += "            >1: Print bin contents >  Value\n";
		message += "            -1: Print bin contents <= Value\n";
		message += "           <-1: Print bin contents <  Value\n";
		message += "Optional Parameter: Value:\n";
		message += "Optional Parameter: UseMaskValue:        Default = 0\n";
		message += "             0: Mask no bins\n";
		message += "             1: Change Mask for all >= MaskValue\n";
		message += "            >1: Change Mask for all > Mask Value\n";
		message += "            -1: Change Mask for all <= MaskValue\n";
		message += "           <-1: Change Mask for all < Mask Value\n";
		message += "			 NOTE: The same boundaries apply to the mask\n";
		message += "Optional Parameter: MaskValue:\n";
		message += "        Example of Printing values from 0. <= x <= 1.\n";
		message += "             UseValue = 1\n";
		message += "             Value = 0. ## x >= 0.\n";
		message += "             UseMaskValue = 2\n";
		message += "             MaskValue = 1. ## x <= 1.\n";
		message += "        Example of Printing values outside of the range above\n";
		message += "             UseValue = 2\n";
		message += "             Value = 1.\n";
		message += "             ## By Value Only: x > 1.0\n";
		message += "             UseMaskValue = -2\n";
		message += "             MaskValue = 0.\n";
		message += "             ## Including Mask: x < 0.0\n";
		message += "Optional Parameter: TypeValue:      Default = 0\n";
		message += "             0: Value is compared to total entries in bin\n";
		message += "             1: Value is compared to fraction of entries in histogram appearing in bin\n";
		message += "             This also determines how the bin contents will be listed if published.\n";
		message += "1D Histogram Configuration:\n";
		message += "Optional Parameters:  Mask_#\n";
		message += "             0: UnMask bin\n";
		message += "             1: Mask bin\n";
		message += "        Example of Printing all but the first bin\n";
		message += "             ***In algorithm declaration\n";
		message += "             UnMask_All = 3\n";
		message += "             Mask_1 = 1\n";
		message += "2D Histogram Configuration:\n";
		message += "Optional Parameters:  Mask_#_#\n";
		message += "             0: UnMask bin\n";
		message += "             1: Mask bin\n";
		message += "        Example of masking the X = 1, Y = 2 bin:\n";
		message += "             ***In algorithm declaration\n";
		message += "             UnMask_All = 3\n";
		message += "             Mask_1_2 = 1\n";
		message += "\n";
		message += "Optional Parameters:  PrintError\n";
		message += "             1: Print Corresponding Bin Error\n";
		
		out << message;
	}
	
	BinPrint::
	BinPrint()
	: m_name("BinPrint")
	, m_NbinsX(-1)
	, m_NbinsY(-1)
	, m_UnMask_All(-1)
	, m_UseValue(0)
	, m_TypeValue(0)
	, m_Value(0.)
	, m_UseMaskValue(0)
	, m_MaskValue(0.)

	{
		dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name, this );
	}
	
	
	BinPrint::
	~BinPrint()
	{
	}
	
	
	dqm_core::Algorithm*
	BinPrint::
	clone()
	{
		return new BinPrint(*this);
	}
	
	
	dqm_core::Result*
	BinPrint::
	execute( const std::string& name, const TObject& data, const dqm_core::AlgorithmConfig& config)
	{
		//No status flags are set
		dqm_core::Result* result = new dqm_core::Result();
		result->status_ = dqm_core::Result::Undefined;
		//Histogram dimension default = no dimensions
		m_NbinsX = -1;
		m_NbinsY = -1;
		
		if (!data.IsA()->InheritsFrom("TH1")) {
			throw dqm_core::BadConfig(ERS_HERE, name, "does not inherit from TH1");
		}
		TH1* h = (TH1*)&data; // h cannot be null
		if (h->GetDimension() > 2) {
			throw dqm_core::BadConfig(ERS_HERE, name, "dimension > 2 ");
		}
		
		//Get optional parameters
		int Npublished = 0;
		m_UseValue = UseValue_GetFromMap(config.getParameters());
		m_TypeValue = TypeValue_GetFromMap(config.getParameters());
		m_Value = Value_GetFromMap(config.getParameters());
		m_UseMaskValue = UseMaskValue_GetFromMap(config.getParameters());
		m_MaskValue = MaskValue_GetFromMap(config.getParameters());
		const int Error = (int) dqm_algorithms::tools::GetFirstFromMap( "PrintError", config.getParameters(), 0);
	 	//out<<Error<<"Error"<<std::endl;	
		//**********
		// 1D Histogram case
		//**********
		if (h->GetDimension() == 1) {
			m_NbinsX = h->GetNbinsX();
			
			//Get bin mask
			std::vector<bool> Mask;
			Mask = Mask1D_GetFromMap(config.getParameters());
			
			//Rescale m_Value if fractional definition is used
			double h_entries = h->GetEntries();
			if ( m_TypeValue > 0.5 ) m_Value = m_Value * h_entries;
			
			//Check all bins
			for (int bin = 0; bin < m_NbinsX; bin++) {
				double bincon = h->GetBinContent(bin + 1);
				double ebincon = h->GetBinError(bin + 1);
				//out<<"here"<<Mask[bin]<<std::endl;
				//if( !Mask[bin] ) {
				if( Mask[bin] ) {
					bool do_printbin = false;
					if(m_UseValue == 0) do_printbin = true;
					if( (m_UseValue == 1 && bincon >= m_Value) ||
					   (m_UseValue > 1 && bincon > m_Value) ||
					   (m_UseValue == -1 && bincon <= m_Value) ||
					   (m_UseValue < -1 && bincon < m_Value)
					   ) do_printbin = true;
					if( (m_UseMaskValue == 1 && bincon >= m_MaskValue) ||
					   (m_UseMaskValue > 1 && bincon > m_MaskValue) ||
					   (m_UseMaskValue == -1 && bincon <= m_MaskValue) ||
					   (m_UseMaskValue < -1 && bincon < m_MaskValue)
					   ) do_printbin = ! do_printbin;
					if(do_printbin) {
					  //out<<"bin with name " <<  bincon<<std::endl;
						Npublished++;
						if ( m_UnMask_All < 0 || 
							Npublished <= m_UnMask_All ) {
							double binvalX = h->GetXaxis()->GetBinCenter(bin + 1);
							std::string binname = Form("%s_Bin(%u | %.*e)", name.c_str(), bin, 2, binvalX);
							std::string sigbinname = Form("%s_EBin(%u | %.*e)", name.c_str(), bin, 2, binvalX);
							//ERS_DEBUG(1, "bin with name " << binname <<" and value "<< bincon);
							//out<<"bin with name " << binname <<" and value "<< bincon<<std::endl;
							//out<<"bin with name " << sigbinname <<" and value "<< ebincon<<std::endl;
							//From() ~ printf() (s ~ string, u ~ unsigned int, .*e ~ scientific)
							if ( m_TypeValue > 0.5 ) {
								result->tags_[binname.c_str()] = (bincon / h_entries);
								if (Error ==1 ) result->tags_[sigbinname.c_str()] = ebincon ;
							} else {
								result->tags_[binname.c_str()] = bincon;
								if (Error ==1 ) result->tags_[sigbinname.c_str()] = ebincon ;
							}
						}
					}
				}
			}
		}
		
		//**********
		// 2D Histogram case
		//**********
		if (h->GetDimension() == 2) {
			m_NbinsX = h->GetNbinsX();
			m_NbinsY = h->GetNbinsY();
			
			//Get bin mask
			std::vector< std::vector<bool> > Mask;
			Mask = Mask2D_GetFromMap(config.getParameters());
			
			//Rescale m_Value if fractional definition is used
			double h_entries = h->GetEntries();
			if ( m_TypeValue > 0.5 ) m_Value = m_Value * h_entries;
			
			//Check all bins
			for (int binX = 0; binX < m_NbinsX; binX++) {
				for (int binY = 0; binY < m_NbinsY; binY++) {
					double bincon = h->GetBinContent(binX + 1, binY + 1);
					double ebincon = h->GetBinError(binX + 1, binY + 1);
					if( !Mask[binX][binY] ) {
						bool do_printbin = false;
						if(m_UseValue == 0) do_printbin = true;
						if( (m_UseValue == 1 && bincon >= m_Value) ||
						   (m_UseValue > 1 && bincon > m_Value) ||
						   (m_UseValue == -1 && bincon <= m_Value) ||
						   (m_UseValue < -1 && bincon < m_Value)
						   ) do_printbin = true;
						if( (m_UseMaskValue == 1 && bincon >= m_MaskValue) ||
						   (m_UseMaskValue > 1 && bincon > m_MaskValue) ||
						   (m_UseMaskValue == -1 && bincon <= m_MaskValue) ||
						   (m_UseMaskValue < -1 && bincon < m_MaskValue)
						   ) do_printbin = ! do_printbin;
						if(do_printbin) {
							Npublished++;
							if ( m_UnMask_All < 0 || 
								Npublished <= m_UnMask_All ) {
								double binvalX = h->GetXaxis()->GetBinCenter(binX + 1);
								double binvalY = h->GetYaxis()->GetBinCenter(binY + 1);
								std::string binname = Form("%s_Bin((%u,%u) | %.*e,%.*e)", name.c_str(), binX, binY, 2, binvalX, 2, binvalY);						
								std::string sigbinname = Form("%s_EBin((%u,%u) | %.*e,%.*e)", name.c_str(), binX, binY, 2, binvalX, 2, binvalY);						
								if ( m_TypeValue > 0.5 ) {
									result->tags_[binname.c_str()] = (bincon / h_entries);
									result->tags_[sigbinname.c_str()] = ebincon;
								} else {
								}
									result->tags_[binname.c_str()] = bincon;
									result->tags_[sigbinname.c_str()] = ebincon;
							}
						}
					}
				}
			}
		}
		
		//Append statement if all warnings or errors were not printed
		if ( m_UnMask_All >= 0 && 
			Npublished > m_UnMask_All ) {
			std::string pub_excess = Form("%s_PROBLEM_Publishable_UnPrinted",name.c_str());
			result->tags_[pub_excess.c_str()] = Npublished - m_UnMask_All;
		}
		
		return result;
	}
	
	
	// *********************************************************************
	// Protected Methods
	// *********************************************************************
	
	int BinPrint::UseValue_GetFromMap(const std::map<std::string, double> & params)
	{
		std::map<std::string, double>::const_iterator it = params.find("UseValue");
		if ( it != params.end() ){     
			return((int) it->second);
		} else {
			return(0);
		}
	}
	
	int BinPrint::TypeValue_GetFromMap(const std::map<std::string, double> & params)
	{
		std::map<std::string, double>::const_iterator it = params.find("TypeValue");
		if ( it != params.end() ){     
			return((int) it->second);
		} else {
			return(0);
		}
	}
	
	double BinPrint::Value_GetFromMap(const std::map<std::string, double> & params)
	{
		std::map<std::string, double>::const_iterator it = params.find("Value");
		if ( it != params.end() ){     
			return(it->second);
		} else {
			return(0.0);
		}
	}
	
	int BinPrint::UseMaskValue_GetFromMap(const std::map<std::string, double> & params)
	{
		std::map<std::string, double>::const_iterator it = params.find("UseMaskValue");
		if ( it != params.end() ){     
			return((int) it->second);
		} else {
			return(0);
		}
	}
	
	double BinPrint::MaskValue_GetFromMap(const std::map<std::string, double> & params)
	{
		std::map<std::string, double>::const_iterator it = params.find("MaskValue");
		if ( it != params.end() ){     
			return(it->second);
		} else {
			return(0.0);
		}
	}
	
	std::vector<bool> BinPrint::Mask1D_GetFromMap(const std::map<std::string, double> & params)
	{
                std::map<std::string, double>::const_iterator mask_map_bin;
		
		//Make default mask
		bool default_Mask = true;
		m_UnMask_All = -1;
		//Get m_UnMask_All configuration
		mask_map_bin = params.find("UnMask_All");
		if ( mask_map_bin != params.end() ) {
			if(mask_map_bin->second > 0.5) {
				m_UnMask_All = (int) mask_map_bin->second;
				default_Mask = false;
			}
		}
		std::vector<bool> Mask(m_NbinsX, default_Mask);
		
		//Get specific mask settings
		for ( int bin = 0; bin < m_NbinsX; bin++ ) {
			std::string mask_bin = Form("Mask_%d", bin + 1);
			mask_map_bin = params.find(mask_bin.c_str());
			if ( mask_map_bin != params.end()  ) {
				if(mask_map_bin->second > 0.5) Mask[bin] = true;
				else Mask[bin] = false;
			}
		}
		
		return(Mask);
	}
	
	std::vector< std::vector<bool> > BinPrint::Mask2D_GetFromMap(const std::map<std::string, double> & params)
	{
		std::map<std::string, double>::const_iterator mask_map_bin;
		
		//Make default mask
		bool default_Mask = true;
		m_UnMask_All = -1;
		//Get m_UnMask_All configuration
		mask_map_bin = params.find("UnMask_All");
		if ( mask_map_bin != params.end() ) {
			if(mask_map_bin->second > 0.5) {
				m_UnMask_All = (int) mask_map_bin->second;
				default_Mask = false;
			}
		}
		std::vector< std::vector<bool> > Mask(m_NbinsX, std::vector<bool>(m_NbinsY, default_Mask));
		
		//Get specific mask settings
		for ( int binX = 0; binX < m_NbinsX; binX++ ) {
			for ( int binY = 0; binY < m_NbinsY; binY++ ) {
				std::string mask_bin = Form("Mask_%d_%d", binX + 1, binY + 1);
				mask_map_bin = params.find(mask_bin.c_str());
				if ( mask_map_bin != params.end() ) {
					if(mask_map_bin->second > 0.5) Mask[binX][binY] = true;
					else Mask[binX][binY] = false;
				}
			}
		}
		
		return(Mask);
	}
	
} // namespace dqm_algorithms
