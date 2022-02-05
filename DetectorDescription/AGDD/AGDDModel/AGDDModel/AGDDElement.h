/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDElement_H
#define AGDDElement_H

#include <string>
class AGDDMaterialStore;

class AGDDElement {
	friend std::ostream& operator <<(std::ostream&, const AGDDElement &);
public:
	AGDDElement(AGDDMaterialStore& ms,
                    const std::string& n,
                    const std::string& s,
                    int z, double a):
		m_name(n),m_symbol(s),m_z(z),m_a(a),m_created(false),m_theElement(0)
	{
		RegisterToStore(ms, this);
	}
        void RegisterToStore(AGDDMaterialStore& ms, AGDDElement*);
	std::string GetName() const ;
	std::string GetSymbol() const ;
	int GetZ() const {return m_z;}
	double GetA() const {return m_a;}
	void Created(bool b) {m_created=b;}
	bool Extant() {return m_created;}
	void* GetElement() {return m_theElement;}
	void SetElement(void* el) {m_theElement=el;}
private:
	std::string m_name;
	std::string m_symbol;
	int m_z;
	double m_a;
	bool m_created;
	
	void* m_theElement;
};

#endif
