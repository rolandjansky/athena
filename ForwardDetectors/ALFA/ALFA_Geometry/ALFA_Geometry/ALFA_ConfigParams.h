/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AlfaConfigParams_h
#define AlfaConfigParams_h 1

#include <iostream>
#include <string>
#include <map>

//using namespace std;

//typedef basic_string<char> tstring;

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::map<std::string, std::string, std::less<std::string> > MAPSTR2STR;

class ALFA_ConfigParams
{
	public:
		ALFA_ConfigParams();
		virtual ~ALFA_ConfigParams();

	private:
		bool m_bIsValid;
		std::string m_strSection;
		MAPSTR2STR m_mapParams;

	public:
		bool IsKey(const char* szKey) const;
		const char* GetParameter(const char* szKey) const;
		const char* GetSectionName() const { return m_strSection.c_str(); }
		int GetParamsCnt() const { return m_mapParams.size(); }
		int Init(const char* szFile, const char* szSection);
		void UnInitialize();
		void GetMap(MAPSTR2STR& mapParams) const { mapParams=m_mapParams; }
};

#endif // AlfaConfigParams_h
