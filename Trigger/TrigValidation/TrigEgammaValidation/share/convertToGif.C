#include <iostream>
#include <string>

#include "TH1F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TKey.h"

using namespace std;

TCanvas c1;

string searchForFile()
{
	void *dirp = gSystem->OpenDirectory(".");
	if (dirp == NULL)
	{
		cout << "Erro opening directory" << endl;
		return "";
	}
	const char *entry = gSystem->GetDirEntry(dirp);
	while (entry)
	{
		TString tEntry = entry;
		if (tEntry.Contains("root"))
		{
			if ((tEntry.Contains("nav")) || (tEntry.Contains("Nav")))
				return (tEntry);
		}
		entry = gSystem->GetDirEntry(dirp);	
	}

	return "";
}

void searchDirectories(TDirectory *File)
{
	TIter next(File->GetListOfKeys());
	TKey *key;
	TDirectory *motherDir;
	TH1F * histo;

	while (key = (TKey *) next())
	{
		TString keyName = key->GetName();
		if (string (key->GetClassName()).find("TDirectory",0) != string::npos)
		{
			searchDirectories((TDirectory *) key->ReadObj());
			continue;
		}	
		else if (string (key->GetClassName()) != string("TH1F"))
			continue;

		histo = (TH1F *) key->ReadObj();
		if (histo->GetEntries() != 0)
		{
			motherDir = key->GetMotherDir();
			if (!motherDir)
			{
				cout << "An unexpected error ocurred. Cannot get histogram's mother directory" << endl;
				continue;
			}
			
			TString filePath = motherDir->GetPath();
			if (! filePath.Contains("Nav"))
				continue;

			if (filePath.Contains(":"))
			{
				Ssiz_t loc = filePath.Index(":",0);
				filePath.Remove(0, loc + 2);
			}
			Ssiz_t loc = filePath.Index('/', 0);
			filePath.Remove(0, loc + 1);

			string path(filePath );
			string name;
			size_t pos1, pos2;
			int cont = 0;
			for (pos1 = 0, pos2 = path.find('/', 0); pos2!= string::npos; )
			{
				name = name + path.substr(pos1, pos2) + '-';
				pos1 = pos2 +1;
				pos2 = path.find('/', pos1+1);
				cont++;
			}
			name = name + path.substr(pos1) + '-';
			
			string keyName_str (keyName);
			name = name + keyName_str + ".gif"; 
			histo->Draw();
			c1.SaveAs(name.c_str()); 
			for (int i =0; i < cont; i++)
				gDirectory->cd("../");
		}
	}
}

void convertToGif()
{
	string file_root;
	file_root = searchForFile();
	if (file_root != "")
	{
		TFile *file = new TFile(file_root.c_str());
		searchDirectories(file);
		if (file->IsOpen())
			file->Close();
	}
	else
		cout << "It was not possible to find a root file" << endl;
}
