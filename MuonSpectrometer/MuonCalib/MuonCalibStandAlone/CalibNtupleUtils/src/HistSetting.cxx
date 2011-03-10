/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CalibNtupleUtils/HistSetting.h"


#include "TH1.h"
#include "TAxis.h"

AxisSetting::AxisSetting( std::string title ) : 
  m_title(title), m_labelOffset(-1.),m_labelSize(-1.),m_titleOffset(-1.),m_titleSize(-1.) 
{

}

void AxisSetting::setLabel( double labelOffset, double labelSize ) 
{ 
  m_labelOffset = labelOffset; 
  m_labelSize = labelSize; 
}

void AxisSetting::setTitle( double titleOffset, double titleSize ) 
{ 
  m_titleOffset = titleOffset;
  m_titleSize = titleSize; 
}


void AxisSetting::applySetting( TAxis* axis ) const
{
  axis->SetTitle(m_title.c_str());
  if( m_labelSize > 0. )   axis->SetLabelSize( m_labelSize );
  if( m_labelOffset > 0. ) axis->SetLabelOffset( m_labelOffset );
  if( m_titleSize > 0. )   axis->SetTitleSize( m_titleSize );
  if( m_titleOffset > 0. ) axis->SetTitleOffset( m_titleOffset );
} 

HistSetting::HistSetting(std::string title, bool doStat, const AxisSetting* xsettings, const AxisSetting* ysetting  )
  : m_title(title), m_doStat(doStat), m_xsetting(xsettings), m_ysetting(ysetting)
{
  
}
 
void HistSetting::applySetting( TH1* h ) const
{
  h->SetTitle(m_title.c_str());
  h->SetStats(m_doStat);
  if( m_xsetting ) m_xsetting->applySetting( h->GetXaxis() );
  if( m_ysetting ) m_ysetting->applySetting( h->GetYaxis() );
}
