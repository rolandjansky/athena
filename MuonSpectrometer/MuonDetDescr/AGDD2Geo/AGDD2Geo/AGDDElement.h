/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDElement_H
#define AGDDElement_H

#include <string>

class AGDDElement {
	friend std::ostream& operator <<(std::ostream&, const AGDDElement &);
public:
	AGDDElement(std::string n, std::string s, int z, double a):
		_name(n),_symbol(s),_z(z),_a(a),_created(false)
	{
		RegisterToStore(this);
	}
	void RegisterToStore(AGDDElement*);
	std::string GetName() const ;
	std::string GetSymbol() const ;
	int GetZ() const {return _z;}
	double GetA() const {return _a;}
	void Created(bool b) {_created=b;}
	bool Extant() {return _created;}
	void* GetElement() {return theElement;}
	void SetElement(void* el) {theElement=el;}
private:
	std::string _name;
	std::string _symbol;
	int _z;
	double _a;
	bool _created;
	
	void* theElement;
};

#endif
