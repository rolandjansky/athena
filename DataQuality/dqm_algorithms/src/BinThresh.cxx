/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "dqm_algorithms/BinThresh.h"

#include <cmath>
#include <iostream>
#include <map>

#include <TH1.h>
#include <TProfile.h>
#include <TClass.h>

#include "dqm_core/exceptions.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_core/Result.h"

static dqm_algorithms::BinThresh staticInstance;

namespace dqm_algorithms {
	
	// *********************************************************************
	// Public Methods
	// *********************************************************************
	
	void
	BinThresh::
	printDescription(std::ostream& out)
	{
		std::string message;
		message += "\n";
		message += "Algorithm: \"" + m_name + "\"\n";
		message += "Description: Defines a warning and an error threshold for individual bins.\n";
		message += "             The status is the worst case summary of individual bin comparisons.\n";
		message += "             Some of the parameter names depend on whether the histogram is 1D or 2D.\n";
		message += "Mandatory Parameter: UseValue:\n";
		message += "             1: Flag bin contents >= Value\n";
		message += "            >1: Flag bin contents >  Value\n";
		message += "            -1: Flag bin contents <= Value\n";
		message += "           <-1: Flag bin contents <  Value\n";
		message += "Mandatory Parameter: TypeValue:\n";
		message += "             0: Value is compared to total entries in bin\n";
		message += "             1: Value is compared to fraction of entries in histogram appearing in bin\n";
		message += "             This also determines how the bin contents will be listed if published.\n";
		message += "Optional Parameter: Publish: Default = -1\n";
		message += "            -1: Print no values\n";
		message += "             0: Print the number of bins exceeding their threshold\n";
		message += "         # > 0: Print bins exceeding their thresholds individually\n";
		message += "                At most # bins are printed, and the excess is also listed\n";
		message += "Optional Parameter: TypePublish: Default = 0\n";
		message += "             0: Print only bins exceeding their error threshold Values\n";
		message += "             1: Print only bins exceeding their error or warning threshold Values\n";
		message += "             2: Print all unmasked bins\n";
		message += "Profile Configuration:\n";
		message += "Optional Parameter: BinMinEntries: Default = 0\n";
		message += "             Minimum number of entries in a TProfile (1D only) for a bin to be checked against thresolds\n";
		message += "             If a bin does not have a sufficient number of entries it also will not be printed\n";
		message += "1D Histogram Configuration:\n";
		message += "Limits: Value_#\n";
		message += "             Arguments are given for individual bins.\n";
		message += "             To make a single threshold check, set Warning:Value == Error:Value\n";
		message += "             Example of asserting the thresholds for a 3 bin histogram with the 2nd bin masked\n";
		message += "             ***In thresholds declaration:\n";
		message += "             limits Value_1 {\n";
		message += "               warning = 0.0\n";
		message += "               error = 0.5\n";
		message += "             }\n";
		message += "             limits Value_3 {\n";
		message += "               warning = 2.0\n";
		message += "               error = 2.5\n";
		message += "             }\n";
		message += "Limits: Value_All\n";
		message += "             Unmasks and sets thresholds uniformly for all bins.\n";
		message += "             Example of alternative declaration of the 3 bins histogram thresholds and masking above:\n";
		message += "             ***In algorithm declaration\n";
		message += "             Mask_2 = 1\n";
		message += "             ***In thresholds declaration:\n";
		message += "             limits Value_All {\n";
		message += "               warning = 0.0\n";
		message += "               error = 0.5\n";
		message += "             }\n";
		message += "             limits Value_3 {\n";
		message += "               warning = 2.0\n";
		message += "               error = 2.5\n";
		message += "             }\n";
		message += "Optional Parameters:  Mask_#         Default = See Note below.\n";
		message += "             1: Mask bin\n";
		message += "2D Histogram Configuration:\n";
		message += "Limits: Value_#_#\n";
		message += "             Arguments are given for individual bins.\n";
		message += "             Example of setting the X = 1, Y = 2 bin thresholds:\n";
		message += "             ***In thresholds declaration:\n";
		message += "             limits Value_1_2 {\n";
		message += "               warning = 2.0\n";
		message += "               error = 2.5\n";
		message += "             }\n";
		message += "Optional Parameters:  Mask_#_#       Default = See Note below.\n";
		message += "             1: Mask bin\n";
		message += "             Example of masking the X = 1, Y = 2 bin:\n";
		message += "             ***In algorithm declaration\n";
		message += "             Mask_1_2 = 1\n";
		message += "Note Mask_## = 1 has precedence over the unmasking from a Value_## declaration\n";
		message += "and that Value_## settings take precedence over the Value_All settings.\n";
		message += "The un-masking option Mask_## = 0 is ignored in order to avoid unmasking an unconfigured bin.";
		
		out << message << std::endl;
	}
	
	BinThresh::
	BinThresh()
	: m_name("BinThresh")
	, m_NbinsX(-1)
	, m_NbinsY(-1)
	{
		dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name, this );
		
		//NOTE: name will yield the entire directory structure
		//leading up to the algorithm instance.
		//This directory structure is used by other features such as result->tag.
	}
	
	
	BinThresh::
	~BinThresh()
	{
	}
	
	
	dqm_core::Algorithm*
	BinThresh::
	clone()
	{
		return new BinThresh(*this);
	}
	
	
	dqm_core::Result*
	BinThresh::
	execute( const std::string& name, const TObject& data, const dqm_core::AlgorithmConfig& config)
	{
		dqm_core::Result* result = new dqm_core::Result();
		
		//Worst Case Summary default = Good
		int NWarnings = 0;
		int NErrors = 0;
		int NGoodPrint = 0;
		//Histogram dimension default = no dimensions
		m_NbinsX = -1;
		m_NbinsY = -1;
		
		//Get data
		if (!data.IsA()->InheritsFrom("TH1")) {
			throw dqm_core::BadConfig(ERS_HERE, name, "does not inherit from TH1");
		}
		TH1* h = (TH1*)&data; // h cannot be null
		if (h->GetDimension() > 2) { 
			throw dqm_core::BadConfig(ERS_HERE, name, "dimension > 2 ");
		}
		
		//Get optional parameters
		int Publish = Publish_GetFromMap(config.getParameters());
		int TypePublish = TypePublish_GetFromMap(config.getParameters());
		int BinMinEntries = BinMinEntries_GetFromMap(config.getParameters());
		
		//Get mandatory parameters
		int UseValue;
		int TypeValue;
		try {
			UseValue = UseValue_GetFromMap(config.getParameters());
			TypeValue = TypeValue_GetFromMap(config.getParameters());
		}
		catch (dqm_core::Exception & ex) {
			throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
		}
		if ( UseValue == 0 ) ERS_INFO("UseValue == 0 is meaningless");
		
		//**********
		// Profile case
		//**********
		if ( data.IsA()->InheritsFrom("TProfile") ) {
			TProfile* hp = (TProfile*)&data;
			//ASSUME: dimension = 1
			m_NbinsX = hp->GetNbinsX();
			
			//Get threshold limits & masks
			std::vector<BinThresh::mask_limits> Limits;
			try {
				Limits = Limits1D_GetFromMap(config.getParameters(), config.getGreenThresholds(), config.getRedThresholds());
			}
			catch (dqm_core::Exception & ex) {
				throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
			}
			//CHECK
			//Verify threshold limit consistencies
			for(int bin = 0; bin < m_NbinsX; bin++) {
				if ((UseValue > 0 && Limits[bin].WarningValue > Limits[bin].ErrorValue) || 
					(UseValue < 0 && Limits[bin].WarningValue < Limits[bin].ErrorValue)) {
					//Masked thresholds might be used for a different algorithm configuration
					if ( !Limits[bin].Mask ) {
						std::string problem_message = Form("Incompatible Warning and Error Values in bin_%d : Warning = %e -> Error = %e", bin + 1, Limits[bin].WarningValue, Limits[bin].ErrorValue);
						ERS_INFO(problem_message.c_str());
					}
				}
			}
			
			//Rescale Thresholds to test fractional distribution
			double h_entries = hp->GetEntries();
			if ( TypeValue ) {
				for (int bin = 0; bin < m_NbinsX; bin++) {
					Limits[bin].WarningValue = Limits[bin].WarningValue * h_entries;
					Limits[bin].ErrorValue = Limits[bin].ErrorValue * h_entries;
				}
			}
			
			//Check all bins
			for (int bin = 0; bin < m_NbinsX; bin++) {
				double bincon = hp->GetBinContent(bin + 1);
				if ( !Limits[bin].Mask && hp->GetBinEntries(bin + 1) >= BinMinEntries ) {
					//Check for and Print errors
					if ((UseValue == 1 && bincon >= Limits[bin].ErrorValue) ||
						(UseValue > 1 && bincon > Limits[bin].ErrorValue) ||
						(UseValue == -1 && bincon <= Limits[bin].ErrorValue) ||
						(UseValue < -1 && bincon < Limits[bin].ErrorValue) ) {
						NErrors++;
						if ( NErrors + NWarnings + NGoodPrint <= Publish ) { //Publish Errors
							double binval = hp->GetXaxis()->GetBinCenter(bin + 1);
							//Give bin number as well as bin center
							//Use scientific notation (e) to be compatible with all axis scales
							//Include 2 digits after decimal (.*, with *=2) so that partitions up to 1000 are covered.
							//If using fractional thresholds, then print fractional values.
							std::string binname = Form("%s_ErrorBin(%u | %.*e)", name.c_str(), bin, 2, binval);
							//From() ~ printf() (s ~ string, u ~ unsigned int, .*e ~ scientific)
							if ( TypeValue > 0.5 ) {
								result->tags_[binname.c_str()] = (bincon / h_entries);
							} else {
								result->tags_[binname.c_str()] = bincon;
							}
						}
					}
					//Check for and Print warnings, if no Errors found
					else if ((UseValue == 1 && bincon >= Limits[bin].WarningValue) ||
							 (UseValue > 1 && bincon > Limits[bin].WarningValue) ||
							 (UseValue == -1 && bincon <= Limits[bin].WarningValue) ||
							 (UseValue < -1 && bincon < Limits[bin].WarningValue) ) {
						NWarnings++;
						if ( TypePublish >= 1 && NErrors + NWarnings + NGoodPrint <= Publish ) { //Publish Warnings
							float binval = hp->GetXaxis()->GetBinCenter(bin + 1);
							//Use scientific notation (e) to be compatible with all axis scales
							//Include 2 digits after decimal (.*, with *=2) so that partitions up to 1000 are covered.
							//If using fractional thresholds, then print fractional values.
							std::string binname  = Form("%s_WarningBin(%u | %.*e)", name.c_str(), bin, 2, binval);
							if ( TypeValue > 0.5 ) {
								result->tags_[binname.c_str()] = (bincon / h_entries);
							} else {
								result->tags_[binname.c_str()] = bincon;
							}
						}
					}
					//Print remaining bins
					else {
						NGoodPrint++;
						if ( TypePublish >= 2 && NErrors + NWarnings + NGoodPrint <= Publish ) { //Publish Good Bins
							float binval = hp->GetXaxis()->GetBinCenter(bin + 1);
							//Use scientific notation (e) to be compatible with all axis scales
							//Include 2 digits after decimal (.*, with *=2) so that partitions up to 1000 are covered.
							//If using fractional thresholds, then print fractional values.
							std::string binname  = Form("%s_GoodBin(%u | %.*e)", name.c_str(), bin, 2, binval);
							if ( TypeValue > 0.5 ) {
								result->tags_[binname.c_str()] = (bincon / h_entries);
							} else {
								result->tags_[binname.c_str()] = bincon;
							}
						}
					}
				}
			}			
		}
		
		//**********
		// 1D Histogram case
		//**********
		if ( (! data.IsA()->InheritsFrom("TProfile")) && h->GetDimension() == 1 ) {
			m_NbinsX = h->GetNbinsX();
			
			//Get threshold limits & masks
			std::vector<BinThresh::mask_limits> Limits;
			try {
				Limits = Limits1D_GetFromMap(config.getParameters(), config.getGreenThresholds(), config.getRedThresholds());
			}
			catch (dqm_core::Exception & ex) {
				throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
			}
			//CHECK
			//Verify threshold limit consistencies
			for(int bin = 0; bin < m_NbinsX; bin++) {
				if ((UseValue > 0 && Limits[bin].WarningValue > Limits[bin].ErrorValue) || 
					(UseValue < 0 && Limits[bin].WarningValue < Limits[bin].ErrorValue)) {
					//Masked thresholds might be used for a different algorithm configuration
					if ( !Limits[bin].Mask ) {
						std::string problem_message = Form("Incompatible Warning and Error Values in bin_%d : Warning = %e -> Error = %e", bin + 1, Limits[bin].WarningValue, Limits[bin].ErrorValue);
						ERS_INFO(problem_message.c_str());
					}
				}
			}
			
			//Rescale Thresholds to test fractional distribution
			double h_entries = h->GetEntries();
			if ( TypeValue ) {
				for (int bin = 0; bin < m_NbinsX; bin++) {
					Limits[bin].WarningValue = Limits[bin].WarningValue * h_entries;
					Limits[bin].ErrorValue = Limits[bin].ErrorValue * h_entries;
				}
			}
			
			//Check all bins
			for (int bin = 0; bin < m_NbinsX; bin++) {
				double bincon = h->GetBinContent(bin + 1);
				if ( !Limits[bin].Mask ) {
					//Check for and Print errors
					if ((UseValue == 1 && bincon >= Limits[bin].ErrorValue) ||
						(UseValue > 1 && bincon > Limits[bin].ErrorValue) ||
						(UseValue == -1 && bincon <= Limits[bin].ErrorValue) ||
						(UseValue < -1 && bincon < Limits[bin].ErrorValue) ) {
						NErrors++;
						if ( NErrors + NWarnings + NGoodPrint <= Publish ) { //Publish Errors
							double binval = h->GetXaxis()->GetBinCenter(bin + 1);
							//Give bin number as well as bin center
							//Use scientific notation (e) to be compatible with all axis scales
							//Include 2 digits after decimal (.*, with *=2) so that partitions up to 1000 are covered.
							//If using fractional thresholds, then print fractional values.
							std::string binname = Form("%s_ErrorBin(%u | %.*e)", name.c_str(), bin, 2, binval);
							//From() ~ printf() (s ~ string, u ~ unsigned int, .*e ~ scientific)
							if ( TypeValue > 0.5 ) {
								result->tags_[binname.c_str()] = (bincon / h_entries);
							} else {
								result->tags_[binname.c_str()] = bincon;
							}
						}
					}
					//Check for and Print warnings, if no Errors found
					else if ((UseValue == 1 && bincon >= Limits[bin].WarningValue) ||
							 (UseValue > 1 && bincon > Limits[bin].WarningValue) ||
							 (UseValue == -1 && bincon <= Limits[bin].WarningValue) ||
							 (UseValue < -1 && bincon < Limits[bin].WarningValue) ) {
						NWarnings++;
						if ( TypePublish >= 1 && NErrors + NWarnings + NGoodPrint <= Publish ) { //Publish Warnings
							float binval = h->GetXaxis()->GetBinCenter(bin + 1);
							//Use scientific notation (e) to be compatible with all axis scales
							//Include 2 digits after decimal (.*, with *=2) so that partitions up to 1000 are covered.
							//If using fractional thresholds, then print fractional values.
							std::string binname  = Form("%s_WarningBin(%u | %.*e)", name.c_str(), bin, 2, binval);
							if ( TypeValue > 0.5 ) {
								result->tags_[binname.c_str()] = (bincon / h_entries);
							} else {
								result->tags_[binname.c_str()] = bincon;
							}
						}
					}
					//Print remaining bins
					else {
						NGoodPrint++;
						if ( TypePublish >= 2 && NErrors + NWarnings + NGoodPrint <= Publish ) { //Publish Good Bins
							float binval = h->GetXaxis()->GetBinCenter(bin + 1);
							//Use scientific notation (e) to be compatible with all axis scales
							//Include 2 digits after decimal (.*, with *=2) so that partitions up to 1000 are covered.
							//If using fractional thresholds, then print fractional values.
							std::string binname  = Form("%s_GoodBin(%u | %.*e)", name.c_str(), bin, 2, binval);
							if ( TypeValue > 0.5 ) {
								result->tags_[binname.c_str()] = (bincon / h_entries);
							} else {
								result->tags_[binname.c_str()] = bincon;
							}
						}
					}
				}
			}
		}
		
		//**********
		// 2D Histogram case
		//**********
		if ( (! data.IsA()->InheritsFrom("TProfile")) && h->GetDimension() == 2 ) {
			m_NbinsX = h->GetNbinsX();
			m_NbinsY = h->GetNbinsY();
			
			//Get threshold limits & masks
			std::vector< std::vector<BinThresh::mask_limits> > Limits;
			try {
				Limits = Limits2D_GetFromMap(config.getParameters(), config.getGreenThresholds(), config.getRedThresholds());
			}
			catch (dqm_core::Exception & ex) {
				throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
			}
			//CHECK
			//Verify threshold limit consistencies
			for(int binX = 0; binX < m_NbinsX; binX++) {
				for(int binY = 0; binY < m_NbinsY; binY++) {
					if ( (UseValue > 0 && Limits[binX][binY].WarningValue > Limits[binX][binY].ErrorValue) || 
						(UseValue < 0 && Limits[binX][binY].WarningValue < Limits[binX][binY].ErrorValue)) {
						//Masked thresholds might be used for a different algorithm configuration
						if ( !Limits[binX][binY].Mask ) {
							std::string problem_message = Form("Incompatible Warning and Error Values in bin_%d_%d : Warning = %e -> Error = %e", binX + 1, binY + 1, Limits[binX][binY].WarningValue, Limits[binX][binY].ErrorValue);
							ERS_INFO(problem_message.c_str());
						}
					}
				}}
			
			//Rescale Thresholds to test fractional distribution
			double h_entries = h->GetEntries();
			if ( TypeValue ) {
				for (int binX = 0; binX < m_NbinsX; binX++) {
					for (int binY = 0; binY < m_NbinsY; binY++) {
						Limits[binX][binY].WarningValue = Limits[binX][binY].WarningValue * h_entries;
						Limits[binX][binY].ErrorValue = Limits[binX][binY].ErrorValue * h_entries;
					}}
			}
			
			//Check all bins
			for (int binX = 0; binX < m_NbinsX; binX++) {
				for (int binY = 0; binY < m_NbinsY; binY++) {
					double bincon = h->GetBinContent(binX + 1, binY + 1);
					if ( !Limits[binX][binY].Mask ) {
						//Check for and Print errors
						if ( (UseValue == 1 && bincon >= Limits[binX][binY].ErrorValue) ||
							(UseValue > 1 && bincon > Limits[binX][binY].ErrorValue) ||
							(UseValue == -1 && bincon <= Limits[binX][binY].ErrorValue) ||
							(UseValue < -1 && bincon < Limits[binX][binY].ErrorValue) ) {
							NErrors++;
							if ( NErrors + NWarnings + NGoodPrint <= Publish ) { //Publish Errors
								float binvalX = h->GetXaxis()->GetBinCenter(binX + 1);
								float binvalY = h->GetYaxis()->GetBinCenter(binY + 1);
								std::string binname = Form("%s_ErrorBin((%u,%u) | %.*e,%.*e)", name.c_str(), binX, binY, 2, binvalX, 2, binvalY);
								//From() ~ printf() (s ~ string, u ~ unsigned int, .*e ~ scientific)
								if ( TypeValue > 0.5 ) {
									result->tags_[binname.c_str()] = (bincon / h_entries);
								} else {
									result->tags_[binname.c_str()] = bincon;
								}
							}
						}
						//Check for and Print warnings
						else if ( (UseValue == 1 && bincon >= Limits[binX][binY].WarningValue) ||
								 (UseValue > 1 && bincon > Limits[binX][binY].WarningValue) ||
								 (UseValue == -1 && bincon <= Limits[binX][binY].WarningValue) ||
								 (UseValue < -1 && bincon < Limits[binX][binY].WarningValue) ) {
							NWarnings++;
							if ( TypePublish >= 1 && NErrors + NWarnings + NGoodPrint <= Publish ) { //Publish Warnings
								float binvalX = h->GetXaxis()->GetBinCenter(binX + 1);
								float binvalY = h->GetYaxis()->GetBinCenter(binY + 1);
								std::string binname = Form("%s_WarningBin((%u,%u) | %.*e,%.*e)", name.c_str(), binX, binY, 2, binvalX, 2, binvalY);
								if ( TypeValue > 0.5 ) {
									result->tags_[binname.c_str()] = (bincon / h_entries);
								} else {
									result->tags_[binname.c_str()] = bincon;
								}
							}
						}
						//Print remaining bins
						else {
							NGoodPrint++;
							if ( TypePublish >= 2 && NErrors + NWarnings + NGoodPrint <= Publish ) { //Publish Good Bins
								float binvalX = h->GetXaxis()->GetBinCenter(binX + 1);
								float binvalY = h->GetYaxis()->GetBinCenter(binY + 1);
								std::string binname = Form("%s_GoodBin((%u,%u) | %.*e,%.*e)", name.c_str(), binX, binY, 2, binvalX, 2, binvalY);
								if ( TypeValue > 0.5 ) {
									result->tags_[binname.c_str()] = (bincon / h_entries);
								} else {
									result->tags_[binname.c_str()] = bincon;
								}
							}
						}
					}
				}}
		}
		
		//Print Number of Errors & Warnings
		if ( Publish >= 0 ) {
			std::string pub_error = Form("%s_Bin_Errors", name.c_str());
			result->tags_[pub_error.c_str()] = NErrors;
			if ( TypePublish >= 1 ) {
				std::string pub_warning = Form("%s_Bin_Warnings", name.c_str());
				result->tags_[pub_warning.c_str()] = NWarnings;
			}
		}
		
		//Set Flag as worst case summary of all bins
		if ( (NErrors == 0) && (NWarnings == 0) ) result->status_ = dqm_core::Result::Green;
		if ( (NErrors == 0) && (NWarnings > 0) ) result->status_ = dqm_core::Result::Yellow;
		if ( (NErrors > 0) ) result->status_ = dqm_core::Result::Red;
		
		return(result);
	}
	
	// *********************************************************************
	// Protected Methods
	// *********************************************************************
	
	int BinThresh::Publish_GetFromMap(const std::map<std::string, double> & params)
	{
		std::map<std::string, double>::const_iterator it = params.find("Publish");
		if ( it != params.end() ) {
		        return((int) it->second);
		}else {
			return(0);
		}
	}
	
	int BinThresh::TypePublish_GetFromMap(const std::map<std::string, double> & params)
	{
		std::map<std::string, double>::const_iterator it = params.find("TypePublish");
		if ( it != params.end() ) {
			if ( it->second > 1.5 ) return(2);
			else if ( it->second > 0.5 ) return(1);
			else return(0);
		}else {
			return(0);
		}
	}
	
	int BinThresh::UseValue_GetFromMap(const std::map<std::string, double> & params)
	{
		std::map<std::string, double>::const_iterator it = params.find("UseValue");
		if ( it != params.end() ) {
			return((int) it->second);
		}else {
			return(0);
		}
	}
	
	int BinThresh::TypeValue_GetFromMap(const std::map<std::string, double> & params)
	{
		std::map<std::string, double>::const_iterator it = params.find("TypeValue");
		if ( it != params.end() ) {
			if ( it->second > 0.5 ) return(1);
			else return(0);
		}else {
			return(0);
		}
	}
	
	int BinThresh::BinMinEntries_GetFromMap(const std::map<std::string, double> & params)
	{
		std::map<std::string, double>::const_iterator it = params.find("BinMinEntries");
		if ( it != params.end() ) {
			return((int) it->second);
		}else {
			return(0);
		}
	}
	
	std::vector<BinThresh::mask_limits> BinThresh::Limits1D_GetFromMap( const std::map<std::string, double> & params, 
																		  const std::map<std::string, double> & warning_params, 
																		  const std::map<std::string, double> & error_params )
	{
		std::map<std::string, double>::const_iterator warning_map_bin;
		std::map<std::string, double>::const_iterator error_map_bin;
		std::map<std::string, double>::const_iterator mask_map_bin;
		
		//Make Default Limit Values & Masking
		BinThresh::mask_limits default_Limits;
		default_Limits.WarningValue = 0.0;
		default_Limits.ErrorValue = 0.0;
		default_Limits.Mask = true;
		
		//Check for Value_All thresholds to use for all bins
		warning_map_bin = warning_params.find("Value_All");
		error_map_bin = error_params.find("Value_All");
		if ( warning_map_bin != warning_params.end() && 
			error_map_bin != error_params.end() ) {
			default_Limits.WarningValue = warning_map_bin->second;
			default_Limits.ErrorValue = error_map_bin->second;
			default_Limits.Mask = false;
		}
		
		//Make default threshold & mask
		std::vector<BinThresh::mask_limits> Limits(m_NbinsX, default_Limits);
		
		//Get specific bin limits and unmasked bins
		for ( int bin = 0; bin < m_NbinsX; bin++ ) {
			std::string value_bin = Form("Value_%d", bin + 1);
			//Get thresholds for bin
			warning_map_bin = warning_params.find(value_bin.c_str());
			error_map_bin = error_params.find(value_bin.c_str());
			if ( warning_map_bin != warning_params.end() && 
				error_map_bin != error_params.end() ) {
				Limits[bin].WarningValue = warning_map_bin->second;
				Limits[bin].ErrorValue = error_map_bin->second;
				Limits[bin].Mask = false;
			}
			
			//Mask out specified bins
			//The masking that is explicitly defined takes precedence over the implied masking from the thresholds
			std::string mask_bin = Form("Mask_%d", bin + 1);
			mask_map_bin = params.find(mask_bin.c_str());
			if ( mask_map_bin != params.end() ) {
				if(mask_map_bin->second > 0.5) Limits[bin].Mask = true;
				//UnMasking should never be necessary
			}
		}
		
		return(Limits);
	}
	
	std::vector< std::vector<BinThresh::mask_limits> > BinThresh::Limits2D_GetFromMap( const std::map<std::string, double> & params, 
																						 const std::map<std::string, double> & warning_params, 
																						 const std::map<std::string, double> & error_params )
	{
		std::map<std::string, double>::const_iterator warning_map_bin;
		std::map<std::string, double>::const_iterator error_map_bin;
		std::map<std::string, double>::const_iterator mask_map_bin;
		
		//Make Default Limit Values & Masking
		BinThresh::mask_limits default_Limits;
		default_Limits.WarningValue = 0.0;
		default_Limits.ErrorValue = 0.0;
		default_Limits.Mask = true;
		
		//Check for Value_All thresholds to use for all bins
		warning_map_bin = warning_params.find("Value_All");
		error_map_bin = error_params.find("Value_All");
		if ( warning_map_bin != warning_params.end() && 
			error_map_bin != error_params.end() ) {
			default_Limits.WarningValue = warning_map_bin->second;
			default_Limits.ErrorValue = error_map_bin->second;
			default_Limits.Mask = false;
		}
		
		//Make default threshold & mask
		std::vector< std::vector<BinThresh::mask_limits> > Limits(m_NbinsX, std::vector<BinThresh::mask_limits>(m_NbinsY, default_Limits));
		
		//Get specific bin limits and unmasked bins
		for ( int binX = 0; binX < m_NbinsX; binX++ ) {
			for ( int binY = 0; binY < m_NbinsY; binY++ ) {
				std::string value_bin = Form("Value_%d_%d", binX + 1, binY + 1);
				//Get thresholds for bin
				warning_map_bin = warning_params.find(value_bin.c_str());
				error_map_bin = error_params.find(value_bin.c_str());
				if ( warning_map_bin != warning_params.end() && 
					error_map_bin != error_params.end() ) {
					Limits[binX][binY].WarningValue = warning_map_bin->second;
					Limits[binX][binY].ErrorValue = error_map_bin->second;
					Limits[binX][binY].Mask = false;
				}
				
				//Mask out specified bins
				//The masking that is explicitly defined takes precedence over the implied masking from the thresholds
				std::string mask_bin = Form("Mask_%d_%d", binX + 1, binY + 1);
				mask_map_bin = params.find(mask_bin.c_str());
				if ( mask_map_bin != params.end() ) {
					if(mask_map_bin->second > 0.5) Limits[binX][binY].Mask = true;
					//UnMasking should never be necessary
				}
			}}
		
		return(Limits);
	}
	
} // namespace dqm_algorithms
