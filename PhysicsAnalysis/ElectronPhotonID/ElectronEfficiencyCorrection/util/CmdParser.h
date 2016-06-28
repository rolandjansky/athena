/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CMDPARSER_H
#define CMDPARSER_H
#include "StringUtils.h"
#include "Messaging.h"

class CmdParser
{
  public:
    CmdParser()
      : m_isInitialized(false) {};
    ~CmdParser(){};

    Int_t countLeading(const TString& text, const TString& characters, Int_t nMax) {
      Int_t pos = 0;
      while (pos < text.Length() && characters.Index(text[pos]) != kNPOS && (nMax < 0 || pos < nMax)) {
        pos++;
      }

      return pos;
    }

    Int_t	removeLeading	(TString &text, TString characters, Int_t nMax = -1) {
      Int_t pos = countLeading(text, characters, nMax);
      text.Remove(0, pos);
      return pos;
    };



    void addOption(TString lopt, TString sopt, bool hasArg=true)
    {
      m_lopts.push_back(lopt);
      m_sopts.push_back(sopt);
      m_lspairs.push_back(std::make_pair(sopt,lopt));
      m_argpairs.push_back(std::make_pair(sopt,hasArg));
    };

    TString getlopt(TString sopt)
    {
      for (unsigned int i = 0; i<m_lspairs.size(); ++i)
      {
        if (m_lspairs.at(i).first == sopt)
          return m_lspairs.at(i).second;
      }
      return "";
    }

    TString getsopt(TString lopt)
    {
      for (unsigned int i = 0; i<m_lspairs.size(); ++i)
      {
        if (m_lspairs.at(i).second == lopt)
          return m_lspairs.at(i).first;
      }
      return "";
    }

    int Init(int argc, char** argv)
    {
      TString arg = "";
      m_isInitialized = false;
      TString sopt,lopt,t;
      bool hasArg;
      for(int i = 1; i<argc; i++)
      {
        arg = StringUtils::toTString(argv[i]);

        if ( arg.BeginsWith("--") || ((arg.BeginsWith("-") && arg.Remove(0,1).IsAlpha() )) )
        {
          removeLeading(arg,"-");
          if (arg.Length() == 1)
          {
            sopt = arg;
            lopt = this->getlopt(sopt);
          }
          else
          {
            lopt = arg;
            sopt = this->getsopt(lopt);
          }
          hasArg = true;
          for (unsigned int j=0; j<m_argpairs.size(); ++j)
          {
            if (m_argpairs.at(j).first == sopt)
              hasArg = m_argpairs.at(j).second;
          }
          if (!hasArg)
            t = "true";
          else
          {
            if (argc-1 == i)
            {
              MSG_ERROR("command line flag found without argument");
              return 0;     
            }
            else if (argc > i+1 && ( (StringUtils::toTString(argv[i+1]).BeginsWith("-") && (StringUtils::toTString(argv[i+1]).Remove(0,1)).IsAlpha() ) || StringUtils::toTString(argv[i+1]).BeginsWith("--")) )
            {
              MSG_ERROR("command line flag found without argument");
              return 0;     
            }
            t = StringUtils::toTString(argv[i+1]);
          }
          m_pairs.push_back(std::make_pair(StringUtils::toTString(argv[i]),t));
        }
      }
      m_isInitialized=kTRUE;
      return 1;
    };

    bool getBool(TString opt)
    {
      TString t = this->getArg(opt);
      if (t=="true")
        return true;
      else
        return false;
    }

    TString getArg(TString opt)
    {
      TString lopt, sopt;
      for (unsigned int i = 0; i<m_lspairs.size(); i++)
      {
        if (m_lspairs.at(i).first == opt)
        {
          lopt = m_lspairs.at(i).second;
          sopt = opt;
        }
        if (m_lspairs.at(i).second == opt)
        {
          sopt = m_lspairs.at(i).first;
          lopt = opt;
        }
      }

      TString t;
      if (!m_isInitialized)
        MSG_ABORT("CommandLine parser is not initialized!");
      for (unsigned int i = 0; i<m_pairs.size(); i++)
      {
        if (m_pairs.at(i).first == "-"+sopt || m_pairs.at(i).first == "--"+lopt)
        {
          t=m_pairs.at(i).second;
        }
      }
      return t;
        /*for (unsigned int i=0; i<m_vOpts.size(); i++)*/
        /*{*/
        /*while ((m_c = getopt_long (m_argc, m_argv, m_optString.Data(), &m_vOpts[0], &m_option_index) != -1))*/
        /*{*/
        /*switch(m_c)*/
        /*case 'f':*/
        /*MSG_ERROR("WTF???"<<m_c);*/
        /*MSG_INFO(opt<<" , "<<optarg<<" , "<<m_c );*/
        /*if (int(m_c) != int(opt))*/
        /*{*/
        /*t = StringUtils::toTString(optarg);*/
        /*break;*/
        /*}*/
        /*}*/
        /*}*/
        /*return t;*/
    }

  private:
    /*std::vector<option> m_vOpts;*/
    std::vector<TString> m_lopts;
    std::vector<TString> m_sopts;
    std::vector< std::pair<TString,TString> >m_pairs;
    std::vector< std::pair<TString,TString> >m_lspairs;
    std::vector< std::pair<TString,   bool> >m_argpairs;
    bool m_isInitialized;
    /*int m_option_index;*/
    /*int m_c;*/
    /*int m_argc;*/
    /*char **m_argv;*/
    /*TString m_optString;*/
};
#endif 
    
