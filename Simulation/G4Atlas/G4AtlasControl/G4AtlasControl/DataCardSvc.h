/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DataCardSvc_H
#define DataCardSvc_H

#include <map>
#include <string>

class DataCardSvc {
friend class SimControl;
public:
	static DataCardSvc* GetDataCardSvc();

	int GetCardAsInt(const std::string name);
	double GetCardAsDouble(const std::string name);
	std::string GetCardAsString(const std::string name);
private:
	DataCardSvc();
	void DefineCard(const std::string name, const std::string type, int defValue=0);
	void DefineCard(const std::string name, const std::string type, double defValue=0.);
	void DefineCard(const std::string name, const std::string type, const std::string defValue="");
	
	void SetCard(const std::string name, int Value);
	void SetCard(const std::string name, double Value);
	void SetCard(const std::string name, const std::string Value);
	
	static DataCardSvc* thePointer;
	std::map< std::string, int, std::less<std::string> > theInts;
	std::map< std::string, double, std::less<std::string> > theDoubles;
	std::map< std::string, std::string, std::less<std::string> > theStrings;
	std::map< std::string, std::string, std::less<std::string> > XInfo;
};

#define INTCARD( NAME ) DataCardSvc::GetDataCardSvc()->GetCardAsInt( #NAME )
#define DOUBLECARD( NAME ) DataCardSvc::GetDataCardSvc()->GetCardAsDouble( #NAME )
#define STRINGCARD( NAME ) DataCardSvc::GetDataCardSvc()->GetCardAsString( #NAME )

#endif
