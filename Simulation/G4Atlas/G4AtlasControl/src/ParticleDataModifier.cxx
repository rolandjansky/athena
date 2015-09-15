/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasControl/ParticleDataModifier.h"

#include "G4ParticlePropertyTable.hh"
#include "G4ParticleTable.hh"
#include "G4ParticlePropertyData.hh"

#include "G4DecayTable.hh"
#include "G4VDecayChannel.hh"
#include "G4PhaseSpaceDecayChannel.hh"

#include "FadsUtilities/Tokenizer.h"

#include <iostream>

ParticleDataModifier::ParticleDataModifier()
{
	partName="";
	partProperties=G4ParticlePropertyTable::GetParticlePropertyTable();
	partTable=G4ParticleTable::GetParticleTable();
}

ParticleDataModifier::ParticleDataModifier(const std::string pName)
{
	partName=pName;
	partProperties=G4ParticlePropertyTable::GetParticlePropertyTable();
	partTable=G4ParticleTable::GetParticleTable();
}

void ParticleDataModifier::SetParticleMass(const std::string pName, double value)
{
	partName=pName;
	G4ParticleTable::G4PTblDicIterator* theParticleIterator;
    theParticleIterator = partTable->GetIterator();

    // loop over all particles in G4ParticleTable
    theParticleIterator->reset();
    while( (*theParticleIterator)() )
	{
    	G4ParticleDefinition* particle = theParticleIterator->value();
		if (partName==particle->GetParticleName())
		{
			G4ParticlePropertyData* pData=partProperties->GetParticleProperty(partName);
			if (value  != pData->GetPDGMass()) pData->SetPDGMass(value);
			partProperties->SetParticleProperty(*pData);
		}
		else
		{
			std::cout<<"Particle "<<pName<<" not found in the ParticleTable!!!"<<std::endl;
		}
	}
}
void ParticleDataModifier::SetParticleWidth(const std::string pName, double value)
{
	partName=pName;
	G4ParticleTable::G4PTblDicIterator* theParticleIterator;
    theParticleIterator = partTable->GetIterator();

    // loop over all particles in G4ParticleTable
    theParticleIterator->reset();
    while( (*theParticleIterator)() )
	{
    	G4ParticleDefinition* particle = theParticleIterator->value();
		if (partName==particle->GetParticleName())
		{
			G4ParticlePropertyData* pData=partProperties->GetParticleProperty(partName);
			if (value  != pData->GetPDGWidth()) pData->SetPDGWidth(value);
			partProperties->SetParticleProperty(*pData);
		}
		else
		{
			std::cout<<"Particle "<<pName<<" not found in the ParticleTable!!!"<<std::endl;
		}
	}
}
void ParticleDataModifier::SetParticleMass(double value) const
{
	if (partName.empty())
	{
		std::cout<<" Particle name not set!! returning"<<std::endl;
		return;
	}
	G4ParticleTable::G4PTblDicIterator* theParticleIterator;
    theParticleIterator = partTable->GetIterator();

    // loop over all particles in G4ParticleTable
    theParticleIterator->reset();
    while( (*theParticleIterator)() )
	{
    	G4ParticleDefinition* particle = theParticleIterator->value();
		if (partName==particle->GetParticleName())
		{
			G4ParticlePropertyData* pData=partProperties->GetParticleProperty(partName);
			if (value  != pData->GetPDGMass()) pData->SetPDGMass(value);
			partProperties->SetParticleProperty(*pData);
		}
	}
}

void ParticleDataModifier::SetParticleWidth(double value) const
{
	if (partName.empty())
	{
		std::cout<<" Particle name not set!! returning"<<std::endl;
		return;
	}
	G4ParticleTable::G4PTblDicIterator* theParticleIterator;
    theParticleIterator = partTable->GetIterator();

    // loop over all particles in G4ParticleTable
    theParticleIterator->reset();
    while( (*theParticleIterator)() )
	{
    	G4ParticleDefinition* particle = theParticleIterator->value();
		if (partName==particle->GetParticleName())
		{
			G4ParticlePropertyData* pData=partProperties->GetParticleProperty(partName);
			if (value  != pData->GetPDGWidth()) pData->SetPDGWidth(value);
			partProperties->SetParticleProperty(*pData);
		}
	}
}

void ParticleDataModifier::Stable(const std::string pName, bool value)
{
	partName=pName;
	G4ParticleTable::G4PTblDicIterator* theParticleIterator;
    theParticleIterator = partTable->GetIterator();

    // loop over all particles in G4ParticleTable
    theParticleIterator->reset();
    while( (*theParticleIterator)() )
	{
    	G4ParticleDefinition* particle = theParticleIterator->value();
		if (partName==particle->GetParticleName())
		{
			if (value)
				std::cout<<" Setting "<<partName<<" to be stable "<<std::endl;
			else
				std::cout<<" Setting "<<partName<<" to be unstable "<<std::endl;
			particle->SetPDGStable(value);
		}
	}
}

void ParticleDataModifier::Stable(bool value)
{
	if (partName.empty())
	{
		std::cout<<" Particle name not set!! returning"<<std::endl;
		return;
	}
	G4ParticleTable::G4PTblDicIterator* theParticleIterator;
    theParticleIterator = partTable->GetIterator();

    // loop over all particles in G4ParticleTable
    theParticleIterator->reset();
    while( (*theParticleIterator)() )
	{
    	G4ParticleDefinition* particle = theParticleIterator->value();
		if (partName==particle->GetParticleName())
		{
			if (value)
				std::cout<<" Setting "<<partName<<" to be stable "<<std::endl;
			else
				std::cout<<" Setting "<<partName<<" to be unstable "<<std::endl;
			particle->SetPDGStable(value);
		}
	}
}

void ParticleDataModifier::SetParticleLifeTime(double value) const
{
	if (partName.empty())
	{
		std::cout<<" Particle name not set!! returning"<<std::endl;
		return;
	}
	G4ParticleTable::G4PTblDicIterator* theParticleIterator;
    theParticleIterator = partTable->GetIterator();

    // loop over all particles in G4ParticleTable
    theParticleIterator->reset();
    while( (*theParticleIterator)() )
	{
    	G4ParticleDefinition* particle = theParticleIterator->value();
		if (partName==particle->GetParticleName())
		{
			G4ParticlePropertyData* pData=partProperties->GetParticleProperty(partName);
			if (value  != pData->GetPDGLifeTime()) pData->SetPDGLifeTime(value);
			partProperties->SetParticleProperty(*pData);
		}
	}
}

void ParticleDataModifier::SetParticleLifeTime(const std::string pName, double value)
{
	partName=pName;
	G4ParticleTable::G4PTblDicIterator* theParticleIterator;
    theParticleIterator = partTable->GetIterator();

    // loop over all particles in G4ParticleTable
    theParticleIterator->reset();
    while( (*theParticleIterator)() )
	{
    	G4ParticleDefinition* particle = theParticleIterator->value();
		if (partName==particle->GetParticleName())
		{
			G4ParticlePropertyData* pData=partProperties->GetParticleProperty(partName);
			if (value  != pData->GetPDGLifeTime()) pData->SetPDGLifeTime(value);
			partProperties->SetParticleProperty(*pData);
		}
		else
		{
			std::cout<<"Particle "<<pName<<" not found in the ParticleTable!!!"<<std::endl;
		}
	}
}

void ParticleDataModifier::AddDecayChannel(const std::string pName, double bRatio, const std::string decayProd)
{
	FADS::Tokenizer tok("=",decayProd);
	int nProducts=tok.size();

	partName=pName;
	G4ParticleTable::G4PTblDicIterator* theParticleIterator;
    theParticleIterator = partTable->GetIterator();

	// loop over all particles in G4ParticleTable
    theParticleIterator->reset();
    while( (*theParticleIterator)() )
	{
    	G4ParticleDefinition* particle = theParticleIterator->value();
		if (partName==particle->GetParticleName())
		{
			G4DecayTable*   table = particle->GetDecayTable();
			if (table==NULL)
				table = new G4DecayTable();
			G4VDecayChannel** mode = new G4VDecayChannel*[1];
			// really love this one!
			if (nProducts==2)
				mode[0] = new G4PhaseSpaceDecayChannel(partName,bRatio,2,tok[0],tok[1]);
			else if (nProducts==3)
				mode[0] = new G4PhaseSpaceDecayChannel(partName,bRatio,3,tok[0],tok[1],tok[2]);
			else if (nProducts==4)
				mode[0] = new G4PhaseSpaceDecayChannel(partName,bRatio,4,tok[0],tok[1],tok[2],tok[3]);
			else
			{
				std::cout<<"no decay in more than 4 particles allowed!!"<<std::endl;
				delete [] mode;
				return;
			}
			table->Insert(mode[0]);
			delete [] mode;
			particle->SetDecayTable(table);
		}
	}
}

void ParticleDataModifier::AddDecayChannel(double bRatio, const std::string decayProd) const
{
	FADS::Tokenizer tok("=",decayProd);
	int nProducts=tok.size();

	if (partName.empty())
	{
		std::cout<<" Particle name not set!! returning"<<std::endl;
		return;
	}
	G4ParticleTable::G4PTblDicIterator* theParticleIterator;
    theParticleIterator = partTable->GetIterator();

	// loop over all particles in G4ParticleTable
    theParticleIterator->reset();
    while( (*theParticleIterator)() )
	{
    	G4ParticleDefinition* particle = theParticleIterator->value();
		if (partName==particle->GetParticleName())
		{
			G4DecayTable*   table = particle->GetDecayTable();
			if (table==NULL)
				table = new G4DecayTable();
			G4VDecayChannel** mode = new G4VDecayChannel*[1];
			// really love this one!
			if (nProducts==2)
				mode[0] = new G4PhaseSpaceDecayChannel(partName,bRatio,2,tok[0],tok[1]);
			else if (nProducts==3)
				mode[0] = new G4PhaseSpaceDecayChannel(partName,bRatio,3,tok[0],tok[1],tok[2]);
			else if (nProducts==4)
				mode[0] = new G4PhaseSpaceDecayChannel(partName,bRatio,4,tok[0],tok[1],tok[2],tok[3]);
			else
			{
				std::cout<<"no decay in more than 4 particles allowed!!"<<std::endl;
				delete [] mode;
				return;
			}
			table->Insert(mode[0]);
			delete [] mode;
			particle->SetDecayTable(table);
		}
	}
}

void ParticleDataModifier::SetPDGCode(int value) const
{
	if (partName.empty())
	{
		std::cout<<" Particle name not set!! returning"<<std::endl;
		return;
	}
	G4ParticleTable::G4PTblDicIterator* theParticleIterator;
    theParticleIterator = partTable->GetIterator();

    // loop over all particles in G4ParticleTable
    theParticleIterator->reset();
    while( (*theParticleIterator)() )
	{
    	G4ParticleDefinition* particle = theParticleIterator->value();
		if (partName==particle->GetParticleName())
		{
			G4ParticlePropertyData* pData=partProperties->GetParticleProperty(partName);
			if (value  != pData->GetPDGEncoding()) pData->SetPDGEncoding(value);
			partProperties->SetParticleProperty(*pData);
		}
	}
}

void ParticleDataModifier::SetPDGCode(const std::string pName, int value)
{
	partName=pName;
	G4ParticleTable::G4PTblDicIterator* theParticleIterator;
    theParticleIterator = partTable->GetIterator();

    // loop over all particles in G4ParticleTable
    theParticleIterator->reset();
    while( (*theParticleIterator)() )
	{
    	G4ParticleDefinition* particle = theParticleIterator->value();
		if (partName==particle->GetParticleName())
		{
			G4ParticlePropertyData* pData=partProperties->GetParticleProperty(partName);
			if (value  != pData->GetPDGEncoding()) pData->SetPDGEncoding(value);
			partProperties->SetParticleProperty(*pData);
		}
		else
		{
			std::cout<<"Particle "<<pName<<" not found in the ParticleTable!!!"<<std::endl;
		}
	}
}

void ParticleDataModifier::SetParticleCharge(double value) const
{
        if (partName.empty())
        {
                std::cout<<" Particle name not set!! returning"<<std::endl;
                return;
        }
        G4ParticleTable::G4PTblDicIterator* theParticleIterator;
    theParticleIterator = partTable->GetIterator();

    // loop over all particles in G4ParticleTable
    theParticleIterator->reset();
    while( (*theParticleIterator)() )
        {
        G4ParticleDefinition* particle = theParticleIterator->value();
                if (partName==particle->GetParticleName())
                {
                        G4ParticlePropertyData* pData=partProperties->GetParticleProperty(partName);
                        if (value  != pData->GetPDGCharge()) pData->SetPDGCharge(value);
                        partProperties->SetParticleProperty(*pData);
                }
        }
}

void ParticleDataModifier::SetParticleCharge(const std::string pName, double value)
{
        partName=pName;
        G4ParticleTable::G4PTblDicIterator* theParticleIterator;
    theParticleIterator = partTable->GetIterator();

    // loop over all particles in G4ParticleTable
    theParticleIterator->reset();
    while( (*theParticleIterator)() )
        {
        G4ParticleDefinition* particle = theParticleIterator->value();
                if (partName==particle->GetParticleName())
                {
                        G4ParticlePropertyData* pData=partProperties->GetParticleProperty(partName);
                        if (value  != pData->GetPDGCharge()) pData->SetPDGCharge(value);
                        partProperties->SetParticleProperty(*pData);
                }
                else
                {
                        std::cout<<"Particle "<<pName<<" not found in the ParticleTable!!!"<<std::endl;
                }
        }
}

