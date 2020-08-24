/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStandAloneBase/RegionSelectorBase.h"
#include "MuonCalibStandAloneBase/RegionLogicalOperation.h"
#include "MuonCalibStandAloneBase/RegionElement.h"
#include "AthenaKernel/getMessageSvc.h"

namespace MuonCalib {

RegionSelectorBase * RegionSelectorBase::GetRegion(const std::string & input)
	{
	unsigned int i(0);
	return process_region(input, i, false);
	}

RegionSelectorBase * RegionSelectorBase::process_region(const std::string & input, unsigned int & i, bool is_in_braces)
	{
	unsigned int start_sub(i);
//create master region as logical operation
	RegionLogicalOperation *new_region  = new RegionLogicalOperation();
	bool currect_inverse(false);
	MsgStream log(Athena::getMessageSvc(),"RegionSelectorBase");
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
					log<<MSG::WARNING<<"Missing ']' at end of input! Started here:"<<endmsg;
					print_position(input, start_element, &log);
					delete new_region;
					return nullptr;
					}
				RegionElement *reg_el=new RegionElement();
			//syntax error in region
				if(!reg_el->Initialize(element))
					{
					print_position(input, start_element, &log);
					delete reg_el;
					delete new_region;
					return nullptr;
					}
			//add region to operation
				if(!new_region->AddRegion(reg_el, currect_inverse))
					{
					log<<MSG::WARNING<<"Missing operator!"<<endmsg;
					print_position(input, start_element, &log);
					delete new_region;
					return nullptr;
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
					log<<MSG::WARNING<<"'(' at end of input!"<<endmsg;
					print_position(input, start_element, &log);
					delete new_region;
					return nullptr;
					}
				RegionSelectorBase *second_region(process_region(input, i, true));
				if(!second_region)
					{
					delete new_region;
					return nullptr;
					}
				if(!new_region->AddRegion(second_region, currect_inverse))
					{
					log<<MSG::WARNING<<"Missing operator!"<<endmsg;
					print_position(input, start_element, &log);
					delete new_region;
					return nullptr;
					}
				currect_inverse=false;	
				break;
				}
		//invert the following region
			case '!':
				{
				if(currect_inverse)
					{
					log<<MSG::WARNING<<"Surplus '!'"<<endmsg;
					print_position(input, start_element, &log);
					delete new_region;
					return nullptr;
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
					log<<MSG::WARNING<<"Unexpected operator!"<<endmsg;
					print_position(input, start_element, &log);
					delete new_region;
					return nullptr;
					}
				break;
				}
		//end of current subregion
			case ')':
				{
				if(new_region->SurplusOperator())
					{
					log<<MSG::WARNING<<"Surplus operator"<<endmsg;
					print_position(input, start_element, &log);
					delete new_region;
					return nullptr;
					}
				if(is_in_braces)
					{
					return new_region;
					}
				log<<MSG::WARNING<<"Unexpected ')'"<<endmsg;
				print_position(input, start_element, &log);
				delete new_region;
				return nullptr;
				break;
				}
		//ignore whitespaces
			case ' ':
			case '\t':
			case '\n':
				break;
		//any other characters are junk at this point
			default:
			    log<<MSG::WARNING<<"Syntax Error"<<endmsg;
				print_position(input, start_element, &log);
				delete new_region;
				return nullptr;
			}
		}
//if we ar ein a subregion and reached the end of input there is an error
	if(is_in_braces)
		{
		log<<MSG::WARNING<<"Missing ')'"<<endmsg;
		print_position(input, start_sub, &log);
		delete new_region;
		return nullptr;
		}
	if(new_region->SurplusOperator())
		{
		log<<MSG::WARNING<<"Surplus operator"<<endmsg;
		print_position(input, i, &log);
		delete new_region;
		return nullptr;
		}
	return new_region;
	
	}

void RegionSelectorBase::print_position(const std::string & input, const unsigned int &position, MsgStream *msgStr)
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
	*msgStr<<MSG::WARNING<<st1<<"*"<<st2<<endmsg;
	}

}//namespace MuonCalib
