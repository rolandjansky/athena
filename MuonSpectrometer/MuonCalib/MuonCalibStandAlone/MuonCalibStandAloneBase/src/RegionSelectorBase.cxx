/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStandAloneBase/RegionSelectorBase.h"
#include "MuonCalibStandAloneBase/RegionLogicalOperation.h"
#include "MuonCalibStandAloneBase/RegionElement.h"



namespace MuonCalib {

RegionSelectorBase * RegionSelectorBase::GetRegion(const std::string & input)
	{
	unsigned int i(0);
	std::cout<<input<<std::endl;
	return process_region(input, i, false);
	}

RegionSelectorBase * RegionSelectorBase::process_region(const std::string & input, unsigned int & i, bool is_in_braces)
	{
	unsigned int start_sub(i);
//create master region as logical operation
	RegionLogicalOperation *new_region  = new RegionLogicalOperation();
	bool currect_inverse(false);
//loop over characters
	for(; i<input.size(); i++)
		{
		int start_element(i);
		switch(input[i])
			{
			//read elementary region
			case '[':
				{
				std::string element;
				int start_element(i);
				for(; i<input.size(); i++)
					{
					element.push_back(input[i]);
					if(input[i]==']') break;
					}
				if(i==input.size())
					{
					std::cerr<<"Missing ']' at end of input!"<<std::endl;
					std::cerr<<"Started here:"<<std::endl;
					print_position(input, start_element);
					delete new_region;
					return NULL;
					}
				RegionElement *reg_el=new RegionElement();
			//syntax error in region
				if(!reg_el->Initialize(element))
					{
					print_position(input, start_element);
					delete reg_el;
					delete new_region;
					return NULL;
					}
			//add region to operation
				if(!new_region->AddRegion(reg_el, currect_inverse))
					{
					std::cerr<<"Missing operator!"<<std::endl;
					print_position(input, start_element);
					delete new_region;
					return NULL;
					}
				currect_inverse=false;
				break;
				}
		//new substring - start recursive processing
			case '(':
				{
				i++;
				if(i>=input.size())
					{
					std::cerr<<"'(' at end of input!"<<std::endl;
					print_position(input, start_element);
					delete new_region;
					return NULL;
					}
				RegionSelectorBase *second_region(process_region(input, i, true));
				if(second_region == NULL)
					{
					delete new_region;
					return NULL;
					}
				if(!new_region->AddRegion(second_region, currect_inverse))
					{
					std::cerr<<"Missing operator!"<<std::endl;
					print_position(input, start_element);
					delete new_region;
					return NULL;
					}
				currect_inverse=false;	
				break;
				}
		//invert the following region
			case '!':
				{
				if(currect_inverse)
					{
					std::cerr<<"Surplus '!'"<<std::endl;
					print_position(input, start_element);
					delete new_region;
					return NULL;
					}
				currect_inverse = true;
				break;
				}
		//operator
			case '&':
			case '|':
				{
				bool next_op(input[i]=='|');
				if(!new_region->AddOperator(next_op))
					{
					std::cerr<<"Unexpected operator!"<<std::endl;
					print_position(input, start_element);
					delete new_region;
					return NULL;
					}
				break;
				}
		//end of current subregion
			case ')':
				{
				if(new_region->SurplusOperator())
					{
					std::cerr<<"Surplus operator"<<std::endl;
					print_position(input, start_element);
					delete new_region;
					return NULL;
					}
				if(is_in_braces)
					{
					return new_region;
					}
				std::cerr<<"Unexpected ')'"<<std::endl;
				print_position(input, start_element);
				delete new_region;
				return NULL;
				break;
				}
		//ignore whitespaces
			case ' ':
			case '\t':
			case '\n':
				break;
		//any other characters are junk at this point
			default:
				std::cerr<<"Syntax Error"<<std::endl;
				print_position(input, start_element);
				delete new_region;
				return NULL;
			}
		}
//if we ar ein a subregion and reached the end of input there is an error
	if(is_in_braces)
		{
		std::cerr<<"Missing ')'"<<std::endl;
		print_position(input, start_sub);
		delete new_region;
		return NULL;
		}
	if(new_region->SurplusOperator())
		{
		std::cerr<<"Surplus operator"<<std::endl;
		print_position(input, i);
		delete new_region;
		return NULL;
		}
	return new_region;
	
	}

void  RegionSelectorBase:: print_position(const std::string & input, const unsigned int &position)
	{
	unsigned int i(0);
	std::string st1, st2;
	for(; i<input.size() && i<position; i++)
		{
		st1+=input[i];
		}
	for(; i<input.size(); i++)
		{
		st2+=input[i];
		}
	std::cerr<<st1<<"*"<<st2<<std::endl;
	}




}//namespace MuonCalib
